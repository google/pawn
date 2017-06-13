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

#include "third_party/zynamics/pawn/chipset_intel_ich10.h"

namespace security {
namespace zynamics {

IntelIch10Chipset::IntelIch10Chipset(const Chipset::HardwareId& probed_id,
                                   Pci* pci)
    : IntelIch9Chipset(probed_id, pci) {}

}  // namespace zynamics
}  // namespace security
