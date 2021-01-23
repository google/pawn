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

#include "pawn/bits.h"
#include "pawn/chipset_intel_ich8.h"
#include "pawn/pci.h"
#include "pawn/physical_memory.h"

namespace security::pawn {

IntelIch8Chipset::IntelIch8Chipset(const Chipset::HardwareId& probed_id,
                                   Pci* pci)
    : Chipset(probed_id, pci) {}

Chipset::BiosCntl IntelIch8Chipset::ReadBiosCntlRegister() {
  auto bios_cntl = pci()->ReadConfigUint8(kBiosCntlRegister);
  return {
      0,                         // Reserved, SMM_BWP on PCH platforms.
      bits::Test<4>(bios_cntl),  // TSS
      static_cast<Chipset::SpiReadConfiguration>(bits::Value<3, 2>(
          bios_cntl)),           // SRC, these bits map directly for ICH8.
      bits::Test<1>(bios_cntl),  // BLE
      bits::Test<0>(bios_cntl),  // BWE
  };
}

Chipset::Rcba IntelIch8Chipset::ReadRcbaRegister() {
  auto rcba = pci()->ReadConfigUint32(kRcbaRegister);
  return {bits::Raw<31, 14>(rcba), bits::Test<0>(rcba)};  // See p. 475
}

Chipset::Gcs IntelIch8Chipset::ReadGcsRegister() {
  auto gcs = rcrb_mem()->ReadUint32(kGcsRegister);
  constexpr Chipset::BootBiosStraps kBootBiosStraps[] = {
      Chipset::kBbsSpi, Chipset::kBbsSpi, Chipset::kBbsPci, Chipset::kBbsLpc};
  return {
      kBootBiosStraps[bits::Value<11, 10>(gcs)],  // BBS
      bits::Test<0>(gcs),                         // BILD
  };
}

Chipset::Bfpr IntelIch8Chipset::ReadBfprRegister() {
  auto bfpr = rcrb_mem()->ReadUint32(SpiBar(kBfprRegisterOffset));
  return {
      bits::Value<31, 29>(bfpr),                     // Reserved
      bits::Set<24, 12>(bits::Value<28, 16>(bfpr)),  // PRL
      bits::Value<15, 13>(bfpr),                     // Reserved
      bits::Set<24, 12>(bits::Value<12, 0>(bfpr))    // PRB
  };
}

Chipset::Hsfs IntelIch8Chipset::ReadHsfsRegister() {
  auto hsfs = rcrb_mem()->ReadUint16(SpiBar(kHsfsRegisterOffset));
  return {
      bits::Test<15>(hsfs),      // FLOCKDN
      bits::Test<14>(hsfs),      // FDV
      bits::Test<13>(hsfs),      // FDOPSS
      bits::Value<12, 6>(hsfs),  // Reserved
      bits::Test<5>(hsfs),       // SCIP
      static_cast<Chipset::BlockSectorEraseSize>(bits::Value<4, 3>(
          hsfs)),           // BERASE, these bits map directly for ICH8.
      bits::Test<2>(hsfs),  // AEL
      bits::Test<1>(hsfs),  // FCERR
      bits::Test<0>(hsfs),  // FDONE
  };
}

void IntelIch8Chipset::WriteHsfsRegister(const Chipset::Hsfs& hsfs) {
  rcrb_mem()->WriteUint16(
      SpiBar(kHsfsRegisterOffset),
      bits::Set<15, 15>(hsfs.flash_configuration_lockdown) |
          bits::Set<14, 14>(hsfs.flash_descriptor_valid) |
          bits::Set<13, 13>(hsfs.flash_descriptor_override_pinstrap_status) |
          bits::Set<12, 6>(hsfs.reserved12) |
          bits::Set<5, 5>(hsfs.spi_cycle_in_progress) |
          bits::Set<4, 3>(static_cast<uint32>(hsfs.blocksector_erase_size)) |
          bits::Set<2, 2>(hsfs.access_error_log) |
          bits::Set<1, 1>(hsfs.flash_cycle_error) |
          bits::Set<0, 0>(hsfs.flash_cycle_done));
}

Chipset::Hsfc IntelIch8Chipset::ReadHsfcRegister() {
  auto hsfc = rcrb_mem()->ReadUint16(SpiBar(kHsfcRegisterOffset));
  return {
      bits::Test<15>(hsfc),                                       // FSMIE
      bits::Test<14>(hsfc),                                       // Reserved
      bits::Value<13, 8>(hsfc),                                   // FDBC
      bits::Value<7, 3>(hsfc),                                    // Reserved
      static_cast<Chipset::FlashCycle>(bits::Value<2, 1>(hsfc)),  // FCYCLE
      bits::Test<0>(hsfc),                                        // FGO
  };
}

void IntelIch8Chipset::WriteHsfcRegister(const Chipset::Hsfc& hsfc) {
  rcrb_mem()->WriteUint16(
      SpiBar(kHsfcRegisterOffset),
      bits::Set<15, 15>(hsfc.flash_spi_smi_enable) |
          bits::Set<14, 14>(hsfc.reserved14) |
          bits::Set<13, 8>(hsfc.flash_data_byte_count) |
          bits::Set<7, 3>(hsfc.reserved7) |
          bits::Set<2, 1>(static_cast<uint32>(hsfc.flash_cycle)) |
          bits::Set<0, 0>(hsfc.flash_cycle_go));
}

Chipset::Faddr IntelIch8Chipset::ReadFaddrRegister() {
  auto faddr = rcrb_mem()->ReadUint32(SpiBar(kFaddrRegisterOffset));
  return {bits::Value<31, 25>(faddr), bits::Value<24, 0>(faddr) /* FLA */};
}

void IntelIch8Chipset::WriteFaddrRegister(const Chipset::Faddr& faddr) {
  rcrb_mem()->WriteUint32(
      SpiBar(kFaddrRegisterOffset),
      bits::Set<31, 25>(faddr.reserved25) |
          bits::Set<24, 0>(faddr.flash_linear_address) /* FLA */);
}

void IntelIch8Chipset::WriteSsfsRegister(const Chipset::Ssfs& ssfs) {
  rcrb_mem()->WriteUint8(
      SpiBar(kSsfsRegisterOffset),
      bits::Set<7, 5>(ssfs.reserved7) |              // Reserved
          bits::Set<4, 4>(ssfs.access_error_log) |   // AEL
          bits::Set<3, 3>(ssfs.flash_cycle_error) |  // FCERR
          bits::Set<2, 2>(ssfs.cycle_done_status) |
          bits::Set<1, 1>(ssfs.reserved1) |              // Reserved
          bits::Set<0, 0>(ssfs.spi_cycle_in_progress));  // SCIP
}

void IntelIch8Chipset::WriteSsfcRegister(const Chipset::Ssfc& ssfc) {
  // Split 24-bit register into an 8-bit and a 16-bit write. Make sure the SCGO
  // bit is written with the second write.
  rcrb_mem()->WriteUint8(SpiBar(kSsfcRegisterOffset),
                         bits::Set<7, 3>(ssfc.reserved7) |  // Reserved
                             bits::Set<2, 0>(static_cast<uint32>(
                                 ssfc.spi_cycle_frequency)));  // SCF
  rcrb_mem()->WriteUint16(
      SpiBar(kSsfcRegisterOffset + 1 /* 1 byte */),
      bits::Set<15, 15>(ssfc.spi_smi_enable) |                    // SME
          bits::Set<14, 14>(ssfc.data_cycle) |                    // DS
          bits::Set<13, 8>(ssfc.data_byte_count) |                // DBC
          bits::Set<7, 7>(ssfc.reserved7) |                       // Reserved
          bits::Set<6, 4>(ssfc.cycle_opcode_pointer) |            // COP
          bits::Set<3, 3>(ssfc.sequence_prefix_opcode_pointer) |  // SPOP
          bits::Set<2, 2>(ssfc.atomic_cycle_sequence) |           // ACS
          bits::Set<1, 1>(ssfc.spi_cycle_go) |                    // SCGO
          bits::Set<0, 0>(ssfc.reserved0));                       // Reserved
}

uint32 IntelIch8Chipset::ReadFdataNRegister(int register_num) {
  if (register_num < 0 || register_num > 16) {
    LOG(FATAL) << "Flash data register out of range (must be in 0..15).";
  }
  return rcrb_mem()->ReadUint32(
      SpiBar(kFdata0RegisterOffset + register_num * 4 /* 32-bit */));
}

Chipset::Frap IntelIch8Chipset::ReadFrapRegister() {
  auto frap = rcrb_mem()->ReadUint32(SpiBar(kFrapRegisterOffset));
  return {
      bits::Value<31, 24>(frap),  // BMWAG
      bits::Value<23, 16>(frap),  // BMRAG
      bits::Value<15, 8>(frap),   // BRWA
      bits::Value<7, 0>(frap)     // BRRA
  };
}

Chipset::FregN IntelIch8Chipset::ReadFregNRegister(int index) {
  auto fregn = rcrb_mem()->ReadUint32(
      SpiBar(kFreg0RegisterOffset + index * 4 /* 32-bit */));
  return {
      bits::Value<31, 29>(fregn),                             // Reserved
      bits::Set<24, 12>(bits::Value<28, 16>(fregn)) | 0xFFF,  // RL
      bits::Value<15, 13>(fregn),                             // Reserved
      bits::Value<12, 0>(fregn)                               // RB
  };
}

Chipset::PrN IntelIch8Chipset::ReadPrNRegister(int index) {
  auto prn = rcrb_mem()->ReadUint32(
      SpiBar(kPr0RegisterOffset + index * 4 /* 32-bit */));
  return {
      bits::Test<31>(prn),       // Write Protection Enable
      bits::Value<30, 29>(prn),  // Reserved
      bits::Set<24, 12>(bits::Value<28, 16>(prn)) |
          0xFFF,                                  // Protected Range Limit
      bits::Test<15>(prn),                        // Read Protection Enable
      bits::Value<14, 13>(prn),                   // Reserved
      bits::Set<24, 12>(bits::Value<12, 0>(prn))  // Protected Range Base
  };
}

Chipset::Ssfs IntelIch8Chipset::ReadSsfsRegister() {
  auto ssfs = rcrb_mem()->ReadUint8(SpiBar(kSsfsRegisterOffset));
  return {
      bits::Value<7, 5>(ssfs),  // Reserved
      bits::Test<4>(ssfs),      // AEL
      bits::Test<3>(ssfs),      // FCERR
      bits::Test<2>(ssfs),      // Cycle Done Status
      bits::Test<1>(ssfs),      // Reserved
      bits::Test<0>(ssfs),      // SCIP
  };
}

Chipset::Ssfc IntelIch8Chipset::ReadSsfcRegister() {
  auto ssfc = rcrb_mem()->ReadUint32(SpiBar(kSsfcRegisterOffset));
  return {
      bits::Value<23, 19>(ssfc),  // Reserved
      static_cast<Chipset::SpiCycleFrequency>(
          bits::Value<18, 16>(ssfc)),  // SCF
      bits::Test<15>(ssfc),            // SME
      bits::Test<14>(ssfc),            // DS
      bits::Value<13, 8>(ssfc),        // DBC
      bits::Test<7>(ssfc),             // Reserved
      bits::Value<6, 4>(ssfc),         // COP
      bits::Test<3>(ssfc),             // SPOP
      bits::Test<2>(ssfc),             // ACS
      bits::Test<1>(ssfc),             // SCGO
      bits::Test<0>(ssfc),             // Reserved
  };
}

}  // namespace security::pawn
