// -----------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------

#ifndef __FIO_PORT_STATE_H__
#define __FIO_PORT_STATE_H__
// DC-840: Direct Cache explodes when dbgset.h is included from here.
// Re-Enable when DC build is fixed.
//#include <fio/port/dbgset.h>


#if defined(__KERNEL__)
#include <fio/port/ktypes.h>
#include <fio/port/kcondvar.h>
#endif
#include <fio/port/vararg.h>

/*---------------------------------------------------------------------------*/

typedef struct
{
    volatile uint32_t state;
#if defined(__KERNEL__)
    fusion_condvar_t cv;
    fusion_cv_lock_t lk;
#endif
} fio_state_t;

#define FIO_STATE_INVALID           0

#ifndef FIO_DEFINE_STATE_NAMES

#define BEGIN_STATE_LIST(name)      typedef enum  { name##_invalid = 0,
#define DECLARE_STATE(name)         name
#define END_STATE_LIST(name)        } name##_t;  \
     extern const char * const name##_names[];

#define STATE_GET_NAME(type, s)  type##_names[(s).state]
#define STATE_GET_NAMEP(type, s)  type##_names[(s)->state]

#else

#define BEGIN_STATE_LIST(name)      const char * const name##_names[] = { "INVALID",
#define DECLARE_STATE(name)            #name
#define END_STATE_LIST(name)        };

#endif // FIO_DEFINE_STATE_NAMES

/*---------------------------------------------------------------------------*/

#if defined(__KERNEL__)

/**
 * @brief __fio_init_state - initializes fusion_event_t
 */
FIO_NONNULL_PARAMS static inline void fio_init_state(fio_state_t *s, const char *name, uint32_t initial)
{
    fusion_condvar_init(&s->cv, name);
    fusion_cv_lock_init(&s->lk, name);
    s->state = initial;
}

/**
 * @brief __fio_init_state - initializes fusion_event_t
 */
FIO_NONNULL_PARAMS static inline void fio_destroy_state(fio_state_t *s)
{
    fusion_condvar_destroy(&s->cv);
    fusion_cv_lock_destroy(&s->lk);
}

/**
 * @brief __fio_set_state_locked - set state, signal
 */
FIO_NONNULL_PARAMS static inline uint32_t __fio_set_state_locked(fio_state_t *s, uint32_t new_state)
{
    uint32_t old_state = s->state;

    if (old_state != new_state)
    {
        s->state = new_state;
        fusion_condvar_broadcast(&s->cv);
    }
    return old_state;
}

/**
 * @brief fio_state_valid - is the state in a valid, defined state, or has it been zeroed
 */
FIO_NONNULL_PARAMS static inline int fio_state_valid(fio_state_t *s)
{
    return s->state != FIO_STATE_INVALID;
}

/**
 * @brief fio_set_state - lock state, set state, signal, unlock
 */
FIO_NONNULL_PARAMS static inline uint32_t fio_set_state(fio_state_t *s, uint32_t new_state)
{
    int old_state;

    fusion_cv_lock_irq(&s->lk);
    old_state = __fio_set_state_locked(s, new_state);
    fusion_cv_unlock_irq(&s->lk);

    return old_state;
}

/**
 * @brief - Do an atomic transition from the allowed states  to new_state
 *
 * @param s          The state variable
 * @param new_state  The state to tranistion to
 * @param nold       The number of allowed from states
 * @param ...        The allowed from states
 */
extern int fio_transition_state(fio_state_t *s, uint32_t new_state, int nold, ...);

/**
 * @brief - Do an atomic transition from the allowed states  to new_state
 *
 * @param s          The state variable
 * @param old_state  The old state, return atomically
 * @param new_state  The state to tranistion to
 * @param nold       The number of allowed from states
 * @param ap         The allowed from states (as a va_list)
 */
extern int fio_transition_state_va(fio_state_t *s, uint32_t *old_state, uint32_t new_state, int nold, va_list ap);

/**
 * @brief __fio_wait_on_state - wait on condition variable
 */
FIO_NONNULL_PARAMS static inline void __fio_wait_on_state(fio_state_t *s, uint32_t wait_on,
                                                          char *name)
{
    // DC-840: Restore when DC no longer explodes
    //dbgprint( DBGS_GENERAL, "Waiting on state transition to %s\n", name );
    fusion_cv_lock_irq(&s->lk);
    while (s->state != wait_on)
    {
        fusion_condvar_wait(&s->cv, &s->lk);
    }
}

FIO_NONNULL_PARAMS static inline void __fio_unlock_state(fio_state_t *s)
{
    fusion_cv_unlock_irq(&s->lk);
}

