//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_KTYPES_64_H
#define _FIO_PORT_KTYPES_64_H


#define ptr_to_u64(ptr) ((uint64_t)(ptr))
#define u64_to_ptr(u64) ((void *)(u64))

#ifndef INT64_C
#define INT64_C(c)    c ## L
#define UINT64_C(c)    c ## UL
#endif

#ifndef INTMAX_C
#define INTMAX_C(c)    c ## L
#define UINTMAX_C(c)    c ## UL
#endif

#endif /* _FIO_PORT_KTYPES_64_H */
