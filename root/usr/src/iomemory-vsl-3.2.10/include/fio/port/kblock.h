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
/** @file include/fio/port/kblock.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_KBLOCK_H__
#define __FIO_PORT_KBLOCK_H__

struct kfio_bio;
struct fio_device;
struct kfio_disk;

#if defined( __linux__)
extern int  kfio_register_blkdev_pre_init(kfio_pci_dev_t *pdev);
extern void kfio_disk_stat_read_update(struct kfio_disk *gd, uint64_t totalsize, uint64_t duration);
extern void kfio_disk_stat_write_update(struct kfio_disk *gd, uint64_t totalsize, uint64_t duration);
extern int  kfio_get_gd_in_flight(struct kfio_disk *gd, int rw);
extern void kfio_set_gd_in_flight(struct kfio_disk *gd, int rw, int in_flight);
extern void kfio_set_write_holdoff(struct kfio_disk *disk);
extern void kfio_clear_write_holdoff(struct kfio_disk *disk);
extern void kfio_mark_lock_pending(struct kfio_disk *disk);
extern void kfio_unmark_lock_pending(struct kfio_disk *disk);
#else
// non-linux stubs
static inline void kfio_disk_stat_write_update(void *gd, uint64_t totalsize, uint64_t duration) { }
static inline int  kfio_get_gd_in_flight(void *gd, int rw) { return 0; }
static inline void kfio_set_gd_in_flight(void *gd, int rw, int in_flight) { }
static inline void kfio_mark_lock_pending(void *disk) { }
static inline void kfio_unmark_lock_pending(void *disk) { }
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

extern void fio_restart_read_bio(struct kfio_bio *bio, int error);

extern uint32_t fio_device_ptrim_available(struct fio_device *dev);

#endif // __FIO_PORT_KBLOCK_H__
