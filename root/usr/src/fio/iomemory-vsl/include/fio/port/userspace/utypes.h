//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_USERSPACE_UTYPES_H_
#define _FIO_PORT_USERSPACE_UTYPES_H_

#ifndef _FIO_PORT_UTYPES_H_
#error Do not include this file directly - instead include <fio/port/utypes.h>
#endif

#include <stdint.h>
#include "commontypes.h"

#if defined(__PPC64__) || defined(__PPC__)
static inline uint64_t fusion_card_to_host64(uint64_t val)
{
    return (((val &           0xffULL)<<56)|
            ((val &             0xff00ULL)<<40)|
            ((val &           0xff0000ULL)<<24)|
            ((val &         0xff000000ULL)<<8) |
            ((val &       0xff00000000ULL)>>8) |
            ((val &     0xff0000000000ULL)>>24)|
            ((val &   0xff000000000000ULL)>>40)|
            ((val & 0xff00000000000000ULL)>>56)); 
}

static inline uint32_t fusion_card_to_host32(uint32_t val)
{
    return (((val &   0xff)<<24) |
            ((val &     0xff00)<<8)  |
            ((val &   0xff0000)>>8)  |
            ((val & 0xff000000)>>24)); 
}

static inline uint16_t fusion_card_to_host16(uint16_t val)
{
    return (((val &   0xff)<<8) |
            ((val &     0xff00)>>8)); 
}
#else
#define fusion_card_to_host64(x) (x)
#define fusion_card_to_host32(x) (x)
#define fusion_card_to_host16(x) (x)
#endif

#define host_to_fusion_card64(x) fusion_card_to_host64(x)
#define host_to_fusion_card32(x) fusion_card_to_host32(x)
#define host_to_fusion_card16(x) fusion_card_to_host16(x)

#if 0

#define MD_SETTER_NOCHK(field,p,v,ww)   (p)->field = host_to_fusion_card##ww(v);
#define MD_SETTER(field,p,v,bits,ww)            \
    do{                                         \
        kassert((uint64_t)v<(1ULL<<(bits)));    \
        MD_SETTER_NOCHK(field,p,v,ww);            \
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

#endif //0

#endif  // _FIO_PORT_USERSPACE_UTYPES_H_
