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
#if !defined (__linux__)
#error "This file supports Linux only"
#endif

#include "port-internal.h"
#include <fio/port/dbgset.h>
#include <fio/port/kfio.h>
#include <fio/port/ktime.h>
#include <fio/port/kblock.h>
#include <fio/port/kscsi.h>
#include <fio/port/sched.h>
#include <fio/port/bitops.h>
#include <fio/port/common-linux/kblock.h>
#include <fio/port/atomic_list.h>
#include <linux/blk_types.h>
#include <linux/genhd.h>
#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <fio/port/cdev.h>
#include <linux/buffer_head.h>

extern int use_workqueue;
static int fio_major;

#define BI_SIZE(bio) (bio->bi_iter.bi_size)
#define BI_SECTOR(bio) (bio->bi_iter.bi_sector)
#define BI_IDX(bio) (bio->bi_iter.bi_idx)

/******************************************************************************
 *   Block request and bio processing methods.                                *
 ******************************************************************************/

struct kfio_disk
{
    struct fio_device    *dev;
    struct gendisk       *gd;
    struct request_queue *rq;
    kfio_pci_dev_t       *pdev;
    fusion_spinlock_t    *queue_lock;
    struct bio           *bio_head;
    struct bio           *bio_tail;
    fusion_bits_t         disk_state;
    fusion_condvar_t      state_cv;
    fusion_cv_lock_t      state_lk;
    uint32_t              maxiosize;
    struct fio_atomic_list  comp_list;
    unsigned int          pending;
    int                   last_dispatch_rw;
    sector_t              last_dispatch;
    atomic_t              lock_pending;
    uint32_t              sector_mask;
    int                   in_do_request;
    struct list_head      retry_list;
    unsigned int          retry_cnt;

    make_request_fn       *make_request_fn;
};

enum {
    KFIO_DISK_HOLDOFF_BIT   = 0,
    KFIO_DISK_COMPLETION    = 1,
};


int iodrive_barrier_sync = 0;

extern int enable_discard;

#define bio_flags(bio) ((bio)->bi_opf & REQ_OP_MASK)

extern int kfio_sgl_map_bio(kfio_sg_list_t *sgl, struct bio *bio);


static void kfio_blk_complete_request(struct request *req, int error);
static kfio_bio_t *kfio_request_to_bio(kfio_disk_t *disk, struct request *req,
                                       bool can_block);

static int kfio_bio_cnt(const struct bio * const bio)
{
    return atomic_read(&bio->__bi_cnt);
}

/******************************************************************************
 *   Functions required to register and unregister fio block device driver    *
 ******************************************************************************/


int kfio_register_blkdev_pre_init(kfio_pci_dev_t *pdev)
{
    /* This should return the major number from the block device file.  Under linux
     * the value set into the global fio_major is correct at this time, and will be
     * fine by the time we create the gendisk.
     */
    (void)pdev;
    return fio_major;
}

int kfio_init_storage_interface(void)
{
    fio_major = register_blkdev(0, "fio");
    return fio_major <= 0 ? -EBUSY : 0;
}


int kfio_teardown_storage_interface(void)
{
    unregister_blkdev(fio_major, "fio");
    return 0;
}

/******************************************************************************
 *   Block device open, close and ioctl handlers                              *
 ******************************************************************************/
static inline int fbio_need_dma_map(kfio_bio_t *fbio)
{
    if (!fbio->fbio_size)
    {
        return IODRIVE_DMA_DIR_INVALID;
    }
    else if (fbio->fbio_cmd == KBIO_CMD_READ)
    {
        return IODRIVE_DMA_DIR_READ;
    }
    else if (fbio->fbio_cmd == KBIO_CMD_WRITE)
    {
        return IODRIVE_DMA_DIR_WRITE;
    }
    else
    {
        return IODRIVE_DMA_DIR_INVALID;
    }
}

static kfio_bio_t *kfio_convert_bio(struct request_queue *queue, struct bio *bio);

static int kfio_open(struct block_device *bdev, fmode_t mode)
{
    struct fio_device *dev = bdev->bd_disk->private_data;

    return fio_open(dev);
}

static void kfio_release(struct gendisk *gd, fmode_t mode)
{
    struct fio_device *dev = gd->private_data;

    fio_release(dev);
}

