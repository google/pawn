// Copyright 2014-2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pawn/physical_memory.h"

#include <fcntl.h>     // O_RDONLY
#include <sys/mman.h>  // mmap()
#include <sys/stat.h>  // open()
#include <unistd.h>    // close()

#include <cerrno>
#include <cstring>
#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

namespace security::pawn {

PhysicalMemory::~PhysicalMemory() {
  if (mem_) {
    munmap(mem_, length_);
    close(mem_fd_);
  }
}

absl::StatusOr<std::unique_ptr<PhysicalMemory>> PhysicalMemory::Create(
    uintptr_t physical_offset, size_t length) {
  std::unique_ptr<PhysicalMemory> mem(new PhysicalMemory());
  if (auto status = mem->Init(physical_offset, length); !status.ok()) {
    return status;
  }
  return mem;  // GCC 7 needs the extra move
}

absl::Status PhysicalMemory::Init(uintptr_t physical_offset, size_t length) {
  length_ = length;

  mem_fd_ = open("/dev/mem", O_RDWR);
  if (mem_fd_ == -1 /* Error */) {
    return absl::FailedPreconditionError(
        "Could not open physical memory file. Make sure this process runs as "
        "root.");
  }

  mem_ = mmap(nullptr /* Address hint */, length_, PROT_READ | PROT_WRITE,
              MAP_SHARED, mem_fd_, physical_offset);
  if (mem_ == MAP_FAILED) {
    mem_ = nullptr;
    return absl::FailedPreconditionError(
        absl::StrCat("Could not map physical memory: ", std::strerror(errno)));
  }
  return absl::OkStatus();
}

void* PhysicalMemory::GetAt(int offset) {
  return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

uint8_t PhysicalMemory::ReadUint8(int offset) const {
  return *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(mem_) +
                                     offset);
}

uint16_t PhysicalMemory::ReadUint16(int offset) const {
  return *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(mem_) +
                                      offset);
}

uint32_t PhysicalMemory::ReadUint32(int offset) const {
  return *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(mem_) +
                                      offset);
}

uint64_t PhysicalMemory::ReadUint64(int offset) const {
  return *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(mem_) +
                                      offset);
}

void PhysicalMemory::WriteUint8(int offset, uint8_t value) {
  *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

void PhysicalMemory::WriteUint16(int offset, uint16_t value) {
  *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

void PhysicalMemory::WriteUint32(int offset, uint32_t value) {
  *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

void PhysicalMemory::WriteUint64(int offset, uint64_t value) {
  *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

}  // namespace security::pawn
