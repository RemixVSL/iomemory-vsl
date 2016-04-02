//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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
 * @brief initialize work item
 */
void noinline fusion_init_work(fusion_work_struct_t *wq, fusion_work_func_t func)
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    INIT_WORK((struct work_struct *) wq, (work_func_t) func);
#elif defined(__VMKLNX__)
    INIT_WORK((struct work_struct *) wq, (void (*)(void *)) func, wq);
#else
    INIT_WORK((struct work_struct *) wq, (work_func_t) func, wq);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_init_work);

/**
 * @brief destroy work item
 */
void noinline fusion_destroy_work(fusion_work_struct_t *work)
{
    return;
}
KFIO_EXPORT_SYMBOL(fusion_destroy_work);

/**
 * @brief schedule some work
 */
void noinline fusion_schedule_work(fusion_work_struct_t *work )
{
    schedule_work((struct work_struct *) work);
}
KFIO_EXPORT_SYMBOL(fusion_schedule_work);

/**
 * @brief initialize delayed work item
 */
void noinline fusion_init_delayed_work(fusion_work_struct_t *wq, fusion_work_func_t func)
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    INIT_DELAYED_WORK((struct delayed_work *) wq, (work_func_t) func);
#elif defined(__VMKLNX__)
    INIT_WORK((struct work_struct *) wq, (void (*)(void *)) func, wq);
#else
    INIT_WORK((struct work_struct *) wq, (work_func_t) func, wq);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_init_delayed_work);

/**
 * @brief destroy work item
 */
void noinline fusion_destroy_delayed_work(fusion_work_struct_t *work)
{
    return;
}
KFIO_EXPORT_SYMBOL(fusion_destroy_delayed_work);

/**
 * @brief schedule delayed work
 * @param wq
 * @param sleep_time - delay time in micro seconds.
 */
void noinline fusion_schedule_delayed_work(fusion_work_struct_t *wq, uint64_t sleep_time )
{
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    schedule_delayed_work((struct delayed_work *) wq, fusion_usectohz(sleep_time));
#else
    schedule_delayed_work((struct work_struct *) wq, fusion_usectohz(sleep_time));
#endif
}

void noinline fusion_might_sleep(void)
{
    might_sleep();
}
KFIO_EXPORT_SYMBOL(fusion_might_sleep);
