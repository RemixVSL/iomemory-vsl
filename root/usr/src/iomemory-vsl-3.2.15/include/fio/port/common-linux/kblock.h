//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015 SanDisk Corp. and/or all its affiliates. All rights reserved.
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
#ifndef __FIO_PORT_LINUX_KBLOCK_H__
#define __FIO_PORT_LINUX_KBLOCK_H__

#define FIO_NUM_MINORS         16

#include <fio/port/vectored.h>

typedef enum
{
    destroy_type_normal,
    destroy_type_force
} destroy_type_t;

extern int  kfio_init_storage_interface(void);
extern int  kfio_teardown_storage_interface(void);
#ifdef __VMKLNX__
extern int kfio_register_esx_blkdev(kfio_pci_dev_t *pdev);
extern int kfio_unregister_esx_blkdev(unsigned int major, const char *name);
#endif

extern int  kfio_create_disk(struct fio_device *dev, kfio_pci_dev_t *pci_dev,
                             uint32_t sector_size,
                             uint32_t max_sectors_per_request,
                             uint32_t max_sg_elements_per_request,
                             fusion_spinlock_t *queue_lock, kfio_disk_t **diskp,
                             kfio_numa_node_t node);
extern int  kfio_expose_disk(kfio_disk_t *dp, char *name, int major, int disk_index,
                             uint64_t reported_capacity, uint32_t sector_size,
                             uint32_t max_sg_elements_per_request);
extern void kfio_destroy_disk(kfio_disk_t *disk, destroy_type_t dt);
extern kfio_bio_t *kfio_fetch_next_bio(kfio_disk_t *disk);

extern void fio_bio_posted(struct fio_device *dev);

#define FIO_ATOMIC_WRITE_USER_IOV      0x01
#define FIO_ATOMIC_WRITE_USER_PAGES    0x02

extern int kfio_handle_atomic(struct fio_device *dev, const struct kfio_iovec *iov, uint32_t iovcnt, uint32_t *bytes_written, uint32_t flags);
extern int fio_count_sectors_inuse(struct fio_device *dev, uint64_t base, uint64_t length, uint64_t *count);

#endif // __FIO_PORT_LINUX_KBLOCK_H__
