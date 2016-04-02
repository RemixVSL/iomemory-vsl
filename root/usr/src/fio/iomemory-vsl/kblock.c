//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if !defined (__linux__)
#error This file supports linux only
#endif

#include "port-internal.h"
#include <fio/port/dbgset.h>
#include <fio/port/kfio.h>
#include <fio/port/bitops.h>
#include <fio/port/linux/kblock.h>

#include <linux/version.h>
#include <linux/fs.h>
#if !defined(__VMKLNX__)
#include <linux/buffer_head.h>
#endif
#include <linux/interrupt.h> // in_interrupt()

extern int use_workqueue;
static int fio_major;

/*
 * RHEL6.1 will trigger both old and new scheme due to their backport,
 * whereas new kernels will trigger only the new scheme. So for the RHEL6.1
 * case, just disable the old scheme to avoid to much ifdef hackery.
 */
#if KFIOC_NEW_BARRIER_SCHEME == 1
#undef KFIOC_BARRIER
#define KFIOC_BARRIER	0
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

extern int kfio_sgl_map_bio(kfio_sg_list_t *sgl, struct bio *bio);

/******************************************************************************
 *   Functions required to register and unregister fio block device driver    *
 ******************************************************************************/

#if defined(__VMKLNX__)
/*
 * The highest existing major number in ESX 4.0 is
 *   VIOTAPE_MAJOR           230
 * found in
 *   src26/include/linux/major.h
 * in subsequent releases of ESX, they will implement
 * automatic assignment of major number, at which point
 * this code must be revisited.
 * Return the block device major number.
 */
#define VMKLNX_FIRST_MAJOR 231
static int
esx_register_blkdev(kfio_pci_dev_t *pdev)
{
    int status = 1;
    int major = VMKLNX_FIRST_MAJOR;

    for(; status && major < MAX_BLKDEV; major++)
    {
        status = vmklnx_register_blkdev(major,
                        register_as_cciss? "cciss" : "fio",
                        kfio_pci_get_bus(pdev),
                        kfio_pci_get_function(pdev),
                        kfio_pci_get_drvdata(pdev));

        /* Without the following break, major is too large by one */
        if (!status)
        {
            break;
        }
    }
    if (status)
    {
        /* Non-zero return value indicates failure */
        return -1;
    }
    return major;
}
#endif /* defined(__VMKLNX__) */

int
kfio_register_blkdev_pre_init(kfio_pci_dev_t *pdev)
{
    /* This should return the major number from the block device file.  Under linux
     * the value set into the global fio_major is correct at this time, and will be
     * fine by the time we create the gendisk.  However, under ESX due to the
     * tweaked ordering of assigning the major number and creating the gendisk,
     * we need to store away the major into a device structure and recall it at the
     * time gendisk is made.
     */
#if defined(__VMKLNX__)
    return esx_register_blkdev(pdev);
#else
    (void)pdev;
    return fio_major;
#endif
}

int kfio_register_blkdev(const char *name)
{
#if defined(__VMKLNX__)
    /*
     * Just has to return >0 so init_fio_blk isn't seen as failure.
     * ESX will register its block device during pci probe.
     */
    fio_major = 1;
#else
    fio_major = register_blkdev(0, name);
#endif
    return fio_major;
}


int kfio_unregister_blkdev(const char *name)
{
    int rc = 0;

#if KFIOC_UNREGISTER_BLKDEV_RETURNS_VOID
    unregister_blkdev(fio_major, name);
#else
    rc = unregister_blkdev(fio_major, name);
#endif

    return rc;
}

/******************************************************************************
 *   Block device open, close and ioctl handlers                              *
 ******************************************************************************/

#if KFIOC_HAS_NEW_BLOCK_METHODS

static int kfio_open(struct block_device *bdev, fmode_t mode)
{
    struct fio_device *dev = bdev->bd_disk->private_data;

    return fio_open(dev);
}

