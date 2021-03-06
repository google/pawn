# Copyright 2014-2021 Google LLC
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
  GIT_TAG        a09b5de0d57d7b2179210989ab63361c3c1894f5 # 2021-03-25
)
set(ABSL_CXX_STANDARD ${CMAKE_CXX_STANDARD} CACHE STRING "" FORCE)
set(ABSL_USE_EXTERNAL_GOOGLETEST ON CACHE BOOL "" FORCE)
# Workaround CMake issue in Abseil
# FetchContent_MakeAvailable(absl)
if(NOT absl_POPULATED)
  FetchContent_Populate(absl)
  add_subdirectory(${absl_SOURCE_DIR} ${absl_BINARY_DIR})
  add_subdirectory(${absl_SOURCE_DIR}/absl/cleanup ${absl_BINARY_DIR}/absl/cleanup)
endif()
pawn_check_target(absl::core_headers)
