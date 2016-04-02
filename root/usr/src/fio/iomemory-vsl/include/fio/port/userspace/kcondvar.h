//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_KCONDVAR_H__
#error Please include <fio/port/kcondvar.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_USERSPACE_KCONDVAR_H__
#define __FIO_PORT_USERSPACE_KCONDVAR_H__

typedef fusion_spinlock_t fusion_cv_lock_t;

extern void fusion_cond_resched(void);

#define fusion_cv_lock_init(lk, n) fusion_init_spin(lk, n)
#define fusion_cv_lock_destroy(lk) fusion_destroy_spin(lk)
#define fusion_cv_lock(lk)         fusion_spin_lock(lk)
#define fusion_cv_unlock(lk)       fusion_spin_unlock(lk)
#define fusion_cv_lock_irq(lk)     fusion_spin_lock_irqsave(lk)
#define fusion_cv_unlock_irq(lk)   fusion_spin_unlock_irqrestore(lk)

#endif

