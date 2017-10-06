//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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
/** @file
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
 * define a character structure large enough to accommodate the platform-specific
 * structure of any supported platforms.  The latter is ugly, but the one in use.
 *
 * A corollary to the above is that OS-specific header files should be included
 * in the porting layer's source files but _not_ in header files accessed by
 * non- porting layer include or source files.
 ******************************************************************************/

#include <fio/port/port_config.h>


#include <fio/port/common-linux/kassert.h>
#include <fio/port/common-linux/commontypes.h>
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

