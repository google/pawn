// Copyright 2014-2021 Google LLC. All Rights Reserved.
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

#ifndef PAWN_PHYSICAL_MEMORY_H_
#define PAWN_PHYSICAL_MEMORY_H_

#include <cstdint>
#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace security::pawn {

// Provides access to physical memory.
class PhysicalMemory {
 public:
  PhysicalMemory(const PhysicalMemory&) = delete;
  PhysicalMemory& operator=(const PhysicalMemory&) = delete;

  ~PhysicalMemory();

  static absl::StatusOr<std::unique_ptr<PhysicalMemory>> Create(
      uintptr_t physical_offset, size_t length);

  // Provides raw access to physical memory. See note below.
  void* GetAt(int offset);

  // Reads and writes physical memory in quantities of 1, 2 and 4 bytes at
  // physical location physical_offset + offset.
  // Note: No attempt is made to restrict offset in any way. Accesses beyond
  //       physical_offset + length - 1 will result in SIGSEGV or worse.
  uint8_t ReadUint8(int offset) const;
  uint16_t ReadUint16(int offset) const;
  uint32_t ReadUint32(int offset) const;
  uint64_t ReadUint64(int offset) const;
  void WriteUint8(int offset, uint8_t value);
  void WriteUint16(int offset, uint16_t value);
  void WriteUint32(int offset, uint32_t value);
  void WriteUint64(int offset, uint64_t value);

 private:
  PhysicalMemory() = default;

  absl::Status Init(uintptr_t physical_offset, size_t length);

  int mem_fd_ = -1;  // Error
  void* mem_ = nullptr;
  size_t length_ = 0;
};

}  // namespace security::pawn

#endif  // PAWN_PHYSICAL_MEMORY_H_
