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

/* XXX work_func_t undefined in early kernels:
 * the func member of work_struct is   void (*func)(void *)
 */

#if KFIOC_MISSING_WORK_FUNC_T && !defined(__VMKLNX__)
typedef void (*work_func_t)(void *);
#endif
#include <linux/workqueue.h>

void noinline fusion_setup_dpc(fusion_dpc_t *wq, fusion_work_func_t func)
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    INIT_WORK((struct work_struct *) wq, (work_func_t) func);
#elif defined(__VMKLNX__)
    INIT_WORK((struct work_struct *) wq, (void (*)(void *)) func, wq);
#else
    INIT_WORK((struct work_struct *) wq, (work_func_t) func, wq);
#endif
}

void noinline fusion_setup_delayed_dpc(fusion_delayed_dpc_t *wq, fusion_work_func_t func)
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    INIT_DELAYED_WORK((struct delayed_work *) wq, (work_func_t) func);
#elif defined(__VMKLNX__)
    INIT_WORK((struct work_struct *) wq, (void (*)(void *)) func, wq);
#else
    INIT_WORK((struct work_struct *) wq, (work_func_t) func, wq);
#endif
}

/**
 */
void noinline fusion_schedule_dpc(fusion_dpc_t *work)
{
    fusion_schedule_work(work);
}

/**
 * @brief schedule some work
 */
void noinline fusion_schedule_work(fusion_dpc_t *work )
{
    schedule_work((struct work_struct *) work);
}
EXPORT_SYMBOL(fusion_schedule_work);

/**
 * @brief schedule delayed work
 * @parameter wq
 * @parameter sleepTime - delay time in micro seconds.
 */
void fusion_schedule_delayed_work(fusion_delayed_dpc_t *wq, uint64_t sleepTime )
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    schedule_delayed_work((struct delayed_work *) wq, fusion_usectohz(sleepTime));
#else
    schedule_delayed_work((struct work_struct *) wq, fusion_usectohz(sleepTime));
#endif
}