static int kfio_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg)
{
    struct fio_device *dev = bdev->bd_disk->private_data;

    return fio_ioctl(dev, cmd, arg);
}

static int kfio_compat_ioctl(struct block_device *bdev, fmode_t mode, unsigned cmd, unsigned long arg)
{
    struct fio_device *dev = bdev->bd_disk->private_data;
    int rc;

    rc = fio_ioctl(dev, cmd, arg);

    if (rc == -ENOTTY)
    {
        return -ENOIOCTLCMD;
    }

    return rc;
}

static struct block_device_operations fio_bdev_ops =
{
    .owner =        THIS_MODULE,
    .open =         kfio_open,
    .release =      kfio_release,
    .ioctl =        kfio_ioctl,
    .compat_ioctl = kfio_compat_ioctl,
};


static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp, kfio_numa_node_t node);
static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio);
static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error);

static void kfio_invalidate_bdev(struct block_device *bdev);


kfio_bio_t *kfio_fetch_next_bio(struct kfio_disk *disk)
{
    struct request_queue *q;

    q = disk->rq;

    if (use_workqueue != USE_QUEUE_RQ)
    {
        struct bio *bio;
#define Q_LOCK &q->queue_lock
        spin_lock_irq(Q_LOCK);
        if ((bio = disk->bio_head) != NULL)
        {
            disk->bio_head = bio->bi_next;
            if (disk->bio_head == NULL)
                disk->bio_tail = NULL;
        }
        spin_unlock_irq(Q_LOCK);

        if (bio != NULL)
        {
            kfio_bio_t *fbio;

            fbio = kfio_convert_bio(q, bio);
            if (fbio == NULL)
            {
                __kfio_bio_complete(bio,  0, -EIO);
            }
            return fbio;
        }
    }
    return NULL;
}

static void kfio_bdput(struct block_device *bdev);

/* @brief Parameter to the work queue call. */
struct kfio_blk_add_disk_param
{
    struct fusion_work_struct work;
    struct kfio_disk *disk;
    bool              done;
};

/*
 * @brief Run add-disk in a worker thread context.
 */
static void kfio_blk_add_disk(fusion_work_struct_t *work)
{
    struct kfio_blk_add_disk_param *param = container_of(work, struct kfio_blk_add_disk_param, work);
    struct kfio_disk *disk = param->disk;

    add_disk(disk->gd);

    /* Tell waiter we are done. */
    fusion_cv_lock_irq(&disk->state_lk);
    param->done = true;
    fusion_condvar_broadcast(&disk->state_cv);
    fusion_cv_unlock_irq(&disk->state_lk);
}

int kfio_create_disk(struct fio_device *dev, kfio_pci_dev_t *pdev, uint32_t sector_size,
                     uint32_t max_sectors_per_request, uint32_t max_sg_elements_per_request,
                     fusion_spinlock_t *queue_lock, kfio_disk_t **diskp,
                     kfio_numa_node_t node)
{
    struct kfio_disk     *dp;
    struct request_queue *rq;

    /*
     * We are not prepared to expose device kernel block layer cannot handle.
     */
    if (sector_size < KERNEL_SECTOR_SIZE  || (sector_size & (sector_size - 1)) != 0)
    {
        return -EINVAL;
    }

    dp = kfio_malloc_node(sizeof(*dp), node);
    if (dp == NULL)
    {
        return -ENOMEM;
    }

    memset(dp, 0, sizeof(*dp));
    dp->dev  = dev;
    dp->pdev = pdev;
    dp->queue_lock = queue_lock;
    dp->last_dispatch_rw = -1;
    dp->sector_mask = sector_size - 1;
    dp->maxiosize = max_sectors_per_request * KERNEL_SECTOR_SIZE;

    fusion_condvar_init(&dp->state_cv, "fio_disk_cv");
    fusion_cv_lock_init(&dp->state_lk, "fio_disk_lk");

    fusion_atomic_list_init(&dp->comp_list);

    atomic_set(&dp->lock_pending, 0);
    INIT_LIST_HEAD(&dp->retry_list);

    if (use_workqueue != USE_QUEUE_RQ)
    {
        dp->rq = kfio_alloc_queue(dp, node);
    }

    if (dp->rq == NULL)
    {
        kfio_destroy_disk(dp, destroy_type_normal);
        return -ENOMEM;
    }

    rq = dp->rq;

    blk_limits_io_min(&rq->limits, sector_size);
    blk_limits_io_opt(&rq->limits, fio_dev_optimal_blk_size);
    blk_queue_max_hw_sectors(rq, max_sectors_per_request);
    blk_queue_max_segments(rq, max_sg_elements_per_request);
    blk_queue_max_segment_size(rq, PAGE_SIZE);
    blk_queue_logical_block_size(rq, sector_size);

    if (enable_discard)
    {
        blk_queue_flag_set(QUEUE_FLAG_DISCARD, rq);
        // XXXXXXX !!! WARNING - power of two sector sizes only !!! (always true in standard linux)
        blk_queue_max_discard_sectors(rq, (UINT_MAX & ~((unsigned int) sector_size - 1)) >> 9);
    }

    blk_queue_flag_set(QUEUE_FLAG_WC, rq);
    blk_queue_flag_set(QUEUE_FLAG_NONROT, rq);
    blk_queue_flag_clear(QUEUE_FLAG_ADD_RANDOM, rq);

    *diskp = dp;
    return 0;
}

