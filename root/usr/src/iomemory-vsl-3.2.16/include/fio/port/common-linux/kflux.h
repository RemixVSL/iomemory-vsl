/*
 * <insert license>
 */

#ifndef __FUSION_LINUX_KERNEL_FLUX_H__
#define __FUSION_LINUX_KERNEL_FLUX_H__

#include <fio/port/kfio_config.h>

#if KFIOC_X_HAS_MMAP_LOCK
#define MMAP_LOCK_SEM mmap_lock
#else /* ! KFIOC_X_HAS_MMAP_LOCK */
#define MMAP_LOCK_SEM mmap_sem
#endif /* KFIOC_X_HAS_MMAP_LOCK */

#endif /* __FUSION_LINUX_KERNEL_FLUX_H__ */
