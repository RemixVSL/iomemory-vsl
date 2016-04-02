
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

#ifndef __FIO_PORT_LINUX_STDINT_H__
#define __FIO_PORT_LINUX_STDINT_H__

#ifndef _LINUX_TYPES_H

/* Exact integral types.  */

/* Signed.  */

typedef signed char        int8_t;
typedef short int        int16_t;
typedef int            int32_t;
# if defined(__x86_64__) || defined(__PPC64__) || defined(__mips64)
typedef long int        int64_t;

# else
__extension__
typedef long long int        int64_t;

# endif

/* Unsigned.  */
typedef unsigned char        uint8_t;
typedef unsigned short int    uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int        uint32_t;
# define __uint32_t_defined
#endif

#if defined(__x86_64__) || defined(__PPC64__) || defined(__mips64)
typedef unsigned long int    uint64_t;

#else
__extension__
typedef unsigned long long int    uint64_t;

#endif

#endif // _LINUX_TYPES_H

#if defined(__x86_64__) || defined(__PPC64__) || defined(__mips64)
#if !defined PRIi64
#define PRIi64 "ld"
#endif
#if !defined PRIu64
#define PRIu64 "lu"
#endif
#if !defined PRIx64
#define PRIx64 "lx"
#endif

#else
#if !defined PRIi64
#define PRIi64 "lld"
#endif
#if !defined PRIu64
#define PRIu64 "llu"
#endif
#if !defined PRIx64
#define PRIx64 "llx"
#endif

#endif

#endif
