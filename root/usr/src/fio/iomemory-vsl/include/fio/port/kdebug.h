//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_KDEBUG_H__
#define __FIO_PORT_KDEBUG_H__

#define kbreak_on(i)    do {  if(i) kbreakpoint(); } while(0)

#endif /* __FIO_PORT_KDEBUG_H__ */
