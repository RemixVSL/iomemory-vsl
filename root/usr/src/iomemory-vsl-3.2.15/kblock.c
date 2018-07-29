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

#if defined(__VMKLNX__)
// ESX block layer code is buggy.
// Map all block layer calls to our own block layer module (modified from the DDK version).
#define bdget fio_esx_bdget
#define bdput fio_esx_bdput
#define add_disk fio_esx_add_disk
#define vmklnx_block_register_sglimit fio_esx_vmklnx_block_register_sglimit
#define vmklnx_register_blkdev fio_esx_vmklnx_register_blkdev
#define unregister_blkdev fio_esx_unregister_blkdev
#define put_disk fio_esx_put_disk
#define vmklnx_block_init_done fio_esx_vmklnx_block_init_done
#define blk_complete_request fio_esx_blk_complete_request
#define end_that_request_last fio_esx_end_that_request_last
#define blk_queue_max_sectors fio_esx_blk_queue_max_sectors
#define blk_cleanup_queue fio_esx_blk_cleanup_queue
#define blk_queue_max_hw_segments fio_esx_blk_queue_max_hw_segments
#define blk_queue_softirq_done fio_esx_blk_queue_softirq_done
#define blk_queue_hardsect_size fio_esx_blk_queue_hardsect_size
#define blk_queue_max_phys_segments fio_esx_blk_queue_max_phys_segments
#define alloc_disk fio_esx_alloc_disk
#define blk_init_queue fio_esx_blk_init_queue
#define elv_next_request fio_esx_elv_next_request
#define blk_stop_queue fio_esx_blk_stop_queue
#define bio_endio fio_esx_bio_endio
#endif

#include "port-internal.h"
#include <fio/port/dbgset.h>
#include <fio/port/kfio.h>
#include <fio/port/bitops.h>
#include <fio/port/common-linux/kblock.h>
#include <fio/port/atomic_list.h>

#include <linux/version.h>
#include <linux/fs.h>
#if !defined(__VMKLNX__)
#include <linux/buffer_head.h>
#endif

extern int use_workqueue;
#if !defined(__VMKLNX__)
static int fio_major;
#endif

#if KFIOC_HAS_BIOVEC_ITERATORS
#define BI_SIZE(bio) (bio->bi_iter.bi_size)
#define BI_SECTOR(bio) (bio->bi_iter.bi_sector)
#define BI_IDX(bio) (bio->bi_iter.bi_idx)
#else
#define BI_SIZE(bio) (bio->bi_size)
#define BI_SECTOR(bio) (bio->bi_sector)
#define BI_IDX(bio) (bio->bi_idx)
#endif

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

/*
 * RHEL6.1 will trigger both old and new scheme due to their backport,
 * whereas new kernels will trigger only the new scheme. So for the RHEL6.1
 * case, just disable the old scheme to avoid to much ifdef hackery.
 */
#if KFIOC_NEW_BARRIER_SCHEME == 1
#undef KFIOC_BARRIER
#define KFIOC_BARRIER   0
#endif

#if KFIOC_REQ_HAS_ERRORS
#define KFIOC_ERROR req->errors
#elif KFIOC_REQ_HAS_ERROR_COUNT
#define KFIOC_ERROR req->error_count
#elif KFIOC_BIO_HAS_ERROR
#define KFIOC_ERROR bio->errors
#endif

/*
 * Enable tag flush barriers by default, and default to safer mode of
 * operation on cards that don't have powercut support. Barrier mode can
 * also be QUEUE_ORDERED_TAG, or QUEUE_ORDERED_NONE for no barrier support.
 */
#if KFIOC_BARRIER == 1
static int iodrive_barrier_type = QUEUE_ORDERED_TAG_FLUSH;
int iodrive_barrier_sync = 1;
#else
int iodrive_barrier_sync = 0;
#endif

#if KFIOC_DISCARD == 1
extern int enable_discard;
#endif

#if KFIOC_HAS_SEPARATE_OP_FLAGS
#if !defined(bio_flags)
// bi_opf defined in kernel 4.8, but bio_flags not defined till 4.9
#define bio_flags(bio)  (bio)->bi_opf
#endif
#endif

#if defined(__VMKLNX__) || KFIOC_HAS_RQ_POS_BYTES == 0
#define blk_rq_pos(rq)    ((rq)->sector)
#define blk_rq_bytes(rq)  ((rq)->nr_sectors << 9)
#endif

extern int kfio_sgl_map_bio(kfio_sg_list_t *sgl, struct bio *bio);


static void kfio_blk_complete_request(struct request *req);
static kfio_bio_t *kfio_request_to_bio(kfio_disk_t *disk, struct request *req,
                                       bool can_block);

static int kfio_bio_cnt(const struct bio * const bio)
{
    return
#if KFIOC_BIO_HAS_USCORE_BI_CNT
    atomic_read(&bio->__bi_cnt);
#else
    atomic_read(&bio->bi_cnt);
#endif
}

/******************************************************************************
 *   Functions required to register and unregister fio block device driver    *
 ******************************************************************************/


// These are empty functions for ESX because ESX registers/unregisters its block device
// during block device creation/destruction.
int kfio_register_blkdev_pre_init(kfio_pci_dev_t *pdev)
{
    /* This should return the major number from the block device file.  Under linux
     * the value set into the global fio_major is correct at this time, and will be
     * fine by the time we create the gendisk.
     */
#if defined(__VMKLNX__)
    return 0;
#else
    (void)pdev;
    return fio_major;
#endif
}

int kfio_init_storage_interface(void)
{
#if defined(__VMKLNX__)
    return 0;
#else
    fio_major = register_blkdev(0, "fio");
    return fio_major <= 0 ? -EBUSY : 0;
#endif
}


int kfio_teardown_storage_interface(void)
{
#if defined(__VMKLNX__)
    return 0;
#else
    int rc = 0;

#if KFIOC_UNREGISTER_BLKDEV_RETURNS_VOID
    unregister_blkdev(fio_major, "fio");
#else
    rc = unregister_blkdev(fio_major, "fio");
#endif

    return rc;
#endif
}

#if defined(__VMKLNX__)
// "Real" ESX register functions
int kfio_register_esx_blkdev(kfio_pci_dev_t *pdev)
{
    return vmklnx_register_blkdev(0, "fio",
                                  kfio_pci_get_bus(pdev),
                                  kfio_pci_get_function(pdev),
                                  kfio_pci_get_drvdata(pdev));
}

int kfio_unregister_esx_blkdev(unsigned int major, const char *name)
{
    return unregister_blkdev(major, name);
}
#endif

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

#if KFIOC_HAS_NEW_BLOCK_METHODS

static int kfio_open(struct block_device *bdev, fmode_t mode)
{
    struct fio_device *dev = bdev->bd_disk->private_data;

    return fio_open(dev);
}

#if KFIOC_BLOCK_DEVICE_RELEASE_RETURNS_INT
static int kfio_release(struct gendisk *gd, fmode_t mode)
{
    struct fio_device *dev = gd->private_data;

    return fio_release(dev);
}
#else
static void kfio_release(struct gendisk *gd, fmode_t mode)
{
    struct fio_device *dev = gd->private_data;

    fio_release(dev);
}
#endif

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

#else

static void *kfio_inode_bd_disk_private_data(struct inode *ip)
{
    return ip->i_bdev->bd_disk->private_data;
}

static int kfio_open(struct inode *inode, struct file *filp)
{
    struct fio_device *dev = kfio_inode_bd_disk_private_data(inode);

    filp->private_data = dev;

    return fio_open(dev);
}

static int kfio_release(struct inode *inode, struct file *filp)
{
    struct fio_device *dev = kfio_inode_bd_disk_private_data(inode);

    return fio_release(dev);
}

static int kfio_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct fio_device *dev = kfio_inode_bd_disk_private_data(inode);

    return fio_ioctl(dev, cmd, arg);
}

