//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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
//----------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef __FIO_PORT_UNALIGNED_H__
#define __FIO_PORT_UNALIGNED_H__

#if defined(__KERNEL__)
#include <fio/port/kfio.h>
#else
#include <fio/port/ufio.h>
#endif

// Allow HP-UX to override
#if defined(__hpux__) && defined (__ia64__)
#include <fio/port/hpux/unaligned.h>
#endif


// macros for unaligned access
// override these in OS specific variants of this file if your platform needs something special
#ifndef __HAVE_UNALIGNED_ACCESSORS
#define __HAVE_UNALIGNED_ACCESSORS

static inline uint16_t get_unaligned16(const void* ptr)
{
    return *(const uint16_t*)ptr;
}

static inline uint32_t get_unaligned32(const void* ptr)
{
    return *(const uint32_t*)ptr;
}

static inline uint64_t get_unaligned64(const void* ptr)
{
    return *(const uint64_t*)ptr;
}

static inline void put_unaligned16(void* ptr, uint16_t v)
{
    *(uint16_t*)ptr = v;
}

static inline void put_unaligned32(void* ptr, uint32_t v)
{
    *(uint32_t*)ptr = v;
}

static inline void put_unaligned64(void* ptr, uint64_t v)
{
    *(uint64_t*)ptr = v;
}

#endif // __HAVE_UNALIGNED_ACCESSORS

#ifndef __HAVE_UNALIGNED_MACROS
#define __HAVE_UNALIGNED_MACROS

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2

// The most optimized is for GCC in which we do compile time evaluation instead of runtime if/else

extern void unsupported_unaligned_access(void);  // never defined so it will generate a link error

#define get_unaligned(ptr) \
    (__builtin_choose_expr(sizeof(*(ptr)) == 1, *(ptr),                             \
     __builtin_choose_expr(sizeof(*(ptr)) == 2, get_unaligned16((ptr)),             \
     __builtin_choose_expr(sizeof(*(ptr)) == 4, get_unaligned32((ptr)),             \
     __builtin_choose_expr(sizeof(*(ptr)) == 8, get_unaligned64((ptr)),             \
     unsupported_unaligned_access())))))

#define put_unaligned(ptr, v) \
    (__builtin_choose_expr(sizeof(*(ptr)) == 1, ({*(uint8_t*)(ptr) = v;}),          \
     __builtin_choose_expr(sizeof(*(ptr)) == 2, ({put_unaligned16((ptr), v);}),     \
     __builtin_choose_expr(sizeof(*(ptr)) == 4, ({put_unaligned32((ptr), v);}),     \
     __builtin_choose_expr(sizeof(*(ptr)) == 8, ({put_unaligned64((ptr), v);}),     \
     unsupported_unaligned_access())))))

#else

static inline int unsupported_unaligned_access(void)
{
    kassert(0 && "unsupported unaligned access");
    return 0;
}

#define get_unaligned(ptr) \
    ( sizeof(*(ptr)) == 1 ? *(ptr) : \
    ( sizeof(*(ptr)) == 2 ? get_unaligned16((void*)(ptr)) : \
    ( sizeof(*(ptr)) == 4 ? get_unaligned32((void*)(ptr)) : \
    ( sizeof(*(ptr)) == 8 ? get_unaligned64((void*)(ptr)) : \
    ( unsupported_unaligned_access() )))))

#define put_unaligned(ptr,v) ({ \
    void *_ptr = (void*)(ptr);                                                 \
    switch(sizeof(*(ptr))) {                                                    \
        case 1: *(uint8_t*)_ptr = v;                            break;          \
        case 2: put_unaligned16(_ptr, (uint16_t)v);             break;          \
        case 4: put_unaligned32(_ptr, (uint32_t)v);             break;          \
        case 8: put_unaligned64(_ptr, (uint64_t)v);             break;          \
        default: unsupported_unaligned_access();                break;          \
    }})

#endif
#endif // __HAVE_UNALIGNED_MACROS

#endif // __FIO_PORT_UNALIGNED_H__
