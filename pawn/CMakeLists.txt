# Copyright 2014-2021 Google LLC. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

configure_file(version.h.in version.h ESCAPE_QUOTES @ONLY)

add_executable(pawn
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  bits.h
  chipset.cc
  chipset.h
  chipset_intel_6_series.cc
  chipset_intel_6_series.h
  chipset_intel_7_series.cc
  chipset_intel_7_series.h
  chipset_intel_8_series.cc
  chipset_intel_8_series.h
  chipset_intel_9_series.cc
  chipset_intel_9_series.h
  chipset_intel_ich10.cc
  chipset_intel_ich10.h
  chipset_intel_ich8.cc
  chipset_intel_ich8.h
  chipset_intel_ich9.cc
  chipset_intel_ich9.h
  pawn.cc
  pci.cc
  pci.h
  physical_memory.cc
  physical_memory.h
)
target_link_libraries(pawn
  pawn_base
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::str_format
  absl::status
  absl::strings
)

install(TARGETS pawn DESTINATION ${CMAKE_INSTALL_SBINDIR})
