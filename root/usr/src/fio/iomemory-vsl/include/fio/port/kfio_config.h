//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_KFIO_CONFIG_H_
#define _FIO_PORT_KFIO_CONFIG_H_


#include <fio/port/config.h>

#if defined(USERSPACE_KERNEL)
# include <fio/port/userspace/kfio_config.h>
#elif defined(__ESXI41__)
#include <fio/port/esxi41/kfio_config.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/kfio_config.h>
#elif defined(__linux__)
#include <fio/port/linux/kfio_config.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/solaris/kfio_config.h>
#include <fio/port/ktypes.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/kfio_config.h>
#include <fio/port/ktypes.h>
#elif defined(_AIX)
#include <fio/port/aix/kfio_config.h>
#include <fio/port/ktypes.h>
#elif defined(__OSX__)
#include <fio/port/osx/kfio_config.h>
#include <fio/port/ktypes.h>
#elif defined(WINNT) || defined(WIN32)
#define kfio_kmalloc    kmalloc
#include <fio/port/ktypes.h>
#elif defined (EXTERNAL_OS)
#include "fio/port/external_os_kfio_config.h"
#else
#error Unsupported OS
#endif

#endif /* _FIO_PORT_KFIO_CONFIG_H_ */

