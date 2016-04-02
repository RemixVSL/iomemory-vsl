//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#include <fio/obj/types.h>

struct fio_trimmon;

#if defined(WINNT) || defined(WIN32)
extern int fio_trimmon_update_bitmap(struct fio_trimmon*, fio_block_range_t); // Addes in-flight completions to local bitmap base block and length
extern int fio_trimmon_update_bitmap_req(struct fio_trimmon *t, struct fusion_request *req);
#else
#define fio_trimmon_update_bitmap(t,r) ((void)(t),(void)(r),0)
#define fio_trimmon_update_bitmap_req(t,r) (void)((void)(t),(void)(r),0)
#endif

