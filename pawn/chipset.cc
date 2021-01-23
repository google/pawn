// Copyright 2014-2021 Google LLC. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <vector>

#include "pawn/chipset.h"

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pawn/chipset_intel_6_series.h"
#include "pawn/chipset_intel_7_series.h"
#include "pawn/chipset_intel_8_series.h"
#include "pawn/chipset_intel_9_series.h"
#include "pawn/chipset_intel_ich10.h"
#include "pawn/chipset_intel_ich8.h"
#include "pawn/chipset_intel_ich9.h"
#include "pawn/mini_google.h"
#include "pawn/pci.h"
#include "pawn/physical_memory.h"

namespace security::pawn {

absl::StatusOr<std::unique_ptr<Chipset>> Chipset::Create(
    Pci& pci, Chipset::HardwareId& probed_id) {
  const Chipset::HardwareId hw_id = {pci.ReadConfigUint16(pci::kVidRegister),
                                     pci.ReadConfigUint16(pci::kDidRegister),
                                     pci.ReadConfigUint8(pci::kRidRegister)};
  probed_id = hw_id;
  if (hw_id.vendor != 0x8086 /* Intel */) {
    return absl::UnimplementedError(
        "Only Intel chipsets are currently supported");
  }

  if (IntelIch8Chipset::SupportsDevice(hw_id)) {
    return absl::make_unique<IntelIch8Chipset>(Tag{}, hw_id, pci);
  }
  if (IntelIch9Chipset::SupportsDevice(hw_id)) {
    return absl::make_unique<IntelIch9Chipset>(Tag{}, hw_id, pci);
  }
  if (IntelIch10Chipset::SupportsDevice(hw_id)) {
    return absl::make_unique<IntelIch10Chipset>(Tag{}, hw_id, pci);
  }
  if (Intel6SeriesChipset::SupportsDevice(hw_id)) {
    return absl::make_unique<Intel6SeriesChipset>(Tag{}, hw_id, pci);
  }
  if (Intel7SeriesChipset::SupportsDevice(hw_id)) {
    return absl::make_unique<Intel7SeriesChipset>(Tag{}, hw_id, pci);
  }
  if (Intel8SeriesChipset::SupportsDevice(hw_id)) {
    return absl::make_unique<Intel8SeriesChipset>(Tag{}, hw_id, pci);
  }
  if (Intel9SeriesChipset::SupportsDevice(hw_id)) {
    return absl::make_unique<Intel9SeriesChipset>(Tag{}, hw_id, pci);
  }

  return absl::UnimplementedError(
      "Unsupported Intel chipset, check hardware id.");
}

const Chipset::HardwareId& Chipset::hardware_id() const {
  return hardware_id_;
}

absl::Status Chipset::MapRootComplex(const Chipset::Rcba& rcba) {
  if (!rcba.enable) {
    return absl::InvalidArgumentError("RCBA Enable (EN) must be set");
  }
  absl::Status status;
  // The size of the root complex is aligned on 4KiB. All Intel chipsets
  // released 2008 or later have 4 pages mapped.
  // See Chipset Configuration Registers (Memory Space), p. 275-276

  auto mem_or = PhysicalMemory::Create(rcba.base_address, 0x4000 /* 16KiB */);
  if (!mem_or.ok()) {
    return mem_or.status();
  }
  rcrb_mem_ = std::move(mem_or).value();
  return status;
}

void Chipset::UnMapRootComplex() {
  rcrb_mem_.reset(nullptr);
}

absl::Status Chipset::ReadSpiWithHardwareSequencing(
    int flash_address, int size, int block_size,
    std::function<bool(int flash_address, const char* data)> block_read,
    std::function<bool(int flash_address)> block_read_error,
    std::function<void()> block_read_done) {
  // Reading the SPI flash using hardware sequencing works as follows: Intel
  // chipsets typically have 16 FDATAN registers for transferring data into or
  // out of the flash chip. This means that at most 64 bytes can be transferred
  // at a time. The actual reading works by clearing the status and error bits
  // in the status register (HSFS) and setting the flash linear address (i.e.
  // where to start reading on the chip) in the FADDR register. Then the number
  // of bytes to be read is programmed in to the control register (HSFC) and a
  // read cycle is triggered by setting the corresponding FCYCLE and FCGO bits.
  // The hardware will signal completion or a flash cycle by setting the FDONE
  // bit in the status register. To keep things simple, this function just uses
  // busy waiting for this.
  // The whole process is repeated until size / block_size blocks have been
  // read.

  CHECK_NOTNULL(block_read);
  if (block_size < 4 || block_size > 64 /* Chipset limit */) {
    // We do not support reading less than 32-bit here.
    return absl::InvalidArgumentError(
        "Block size must be a value between 4-64 (inclusive).");
  }
  if (size % block_size != 0) {
    return absl::InvalidArgumentError("Size must be divisible by block size");
  }

  auto hsfs = ReadHsfsRegister();
  if (hsfs.spi_cycle_in_progress) {
    return absl::UnavailableError("SPI flash cycle in progress");
  }

  std::vector<uint32_t> buf(block_size / 4, 0);
  for (int cur_flash_address = flash_address;
       cur_flash_address < flash_address + size;
       cur_flash_address += block_size) {
    // Copy flash lockdown and read-only bits. Clear all status bits.
    hsfs = ReadHsfsRegister();
    hsfs.access_error_log = false;
    hsfs.flash_cycle_error = false;
    hsfs.flash_cycle_done = false;
    WriteHsfsRegister(hsfs);
    // Initiate SPI flash read cycle.
    auto faddr = ReadFaddrRegister();
    faddr.flash_linear_address = cur_flash_address;
    WriteFaddrRegister(faddr);
    auto hsfc = ReadHsfcRegister();
    hsfc.flash_data_byte_count = block_size - 1;
    hsfc.flash_cycle = FlashCycle::kFcycleRead;
    hsfc.flash_cycle_go = true;
    WriteHsfcRegister(hsfc);
    do {
      hsfs = ReadHsfsRegister();
    } while (!hsfs.flash_cycle_done);
    if (hsfs.flash_cycle_error && block_read_error) {
      // We may have tried to read a protected area.
      if (!block_read_error(cur_flash_address)) {
        return absl::OkStatus();
      }
    }

    // The chipset only decodes memory accesses with a maximum width of 32-bit,
    // hence the loop in 32-bit increments.
    for (int i = 0; i < buf.size(); ++i) {
      buf[i] = ReadFdataNRegister(i);
    }
    if (!block_read(cur_flash_address,
                    reinterpret_cast<const char*>(buf.data()))) {
      return absl::OkStatus();
    }
  }
  if (block_read_done) {
    block_read_done();
  }
  return absl::OkStatus();
}

PhysicalMemory* Chipset::rcrb_mem() {
  if (rcrb_mem_ == nullptr) {
    LOG(FATAL) << "Call MapRootComplex() first.";
  }
  return rcrb_mem_.get();
}

}  // namespace security::pawn
