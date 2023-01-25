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
  GIT_TAG        b0a2b10bb125a90b35727be67b972f4e5b89283b # 2023-01-24
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
    GIT_TAG        2491710524094dc3b77cfa4e8eda2ba8edb0d91c # 2023-01-24
  )
  FetchContent_MakeAvailable(googletest)
  pawn_check_target(gtest)
  pawn_check_target(gtest_main)
  pawn_check_target(gmock)
endif()