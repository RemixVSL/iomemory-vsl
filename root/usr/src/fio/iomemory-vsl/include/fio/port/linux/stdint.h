
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
