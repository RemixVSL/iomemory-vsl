/*
  This "meta" file is supposed to abstract things that change in kblock.c
  and should provide simplified defines that are named after their function.
  Both aimed at making the code cleaner, more readable and perhaps more
  maintainable. Blocks should be isolated and only cover one item.
 */
#ifndef __FIO_KBLOCK_META_H__
#define __FIO_KBLOCK_META_H__

#include <linux/version.h>

#if __has_include("linux/part_stat.h")
#include <linux/part_stat.h>
#endif
#include <linux/blkdev.h>
#include <kblock_meta.h>

#if KFIOC_X_BLK_ALLOC_DISK_EXISTS
  #define BLK_ALLOC_QUEUE dp->gd->queue;
  #define BLK_ALLOC_DISK blk_alloc_disk(FIO_NUM_MINORS);
  #define BLK_MQ_ALLOC_QUEUE blk_mq_init_queue(&disk->tag_set);
#elif KFIOC_X_BLK_ALLOC_DISK_HAS_QUEUE_LIMITS
  #define BLK_ALLOC_QUEUE dp->gd->queue;
  #define BLK_ALLOC_DISK blk_alloc_disk(NULL, FIO_NUM_MINORS);
  #define BLK_MQ_ALLOC_QUEUE blk_mq_alloc_queue(&disk->tag_set, NULL, NULL);
#else /* KFIOC_X_BLK_ALLOC_DISK_EXISTS */
  #if KFIOC_X_HAS_MAKE_REQUEST_FN != 1
    #define BLK_ALLOC_QUEUE blk_alloc_queue(node);
  #endif /* KFIOC_X_HAS_MAKE_REQUEST_FN */
  #define BLK_ALLOC_DISK alloc_disk(FIO_NUM_MINORS);
  #define BLK_MQ_ALLOC_QUEUE blk_mq_init_queue(&disk->tag_set);
#endif /* KFIOC_X_BLK_ALLOC_DISK_EXISTS */


#if KFIOC_X_BIO_HAS_BI_BDEV
  #define BIO_DISK bi_bdev->bd_disk
#else /* KFIOC_X_BIO_HAS_BI_BDEV */
  #define BIO_DISK bi_disk
#endif /* KFIOC_X_BIO_HAS_BI_BDEV */


#if KFIOC_X_HAS_MAKE_REQUEST_FN
  static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio);
  #define KFIO_SUBMIT_BIO_RC return FIO_MFN_RET;
  #define BLK_QUEUE_SPLIT blk_queue_split(queue, &bio);

  #if KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS
    #define BLK_ALLOC_QUEUE blk_alloc_queue_node(GFP_NOIO, node);
  #elif KFIOC_X_BLK_ALLOC_QUEUE_EXISTS
    #define BLK_ALLOC_QUEUE blk_alloc_queue(GFP_NOIO);
  #else
    #define BLK_ALLOC_QUEUE blk_alloc_queue(kfio_make_request, node);
  #endif /* KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS */

#else /* KFIOC_X_HAS_MAKE_REQUEST_FN */
  #if KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T
    #define KFIO_SUBMIT_BIO blk_qc_t kfio_submit_bio(struct bio *bio)
    #define KFIO_SUBMIT_BIO_RC return FIO_MFN_RET;
  #else
    #define KFIO_SUBMIT_BIO void kfio_submit_bio(struct bio *bio)
    #define KFIO_SUBMIT_BIO_RC
  #endif /*KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T */
  KFIO_SUBMIT_BIO;

  #if KFIOC_X_BIO_SPLIT_TO_LIMITS
    #define BLK_QUEUE_SPLIT bio = bio_split_to_limits(bio);
  #else
    #define BLK_QUEUE_SPLIT blk_queue_split(&bio);
  #endif /* KFIOC_X_BIO_SPLIT_TO_LIMITS */
#endif /* KFIOC_X_HAS_MAKE_REQUEST_FN */

// should check for hd_struct vs gendisk
#if KFIOC_X_GENHD_PART0_IS_A_POINTER
  #define GD_PART0 disk->gd->part0
  #define GET_BDEV disk->gd->part0
#else /* KFIOC_X_GENHD_PART0_IS_A_POINTER */
  #define GD_PART0 &disk->gd->part0
  #define GET_BDEV bdget_disk(disk->gd, 0);
#endif /* KFIOC_X_GENHD_PART0_IS_A_POINTER */


#if KFIOC_X_VOID_ADD_DISK
#define ADD_DISK add_disk(disk->gd);
#else
#define ADD_DISK if (add_disk(disk->gd)) { infprint("Error while adding disk!"); }
#endif /* KFIOC_X_VOID_ADD_DISK */

#if KFIOC_X_DISK_HAS_OPEN_MUTEX
#define SHUTDOWN_MUTEX &disk->gd->open_mutex
#else
#define SHUTDOWN_MUTEX &linux_bdev->bd_mutex
#endif /* KFIOC_X_DISK_HAS_OPEN_MUTEX */

#if !defined(GENHD_FL_EXT_DEVT)
// 5.17 moved GD to explicitly do this by default
#define KFIO_DISABLE_GENHD_FL_EXT_DEVT 1
#endif

#if !defined(QUEUE_FLAG_DISCARD)
#define BD_OPENERS atomic_read(&linux_bdev->bd_openers)
#define SET_QUEUE_FLAG_DISCARD
#else
#define BD_OPENERS linux_bdev->bd_openers
// https://lore.kernel.org/linux-btrfs/20220409045043.23593-25-hch@lst.de/
#define SET_QUEUE_FLAG_DISCARD blk_queue_flag_set(QUEUE_FLAG_DISCARD, rq);
#endif /* ! QUEUE_FLAG_DISCARD */


#endif /* __FIO_KBLOCK_META_H__ */
