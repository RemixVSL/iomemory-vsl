//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
/** @file include/fio/port/kbio.h 
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */
#ifndef __FIO_PORT_KBIO_H__
#define __FIO_PORT_KBIO_H__

struct fio_device;
struct fusion_request;

typedef struct kfio_bio kfio_bio_t;
typedef void (*kfio_bio_completor_t)(kfio_bio_t *bio, uint32_t bytes_complete, int error);

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
#define KBIO_FLG_DIRTY   8

struct kfio_bio
{
    struct fio_device     *fbio_dev;
    struct fusion_request *fbio_req;
    kfio_sg_list_t        *fbio_sgl;
    kfio_dma_map_t        *fbio_dmap;
    fusion_list_entry_t    fbio_list;
    uint32_t               fbio_flags;
    uint32_t               fbio_size;
    uint64_t               fbio_offset;
    uint8_t                fbio_cmd;
    kfio_bio_completor_t   fbio_completor;
    fio_uintptr_t          fbio_parameter;
};

#define kfio_bio_sync(bio)  (((bio)->fbio_flags & KBIO_FLG_SYNC) != 0)

extern kfio_bio_t *kfio_bio_alloc(struct fio_device *dev);
extern kfio_bio_t *kfio_bio_try_alloc(struct fio_device *dev);
extern void kfio_bio_free(kfio_bio_t *bio);

extern void kfio_bio_submit(kfio_bio_t *bio);
extern int  kfio_bio_wait(kfio_bio_t);

extern void kfio_bio_endio(kfio_bio_t *bio, unsigned int bytes_done, int error);

#endif /* __FIO_PORT_KBIO_H__ */
