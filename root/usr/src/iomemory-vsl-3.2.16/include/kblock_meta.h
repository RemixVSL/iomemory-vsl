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

#if KFIOC_X_HAS_MAKE_REQUEST_FN
  static unsigned int kfio_make_request(struct request_queue *queue, struct bio *bio);
  #define BLK_QUEUE_SPLIT blk_queue_split(queue, &bio);

  #if KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS
    #define BLK_ALLOC_QUEUE blk_alloc_queue_node(GFP_NOIO, node);
  #else /* KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS */
    #define BLK_ALLOC_QUEUE blk_alloc_queue(kfio_make_request, node);
  #endif /* KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS */

#else /* KFIOC_X_HAS_MAKE_REQUEST_FN */
  blk_qc_t kfio_submit_bio(struct bio *bio);

  #define BLK_ALLOC_QUEUE blk_alloc_queue(node);
  #define BLK_QUEUE_SPLIT blk_queue_split(&bio);
#endif /* KFIOC_X_HAS_MAKE_REQUEST_FN */

#endif /* __FIO_KBLOCK_META_H__ */
