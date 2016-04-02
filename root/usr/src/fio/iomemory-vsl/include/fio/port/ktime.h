//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/** @file include/fio/port/ktime.h 
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OSX v 10.6+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KTIME_H__
#define __FIO_PORT_KTIME_H__


#ifndef kfio_typecheck
#define kfio_typecheck(type,x)                  \
    ({      type __dummy;                       \
        __typeof__(x) __dummy2;                 \
        (void)(&__dummy == &__dummy2);          \
        1;                                      \
    })
#endif

/* git commit:     3b171672831b9633c2ed8fa94805255cd4d5af19
 * kernel version: 2.6.19
 * comments:       These are for use with 64-bit jiffies.  
 *                 The define are taken directly
 *                 from <linux/jiffies.h>.
 */
#ifndef kfio_time_after64
#define kfio_time_after64(a,b)                  \
    (kfio_typecheck(uint64_t, a) &&             \
     kfio_typecheck(uint64_t, b) &&             \
     ((int64_t)(b) - (int64_t)(a) < 0))
#define kfio_time_before64(a,b) kfio_time_after64(b,a)


#define kfio_time_after_eq64(a,b)               \
    (kfio_typecheck(uint64_t, a) &&             \
     kfio_typecheck(uint64_t, b) &&             \
     ((int64_t)(a) - (int64_t)(b) >= 0))
#define kfio_time_before_eq64(a,b) kfio_time_after_eq64(b,a)
#endif /* kfio_time_after64 */

/* convert milliseconds to microseconds */
#define MILLI_SECONDS_TO_MICRO_SECONDS(x) ((uint64_t)x * 1000ULL)

/*
 * Test if *now* is after the given starting time + a period
 * *now* and *then* are in microsecond units
 * *period* is in millisecond units
 */
#define PERIOD_OVER(now, then, period) \
    kfio_time_after64((now), (uint64_t)(then + MILLI_SECONDS_TO_MICRO_SECONDS(period)))

/**
 * @brief wrapper struct around:
 *  Linux - struct timer_list
 *  MS Windows - struct _windows_timer_list (160 bytes)
 *  Solaris - struct _solaris_timer_list
 *  FreeBSD - struct _freebsd_timer_list
 *  OSX     - struct _osx_timer_list
 */
struct fusion_timer_list {
    char dummy[160];
};
#if defined(__KERNEL__) || defined (EXTERNAL_OS)
extern void kfio_udelay(unsigned int usecs);
#endif
extern void kfio_msleep(unsigned int millisecs);

extern uint32_t kfio_get_seconds(void);
extern uint64_t fusion_get_lbolt(void);
extern int fusion_HZ(void);
extern int fusion_usectohz(int64_t microsecs);
extern int64_t fusion_hztousec(int hertz);

// Milliseconds to microseconds
#define fusion_msectousec(x)  ((x) * 1000LL)

// seconds to milliseconds
#define fusion_sectomsec(x)  ((x) * 1000)

// seconds to microseconds
#define fusion_sectousec(x)  ((x) * 1000000LL)

// Milliseconds to hz
#define fusion_msectohz(x)   fusion_usectohz((x) * 1000)
// Seconds to hz
#define fusion_sectohz(x)    fusion_usectohz((x) * 1000000)

#define fusion_get_lbolt_usec()   fusion_hztousec((int32_t)fusion_get_lbolt()) // this is bad

extern void fusion_init_timer(struct fusion_timer_list* timer);
extern void fusion_set_relative_timer(struct fusion_timer_list* timer, uint64_t t);
extern void fusion_add_timer(struct fusion_timer_list* timer);
// extern void fusion_set_timer_expiry(struct fusion_timer_list* timer, unsigned long t);
extern void fusion_set_timer_function(struct fusion_timer_list* timer, 
                                      void (*f) (fio_uintptr_t));
extern void fusion_set_timer_data(struct fusion_timer_list* timer, fio_uintptr_t d);
extern void fusion_del_timer(struct fusion_timer_list* timer);
#if defined (EXTERNAL_OS)
#else
extern uint64_t fusion_getnanotime(void);
extern uint64_t fusion_getmicrotime(void);
#endif
extern uint64_t fusion_getwallclocktime(void);

#endif //__FIO_PORT_KTIME_H__
