//-----------------------------------------------------------------------------                      
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/// @file userspace block device interface emulation.

#ifndef US_BLOCKDEV_H_
# define US_BLOCKDEV_H_

# ifdef __KERNEL__
#  include <fio/port/ktypes.h>
#  include <fio/port/kblock.h>

typedef struct 
{
    int (* ioctl_handler)(void *dev, unsigned int ioctl, fio_uintptr_t arg);
    int (* write_handler)(struct fio_device *dev, kfio_bio_t *bio);
    int (* read_handler)(struct fio_device *dev, kfio_bio_t *bio);
    int (* trim_handler)(struct fio_device *dev, kfio_bio_t *bio);
} bdev_server_funcs_t;

struct fio_device;

int bdev_server_start(const char *name, uint16_t port, const bdev_server_funcs_t *funcs, struct fio_device *dev);

int bdev_server_halt(const char *name);

#endif // __KERNEL__

# define BDEV_SERVER_BLOCKDEV_BASE_PORT 10322
# define BDEV_SERVER_CHARDEV_BASE_PORT  10422

struct fio_device;

extern uint32_t fio_get_sector_size_bytes(struct fio_device *dev);
extern uint64_t fio_get_device_size_bytes(struct fio_device *dev);

#endif