static int kfio_release(struct gendisk *gd, fmode_t mode)
{
    struct fio_device *dev = gd->private_data;

    return fio_release(dev);
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
    .open =         kfio_open,
    .release =      kfio_release,
    .ioctl =        kfio_ioctl,
#if KFIOC_HAS_COMPAT_IOCTL_METHOD
    .compat_ioctl = kfio_compat_ioctl,
#endif
};


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
};

enum
{
    KFIO_DISK_HOLDOFF_BIT = 0,
};

#if !defined(__VMKLNX__)
static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp);
#if KFIOC_MAKE_REQUEST_FN_VOID
static void kfio_make_request(struct request_queue *queue, struct bio *bio);
#else
static int kfio_make_request(struct request_queue *queue, struct bio *bio);
#endif
static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error);
#endif

#if KFIOC_USE_IO_SCHED
static struct request_queue *kfio_init_queue(struct kfio_disk *dp);
static void kfio_do_request(struct request_queue *queue);
static void kfio_blk_do_softirq(struct request *req);
static struct request *kfio_blk_fetch_request(struct request_queue *q);
#endif

#if KFIOC_BARRIER == 1
static void kfio_prepare_flush(struct request_queue *q, struct request *rq);
#endif

static void kfio_invalidate_bdev(struct block_device *bdev);
static void kfio_bdput(struct block_device *bdev);