#if KFIOC_HAS_COMPAT_IOCTL_METHOD
#if KFIOC_COMPAT_IOCTL_RETURNS_LONG
static long kfio_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int kfio_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    struct fio_device *dev;
    int rc;

    if (NULL == filp)
    {
        return -EBADF;
    }
    dev = filp->private_data;

    if (NULL == dev)
    {
        return -EINVAL;
    }

    rc = fio_ioctl(dev, cmd, arg);

    if (rc == -ENOTTY)
    {
        return -ENOIOCTLCMD;
    }

    return rc;
 }
#endif
#endif

static struct block_device_operations fio_bdev_ops =
{
    .owner =        THIS_MODULE,
    .open =         kfio_open,
    .release =      kfio_release,
    .ioctl =        kfio_ioctl,
#if KFIOC_HAS_COMPAT_IOCTL_METHOD
    .compat_ioctl = kfio_compat_ioctl,
#endif
};



#if !defined(__VMKLNX__)
static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp, kfio_numa_node_t node);
#if KFIOC_MAKE_REQUEST_FN_VOID
static void kfio_make_request(struct request_queue *queue, struct bio *bio);
#elif KFIOC_MAKE_REQUEST_FN_UINT
static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio);
#else
static int kfio_make_request(struct request_queue *queue, struct bio *bio);
#endif
static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error);
#endif

#if KFIOC_USE_IO_SCHED
static struct request_queue *kfio_init_queue(struct kfio_disk *dp, kfio_numa_node_t node);
static void kfio_do_request(struct request_queue *queue);
static struct request *kfio_blk_fetch_request(struct request_queue *q);
static void kfio_restart_queue(struct request_queue *q);
static void kfio_end_request(struct request *req, int uptodate);
#else
static void kfio_restart_queue(struct request_queue *q)
{
}
#endif

#if KFIOC_BARRIER == 1
static void kfio_prepare_flush(struct request_queue *q, struct request *rq);
#endif

static void kfio_invalidate_bdev(struct block_device *bdev);


kfio_bio_t *kfio_fetch_next_bio(struct kfio_disk *disk)
{
    struct request_queue *q;

    q = disk->rq;

#if !defined(__VMKLNX__)
    if (use_workqueue != USE_QUEUE_RQ)
    {
        struct bio *bio;

        spin_lock_irq(q->queue_lock);
        if ((bio = disk->bio_head) != NULL)
        {
            disk->bio_head = bio->bi_next;
            if (disk->bio_head == NULL)
                disk->bio_tail = NULL;
        }
        spin_unlock_irq(q->queue_lock);

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
#endif

# if KFIOC_USE_IO_SCHED
    if (use_workqueue == USE_QUEUE_RQ)
    {
        struct request *creq;

        spin_lock_irq(q->queue_lock);
        creq = kfio_blk_fetch_request(q);
        spin_unlock_irq(q->queue_lock);

        if (creq != NULL)
        {
            kfio_bio_t *fbio;

            fbio = kfio_request_to_bio(disk, creq, true);
            if (fbio == NULL)
            {
                fbio->fbio_error = -EIO;
                kfio_blk_complete_request(creq);
            }
            return fbio;
        }
    }
#endif
    return NULL;
}

static void kfio_bdput(struct block_device *bdev);

#if !defined(__VMKLNX__)

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
#endif /* defined(__VMKLNX__) */


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
#if !defined(__VMKLNX__)
        dp->rq = kfio_alloc_queue(dp, node);
#endif
    }
# if KFIOC_USE_IO_SCHED
    else
    {
        dp->rq = kfio_init_queue(dp, node);
    }
# endif /* KFIOC_USE_IO_SCHED */

    if (dp->rq == NULL)
    {
        kfio_destroy_disk(dp, destroy_type_normal);
        return -ENOMEM;
    }

    rq = dp->rq;

#if KFIOC_HAS_BLK_LIMITS_IO_MIN
    blk_limits_io_min(&rq->limits, sector_size);
#endif
#if KFIOC_HAS_BLK_LIMITS_IO_OPT
    blk_limits_io_opt(&rq->limits, fio_dev_optimal_blk_size);
#endif

#if KFIOC_HAS_BLK_QUEUE_MAX_SEGMENTS

    blk_queue_max_hw_sectors(rq, max_sectors_per_request);
    blk_queue_max_segments(rq, max_sg_elements_per_request);
#else
    blk_queue_max_sectors(rq, max_sectors_per_request);
    blk_queue_max_phys_segments(rq, max_sg_elements_per_request);
    blk_queue_max_hw_segments  (rq, max_sg_elements_per_request);
#endif

#if KFIOC_HAS_QUEUE_FLAG_CLUSTER
# if KFIOC_HAS_QUEUE_FLAG_CLEAR_UNLOCKED
    queue_flag_clear_unlocked(QUEUE_FLAG_CLUSTER, rq);
# else
    rq->queue_flags &= ~(1 << QUEUE_FLAG_CLUSTER);
# endif
#elif KFIOC_HAS_QUEUE_LIMITS_CLUSTER
    rq->limits.cluster = 0;
#else
# ifndef __VMKLNX__
#  error "Do not know how to disable request queue clustering for this kernel."
# endif
#endif

    blk_queue_max_segment_size(rq, PAGE_SIZE);

#if KFIOC_HAS_BLK_QUEUE_HARDSECT_SIZE
    blk_queue_hardsect_size(rq, sector_size);
#else
    blk_queue_logical_block_size(rq, sector_size);
#endif
#if KFIOC_DISCARD == 1
    if (enable_discard)
    {

#if KFIOC_DISCARD_ZEROES_IN_LIMITS == 1
        if (fio_device_ptrim_available(dev))
        {
            rq->limits.discard_zeroes_data = 1;
        }
#endif  /* KFIOC_DISCARD_ZEROES_IN_LIMITS */

        queue_flag_set_unlocked(QUEUE_FLAG_DISCARD, rq);
        // XXXXXXX !!! WARNING - power of two sector sizes only !!! (always true in standard linux)
        blk_queue_max_discard_sectors(rq, (UINT_MAX & ~((unsigned int) sector_size - 1)) >> 9);
#if KFIOC_DISCARD_GRANULARITY_IN_LIMITS
        rq->limits.discard_granularity = sector_size;
#endif
    }
#else
    if (enable_discard)
    {
        infprint("enable_discard set but discard not supported on this linux version\n");
    }
#endif

#if KFIOC_NEW_BARRIER_SCHEME == 1
    /*
     * Do this manually, as blk_queue_flush() is a GPL only export.
     *
     * We set REQ_FUA and REQ_FLUSH to ensure ordering (barriers) and to flush (on non-powercut cards).
     * Note on REQ_FUA: "strict_sync=1" needs to be set or this is ignored on cards w/o powercut support.
     */
    rq->flush_flags = REQ_FUA | REQ_FLUSH;
/* as KFIOC_NEW_BARRIER_SCHEME exits with 2 on newer kernels... */
#elif KFIOC_NEWER_BARRIER_SCHEME == 1
    rq->queue_flags = REQ_FUA | REQ_PREFLUSH; 
#elif KFIOC_BARRIER_USES_QUEUE_FLAGS
    queue_flag_set(QUEUE_FLAG_WC, rq);
#elif KFIOC_BARRIER == 1
    // Ignore if ordered mode is wrong - linux will complain
    blk_queue_ordered(rq, iodrive_barrier_type, kfio_prepare_flush);
#else
#error No barrier scheme supported
#endif

#if KFIOC_QUEUE_HAS_NONROT_FLAG
    /* Tell the kernel we are a non-rotational storage device */
    queue_flag_set_unlocked(QUEUE_FLAG_NONROT, rq);
#endif
#if KFIOC_QUEUE_HAS_RANDOM_FLAG
    /* Disable device global entropy contribution */
    queue_flag_clear_unlocked(QUEUE_FLAG_ADD_RANDOM, rq);
#endif

    *diskp = dp;
    return 0;
}

