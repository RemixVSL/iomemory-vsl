//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KSIMPLE_EVENT_H__
#define __FIO_PORT_KSIMPLE_EVENT_H__

/** @brief fusion_simple_events are similar to condition variables that
 * do not have a lock or an actual condition to check.
 *
 * MS Windows requires specification at initialization time whether it 
 * will wake one waiting thread or multiple on a signal.
 *
 */


/**
 * @struct _fusion_simple_event_t
 * @brief placeholder struct that is large enough for the @e real OS-specific structure:
 * Linux => wait_queue_head_t (size 72)
 * Solaris =>  kcondvar_t
 * Windows =>  KEVENT
 * FreeBSD =>  struct cv (16 bytes)
 * OSX     =>  IOLock * + int (16 bytes)
 */
struct _fusion_simple_event_t {
    char dummy[196];
} FUSION_STRUCT_ALIGN(8);

typedef struct _fusion_simple_event_t  fusion_simple_event_t;


/* These are defined in kcondvar.h which is included before this file 
 * in ktypes.h
 */
//#define FUSION_WAIT_TIMEDOUT      (0)
//#define FUSION_WAIT_INTERRUPTED   (-1)
//#define FUSION_WAIT_TRIGGERED     (1)

/**
 * @brief fusion_simple_event_init()
 * b_broadcast is set to 0 if the the signal of the simple_event will 
 * wake one waiting thread, and set to non-zero if the signal of the 
 * simple_event will wake all waiting threads.
 */
extern void fusion_simple_event_init(fusion_simple_event_t *se, int b_broadcast);
extern void fusion_simple_event_destroy(fusion_simple_event_t *se);
/**
 * @brief fusion_simple_event_signal()
 * @parameter se the fusion_simple_event_t that receives the signal
 *
 * @sa fusion_simple_event_signal() will wake one or more waiting thread
 *  depending on the b_broadcast parameter at initialization, q.v.
 *  There is no way to single a single waiting thread on a broadcast
 *  simple_event or vice versa.
 *
 *  simple_events have no provision for locking in contrast to condition variables.
 */
extern void fusion_simple_event_signal(fusion_simple_event_t *se);
/**
 * @brief fusion_simple_event_wait() - sleep until event gets signaled
 * @parameter se the fusion_simple_event_t to wait on
 * @parameter timeout maximum number of microseconds to wait before returning.
 *
 * The function @sa fusion_simple_event_signal() should be called when one or
 *  more threads waiting on the eimple_event need to be woken.
 *
 *  simple_events have no provision for locking in contrast to condition variables.
 */
extern void fusion_simple_event_wait(fusion_simple_event_t *se);
extern int  fusion_simple_event_timedwait(fusion_simple_event_t *se, long long timeout);

#endif  /* __FIO_PORT_KSIMPLE_EVENT__ */

