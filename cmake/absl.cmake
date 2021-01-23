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

# Directly build with the project as recommended upstream
find_path(absl_src_dir
  absl/base/port.h
  HINTS ${ABSL_ROOT_DIR}
  PATHS ${PROJECT_BINARY_DIR}/absl
)
add_subdirectory(${absl_src_dir} ${PROJECT_BINARY_DIR}/absl
                 EXCLUDE_FROM_ALL)
