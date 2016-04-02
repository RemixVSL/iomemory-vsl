//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_GENERIC_IOCTL_H_
#define _FIO_GENERIC_IOCTL_H_

#if defined(__FreeBSD__)
#include <fio/port/freebsd/platform_ioctl.h>
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

