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

//
// This file is shared between kernel and user source files.  All items shared
// between the two environments are included in here...
//

#ifndef __FUSION_LINUX_COMMONTYPES_H__
#define __FUSION_LINUX_COMMONTYPES_H__

#include <stddef.h>

#define ATTRIBUTE_ALIGNED(x)                    \
    __attribute__((aligned(x)))

#ifdef __cplusplus
// c/c++ linkage errors eliminated
#define C_ASSERT(x) extern "C" int __CPP_ASSERT__ [(x)?1:-1]
#else
#define C_ASSERT(x) extern int __C_ASSERT__ [(x)?1:-1]
#endif

#define UFIO_CONTROL_DEVICE_PATH       "/dev/"
#define UFIO_CONTROL_DEVICE_COMMON_PATH UFIO_CONTROL_DEVICE_PATH
#define UFIO_CONTROL_DEVICE_PREFIX     "fct"
#if (KFIO_SCSI_DEVICE==1)
#define UFIO_BLOCK_DEVICE_PREFIX             "scsi"
#else
#define UFIO_BLOCK_DEVICE_PREFIX             "fio"
#endif
#define UFIO_CONTROL_DEVICE_BASE       UFIO_CONTROL_DEVICE_PATH UFIO_CONTROL_DEVICE_PREFIX

// Add 10 to ensure that we have sufficient padding in the device name
// which will either be "fct" or "fio" plus the identifier.
#define UFIO_DEVICE_FILE_MAX_LEN       sizeof(UFIO_CONTROL_DEVICE_PATH) + 10

#endif // __FUSION_LINUX_COMMONTYPES_H__

