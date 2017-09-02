// Copyright 2014-2017 Google Inc. All Rights Reserved.
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

#ifndef SECURITY_ZYNAMICS_PAWN_PHYSICAL_MEMORY_H_
#define SECURITY_ZYNAMICS_PAWN_PHYSICAL_MEMORY_H_

#include <cstdint>
#include <memory>

#include "third_party/zynamics/pawn/mini_google.h"

namespace security {
namespace zynamics {

// Provides access to physical memory.
class PhysicalMemory {
 public:
  PhysicalMemory(const PhysicalMemory&) = delete;
  PhysicalMemory& operator=(const PhysicalMemory&) = delete;

  ~PhysicalMemory();

  static std::unique_ptr<PhysicalMemory> Create(uintptr_t physical_offset,
                                                size_t length,
                                                util::Status* status);

  // Raw access to physical memory. See note below.
  void* GetAt(int offset);

  // Read and write physical memory in quantities of 1, 2 and 4 bytes at
  // physical location physical_offset + offset.
  // Note: No attempt is made to restrict offset in any way. Accesses beyond
  //       physical_offset + length - 1 will result in SIGSEGV or worse.
  uint8 ReadUint8(int offset) const;
  uint16 ReadUint16(int offset) const;
  uint32 ReadUint32(int offset) const;
  uint64 ReadUint64(int offset) const;
  void WriteUint8(int offset, uint8 value);
  void WriteUint16(int offset, uint16 value);
  void WriteUint32(int offset, uint32 value);
  void WriteUint64(int offset, uint64 value);

 private:
  PhysicalMemory();
  util::Status Init(uintptr_t physical_offset, size_t length);

  int mem_fd_;
  void* mem_;
  size_t length_;
};

}  // namespace zynamics
}  // namespace security

#endif  // SECURITY_ZYNAMICS_PAWN_PHYSICAL_MEMORY_H_
