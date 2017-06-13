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

#include "third_party/zynamics/pawn/physical_memory.h"

#include <fcntl.h>     // O_RDONLY
#include <sys/mman.h>  // mmap()
#include <sys/stat.h>  // open()
#include <unistd.h>    // close()

namespace security {
namespace zynamics {

PhysicalMemory::PhysicalMemory()
    : mem_fd_(-1 /* Error */), mem_(nullptr), length_(0) {}

PhysicalMemory::~PhysicalMemory() {
  munmap(mem_, length_);
  close(mem_fd_);
}

std::unique_ptr<PhysicalMemory> PhysicalMemory::Create(
    uintptr_t physical_offset, size_t length, util::Status* status) {
  CHECK_NOTNULL(status);
  std::unique_ptr<PhysicalMemory> mem(new PhysicalMemory());
  *status = mem->Init(physical_offset, length);
  return status->ok() ? std::move(mem) : nullptr;
}

util::Status PhysicalMemory::Init(uintptr_t physical_offset, size_t length) {
  length_ = length;

  mem_fd_ = open("/dev/mem", O_RDWR);
  if (mem_fd_ == -1 /* Error */) {
    return util::Status(util::error::FAILED_PRECONDITION,
                        "Could not open physical memory file. Make sure this "
                        "process runs as root.");
  }

  mem_ = mmap(nullptr /* Address hint */, length_, PROT_READ | PROT_WRITE,
              MAP_SHARED, mem_fd_, physical_offset);
  if (mem_ == MAP_FAILED) {
    return util::Status(util::error::FAILED_PRECONDITION,
                        "Could not map physical memory.");
  }
  return util::Status();  // OK
}

void* PhysicalMemory::GetAt(int offset) {
  return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

uint8 PhysicalMemory::ReadUint8(int offset) const {
  return *reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

uint16 PhysicalMemory::ReadUint16(int offset) const {
  return *reinterpret_cast<uint16*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

uint32 PhysicalMemory::ReadUint32(int offset) const {
  return *reinterpret_cast<uint32*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

uint64 PhysicalMemory::ReadUint64(int offset) const {
  return *reinterpret_cast<uint64*>(reinterpret_cast<uintptr_t>(mem_) + offset);
}

void PhysicalMemory::WriteUint8(int offset, uint8 value) {
  *reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(mem_) + offset) = value;
}

void PhysicalMemory::WriteUint16(int offset, uint16 value) {
  *reinterpret_cast<uint16*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

void PhysicalMemory::WriteUint32(int offset, uint32 value) {
  *reinterpret_cast<uint32*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

void PhysicalMemory::WriteUint64(int offset, uint64 value) {
  *reinterpret_cast<uint64*>(reinterpret_cast<uintptr_t>(mem_) + offset) =
      value;
}

}  // namespace zynamics
}  // namespace security