int kfio_expose_disk(kfio_disk_t *dp, char *name, int major, int disk_index,
                     uint64_t reported_capacity, uint32_t sector_size,
                     uint32_t max_sg_elements_per_request)
{
    struct kfio_blk_add_disk_param param;
    struct gendisk *gd;

    dp->gd = gd = alloc_disk(FIO_NUM_MINORS);

    if (dp->gd == NULL)
    {
        kfio_destroy_disk(dp, destroy_type_normal);
        return -ENOMEM;
    }

    gd->major = major;
    gd->first_minor = FIO_NUM_MINORS * disk_index;
    gd->minors = FIO_NUM_MINORS;
    gd->fops = &fio_bdev_ops;
    gd->queue = dp->rq;
    gd->private_data = dp->dev;
    gd->flags = GENHD_FL_EXT_DEVT;

    fio_bdev_ops.owner = THIS_MODULE;

    strncpy(gd->disk_name, name, 32);

    set_capacity(gd, reported_capacity * sector_size / KERNEL_SECTOR_SIZE);

    infprint("%s: Creating block device %s: major: %d minor: %d sector size: %d...\n",
             fio_device_get_bus_name(dp->dev), gd->disk_name, gd->major,
             gd->first_minor, sector_size);

    /*
     * Offload device exposure to separate worker thread. On some kernels from
     * certain vendores add_disk is happy do do a lot of nested processing,
     * eating through limited kernel stack space.
     */
    fusion_init_work(&param.work, kfio_blk_add_disk);

    param.disk = dp;
    param.done = false;
    fusion_schedule_work(&param.work);

    /* Wait for work thread to expose our disk. */
    fusion_cv_lock_irq(&dp->state_lk);
    while (!param.done)
    {
        fusion_condvar_wait(&dp->state_cv, &dp->state_lk);
    }
    fusion_cv_unlock_irq(&dp->state_lk);

    fusion_destroy_work(&param.work);

    /*
     * The following is used because access to udev events are restricted
     * to GPL licensed symbols.  Since we have a propriatary licence, we can't
     * use the better interface.  Instead we must poll for the device creation.
     */
    fio_wait_for_dev(gd->disk_name);

    return 0;
}

static void kfio_kill_requests(struct request_queue *q)
{
}

