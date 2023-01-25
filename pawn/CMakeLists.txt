# Copyright 2014-2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

configure_file(version.h.in version.h ESCAPE_QUOTES @ONLY)

add_library(pawn_bits STATIC
  bits.h
)
add_library(pawn::bits ALIAS pawn_bits)
set_target_properties(pawn_bits PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pawn_bits PRIVATE
  pawn_base
)
if(BUILD_TESTING AND PAWN_BUILD_TESTING)
  add_executable(pawn_bits_test
    bits_test.cc
  )
  target_link_libraries(pawn_bits_test PUBLIC
    pawn::base
    pawn::test_base
  )
  gtest_discover_tests(pawn_bits_test)
endif()

add_library(pawn_memory STATIC
  physical_memory.cc
  physical_memory.h
)
add_library(pawn::memory ALIAS pawn_memory)
target_link_libraries(pawn_memory PRIVATE
  pawn_base
  absl::status
)

add_library(pawn_pci STATIC
  pci.cc
  pci.h
)
add_library(pawn::pci ALIAS pawn_pci)
target_link_libraries(pawn_pci PRIVATE
  pawn::base
  absl::status
  absl::statusor
  pawn::bits
)

add_library(pawn_chipsets STATIC
  chipset.cc
  chipset.h
  chipset_intel_6_series.cc
  chipset_intel_6_series.h
  chipset_intel_7_series.h
  chipset_intel_8_series.cc
  chipset_intel_8_series.h
  chipset_intel_9_series.h
  chipset_intel_ich8.cc
  chipset_intel_ich8.h
  chipset_intel_ich9.h
  chipset_intel_ich10.h
)
add_library(pawn::chipsets ALIAS pawn_chipsets)
target_link_libraries(pawn_chipsets PRIVATE
  pawn_base
  absl::die_if_null
  absl::log
  absl::status
  absl::statusor
  pawn::bits
  pawn::memory
  pawn::pci
)

add_executable(pawn
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  pawn.cc
)
target_link_libraries(pawn
  pawn_base
  absl::cleanup
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::status
  absl::str_format
  absl::strings
  pawn::chipsets
  absl::log
  pawn::memory
  pawn::pci
)

install(TARGETS pawn DESTINATION ${CMAKE_INSTALL_SBINDIR})
