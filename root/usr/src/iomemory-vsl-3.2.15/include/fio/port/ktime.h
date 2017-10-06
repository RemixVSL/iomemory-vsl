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
#if defined(__KERNEL__)
extern void kfio_udelay(unsigned int usecs);
#endif
extern void kfio_msleep(unsigned int millisecs);
extern void kfio_msleep_noload(unsigned int millisecs);

extern uint32_t kfio_get_seconds(void);
extern uint64_t fusion_get_lbolt(void);
extern int fusion_HZ(void);
extern uint64_t fusion_usectohz(uint64_t microsecs);
extern uint64_t fusion_hztousec(uint64_t hertz);

// Milliseconds to microseconds
#define fusion_msectousec(x)  ((x) * 1000LL)

// seconds to milliseconds
#define fusion_sectomsec(x)  ((x) * 1000)

// seconds to microseconds
#define fusion_sectousec(x)  ((x) * 1000000LL)

#ifndef SEC_PER_MIN
#define SEC_PER_MIN    60
#endif

#ifndef MIN_PER_HOUR
#define MIN_PER_HOUR   60
#endif

/* Solaris defines this already.  It *is* hard to guess what 24 stands
for in an equation about time, you know */
#ifndef HOURS_PER_DAY
#define HOURS_PER_DAY  24
#endif

// seconds to days
#define fusion_sectodays(x)  ((x) / (SEC_PER_MIN * MIN_PER_HOUR * HOURS_PER_DAY))
#define fusion_usectodays(x)  ((x) / (1000000LL * SEC_PER_MIN * MIN_PER_HOUR * HOURS_PER_DAY))

// Milliseconds to hz
#define fusion_msectohz(x)   fusion_usectohz((x) * 1000)
// Seconds to hz
#define fusion_sectohz(x)    fusion_usectohz((x) * 1000000)

#define fusion_get_lbolt_usec()   fusion_hztousec(fusion_get_lbolt())

extern void fusion_init_timer(struct fusion_timer_list* timer);
extern void fusion_set_relative_timer(struct fusion_timer_list* timer, uint64_t t);
extern void fusion_add_timer(struct fusion_timer_list* timer);
// extern void fusion_set_timer_expiry(struct fusion_timer_list* timer, unsigned long t);
extern void fusion_set_timer_function(struct fusion_timer_list* timer,
                                      void (*f) (fio_uintptr_t));
extern void fusion_set_timer_data(struct fusion_timer_list* timer, fio_uintptr_t d);
extern void fusion_del_timer(struct fusion_timer_list* timer);
extern uint64_t fusion_getnanotime(void);
extern uint64_t fusion_getmicrotime(void);
extern uint64_t fusion_getwallclocktime(void);

#endif //__FIO_PORT_KTIME_H__
