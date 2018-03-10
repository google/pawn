// Copyright 2014-2018 Google LLC. All Rights Reserved.
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

#ifndef SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH9_H_
#define SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH9_H_

#include "third_party/zynamics/pawn/chipset_intel_ich8.h"
#include "third_party/zynamics/pawn/pci.h"

namespace security {
namespace zynamics {

// All page references mentioned in this header and its implementation refer to
// the Intel I/O Controller Hub 9 Family Datasheet, August 2008 (document number
// 316972-004).
class IntelIch9Chipset : public IntelIch8Chipset {
 public:
  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken from the Intel I/O Controller Hub 9 (ICH9) Family
    // Specification Update, May 2012 (document number 316973-025).
    return id.vendor == 0x8086 /* Intel */ &&
           (id.device == 0x2912 /* ICH9DH */ ||
            id.device == 0x2914 /* ICH9DO */ ||
            id.device == 0x2916 /* ICH9R */ ||
            id.device == 0x2918 /* ICH9 */ ||
            id.device == 0x2917 /* ICH9M-E */ ||
            id.device == 0x2919 /* ICH9M */);
  }

 protected:
  friend class Chipset;
  IntelIch9Chipset(const Chipset::HardwareId& probed_id, Pci* pci);

  uint16 SpiBar(int offset) const override {
    return 0x3800 + offset;  // Page 821
  }
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH9_H_
