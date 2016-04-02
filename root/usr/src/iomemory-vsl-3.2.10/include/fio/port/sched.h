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

/** @file include/fio/port/sched.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * MS Windows 2003 64 bit
 * Apple OSX v 10.6+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_SCHED_H__
#define __FIO_PORT_SCHED_H__
/*******************************************************************************
 * The constraints include not requiring recompilation for different versions
 * (debug/release, linux kernel versions &c.) for our proprietary portion of the
 * code.  Many internal Fusion-io structures include fusion_event_t stuctures as
 * a (non dynamically allocated) member.  We can either create and require using
 * dynamic allocators/deallocators and internally use pointers to structures, or
 * define a character structure large enough to accommodate the platform-specific
 * structure of any supported platforms.  The latter is ugly, but the one in use.
 *
 * A corollary to the above is that OS-specific header files should be included
 * in the porting layer's source files but _not_ in header files accessed by
 * non- porting layer include or source files.
 ******************************************************************************/

/**
 * @struct fusion_work_struct
 * @brief placeholder struct that is large enough for the @e real OS-specific
 * structure:
 * Linux => work_struct (size 32) or delayed_work struct (192 on 2.6.31 debug kernel)
 * Solaris => fusion_solaris_work_struct (size 16)
 * FreeBSD => struct of struct task (32 bytes) & function ptr (8 bytes)
 * MS Windows => fusion_windows_work_struct 2 * (size void *)
 */
typedef struct fusion_work_struct {
    uint64_t dummy[216/sizeof(uint64_t)];
}fusion_work_struct_t;

typedef struct fusion_work_struct fusion_dpc_t;
typedef struct fusion_work_struct fusion_delayed_dpc_t;

typedef void (*fusion_work_func_t)(struct fusion_work_struct *ws);

// TBD: The DPC functions need to disappear - no one uses them as a DPC. Usages should
// be mapped over to the work variants below.
extern void fusion_setup_dpc(fusion_dpc_t *wq, fusion_work_func_t func);
extern void fusion_setup_delayed_dpc(fusion_delayed_dpc_t *wq, fusion_work_func_t func);
extern void fusion_schedule_dpc(fusion_dpc_t *work);

extern void fusion_init_work(fusion_work_struct_t *work, fusion_work_func_t func);
extern void fusion_destroy_work(fusion_work_struct_t *work);
extern void fusion_schedule_work(fusion_work_struct_t *work);
extern void fusion_init_delayed_work(fusion_work_struct_t *work, fusion_work_func_t func);
extern void fusion_destroy_delayed_work(fusion_work_struct_t *work);
extern void fusion_schedule_delayed_work(fusion_work_struct_t *wq, uint64_t sleep_time);

/// @brief assert or otherwise indicate badness if the caller is in a context where sleeping is disallowed (e.g. ISR).
#if defined(__linux__)
extern void fusion_might_sleep(void);
#else
# define fusion_might_sleep()
#endif

#endif // __FIO_PORT_SCHED_H__