int kfio_create_disk(struct fio_device *dev, kfio_pci_dev_t *pdev, uint32_t sector_size,
                     uint32_t max_sectors_per_request, uint32_t max_sg_elements_per_request,
                     fusion_spinlock_t *queue_lock, kfio_disk_t **diskp)
{
    struct kfio_disk     *dp;
    struct request_queue *rq;

    dp = kfio_vmalloc(sizeof(*dp));
    if (dp == NULL)
    {
        return -ENOMEM;
    }

    dp->dev  = dev;
    dp->pdev = pdev;
    dp->gd   = NULL;
    dp->rq   = NULL;
    dp->queue_lock = queue_lock;
    dp->bio_tail = NULL;
    dp->bio_head = NULL;
    dp->disk_state = 0;

    fusion_condvar_init(&dp->state_cv, "fio_disk_cv");
    fusion_cv_lock_init(&dp->state_lk, "fio_disk_lk");

#if !defined(EMC)
    if (use_workqueue != USE_QUEUE_RQ)
    {
#endif
#if !defined(__VMKLNX__)
        dp->rq = kfio_alloc_queue(dp);
#endif
#if !defined(EMC)
    }
# if KFIOC_USE_IO_SCHED
    else
    {
        dp->rq = kfio_init_queue(dp);
    }
# endif /* KFIOC_USE_IO_SCHED */
#endif

    if (dp->rq == NULL)
    {
        kfio_destroy_disk(dp);
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

    blk_queue_max_segment_size(rq, PAGE_CACHE_SIZE);

#if KFIOC_HAS_BLK_QUEUE_HARDSECT_SIZE
    blk_queue_hardsect_size(rq, sector_size);
#else
    blk_queue_logical_block_size(rq, sector_size);
#endif
#if KFIOC_DISCARD == 1
    if (enable_discard)
    {
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
    // blk_queue_flush() is a GPL export
    rq->flush_flags = REQ_FLUSH | REQ_FUA;
#elif KFIOC_BARRIER == 1
    // Ignore if ordered mode is wrong - linux will complain
    blk_queue_ordered(rq, iodrive_barrier_type, kfio_prepare_flush);
#endif

    *diskp = dp;
    return 0;
}

int kfio_expose_disk(kfio_disk_t *dp, char *name, int major, int disk_index,
                     uint64_t reported_capacity, uint32_t sector_size,
                     uint32_t max_sg_elements_per_request)
{
    struct gendisk *gd;

    dp->gd = gd = alloc_disk(FIO_NUM_MINORS);

    if (dp->gd == NULL)
    {
        kfio_destroy_disk(dp);
        return -ENOMEM;
    }

    gd->major = major;
    gd->first_minor = FIO_NUM_MINORS * disk_index;
    gd->minors = FIO_NUM_MINORS;
    gd->fops = &fio_bdev_ops;
    gd->queue = dp->rq;
    gd->private_data = dp->dev;
#if defined(__VMKLNX__)
    gd->maxXfer = 16 * PAGE_SIZE; /// experiment with making this much bigger??  8 seems OK
#endif

    fio_bdev_ops.owner = fusion_get_this_module();

    strncpy(gd->disk_name, name, 32);

    set_capacity(gd, reported_capacity * sector_size / KERNEL_SECTOR_SIZE);

    infprint("%s: Creating block device %s: major: %d minor: %d sector size: %d...\n",
             fio_device_get_bus_name(dp->dev), gd->disk_name, gd->major,
             gd->first_minor, sector_size);

    add_disk(dp->gd);

    /*
     * The following is used because access to udev events are restricted
     * to GPL licensed symbols.  Since we have a propriatary licence, we can't
     * use the better interface.  Instead we must poll for the device creation.
     */
#if !defined(__VMKLNX__)
    /* XXXesx missing APIs */
    fio_wait_for_dev(gd->disk_name);
#else // __VMKLNX__
    vmklnx_block_register_sglimit(major, max_sg_elements_per_request);
    vmklnx_block_init_done(major);
#endif

    return 0;
}

void kfio_destroy_disk(kfio_disk_t *disk)
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

#if KFIOC_USE_IO_SCHED
        /*
         * The queue is stopped and dead and no new user requests will be
         * coming to it anymore. Fetch remaining already queued requests
         * and fail them,
         */
        if (use_workqueue == USE_QUEUE_RQ)
        {
            struct request *req;

            while ((req = kfio_blk_fetch_request(disk->rq)) != NULL)
            {
                req->errors = -EIO;
                blk_complete_request(req);
            }
        }
#endif
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
    kfio_vfree(disk, sizeof(*disk));
}

static void kfio_invalidate_bdev(struct block_device *bdev)
{
#if !defined(__VMKLNX__)
#if ! KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS
    invalidate_bdev(bdev, 0);
#else
    invalidate_bdev(bdev);
#endif /* KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS */
#else
    /* XXXesx missing API */
#endif
}

static void kfio_bdput(struct block_device *bdev)
{
#if !defined(__VMKLNX__)
    bdput(bdev);
#else
    /* XXXesx missing API */
#endif
}

/**
 * @parameter duration - in microseconds
 */
void kfio_disk_stat_write_update(kfio_disk_t *fgd, uint32_t totalsize, uint64_t duration)
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
# endif /* KFIOC_CONFIG_PREEMPT_RT */
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

void kfio_disk_stat_read_update(kfio_disk_t *fgd, uint32_t totalsize, uint64_t duration)
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
#else
    return gd->in_flight;
#endif
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
#else
        gd->in_flight = in_flight;
#endif
    }
}

/**
 * @brief returns 1 if bio is O_SYNC priority
 */
#if KFIOC_DISCARD == 1
static int kfio_bio_is_discard(struct bio *bio)
{
    /*
     * RHEL6.1 backported a partial set of the unified blktypes, but
     * still has separate bio and req DISCARD flags. If BIO_RW_DISCARD
     * exists, then that is used on the bio.
     */
#if KFIOC_HAS_UNIFIED_BLKTYPES
#if KFIOC_HAS_BIO_RW_DISCARD
    return bio->bi_rw & (1 << BIO_RW_DISCARD);
#else
    return bio->bi_rw & REQ_DISCARD;
#endif
#else
    return bio_rw_flagged(bio, BIO_RW_DISCARD);
#endif
}
#endif

void kfio_set_write_holdoff(struct kfio_disk *disk)
{
    return fio_set_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}

