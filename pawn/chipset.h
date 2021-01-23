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

// Interface to Intel chipsets that aims to abstract away the differences
// between the various chipset generations.
// Use like this:
//   // Get access to PCI.
//   util::Status status;
//   auto pci = Pci::Create(&status);
//   QCHECK_OK(status);
//   // Get an instance of Chipset for the current hardware.
//   auto chipset = Chipset::Create(pci.get(),
//                                  nullptr /* Do not fill hardware id */,
//                                  &status);
//   QCHECK_OK(status);
//   QCHECK_OK(chipset->MapRootComplex(chipset->ReadRcbaRegister())));
//   ...
//
// All abbreviations and terms are taken from the latest publicly available
// Intel datasheets (the "Intel 8 Series/C220 Series Chipset Family Platform
// Controller Hub (PCH)" datasheet (document number 328904-003) at the time of
// this writing).

#ifndef SECURITY_ZYNAMICS_PAWN_CHIPSET_H_
#define SECURITY_ZYNAMICS_PAWN_CHIPSET_H_

#include <functional>
#include <memory>

#include "pawn/mini_google.h"

namespace security::pawn {

class Pci;
class PhysicalMemory;

class Chipset {
 public:
  struct HardwareId {
    uint16 vendor;
    uint16 device;
    uint8 revision;
  };

  enum SpiReadConfiguration {
    kSrcNoPrefetchButCache = 0,
    kSrcNoPrefetchNoCache,
    kSrcPrefetchAndCache,
    kSrcReserved
  };

  // BIOS Control Register
  struct BiosCntl {
    bool smm_bios_write_protect_disable : 1;          // SMM_BWP or Reserved
    bool top_swap_status : 1;                         // TSS
    SpiReadConfiguration spi_read_configuration : 2;  // SRC
    bool bios_lock_enable : 1;                        // BLE
    bool bios_write_enable : 1;                       // BWE
  };

  // Root Complex Base Address Register
  struct Rcba {
    uint32 base_address;  // BA
    bool enable;          // EN
  };

  enum BootBiosStraps { kBbsLpc = 0, kBbsReserved, kBbsPci, kBbsSpi };

  // General Control Status Register
  struct Gcs {
    BootBiosStraps boot_bios_straps;  // BBS
    bool bios_interface_lockdown;     // BILD
  };

  // BIOS Flash Primary Region Register
  struct Bfpr {
    uint32 reserved31 : 3;                   // Reserved
    uint32 bios_flash_primary_region_limit;  // PRL
    uint32 reserved15 : 3;                   // Reserved
    uint32 bios_flash_primary_region_base;   // PRB
  };

  enum BlockSectorEraseSize {
    kBerase256Kb = 0,
    kBerase4Kb,
    kBerase8KbOrReserved,
    kBerase64Kb
  };

  // Hardware Sequencing Flash Status Register
  struct Hsfs {
    bool flash_configuration_lockdown : 1;               // FLOCKDN
    bool flash_descriptor_valid : 1;                     // FDV
    bool flash_descriptor_override_pinstrap_status : 1;  // FDOPSS
    uint32 reserved12 : 7;                               // Reserved
    bool spi_cycle_in_progress : 1;                      // SCIP
    BlockSectorEraseSize blocksector_erase_size : 2;     // BERASE
    bool access_error_log : 1;                           // AEL
    bool flash_cycle_error : 1;                          // FCERR
    bool flash_cycle_done : 1;                           // FDONE
  };

  enum FlashCycle {
    kFcycleRead = 0,
    kFcycleReserved,
    kFcycleWrite,
    kFcycleBlockErase
  };

  // Hardware Sequencing Flash Control Register
  struct Hsfc {
    bool flash_spi_smi_enable : 1;     // FSMIE
    bool reserved14 : 1;               // Reserved
    uint32 flash_data_byte_count : 6;  // FDBC
    uint32 reserved7 : 5;              // Reserved
    FlashCycle flash_cycle : 2;        // FCYCLE
    bool flash_cycle_go : 1;           // FGO
  };

  // Flash Address Register
  struct Faddr {
    uint32 reserved25 : 7;             // Reserved
    uint32 flash_linear_address : 25;  // FLA
  };

  // Flash Regions Access Permissions Register
  struct Frap {
    uint32 bios_master_write_access_grant : 8;  // BMWAG
    uint32 bios_master_read_access_grant : 8;   // BMRAG
    uint32 bios_region_write_access : 8;        // BRWA
    uint32 bios_region_read_access : 8;         // BRRA
  };

  // Flash Region N Register
  struct FregN {
    uint32 reserved31 : 3;     // Reserved
    uint32 region_limit : 13;  // RL
    uint32 reserved15 : 3;     // Reserved
    uint32 region_base : 13;   // RB
  };

