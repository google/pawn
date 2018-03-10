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

#ifndef SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH10_H_
#define SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH10_H_

#include "third_party/zynamics/pawn/chipset_intel_ich9.h"
#include "third_party/zynamics/pawn/pci.h"

namespace security {
namespace zynamics {

// All page references mentioned in this header and its implementation refer to
// the Intel I/O Controller Hub 10 Family Datasheet, October 2008 (document
// number 319973-003).
class IntelIch10Chipset : public IntelIch9Chipset {
 public:
  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken from the Intel I/O Controller Hub 10 (Intel ICH10)
    // Family Specification Update, September 2013 (document number
    // 319974-017US).
    return id.vendor == 0x8086 /* Intel */ &&
           (id.device == 0x3A14 /* ICH10DO */ ||
            id.device == 0x3A1A /* ICH10D */ ||
            id.device == 0x3A16 /* ICH10R */ ||
            id.device == 0x3A18 /* ICH10 (Consumer Base) */);
  }

 protected:
  friend class Chipset;
  IntelIch10Chipset(const Chipset::HardwareId& probed_id, Pci* pci);
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_ICH10_H_
