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

#ifndef _FIO_PORT_KTYPES_32_H
#define _FIO_PORT_KTYPES_32_H


#define ptr_to_u64(ptr) ((uint64_t)((fio_uintptr_t)(ptr)))

#define u64_to_ptr(u64, type) ({                \
            uint64_t _u64 = (u64);              \
            kassert(_u64 < 0xffffffffULL);      \
            (type)((fio_uintptr_t)_u64);        \
        })

#if !defined(INT64_C)
#define INT64_C(c)    c ## LL
#endif
#if !defined(UINT64_C)
#define UINT64_C(c)    c ## ULL
#endif

#if !defined(INTMAX_C)
#define INTMAX_C(c)    c ## LL
#endif
#if !defined(UINTMAX_C)
#define UINTMAX_C(c)    c ## ULL
#endif

#if defined(WINNT) || defined(WIN32)
#define FUSION_ALIGN(x)        __declspec(align(4)) x
#else
#define FUSION_ALIGN(x)        x __attribute__((aligned(4)))
#endif


#endif /* _FIO_PORT_KTYPES_32_H */