  // Protected Range N Register
  struct PrN {
    bool write_protection_enable : 1;
    uint32 reserved30 : 2;  // Reserved
    uint32 protected_range_limit : 13;
    bool read_protection_enable : 1;
    uint32 reserved14 : 2;  // Reserved
    uint32 protected_range_base : 13;
  };

  // Software Sequencing Flash Status Register
  struct Ssfs {
    uint32 reserved7 : 3;        // Reserved
    bool access_error_log : 1;   // AEL
    bool flash_cycle_error : 1;  // FCERR
    bool cycle_done_status : 1;
    bool reserved1 : 1;              // Reserved
    bool spi_cycle_in_progress : 1;  // SCIP
  };

  enum SpiCycleFrequency {
    kScf20Mhz = 0,
    kScf33Mhz
  };

  // Software Sequencing Flash Control Register
  struct Ssfc {
    uint32 reserved23 : 5;                      // Reserved
    SpiCycleFrequency spi_cycle_frequency : 3;  // SCF
    bool spi_smi_enable : 1;                    // SME
    bool data_cycle : 1;                        // DS
    uint32 data_byte_count : 6;                 // DBC
    bool reserved7 : 1;                         // Reserved
    uint32 cycle_opcode_pointer : 3;            // COP
    bool sequence_prefix_opcode_pointer : 1;    // SPOP
    bool atomic_cycle_sequence : 1;             // ACS
    bool spi_cycle_go : 1;                      // SCGO
    bool reserved0 : 1;                         // Reserved
  };

  Chipset(const Chipset&) = delete;
  Chipset& operator=(const Chipset&) = delete;

  virtual ~Chipset();

  // Creates a new Chipset instance by probing the PCI bus and setting the
  // appropriate register and base offsets if the chipset is supported.
  // Otherwise, status is set accordingly. If probed_id is non-nullptr, it is
  // filled with the PCI hardware id of the chipset's LPC device.
  static std::unique_ptr<Chipset> Create(Pci* pci, HardwareId* probed_id,
                                         util::Status* status);

  const HardwareId& hardware_id() const;

  // Registers in PCI Configuration Space.
  virtual BiosCntl ReadBiosCntlRegister() = 0;
  virtual Rcba ReadRcbaRegister() = 0;

  // Map the Chipset Configuration Space physical memory.
  virtual util::Status MapRootComplex(const Rcba& rcba);
  void UnMapRootComplex();

  // Registers in Chipset Configuration Space (Memory Space).
  virtual Gcs ReadGcsRegister() = 0;

  // SPI Memory Mapped Configuration Registers.
  virtual Bfpr ReadBfprRegister() = 0;
  virtual Hsfs ReadHsfsRegister() = 0;
  virtual Hsfc ReadHsfcRegister() = 0;
  virtual Faddr ReadFaddrRegister() = 0;
  virtual uint32 ReadFdataNRegister(int register_num) = 0;
  virtual Frap ReadFrapRegister() = 0;
  virtual FregN ReadFregNRegister(int index) = 0;
  virtual PrN ReadPrNRegister(int index) = 0;
  virtual Ssfs ReadSsfsRegister() = 0;
  virtual Ssfc ReadSsfcRegister() = 0;

  // Reads the contents of the SPI flash using the hardware sequencing method.
  // This method reads size bytes in blocks of block_size starting at flash
  // linear address flash_address.
  // Callbacks: On each successful read block, block_read is called with the
  // current flash linear address and the data that was read. If there was a
  // read error for any reason (including SPI read protection), block_read_error
  // is called with the current block address. block_read_error may be nullptr.
  // If either of the block_read or block_read_error callbacks return false,
  // this function stops reading and returns with util::Status::OK.
  // block_read_done is called after reading.
  virtual util::Status ReadSpiWithHardwareSequencing(
      int flash_address, int size, int block_size,
      std::function<bool(int flash_address, const char* data)> block_read,
      std::function<bool(int flash_address)> block_read_error,
      std::function<void()> block_read_done);

  // TODO(cblichmann): Public for now, the Pawn command-line tool currently
  //                   needs this.
  PhysicalMemory* rcrb_mem();

 protected:
  // Make constructor available to deriving classes.
  Chipset(const HardwareId& probed_id, Pci* pci);

  Pci* pci();

  // Returns this chipset's SPIBAR value, usually 0x3800.
  virtual uint16 SpiBar(int offset) const = 0;

  virtual void WriteHsfsRegister(const Hsfs& hsfs) = 0;
  virtual void WriteHsfcRegister(const Hsfc& hsfc) = 0;
  virtual void WriteFaddrRegister(const Faddr& faddr) = 0;
  virtual void WriteSsfsRegister(const Ssfs& ssfs) = 0;
  virtual void WriteSsfcRegister(const Ssfc& ssfc) = 0;

 private:
  HardwareId hardware_id_;
  Pci* pci_;
  std::unique_ptr<PhysicalMemory> rcrb_mem_;
};

}  // namespace security::pawn

#endif  // SECURITY_ZYNAMICS_PAWN_CHIPSET_H_
