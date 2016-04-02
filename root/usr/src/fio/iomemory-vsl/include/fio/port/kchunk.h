//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OSX v 10.6+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KCHUNK_H__
#define __FIO_PORT_KCHUNK_H__

int fio_chunk_init (unsigned int chunk_size_mb);
void fio_chunk_deinit (void);
void * fio_chunk_alloc_page (void);
int fio_chunk_free_page (void * pg);

#endif //__FIO_PORT_KBMP_H__