void kfio_destroy_disk(kfio_disk_t *disk, destroy_type_t dt)
{
    if (disk->gd != NULL)
    {
        struct block_device *bdev;

        bdev = bdget_disk(disk->gd, 0);

        if (bdev != NULL)
        {
            kfio_invalidate_bdev(bdev);
            kfio_bdput(bdev);
        }

        set_capacity(disk->gd, 0);

        if (disk->queue_lock != NULL) {
            fusion_spin_lock_irqsave(disk->queue_lock);
        }

        /* Stop delivery of new io from user. */
        set_bit(QUEUE_FLAG_DEAD, &disk->rq->queue_flags);

        /*
         * Prevent request_fn callback from interfering with
         * the queue shutdown.
         */
        blk_mq_stop_hw_queues(disk->rq);
        /*
         * The queue is stopped and dead and no new user requests will be
         * coming to it anymore. Fetch remaining already queued requests
         * and fail them,
         */
        if (use_workqueue == USE_QUEUE_RQ)
        {
            kfio_kill_requests(disk->rq);
        }
        if (use_workqueue != USE_QUEUE_RQ)
        {
            /* Fail all bio's already on internal bio queue. */
            struct bio *bio;

            while ((bio = disk->bio_head) != NULL)
            {
                disk->bio_head = bio->bi_next;
                __kfio_bio_complete(bio,  0, -EIO);
            }
            disk->bio_tail = NULL;
        }

        if (disk->queue_lock != NULL) {
            fusion_spin_unlock_irqrestore(disk->queue_lock);
        }

        del_gendisk(disk->gd);

        put_disk(disk->gd);
        disk->gd = NULL;
    }

    if (disk->rq != NULL)
    {
        blk_cleanup_queue(disk->rq);
        disk->rq = NULL;
    }

    fusion_condvar_destroy(&disk->state_cv);
    fusion_cv_lock_destroy(&disk->state_lk);

    // XXX Temporary workaround to avoid crashing on shutdown with LVM (see CRT-10)
    if (dt == destroy_type_normal)
    {
        kfio_free(disk, sizeof(*disk));
    }
}

static void kfio_invalidate_bdev(struct block_device *bdev)
{
    invalidate_bdev(bdev);
}

static void kfio_bdput(struct block_device *bdev)
{
    bdput(bdev);
}

/**
 * @note param duration - in microseconds
 */
void kfio_disk_stat_write_update(kfio_disk_t *fgd, uint64_t totalsize, uint64_t duration)
{
    if (use_workqueue != USE_QUEUE_RQ)
    {
        struct gendisk *gd = fgd->gd;
        part_stat_lock();
        part_stat_inc(&gd->part0, ios[1]);
        part_stat_add(&gd->part0, sectors[1], totalsize >> 9);
	      part_stat_add(&gd->part0, nsecs[1],   duration * 1000);
        part_stat_unlock();
    }
}

void kfio_disk_stat_read_update(kfio_disk_t *fgd, uint64_t totalsize, uint64_t duration)
{
    if (use_workqueue != USE_QUEUE_RQ)
    {
        struct gendisk *gd = fgd->gd;
        part_stat_lock();
        part_stat_inc(&gd->part0, ios[0]);
        part_stat_add(&gd->part0, sectors[0], totalsize >> 9);
        part_stat_add(&gd->part0, nsecs[0],   duration * 1000);
        part_stat_unlock();
    }
}


int kfio_get_gd_in_flight(kfio_disk_t *fgd, int rw)
{
    struct gendisk *gd = fgd->gd;
    return part_stat_read(&gd->part0, ios[STAT_WRITE]);
}

void kfio_set_gd_in_flight(kfio_disk_t *fgd, int rw, int in_flight)
{
    struct gendisk *gd = fgd->gd;


    if (use_workqueue != USE_QUEUE_RQ)
    {
        part_stat_set_all(&gd->part0, in_flight);
    }
}

/**
 * @brief returns 1 if bio is O_SYNC priority
 */
static int kfio_bio_is_discard(struct bio *bio)
{
    return bio_op(bio) == REQ_OP_DISCARD;
}

/// @brief   Dump an OS bio to the log
/// @param   msg   prefix for message
/// @param   bio   the bio to drop
static void kfio_dump_bio(const char *msg, struct bio * bio)
{
    uint64_t sector;

    kassert(bio);

    // Use a local conversion to avoid printf format warnings on some platforms
    sector = (uint64_t)BI_SECTOR(bio);
    infprint("%s: sector: %llx: flags: %lx : op: %x : op_flags: %x : vcnt: %x", msg,
             sector, (unsigned long)bio->bi_flags, bio_op(bio), bio_flags(bio), bio->bi_vcnt);

    infprint("%s : idx: %x : phys_segments: %x : size: %x",
             msg, BI_IDX(bio), bio_segments(bio), BI_SIZE(bio));


    infprint("%s: max_vecs: %x : io_vec %p : end_io: %p : private: %p",
             msg, bio->bi_max_vecs, bio->bi_io_vec,
             bio->bi_end_io, bio->bi_private);

    infprint("%s: max_vecs: %x : cnt %x : io_vec %p : end_io: %p : private: %p",
             msg, bio->bi_max_vecs, kfio_bio_cnt(bio), bio->bi_io_vec,
             bio->bi_end_io, bio->bi_private);
    infprint("%s: integrity: %p", msg, bio_integrity(bio) );
}

