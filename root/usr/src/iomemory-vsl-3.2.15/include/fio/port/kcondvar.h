//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
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

#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KCONDVAR_H__
#define __FIO_PORT_KCONDVAR_H__

/** @brief Recommended use of condition variables and their associated locks:
 *
 * Initialization:
 *
 *   fusion_spinlock_t lock;
 *   fusion_condvar_t cv;
 *   volatile int condition;
 *
 *   fusion_cv_lock_init(&lock);
 *   fusion_condvar_init(&cv);
 *
 * Waiting on a condition:
 *
 *   fusion_cv_lock(&lock);
 *   while(!condition)
 *   {
 *       fusion_condvar_wait(&cv, &lock);
 *   }
 *   // lock is held and condition is non-zero
 *   fusion_cv_unlock(&lock);
 *
 * Signaling threads waiting on condition variable:
 *
 *   fusion_spin_lock(&lock);
 *   condition = 1;
 *   fusion_condvar_signal(&cv)
 *   fusion_spin_unlock(&lock)
 *
 * Cleanup:
 *
 *   fusion_cv_lock_destroy(&lock);
 *   fusion_condvar_destroy(&cv);
 *
 */

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/kcondvar.h>
#elif defined(__linux__) || defined(__VMKLNX__)
#  include <fio/port/common-linux/kcondvar.h>
#elif defined(__SVR4) && defined(__sun)
#  include <fio/port/solaris/kcondvar.h>
#elif defined(__hpux__)
#  include <fio/port/hpux/kcondvar.h>
#elif defined(__FreeBSD__)
#  include <fio/port/freebsd/kcondvar.h>
#elif defined(_AIX)
#  include <fio/port/aix/kcondvar.h>
#elif defined(__OSX__)
#  include <fio/port/osx/kcondvar.h>
#elif defined(WINNT) || defined(WIN32)
#  include <fio/port/windows/kcondvar.h>
#elif defined(UEFI)
#  include <fio/port/uefi/kcondvar.h>
#else
#error OS not yet supported.
#endif

extern void fusion_schedule_yield(void);

/**
 * @struct _fusion_condvar_t
 * @brief placeholder struct that is large enough for the @e real OS-specific
 * structure:
 * Linux => wait_queue_head_t (size 72)
 * Solaris =>  kcondvar_t
 * Windows =>  LIST_ENTRY
 * FreeBSD =>  struct cv
 * OS X    =>  nothing
 * AIX     =>  tid_t + struct trb * (size 16)
 */
struct FUSION_STRUCT_ALIGN(8) _fusion_condvar_t {
    char dummy[148];
};

typedef struct _fusion_condvar_t  fusion_condvar_t;

FUSION_STRUCT_ALIGN_VERIFY(8, fusion_condvar_t);

#define FUSION_WAIT_TIMEDOUT      (0)
#define FUSION_WAIT_INTERRUPTED   (-1)
#define FUSION_WAIT_TRIGGERED     (1)

extern void fusion_condvar_init(fusion_condvar_t *cv, const char *name);
extern void fusion_condvar_destroy(fusion_condvar_t *cv);
/**
 * @brief fusion_condvar_signal()
 * @param cv the condition variable that receives the signal
 *
 * @sa fusion_condvar_signal() will wake one thread waiting on the condition
 *   variable, @sa fusion_condvar_broadcast() will wake all threads waiting
 *   on the condition variable.
 * The lock passed in as the second parameter to the fusion_condvar_?wait()
 *   functions must be held before calling @sa fusion_condvar_signal() or
 *   @sa fusion_condvar_broadcast().
 */
extern void fusion_condvar_signal(fusion_condvar_t *cv);
extern void fusion_condvar_broadcast(fusion_condvar_t *cv);
/**
 * @brief fusion_condvar_wait - sleep until a condition is true
 * @param cv the fusion_condvar_t to wait on
 * @param lock the lock needs to be held before calling this function
 *          and will be held on return.
 *
 * The function @sa fusion_condvar_signal() or fusion_condvar_broadcast()
 *  should be called when a change to the wait condition occurs to wake
 *  any thread blocked on the condition variable.
 *
 * On return, the condition is very likely true, but should be rechecked.
 *   The lock is held on return.
 *
 */
extern void fusion_condvar_wait(fusion_condvar_t *cv,
                                fusion_cv_lock_t *lock);

/**
 * @param cv the fusion_condvar_t to wait on
 * @param lock the lock needs to be held before calling this function
 *          and will be held on return.
 * @param timeout_us maximum number of microseconds to wait before returning.
 */
extern int  fusion_condvar_timedwait(fusion_condvar_t *cv,
                                     fusion_cv_lock_t *lock,
                                     int64_t timeout_us);
extern int  fusion_condvar_timedwait_noload(fusion_condvar_t *cv,
                                            fusion_cv_lock_t *lock,
                                            int64_t timeout_us);
extern uint64_t  fusion_condvar_timedwait_noload_elapsed(fusion_condvar_t *cv,
                                                         fusion_cv_lock_t *lock,
                                                         int64_t timeout_us);
/**
 * @brief sleep iff the OS deems it necessary.
 */
extern void fusion_cond_resched(void);

#endif  /* __FIO_PORT_KCONDVAR_H__ */