int kfio_expose_disk(kfio_disk_t *dp, char *name, int major, int disk_index,
                     uint64_t reported_capacity, uint32_t sector_size,
                     uint32_t max_sg_elements_per_request)
{
#if !defined(__VMKLNX__)
    struct kfio_blk_add_disk_param param;
#endif
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
#if defined GENHD_FL_EXT_DEVT
    gd->flags = GENHD_FL_EXT_DEVT;
#endif
#if defined(__VMKLNX__)
    gd->maxXfer = 1024 * 1024; // 1M - matches BLK_DEF_MAX_SECTORS
#endif

    fio_bdev_ops.owner = fusion_get_this_module();

    strncpy(gd->disk_name, name, 32);

    set_capacity(gd, reported_capacity * sector_size / KERNEL_SECTOR_SIZE);

    infprint("%s: Creating block device %s: major: %d minor: %d sector size: %d...\n",
             fio_device_get_bus_name(dp->dev), gd->disk_name, gd->major,
             gd->first_minor, sector_size);

#if !defined(__VMKLNX__)
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
#else // __VMKLNX__
    add_disk(gd);

    vmklnx_block_register_sglimit(major, max_sg_elements_per_request);
    vmklnx_block_init_done(major);
#endif

    return 0;
}

static void kfio_kill_requests(struct request_queue *q)
{
#if KFIOC_USE_IO_SCHED
    struct kfio_disk *disk = q->queuedata;
    struct request *req;

    while ((req = kfio_blk_fetch_request(disk->rq)) != NULL)
    {
        kfio_end_request(req, -EIO);
    }

    while (!list_empty(&disk->retry_list))
    {
        req = list_entry(disk->retry_list.next, struct request, queuelist);
        list_del_init(&req->queuelist);
        kfio_end_request(req, -EIO);
    }
#endif
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

        fusion_spin_lock_irqsave(disk->queue_lock);

        /* Stop delivery of new io from user. */
        set_bit(QUEUE_FLAG_DEAD, &disk->rq->queue_flags);

        /*
         * Prevent request_fn callback from interfering with
         * the queue shutdown.
         */
        blk_stop_queue(disk->rq);

        /* Fail all bio's already on internal bio queue. */
#if !defined(__VMKLNX__)
        if (use_workqueue != USE_QUEUE_RQ)
        {
            struct bio *bio;

            while ((bio = disk->bio_head) != NULL)
            {
                disk->bio_head = bio->bi_next;
                __kfio_bio_complete(bio,  0, -EIO);
            }
            disk->bio_tail = NULL;
        }
#endif

        /*
         * The queue is stopped and dead and no new user requests will be
         * coming to it anymore. Fetch remaining already queued requests
         * and fail them,
         */
        if (use_workqueue == USE_QUEUE_RQ)
        {
            kfio_kill_requests(disk->rq);
        }

        fusion_spin_unlock_irqrestore(disk->queue_lock);

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
        errprint("sometimes crash on later kernels");
        kfio_free(disk, sizeof(*disk));
    }
}

static void kfio_invalidate_bdev(struct block_device *bdev)
{
#if !defined(__VMKLNX__)
    struct address_space *mapping = bdev->bd_inode->i_mapping;
    errprint("pages: %i", mapping->nrpages == 0);
    invalidate_mapping_pages(mapping, 0, -1);
#if ! KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS
    invalidate_bdev(bdev, 0);
#else
    invalidate_bdev(bdev);
#endif /* KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS */
#else
    errprint("API invalidate_bdev missing.");
    /* XXXesx missing API */
#endif
    errprint("pages after invaldiation: %i", mapping->nrpages == 0);
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
#if !defined(__VMKLNX__)
    if (use_workqueue != USE_QUEUE_RQ)
    {
#if !(KFIOC_PARTITION_STATS && (KFIOC_CONFIG_PREEMPT_RT || KFIOC_CONFIG_TREE_PREEMPT_RCU))
        struct gendisk *gd = fgd->gd;
#endif
# if KFIOC_PARTITION_STATS
# if !KFIOC_CONFIG_PREEMPT_RT && !KFIOC_CONFIG_TREE_PREEMPT_RCU
        int cpu;

       /*
        * part_stat_lock() with CONFIG_PREEMPT_RT can't be used! It ends up calling
        * rcu_read_update which is GPL in the RT patch set.
        */
        cpu = part_stat_lock();
        part_stat_inc(cpu, &gd->part0, ios[1]);
        part_stat_add(cpu, &gd->part0, sectors[1], totalsize >> 9);
        part_stat_add(cpu, &gd->part0, ticks[1],   kfio_div64_64(duration * HZ, 1000000));
        part_stat_unlock();
# endif /* defined(KFIOC_CONFIG_PREEMPT_RT) */
# else /* KFIOC_PARTITION_STATS */

#  if KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS
        disk_stat_inc(gd, ios[1]);
        disk_stat_add(gd, sectors[1], totalsize >> 9);
        disk_stat_add(gd, ticks[1], fusion_usectohz(duration));
#  else /* KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS */
        disk_stat_inc(gd, writes);
        disk_stat_add(gd, write_sectors, totalsize >> 9);
        disk_stat_add(gd, write_ticks, fusion_usectohz(duration));
#  endif /* else ! KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS */
        // TODO: #warning Need disk_round_stats() implementation to replace GPL version.
        // disk_round_stats(gd);
        disk_stat_add(gd, time_in_queue,
            kfio_get_gd_in_flight(fgd, BIO_DIR_WRITE));
# endif /* else ! KFIOC_PARTITION_STATS */
    }
#endif /* !defined(__VMKLNX__) */
}

void kfio_disk_stat_read_update(kfio_disk_t *fgd, uint64_t totalsize, uint64_t duration)
{
#if !defined(__VMKLNX__)
    if (use_workqueue != USE_QUEUE_RQ)
    {
#if !(KFIOC_PARTITION_STATS && (KFIOC_CONFIG_PREEMPT_RT || KFIOC_CONFIG_TREE_PREEMPT_RCU))
        struct gendisk *gd = fgd->gd;
#endif
# if KFIOC_PARTITION_STATS
# if !KFIOC_CONFIG_PREEMPT_RT && !KFIOC_CONFIG_TREE_PREEMPT_RCU
        int cpu;

    /* part_stat_lock() with CONFIG_PREEMPT_RT can't be used!
       It ends up calling rcu_read_update which is GPL in the RT patch set */
        cpu = part_stat_lock();
        part_stat_inc(cpu, &gd->part0, ios[0]);
        part_stat_add(cpu, &gd->part0, sectors[0], totalsize >> 9);
        part_stat_add(cpu, &gd->part0, ticks[0],   kfio_div64_64(duration * HZ, 1000000));
        part_stat_unlock();
# endif /* KFIO_CONFIG_PREEMPT_RT */
# else /* KFIO_PARTITION_STATS */
#  if KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS
        disk_stat_inc(gd, ios[0]);
        disk_stat_add(gd, sectors[0], totalsize >> 9);
        disk_stat_add(gd, ticks[0], fusion_usectohz(duration));
#  else /* KFIO_C_HAS_DISK_STATS_READ_WRITE_ARRAYS */
        disk_stat_inc(gd, reads);
        disk_stat_add(gd, read_sectors, totalsize >> 9);
        disk_stat_add(gd, read_ticks, fusion_usectohz(duration));
#  endif /* else ! KFIO_C_HAS_DISK_STATS_READ_WRITE_ARRAYS */

        // TODO: #warning Need disk_round_stats() implimentation to replace GPL version.
        // disk_round_stats(gd);
        disk_stat_add(gd, time_in_queue,
            kfio_get_gd_in_flight(fgd, BIO_DIR_READ));
# endif /* else ! KFIO_PARTITION_STATS */
    }
#endif /* !defined(__VMKLNX__) */
}


int kfio_get_gd_in_flight(kfio_disk_t *fgd, int rw)
{
    struct gendisk *gd = fgd->gd;
#if KFIOC_PARTITION_STATS
#if KFIOC_HAS_INFLIGHT_RW || KFIOC_HAS_INFLIGHT_RW_ATOMIC 
    int dir = 0;

    // In the Linux kernel the direction isn't explicitly defined, however
    // in linux/bio.h, you'll notice that its referenced as 1 for write and 0
    // for read.

    if (rw == BIO_DIR_WRITE)
        dir = 1;

#if KFIOC_HAS_INFLIGHT_RW_ATOMIC
    return atomic_read(&gd->part0.in_flight[dir]);
#else
    return gd->part0.in_flight[dir];
#endif
#else
    return gd->part0.in_flight;
#endif
# else
    return gd->in_flight;
# endif
}

