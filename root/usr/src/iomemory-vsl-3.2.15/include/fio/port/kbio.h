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
/** @file include/fio/port/kbio.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_KBIO_H__
#define __FIO_PORT_KBIO_H__

#include "fio/port/compiler.h"

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

#define KBIO_FLG_SYNC       1
#define KBIO_FLG_BARRIER    2
#define KBIO_FLG_DC         4
#define KBIO_FLG_DIRTY      8
#define KBIO_FLG_WAIT       16
#define KBIO_FLG_NONBLOCK   32
#define KBIO_FLG_DUMP       64

// This is just large enough to hold the only client.
// If new clients are added, it may have to be increased.
#define FUSION_BIO_CLIENT_RESERVED_SIZE      112

#define FIO_BIO_INVALID_SECTOR   0xFFFFFFFFFFFFFFFFULL

struct kfio_bio
{
    struct fio_device     *fbio_dev;
    struct fusion_ioctx   *fbio_ctx;
    kfio_sg_list_t        *fbio_sgl;
    kfio_dma_map_t        *fbio_dmap;
    fusion_list_entry_t    fbio_list;
    int32_t                fbio_error;
    uint32_t               fbio_flags;
    uint64_t               fbio_size;
    uint64_t               fbio_offset; // do not use if fbio_sector is valid
    uint64_t               fbio_sector; // use if not FIO_BIO_INVALID_SECTOR
    uint8_t                fbio_cmd;
    kfio_cpu_t             fbio_cpu;
    kfio_bio_completor_t   fbio_completor;
    fio_uintptr_t          fbio_parameter;
    uint64_t               fbio_start_time;
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
