//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _KFIO_USERSPACE_DIV64_H
#define _KFIO_USERSPACE_DIV64_H

#if FIO_BITS_PER_LONG == 64

static inline uint64_t kfio_div64_64(uint64_t dividend, uint64_t divisor)
{
    return dividend / divisor;        // For 64-bit, can directly return the quotient.
}

static inline uint64_t kfio_mod64_64(uint64_t dividend, uint64_t divisor)
{
    return dividend % divisor;
}

#else

#include <stdlib.h>

static inline uint64_t kfio_div64_64(uint64_t dividend, uint64_t divisor)
{
    lldiv_t l;

    l = lldiv(dividend, divisor);

    return l.quot;
}

static inline uint64_t kfio_mod64_64(uint64_t dividend, uint64_t divisor)
{
    lldiv_t l;

    l = lldiv(dividend, divisor);

    return l.rem;
}

#endif

#endif /* _KFIO_USERSPACE_DIV64_H */