static inline void kfio_set_comp_cpu(kfio_bio_t *fbio, struct bio *bio)
{
    fbio->fbio_cpu = kfio_current_cpu();
}

static unsigned long __kfio_bio_sync(struct bio *bio)
{
    return bio_flags(bio) == REQ_SYNC;
}

static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error)
{
    if (unlikely(error != 0))
    {
          bio->bi_status = errno_to_blk_status(error);
    }
    bio_endio(bio);
}

static void kfio_bio_completor(kfio_bio_t *fbio, uint64_t bytes_complete, int error)
{
    struct bio *bio = (struct bio *)fbio->fbio_parameter;
    uint64_t bytes_completed = 0;

    if (unlikely(fbio->fbio_flags & KBIO_FLG_DUMP))
    {
        kfio_dump_fbio(fbio);
    }

    if (fbio_need_dma_map(fbio) != IODRIVE_DMA_DIR_INVALID)
    {
        kfio_sgl_dma_unmap(fbio->fbio_sgl);
    }

    do
    {
        struct bio *next = bio->bi_next;

        bytes_completed += BI_SIZE(bio);

        if (unlikely(fbio->fbio_flags & KBIO_FLG_DUMP))
        {
            kfio_dump_bio(FIO_DRIVER_NAME, bio);
        }

        __kfio_bio_complete(bio, BI_SIZE(bio), error);
        bio = next;
    } while (bio);

    kassert(bytes_complete == bytes_completed);
}

/*
 * Attempt to add a Linux bio to an existing fbio. This is only allowed
 * when the bio is LBA contiguous with the fbio, and it's the same type.
 * We also require enough room in the SG list. A return value of 1 here
 * does not mean error, it just means that the existing fbio should be
 * submitted and a new one allocated for this bio.
 */
static int kfio_kbio_add_bio(kfio_bio_t *fbio, struct bio *bio, uint32_t maxiosize)
{
    int error;

    if (kfio_bio_is_discard(bio))
    {
        return 1;
    }

    /*
     * Need matching direction, and sequential offset
     */
    if ((bio_data_dir(bio) == WRITE && fbio->fbio_cmd != KBIO_CMD_WRITE) ||
        (bio_data_dir(bio) == READ && fbio->fbio_cmd != KBIO_CMD_READ))
    {
        return 1;
    }

    if (fbio->fbio_offset + fbio->fbio_size != BI_SECTOR(bio) * KERNEL_SECTOR_SIZE)
    {
        return 1;
    }

    if (fbio->fbio_size + BI_SIZE(bio) > maxiosize)
    {
        return 1;
    }

    error = kfio_sgl_map_bio(fbio->fbio_sgl, bio);
    if (error)
    {
        return 1;
    }

    fbio->fbio_size += BI_SIZE(bio);
    return 0;
}

static kfio_bio_t *kfio_map_to_fbio(struct request_queue *queue, struct bio *bio)
{
    struct kfio_disk *disk = queue->queuedata;
    kfio_bio_t       *fbio;
    int error;

    if ((((BI_SECTOR(bio) * KERNEL_SECTOR_SIZE) & disk->sector_mask) != 0) ||
        ((BI_SIZE(bio) & disk->sector_mask) != 0))
    {
        engprint("Rejecting malformed bio %p sector %lu size 0x%08x flags 0x%08lx op 0x%08x op_flags 0x%04x\n", bio,
                 (unsigned long)BI_SECTOR(bio), BI_SIZE(bio), (unsigned long)bio->bi_flags, bio_op(bio), bio_flags(bio));
        return NULL;
    }

    /*
     * This should use kfio_bio_try_alloc and should automatically
     * retry later when some requests become available.
     */
    fbio = kfio_bio_alloc(disk->dev);
    if (fbio == NULL)
    {
        return NULL;
    }

    // Convert Linux bio to Fusion-io bio and send it down to processing.
    fbio->fbio_flags  = 0;
    fbio->fbio_size   = BI_SIZE(bio);
    fbio->fbio_offset = BI_SECTOR(bio) * KERNEL_SECTOR_SIZE;

    fbio->fbio_completor = kfio_bio_completor;
    fbio->fbio_parameter = (fio_uintptr_t)bio;

    kfio_set_comp_cpu(fbio, bio);

    if (kfio_bio_is_discard(bio))
    {
        fbio->fbio_cmd = KBIO_CMD_DISCARD;
    }
    else
    {
        if (bio_data_dir(bio) == WRITE)
        {
            fbio->fbio_cmd = KBIO_CMD_WRITE;

            /* XXXKAN: Does sync make sense for reads? */
            if (__kfio_bio_sync(bio) != 0)
            {
                fbio->fbio_flags |= KBIO_FLG_SYNC;
            }
        }
        else
        {
            fbio->fbio_cmd = KBIO_CMD_READ;
        }

        error = kfio_sgl_map_bio(fbio->fbio_sgl, bio);
        if (error != 0)
        {
            /* This should not happen. */
            kfio_bio_free(fbio);
            return NULL;
        }
    }

    return fbio;
}

