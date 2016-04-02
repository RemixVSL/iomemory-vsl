
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_LINUX_UFIO_H_
# define _FIO_PORT_LINUX_UFIO_H_

# ifndef _FIO_PORT_UFIO_H_
#  error Do not include this file directly - instead include <fio/port/ufio.h>
# endif

# define UFIO_CONTROL_DEVICE_PATH "/dev/"

# define UFIO_KINFO_ROOT "fusion"

# define UFIO_MODPARAMS_DIR "/sys/module/" FIO_DRIVER_NAME "/parameters"

# ifndef _LARGEFILE64_SOURCE
#  define _LARGEFILE64_SOURCE
# endif

# if !defined(__KERNEL__)
#  include <stdint.h>
#  include <stdlib.h>

#  if FUSION_DEBUG

#if defined(__x86_64__) || defined(__i386__)
#   define kassert(x)   do { if(!(x)) { fprintf(stderr, "ASSERT FAILED: %s:%d %s\n", __FILE__, __LINE__, #x); asm("int3"); } } while(0)
#else
#   define kassert(x)   do { if(!(x)) { fprintf(stderr, "ASSERT FAILED: %s:%d %s\n", __FILE__, __LINE__, #x); exit(-1); } } while(0)
#endif
#   define kdebug(x) x

#  else

#   define kassert(x)   do { } while(0)
#   define kdebug(x)    do { } while(0)

#  endif

# endif /* !defined(__KERNEL__) */

#endif /* _FIO_PORT_LINUX_UFIO_H_ */
