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

#ifndef SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH8_H_
#define SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH8_H_

#include "third_party/zynamics/pawn/chipset.h"
#include "third_party/zynamics/pawn/pci.h"

namespace security {
namespace zynamics {

// All page references mentioned in this header and its implementation refer to
// the Intel I/O Controller Hub 8 Family Datasheet, May 2014 (document number
// 313056-004).
class IntelIch8Chipset : public Chipset {
 public:
  enum {
    // BIOS Control Register (Page 360, 8-bit)
    kBiosCntlRegister = pci::MakeConfigAddress(0x00, 31, 0, 0xDC),
    // Root Complex Base Address Register (Page 363, 32-bit)
    kRcbaRegister = pci::MakeConfigAddress(0x00, 31, 0, 0xF0),

    // General Control and Status Register (Page 405, 32-bit)
    kGcsRegister = 0x3410,

    kBfprRegisterOffset = 0x00,
    kHsfsRegisterOffset = 0x04,
    kHsfcRegisterOffset = 0x06,
    kFaddrRegisterOffset = 0x08,
    kFdata0RegisterOffset = 0x10,
    kFrapRegisterOffset = 0x50,
    kFreg0RegisterOffset = 0x54,
    kPr0RegisterOffset = 0x74,
    kSsfsRegisterOffset = 0x90,
    kSsfcRegisterOffset = 0x91,
  };

  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken from the Intel I/O Controller Hub 8 (ICH8) Family
    // Specification Update, May 2012 (document number 313057-025).
    return id.vendor == 0x8086 /* Intel */ &&
           (id.device == 0x2810 /* ICH8/ICH8R */ ||
            id.device == 0x2812 /* ICH8DH */ ||
            id.device == 0x2814 /* ICH8DO */ ||
            id.device == 0x2811 /* ICH8M-E */);
  }

  BiosCntl ReadBiosCntlRegister() override;
  Chipset::Rcba ReadRcbaRegister() override;

  Chipset::Gcs ReadGcsRegister() override;

  Chipset::Bfpr ReadBfprRegister() override;
  Chipset::Hsfs ReadHsfsRegister() override;
  Chipset::Hsfc ReadHsfcRegister() override;
  Chipset::Faddr ReadFaddrRegister() override;
  uint32 ReadFdataNRegister(int index) override;
  Chipset::Frap ReadFrapRegister() override;
  Chipset::FregN ReadFregNRegister(int index) override;
  Chipset::PrN ReadPrNRegister(int index) override;
  Chipset::Ssfs ReadSsfsRegister() override;
  Chipset::Ssfc ReadSsfcRegister() override;

 protected:
  friend class Chipset;
  IntelIch8Chipset(const Chipset::HardwareId& probed_id, Pci* pci);

  uint16 SpiBar(int offset) const override {
    return 0x3020 + offset;  // Page 747
  }

  void WriteHsfsRegister(const Chipset::Hsfs& hsfs) override;
  void WriteHsfcRegister(const Chipset::Hsfc& hsfc) override;
  void WriteFaddrRegister(const Chipset::Faddr& faddr) override;
  void WriteSsfsRegister(const Chipset::Ssfs& ssfs) override;
  void WriteSsfcRegister(const Chipset::Ssfc& ssfc) override;
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH8_H_
