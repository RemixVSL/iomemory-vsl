//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2016 SanDisk Corp. and/or all its affiliates. All rights reserved.
// Copyright (c) 2016-2017 Western Digital Technologies, Inc. All rights reserved.
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
/** @file include/fio/port/kbio.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_KBIO_H__
#define __FIO_PORT_KBIO_H__

#include "fio/port/compiler.h"
#include <fio/port/kscatter.h>
#include <fio/port/errno.h>

struct fio_device;
struct fusion_ioctx;

typedef struct kfio_bio kfio_bio_t;
typedef void (*kfio_bio_completor_t)(kfio_bio_t *bio, uint64_t bytes_complete, int error);

#define BIO_DIR_READ        0
#define BIO_DIR_WRITE       1

#define KBIO_CMD_INVALID    0
#define KBIO_CMD_READ       1
#define KBIO_CMD_WRITE      2
#define KBIO_CMD_DISCARD    3
#define KBIO_CMD_FLUSH      4

/**
 * Fusion BIO Flags
 *
 * @note With the exception of ACTIVE, DONE, and DUMP flags, all other flags
 * must remain constant after kfio_bio_submit call.
 * @{
 */
#define KBIO_FLG_SYNC       0x001
#define KBIO_FLG_BARRIER    0x002
#define KBIO_FLG_DC         0x004
#define KBIO_FLG_DIRTY      0x008
#define KBIO_FLG_WAIT       0x010
#define KBIO_FLG_NONBLOCK   0x020
#define KBIO_FLG_DUMP       0x040
/** @} */

#define FIO_BIO_INVALID_SECTOR   0xFFFFFFFFFFFFFFFFULL

#define FUSION_BIO_CLIENT_RESERVED_SIZE      112

struct kfio_bio
{
    struct fio_device     *fbio_dev;
    struct fusion_ioctx   *fbio_ctx;
    kfio_sg_list_t        *fbio_sgl;              //< The SGL this bio will use (non-discard)
    kfio_dma_map_t        *fbio_dmap;
    fusion_list_entry_t    fbio_list;
    int32_t                fbio_error;            //< Error status of this bio
    uint32_t               fbio_flags;            //< Additional KBIO_FLG options to use
    uint64_t               fbio_size;
    uint64_t               fbio_offset;
    uint64_t               fbio_sector;
    uint8_t                fbio_cmd;              //< The operation type for this bio
    kfio_cpu_t             fbio_cpu;
    kfio_bio_completor_t   fbio_completor;
    fio_uintptr_t          fbio_parameter;
    uint64_t               fbio_start_time;       //< Microtime used by stats for R/W requests.
    uint64_t               fbio_bytes_done;
#if (KFIO_SCSI_DEVICE==1)
    uint8_t                fbio_data[FUSION_BIO_CLIENT_RESERVED_SIZE]; ///< space for client data
#endif
};

#define kfio_bio_sync(bio)  (((bio)->fbio_flags & KBIO_FLG_SYNC) != 0)

extern int kfio_bio_should_fail_requests(struct fio_device *dev);

extern kfio_bio_t *kfio_bio_alloc(struct fio_device *dev);
extern kfio_bio_t *kfio_bio_try_alloc(struct fio_device *dev);
extern void kfio_bio_free(kfio_bio_t *bio);

__must_use_result
extern int kfio_bio_submit_handle_retryable(kfio_bio_t *bio);
extern int kfio_bio_submit(kfio_bio_t *bio);
extern int  kfio_bio_wait(kfio_bio_t *bio);

// Test if result of kfio_bio_submit_handle_retryable() is a retryable failure or a permanent failure.
//
// If the failure is permanent, the request has already been completed and the caller must *not*
// touch the bio. Otherwise the bio is still live and caller may choose what to do with it.
static inline int kfio_bio_failure_is_retryable(int status)
{
    return (status == -ENOSPC) || (status == -EBUSY);
}

extern void kfio_bio_endio(kfio_bio_t *bio, uint64_t bytes_done, int error);
struct iodrive_channel;
extern void kfio_bio_queue_completion(struct iodrive_channel *chan, kfio_bio_t *bio, uint64_t bytes_done, int error);

extern void kfio_dump_fbio(kfio_bio_t *bio);

#endif /* __FIO_PORT_KBIO_H__ */
