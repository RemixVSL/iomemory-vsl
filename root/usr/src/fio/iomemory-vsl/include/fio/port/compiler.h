//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/* FIXME - this should be added to some general include so these are globally available
 * without specifically including this file.
 */

#ifndef __FIO_PORT_COMPILER_H__
#define __FIO_PORT_COMPILER_H__

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/compiler.h>
#elif defined(__ESXI41__)
# include <fio/port/esxi41/compiler.h>
#elif defined(__ESXI5__)
# include <fio/port/esxi5/compiler.h>
#elif defined(__linux__)
# include <fio/port/linux/compiler.h>
#elif defined(__SVR4) && defined(__sun)
# include <fio/port/solaris/compiler.h>
#elif defined(__FreeBSD__)
# include <fio/port/freebsd/compiler.h>
#elif defined(_AIX)
# include <fio/port/aix/compiler.h>
#elif defined(__OSX__)
# include <fio/port/osx/compiler.h>
#elif defined(WIN32) || defined(WINNT)
# include <fio/port/windows/compiler.h>
#else
# error Unsupported OS - please define a fio/port/<platform>/compiler.h and include it in fio/port/compiler.h
#endif

#endif
