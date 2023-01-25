// Copyright 2014-2023 Google LLC
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

#include "pawn/chipset_intel_6_series.h"

#include "pawn/bits.h"
#include "pawn/pci.h"
#include "pawn/physical_memory.h"

namespace security::pawn {

Chipset::BiosCntl Intel6SeriesChipset::ReadBiosCntlRegister() {
  auto bios_cntl = pci().ReadConfigUint8(kBiosCntlRegister);
  return {
      bits::Test<5>(bios_cntl),  // SMM_BWP
      bits::Test<4>(bios_cntl),  // TSS
      static_cast<Chipset::SpiReadConfiguration>(bits::Value<3, 2>(
          bios_cntl)),           // SRC, these bits map directly for 6 Series
      bits::Test<1>(bios_cntl),  // BLE
      bits::Test<0>(bios_cntl),  // BWE
  };
}

Chipset::Gcs Intel6SeriesChipset::ReadGcsRegister() {
  auto gcs = rcrb_mem()->ReadUint32(kGcsRegister);
  return {
      static_cast<Chipset::BootBiosStraps>(
          bits::Value<11, 10>(gcs)),  // BBS, 6-Series, these bits map directly.
      bits::Test<0>(gcs),             // BILD
  };
}

}  // namespace security::pawn
