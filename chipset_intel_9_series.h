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

#ifndef SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_9_SERIES_H_
#define SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_9_SERIES_H_

#include "third_party/zynamics/pawn/chipset_intel_8_series.h"
#include "third_party/zynamics/pawn/pci.h"

namespace security {
namespace zynamics {

// All page references mentioned in this header and its implementation refer to
// the Intel 9 Series Chipset Datasheet, May 2014 (document number 330550-001).
// References to the mobile variants refer to the Mobile 5th Generation Intel
// Core Processor Familiy I/O Datasheet, January 2015 (document number
// 330837-003).
class Intel9SeriesChipset : public Intel8SeriesChipset {
 public:
  static constexpr bool IsIntegratedIo(uint16 device) {
    // Device ids were taken from the mobile datasheet, page 63.
    return (device == 0x9CC1 /* Engineering Sample, U-Processor */ ||
            device == 0x9CC2 /* Engineering Sample, U-Processor */ ||
            device == 0x9CC3 /* Premium SKU, U-Processor */ ||
            device == 0x9CC5 /* Base SKU, U-Processor */ ||
            device == 0x9CC6 /* Engineering Sample, M-Processor */ ||
            device == 0x9CC7 /* Premium SKU, M-Processor */ ||
            device == 0x9CC9 /* Base SKU, M-Processor */);
  }

  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken directly from the datasheet, page 54.
    return id.vendor == 0x8086 &&
           (IsIntegratedIo(id.device) ||
            id.device == 0x8CC2 /* Full featured engineering sample */ ||
            id.device == 0x8CC4 /* Z97 */ || id.device == 0x8CC6 /* H97 */);
  }

 protected:
  friend class Chipset;
  Intel9SeriesChipset(const Chipset::HardwareId& probed_id, Pci* pci);

  virtual bool IsIntegratedIo() {
    // Call the static constexpr version, hence the identical code compared to
    // Intel8SeriesChipset.
    return IsIntegratedIo(hardware_id().device);
  }
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_CHIPSET_INTEL_9_SERIES_H_
