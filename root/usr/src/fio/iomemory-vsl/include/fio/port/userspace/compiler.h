//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/* FIXME - this should be more selective about being gcc only and which version */

#ifndef __FIO_PORT_COMPILER_USERSPACE_H__
#define __FIO_PORT_COMPILER_USERSPACE_H__

#ifndef __FIO_PORT_COMPILER_H__
# error Do not include this file directly - instead inclued <fio/port/compiler.h>
#endif

#define __must_use_result       __attribute__((warn_unused_result))

#endif
