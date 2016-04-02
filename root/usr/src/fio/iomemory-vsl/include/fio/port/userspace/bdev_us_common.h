//-----------------------------------------------------------------------------                      
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/// @file userspace block device emulation operations and structures.

#ifndef BDEV_US_COMMON_H_
# define BDEV_US_COMMON_H_

# include <stdint.h>

#ifndef _BSD_SOURCE
# define _BSD_SOURCE
#endif
#include <endian.h>

#define htonll htobe64
#define ntohll be64toh 

# include <linux/nbd.h>

// Extensions to NBD request.
# define NBD_CMD_IOCTL 3  // 'from' = ioctl number.
# define NBD_CMD_TRIM  4 

// Extensions to NBD reply protocol.
# define NBD_REPLY_MAGIC_COPY_FROM_USER 0x4b6c4701
# define NBD_REPLY_MAGIC_COPY_TO_USER   0x56670175

# define NBD_SET_SEQ(req, seq) *((uint64_t *)&req.handle) = seq
# define NDB_CMP_SEQ(req, seq) (*((uint64_t *)&req.handle) == seq)
# define NDB_GET_SEQ(req)      (*((uint64_t *)&req.handle))

#endif
