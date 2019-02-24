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

/** @file
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OS X v 10.6+
 * (MS Windows 2003 64 bit)
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_CDEV_H__
#define __FIO_PORT_CDEV_H__

struct fusion_nand_device;

/**
 * @struct fusion_misc_device
 * @brief placeholder struct that is large enough for the @e real OS-specific
 *        variant
 */
struct fusion_misc_device {
    char foo[120];
};
typedef struct fusion_misc_device kfio_misc_device_t;

typedef struct __fusion_poll_struct kfio_poll_struct;

extern void kfio_poll_init(kfio_poll_struct *p);

/**
 * @brief Returns the first fusion_nand_device in the system
 */
struct fusion_nand_device *fusion_nand_get_first(void);

/**
 * @brief Return the next fusion_nand_device in the system
 */
struct fusion_nand_device *fusion_nand_get_next(struct fusion_nand_device *last);

/**
 * @brief Gets the kernels miscdevice structure located inside the
 *        fusion_nand_device
 */
kfio_misc_device_t *fusion_nand_get_miscdev(struct fusion_nand_device *entry);

/**
 * @brief Get the device number of the nand device
 */
uint32_t fusion_nand_get_devnum(struct fusion_nand_device *entry);

/**
 * @brief Get the PCI bus name of the device
 */
const char *fusion_nand_get_bus_name(struct fusion_nand_device *entry);

/**
 * @brief Get the device name of the fusion_nand_device
 */
char *fusion_nand_get_dev_name(struct fusion_nand_device *entry);

/**
 * @brief get kfio_pci_dev_t * given fusion_nand_device
 */
void *fusion_nand_get_pci_dev(const struct fusion_nand_device *entry);

/**
 * @brief The wrapper will call this function to get ioctl information to the
 *        kernel.
 */
int fusion_control_ioctl(void *nand, unsigned int cmd, fio_uintptr_t arg);
/**
 * Returns 1 if event set, else 0
 */
int is_nand_device_event_set(struct fusion_nand_device *entry);

/**
 * @brief get kfio_poll_struct * given fusion_nand_device
 */
kfio_poll_struct *nand_device_get_poll_struct(struct fusion_nand_device *entry);

/**
 * @brief wakes anyone waiting on poll()
 */
void kfio_poll_wake(kfio_poll_struct *p);

/**
 * @brief returns 1 iff the first argument refers to an ioctl that must
 * be called on a block device.
 */
int fusion_is_block_ioctl(unsigned cmd);

#endif // __FIO_PORT_CDEV_H__