void kfio_set_gd_in_flight(kfio_disk_t *fgd, int rw, int in_flight)
{
    struct gendisk *gd = fgd->gd;


    if (use_workqueue != USE_QUEUE_RQ)
    {
#if KFIOC_PARTITION_STATS
#if KFIOC_HAS_INFLIGHT_RW || KFIOC_HAS_INFLIGHT_RW_ATOMIC
        // In the Linux kernel the direction isn't explicitly defined, however
        // in linux/bio.h, you'll notice that its referenced as 1 for write and 0
        // for read.
        int dir = 0;

        if (rw == BIO_DIR_WRITE)
            dir = 1;

#if KFIOC_HAS_INFLIGHT_RW_ATOMIC
        atomic_set(&gd->part0.in_flight[dir], in_flight);
#else
        gd->part0.in_flight[dir] = in_flight;
#endif
#else
        gd->part0.in_flight = in_flight;
#endif
# else
        gd->in_flight = in_flight;
# endif
    }
}

/**
 * @brief returns 1 if bio is O_SYNC priority
 */
#if KFIOC_DISCARD == 1
static int kfio_bio_is_discard(struct bio *bio)
{
#if !KFIOC_HAS_SEPARATE_OP_FLAGS
#if KFIOC_HAS_UNIFIED_BLKTYPES
    /*
     * RHEL6.1 backported a partial set of the unified blktypes, but
     * still has separate bio and req DISCARD flags. If BIO_RW_DISCARD
     * exists, then that is used on the bio.
     */
#if KFIOC_HAS_BIO_RW_DISCARD
    return bio->bi_rw & (1 << BIO_RW_DISCARD);
#else
    return bio->bi_rw & REQ_DISCARD;
#endif
#else
    return bio_rw_flagged(bio, BIO_RW_DISCARD);
#endif
#else
    return bio_op(bio) == REQ_OP_DISCARD;
#endif
}
#endif

// kfio_dump_bio not supported for ESX4
#if !defined(__VMKLNX__)
/// @brief   Dump an OS bio to the log
/// @param   msg   prefix for message
/// @param   bio   the bio to drop
static void kfio_dump_bio(const char *msg, const struct bio * const bio)
{
    uint64_t sector;

    kassert(bio);

    // Use a local conversion to avoid printf format warnings on some platforms
    sector = (uint64_t)BI_SECTOR(bio);
#if KFIOC_HAS_SEPARATE_OP_FLAGS
    infprint("%s: sector: %llx: flags: %lx : op: %x, op_flags: %x : vcnt: %x", msg,
             sector, (unsigned long)bio->bi_flags, bio_op(bio), bio_flags(bio), bio->bi_vcnt);
#else
    infprint("%s: sector: %llx: flags: %lx : rw: %lx : vcnt: %x", msg,
             sector, (unsigned long)bio->bi_flags, bio->bi_rw, bio->bi_vcnt);
#endif
    infprint("%s : idx: %x : phys_segments: %x : size: %x",
             msg, BI_IDX(bio), bio->bi_phys_segments, BI_SIZE(bio));

#if KFIOC_BIO_HAS_HW_SEGMENTS
    infprint("%s: hw_segments: %x : hw_front_size: %x : hw_back_size %x", msg,
             bio->bi_hw_segments, bio->bi_hw_front_size, bio->bi_hw_back_size);
#endif
#if KFIOC_BIO_HAS_SEG_SIZE
    infprint("%s: seg_front_size %x : seg_back_size %x", msg,
             bio->bi_seg_front_size, bio->bi_seg_back_size);
#endif
# if KFIOC_BIO_HAS_ATOMIC_REMAINING
    infprint("%s: remaining %x", msg, atomic_read(&bio->bi_remaining));
#endif
#if KFIOC_HAS_BIO_COMP_CPU
    infprint("%s: comp_cpu %u", msg, bio->bi_comp_cpu);
#endif

    infprint("%s: max_vecs: %x : cnt %x : io_vec %p : end_io: %p : private: %p",
             msg, bio->bi_max_vecs, kfio_bio_cnt(bio), bio->bi_io_vec,
             bio->bi_end_io, bio->bi_private);
#if KFIOC_BIO_HAS_DESTRUCTOR
    infprint("%s: destructor: %p", msg, bio->bi_destructor);
#endif
#if KFIOC_BIO_HAS_INTEGRITY
    // Note that we don't use KFIOC_BIO_HAS_SPECIAL as yet.
    infprint("%s: integrity: %p", msg, bio_integrity(bio) );
#endif
}
#endif // !__VMKLNX__

static inline void kfio_set_comp_cpu(kfio_bio_t *fbio, struct bio *bio)
{
#if KFIOC_HAS_BIO_COMP_CPU
    fbio->fbio_cpu = bio->bi_comp_cpu;
    if (fbio->fbio_cpu == -1)
    {
        fbio->fbio_cpu = kfio_current_cpu();
    }
#else
    fbio->fbio_cpu = kfio_current_cpu();
#endif
}

#if !defined(__VMKLNX__)

static unsigned long __kfio_bio_sync(struct bio *bio)
{
#if KFIOC_HAS_SEPARATE_OP_FLAGS
    return bio_flags(bio) == REQ_SYNC;
#else
#if KFIOC_HAS_UNIFIED_BLKTYPES
    return bio->bi_rw & REQ_SYNC;
#elif KFIOC_HAS_BIO_RW_FLAGGED
    return bio_rw_flagged(bio, BIO_RW_SYNCIO);
#else
    return bio_sync(bio);
#endif
#endif
}

/* Rethink whole block, too messy now! */
static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error)
{
#if KFIOC_BIO_HAS_ERROR
    /* now a member of the bio struct */
    bio->bi_error = error;
#endif /* KFIOC_BIO_HAS_ERROR*/
    bio_endio(bio
#if KFIOC_BIO_ENDIO_HAS_BYTES_DONE
             , bytes_complete
#endif /* ! KFIO_BIO_ENDIO_HAS_BYTES_DONE */
#if !KFIOC_BIO_ENDIO_REMOVED_ERROR
             , error
#endif /* KFIOC_BIO_ENDIO_REMOVED_ERROR*/
           );
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

#if KFIOC_DISCARD == 1
    if (kfio_bio_is_discard(bio))
    {
        return 1;
    }
#endif

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
#if KFIOC_HAS_SEPARATE_OP_FLAGS
    engprint("Rejecting malformed bio %p sector %lu size 0x%08x flags 0x%08lx op 0x%08x op_flags 0x%04x\n", bio,
             (unsigned long)BI_SECTOR(bio), BI_SIZE(bio), (unsigned long)bio->bi_flags, bio_op(bio), bio_flags(bio));
#else
    engprint("Rejecting malformed bio %p sector %lu size 0x%08x flags 0x%08lx rw 0x%08lx\n", bio,
             (unsigned long)BI_SECTOR(bio), BI_SIZE(bio), (unsigned long)bio->bi_flags, bio->bi_rw);
#endif
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

#if KFIOC_DISCARD == 1
    if (kfio_bio_is_discard(bio))
    {
        fbio->fbio_cmd = KBIO_CMD_DISCARD;
    }
    else
#endif
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
#if KFIOC_HAS_BIO_RW_SYNC == 1
    if (bio->bi_rw & (1 << BIO_RW_SYNC))
    {
        return 1;
    }
#endif

#if KFIOC_HAS_BIO_RW_UNPLUG == 1
    if (bio->bi_rw & (1 << BIO_RW_UNPLUG))
    {
        return 1;
    }
#endif

#if KFIOC_HAS_REQ_UNPLUG == 1
    if (bio->bi_rw & REQ_UNPLUG)
    {
        return 1;
    }
#endif
#if KFIOC_DISCARD == 1
    return kfio_bio_is_discard(bio);
#else
    return 0;
#endif
}

