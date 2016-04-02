//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_KTYPES_H__
#define __FIO_PORT_KTYPES_H__

#include <fio/port/config.h>
#include <fio/port/kglobal.h>

#if !defined(BUILDING_MODULE)
#include <fio/port/stdint.h>
#endif

#ifdef __GNUC__
#if defined(__mips64)
#include <fio/port/arch/mips_atomic.h>
#include <fio/port/arch/mips_cache.h>
#elif defined(_AIX)
#include <fio/port/arch/ppc/ppc_atomic.h>
#include <fio/port/arch/ppc/ppc_cache.h>
#elif defined(__PPC__) || defined(__PPC64__)
#include <fio/port/arch/ppc_atomic.h>
#include <fio/port/arch/ppc_cache.h>
#elif defined(__sparc__)
#include <fio/port/arch/sparc_cache.h>
#elif !defined (__SVR4) || !defined(__sun)
#include <fio/port/arch/x86_atomic.h>
#include <fio/port/arch/x86_cache.h>
#endif
#endif

#if defined(USERSPACE_KERNEL)
#include <fio/port/userspace/ktypes.h>
#elif defined(__ESXI41__)
#include <fio/port/esxi41/ktypes.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/ktypes.h>
#elif defined(__linux__)
#include <fio/port/linux/ktypes.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/bitops.h>
#include <fio/port/solaris/ktypes.h>
#elif defined(__FreeBSD__)
#include <fio/port/bitops.h>
#include <fio/port/freebsd/ktypes.h>
#elif defined(_AIX)
#include <fio/port/aix/ktypes.h>
#elif defined(__OSX__)
#include <fio/port/bitops.h>
#include <fio/port/osx/ktypes.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/ktypes.h>
#elif defined (EXTERNAL_OS)
#include "external_os_ktypes.h"
#else
#error "Unsupported OS - if you are porting, try starting with a copy of stubs"
#endif

#ifndef FIO_WORD_SIZE
#error fio/port/<platform>/ktypes.h must define FIO_WORD_SIZE
#endif

#if FIO_WORD_SIZE == 8
#include <fio/port/ktypes_64.h>
#elif FIO_WORD_SIZE == 4
#include <fio/port/ktypes_32.h>
#else
#error Unsupported wordsize!
#endif

/**** common, OS-independent includes for abstracted structures and functions ***/
#if !defined(EXTERNAL_OS)
#include <fio/port/kbitops.h>
#include <fio/port/kblock.h>
#include <fio/port/kmisc.h>
#include <fio/port/cdev.h>
#endif 

#include <fio/port/kcondvar.h>
#include <fio/port/kdebug.h>
#include <fio/port/kmem.h>
#include <fio/port/kchunk.h>
#include <fio/port/kbmp.h>
#include <fio/port/kscatter.h>
#include <fio/port/ksimple_event.h>
#include <fio/port/ktime.h>
#include <fio/port/sched.h>
#include <fio/port/kinfo.h>
#include <fio/port/kcache.h>
#include <fio/port/kbio.h>

#ifndef KFIO_INT_MAX
#define KFIO_INT_MAX       2147483647 
#endif
#ifndef KFIO_UINT_MAX
#define KFIO_UINT_MAX      4294967295U
#endif

#define MAX_PCI_DEVICES                   384

#ifndef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 0x1234
#endif
#ifndef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN    0x4321
#endif
#ifndef FUSION_PDP_ENDIAN
#define FUSION_PDP_ENDIAN    0x3412
#endif

#if defined(__PPC__) || defined(__PPC64__) || defined(__sparc__) || defined(__mips64)
#define FUSION_BYTE_ORDER FUSION_BIG_ENDIAN
#else
#define FUSION_BYTE_ORDER FUSION_LITTLE_ENDIAN
#endif

#if defined(__GNUC__)
#ifndef likely
#define likely(x)       __builtin_expect(!!(x),1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect(!!(x),0)
#endif
#else
#ifndef likely
#define likely(x) (x)
#endif
#ifndef unlikely
#define unlikely(x) (x)
#endif
#endif

#ifndef __user
#define __user
#endif
#ifndef __iomem
#define __iomem
#endif

#endif /* __FIO_PORT_KTYPES_H__ */
