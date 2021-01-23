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

# Compiler checks
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  # GCC
  set(_pawn_compiler_supported TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")  # Clang or Apple Clang
  set(_pawn_compiler_supported TRUE)

  include(Sanitizers)
else()
  message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# These affect ABI and linking, so set them globally, even for dependencies
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(_pawn_compiler_supported)
  add_compile_options(-fno-exceptions)
endif()
