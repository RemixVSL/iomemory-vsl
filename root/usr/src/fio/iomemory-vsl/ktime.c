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
EXPORT_SYMBOL(kfio_udelay);
#endif

/**
 * @brief delays in kernel context
 */
void noinline kfio_msleep(unsigned int millisecs)
{
    msleep(millisecs);
}
EXPORT_SYMBOL(kfio_msleep);

/**
 * @brief returns number of clock ticks since last system reboot
 */
uint64_t noinline fusion_get_lbolt(void)
{
    return get_jiffies_64();
}
EXPORT_SYMBOL(fusion_get_lbolt);

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
EXPORT_SYMBOL(fusion_getmicrotime);

/// @brief return current UTC wall clock time in seconds since the Unix epoch (Jan 1 1970).
uint64_t noinline fusion_getwallclocktime(void)
{
    struct timespec ts = current_kernel_time();

    return (uint64_t)ts.tv_sec;
}

/*******************************************************************************
*******************************************************************************/
int noinline fusion_usectohz (int64_t u)
{
    if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
        return (int) MAX_JIFFY_OFFSET;
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
int64_t noinline fusion_hztousec (int hertz)
{
    return (hertz * (1000000 / HZ));
}
EXPORT_SYMBOL(fusion_usectohz);

/**
 * support for delayed one shots
 */
void noinline fusion_init_timer(struct fusion_timer_list* timer)
{
    init_timer((struct timer_list *) timer);
}
EXPORT_SYMBOL(fusion_init_timer);

/**
 *
 */
void noinline fusion_set_timer_function(struct fusion_timer_list* timer, 
    void (*f) (fio_uintptr_t)) 
{
    ((struct timer_list *) timer)->function = f;    
}
EXPORT_SYMBOL(fusion_set_timer_function);

/**
 *
 */
void noinline fusion_set_timer_data(struct fusion_timer_list* timer, fio_uintptr_t d) 
{
    ((struct timer_list *) timer)->data = d;    
}
EXPORT_SYMBOL(fusion_set_timer_data);

/**
 * @parameter t is in ticks
 */
void noinline fusion_set_relative_timer(struct fusion_timer_list* timer, uint64_t t)
{
    mod_timer((struct timer_list *) timer, fusion_get_lbolt() + t);    
}
EXPORT_SYMBOL(fusion_set_relative_timer);
/**
 *
 */
void noinline fusion_del_timer(struct fusion_timer_list* timer)
{
    del_timer_sync((struct timer_list *) timer);
}
EXPORT_SYMBOL(fusion_del_timer);

