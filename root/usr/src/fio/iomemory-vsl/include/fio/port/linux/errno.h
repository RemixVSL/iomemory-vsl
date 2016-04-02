
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_LINUX_ERRNO_H__
#define __FIO_PORT_LINUX_ERRNO_H__

#if !defined(__FIO_PORT_ERRNO_H__)
#error Dont include <fio/port/linux/errno.h> directly - instead include <fio/port/errno.h>
#endif

#if !defined(__KERNEL__)
# include <errno.h>
#endif

#endif
