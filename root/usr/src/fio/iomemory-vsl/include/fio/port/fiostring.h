//-----------------------------------------------------------------------------
// Copyright 2010-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_STRINGS_H__
#define __FIO_STRINGS_H__

#include <string.h>

#if defined(WINNT) || defined(WIN32)
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
#endif


#endif  // __FIO_STRINGS_H__
