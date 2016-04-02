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

/*
 * ufio.h contains interface information between kernel and user space.
 * Consequently information in ufio.h (or the platform-specific ufio.h)
 * must be cautious about anything that must only be in either the kernel
 * or user space.  Handling such situations is done by checking whether the
 * '__KERNEL__' macro is defined (or not).
 */

#ifndef _FIO_PORT_UFIO_H_
#define _FIO_PORT_UFIO_H_

#include <fio/port/port_config.h>

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/ufio.h>
#elif defined(__linux__) || defined(__VMKLNX__)
#include <fio/port/common-linux/ufio.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/solaris/ufio.h>
#elif defined(__hpux__)
#include <fio/port/hpux/ufio.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/ufio.h>
#elif defined(_AIX)
#include <fio/port/aix/ufio.h>
#elif defined(__OSX__)
#include <fio/port/osx/ufio.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/ufio.h>
#elif defined(UEFI)
#include <fio/port/uefi/ufio.h>
#else
#error Unsupported OS
#endif

/* Non-platform specific */

#if !defined(__KERNEL__)
#include <fio/port/utypes.h>
#include <fio/port/stdint.h>
#include <fio/port/byteswap.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* If you're gonna be dumb you gotta be tough... */
#define kfio_malloc(s)        malloc(s)
#define kfio_free(p, s)       do { free(p); (void)s; } while(0)
#define kfio_memcpy(x, y, z)  memcpy(x, y, z)
#define kfio_memset(x, y, z)  memset(x, y, z)
#define kfio_memcmp(x, y, z)  memcmp(x, y, z)
#define kfio_memmove(x, y, z) memmove(x, y, z)
#define kfio_strncpy(x, y, z) strncpy(x, y, z)
#define kfio_strtoul(x, y, z) strtoul(x, y, z)
#define kfio_strlen(x)        strlen(x)
#define kfio_print(fmt, ...)        printf(fmt, ##__VA_ARGS__)
//#define infprint              printf
#define kfio_strchr(x, y)     strchr(x, y)
#define kfio_strncmp(x, y, z) strncmp(x, y, z)
#define kfio_strcat(x, y)     strcat(x, y)

#define kfio_div64_64(num, den)  ((num) / (den))

#endif

#define _FIO_UNICODE(str) L##str
#define FIO_UNICODE(str)  _FIO_UNICODE(str)


#endif /* _FIO_PORT_UFIO_H_ */
