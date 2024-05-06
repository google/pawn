// Copyright 2014-2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PAWN_CHIPSET_INTEL_7_SERIES_H_
#define PAWN_CHIPSET_INTEL_7_SERIES_H_

#include "pawn/chipset_intel_6_series.h"
#include "pawn/pci.h"

namespace security::pawn {

// All page references mentioned in this header and its implementation refer to
// the Intel 7 Series / C216 Chipset Datasheet, June 2012 (document number
// 326776-003).
class Intel7SeriesChipset : public Intel6SeriesChipset {
 public:
  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken from the Intel 7 Series / C216 Series Chipset
    // Specification Update, May 2014, Revision 015 (document number
    // 326777-015).
    return id.vendor == 0x8086 &&
           (id.device == 0x1E47 /* Q77 */ || id.device == 0x1E48 /* Q75 */ ||
            id.device == 0x1E49 /* B75 */ || id.device == 0x1E44 /* Z77 */ ||
            id.device == 0x1E46 /* Z75 */ || id.device == 0x1E4A /* H77 */ ||
            id.device == 0x1E53 /* C216 */ || id.device == 0x1E55 /* QM77 */ ||
            id.device == 0x1E58 /* UM77 */ || id.device == 0x1E57 /* HM77 */ ||
            id.device == 0x1E59 /* HM76 */ || id.device == 0x1E5D /* HM75 */ ||
            id.device == 0x1E5E /* HM70 */ || id.device == 0x1E56 /* QS77 */ ||
            id.device == 0x1E5F /* NM70 */);
  }

  Intel7SeriesChipset(Chipset::Tag tag, const Chipset::HardwareId& probed_id,
                      Pci& pci)
      : Intel6SeriesChipset(tag, probed_id, pci) {}
};

}  // namespace security::pawn

#endif  // PAWN_CHIPSET_INTEL_7_SERIES_H_