static kfio_bio_t *kfio_convert_bio(struct request_queue *queue, struct bio *bio)
{
    kfio_bio_t *fbio;
    int dma_dir, error;

    fbio = kfio_map_to_fbio(queue, bio);
    if (!fbio)
    {
        return NULL;
    }

    dma_dir = fbio_need_dma_map(fbio);
    if (dma_dir != IODRIVE_DMA_DIR_INVALID)
    {
        error = kfio_sgl_dma_map(fbio->fbio_sgl, fbio->fbio_dmap, dma_dir);
        if (error < 0)
        {
            kfio_bio_free(fbio);
            return NULL;
        }
    }

    return fbio;
}

/*
 * DMA map (if needed) an fbio and submit it
 */
static void kfio_fbio_map_submit(kfio_bio_t *fbio)
{
    int dma_dir, error;

    dma_dir = fbio_need_dma_map(fbio);
    if (dma_dir != IODRIVE_DMA_DIR_INVALID)
    {
        error = kfio_sgl_dma_map(fbio->fbio_sgl, fbio->fbio_dmap, dma_dir);
        if (error < 0)
        {
            kfio_bio_free(fbio);
            return;
        }
    }

    kfio_bio_submit(fbio);
}

static void kfio_kickoff_plugged_io(struct request_queue *q, struct bio *bio, uint32_t maxiosize)
{
    struct bio *tail, *next;
    kfio_bio_t *fbio;

    tail = NULL;
    fbio = NULL;
    do
    {
        next = bio->bi_next;
        bio->bi_next = NULL;

        if (fbio)
        {
            if (!kfio_kbio_add_bio(fbio, bio, maxiosize))
            {
                /*
                 * Keep track of the tail, until we submit
                 */
                if (tail)
                {
                    tail->bi_next = bio;
                }
                tail = bio;
                continue;
            }

            kfio_fbio_map_submit(fbio);
            fbio = NULL;
            tail = NULL;
        }

        fbio = kfio_map_to_fbio(q, bio);
        if (!fbio)
        {
            __kfio_bio_complete(bio,  0, -EIO);
            continue;
        }

        tail = bio;
    } while ((bio = next) != NULL);

    if (fbio)
    {
        kfio_fbio_map_submit(fbio);
    }
}

static int kfio_bio_should_submit_now(struct bio *bio)
{
    /*
     * Some kernels have a bug where it forgets to mark a discard
     * as sync (or appropriately unplug). Check this here and ensure
     * that we run it now instead of deferring. Check for a zero sized
     * requests too, as some "special" requests (such as a FLUSH) suffer
     * from the same bug.
     */
    if (!BI_SIZE(bio))
    {
        return 1;
    }
    return kfio_bio_is_discard(bio);
}

/* some 3.0 kernels call our unplug callback with
 * irqs off.  We use this variable to track it and
 * avoid plugging on those kernels.
 */
static int dangerous_plugging_callback = -1;

struct kfio_plug {
    struct blk_plug_cb cb;
    struct kfio_disk *disk;
    struct bio *bio_head;
    struct bio *bio_tail;
};

static void kfio_unplug_cb(struct blk_plug_cb *cb, bool from_schedule)
{
    struct kfio_plug *plug = container_of(cb, struct kfio_plug, cb);
    struct kfio_disk *disk = plug->disk;
    struct bio *bio;

    bio = plug->bio_head;
    kfree(plug);

    if (bio)
    {
        kfio_kickoff_plugged_io(disk->rq, bio, disk->maxiosize);
    }
}

