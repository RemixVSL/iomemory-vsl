/*
  This "meta" file is supposed to abstract things that change in kblock.c
  and should provide simplified defines that are named after their function.
  Both aimed at making the code cleaner, more readable and perhaps more
  maintainable. Blocks should be isolated and only cover one item.
 */
#ifndef __FIO_KBLOCK_META_H__
#define __FIO_KBLOCK_META_H__

#if KFIOC_X_LINUX_HAS_PART_STAT_H
#include <linux/part_stat.h>
#endif /* KFIOC_X_LINUX_HAS_PART_STAT_H */

#if KFIOC_X_BLK_ALLOC_DISK_EXISTS
  #define BLK_ALLOC_QUEUE dp->gd->queue;
  #define BLK_ALLOC_DISK blk_alloc_disk
#else /* KFIOC_X_BLK_ALLOC_DISK_EXISTS */
  #if KFIOC_X_HAS_MAKE_REQUEST_FN != 1
    #define BLK_ALLOC_QUEUE blk_alloc_queue(node);
  #endif
  #define BLK_ALLOC_DISK alloc_disk
#endif

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
  #else /* KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS */
    #define BLK_ALLOC_QUEUE blk_alloc_queue(kfio_make_request, node);
  #endif /* KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS */

#else /* KFIOC_X_HAS_MAKE_REQUEST_FN */
  #if KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T
    #define KFIO_SUBMIT_BIO blk_qc_t kfio_submit_bio(struct bio *bio)
    #define KFIO_SUBMIT_BIO_RC return FIO_MFN_RET;
  #else
    #define KFIO_SUBMIT_BIO void kfio_submit_bio(struct bio *bio)
    #define KFIO_SUBMIT_BIO_RC
  #endif
  KFIO_SUBMIT_BIO;

  #define BLK_QUEUE_SPLIT blk_queue_split(&bio);
#endif /* KFIOC_X_HAS_MAKE_REQUEST_FN */
#if KFIOC_X_GENHD_PART0_IS_A_POINTER
  #define GD_PART0 gd->part0
  #define GET_BDEV bdgrab(disk->gd->part0);
#else /* KFIOC_X_GENHD_PART0_IS_A_POINTER */
  #define GD_PART0 &gd->part0
  #define GET_BDEV disk->gd->part0
#endif /* KFIOC_X_GENHD_PART0_IS_A_POINTER */

#if KFIOC_X_VOID_ADD_DISK
#define ADD_DISK add_disk(disk->gd);
#else
#define ADD_DISK if (add_disk(disk->gd)) { infprint("Error while adding disk!"); }
#endif


#endif /* __FIO_KBLOCK_META_H__ */
