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

#if defined (__linux__)
#include <linux/types.h>
#endif

#include <fio/port/state.h>
#include <fio/port/dbgset.h>

/**
 * @brief __fio_state_in_one_of_locked -  compare -- lock must be held by caller
 */
static int __fio_state_in_one_of(fio_state_t *s, uint32_t num_states, va_list ap)
{
    va_list aq;
    int i;
    int cur_state = s->state;

    va_copy(aq, ap);

    for(i = 0; i < num_states; i++)
    {
        uint32_t state = va_arg(aq, uint32_t);

        if (cur_state == state)
        {
            va_end(aq);
            return 1;
        }
    }

    va_end(aq);

    return 0;
}

/**
 * @brief fio_state_in_one_of - lock, compare, unlock
 */
int fio_state_in_one_of(fio_state_t *s, uint32_t num_states, ...)
{
    int retval;
    va_list ap;

    va_start(ap, num_states);

    // lock around so all flags are synchronized
    fusion_cv_lock_irq(&s->lk);
    retval = __fio_state_in_one_of(s, num_states, ap);
    fusion_cv_unlock_irq(&s->lk);

    va_end(ap);

    return retval;
}

/**
 * @brief fio_state_in_one_of - lock, compare, unlock
 */
int fio_state_not_in_one_of(fio_state_t *s, uint32_t num_states, ...)
{
    int cur_state = s->state;
    int retval = 1;
    int i;
    va_list ap;

    va_start(ap, num_states);

    for (i = 0; i < num_states; i++)
    {
        uint32_t state = va_arg(ap, uint32_t);

        if (cur_state == state)
        {
            retval = 0;
            goto exit;
        }
    }

exit:
    va_end(ap);

    return retval;
}

/*
  Does an atomic state transition

  @param s          the state variable to change
  @param new_state  the state to transition into
  @param old_state  The old state we were in (old_state != NULL)
  @param nold       the number of old states specified
                    if this parameter is zero, will tranistion from any state
                    except new_state
  @param ap         the allowed states to transition from as va_list

  @returns          0 on no transition
                    1 on transition
*/
int fio_transition_state_va(fio_state_t *s, uint32_t *old_state, uint32_t new_state, int nold, va_list ap)
{
    int retval = 0;

    fusion_cv_lock_irq(&s->lk);

    if (old_state)
    {
        *old_state = s->state;
    }

    if (nold == 0)
    {
        if (s->state == new_state)
        {
            goto exit;
        }
    }
    else
    {
        if (__fio_state_in_one_of(s, (unsigned) nold, ap) != 1)
        {
            goto exit;
        }
    }

    __fio_set_state_locked(s, new_state);

    retval = 1;

exit:
    fusion_cv_unlock_irq(&s->lk);

    return retval;
}

/*
  Does an atomic state transition

  @param s          the state variable to change
  @param new_state  the state to transition into
  @param nold       the number of old states specified
                    if this parameter is zero, will tranistion from any state
                    except new_state
  @param ...        the allowed states to transition from

  @returns          0 on no transition
                    1 on transition
*/
int fio_transition_state(fio_state_t *s, uint32_t new_state, int nold, ...)
{
    int retval;
    va_list ap;

    va_start(ap, nold);

    retval = fio_transition_state_va(s, NULL, new_state, nold, ap);

    va_end(ap);

    return retval;
}

void __fio_wait_on_states(fio_state_t *s, uint32_t num_states, char *names, ...)
{
    va_list ap;

    va_start(ap, names);

    dbgprint(DBGS_GENERAL, "Waiting on state transition to (%s)\n", names);
    fusion_cv_lock_irq(&s->lk);
    while (!__fio_state_in_one_of(s, num_states, ap) )
    {
        fusion_condvar_wait(&s->cv, &s->lk);
    }
    //unlock done by fio_unlock_state
    dbgprint(DBGS_GENERAL, "Completed waiting on state transition to (%s)\n", names);

    va_end(ap);
}

/*---------------------------------------------------------------------------*/

/// @brief Move the state to the next higher state (increment)
void fio_state_up(fio_state_t *s)
{
    fusion_cv_lock_irq(&s->lk);
    __fio_set_state_locked(s, s->state + 1);
    fusion_cv_unlock_irq(&s->lk);
}

/// @brief Move the state to the next lower state (decrement)
void fio_state_down(fio_state_t *s)
{
    fusion_cv_lock_irq(&s->lk);
    __fio_set_state_locked(s, s->state - 1);
    fusion_cv_unlock_irq(&s->lk);
}

/*---------------------------------------------------------------------------*/

/// @brief Execute a state transition per possible transitions described in an arc table
/// @param s           State variable
/// @param arcs        Structure of arc descriptions
/// @param arc_action  Optional callback to handle arcs inside of lock state
/// @param arg         Parameter for callback
/// @return            This is the arc_num from the structure _or_ the result of processing

int fio_state_transition(fio_state_t *s, fio_state_arc_t arcs[],
                         arc_action_t arc_action, void *arg)
{
    int              arc_num = STATE_ARC_NOTFOUND;
    fio_state_arc_t *arc;
    uint32_t         state_mask;

    fusion_cv_lock_irq(&s->lk);

    // Find the arc for the current state
    for (arc = &arcs[0], state_mask = STATE_M1(s->state);
         arc->arc_num != STATE_ARC_NOTFOUND;
         arc++)
    {
        if ((state_mask & arc->states_from_mask) != 0)
        {
            arc_num = arc->arc_num;
            if (arc->state_to != FIO_STATE_SAME)
            {
                __fio_set_state_locked(s, (uint32_t)arc->state_to);
            }
            break;
        }
    }

    // If there is a handler to run, run it, optionally changing the meaning of the return
    if (arc_action)
    {
        arc_num = (*arc_action)(arc_num, arg);
    }

    fusion_cv_unlock_irq(&s->lk);

    return arc_num;
}

/*---------------------------------------------------------------------------*/
