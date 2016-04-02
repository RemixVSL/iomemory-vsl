//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
 * define a character stucture large enough to accommodate the platform-specific 
 * stucture of any supported platforms.  The latter is ugly, but the one in use.
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
 * MS Windows => fusion_windows_work_struct (size 8 + sizeof (KDPC) )
 */
typedef struct fusion_work_struct {
    char dummy[216];
}fusion_work_struct_t;

typedef struct fusion_work_struct fusion_dpc_t;
typedef struct fusion_work_struct fusion_delayed_dpc_t;

typedef void (*fusion_work_func_t)(struct fusion_work_struct *ws);

// TBD: The DPC functions need to disappear - no one uses them as a DPC. Usages should
// be mapped over to the work variants below.
extern void fusion_setup_dpc(fusion_dpc_t *wq, fusion_work_func_t func);
extern void fusion_setup_delayed_dpc(fusion_delayed_dpc_t *wq, fusion_work_func_t func);
extern void fusion_schedule_dpc(fusion_dpc_t *work);

extern void fusion_schedule_work(fusion_dpc_t *work);
extern void fusion_schedule_delayed_work(fusion_delayed_dpc_t *work, uint64_t t);

#endif // __FIO_PORT_SCHED_H__
