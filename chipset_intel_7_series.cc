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

#include "third_party/zynamics/pawn/bits.h"
#include "third_party/zynamics/pawn/chipset_intel_7_series.h"
#include "third_party/zynamics/pawn/pci.h"
#include "third_party/zynamics/pawn/physical_memory.h"

namespace security {
namespace zynamics {

Intel7SeriesChipset::Intel7SeriesChipset(const Chipset::HardwareId& probed_id,
                                         Pci* pci)
    : Intel6SeriesChipset(probed_id, pci) {}

}  // namespace zynamics
}  // namespace security
