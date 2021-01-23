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
#include "pawn/chipset_intel_8_series.h"
#include "pawn/pci.h"
#include "pawn/physical_memory.h"

namespace security::pawn {

Intel8SeriesChipset::Intel8SeriesChipset(const Chipset::HardwareId& probed_id,
                                         Pci* pci)
    : Intel7SeriesChipset(probed_id, pci) {}

Chipset::Gcs Intel8SeriesChipset::ReadGcsRegister() {
  auto gcs = rcrb_mem()->ReadUint32(kGcsRegister);
  constexpr Chipset::BootBiosStraps kBootBiosStraps[] = {
      Chipset::kBbsLpc, Chipset::kBbsReserved, Chipset::kBbsReserved,
      Chipset::kBbsSpi};
  constexpr Chipset::BootBiosStraps kIntegratedIoBootBiosStrap[] = {
      Chipset::kBbsSpi, Chipset::kBbsLpc};
  return {
      // Integrated on-processor chipsets only use 1-bit for BBS.
      IsIntegratedIo()
          ? kIntegratedIoBootBiosStrap[bits::Value<10, 10>(
                gcs)] /* See Mobile p. 351 */
          : kBootBiosStraps[bits::Value<11, 10>(gcs)] /* See p. 358 */,
      bits::Test<0>(gcs),  // BILD
  };
}

}  // namespace zynamics
