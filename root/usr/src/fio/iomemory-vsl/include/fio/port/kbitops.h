//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/** @file include/fio/port/kbitops.h 
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OS X v 10.6+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KBITOPS_H__
#define __FIO_PORT_KBITOPS_H__
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

extern int           kfio_test_bit          (int v, const volatile fusion_bits_t *p);
extern void          kfio_set_bit           (int v, volatile fusion_bits_t *p);
extern void          kfio_clear_bit         (int v, volatile fusion_bits_t *p);
extern unsigned char kfio_test_and_set_bit  (int v, volatile fusion_bits_t *p);
extern unsigned char kfio_test_and_clear_bit(int v, volatile fusion_bits_t *p);

#endif  /* __FIO_PORT_KBITOPS_H__ */
