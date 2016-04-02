//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_STDINT_H__
#define __FIO_PORT_STDINT_H__

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/stdint.h>
#elif defined(__ESXI41__)
# include <fio/port/esxi41/stdint.h>
#elif defined(__ESXI5__)
# include <fio/port/esxi5/stdint.h>
#elif defined(__linux__) && !defined(SICORTEX)
# include <fio/port/linux/stdint.h>
#elif defined(__SVR4) && defined(__sun)
# include <fio/port/solaris/stdint.h>
# include <stdint.h>
#elif defined(__FreeBSD__)
# include <fio/port/freebsd/stdint.h>
#elif defined(_AIX)
# include <fio/port/aix/stdint.h>
#elif defined(__OSX__)
# include <fio/port/osx/stdint.h>
#elif defined(WIN32) || defined(WINNT)
# include <fio/port/windows/stdint.h>
#elif defined(EXTERNAL_OS)
# include "fio/port/external_os_stdint.h"
#endif

#endif
