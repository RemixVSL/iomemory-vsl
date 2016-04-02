//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the SanDisk Corp. nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

/// @file
///     userspace PCI CSR access emulation.

#ifndef CSR_SIMULATOR_H_
# define CSR_SIMULATOR_H_

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <pthread.h>

# include <fio/port/ktypes.h>

struct userspace_fake_pci_dev;
typedef void (* simulated_register_write)(uint32_t val, uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef uint32_t (* simulated_register_read)(uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef void (* simulated_config_write8)(uint8_t val, uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef uint8_t (* simulated_config_read8)(uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef void (* simulated_config_write16)(uint16_t val, uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef uint16_t (* simulated_config_read16)(uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef void (* simulated_config_write32)(uint32_t val, uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef uint32_t (* simulated_config_read32)(uint32_t reg_addr, struct userspace_fake_pci_dev *context);
typedef void (* simulated_interrupt)(void);

typedef void (*userspace_fake_pci_dump_state)(struct userspace_fake_pci_dev *pci_dev);
typedef void (*bitbang_readid)(struct userspace_fake_pci_dev *pci_dev, uint32_t bar_offset, uint8_t ce, uint32_t pads, uint32_t bufwidth, uint8_t idlen, uint8_t *buffer);
typedef void (*bitbang_readstat)(struct userspace_fake_pci_dev *pci_dev, uint32_t bar_offset, uint8_t ce, uint32_t pads, uint8_t *buffer);

struct fake_pci_mmio_access
{
    simulated_register_read  read;
    simulated_register_write write;
};

typedef struct userspace_fake_pci_dev
{
    char                     name[32];
    uint16_t                 vendor_id;
    uint16_t                 device_id;
    uint16_t                 subsystem_vendor_id;
    uint16_t                 subsystem_device_id;
    uint8_t                  slotnum;
    uint8_t                  bus;
    uint8_t                  devicenum;
    uint8_t                  function;

    struct fake_pci_mmio_access bar[6];
    struct fake_pci_mmio_access expansion_rom;
    simulated_config_read8   config_read8;
    simulated_config_write8  config_write8;
    simulated_config_read16  config_read16;
    simulated_config_write16 config_write16;
    simulated_config_read32  config_read32;
    simulated_config_write32 config_write32;
    simulated_interrupt      interrupt;
    void                    *isr_arg;

    userspace_fake_pci_dump_state dump_state; // Debug hook

    void                    *private_data; // private storage for the structure owner
    void                    *drvdata;      // storage for pci_get_drvdata/pci_set_drvdata

    unsigned                 refcount;
    pthread_t                interrupt_thread;
    uint32_t                 terminate_interrupt_thread;

    // Ugly NANDSIM specific hooks since bitbang registers are not directly supported
    bitbang_readid           bitbang_readid;
    bitbang_readstat         bitbang_readstat;
} userspace_fake_pci_dev_t;

#endif
