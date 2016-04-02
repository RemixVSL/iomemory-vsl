//-----------------------------------------------------------------------------                      
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/// @file userspace PCI CSR access emulation.

#ifndef CSR_SIMULATOR_H_
# define CSR_SIMULATOR_H_

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# include <pthread.h>

# include <fio/port/ktypes.h>

typedef void (* simulated_register_write)(uint32_t val, uint32_t reg_addr, void *context);
typedef uint32_t (* simulated_register_read)(uint32_t reg_addr, void *context);

struct userspace_fake_pci_dev;

typedef void (* ufpd_destructor)(struct userspace_fake_pci_dev *dev);

typedef struct userspace_fake_pci_dev
{
    char                     name[32];
    unsigned                 refcount;
    uint16_t                 vendor;
    uint32_t                 devnum;
    uint16_t                 subsystem_vendor;
    uint16_t                 subsystem_device;
    void                    *drvdata;
    void                    *client_data;
    uint8_t                  slotnum;
    uint8_t                  bus;
    uint8_t                  devicenum;
    uint8_t                  function;
    simulated_register_read  simulated_read;
    simulated_register_write simulated_write;
    uint32_t                 indirect_reg;
    pthread_t                interrupt_thread;
    uint32_t                 terminate_interrupt_thread;
    void                    *isr_arg;
    ufpd_destructor          destructor;
} userspace_fake_pci_dev_t;

#endif
