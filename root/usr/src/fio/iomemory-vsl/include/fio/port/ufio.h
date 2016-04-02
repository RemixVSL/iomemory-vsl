//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/*
 * ufio.h contains interface information between kernel and user space.
 * Consequently information in ufio.h (or the platfrom-specific ufio.h)
 * must be cautious about anything that must only be in either the kernel
 * or user space.  Handling such situations is done by checking whether the
 * '__KERNEL__' macro is defined (or not).
 */

#ifndef _FIO_PORT_UFIO_H_
#define _FIO_PORT_UFIO_H_

#include <fio/port/config.h>

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/ufio.h>
#elif defined(__ESXI41__)
#include <fio/port/esxi41/ufio.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/ufio.h>
#elif defined(__linux__)
#include <fio/port/linux/ufio.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/solaris/ufio.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/ufio.h>
#elif defined(_AIX)
#include <fio/port/aix/ufio.h>
#elif defined(__OSX__)
#include <fio/port/osx/ufio.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/ufio.h>
#else
#error Unsupported OS
#endif

/* Non-platform specific */

#if !defined(__KERNEL__)
#include <fio/port/utypes.h>
#endif

#define _FIO_UNICODE(str) L##str
#define FIO_UNICODE(str)  _FIO_UNICODE(str)

#endif /* _FIO_PORT_UFIO_H_ */

