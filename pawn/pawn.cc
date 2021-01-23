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

// Pawn, a utility to extract BIOS/UEFI firmware.

#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_format.h"
#include "pawn/bits.h"
#include "pawn/chipset.h"
#include "pawn/pci.h"
#include "pawn/physical_memory.h"
#include "pawn/version.h"

ABSL_FLAG(bool, logo, true, "display version/copyright information");

namespace security::pawn {
namespace {

int PawnMain(int argc, char* argv[]) {
  const std::string usage = absl::StrFormat(
      "Extract BIOS/UEFI firmware\n"
      "Usage: %1$s [OPTION] OUTPUT\n",
      basename(argv[0]));
  absl::SetProgramUsageMessage(usage);

  std::vector<char*> parsed_argv = absl::ParseCommandLine(argc, argv);

  const char* dump_filename = parsed_argv.size() ==
      2 ? parsed_argv[1] : "bios_via_spi_hs.bin";
  QCHECK(parsed_argv.size() <= 2);  // NOLINT

  if (absl::GetFlag(FLAGS_logo)) {
    absl::PrintF("%s %s, %s\n", kPawnName, kPawnDetailedVersion,
                 kPawnCopyright);
  }
  util::Status status;

  // We need to access the PCI configuration space, which requires to enable
  // ring-3 I/O privileges. This needs to be done as root.
  absl::PrintF("Acquiring I/O port read permissions, this may fail...\n");
  auto pci = Pci::Create(&status);
  QCHECK_OK(status);

  // Read chipset vendor and device ids as well as the hardware revision. Hint:
  // a vendor id of 0x8086 is "Intel".
  // TODO(cblichmann): Do what lspci does and lookup the vendor/device to
  //                   display the device name.
  absl::PrintF("Reading chipset LPC device identification: ");
  Chipset::HardwareId hw_id;
  auto chipset = Chipset::Create(pci.get(), &hw_id, &status);
  // TODO(cblichmann): Deal with Intel's "Compatible Revision Ids". They're
  //                   essentially faking RIDs on boot.
  absl::PrintF("  VID: 0x%04X  DID: 0x%04X  RID: 0x%02X (%d)\n", hw_id.vendor,
               hw_id.device, hw_id.revision, hw_id.revision);
  QCHECK_OK(status);

  // Map 16KiB of chipset configuration space at the physical address indicated
  // by the RCBA register into our process. This also requires elevated
  // privileges.
  auto rcba = chipset->ReadRcbaRegister();
  // Root Complex Register Block Chipset Register Space
  absl::PrintF(
      "Mapping 16KiB chipset configuration space at RCBA = 0x%8X, this may "
      "fail...\n",
      rcba.base_address);
  status = chipset->MapRootComplex(rcba);
  if (!status.ok()) {
    absl::PrintF(
        "Error: %s\n"
        "       Check if your kernel was compiled with IO_STRICT_DEVMEM=y.\n"
        "       On Debian kernels > 4.8.4, boot with iomem=relaxed to\n"
        "       temporarily disable /dev/mem IO protection.\n",
        status.error_message().c_str());
    return EXIT_FAILURE;
  }

  auto gcs = chipset->ReadGcsRegister();
  constexpr const char* kBootBiosStrapsDesc[] = {"LPC", "Reserved", "PCI",
                                                 "SPI"};
  absl::PrintF("Boot BIOS Straps (BBS): %s\n",
               kBootBiosStrapsDesc[gcs.boot_bios_straps]);
  if (gcs.boot_bios_straps != Chipset::kBbsSpi) {
    absl::PrintF("Error: BIOS not located in SPI flash.\n");
    return EXIT_FAILURE;
  }

  auto bfpr = chipset->ReadBfprRegister();
  absl::PrintF("PRB: 0x%08X  PRL: 0x%08X\n",
               bfpr.bios_flash_primary_region_base,
               bfpr.bios_flash_primary_region_limit);

  auto frap = chipset->ReadFrapRegister();
  absl::PrintF("FRAP: 0x%08X\n", *reinterpret_cast<uint32*>(&frap));

  enum { kNumFlashRegions = 5 };
  struct FlashRegion {
    Chipset::FregN freg;
    Chipset::PrN pr;
  } regions[kNumFlashRegions];
  for (int i = 0; i < arraysize(regions); ++i) {
    auto& region = regions[i];
    region = {chipset->ReadFregNRegister(i), chipset->ReadPrNRegister(i)};
    absl::PrintF("FREG%d  Base: 0x%08X  Limit: 0x%08X\n", i,
                 region.freg.region_base, region.freg.region_limit);
  }

  absl::PrintF("BIOS protection mechanisms:\n");
  auto hsfs = chipset->ReadHsfsRegister();

  absl::PrintF("  HSFS Flash Configuration Lock-Down (FLOCKDN): %d\n",
               hsfs.flash_configuration_lockdown);
  absl::PrintF("  BIOS Control Register (BIOS_CNTL):\n");
  auto bios_cntl = chipset->ReadBiosCntlRegister();
  absl::PrintF("    SMM BIOS Write Protect Disable (SMM_BWP):   %d\n",
               bios_cntl.smm_bios_write_protect_disable);
  absl::PrintF("    BIOS Lock Enable (BLE):                     %d\n",
               bios_cntl.bios_lock_enable);
  absl::PrintF("    BIOS Write Enable (BIOSWE):                 %d\n",
               bios_cntl.bios_write_enable);

  absl::PrintF("  Protected Range Registers:\n");
  for (int i = 0; i < arraysize(regions); ++i) {
    const auto& region = regions[i];
    absl::PrintF("    PR%d Write Protection Enable: %d\n", i,
                 region.pr.write_protection_enable);
    absl::PrintF("        Read Protection Enable:  %d\n",
                 region.pr.read_protection_enable);
    absl::PrintF("        Protected Range Base:    0x%08X  Limit: 0x%08X\n",
                 region.pr.protected_range_base,
                 region.pr.protected_range_limit);
  }

  // Ensure the chipset considers the flash descriptor valid. We don't bother
  // with the now obsolete non-descriptor mode (machines before 2009).
  absl::PrintF("Flash Descriptor Valid (FDV): %d\n",
               hsfs.flash_descriptor_valid);
  if (!hsfs.flash_descriptor_valid) {
    absl::PrintF("Warning: System not in descriptor mode!\n");
    return EXIT_FAILURE;
  }

  auto* dump = fopen(dump_filename, "wb");
  if (dump == nullptr) {
    absl::PrintF("Error: Could not open output file for writing.\n");
    return 1;
  }
  std::shared_ptr<FILE> dump_closer(dump, [](FILE* dump) { fclose(dump); });

  enum {
    kBlockSize = 64,
    kMaxFlash = 16 << 20 /* 16MiB, must be divisible by kBlockSize */
  };
  absl::PrintF("Reading SPI flash");
  fflush(STDIN_FILENO);

  auto ssfs = chipset->ReadSsfsRegister();
  if (ssfs.spi_cycle_in_progress) {
    absl::PrintF("SPI flash cycle in progress");
    return EXIT_FAILURE;
  }

  QCHECK_OK(chipset->ReadSpiWithHardwareSequencing(
      0 /* Start address */, kMaxFlash, kBlockSize,
      [&dump](int64 fla, const char* data) -> bool {
        if (fla / kBlockSize % 256 == 0) {
          absl::PrintF(".");
          fflush(STDIN_FILENO);
        }
        if (fwrite(static_cast<const void*>(data), 1 /* Size */, kBlockSize,
                   dump) != kBlockSize) {
          LOG(FATAL) << "Could not write " << kBlockSize << " bytes.";
        }
        return true;
      },
      nullptr /* Ignore block read errors */, [] { absl::PrintF("\n"); }));
  return EXIT_SUCCESS;
}

}  // anonymous namespace
}  // namespace security::pawn

int main(int argc, char* argv[]) {
  return security::pawn::PawnMain(argc, argv);
}
