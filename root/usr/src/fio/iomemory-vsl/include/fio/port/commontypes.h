//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_COMMONTYPES_H_
#define _FIO_PORT_COMMONTYPES_H_

#include <fio/port/config.h>

#if defined(USERSPACE_KERNEL)
#include <fio/port/userspace/commontypes.h>
#elif defined(__ESXI41__)
#include <fio/port/esxi41/commontypes.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/commontypes.h>
#elif defined(__linux__)
#include <fio/port/linux/commontypes.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/solaris/commontypes.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/commontypes.h>
#elif defined(__OSX__)
#include <fio/port/osx/commontypes.h>
#elif defined(_AIX)
#include <fio/port/aix/commontypes.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/commontypes.h>
#elif defined (EXTERNAL_OS)
#include "external_os_commontypes.h"
#else
#error Unsupported OS - if you are porting, try starting with a copy of stubs
#endif

#endif /* _FIO_PORT_COMMONTYPES_H_ */
