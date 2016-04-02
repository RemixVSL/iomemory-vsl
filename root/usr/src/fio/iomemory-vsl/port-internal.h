//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
/** @file /src/kernel/fio-port/port-internal.h
 * OS-specific includes and references for unse in the porting layer only.
 * Do not include this file directly or indirectly in the driver
 *
 */

#ifdef FUSION_CORE
#error Only include this file when building the porting layer to avoid kernel-specific contamination.
#endif

#if !defined (__linux__)
#error This file supports linux only
#endif

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

#include <fio/port/config.h>


#include <fio/port/linux/kassert.h>
#include <fio/port/linux/commontypes.h>
#include <fio/port/kfio_config.h> 
#include <linux/blkdev.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#if !defined(__VMKLNX__)
  #include <linux/file.h>
#endif
#include <linux/notifier.h>
#include <linux/pci.h>
#if KFIOC_HAS_LINUX_SCATTERLIST_H
  #include <linux/scatterlist.h>
#endif
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <stdbool.h>

#include <fio/port/ktypes.h>
#include <fio/port/kfio.h>

