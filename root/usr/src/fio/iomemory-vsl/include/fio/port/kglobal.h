//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
#elif defined (EXTERNAL_OS)
#include "fio/port/external_os_kglobal.h"
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

#if defined(__x86_64__) || defined(_WIN64) || defined(__PPC64__) || defined (__sparc__) || defined(__mips64) || defined(DMA64_OS32)
typedef unsigned long long fusion_paddr_t;  /* dma_addr_t */
#else
typedef unsigned fusion_paddr_t;
#endif


#endif //__FIO_PORT_GLOBAL_H__
