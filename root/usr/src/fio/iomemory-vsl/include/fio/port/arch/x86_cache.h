
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_X86_CACHE__
#define __FIO_PORT_X86_CACHE__

#define FUSION_CACHE_LINE_SIZE  64
#define FUSION_HAS_PREFETCH      1

#ifdef __GNUC__

static inline void fusion_prefetch_t0(void *addr){
    __asm__ __volatile__("\n\tprefetcht0 %0" 
                         :
                         : "m" (addr));
}

static inline void fusion_prefetch_nta(void *addr){
    __asm__ __volatile__("\n\tprefetchnta %0" 
                         :
                         : "m" (addr));
}


static inline void fusion_prefetch_page(void *addr)
{
    __asm__ __volatile__(
        "    prefetcht0 0*64(%0)\n" 
        :
        : "r" (addr));
}

#else  /* __GNUC__ */

/* 
   The following functions should be provided for different compilers
   WINNT in particular.  Until then, they are going to do nothing.
*/

static inline void fusion_prefetch_t0(void *addr)
{
}

static inline void fusion_prefetch_nta(void *addr)
{
}

static inline void fusion_prefetch_page(void *addr)
{
}

#endif /* __GNUC__ */

#endif /* __FIO_PORT_X86_CACHE__ */