#if KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN
static void kfio_unplug(struct request_queue *q)
{
    struct kfio_disk *disk = q->queuedata;
    struct bio *bio = NULL;

    spin_lock_irq(q->queue_lock);
    if (blk_remove_plug(q))
    {
        bio = disk->bio_head;
        disk->bio_head = NULL;
        disk->bio_tail = NULL;
    }
    spin_unlock_irq(q->queue_lock);

    if (bio)
    {
        kfio_kickoff_plugged_io(q, bio, disk->maxiosize);
    }
}

static void test_safe_plugging(void)
{
    /* empty */
}

#else

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

#if KFIOC_REQUEST_QUEUE_UNPLUG_FN_HAS_EXTRA_BOOL_PARAM
static void kfio_unplug_cb(struct blk_plug_cb *cb, bool from_schedule)
#else
static void kfio_unplug_cb(struct blk_plug_cb *cb)
#endif
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
struct test_plug {
    struct blk_plug_cb cb;
    int safe;
};

#if KFIOC_REQUEST_QUEUE_UNPLUG_FN_HAS_EXTRA_BOOL_PARAM
static void safe_unplug_cb(struct blk_plug_cb *cb, bool from_schedule)
#else
static void safe_unplug_cb(struct blk_plug_cb *cb)
#endif
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
#endif

static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp,
                                              kfio_numa_node_t node)
{
    struct request_queue *rq;

    test_safe_plugging();

#if KFIOC_HAS_BLK_ALLOC_QUEUE_NODE
    rq = blk_alloc_queue_node(GFP_NOIO, node);
#else
    rq = blk_alloc_queue(GFP_NOIO);
#endif
    if (rq != NULL)
    {
        rq->queuedata = dp;
        blk_queue_make_request(rq, kfio_make_request);
        rq->queue_lock = (spinlock_t *)dp->queue_lock;
#if KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN
        rq->unplug_fn = kfio_unplug;
#endif
    }
    return rq;
}

static int should_holdoff_writes(struct kfio_disk *disk)
{
    return fio_test_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}
#endif /* !defined(__VMKLNX__) */

void kfio_set_write_holdoff(struct kfio_disk *disk)
{
    fio_set_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}

void kfio_clear_write_holdoff(struct kfio_disk *disk)
{
    struct request_queue *q = disk->gd->queue;

    fusion_cv_lock_irq(&disk->state_lk);
    fio_clear_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
    fusion_condvar_broadcast(&disk->state_cv);
    fusion_cv_unlock_irq(&disk->state_lk);

    /*
     * Re-kick the queue, if we stopped handing out writes
     * due to log pressure. Do this out-of-line, since we can
     * be called here with internal locks held and with IRQs
     * disabled.
     */
    if (q->request_fn)
    {

        /*
         * The VMKernel initializes the request_queue in their implementation of
         * blk_queue_init().  However, they both neglect to initialize the
         * unplug handler, and also attempt to call it.  This badness is worked
         * around here and also in kfio_init_queue().
         */
#if defined(__VMKLNX__)
        if (q->unplug_work.work.pending & __WORK_OLD_COMPAT_BIT)
        {
            if (q->unplug_work.work.func.old == NULL)
            {
                engprint("Null callback avoided; (__WORK_OLD_COMPAT_BIT)\n");
                return;
            }
        }
        else
        {
            if (q->unplug_work.work.func.new == NULL)
            {
                engprint("Null callback avoided; (!__WORK_OLD_COMPAT_BIT)\n");
                return;
            }
        }
#endif /* defined(__VMKLNX__) */

        kfio_restart_queue(q);
    }
}

/*
 * Increment the exclusive lock pending count
 */
void kfio_mark_lock_pending(kfio_disk_t *fgd)
{
#if !defined(__VMKLNX__)
    struct gendisk *gd = fgd->gd;
    struct request_queue *q = gd->queue;

    /*
     * Only the request_fn driven model issues requests in a non-blocking
     * manner. The direct queued model does not need this.
     */
    if (q->request_fn)
    {
        kfio_disk_t *disk = q->queuedata;

        atomic_inc(&disk->lock_pending);
    }
#endif
}

/*
 * Decrement the exclusive lock pending count
 */
void kfio_unmark_lock_pending(kfio_disk_t *fgd)
{
#if !defined(__VMKLNX__)
    struct gendisk *gd = fgd->gd;
    struct request_queue *q = gd->queue;

    if (q->request_fn)
    {
        kfio_disk_t *disk = q->queuedata;

        /*
         * Reset pending back to zero, since we now successfully dropped
         * the lock. If the pending value was bigger than 1, then somebody
         * else likely tried and failed to get the lock. In that case,
         * kick off the queue.
         */
        if (atomic_dec_return(&disk->lock_pending) > 0)
        {
            atomic_set(&disk->lock_pending, 0);
            kfio_restart_queue(q);
        }
    }
#endif
}


#if !defined(__VMKLNX__)
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

#if KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN
static inline void *kfio_should_plug(struct request_queue *q)
{
    if (use_workqueue == USE_QUEUE_NONE)
    {
        return q;
    }

    return NULL;
}
static struct bio *kfio_add_bio_to_plugged_list(void *data, struct bio *bio)
{
    struct request_queue *q = data;
    struct kfio_disk *disk = q->queuedata;
    struct bio *ret = NULL;

    spin_lock_irq(q->queue_lock);
    if (disk->bio_tail)
    {
        disk->bio_tail->bi_next = bio;
    }
    else
    {
        disk->bio_head = bio;
    }
    disk->bio_tail = bio;
    if (kfio_bio_should_submit_now(bio) && use_workqueue == USE_QUEUE_NONE)
    {
        ret = disk->bio_head;
        disk->bio_head = disk->bio_tail = NULL;
    }
    blk_plug_device(q);
    spin_unlock_irq(q->queue_lock);

    return ret;
}
#else
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
#endif

#if KFIOC_MAKE_REQUEST_FN_VOID
static void kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET
#elif KFIOC_MAKE_REQUEST_FN_UINT
static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET 0
#else
static int kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET 0
#endif
{
    struct kfio_disk *disk = queue->queuedata;
    void *plug_data;

    if (bio_data_dir(bio) == WRITE && !holdoff_writes_under_pressure(disk))
    {
        kassert_once(!"timed out waiting for queue to unstall.");
        __kfio_bio_complete(bio, 0, -EIO);
        return FIO_MFN_RET;
    }

#if KFIOC_HAS_BIO_COMP_CPU
    if (bio->bi_comp_cpu == -1)
    {
        bio_set_completion_cpu(bio, kfio_current_cpu());
    }
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,13,0)
    blk_queue_bounce(queue, &bio);
#endif
    // blk_queue_bounce_limit(queue, BLK_BOUNCE_ANY);

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

#endif /* !defined(__VMKLNX__) */

#if KFIOC_BARRIER == 1
static void kfio_prepare_flush(struct request_queue *q, struct request *req)
{
}
#endif

#if KFIOC_USE_IO_SCHED

static unsigned long kfio_get_req_hard_nr_sectors(struct request *req)
{
    kassert(use_workqueue == USE_QUEUE_RQ);

#if KFIOC_USE_NEW_IO_SCHED
    return blk_rq_sectors(req);
#else
    return req->hard_nr_sectors;
#endif
}

static int kfio_end_that_request_first(struct request *req, int success, int count)
{
#if defined(__VMKLNX__)
    return 0;
#else

    kassert(use_workqueue == USE_QUEUE_RQ);

# if KFIOC_HAS_END_REQUEST
    return end_that_request_first(req, success, count);
# else
    // We are already holding the queue lock so we call __blk_end_request
    // If we ever decide to call this _without_ holding the lock, we should
    // use blk_end_request() instead.
    return __blk_end_request(req, success == 1 ? 0 : success, blk_rq_bytes(req));
# endif /* KFIOC_HAS_END_REQUEST */
#endif /* __VMKLNX__ */
}

static void kfio_end_that_request_last(struct request *req, int uptodate)
{
#if !KFIOC_USE_NEW_IO_SCHED
    kassert(use_workqueue == USE_QUEUE_RQ);

#if KFIOC_HAS_END_REQUEST
    end_that_request_last(req, uptodate);
#endif
#endif
}