/*
 * this is used once while we create the block device,
 * just to make sure unplug callbacks aren't run with irqs off
 */
struct test_plug
{
    struct blk_plug_cb cb;
    int safe;
};

static void safe_unplug_cb(struct blk_plug_cb *cb, bool from_schedule)
{
    struct test_plug *test_plug = container_of(cb, struct test_plug, cb);

    if (irqs_disabled())
        test_plug->safe = 0;
    else
        test_plug->safe = 1;
}

/*
 * trigger an unplug callback without any IO.  This tests
 * if irqs are on when the callback runs, it is safe
 * to use our plugging code.  Otherwise we have to turn it
 * off.  Some 3.0 based kernels have irqs off during the
 * unplug callback.
 */
static void test_safe_plugging(void)
{
    struct blk_plug plug;
    struct test_plug test_plug;

    /* we only need to do this probe once */
    if (dangerous_plugging_callback != -1)
        return;

    /* setup a plug.  We don't need to worry about
     * the block device being ready because we won't do any IO
     */
    blk_start_plug(&plug);

    test_plug.safe = -1;

    /* add ourselves to the list of callbacks */
    test_plug.cb.callback = safe_unplug_cb;
    list_add_tail(&test_plug.cb.list, &plug.cb_list);

    /*
     * force a schedule.  Later kernels won't run the callbacks
     * if our state is TASK_RUNNING at the time of the schedule
     */
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout(1);
    set_current_state(TASK_RUNNING);

    /* remove our plug */
    blk_finish_plug(&plug);

    /* if we failed the safety test, set our global to disable plugging */
    if (test_plug.safe == 0) {
        infprint("Unplug callbacks are run with irqs off.  Plugging disabled\n");
        dangerous_plugging_callback = 1;
    } else {
        dangerous_plugging_callback = 0;
    }
}

static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp,
                                              kfio_numa_node_t node)
{
    struct request_queue *rq;

    test_safe_plugging();

    rq = blk_alloc_queue_node(GFP_NOIO, node);
    if (rq != NULL)
    {
        rq->queuedata = dp;
        blk_queue_make_request(rq, kfio_make_request);

        // TODO:
        // rq->queue_lock = (spinlock_t *)dp->queue_lock;
        memcpy(&dp->queue_lock, &rq->queue_lock, sizeof(dp->queue_lock));
    }
    return rq;
}

static int should_holdoff_writes(struct kfio_disk *disk)
{
    return fio_test_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}

void kfio_set_write_holdoff(struct kfio_disk *disk)
{
    fio_set_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}

void kfio_clear_write_holdoff(struct kfio_disk *disk)
{
    fusion_cv_lock_irq(&disk->state_lk);
    fio_clear_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
    fusion_condvar_broadcast(&disk->state_cv);
    fusion_cv_unlock_irq(&disk->state_lk);
}

/*
 * Increment the exclusive lock pending count
 */
void kfio_mark_lock_pending(kfio_disk_t *fgd)
{
}

/*
 * Decrement the exclusive lock pending count
 */
void kfio_unmark_lock_pending(kfio_disk_t *fgd)
{
}


static int holdoff_writes_under_pressure(struct kfio_disk *disk)
{
    int count = 0;

    while (should_holdoff_writes(disk))
    {
        int reason = FUSION_WAIT_TRIGGERED;
        fusion_cv_lock_irq(&disk->state_lk);
        // wait until unstalled. Hard-coded 1 minute timeout.
        while (should_holdoff_writes(disk))
        {
            reason = fusion_condvar_timedwait(&disk->state_cv,
                                              &disk->state_lk,
                                              5 * 1000 * 1000);
            if (reason == FUSION_WAIT_TIMEDOUT)
            {
                break;
            }
        }

        fusion_cv_unlock_irq(&disk->state_lk);

        if (reason == FUSION_WAIT_TIMEDOUT)
        {
            count++;
            engprint("%s %d: seconds %d %s\n", __FUNCTION__, __LINE__, count * 5,
                     should_holdoff_writes(disk) ? "stalled" : "unstalled");

            if (count >= 12)
            {
                return !should_holdoff_writes(disk);
            }
        }
    }

    return 1;
}

