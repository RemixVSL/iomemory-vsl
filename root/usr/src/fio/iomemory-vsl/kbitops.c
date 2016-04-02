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
#include <linux/bitops.h>

int noinline kfio_test_bit(int v, const volatile fusion_bits_t *p)
{
    return test_bit(v, p);
}

void noinline kfio_set_bit(int v, volatile fusion_bits_t *p)
{
    set_bit(v, p);
}

void noinline kfio_clear_bit(int v, volatile fusion_bits_t *p)
{
    clear_bit(v, p);
}

unsigned char noinline kfio_test_and_set_bit(int v, volatile fusion_bits_t *p)
{
    return test_and_set_bit(v, p);
}

unsigned char noinline kfio_test_and_clear_bit(int v, volatile fusion_bits_t *p)
{
    return test_and_clear_bit(v, p);
}

