//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * The Linux port's header files aren't quite self-consistent enough, so we have to
 * work around that here.
 */
#ifdef __linux
#include <linux/types.h>
#endif
#include <fio/port/errno.h>
#include <fio/port/dbgset.h>
#include <fio/port/ifio.h>

struct fio_strerror_entry {
    const int   errno_num;
    const char *errno_sym;
    const char *errno_msg;
};

#ifndef ENONE
#define ENONE 0
#endif

# define FIO_STRERROR_ENTRY(errnum, msg) [errnum] = { .errno_num = errnum, .errno_sym = #errnum, .errno_msg = msg }


static struct fio_strerror_entry fio_strerror_entries[] = {
    FIO_STRERROR_ENTRY(ENONE,             "No error"),
    FIO_STRERROR_ENTRY(EPERM,             "Operation not permitted"),
    FIO_STRERROR_ENTRY(ENOENT,            "No such file or directory"),
    FIO_STRERROR_ENTRY(ESRCH,             "No such process"),
    FIO_STRERROR_ENTRY(EINTR,             "Interrupted system call"),
    FIO_STRERROR_ENTRY(EIO,               "I/O error"),
    FIO_STRERROR_ENTRY(ENXIO,             "No such device or address"),
    FIO_STRERROR_ENTRY(E2BIG,             "Argument list too long"),
    FIO_STRERROR_ENTRY(ENOEXEC,           "Exec format error"),
    FIO_STRERROR_ENTRY(EBADF,             "Bad file number"),
    FIO_STRERROR_ENTRY(ECHILD,            "No child processes"),
    FIO_STRERROR_ENTRY(EAGAIN,            "Try again"),
    FIO_STRERROR_ENTRY(ENOMEM,            "Out of memory"),
    FIO_STRERROR_ENTRY(EACCES,            "Permission denied"),
    FIO_STRERROR_ENTRY(EFAULT,            "Bad address"),
    //FIO_STRERROR_ENTRY(ENOTBLK,           "Block device required"),   // If this is ever used it needs to be defined in the Windows case
    FIO_STRERROR_ENTRY(EBUSY,             "Device or resource busy"),
    FIO_STRERROR_ENTRY(EEXIST,            "File exists"),
    FIO_STRERROR_ENTRY(EXDEV,             "Cross-device link"),
    FIO_STRERROR_ENTRY(ENODEV,            "No such device"),
    FIO_STRERROR_ENTRY(ENOTDIR,           "Not a directory"),
    FIO_STRERROR_ENTRY(EISDIR,            "Is a directory"),
    FIO_STRERROR_ENTRY(EINVAL,            "Invalid argument"),
    FIO_STRERROR_ENTRY(ENFILE,            "File table overflow"),
    FIO_STRERROR_ENTRY(EMFILE,            "Too many open files"),
    FIO_STRERROR_ENTRY(ENOTTY,            "Not a typewriter"),
    //FIO_STRERROR_ENTRY(ETXTBSY,           "Text file busy"),          // If this is ever used it needs to be defined in the Windows case
    FIO_STRERROR_ENTRY(EFBIG,             "File too large"),
    FIO_STRERROR_ENTRY(ENOSPC,            "No space left on device"),
    FIO_STRERROR_ENTRY(ESPIPE,            "Illegal seek"),
    FIO_STRERROR_ENTRY(EROFS,             "Read-only file system"),
    FIO_STRERROR_ENTRY(EMLINK,            "Too many links"),
    FIO_STRERROR_ENTRY(EPIPE,             "Broken pipe"),
    FIO_STRERROR_ENTRY(EDOM,              "Math argument out of domain of func"),
    FIO_STRERROR_ENTRY(ERANGE,            "Math result not representable"),
    FIO_STRERROR_ENTRY(EUNCORRECTABLEECC, "Uncorrectable ECC error"),
    FIO_STRERROR_ENTRY(EUECC_BMOK,        "Uncorrectable ECC error"),
    FIO_STRERROR_ENTRY(EECC_VALIDATION_FAIL, "ECC validation failure"),
    { -1, "EUNKNOWN", "Unknown error" }
};


/* NOTICE: This is a slow linear search.  It is anticipated that printing
 * large volumes of error messages is not a performance requirement.
 */

static int get_strerror_entries_index(int errnum)
{
    int idx;

    errnum = -errnum;

    if (errnum <= 0)
        errnum = ENONE;

    for (idx=0; fio_strerror_entries[idx].errno_num != -1; idx++)
        if (errnum == fio_strerror_entries[idx].errno_num)
            break;

    if (fio_strerror_entries[idx].errno_num == -1)
        errprint("Unable to find translation for errno %d\n", errnum);

    return idx;
}


const char *ifio_strerror(int errnum)
{
    return fio_strerror_entries[get_strerror_entries_index(errnum)].errno_msg;
}


const char *ifio_strerror_sym(int errnum)
{
    return fio_strerror_entries[get_strerror_entries_index(errnum)].errno_sym;
}

#if defined(__cplusplus)
}
#endif
