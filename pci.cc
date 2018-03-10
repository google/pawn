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

#include <sys/io.h>  // iopl(), inb(), inw(), inl(), outl()

#include "third_party/zynamics/pawn/mini_google.h"
#include "third_party/zynamics/pawn/pci.h"

namespace security {
namespace zynamics {

Pci::~Pci() {
  iopl(0 /* Reset to ring 0 access. */);  // Ignore error.
}

std::unique_ptr<Pci> Pci::Create(util::Status* status) {
  std::unique_ptr<Pci> pci(new Pci());
  *CHECK_NOTNULL(status) = pci->Init();
  return status->ok() ? std::move(pci) : nullptr;
}

util::Status Pci::Init() {
  // Note: This will not work on Windows, since there is no official API that
  //       allows to do this. A small kernel driver is needed there.
  if (iopl(3 /* Request ring 3 access to all I/O ports. */) !=
            0 /* Success */) {
    return util::Status(util::error::FAILED_PRECONDITION,
                        "Failed to acquire I/O privileges. Make sure this "
                        "process runs as root and/or has CAP_SYS_RAWIO.");
  }
  return util::OkStatus();
}

enum {
  // I/O ports used for accessing the PCI configuration space.
  kConfigAddress = 0xCF8,
  kConfigData = 0xCFC
};

#define DEFINE_READCONFIGUINT(read_config, int_type, in_call)                  \
  int_type read_config(uint32 config_address) {                                \
    /* Do not touch reserved bits. */                                          \
    uint32 reserved_bits = inl(kConfigAddress) & 0x7F000000;                   \
    outl(config_address | reserved_bits, kConfigAddress);                      \
    /* Since the PCI bridge will always copy 32-bits (as the PCI bus is */     \
    /* 32-bits wide), we must offset 3 bytes for 8-bit reads and 2 bytes */    \
    /* for 16-bit reads. The bitwise AND does the right thing for 8, 16 */     \
    /* and 32 bit accesses. */                                                 \
    return in_call(kConfigData +                                               \
                   (config_address & (sizeof(uint32) - sizeof(int_type))));    \
  }                                                                            \
                                                                               \
  int_type read_config(int bus, int device, int function, int offset) {        \
    return read_config(pci::MakeConfigAddress(bus, device, function, offset)); \
  }

DEFINE_READCONFIGUINT(Pci::ReadConfigUint8, uint8, inb);
DEFINE_READCONFIGUINT(Pci::ReadConfigUint16, uint16, inw);
DEFINE_READCONFIGUINT(Pci::ReadConfigUint32, uint32, inl);
#undef DEFINE_READCONFIGUINT

}  // namespace zynamics
}  // namespace security