static void kfio_end_request(struct request *req, int uptodate)
{

    if (kfio_end_that_request_first(req, uptodate, kfio_get_req_hard_nr_sectors(req)))
    {
        kfail();
    }
    kfio_end_that_request_last(req, uptodate);
}

static void kfio_restart_queue(struct request_queue *q)
{
#if KFIOC_HAS_BLK_DELAY_QUEUE
    blk_delay_queue(q, 0);
#else
    if (!test_and_set_bit(QUEUE_FLAG_PLUGGED, &q->queue_flags))
    {
#if KFIOC_KBLOCKD_SCHEDULE_HAS_QUEUE_ARG
        kblockd_schedule_work(q, &q->unplug_work);
#else
        kblockd_schedule_work(&q->unplug_work);
#endif /* KFIOC_KBLOCKD_SCHEDULE_HAS_QUEUE_ARG */
    }
#endif /* KFIOC_HAS_BLK_DELAY_QUEUE */
}

#if !defined(__VMKLNX__)
/*
 * Returns non-zero if we have pending requests, either at the OS level
 * or on our internal retry list
 */
static int kfio_has_pending_requests(struct request_queue *q)
{
        kfio_disk_t *disk = q->queuedata;
        int ret;

#if KFIOC_USE_NEW_IO_SCHED
        ret = blk_peek_request(q) != NULL;
#else
        ret = !elv_queue_empty(q);
#endif
        return ret || disk->retry_cnt;
}

/*
 * Typeless container_of(), since we are abusing a different type for
 * our atomic list entry.
 */
#define void_container(e, t, f) (t*)(((fio_uintptr_t)(t*)((char *)(e))-((fio_uintptr_t)&((t*)0)->f)))

/*
 * Splice completion list to local list and handle them
 */
static int complete_list_entries(struct request_queue *q, struct kfio_disk *dp)
{
    struct request *req;
    struct fio_atomic_list list;
    struct fio_atomic_list *entry, *tmp;
    int completed = 0;

    fusion_atomic_list_init(&list);
    fusion_atomic_list_splice(&dp->comp_list, &list);
    if (fusion_atomic_list_empty(&list))
        return completed;

    fusion_atomic_list_for_each(entry, tmp, &list)
    {
        req = void_container(entry, struct request, special);
        kfio_end_request(req, KFIOC_ERROR < 0 ? KFIOC_ERROR : 1);
        completed++;
    }

    kassert(dp->pending >= completed);
    dp->pending -= completed;
    return completed;
}

static void kfio_blk_complete_request(struct request *req)
{
    struct request_queue *q = req->q;
    struct kfio_disk *dp = q->queuedata;
    struct fio_atomic_list *entry;
    sector_t last_sector;
    int      last_rw;
    int i;

    /*
     * Save a local copy of the last sector in the request before putting
     * it onto atomic completion list. Once it is there, requests is up for
     * grabs for others completors and we cannot legally access its fields
     * anymore.
     */
    last_sector = blk_rq_pos(req) + (blk_rq_bytes(req) >> 9);
    last_rw = rq_data_dir(req);

    /*
     * Add this completion entry atomically to the shared completion
     * list.
     */
    entry = (struct fio_atomic_list *) &req->special;
    fusion_atomic_list_init(entry);
    fusion_atomic_list_add(entry, &dp->comp_list);

    /*
     * Next try and grab the queue_lock, which we need for completion.
     * If we succeed, set that we are now doing completions with the
     * KFIO_DISK_COMPLETION bit. If we fail, check if someone is already
     * doing completions. If they are, we are golden. They will see our
     * completion entry and do it in due time. If not, repeat the lock
     * check and completion bit check.
     */
    while (!fusion_spin_trylock_irqsave(dp->queue_lock))
    {
        if (fio_test_bit(KFIO_DISK_COMPLETION, &dp->disk_state))
            return;

        cpu_relax();
    }

    if (dp->last_dispatch == last_sector && dp->last_dispatch_rw == last_rw)
    {
        dp->last_dispatch = 0;
        dp->last_dispatch_rw = -1;
    }

    /*
     * Great, lets do completions. Mark us as doing that, and complete
     * the list for up to 8 runs (or until it was empty).
     */
    fio_set_bit(KFIO_DISK_COMPLETION, &dp->disk_state);
    for (i = 0; i < 8; i++)
    {
        if (!complete_list_entries(q, dp))
        {
            break;
        }
    }

    /*
     * Complete the list after clearing the bit, in case we raced with
     * someone adding entries.
     */
    fio_clear_bit(KFIO_DISK_COMPLETION, &dp->disk_state);
    complete_list_entries(q, dp);

    /*
     * We usually don't have to re-kick the queue, it happens automatically.
     * But if we dropped out of the queueing loop, we do have to guarantee
     * that we kick things into gear on our own. So do that if we end up
     * in the situation where we have pending IO from the OS but nothing
     * left internally.
     */
    if (!dp->pending && kfio_has_pending_requests(q))
    {
        kfio_restart_queue(q);
    }

    fusion_spin_unlock_irqrestore(dp->queue_lock);
}

#else // #if defined(__VMKLNX__)

static void kfio_blk_do_softirq(struct request *req)
{
    struct request_queue *rq;
    struct kfio_disk     *dp;

    rq = req->q;
    dp = rq->queuedata;
    bi = req->bio;

    fusion_spin_lock_irqsave(dp->queue_lock);
    kfio_end_request(req, KFIOC_ERROR < 0 ? KFIOC_ERROR : 1);
    fusion_spin_unlock_irqrestore(dp->queue_lock);
}

static void kfio_blk_complete_request(struct request *req)
{
    // On ESX completions must be done through the softirq handler.
    blk_complete_request(req);
}
#endif

static void kfio_elevator_change(struct request_queue *q, char *name)
{
// We don't use the real elevator_change since it isn't in the RedHat Whitelist
// see FH-14626 for the gory details.
#if !defined(__VMKLNX__)
# if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
    elevator_exit(q, q->elevator);
# else
    elevator_exit(q->elevator);
# endif
# if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    q->elevator = NULL;
# endif
    if (elevator_init(q, name))
    {
        errprint("Failed to initialize noop io scheduler\n");
    }
#endif
}

#if KFIOC_HAS_BIO_COMP_CPU
#if KFIOC_MAKE_REQUEST_FN_VOID
static void kfio_rq_make_request_fn(struct request_queue *q, struct bio *bio)
#else
static int kfio_rq_make_request_fn(struct request_queue *q, struct bio *bio)
#endif
{
    kfio_disk_t *disk = q->queuedata;

    if (unlikely(kfio_bio_should_fail_requests(disk->dev)))
    {
        __kfio_bio_complete(bio, 0, -EIO);
#if KFIOC_MAKE_REQUEST_FN_VOID
        return;
#else
        return 0;
#endif
    }

    if (bio->bi_comp_cpu == -1)
    {
        bio_set_completion_cpu(bio, kfio_current_cpu());
    }

#if KFIOC_MAKE_REQUEST_FN_VOID
    disk->make_request_fn(q, bio);
#else
    return disk->make_request_fn(q, bio);
#endif
}
#endif

static void kfio_set_queue_depth(struct request_queue *q, unsigned int depth)
{
    int cong;

    q->nr_requests = depth;

    /* set watermark for congestion on */
    cong = q->nr_requests - (q->nr_requests / 8) + 1;
    if (cong > q->nr_requests)
        cong = q->nr_requests;

    q->nr_congestion_on = cong;

    /* set watermark for congestion off */
    cong = q->nr_requests - (q->nr_requests / 8) - (q->nr_requests / 16) - 1;
    if (cong < 1)
        cong = 1;

    q->nr_congestion_off = cong;
}

#if defined(__VMKLNX__)
static void dummy_unplug(struct work_struct *p)
{
    engprint("Null callback avoided in dummy_unplug()\n");
}
#endif /* #if defined(__VMKLNX__) */

