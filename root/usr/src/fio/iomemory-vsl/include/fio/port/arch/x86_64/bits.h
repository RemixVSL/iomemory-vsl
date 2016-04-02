
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FUSION_ARCH_BITS_64__
#define __FUSION_ARCH_BITS_64__

/**
 * @brief atomic clear of bit # v of *p.
 */
static inline void fio_clear_bit_64(int v, volatile uint64_t *p)
{
    __asm__ __volatile__("\n\tlock; btrl %1, %0"
                         :"=m"(*p)
                         :"Jr"(v), "m"(*p)
                         : "cc", "memory" );

}

#endif //__FUSION_ARCH_BITS_64__
