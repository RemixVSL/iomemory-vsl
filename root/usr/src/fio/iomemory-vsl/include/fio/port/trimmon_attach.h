//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

struct fio_trimmon;

#if defined(WINNT) || defined(WIN32)
int fio_trimmon_init(struct fio_trimmon **tdata);
void fio_trimmon_teardown(struct fio_trimmon **tdata);
#else
#define fio_trimmon_init(trimmon_pp) (*(trimmon_pp)=NULL,0) // "returns" no error
#define fio_trimmon_teardown(a)      ((void)(a))
#endif