static struct request_queue *kfio_init_queue(struct kfio_disk *dp,
                                             kfio_numa_node_t node)
{
    struct request_queue *rq;
    static char elevator_name[] = "noop";

    kassert(use_workqueue == USE_QUEUE_RQ);

    rq = blk_init_queue_node(kfio_do_request, (spinlock_t *)dp->queue_lock, node);
    if (rq != NULL)
    {
        rq->queuedata = dp;

#if KFIOC_HAS_BIO_COMP_CPU
        dp->make_request_fn = rq->make_request_fn;
        rq->make_request_fn = kfio_rq_make_request_fn;
#endif

        /* Change out the default io scheduler, and use noop instead */
        kfio_elevator_change(rq, elevator_name);
#if defined(__VMKLNX__)
        // ESX expects completions to be done in the softirq handler.
        // Otherwise we get mysterious hangs with no error messages.
        blk_queue_softirq_done(rq, kfio_blk_do_softirq);
#endif

        /* Increase queue depth  */
        kfio_set_queue_depth(rq, DEFAULT_LINUX_MAX_NR_REQUESTS);
    }

    /*
     * The VMKernel initializes the request_queue in their implementation of
     * blk_queue_init().  However, they both neglect to initialize the
     * unplug handler, and also attempt to call it.  This badness is worked
     * around here and also in kfio_clear_write_holdoff()
     */
#if defined(__VMKLNX__)
    if (rq->unplug_work.work.pending & __WORK_OLD_COMPAT_BIT)
    {
        if (rq->unplug_work.work.func.old == NULL)
        {
            engprint("Null work func callback found & fixed (__WORK_OLD_COMPAT_BIT)\n");
            rq->unplug_work.work.func.old = (old_work_func_t) dummy_unplug;
        }
    }
    else
    {
        if (rq->unplug_work.work.func.new == NULL)
        {
            engprint("Null work func callback found & fixed (!__WORK_OLD_COMPAT_BIT)\n");
            rq->unplug_work.work.func.new = (work_func_t) dummy_unplug;
        }
    }
#endif /* #if defined(__VMKLNX__) */

    return rq;
}

static struct request *kfio_blk_fetch_request(struct request_queue *q)
{
    struct request *req;

    kassert(use_workqueue == USE_QUEUE_RQ);

#if KFIOC_USE_NEW_IO_SCHED
    req = blk_fetch_request(q);
#else
    req = elv_next_request(q);
    if (req != NULL)
    {
        blkdev_dequeue_request(req);
    }
#endif

    return req;
}

static void kfio_req_completor(kfio_bio_t *fbio, uint64_t bytes_done, int error)
{
    struct request *req = (struct request *)fbio->fbio_parameter;
    // struct bio *bio = req->bio;

    if (fbio->fbio_cmd == KBIO_CMD_READ || fbio->fbio_cmd == KBIO_CMD_WRITE)
        kfio_sgl_dma_unmap(fbio->fbio_sgl);

    // bio->bi_error = error;
    KFIOC_ERROR = error;

    if (unlikely(fbio->fbio_flags & KBIO_FLG_DUMP))
    {
        kfio_dump_fbio(fbio);
#if !defined(__VMKLNX__)
        blk_dump_rq_flags(req, FIO_DRIVER_NAME);
#endif
    }

    kfio_blk_complete_request(req);
}

#if defined(__VMKLNX__) || KFIOC_HAS_RQ_FOR_EACH_BIO == 0
#  define __rq_for_each_bio(lbio, req) \
           if ((req->bio)) \
               for (lbio = (req)->bio; lbio; lbio = lbio->bi_next)
#endif
#if defined(__VMKLNX__) || KFIOC_HAS_RQ_IS_SYNC == 0
#  if KFIOC_HAS_REQ_RW_SYNC == 1
#    define rq_is_sync(rq)  (((rq)->flags & REQ_RW_SYNC) != 0)
#  else
#    define rq_is_sync(rq)  (0)
#  endif
#endif

/// @brief determine if a block request is an empty flush.
///
/// NB: any write request may have the flush bit set, but we do not treat
/// those as special, since all writes are powercut safe and may not be
/// reordered. This function detects only zero-length requests with the
/// flush bit set, which do require special handling.
static inline bool rq_is_empty_flush(const struct request *req)
{
#if KFIOC_NEW_BARRIER_SCHEME == 1
    if (blk_rq_bytes(req) == 0 && req->cmd_flags & REQ_FLUSH)
    {
        return true;
    }
#elif KFIOC_BARRIER_USES_QUEUE_FLAGS == 1
    if (blk_rq_bytes(req) == 0 && req_op(req) == REQ_OP_FLUSH)
    {
        return true;
    }
#elif KFIOC_BARRIER == 1
    if (blk_rq_bytes(req) == 0 && blk_barrier_rq(req))
    {
        return true;
    }
#endif
    return false;
 }

static kfio_bio_t *kfio_request_to_bio(kfio_disk_t *disk, struct request *req,
                                       bool can_block)
{
    struct fio_device *dev = disk->dev;
    kfio_bio_t *fbio;

    if (can_block)
    {
        fbio = kfio_bio_alloc(dev);
    }
    else
    {
        fbio = kfio_bio_try_alloc(dev);
    }
    if (fbio == NULL)
    {
        return NULL;
    }

    fbio->fbio_offset = blk_rq_pos(req) << 9;
    fbio->fbio_size   = blk_rq_bytes(req);

    fbio->fbio_completor = kfio_req_completor;
    fbio->fbio_parameter = (fio_uintptr_t)req;

    /* Detect flush barrier requests. */
    if (rq_is_empty_flush(req))
    {
        fbio->fbio_cmd = KBIO_CMD_FLUSH;

        // Zero-length flush requests sometimes have uninitialized blk_rq_pos
        // when received from the OS. Force a valid address to prevent woe
        // elsewhere in the driver.
        fbio->fbio_offset = 0;

        /* Actually flush on non-powercut-safe cards */
        fbio->fbio_flags |= KBIO_FLG_SYNC;
    }
    else
    /* Detect trim requests. */
#if KFIOC_DISCARD == 1
    if (enable_discard &&
#if KFIOC_HAS_SEPARATE_OP_FLAGS
        (req_op(req) == REQ_OP_DISCARD))
#else
        (req->cmd_flags & REQ_DISCARD))
#endif
    {
        fbio->fbio_cmd = KBIO_CMD_DISCARD;
    }
    else
