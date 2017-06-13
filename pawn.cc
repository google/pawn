// Copyright 2014-2017 Google Inc. All Rights Reserved.
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

// Pawn, a companion utility to Bishop (go/bishop) to extract BIOS firmware from
// corp machines.

#include <unistd.h>
#include <cstddef>
#include <cstdio>
#include <iomanip>
#include <memory>

#include "third_party/zynamics/pawn/bits.h"
#include "third_party/zynamics/pawn/chipset.h"
#include "third_party/zynamics/pawn/pci.h"
#include "third_party/zynamics/pawn/physical_memory.h"

namespace security {
namespace zynamics {
namespace {

int PawnMain(const char* dump_filename) {
  util::Status status;

  // We need to access the PCI configuration space, which requires to enable
  // ring-3 I/O privileges. This needs to be done as root.
  printf("Acquiring I/O port read permissions, this may fail...\n");
  auto pci(Pci::Create(&status));
  QCHECK_OK(status);

  // Read chipset vendor and device ids as well as the hardware revision. Hint:
  // a vendor id of 0x8086 is "Intel".
  // TODO(cblichmann): Do what lspci does and lookup the vendor/device to
  //                   display the device name.
  printf("Reading chipset LPC device identification: ");
  Chipset::HardwareId hw_id;
  auto chipset(Chipset::Create(pci.get(), &hw_id, &status));
  // TODO(cblichmann): Deal with Intel's "Compatible Revision Ids". They're
  //                   essentially faking RIDs on boot.
  printf("  VID: 0x%04X  DID: 0x%04X  RID: 0x%02X (%d)\n", hw_id.vendor,
         hw_id.device, hw_id.revision, hw_id.revision);
  QCHECK_OK(status);

  // Map 16KiB of chipset configuration space at the physical address indicated
  // by the RCBA register into our process. This also requires elevated
  // privileges.
  auto rcba = chipset->ReadRcbaRegister();
  // Root Complex Register Block Chipset Register Space
  printf(
      "Mapping 16KiB chipset configuration space at RCBA = 0x%8X, this may "
      "fail...\n",
      rcba.base_address);
  QCHECK_OK(chipset->MapRootComplex(rcba));

  auto gcs = chipset->ReadGcsRegister();
  const char* kBootBiosStrapsDesc[] = {"LPC", "Reserved", "PCI", "SPI"};
  printf("Boot BIOS Straps (BBS): %s\n",
         kBootBiosStrapsDesc[gcs.boot_bios_straps]);
  if (gcs.boot_bios_straps != Chipset::kBbsSpi) {
    printf("Error: BIOS not located in SPI flash.\n");
    return 1;
  }

  auto bfpr = chipset->ReadBfprRegister();
  printf("PRB: 0x%08X  PRL: 0x%08X\n", bfpr.bios_flash_primary_region_base,
         bfpr.bios_flash_primary_region_limit);

  auto frap = chipset->ReadFrapRegister();
  printf("FRAP: 0x%08X\n", *reinterpret_cast<uint32*>(&frap));

  enum { kNumFlashRegions = 5 };
  struct FlashRegion {
    Chipset::FregN freg;
    Chipset::PrN pr;
  } regions[kNumFlashRegions];
  for (int i = 0; i < arraysize(regions); ++i) {
    auto& region = regions[i];
    region = {chipset->ReadFregNRegister(i), chipset->ReadPrNRegister(i)};
    printf("FREG%d  Base: 0x%08X  Limit: 0x%08X\n", i, region.freg.region_base,
           region.freg.region_limit);
  }

  printf("BIOS protection mechanisms:\n");
  auto hsfs = chipset->ReadHsfsRegister();

  printf("  HSFS Flash Configuration Lock-Down (FLOCKDN): %d\n",
         hsfs.flash_configuration_lockdown);
  printf("  BIOS Control Register (BIOS_CNTL):\n");
  auto bios_cntl = chipset->ReadBiosCntlRegister();
  printf("    SMM BIOS Write Protect Disable (SMM_BWP):   %d\n",
         bios_cntl.smm_bios_write_protect_disable);
  printf("    BIOS Lock Enable (BLE):                     %d\n",
         bios_cntl.bios_lock_enable);
  printf("    BIOS Write Enable (BIOSWE):                 %d\n",
         bios_cntl.bios_write_enable);

  printf("  Protected Range Registers:\n");
  for (int i = 0; i < arraysize(regions); ++i) {
    const auto& region = regions[i];
    printf("    PR%d Write Protection Enable: %d\n", i,
           region.pr.write_protection_enable);
    printf("        Read Protection Enable:  %d\n",
           region.pr.read_protection_enable);
    printf("        Protected Range Base:    0x%08X  Limit: 0x%08X\n",
           region.pr.protected_range_base, region.pr.protected_range_limit);
  }

  // Ensure the chipset considers the flash descriptor valid. We don't bother
  // with the now obsolete non-descriptor mode (machines before 2009).
  printf("Flash Descriptor Valid (FDV): %d\n", hsfs.flash_descriptor_valid);
  if (!hsfs.flash_descriptor_valid) {
    printf("Warning: System not in descriptor mode!\n");
    return 1;
  }

  auto* dump = fopen(dump_filename, "wb");
  if (dump == nullptr) {
    printf("Error: Could not open output file for writing.\n");
    return 1;
  }
  std::shared_ptr<FILE> dump_closer(dump, [](FILE* dump) { fclose(dump); });

  enum {
    kBlockSize = 64,
    kMaxFlash = 16 << 20 /* 16MiB, must be divisible by kBlockSize */
  };
  printf("Reading SPI flash");
  fflush(STDIN_FILENO);

  auto ssfs = chipset->ReadSsfsRegister();
  if (ssfs.spi_cycle_in_progress) {
    printf("SPI flash cycle in progress");
    return 1;
  }

  QCHECK_OK(chipset->ReadSpiWithHardwareSequencing(
      0 /* Start address */, kMaxFlash, kBlockSize,
      [&dump](int64 fla, const char* data) -> bool {
        if (fla / kBlockSize % 256 == 0) {
          printf(".");
          fflush(STDIN_FILENO);
        }
        if (fwrite(static_cast<const void*>(data), 1 /* Size */, kBlockSize,
            dump) != kBlockSize) {
          LOG(FATAL) << "Could not write " << kBlockSize << " bytes.";
        }
        return true;
      },
      nullptr /* Ignore block read errors */, [] { printf("\n"); }));
  return 0;
}

}  // anonymous namespace
}  // namespace zynamics
}  // namespace security

int main(int argc, char* argv[]) {
  const char* dump_filename = argc == 2 ? argv[1] : "bios_via_spi_hs.bin";
  QCHECK(argc <= 2);  // NOLINT
  return security::zynamics::PawnMain(dump_filename);
}