void kfio_clear_write_holdoff(struct kfio_disk *disk)
{
    fio_clear_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);

    fusion_cv_lock_irq(&disk->state_lk);
    fusion_condvar_broadcast(&disk->state_cv);
    fusion_cv_unlock_irq(&disk->state_lk);
}

#if !defined(__VMKLNX__)

static unsigned long __kfio_bio_sync(struct bio *bio)
{
#if KFIOC_HAS_UNIFIED_BLKTYPES
    return bio->bi_rw & REQ_SYNC;
#elif KFIOC_HAS_BIO_RW_FLAGGED
    return bio_rw_flagged(bio, BIO_RW_SYNCIO);
#else
    return bio_sync(bio);
#endif
}

static void __kfio_bio_complete(struct bio *bio, uint32_t bytes_complete, int error)
{
    bio_endio(bio,
#if ! KFIOC_BIO_ENDIO_REMOVED_BYTES_DONE
              bytes_complete,
#endif /* ! KFIO_BIO_ENDIO_REMOVED_BYTES_DONE */
              error);
}

static struct request_queue *kfio_alloc_queue(struct kfio_disk *dp)
{
    struct request_queue *rq;

    rq = blk_alloc_queue(GFP_NOIO);
    if (rq != NULL)
    {
        rq->queuedata = dp;
        blk_queue_make_request(rq, kfio_make_request);
        rq->queue_lock = (spinlock_t *)dp->queue_lock;
    }
    return rq;
}

static void kfio_bio_completor(kfio_bio_t *fbio, uint32_t bytes_complete, int error)
{
    struct bio *bio = (struct bio *)fbio->fbio_parameter;

    if (fbio->fbio_cmd == KBIO_CMD_READ || fbio->fbio_cmd == KBIO_CMD_WRITE)
        kfio_sgl_dma_unmap(fbio->fbio_sgl);

    __kfio_bio_complete(bio,  bytes_complete, error);
}

static int should_holdoff_writes(struct kfio_disk *disk)
{
    return fio_test_bit(KFIO_DISK_HOLDOFF_BIT, &disk->disk_state);
}

/* returns 1 if we don't or no longer need to stall under write pressure,
 *  else returns 0 if we are still stalled.
 */
static int holdoff_writes_under_pressure(struct kfio_disk *disk)
{
    int reason = FUSION_WAIT_TRIGGERED;
    int count = 0;

    if (!should_holdoff_writes(disk))
    {
        return 1;
    }
    while (1)
    {
        fusion_cv_lock_irq(&disk->state_lk);
        // wait until unstalled: hardcoded 1 minute timeout
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

        fusion_cv_unlock(&disk->state_lk);

        if (reason == FUSION_WAIT_TIMEDOUT)
        {
            count++;
            errprint("%s %d: seconds %d %s\n", __FUNCTION__, __LINE__, count * 5,
                should_holdoff_writes(disk) ? "stalled" : "unstalled");

            if (count >= 12)
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }
    return 1;
}

static kfio_bio_t *kfio_convert_bio(struct request_queue *queue, struct bio *bio)
{
    struct kfio_disk *disk = queue->queuedata;
    kfio_bio_t       *fbio;
    int error;
    /*
     * This should use kfio_bio_try_alloc and should automatically
     * retry later when some requests become available.
     */
    fbio = kfio_bio_alloc(disk->dev);
    if (fbio == NULL)
    {
        return NULL;
    }

    blk_queue_bounce(queue, &bio);

    // Convert Linux bio to Fusion-io bio and send it down to processing.
    fbio->fbio_flags  = 0;
    fbio->fbio_size   = bio->bi_size;
    fbio->fbio_offset = bio->bi_sector * KERNEL_SECTOR_SIZE;

    fbio->fbio_completor = kfio_bio_completor;
    fbio->fbio_parameter = (fio_uintptr_t)bio;

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

        error = kfio_sgl_dma_map(fbio->fbio_sgl, fbio->fbio_dmap,
                    fbio->fbio_cmd == KBIO_CMD_READ ? IODRIVE_DMA_DIR_READ : IODRIVE_DMA_DIR_WRITE);
        if (error < 0)
        {
            kfio_bio_free(fbio);
            return NULL;
        }
    }
    return fbio;
}

