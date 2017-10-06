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

/* FIXME - this should be added to some general include so these are globally available
 * without specifically including this file.
 */

#ifndef __FIO_PORT_COMPILER_H__
#define __FIO_PORT_COMPILER_H__

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/compiler.h>
#elif defined(__linux__) || defined(__VMKLNX__)
# include <fio/port/common-linux/compiler.h>
#elif defined(__SVR4) && defined(__sun)
# include <fio/port/solaris/compiler.h>
#elif defined(__hpux__)
# include <fio/port/hpux/compiler.h>
#elif defined(__FreeBSD__)
# include <fio/port/freebsd/compiler.h>
#elif defined(_AIX)
# include <fio/port/aix/compiler.h>
#elif defined(__OSX__)
# include <fio/port/osx/compiler.h>
#elif defined(WIN32) || defined(WINNT)
# include <fio/port/windows/compiler.h>
#elif defined(UEFI)
# include <fio/port/uefi/compiler.h>
#else
# error Unsupported OS - please define a fio/port/<platform>/compiler.h and include it in fio/port/compiler.h
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 9) || (__GNUC__ > 4) || \
    (defined(__clang__) && __has_attribute(returns_nonnull))
#define FIO_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define FIO_RETURNS_NONNULL
#endif

#if (defined __GNUC__) || (defined(__clang__) && __has_attribute(nonnull))
#define FIO_NONNULL_PARAMS __attribute__((nonnull))
#else
#define FIO_NONNULL_PARAMS
#endif

#endif
