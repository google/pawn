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

# Build Types
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "\
Choose the type of build, options are: None Debug Release RelWithDebInfo \
MinSizeRel asan msan" FORCE)

# AddressSanitizer
set(_pawn_asan_flags
"-fsanitize=address\
 -fno-optimize-sibling-calls\
 -fsanitize-address-use-after-scope\
 -fno-omit-frame-pointer\
 -g\
 -O1\
")
set(CMAKE_C_FLAGS_ASAN ${_pawn_asan_flags} CACHE STRING
    "Flags used by the C compiler during AddressSanitizer builds." FORCE)
set(CMAKE_CXX_FLAGS_ASAN ${_pawn_asan_flags} CACHE STRING
    "Flags used by the C++ compiler during AddressSanitizer builds." FORCE)

# MemorySanitizer
set(_pawn_msan_flags
"-fsanitize=memory\
 -fno-optimize-sibling-calls\
 -fsanitize-memory-track-origins=2\
 -fno-omit-frame-pointer\
 -g\
 -O2\
")
set(CMAKE_C_FLAGS_MSAN ${_pawn_msan_flags} CACHE STRING
    "Flags used by the C compiler during MemorySanitizer builds." FORCE)
set(CMAKE_CXX_FLAGS_MSAN ${_pawn_msan_flags} CACHE STRING
    "Flags used by the C++ compiler during MemorySanitizer builds." FORCE)
