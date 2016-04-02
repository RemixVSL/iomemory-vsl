//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/types.h>
#include <fio/port/ktypes.h>
#include <asm/atomic.h>

int noinline kfio_atomic_sub(int val, volatile int *p)
{
    return atomic_sub_return(val, (atomic_t *)p);
}

int noinline kfio_atomic_add(int val, volatile int *p)
{
    return atomic_add_return(val, (atomic_t *)p);
}