#endif
    /* This is a read or write request. */
    {
        struct bio *lbio;
        int error = 0;

        kfio_set_comp_cpu(fbio, req->bio);

        if (rq_data_dir(req) == WRITE)
        {
            int sync_write = rq_is_sync(req);

            fbio->fbio_cmd = KBIO_CMD_WRITE;

#if KFIOC_NEW_BARRIER_SCHEME == 1 || KFIOC_BARRIER_USES_QUEUE_FLAGS == 1 || KFIOC_NEW_BARRIER_SCHEME == 1
            sync_write |= req->cmd_flags & REQ_FUA;
#endif

            if (sync_write)
            {
                fbio->fbio_flags |= KBIO_FLG_SYNC;
            }
        }
        else
        {
            fbio->fbio_cmd = KBIO_CMD_READ;
        }

        __rq_for_each_bio(lbio, req)
        {
            error = kfio_sgl_map_bio(fbio->fbio_sgl, lbio);
            if (error != 0)
            {
                break;
            }
        }

        /*
         * Sanity checking: combined length of all bios should cover request
         * size. Only makes sense if above iteration over bios was successful.
         */
        if (error == 0 && fbio->fbio_size != kfio_sgl_size(fbio->fbio_sgl))
        {
            // ESX 40u1 wins this time...
            errprint("Request size mismatch. Request size %u dma size %u\n",
                     (unsigned)fbio->fbio_size, kfio_sgl_size(fbio->fbio_sgl));
#if KFIOC_DISCARD == 1
            infprint("Request cmd 0x%016llx\n", (uint64_t)req->cmd_flags);
#endif
            __rq_for_each_bio(lbio, req)
            {
#if KFIOC_HAS_BIOVEC_ITERATORS
                struct bio_vec vec;
                struct bvec_iter bv_i;
#else
                struct bio_vec *vec;
                int bv_i;
#endif
#if KFIOC_HAS_SEPARATE_OP_FLAGS
                infprint("\tbio %p sector %lu size 0x%08x flags 0x%08lx op 0x%04x op_flags 0x%04x\n", lbio,
                         (unsigned long)BI_SECTOR(lbio), BI_SIZE(lbio), (unsigned long)lbio->bi_flags,
                         bio_op(lbio), bio_flags(lbio));
#else
                infprint("\tbio %p sector %lu size 0x%08x flags 0x%08lx rw 0x%08lx\n", lbio,
                         (unsigned long)BI_SECTOR(lbio), BI_SIZE(lbio), (unsigned long)lbio->bi_flags, lbio->bi_rw);
#endif
                infprint("\t\tvcnt %u idx %u\n", lbio->bi_vcnt, BI_IDX(lbio));

                bio_for_each_segment(vec, lbio, bv_i)
                {
#if KFIOC_HAS_BIOVEC_ITERATORS
                    infprint("vec %d: page %p offset %u len %u\n", bv_i.bi_idx, vec.bv_page,
                             vec.bv_offset, vec.bv_len);
#else
                    infprint("vec %d: page %p offset %u len %u\n", bv_i, vec->bv_page,
                             vec->bv_offset, vec->bv_len);
#endif
                }
            }
            infprint("SGL content:\n");
            kfio_sgl_dump(fbio->fbio_sgl, NULL, "\t", 0);
            error = -EIO; /* Any non-zero value will serve. */
        }

        if (error != 0)
        {
            /* This should not happen. */
            kfail();
            kfio_bio_free(fbio);
            return NULL;
        }

        error = kfio_sgl_dma_map(fbio->fbio_sgl, fbio->fbio_dmap,
            fbio->fbio_cmd == KBIO_CMD_READ ? IODRIVE_DMA_DIR_READ : IODRIVE_DMA_DIR_WRITE);

        if (error < 0)
        {
            kfio_bio_free(fbio);
            return NULL;
        }

        kassert(fbio->fbio_size == kfio_sgl_size(fbio->fbio_sgl));
    }

    return fbio;
}

#if defined(__VMKLNX__)
static void kfio_do_request(struct request_queue *q)
{
    kfio_disk_t       *disk = q->queuedata;

    /*
     * In this mode we do queueing, so all we need is to signal the queue
     * handler to come back to us and start fetching newly added requests.
     */
    fio_bio_posted(disk->dev);
}

#else

/*
 * Pull off as many requests as we can from the IO scheduler, then
 * drop the linux block device queue lock and map/submit them. If we
 * fail allocating internal fbios, requeue the leftovers.
 */
static void kfio_do_request(struct request_queue *q)
{
    kfio_disk_t *disk = q->queuedata;
    struct request *req;
    LIST_HEAD(list);
    int rc, queued, requeued, rw, wait_for_merge;
    struct list_head *entry, *tmp;

    /*
     * Guard against unwanted recursion.
     * While we drop the request lock to allow more requests to be enqueued,
     * we don't allow another thread to process requests in the driver until
     * we process the ones we removed in order to maintain ordering requirements.
     */
    if (disk->in_do_request)
    {
        return;
    }

    if (unlikely(kfio_bio_should_fail_requests(disk->dev)))
    {
        kfio_kill_requests(q);
        return;
    }
    disk->in_do_request = 1;

    for ( ; ; )
    {
        wait_for_merge = rw = queued = requeued = 0;
        /*
         * Grab requests that we queued internally for retry first before
         * diving into the OS pending queue.
         */
        if (!list_empty(&disk->retry_list))
        {
            list_splice_init(&disk->retry_list, &list);
            rw = disk->retry_cnt;
            disk->retry_cnt = 0;
        }
        else
        {
            while ((req = kfio_blk_fetch_request(q)))
            {
                // If the OS ever hands us a request with 'special' set, we'll blindly
                // trust that it is a fbio pointer. That would be bad. Hence the
                // following assert.
                kassert(req->special == 0);

#if KFIOC_HAS_BLK_FS_REQUEST
                if (blk_fs_request(req))
#elif KFIOC_HAS_BLK_RQ_IS_PASSTHROUGH
                if (!blk_rq_is_passthrough(req))
#else
                if (req->cmd_type == REQ_TYPE_FS)
#endif
                {
                    // Do not allow improperly aligned requests to go through. OS should
                    // not really allow these to reach our entry point, but be paranoid
                    // and fail ill-formed requests before they get to do any damage at
                    // lower layers.
                    if (((blk_rq_pos(req) << 9) & disk->sector_mask) != 0 ||
                        (blk_rq_bytes(req) & disk->sector_mask) != 0)
                    {
                        if (!rq_is_empty_flush(req))
                        {
                            errprint("Rejecting unaligned request %llu:%lu, device sector size is %u\n",
                                     (unsigned long long)(blk_rq_pos(req) << 9), (unsigned long)blk_rq_bytes(req),
                                     disk->sector_mask + 1);

                            kfio_end_request(req, -EIO);
                            continue;
                        }
                    }

                    if (disk->pending && disk->last_dispatch == blk_rq_pos(req) &&
                        disk->last_dispatch_rw == rq_data_dir(req) &&
                        req->bio != 0 &&
                        req->bio->bi_vcnt == 1)
                    {
                        blk_requeue_request(q, req);
                        wait_for_merge = 1;
                        break;
                    }
                }
                list_add_tail(&req->queuelist, &list);
                disk->last_dispatch = blk_rq_pos(req) + (blk_rq_bytes(req) >> 9);
                disk->last_dispatch_rw = rq_data_dir(req);
                rw++;
            }
        }

        /*
         * By and large we always queue everything we pull off, so
         * add the total sum here. We'll decrement if we need to
         * requeue again
         */
        disk->pending += rw;
        spin_unlock_irq(q->queue_lock);

        list_for_each_safe(entry, tmp, &list)
        {
            kfio_bio_t *fbio;

            req = list_entry(entry, struct request, queuelist);

            fbio = req->special;
            if (!fbio)
                fbio = kfio_request_to_bio(disk, req, false);
            if (!fbio)
                break;

            list_del_init(&req->queuelist);
            fbio->fbio_flags |= KBIO_FLG_NONBLOCK;

            rc = kfio_bio_submit_handle_retryable(fbio);
            if (rc)
            {
                if (kfio_bio_failure_is_retryable(rc))
                {
                    /*
                     * "busy error" conditions. Store the prepped part
                     * for faster retry, and exit.
                     */
                    req->special = fbio;
                    list_add(&req->queuelist, &list);
                    break;
                }
                // Bio is already finished, do not touch it.
                continue;
            }
            queued++;
        }

        spin_lock_irq(q->queue_lock);
        if (!list_empty(&list))
        {
            LIST_HEAD(tmp_list);

            /*
             * Splice not-done requests to our internal retry list.
             * Old kernels have neither list_splice_tail() nor a
             * __list_splice() we can use, so splice the old list out
             * first to maintain ordering.
             * The initial count was 'rw', 'queued' is how many we
             * actually sent off. So remaining count is rw - queued.
             */
            list_splice_init(&disk->retry_list, &tmp_list);
            list_splice_init(&list, &disk->retry_list);
            list_splice(&tmp_list, &disk->retry_list);
            disk->retry_cnt += (rw - queued);
            requeued = (rw - queued);
        }

        if (requeued)
        {
            /*
             * Subtract what we did not hand off to the hardware
             */
            disk->pending -= requeued;
        }

        /*
         * If we have pending IO, just let some of that restart us. If not,
         * then ensure that we get reentered. This is essentially a
         * "should not happen" situation.
         */
        if (!disk->pending)
        {
            /*
             * IO must have completed already, re-loop to potentially
             * avoid punting to kblockd
             */
            if (wait_for_merge)
            {
                continue;
            }
#if KFIOC_HAS_BLK_DELAY_QUEUE
            blk_delay_queue(q, 1);
#else
            blk_plug_device(q);
#endif
        }

        if (!queued || requeued || wait_for_merge)
        {
            break;
        }
    }
    disk->in_do_request = 0;
}
#endif /* __VMKLNX__ */

#endif /* KFIOC_USE_IO_SCHED */

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
