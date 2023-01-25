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

cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)
project(pawn VERSION 0.2)

# System checks
if(NOT UNIX)
  # Pawn currently only supports Linux. The BSDs _may_ also work.
  message(FATAL_ERROR "Unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()

# Pawn settings
set(PAWN_BINARY_DIR "${PROJECT_BINARY_DIR}" CACHE INTERNAL "" FORCE)
set(PAWN_SOURCE_DIR "${PROJECT_SOURCE_DIR}" CACHE INTERNAL "" FORCE)

# CMake settings
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
list(APPEND CMAKE_MODULE_PATH "${PAWN_SOURCE_DIR}/cmake")

# CMake modules, order matters
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
include(CompileOptions)
include(PawnOptions)
include(PawnDeps)

include(CTest)
if(BUILD_TESTING AND PAWN_BUILD_TESTING)
  include(GoogleTest)
  enable_testing()
endif()

# Find the Git revision number, if applicable
set(pawn_REVISION unknown)
if(Git_FOUND)
  execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
                  WORKING_DIRECTORY "${PAWN_SOURCE_DIR}"
                  OUTPUT_VARIABLE pawn_REVISION ERROR_QUIET)
  if(NOT pawn_REVISION STREQUAL "")
    string(STRIP "${pawn_REVISION}" pawn_REVISION)
  else()
    set(pawn_REVISION internal)
  endif()
endif()

# Interface library with include paths and settings
add_library(pawn_base INTERFACE)
target_include_directories(pawn_base INTERFACE
  ${PAWN_SOURCE_DIR}
  ${PAWN_BINARY_DIR}  # For generated files
)
add_library(pawn::base ALIAS pawn_base)

if(BUILD_TESTING AND PAWN_BUILD_TESTING)
  # Interface library to be used by tests
  add_library(pawn_test_base INTERFACE)
  target_link_libraries(pawn_test_base INTERFACE
    gtest_main
    gmock
  )
  add_library(pawn::test_base ALIAS pawn_test_base)
endif()

add_subdirectory(pawn)
