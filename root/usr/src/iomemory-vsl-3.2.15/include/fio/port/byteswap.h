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

#ifndef __FIO_PORT_BYTESWAP_H__
#define __FIO_PORT_BYTESWAP_H__

#ifndef FUSION_LITTLE_ENDIAN
#define FUSION_LITTLE_ENDIAN 0x1234
#endif
#ifndef FUSION_BIG_ENDIAN
#define FUSION_BIG_ENDIAN    0x4321
#endif
#ifndef FUSION_PDP_ENDIAN
#define FUSION_PDP_ENDIAN    0x3412
#endif

#if defined(__PPC__) || defined(__PPC64__) || defined(__sparc__) \
    || ((defined(__mips__) && defined(__MIPSEB__))) \
    || ((defined(__hpux__) && defined(__ia64__)))
#define FUSION_BYTE_ORDER FUSION_BIG_ENDIAN
#else
#define FUSION_BYTE_ORDER FUSION_LITTLE_ENDIAN
#endif

#include <fio/port/stdint.h>
#include <fio/port/commontypes.h>

// Include platform specific byteswap
#if defined(__hpux__) && defined(__ia64__)
#include "fio/port/hpux/byteswap.h"
#endif

/* Generic byteswap implementation. */
static inline uint64_t fusion_bswap64(uint64_t val)
{
    uint64_t tmp;
    tmp = ((val << 8) & 0xff00ff00ff00ff00ULL) | ((val >> 8) & 0x00ff00ff00ff00ffULL);
    tmp = ((tmp << 16) & 0xffff0000ffff0000ULL) | ((tmp >> 16) & 0x0000ffff0000ffffULL);
    return (((tmp << 32) & 0xffffffff00000000ULL) | ((tmp >> 32) & 0x00000000ffffffffULL));
}

static inline uint32_t fusion_bswap32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);

    return (val << 16) | (val >> 16);
}

static inline uint16_t fusion_bswap16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}

#if defined(htole64)
#define FUSION_HAS_PLATFORM_BYTESWAP
#endif

#if !defined(FUSION_HAS_PLATFORM_BYTESWAP)
#if (FUSION_BYTE_ORDER == FUSION_LITTLE_ENDIAN)

#define le64toh(x) ((uint64_t)(x))
#define htole64(x) ((uint64_t)(x))
#define le32toh(x) ((uint32_t)(x))
#define htole32(x) ((uint32_t)(x))
#define le16toh(x) ((uint16_t)(x))
#define htole16(x) ((uint16_t)(x))

#define be64toh(x) fusion_bswap64(x)
#define htobe64(x) fusion_bswap64(x)
#define be32toh(x) fusion_bswap32(x)
#define htobe32(x) fusion_bswap32(x)
#define be16toh(x) fusion_bswap16(x)
#define htobe16(x) fusion_bswap16(x)

#elif (FUSION_BYTE_ORDER == FUSION_BIG_ENDIAN)

#define le64toh(x) fusion_bswap64(x)
#define htole64(x) fusion_bswap64(x)
#define le32toh(x) fusion_bswap32(x)
#define htole32(x) fusion_bswap32(x)
#define le16toh(x) fusion_bswap16(x)
#define htole16(x) fusion_bswap16(x)

#define be64toh(x) ((uint64_t)(x))
#define htobe64(x) ((uint64_t)(x))
#define be32toh(x) ((uint32_t)(x))
#define htobe32(x) ((uint32_t)(x))
#define be16toh(x) ((uint16_t)(x))
#define htobe16(x) ((uint16_t)(x))

#else

#error "Endianness macro not defined"

#endif
#endif

#define fusion_card_to_host64(x) (le64toh(x))
#define host_to_fusion_card64(x) (htole64(x))
#define fusion_card_to_host32(x) (le32toh(x))
#define host_to_fusion_card32(x) (htole32(x))
#define fusion_card_to_host16(x) (le16toh(x))
#define host_to_fusion_card16(x) (htole16(x))

#define MD_SETTER_NOCHK(field,p,v,ww)   (p)->field = host_to_fusion_card##ww(v)
#define MD_SETTER(field,p,v,bits,ww)            \
    do{                                         \
        kassert((uint64_t)v<(1ULL<<(bits)));    \
        MD_SETTER_NOCHK(field,p,v,ww);          \
    } while(0)
#define MD_GETTER(field,p,t,ww)         ((t)fusion_card_to_host##ww((p)->field))

#define NMASK(n) ((1ULL<<(n))-1)

#define MD_SETTER_BF(field,p,v,bits,ww,off,len)         \
    do{                                                 \
        uint64_t val = MD_GETTER(field,p,uint64_t,ww);  \
        kassert((uint64_t)v<(1ULL<<(bits)));            \
        val &= ~(NMASK(len) << (off));                  \
        val |= ((uint64_t)(v) << (off));                \
        MD_SETTER_NOCHK(field,p,val,ww);                \
    } while(0)
#define MD_GETTER_BF(field,p,t,ww,off,len)                          \
    (t)((MD_GETTER(field,p,uint64_t,ww) >> (off)) & (NMASK(len)))

#endif
