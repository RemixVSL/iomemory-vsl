//-----------------------------------------------------------------------------
// Copyright 2011-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_STAT_H__
#define __FIO_PORT_STAT_H__

// This file creates portability between OS's for the sys/stat.h functions.

#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(WINNT)
    #define stat        _stat64
    
    #define S_IFBLK     _S_IFBLK
    #define S_IFCHR     _S_IFCHR
    #define S_IFIFO     _S_IFIFO
    #define S_IFREG     _S_IFREG
    #define S_IFLNK     _S_IFLNK
    #define S_IFSOCK    _S_IFSOCK

    #define S_ISBLK(m)  (((m) & _S_IFMT) == (_S_IFCHR | _S_IFDIR))    // Maybe this is same as linux?
    #define S_ISCHR(m)  (((m) & _S_IFMT) == _S_IFCHR)
#endif

#endif  // WIN32 || WINNT
