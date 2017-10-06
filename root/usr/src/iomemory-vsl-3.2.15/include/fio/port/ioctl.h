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

#ifndef _FIO_GENERIC_IOCTL_H_
#define _FIO_GENERIC_IOCTL_H_

#if defined(__FreeBSD__)
#include <fio/port/freebsd/platform_ioctl.h>
#elif defined(__hpux__)
#include <fio/port/hpux/platform_ioctl.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/platform_ioctl.h>
#elif defined(__OSX__)
#include <fio/port/osx/platform_ioctl.h>
#endif

/* If needed these can be overriden in the platform_ioctl.h file */
#ifndef FIO_IOC_SIZEBITS
    #define FIO_IOC_SIZEBITS   14
#endif
#ifndef FIO_IOC_DIRBITS
    #define FIO_IOC_DIRBITS    2
#endif

#ifndef FIO_IOC_NONE
    #define FIO_IOC_NONE       0U
#endif
#ifndef FIO_IOC_WRITE
    #define FIO_IOC_WRITE      1U  /* copyout parameters */
#endif
#ifndef FIO_IOC_READ
    #define FIO_IOC_READ       2U  /* copyin parameters */
#endif

#define FIO_IOC_NRBITS     8
#define FIO_IOC_TYPEBITS   8

#define FIO_IOC_NRMASK     ((1 << FIO_IOC_NRBITS)-1)
#define FIO_IOC_TYPEMASK   ((1 << FIO_IOC_TYPEBITS)-1)
#define FIO_IOC_SIZEMASK   ((1 << FIO_IOC_SIZEBITS)-1)
#define FIO_IOC_DIRMASK    ((1 << FIO_IOC_DIRBITS)-1)

#define FIO_IOC_NRSHIFT    0
#define FIO_IOC_TYPESHIFT  (FIO_IOC_NRSHIFT+FIO_IOC_NRBITS) // 8
#define FIO_IOC_SIZESHIFT  (FIO_IOC_TYPESHIFT+FIO_IOC_TYPEBITS) // 16
#define FIO_IOC_DIRSHIFT   (FIO_IOC_SIZESHIFT+FIO_IOC_SIZEBITS) // 30, FreeBSD:29

#ifdef FIO_PLATFORM_IOC
#  define FIO_IOC(dir, type, nr, size) FIO_PLATFORM_IOC(dir, type, nr, size)
#else
#  define FIO_IOC(dir, type, nr, size)   \
        (((dir)  << FIO_IOC_DIRSHIFT)  | \
         ((type) << FIO_IOC_TYPESHIFT) | \
         ((nr)   << FIO_IOC_NRSHIFT)   | \
         ((size) << FIO_IOC_SIZESHIFT))
#endif

/* provoke compile error for invalid uses of size argument */
#if 0
extern unsigned int __invalid_size_argument_for_IOC;
#define FIO_IOC_TYPECHECK(t) \
        ((sizeof(t) == sizeof(t[1]) && \
          sizeof(t) < (1 << FIO_IOC_SIZEBITS)) ? \
          sizeof(t) : __invalid_size_argument_for_IOC)
#endif
#define FIO_IOC_TYPECHECK(t) (sizeof(t))

/* used to create numbers */
#define FIO_IO(type,nr)            FIO_IOC(FIO_IOC_NONE,(type),(nr),0)
#define FIO_IOR(type,nr,size)      FIO_IOC(FIO_IOC_READ,(type),(nr),(FIO_IOC_TYPECHECK(size)))
#define FIO_IOW(type,nr,size)      FIO_IOC(FIO_IOC_WRITE,(type),(nr),(FIO_IOC_TYPECHECK(size)))
#define FIO_IOWR(type,nr,size)     FIO_IOC(FIO_IOC_READ|FIO_IOC_WRITE,(type),(nr),(FIO_IOC_TYPECHECK(size)))
#define FIO_IOR_BAD(type,nr,size)  FIO_IOC(FIO_IOC_READ,(type),(nr),sizeof(size))
#define FIO_IOW_BAD(type,nr,size)  FIO_IOC(FIO_IOC_WRITE,(type),(nr),sizeof(size))
#define FIO_IOWR_BAD(type,nr,size) FIO_IOC(FIO_IOC_READ|FIO_IOC_WRITE,(type),(nr),sizeof(size))

/* used to decode ioctl numbers.. */
#define FIO_IOC_DIR(nr)            (((nr) >> FIO_IOC_DIRSHIFT) & FIO_IOC_DIRMASK)
#define FIO_IOC_TYPE(nr)           (((nr) >> FIO_IOC_TYPESHIFT) & FIO_IOC_TYPEMASK)
#define FIO_IOC_NR(nr)             (((nr) >> FIO_IOC_NRSHIFT) & FIO_IOC_NRMASK)
#define FIO_IOC_SIZE(nr)           (((nr) >> FIO_IOC_SIZESHIFT) & FIO_IOC_SIZEMASK)

#endif // _FIO_GENERIC_IOCTL_H_

