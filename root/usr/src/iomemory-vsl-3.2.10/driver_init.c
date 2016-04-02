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
// -----------------------------------------------------------------------------
#include <linux/module.h>

#include <fio/port/kfio_config.h>

#include "fio/port/kfio.h"

#define AUTO_ATTACH_DEFAULT 1

// Global driver options
char       *numa_node_override[MAX_PCI_DEVICES];
int         num_numa_node_override;
extern int  numa_node_forced_local;

KFIO_MODULE_PARAM(numa_node_forced_local, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(numa_node_forced_local, "Only schedule fio-wq completion threads for use on NUMA node local to fct-worker");

#if PORT_SUPPORTS_NUMA_NODE_OVERRIDE
# if !defined(__VMKLNX__)
KFIO_MODULE_PARAM_DESC(numa_node_override, "Override device to NUMA node binding");
#  if KFIOC_MODULE_PARAM_ARRAY_NUMP
KFIO_MODULE_PARAM_ARRAY(numa_node_override, charp, &num_numa_node_override, S_IRUGO | S_IWUSR);
#  else
KFIO_MODULE_PARAM_ARRAY(numa_node_override, charp, num_numa_node_override, S_IRUGO | S_IWUSR);
#  endif
# endif
#endif

#if defined(__VMKLNX__)
int use_workqueue = USE_QUEUE_RQ;
#else
int use_workqueue = USE_QUEUE_NONE;
#endif

KFIO_MODULE_PARAM(use_workqueue, int, S_IRUGO | S_IWUSR);

