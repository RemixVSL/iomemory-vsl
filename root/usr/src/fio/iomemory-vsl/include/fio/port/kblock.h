//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
/** @file include/fio/port/kblock.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_KBLOCK_H__
#define __FIO_PORT_KBLOCK_H__

struct kfio_bio;
struct fio_device;

#if defined( __linux__)
extern int  kfio_register_blkdev_pre_init(kfio_pci_dev_t *pdev);
extern void kfio_disk_stat_read_update(kfio_disk_t *gd, uint32_t totalsize, uint64_t duration);
extern void kfio_disk_stat_write_update(kfio_disk_t *gd, uint32_t totalsize, uint64_t duration);
extern int  kfio_get_gd_in_flight(kfio_disk_t *gd, int rw);
extern void kfio_set_gd_in_flight(kfio_disk_t *gd, int rw, int in_flight);
extern void kfio_set_write_holdoff(kfio_disk_t *disk);
extern void kfio_clear_write_holdoff(kfio_disk_t *disk);
#endif

/*
 * Functions implemented in OS-specific kblock code and called from
 * parts of the core that are not OS-aware.
 */
extern int  fio_create_blockdev(struct fio_device *dev);
extern void fio_teardown_blockdev(struct fio_device *dev);
extern void fio_start_submit_thread(struct fio_device *dev);
extern void fio_stop_submit_thread(struct fio_device *dev);

/*
 * Generic kblock-related services provided by core and available
 * to OS-specific block code.
 */
extern int  fio_open(struct fio_device *dev);
extern int  fio_release(struct fio_device *dev);
extern int  fio_ioctl(struct fio_device *dev, unsigned cmd, fio_uintptr_t arg);

extern int  fio_handle_read_bio(struct fio_device *dev, struct kfio_bio *bio);
extern int  fio_handle_write_bio(struct fio_device *dev, struct kfio_bio *bio);
extern int  fio_handle_discard_bio(struct fio_device *dev, struct kfio_bio *bio);
extern int  fio_handle_flush_bio(struct fio_device *dev, struct kfio_bio *bio);

#endif // __FIO_PORT_KBLOCK_H__
