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

#ifndef __FIO_PORT_KTYPES_H__
#define __FIO_PORT_KTYPES_H__

#include <fio/port/port_config.h>
#include <fio/port/align.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config.h>
#endif
#include <fio/port/kglobal.h>

#if !defined(BUILDING_MODULE)
#include <fio/port/stdint.h>
#endif

#ifdef __GNUC__
#if defined(__mips64)
#include <fio/port/arch/mips_atomic.h>
#include <fio/port/arch/mips_cache.h>
#elif defined(__ia64__)
#include <fio/port/arch/ia64_atomic.h>
#include <fio/port/arch/ia64_cache.h>
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
#elif defined(__hpc__) && defined (__ia64__)
#include <fio/port/arch/ia64_atomic.h>
#include <fio/port/arch/ia64_cache.h>
#endif

#if defined(USERSPACE_KERNEL)
#include <fio/port/userspace/ktypes.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/ktypes.h>
#elif defined(__linux__)
#include <fio/port/linux/ktypes.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/bitops.h>
#include <fio/port/solaris/ktypes.h>
#elif defined(__hpux__)
#include <fio/port/bitops.h>
#include <fio/port/hpux/ktypes.h>
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
#elif defined(UEFI)
#include <fio/port/uefi/ktypes.h>
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

/*
 * Shared type for now. We'll likely need accessor functions to iterate
 * CPUs on Windows, so it's a good idea to have a fixed type to use for this.
 */
typedef int kfio_cpu_t;

/**** common, OS-independent includes for abstracted structures and functions ***/
#include <fio/port/kbitops.h>
#include <fio/port/kblock.h>
#include <fio/port/kmisc.h>
#include <fio/port/cdev.h>

#include <fio/port/kcondvar.h>
#include <fio/port/kdebug.h>
#include <fio/port/kmem.h>
#include <fio/port/kchunk.h>
#include <fio/port/kbmp.h>
#include <fio/port/kscatter.h>
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
#ifndef KFIO_UINT64_MAX
#define KFIO_UINT64_MAX    18446744073709551615ULL
#endif

#define MAX_PCI_DEVICES                  2048

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

/// @brief Return uint64 converted to uint32 with kassert on overflow
static inline uint32_t safe_u32(uint64_t v)
{
    kassert(v <= KFIO_UINT_MAX);

    return (uint32_t)v;
}

/// @brief Return uint64 converted to int32 with kassert on overflow
static inline int32_t safe_i32(uint64_t v)
{
    kassert(v <= KFIO_INT_MAX);

    return (int32_t)v;
}

/// Ensure that all memory stores issued prior to this call (in program order) are globally visible.
#ifndef kfio_store_barrier
# define kfio_store_barrier kfio_barrier
#endif

/// Ensure that all memory loads issued prior to this call (in program order) are globally visible.
/// (i.e. any other task which subsequently reads memory will see the same data the caller does,
/// absent any memory writes).
#ifndef kfio_load_barrier
# define kfio_load_barrier kfio_barrier
#endif

#ifndef KFIO_CACHE_LINE_SIZE_BYTES
# define KFIO_CACHE_LINE_SIZE_BYTES 64
#endif

#include <fio/port/byteswap.h>
#include <fio/port/unaligned.h>

#endif /* __FIO_PORT_KTYPES_H__ */
