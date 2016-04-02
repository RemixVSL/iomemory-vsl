//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_ERRNO_H__
#define __FIO_PORT_ERRNO_H__

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/errno.h>
#elif defined(__ESXI41__)
# include <fio/port/esxi41/errno.h>
#elif defined(__ESXI5__)
# include <fio/port/esxi5/errno.h>
#elif defined(__linux__)
# include <fio/port/linux/errno.h>
#elif defined(__SVR4) && defined(__sun)
# include <fio/port/solaris/errno.h>
#elif defined(__FreeBSD__)
# include <fio/port/freebsd/errno.h>
#elif defined(__OSX__)
# include <fio/port/osx/errno.h>
#elif defined(WIN32) || defined(WINNT)
# include <fio/port/windows/win_errno.h>
#endif

#if !defined(__FreeBSD__) && !defined(__OSX__) && !defined(ETXTBSY)

#define ENONE                0

#define EPERM                1    /* Operation not permitted */
#define ENOENT               2    /* No such file or directory */
#define ESRCH                3    /* No such process */
#define EINTR                4    /* Interrupted system call */
#define EIO                  5    /* I/O error */
#define ENXIO                6    /* No such device or address */
#define E2BIG                7    /* Argument list too long */
#define ENOEXEC              8    /* Exec format error */
#define EBADF                9    /* Bad file number */
#define ECHILD              10    /* No child processes */
#define EAGAIN              11    /* Try again */
#define ENOMEM              12    /* Out of memory */
#define EACCES              13    /* Permission denied */
#define EFAULT              14    /* Bad address */
#define ENOTBLK             15    /* Block device required */
#define EBUSY               16    /* Device or resource busy */
#define EEXIST              17    /* File exists */
#define EXDEV               18    /* Cross-device link */
#define ENODEV              19    /* No such device */
#define ENOTDIR             20    /* Not a directory */
#define EISDIR              21    /* Is a directory */
#define EINVAL              22    /* Invalid argument */
#define ENFILE              23    /* File table overflow */
#define EMFILE              24    /* Too many open files */
#define ENOTTY              25    /* Not a typewriter */
#define ETXTBSY             26    /* Text file busy */
#define EFBIG               27    /* File too large */
#define ENOSPC              28    /* No space left on device */
#define ESPIPE              29    /* Illegal seek */
#define EROFS               30    /* Read-only file system */
#define EMLINK              31    /* Too many links */
#define EPIPE               32    /* Broken pipe */
#define EDOM                33    /* Math argument out of domain of func */
#define ERANGE              34    /* Math result not representable */

#endif /* __FreeBSD__ && OSX */

/* Fusion-specific errornos start at 1024 this provides a gap so that new
 * Linux errnos don't step on Fusion-io errnos.  Linux requires that errnos
 * must be less than 4096. */
#define EUNCORRECTABLEECC 1024
#define EUNKNOWN          1025

#endif