#if KFIOC_MAKE_REQUEST_FN_VOID
static void kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET
#else
static int kfio_make_request(struct request_queue *queue, struct bio *bio)
#define FIO_MFN_RET 0
#endif
{
    struct kfio_disk *disk = queue->queuedata;
    struct kfio_bio  *fbio;

    if (bio_data_dir(bio) == WRITE && !holdoff_writes_under_pressure(disk))
    {
        kassert_once(!"timed out waiting for queue to unstall.");
        __kfio_bio_complete(bio, 0, -EIO);
        return FIO_MFN_RET;
    }

    /* Directly convert and dispatch the bio. */
    if (use_workqueue == USE_QUEUE_NONE)
    {
        fbio = kfio_convert_bio(queue, bio);
        if (fbio == NULL)
        {
            __kfio_bio_complete(bio,  0, -EIO);
        }
        else
        {
            kfio_bio_submit(fbio);
        }
    }
    /* Queue bio for later retrieval by queue runner. */
    else
    {
        spin_lock_irq(queue->queue_lock);
        if (disk->bio_tail)
            disk->bio_tail->bi_next = bio;
        else
            disk->bio_head = bio;
        disk->bio_tail = bio;
        spin_unlock_irq(queue->queue_lock);
        fio_bio_posted(disk->dev);
    }

    return FIO_MFN_RET;
}

#endif /* !defined(__VMKLNX__) */

