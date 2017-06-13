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

#ifndef SECURITY_ZYNAMICS_PAWN_PCI_H_
#define SECURITY_ZYNAMICS_PAWN_PCI_H_

#ifndef __linux__
#error "Unsupported operating system."
#endif

#include <memory>

#include "third_party/zynamics/pawn/bits.h"
#include "third_party/zynamics/pawn/mini_google.h"

namespace security {
namespace zynamics {

namespace pci {

constexpr uint32 MakeConfigAddress(int bus, int device, int function,
                                   int offset) {
  // See PCI Local Bus Specification 3.0, Volume 1, Page 50:
  return bits::Set<31, 31>(1) |        // Bit 31: Config Transaction Enable
         bits::Set<23, 16>(bus) |      // Bits 23-16: Bus Number
         bits::Set<15, 11>(device) |   // Bits 15-11: Device Number
         bits::Set<10, 8>(function) |  // Bits 10-8: Function Number
         // Bits 2-7: Register Number, Bits 0-1: Should be zero (per PCI spec)
         // Note: We don't mask out bits 0-1, since they are used to access the
         //       PCI header data in I/O configuration space.
         bits::Set<7, 0>(offset);
}

enum {
  // The following registers should always be available at B0:D31:F0 and are
  // part of the (standardized) PCI header.

  // Vendor Identification Register (16-bit)
  kVidRegister = MakeConfigAddress(0x00, 31, 0, 0x00),

  // Device Identification Register (16-bit)
  kDidRegister = MakeConfigAddress(0x00, 31, 0, 0x02),

  // Device Identification Register (8-bit)
  kRidRegister = MakeConfigAddress(0x00, 31, 0, 0x08),

  // Note: These are all taken from the Intel X79 Express Chipset Datasheet
  //       (November 2011).

  // BIOS Decode Enable Register (Page 358, 16-bit)
  kBiosDecEn1Register = MakeConfigAddress(0x00, 31, 0, 0xD8),
};

}  // namespace pci

// Utility class for accessing the PCI configuration space.
class Pci {
 public:
  ~Pci();

  static std::unique_ptr<Pci> Create(util::Status* status);

  uint8 ReadConfigUint8(int bus, int device, int function, int offset);
  uint8 ReadConfigUint8(uint32 config_address);
  uint16 ReadConfigUint16(int bus, int device, int function, int offset);
  uint16 ReadConfigUint16(uint32 config_address);
  uint32 ReadConfigUint32(int bus, int device, int function, int offset);
  uint32 ReadConfigUint32(uint32 config_address);

 private:
  Pci();
  util::Status Init();

  DISALLOW_COPY_AND_ASSIGN(Pci);
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_PCI_H_
