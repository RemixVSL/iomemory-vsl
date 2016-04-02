//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
/** @file include/fio/port/linux/kblock.h 
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_LINUX_KBLOCK_H__
#define __FIO_PORT_LINUX_KBLOCK_H__

#define FIO_NUM_MINORS         16

extern int  kfio_register_blkdev(const char *name);
extern int  kfio_unregister_blkdev(const char *name);

extern int  kfio_create_disk(struct fio_device *dev, kfio_pci_dev_t *pci_dev,
                             uint32_t sector_size,
                             uint32_t max_sectors_per_request,
                             uint32_t max_sg_elements_per_request,
                             fusion_spinlock_t *queue_lock, kfio_disk_t **diskp);
extern int  kfio_expose_disk(kfio_disk_t *dp, char *name, int major, int disk_index,
                             uint64_t reported_capacity, uint32_t sector_size,
                             uint32_t max_sg_elements_per_request);
extern void kfio_destroy_disk(kfio_disk_t *disk);
extern kfio_bio_t *kfio_fetch_next_bio(kfio_disk_t *disk);

extern void fio_bio_posted(struct fio_device *dev);

#endif // __FIO_PORT_LINUX_KBLOCK_H__
