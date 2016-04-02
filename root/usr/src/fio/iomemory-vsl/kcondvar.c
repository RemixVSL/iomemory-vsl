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
#include <fio/port/kcondvar.h>
#include <fio/port/dbgset.h>

/*
  URGENT!!! Pay special attention to the wait state!

  On Linux, waiting with the uninterruptible flag will cause
  process accounting to consider the task _running_, even
  though it is really sleeping!  This thereby drastically
  increases the amount of CPU the task is recorded as using.

  We still want to block as uninterruptible but ensure we are counted
  accurately as sleeping, so we must set the interruptible flag
  and then ignore any signals which may be received.  The following
  table explains our behavior based on the platform:

  Linux:
  fusion_condvar_timedwait()         - uninterruptible
  fusion_condvar_timedwait_noload()  - interruptible, ignore signals anyway

  Solaris, FreeBSD, etc.:
  fusion_condvar_timedwait()         - uninterruptible
  fusion_condvar_timedwait_noload()  - uninterruptible

*/

/*******************************************************************************
*******************************************************************************/
void noinline fusion_schedule_yield(void)
{
    schedule();
}

/**
*/
void noinline fusion_condvar_init (fusion_condvar_t *cv, const char *name)
{
    init_waitqueue_head((wait_queue_head_t *) cv); // public kernel interface
    return;
}
EXPORT_SYMBOL(fusion_condvar_init);

/**
 */
void noinline fusion_condvar_destroy (fusion_condvar_t *cv)
{
    return;
}
EXPORT_SYMBOL(fusion_condvar_destroy);

/**
 * Signals one waiter on this cv.
 */
void noinline fusion_condvar_signal (fusion_condvar_t *cv)
{
    wait_queue_head_t *q = (wait_queue_head_t *) cv;
    wake_up(q);
}
EXPORT_SYMBOL(fusion_condvar_signal);

/**
 *  Signals all waiters on this cv.
 */
void noinline fusion_condvar_broadcast (fusion_condvar_t *cv)
{
    wait_queue_head_t *q = (wait_queue_head_t *) cv;
    wake_up_all(q);
}

/**
 * The function @sa fusion_condvar_signal(cv) must be called when a change to
 * the wait condition occurs.
 *
 * On return the lock is held.
 *
 * N.B. To ensure SMP safety, use the functions @sa fusion_cv_lock()
 *   and @sa fusion_cv_unlock() around writes that changes the condition
 *   protected by this cv.
 */
void noinline fusion_condvar_wait(fusion_condvar_t *cv,
                                  fusion_cv_lock_t *lock)
{   int is_irqsaved;
    DEFINE_WAIT(_wait);
    wait_queue_head_t *q = (wait_queue_head_t *) cv;

    is_irqsaved = fusion_cv_lock_is_irqsaved(lock);
    /* Linux wakes all threads in wake_up() unless
     * prepare_to_wait_exclusive() is called */
    prepare_to_wait_exclusive(q, &_wait, TASK_INTERRUPTIBLE);

    if (is_irqsaved)
    {
        fusion_cv_unlock_irq(lock);
    }
    else
    {
        fusion_cv_unlock(lock);
    }

    schedule();

    if (is_irqsaved)
    {
        fusion_cv_lock_irq(lock);
    }
    else
    {
        fusion_cv_lock(lock);
    }

    finish_wait(q, &_wait);
}
EXPORT_SYMBOL(fusion_condvar_wait);

/**
 * @parameter[out] reason return value:
 * -FUSION_WAIT_TIMEDOUT      (0)
 * -FUSION_WAIT_INTERRUPTED   (-1)
 * -FUSION_WAIT_TRIGGERED     (1)
 */
int noinline fusion_condvar_timedwait(fusion_condvar_t *cv,
                                      fusion_cv_lock_t *lock,
                                      int64_t timeout)
{
    int is_irqsaved;
    DEFINE_WAIT(_wait);
    wait_queue_head_t *q = (wait_queue_head_t *) cv;
    int tmo = fusion_usectohz(timeout);
    int ret;

    is_irqsaved = fusion_cv_lock_is_irqsaved(lock);
    /* Linux wakes all threads in wake_up() unless
     * prepare_to_wait_exclusive() is called */
    prepare_to_wait_exclusive(q, &_wait, TASK_UNINTERRUPTIBLE);

    if (is_irqsaved)
    {
        fusion_cv_unlock_irq(lock);
    }
    else
    {
        fusion_cv_unlock(lock);
    }

    ret = schedule_timeout(tmo);

    if (is_irqsaved)
    {
        fusion_cv_lock_irq(lock);
    }
    else
    {
        fusion_cv_lock(lock);
    }

    finish_wait(q, &_wait);

    return (ret > 0) ? FUSION_WAIT_TIMEDOUT : FUSION_WAIT_TRIGGERED;
}

/**
 * This is identical to @sa fusion_condvar_timedwait() but the thread is
 *  set as interruptible to avoid adding to the load average.
 */
int noinline fusion_condvar_timedwait_noload(fusion_condvar_t *cv,
                                             fusion_cv_lock_t *lock,
                                             int64_t timeout)
{
    int is_irqsaved;
    DEFINE_WAIT(_wait);
    wait_queue_head_t *q = (wait_queue_head_t *) cv;
    int tmo = fusion_usectohz(timeout);
    int ret;

    is_irqsaved = fusion_cv_lock_is_irqsaved(lock);
    /* Linux wakes all threads in wake_up() unless
     * prepare_to_wait_exclusive() is called */
    prepare_to_wait_exclusive(q, &_wait, TASK_INTERRUPTIBLE);

    if (is_irqsaved)
    {
        fusion_cv_unlock_irq(lock);
    }
    else
    {
        fusion_cv_unlock(lock);
    }
    if (signal_pending(current) )
    {
        flush_signals(current); /* we do not handle signals */
    }
    if (0 == schedule_timeout(tmo) )
    {
        ret = FUSION_WAIT_TIMEDOUT;
    }
    else
    {
        ret = FUSION_WAIT_TRIGGERED;
    }

    if (is_irqsaved)
    {
        fusion_cv_lock_irq(lock);
    }
    else
    {
        fusion_cv_lock(lock);
    }

    finish_wait(q, &_wait);

    return ret;
}

void noinline fusion_cond_resched(void)
{
    cond_resched();
}

