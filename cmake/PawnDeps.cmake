# Copyright 2014-2024 Google LLC
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

function(pawn_check_target target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR
      "Compiling ${PROJECT_NAME} requires a ${target} CMake "
      "target in your project")
  endif()
endfunction()

include(FetchContent)

find_package(Git)

# Abseil
FetchContent_Declare(absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp
  GIT_TAG        c1e1b47d989978cde8c5a2a219df425b785a0c47 # 2024-05-03
)
set(ABSL_CXX_STANDARD ${CMAKE_CXX_STANDARD} CACHE STRING "" FORCE)
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
set(ABSL_USE_EXTERNAL_GOOGLETEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(absl)
pawn_check_target(absl::core_headers)

if(BUILD_TESTING AND PAWN_BUILD_TESTING)
  # Googletest (needs to come after Abseil due to C++ standard propagation)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        2d16ed055d09c3689d44b272adc097393de948a0 # 2024-05-03
  )
  FetchContent_MakeAvailable(googletest)
  pawn_check_target(gtest)
  pawn_check_target(gtest_main)
  pawn_check_target(gmock)
endif()
