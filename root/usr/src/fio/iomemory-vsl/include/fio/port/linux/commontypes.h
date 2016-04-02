
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

//
// This file is shared between kernel and user source files.  All items shared
// between the two environments are included in here...
//

#ifndef __FUSION_LINUX_COMMONTYPES_H__
#define __FUSION_LINUX_COMMONTTYPES_H__

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
#define UFIO_CONTROL_DEVICE_PREFIX     "fct"
#define UFIO_BLOCK_DEVICE_PREFIX       "fio"
#define UFIO_CONTROL_DEVICE_BASE       UFIO_CONTROL_DEVICE_PATH UFIO_CONTROL_DEVICE_PREFIX

// Add 10 to ensure that we have sufficient padding in the device name
// which will either be "fct" or "fio" plus the identifier.
#define UFIO_DEVICE_FILE_MAX_LEN       sizeof(UFIO_CONTROL_DEVICE_PATH) + 10 

#endif // __FUSION_LINUX_COMMONTYPES_H__