#if KFIOC_BARRIER == 1
static void kfio_prepare_flush(struct request_queue *q, struct request *req)
{
    req->special = req;
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

static void kfio_blk_do_softirq(struct request *req)
{
    struct request_queue *rq;
    struct kfio_disk     *dp;

    rq = req->q;
    dp = rq->queuedata;

    fusion_spin_lock_irqsave(dp->queue_lock);
    kfio_end_request(req, req->errors < 0 ? req->errors : 1);
    fusion_spin_unlock_irqrestore(dp->queue_lock);

#if KFIOC_BARRIER == 1
    blk_run_queue(rq);
#endif
}

static void kfio_elevator_change(struct request_queue *q, char *name)
{
#if !defined(__VMKLNX__)
    elevator_exit(q->elevator);
# if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    q->elevator = NULL;
# endif
    if (elevator_init(q, name))
    {
        errprint("Failed to initialize noop io scheduler\n");
    }
#endif
}

static struct request_queue *kfio_init_queue(struct kfio_disk *dp)
{
    struct request_queue *rq;
    static char elevator_name[] = "noop";

    kassert(use_workqueue == USE_QUEUE_RQ);

    rq = blk_init_queue(kfio_do_request, (spinlock_t *)dp->queue_lock);

    if (rq != NULL)
    {
        rq->queuedata = dp;

        /* Change out the default io scheduler, and use noop instead */
        kfio_elevator_change(rq, elevator_name);
        blk_queue_softirq_done(rq, kfio_blk_do_softirq);
    }
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

static void kfio_req_completor(kfio_bio_t *fbio, unsigned int bytes_done, int error)
{
    struct request *req = (struct request *)fbio->fbio_parameter;

    if (fbio->fbio_cmd == KBIO_CMD_READ || fbio->fbio_cmd == KBIO_CMD_WRITE)
        kfio_sgl_dma_unmap(fbio->fbio_sgl);

    req->errors = error;
    /* Do we really need to complete through softint? */
    blk_complete_request(req);
}

#if defined(__VMKLNX__) || KFIOC_HAS_RQ_POS_BYTES == 0
#  define blk_rq_pos(rq)    ((rq)->sector)
#  define blk_rq_bytes(rq)  ((rq)->nr_sectors << 9)
#endif
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

static kfio_bio_t *kfio_request_to_bio(kfio_disk_t *disk, struct request *req)
{
    struct fio_device *dev = disk->dev;

    kfio_bio_t *fbio = kfio_bio_alloc(dev);
    if (fbio == NULL)
    {
        errprint("%s: kfio_bio_alloc() failed.\n", fio_device_get_bus_name(dev));
        return NULL;
    }

    fbio->fbio_offset = blk_rq_pos(req) << 9;
    fbio->fbio_size   = blk_rq_bytes(req);

    fbio->fbio_completor = kfio_req_completor;
    fbio->fbio_parameter = (fio_uintptr_t)req;

    /* Detect flush barriere requests. */
#if KFIOC_NEW_BARRIER_SCHEME == 1
    if (req->cmd_flags & REQ_FLUSH)
    {
        fbio->fbio_cmd = KBIO_CMD_FLUSH;
    }
    else
#elif KFIOC_BARRIER == 1
    if (req->special != NULL || (fbio->fbio_size == 0 && blk_barrier_rq(req)))
    {
        fbio->fbio_cmd = KBIO_CMD_FLUSH;

        if (iodrive_barrier_sync != 0)
        {
            fbio->fbio_flags |= KBIO_FLG_SYNC;
        }
    }
    else
#endif

#if KFIOC_DISCARD == 1
    // KBV: this test *was*:
    //
    //  if (enable_discard && blk_discard_rq(req))
    //
    // however, blk_discard_rq() disappeared in linux 2.6.36, and was previously defined as
    // this flag test.
    if (enable_discard && (req->cmd_flags & REQ_DISCARD))
    {
        fbio->fbio_cmd = KBIO_CMD_DISCARD;
    }
    else
#endif
    /* This is a read or write request. */
    {
        struct bio *lbio;
        int error = 0;

        if (rq_data_dir(req) == WRITE)
        {
            int sync_write = rq_is_sync(req);

            fbio->fbio_cmd = KBIO_CMD_WRITE;

#if KFIOC_NEW_BARRIER_SCHEME == 1
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
                /* This should not happen. */
                kfail();
                kfio_bio_free(fbio);
                return NULL;
            }
        }

        if (error == 0)
        {
            unsigned tries = 0;

            do
            {
                error = kfio_sgl_dma_map(fbio->fbio_sgl, fbio->fbio_dmap,
                        fbio->fbio_cmd == KBIO_CMD_READ ? IODRIVE_DMA_DIR_READ : IODRIVE_DMA_DIR_WRITE);

                if (error < 0)
                {
                    if (!in_interrupt())
                    {
                        set_current_state(TASK_UNINTERRUPTIBLE);
                        schedule_timeout(1);
                    }
                }
            } while ((error < 0) && (tries++ < 10));

            if (error < 0)
            {
                errprint("%s: kfio_sgl_dma_map failed(%d).\n", __func__, error);
                kfio_bio_free(fbio);
                return NULL;
            }

            kassert(fbio->fbio_size == kfio_sgl_size(fbio->fbio_sgl));
        }
    }
    return fbio;
}

static void kfio_do_request(struct request_queue *q)
{
    kfio_disk_t *disk = q->queuedata;

    /*
     * In this mode we do queueing, so all we need is to signal the queue
     * handler to come back to us and start fetching newly added requests.
     */
    fio_bio_posted(disk->dev);
}

#endif /* KFIOC_USE_IO_SCHED */

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
            
            fbio = kfio_request_to_bio(disk, creq);
            if (fbio == NULL)
            {
                errprint("%s: kfio_request_to_bio() failed.\n", fio_device_get_bus_name(disk->dev));
                creq->errors = -EIO;
                blk_complete_request(creq);
            }
            return fbio;
        }
    }
#endif
    return NULL;
}

