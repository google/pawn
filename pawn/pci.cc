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

#include <sys/io.h>  // iopl(), inb(), inw(), inl(), outl()

#include <cstdint>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "pawn/pci.h"

namespace security::pawn {

Pci::Pci(Pci&& other) {
  *this = std::move(other);
}

Pci& Pci::operator=(Pci&& other) {
  iopl_done_ = other.iopl_done_;
  other.iopl_done_ = false;
  return *this;
}

Pci::~Pci() {
  if (iopl_done_) {
    iopl(0 /* Reset to ring 0 access. */);  // Ignore error.
  }
}

absl::StatusOr<Pci> Pci::Create() {
  // Note: This will not work on Windows, since there is no official API that
  //       allows to do this. A small kernel driver is needed there.
  if (iopl(3 /* Request ring 3 access to all I/O ports. */) !=
      0 /* Success */) {
    return absl::FailedPreconditionError(
        "Failed to acquire I/O privileges. Make sure this "
        "process runs as root and/or has CAP_SYS_RAWIO.");
  }

  Pci pci;
  pci.iopl_done_ = true;
  return std::move(pci);  // GCC 7 needs the extra move
}

enum {
  // I/O ports used for accessing the PCI configuration space.
  kConfigAddress = 0xCF8,
  kConfigData = 0xCFC,
};

#define DEFINE_READCONFIGUINT(read_config, int_type, in_call)                  \
  int_type read_config(uint32_t config_address) {                              \
    /* Do not touch reserved bits. */                                          \
    uint32_t reserved_bits = inl(kConfigAddress) & 0x7F000000;                 \
    outl(config_address | reserved_bits, kConfigAddress);                      \
    /* Since the PCI bridge will always copy 32-bits (as the PCI bus is */     \
    /* 32-bits wide), we must offset 3 bytes for 8-bit reads and 2 bytes */    \
    /* for 16-bit reads. The bitwise AND does the right thing for 8, 16 */     \
    /* and 32 bit accesses. */                                                 \
    return in_call(kConfigData +                                               \
                   (config_address & (sizeof(uint32_t) - sizeof(int_type))));  \
  }                                                                            \
                                                                               \
  int_type read_config(int bus, int device, int function, int offset) {        \
    return read_config(pci::MakeConfigAddress(bus, device, function, offset)); \
  }

DEFINE_READCONFIGUINT(Pci::ReadConfigUint8, uint8_t, inb);
DEFINE_READCONFIGUINT(Pci::ReadConfigUint16, uint16_t, inw);
DEFINE_READCONFIGUINT(Pci::ReadConfigUint32, uint32_t, inl);
#undef DEFINE_READCONFIGUINT

}  // namespace security::pawn