extern void __fio_wait_on_states(fio_state_t *s, uint32_t num_states,
    char *names, ...);

/**
 * @brief __fio_in_state - lock, compare, unlock
 */
FIO_NONNULL_PARAMS static inline int fio_in_state(fio_state_t *s, uint32_t wait_on)
{
    int ret;

    ret = (s->state == wait_on);

    return ret;
}

FIO_NONNULL_PARAMS static inline int fio_state_in_one_of_2(fio_state_t *s, uint32_t state0, uint32_t state1)
{
    int state = s->state;

    return (state == state0) || (state == state1);
}

FIO_NONNULL_PARAMS static inline int fio_state_in_one_of_3(fio_state_t *s, uint32_t state0, uint32_t state1, uint32_t state2)
{
    int state = s->state;

    return (state == state0) || (state == state1) || (state == state2);
}

FIO_NONNULL_PARAMS static inline int fio_state_in_one_of_4(fio_state_t *s, uint32_t state0, uint32_t state1, uint32_t state2,
                                                           uint32_t state3)
{
    int state = s->state;

    return (state == state0) || (state == state1) || (state == state2) || (state == state3);
}

FIO_NONNULL_PARAMS static inline int fio_state_in_one_of_5(fio_state_t *s, uint32_t state0, uint32_t state1, uint32_t state2,
                                                           uint32_t state3, uint32_t state4)
{
    int state = s->state;

    return (state == state0) || (state == state1) || (state == state2) || (state == state3) || (state == state4);
}

FIO_NONNULL_PARAMS extern int fio_state_in_one_of(fio_state_t *s, uint32_t num_states, ...);

FIO_NONNULL_PARAMS extern int fio_state_not_in_one_of(fio_state_t *s, uint32_t num_states, ...);

#define fio_wait_on_state(s, v)  __fio_wait_on_state(s, v, #v)
#define fio_wait_on_states(s, v, ...)  __fio_wait_on_states(s, v, #__VA_ARGS__, __VA_ARGS__)
#define fio_unlock_state(s)  __fio_unlock_state(s)

#define fio_set_state_verbose(type, s, v)                       \
    do {                                                        \
        infprint(#s " enter: %s\n", STATE_GET_NAMEP(type, s));  \
        fio_set_state(s, v, #v);                                \
        infprint(#s " exit: %s\n", STATE_GET_NAMEP(type, s));   \
    } while(0)

/*---------------------------------------------------------------------------*/

/**
   @brief Move the state to the next higher state (increment)
*/
extern void fio_state_up(fio_state_t *s);

/**
   @brief Move the state to the next lower state (decrement)
*/
extern void fio_state_down(fio_state_t *s);

/*---------------------------------------------------------------------------*/

/// @brief Describes a state transition arc
typedef struct
{
    uint32_t states_from_mask; ///< Bit mask of states from which to transition for this arc
    int16_t  state_to;         ///< State to transition to
    int16_t  arc_num;          ///< Arc number
} fio_state_arc_t;

#define STATE_ARC_NOTFOUND     (-1)
#define FIO_STATE_SAME         (-1)

#define STATE_M1(a)                   (((uint32_t)1) << (a))
#define STATE_M2(a,b)                 (STATE_M1(a) | STATE_M1(b))
#define STATE_M3(a,b,c)               (STATE_M1(a) | STATE_M2(b, c))
#define STATE_M4(a,b,c,d)             (STATE_M1(a) | STATE_M3(b, c, d))
#define STATE_M5(a,b,c,d,e)           (STATE_M1(a) | STATE_M4(b, c, d, e))
#define STATE_M6(a,b,c,d,e,f)         (STATE_M1(a) | STATE_M5(b, c, d, e, f))
#define STATE_M7(a,b,c,d,e,f,g)       (STATE_M1(a) | STATE_M6(b, c, d, e, f, g))
#define STATE_M8(a,b,c,d,e,f,g,h)     (STATE_M1(a) | STATE_M7(b, c, d, e, f, g, h))
#define STATE_M9(a,b,c,d,e,f,g,h,i)   (STATE_M1(a) | STATE_M8(b, c, d, e, f, g, h, i))
#define STATE_MASK_ANY                (((uint32_t)(~0ULL)) ^ 1)

typedef int (*arc_action_t)(int arc_num, void *arg);

extern int fio_state_transition(fio_state_t *s, fio_state_arc_t arcs[],
                                arc_action_t arc_action, void *arg);

#endif // defined(__KERNEL__)

#endif // __FIO_PORT_STATE_H__