static void *kfio_should_plug(struct request_queue *q)
{
    struct kfio_disk *disk = q->queuedata;
    struct kfio_plug *kplug;
    struct blk_plug *plug;

    if (use_workqueue != USE_QUEUE_NONE)
    {
        return NULL;
    }

    /* this might be -1, 0, or 1 */
    if (dangerous_plugging_callback == 1)
        return NULL;

    plug = current->plug;
    if (!plug)
    {
        return NULL;
    }

    list_for_each_entry(kplug, &plug->cb_list, cb.list)
    {
        if (kplug->cb.callback == kfio_unplug_cb && kplug->disk == disk)
        {
            return kplug;
        }
    }

    kplug = kmalloc(sizeof(*kplug), GFP_ATOMIC);
    if (!kplug)
    {
        return NULL;
    }

    kplug->disk = disk;
    kplug->cb.callback = kfio_unplug_cb;
    kplug->bio_head = kplug->bio_tail = NULL;
    list_add_tail(&kplug->cb.list, &plug->cb_list);
    return kplug;
}
static struct bio *kfio_add_bio_to_plugged_list(void *data, struct bio *bio)
{
    struct kfio_plug *plug = data;
    struct bio *ret = NULL;

    if (plug->bio_tail)
    {
        plug->bio_tail->bi_next = bio;
    }
    else
    {
        plug->bio_head = bio;
    }
    plug->bio_tail = bio;

    if (kfio_bio_should_submit_now(bio) && use_workqueue == USE_QUEUE_NONE)
    {
        ret = plug->bio_head;
        plug->bio_head = plug->bio_tail = NULL;
    }

    return ret;
}

static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET 0
{
    struct kfio_disk *disk = queue->queuedata;
    void *plug_data;

    if (bio_data_dir(bio) == WRITE && !holdoff_writes_under_pressure(disk))
    {
        kassert_once(!"timed out waiting for queue to unstall.");
        __kfio_bio_complete(bio, 0, -EIO);
        return FIO_MFN_RET;
    }


    if (bio_segments(bio) >= queue_max_segments(queue))
        blk_queue_split(queue, &bio);


    // iomemory-vsl4 has atom bio here

    plug_data = kfio_should_plug(queue);
    if (!plug_data)
    {
        /*
         * No posting/deferral, kick off now
         */
        kfio_bio_t *fbio;

        fbio = kfio_convert_bio(queue, bio);
        if (fbio)
        {
            kfio_bio_submit(fbio);
        }
        else
        {
            __kfio_bio_complete(bio,  0, -EIO);
        }
    }
    else
    {
        struct bio *ret;

        /*
         * Queue up
         */
        ret = kfio_add_bio_to_plugged_list(plug_data, bio);

        if (use_workqueue != USE_QUEUE_NONE)
        {
            fio_bio_posted(disk->dev);
        }
        else if (ret)
        {
            /*
             * If kfio_add_bio_to_plugged_list() returned us work to do,
             * kick that off now.
             */
            kfio_kickoff_plugged_io(queue, ret, disk->maxiosize);
        }
    }

    return FIO_MFN_RET;
}

/******************************************************************************
 *   Kernel Atomic Write API
 *
 ******************************************************************************/
/// @brief extracts fio_device pointer and passes control to kfio_handle_atomic
///
int kfio_vectored_atomic(struct block_device *bdev,
                         const struct kfio_iovec *iov,
                         uint32_t iovcnt,
                         bool user_pages)
{

    uint32_t bytes_written;
    struct fio_device *dev;
    int retval;

    if (!bdev || !iov)
    {
        return -EFAULT;
    }

    dev = bdev->bd_disk->private_data;
    if (!dev)
    {
        return -ENODEV;
    }
    retval = kfio_handle_atomic(dev, iov, iovcnt, &bytes_written, user_pages ? FIO_ATOMIC_WRITE_USER_PAGES : 0);
    return retval == 0 ? (int)bytes_written : retval;
}

int kfio_count_sectors_inuse(struct block_device *bdev,
                           uint64_t base,
                           uint64_t length,
                           uint64_t *count)
{
    struct fio_device *dev;

    if (!bdev)
    {
        return -EFAULT;
    }
    dev = bdev->bd_disk->private_data;
    if (!dev)
    {
        return -ENODEV;
    }

    return fio_count_sectors_inuse(dev, base, length, count);
}
