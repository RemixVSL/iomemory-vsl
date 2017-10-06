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

/** @file include/fio/port/kglobal.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */


/**
 * @brief global types that are included by both the core and the porting
 * layer code.
 */

#ifndef __FIO_PORT_GLOBAL_H__
#define __FIO_PORT_GLOBAL_H__

typedef long fio_off_t;
typedef long long fio_loff_t;
typedef int fio_mode_t;

#if defined(__linux__)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(__SVR4) && defined(__sun)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(__hpux__)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(__FreeBSD__)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(_AIX)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(__OSX__)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#elif defined(WINNT)|| defined(WIN32)
#if defined(_WIN64)
typedef          long long  fio_intptr_t;
typedef unsigned long long  fio_uintptr_t;
typedef unsigned long long  fio_size_t;
typedef          long long  fio_ssize_t;
#else
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#endif
#elif defined(UEFI)
typedef          long long  fio_intptr_t;
typedef unsigned long long  fio_uintptr_t;
typedef unsigned long long  fio_size_t;
typedef          long long  fio_ssize_t;
#elif defined (USERSPACE_KERNEL)
typedef          long  fio_intptr_t;
typedef unsigned long  fio_uintptr_t;
typedef unsigned long  fio_size_t;
typedef          long  fio_ssize_t;
#else
#error Unsupported OS
#endif


/**
 * @typedef kfio_maa_t
 * @brief Memory Allocation Action describing the strategy that the
 * allocator should use.  This corresponds to gfp_t (Get Free Pages)
 * in Linux.
 */
typedef unsigned  kfio_maa_t;

#if defined(__x86_64__) || defined(_WIN64) || defined(__PPC64__) || defined (__sparc__) || defined(__mips64) || defined(__ia64__) || defined(DMA64_OS32)
typedef unsigned long long fusion_paddr_t;  /* dma_addr_t */
#else
typedef unsigned fusion_paddr_t;
#endif


#endif //__FIO_PORT_GLOBAL_H__
