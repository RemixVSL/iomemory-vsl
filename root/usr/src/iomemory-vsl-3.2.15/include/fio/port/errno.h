//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015 SanDisk Corp. and/or all its affiliates. All rights reserved.
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

#ifndef __FIO_PORT_ERRNO_H__
#define __FIO_PORT_ERRNO_H__

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/errno.h>
#elif defined(__linux__) || defined(__VMKLNX__)
# include <fio/port/common-linux/errno.h>
#elif defined(__SVR4) && defined(__sun)
# include <fio/port/solaris/errno.h>
#elif defined(__hpux__)
# include <fio/port/hpux/errno.h>
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
#define ENOTSUP            252    /* Function not supported */

#endif /* __FreeBSD__ && OSX */

#if !defined(EDOOFUS)
#define EDOOFUS             88    /* Programmer error */
#endif

/* Fusion-specific error numbers start at 1024 this provides a gap so that new
 * error numbers don't step on Fusion-io errnos.
 * Linux requires that error numbers must be less than 4096. */
#define EUNCORRECTABLEECC      1024 ///< Keep this first (or fix ERROR_CODE_IS_FUSION_SPECIFIC).
#define EUECC_BMOK             1025
#define EAFU                   1026
#define EECC_VALIDATION_FAIL   1027
#define EERASED                1028 ///< erased space was read.
#define EVERRNG                1029 ///< Version is out of range
#define EUNKNOWN               1030 ///< Keep this last

// NB: this macro expects a positive value.
#define ERROR_CODE_IS_FUSION_SPECIFIC(err) \
    (((err) >= EUNCORRECTABLEECC) && ((err) <= EUNKNOWN))

#endif

