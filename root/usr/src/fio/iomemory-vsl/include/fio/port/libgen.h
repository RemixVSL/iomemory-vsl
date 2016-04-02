//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if !defined( __PORT_LIBGEN_H__ )
#define __PORT_LIBGEN_H__

#if defined (WIN32) || defined(WINNT)
    #include <fio/port/windows/libgen.h>
#else
    #include <libgen.h>
#endif

#endif // __PORT_LIBGEN_H__

