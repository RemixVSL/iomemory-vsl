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
#include <fio/port/ktypes.h>

struct _linux_simple_event {
    wait_queue_head_t  wq;
    int                b_broadcast; /*0 => wake one waiter else wake all*/
} FUSION_STRUCT_ALIGN(8);
typedef struct _linux_simple_event linux_simple_event_t;

/**
 * @parameter b_broadcast - 0 => single waiting thread wakes on signal
 *                          non-zero => all waiting threads wake on signal
 */
void noinline fusion_simple_event_init (fusion_simple_event_t *se, 
    int b_broadcast) 
{
    linux_simple_event_t *pe = (linux_simple_event_t *) se;

    init_waitqueue_head(&pe->wq); // public kernel interface
    pe->b_broadcast = b_broadcast ? 1 : 0;
}

/**
 */
void noinline fusion_simple_event_destroy (fusion_simple_event_t *se) 
{
    return;
}

/**
 */
void noinline fusion_simple_event_signal (fusion_simple_event_t *se)
{
    linux_simple_event_t *pe = (linux_simple_event_t *) se;
    
    if (pe->b_broadcast) 
    {
        wake_up_all(&pe->wq);
    }
    else
    {
        wake_up(&pe->wq);
    }
}

/**
 */
void noinline fusion_simple_event_wait(fusion_simple_event_t *se)
{   
    linux_simple_event_t *pe = (linux_simple_event_t *) se;
    DEFINE_WAIT(_wait);

    if (pe->b_broadcast) 
    {
        prepare_to_wait(&pe->wq, &_wait, TASK_INTERRUPTIBLE); 
    }
    else
    {
        prepare_to_wait_exclusive(&pe->wq, &_wait, TASK_INTERRUPTIBLE); 
    }

    schedule();

    finish_wait(&pe->wq, &_wait);
}

/**
 */
int noinline fusion_simple_event_timedwait(fusion_simple_event_t *se, long long timeout)
{
    linux_simple_event_t *pe = (linux_simple_event_t *) se;
    DEFINE_WAIT(_wait);
    int tmo = fusion_usectohz(timeout);
    int ret;

    if (pe->b_broadcast) 
    {
        prepare_to_wait(&pe->wq, &_wait, TASK_INTERRUPTIBLE); 
    }
    else
    {
        prepare_to_wait_exclusive(&pe->wq, &_wait, TASK_INTERRUPTIBLE); 
    }

    ret = schedule_timeout(tmo);

    finish_wait(&pe->wq, &_wait);

    return (ret > 0) ? FUSION_WAIT_TIMEDOUT : FUSION_WAIT_TRIGGERED;
}

