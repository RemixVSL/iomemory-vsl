
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
