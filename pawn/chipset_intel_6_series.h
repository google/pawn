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

#ifndef PAWN_CHIPSET_INTEL_6_SERIES_H_
#define PAWN_CHIPSET_INTEL_6_SERIES_H_

#include "pawn/chipset_intel_ich10.h"
#include "pawn/pci.h"

namespace security::pawn {

// All page references mentioned in this header and its implementation refer to
// the Intel 6 Series / C200 Chipset Datasheet, May 2011 (document number
// 324645-006).
class Intel6SeriesChipset : public IntelIch10Chipset {
 public:
  static constexpr bool SupportsDevice(const Chipset::HardwareId& id) {
    // Device ids were taken from the Intel 6 Series / C200 Series Chipset
    // Specification Update, June 2013 (document number 324646-020).
    return id.vendor == 0x8086 /* Intel */ &&
           (id.device == 0x1C44 || id.device == 0x1C46 || id.device == 0x1C47 ||
            id.device == 0x1C49 || id.device == 0x1C4A || id.device == 0x1C4B ||
            id.device == 0x1C4C || id.device == 0x1C4D || id.device == 0x1C4E ||
            id.device == 0x1C4F || id.device == 0x1C50 || id.device == 0x1C52 ||
            id.device == 0x1C54 || id.device == 0x1C56 || id.device == 0x1C5C ||
            id.device == 0x1D40 /* X79 Express */ ||
            id.device == 0x1D41 /* X79 Express (HP z240) */);
  }

  Intel6SeriesChipset(Chipset::Tag tag, const Chipset::HardwareId& probed_id,
                      Pci& pci)
      : IntelIch10Chipset(tag, probed_id, pci) {}

  BiosCntl ReadBiosCntlRegister() override;
  Chipset::Gcs ReadGcsRegister() override;
};

}  // namespace security::pawn

#endif  // PAWN_CHIPSET_INTEL_6_SERIES_H_
