//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_BITOPS_H__
#define __FIO_PORT_BITOPS_H__

#if defined(__linux__) && !defined(__PPC64__) && !defined(__PPC__) && !defined(__mips64)
# include <fio/port/arch/bits.h>
#if defined(__x86_64__)
# include <fio/port/arch/x86_64/bits.h>
#endif
#elif defined(__PPC64__) || defined(__PPC__) || defined(__mips64)
#define fio_test_bit           kfio_test_bit
#define fio_set_bit            kfio_set_bit
#define fio_clear_bit          kfio_clear_bit
#define fio_test_and_set_bit   kfio_test_and_set_bit
#define fio_test_and_clear_bit kfio_test_and_clear_bit
#elif defined(__SVR4) && defined(__sun)
#if defined(__x86_64__)
#include <fio/port/arch/bits.h>
#include <fio/port/arch/x86_64/bits.h>
#include <fio/port/arch/x86_atomic.h>
#include <fio/port/arch/x86_cache.h>
#elif defined (__i386__)
#include <fio/port/arch/bits.h>
#include <fio/port/arch/i386/bits.h>
#include <fio/port/arch/x86_atomic.h>
#include <fio/port/arch/x86_cache.h>
#elif defined(__sparc__)
#define fio_test_bit           kfio_test_bit
#define fio_set_bit            kfio_set_bit
#define fio_clear_bit          kfio_clear_bit
#define fio_test_and_set_bit   kfio_test_and_set_bit
#define fio_test_and_clear_bit kfio_test_and_clear_bit

extern void fusion_atomic_set(volatile int *x, int y);
extern int  fusion_atomic_read(volatile int *x);

extern void fusion_atomic_inc(volatile int *x);
extern int  fusion_atomic_incr(volatile int *x);
extern void fusion_atomic_dec(volatile int *x);
extern int  fusion_atomic_decr(volatile int *x);

extern void fusion_atomic_add(int val, volatile int *atomp);
extern int  fusion_atomic_add_return(int val, volatile int *atomp);
extern void fusion_atomic_sub(int val, volatile int *atomp);
extern int  fusion_atomic_sub_return(int val, volatile int *atomp);
extern int  fusion_atomic_exchange(volatile int *atomp, volatile int val);
#else
#error Unknown Solaris Architecture
#endif

#elif defined(__FreeBSD__)
#include <fio/port/arch/bits.h>
#include <fio/port/arch/x86_64/bits.h>
#include <fio/port/arch/x86_atomic.h>

#elif defined(_AIX)
#include <fio/port/arch/ppc/ppc_bits.h>

#elif defined(__OSX__)
#include <fio/port/arch/bits.h>
#include <fio/port/arch/x86_64/bits.h>
#include <fio/port/arch/x86_atomic.h>

#elif defined(WIN32) || defined(WINNT)
//#define fio_test_bit           test_bit
//#define fio_set_bit            set_bit
//#define fio_clear_bit          clear_bit
//#define fio_test_and_set_bit   test_and_set_bit
//#define fio_test_and_clear_bit test_and_clear_bit

#elif defined(USERSPACE_KERNEL)
# if  defined(__x86_64__)
#  include <fio/port/arch/bits.h>
#  include <fio/port/arch/x86_64/bits.h>
# else
#  error USERSPACE_KERNEL only supports x86_64 bitops.
# endif

#else
# error You must define bit operations for your platform.

#endif

#endif /* __FIO_PORT_BITOPS_H__  */
