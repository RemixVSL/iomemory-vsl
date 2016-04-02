//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_KTYPES_32_H
#define _FIO_PORT_KTYPES_32_H


#define ptr_to_u64(ptr) ((uint64_t)((fio_uintptr_t)(ptr)))

#define u64_to_ptr(u64, type) ({                \
            uint64_t _u64 = (u64);                \
            kassert(_u64 < 0xffffffffULL);        \
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


#endif /* _FIO_PORT_KTYPES_32_H */
