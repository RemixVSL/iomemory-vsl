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

#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/delay.h>
#ifndef __KERNEL__
#include <unistd.h>
#endif

/**
 * @brief delays in kernel context
 */
void noinline kfio_udelay(unsigned int usecs)
{
    udelay(usecs);
}
#if defined(__linux__)
KFIO_EXPORT_SYMBOL(kfio_udelay);
#endif

/**
 * @brief delays in kernel context
 */
void noinline kfio_msleep(unsigned int millisecs)
{
    msleep(millisecs);
}
KFIO_EXPORT_SYMBOL(kfio_msleep);

void noinline kfio_msleep_noload(unsigned int millisecs)
{
    unsigned long remainder = msleep_interruptible(millisecs);

    /*
     * FH-22522
     *
     * The msleep_interruptible() function does not add to the task
     * cpu usage accounting, but isn't guaranteed to delay for the
     * full duration.  This has been observed during module load
     * time at boot.
     *
     * Because it's unclear how often a reschedule can occur, if the
     * previous sleep completed early, we'll use the sleep with a
     * guaranteed minimum duration instead.  This will add to the
     * tasks observable load, but won't burn any real cpu time.
     */
    if (remainder)
    {
        msleep(remainder);
    }
}
EXPORT_SYMBOL(kfio_msleep_noload);

/**
 * @brief returns number of clock ticks since last system reboot
 */
uint64_t noinline fusion_get_lbolt(void)
{
    return get_jiffies_64();
}
KFIO_EXPORT_SYMBOL(fusion_get_lbolt);

/**
 * @brief returns # ticks per second
 */
int noinline fusion_HZ(void)
{
    return HZ;
}

/**
 * @brief returns number of seconds since last system reboot
 */
uint32_t noinline kfio_get_seconds(void)
{
    return get_seconds();
}

/**
 * @brief returns current time in nanoseconds
 */
uint64_t noinline fusion_getnanotime(void)
{
    return (fusion_get_lbolt() * 1000000000) / fusion_HZ();
}

/**
 * @brief returns current time in microseconds
 */
uint64_t noinline fusion_getmicrotime(void)
{
    return get_jiffies_64() * 1000000 / HZ;
}
KFIO_EXPORT_SYMBOL(fusion_getmicrotime);

/// @brief return current UTC wall clock time in seconds since the Unix epoch (Jan 1 1970).
uint64_t noinline fusion_getwallclocktime(void)
{
    struct timespec ts = current_kernel_time();

    return (uint64_t)ts.tv_sec;
}

/*******************************************************************************
*******************************************************************************/
uint64_t noinline fusion_usectohz(uint64_t u)
{
    if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
        return MAX_JIFFY_OFFSET;
#if HZ <= USEC_PER_SEC && !(USEC_PER_SEC % HZ)
    return kfio_div64_64(u + (USEC_PER_SEC / HZ) - 1, USEC_PER_SEC / HZ);
#elif HZ > USEC_PER_SEC && !(HZ % USEC_PER_SEC)
    return u * (HZ / USEC_PER_SEC);
#else
    return kfio_div64_64(u * HZ + USEC_PER_SEC - 1, USEC_PER_SEC);
#endif
    return (u * HZ / 1000000);
}

/*******************************************************************************
*******************************************************************************/
uint64_t noinline fusion_hztousec(uint64_t hertz)
{
    return (hertz * (1000000 / HZ));
}
KFIO_EXPORT_SYMBOL(fusion_usectohz);

/**
 * support for delayed one shots
 */
void noinline fusion_init_timer(struct fusion_timer_list* timer)
{
    init_timer((struct timer_list *) timer);
}
KFIO_EXPORT_SYMBOL(fusion_init_timer);

/**
 *
 */
void noinline fusion_set_timer_function(struct fusion_timer_list* timer,
    void (*f) (fio_uintptr_t))
{
    ((struct timer_list *) timer)->function = f;
}
KFIO_EXPORT_SYMBOL(fusion_set_timer_function);

/**
 *
 */
void noinline fusion_set_timer_data(struct fusion_timer_list* timer, fio_uintptr_t d)
{
    ((struct timer_list *) timer)->data = d;
}
KFIO_EXPORT_SYMBOL(fusion_set_timer_data);

/**
 * @note t in ticks
 */
void noinline fusion_set_relative_timer(struct fusion_timer_list* timer, uint64_t t)
{
    mod_timer((struct timer_list *) timer, fusion_get_lbolt() + t);
}
KFIO_EXPORT_SYMBOL(fusion_set_relative_timer);
/**
 *
 */
void noinline fusion_del_timer(struct fusion_timer_list* timer)
{
    del_timer_sync((struct timer_list *) timer);
}
KFIO_EXPORT_SYMBOL(fusion_del_timer);
