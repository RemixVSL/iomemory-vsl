
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FUSION_ARCH_BITS__
#define __FUSION_ARCH_BITS__

/**
 * @brief: returns 1 iff bit # v is set in *p else returns 0.
 */
static inline int fio_test_bit(int v, const volatile unsigned *p)
{
    return ((1U << (v & 31)) & *p) != 0;
}

/**
 * @brief atomic set of bit # v of *p.
 */
static inline void fio_set_bit(int v, volatile unsigned *p)
{
    __asm__ __volatile__("\n\tlock; btsl %1, %0"
                         :"=m"(*p)
                         :"Ir"(v), "m"(*p)
                         : "cc", "memory" );

}

/**
 * @brief atomic clear of bit # v of *p.
 */
static inline void fio_clear_bit(int v, volatile unsigned *p)
{
    __asm__ __volatile__("\n\tlock; btrl %1, %0"
                         :"=m"(*p)
                         :"Ir"(v), "m"(*p)
                         : "cc", "memory" );

}

/**
 * @brief atomic set of bit # v of *p, returns old value.
 */
static inline unsigned char fio_test_and_set_bit(int v, volatile unsigned *p)
{
    unsigned char bit_set;

    __asm__ __volatile__("\n\tlock; btsl %2, %0"
                         "\n\tsetb %1"
                         :"=m"(*p), "=r"(bit_set)
                         :"Ir"(v), "m"(*p)
                         : "cc", "memory" );
    return bit_set;
}

/**
 * @brief atomic clear of bit # v of *p, returns old value.
 */
static inline unsigned char fio_test_and_clear_bit(int v, volatile unsigned *p)
{
    unsigned char bit_set;

    __asm__ __volatile__("\n\tlock; btrl %2, %0"
                         "\n\tsetb %1"
                         :"=m"(*p), "=r"(bit_set)
                         :"Ir"(v), "m"(*p)
                         : "cc", "memory" );
    return bit_set;
}
#endif //__FUSION_ARCH_BITS__
