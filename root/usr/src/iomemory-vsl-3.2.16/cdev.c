//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014 Fusion-io, Inc. (acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015 SanDisk Corp. and/or all its affiliates. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the SanDisk Corp. nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "port-internal.h"
#include <fio/port/dbgset.h>
#include <fio/port/cdev.h>
#include <fio/port/common-linux/kfile.h>

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/miscdevice.h>
#include <linux/poll.h>

int fusion_create_control_device(struct fusion_nand_device *nand_dev);
int fusion_destroy_control_device(struct fusion_nand_device *nand_dev);

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

/**
* @brief given a pointer to struct file returns a pointer to
* the private_data member of the file structure
*/
/* MST maybe not needed any more ... defined inside kfile.c
static inline void *kfio_fs_private_data(struct file *fp)
{
    return fp->private_data;
}
*/

/**
 * Return events that are set.  If the cdev is not alerted,
 * this waits using the cdev's poll_queue, so the user-land application
 * that has called poll(), epoll(), or select() will get notified when something
 * interesting happens.  After return an ioctl needs to be called to get the
 * actual event data.
 */
static unsigned int fusion_control_ioctl_poll(
                    struct file* filep,
                    struct poll_table_struct* polltable)
{
    struct fusion_nand_device *nand_dev =
          (struct fusion_nand_device *) kfio_fs_private_data(filep);
    kfio_poll_struct *ps;

    ps = nand_device_get_poll_struct(nand_dev);
    poll_wait(filep, (wait_queue_head_t *)(ps), polltable);

    if (is_nand_device_event_set(nand_dev) )
    {
        return (POLLIN | POLLPRI);
    }

    return (0);
}

static int fusion_control_open(struct inode *inode, struct file *file)
{
    struct fusion_nand_device *entry;
    int minor = iminor(inode);
    int major = imajor(inode);
    struct miscdevice *misc;

    int rv = -ENODEV;

    if (major != MISC_MAJOR)
    {
        errprint("inode has major %d and is not a MISC_MAJOR device\n", major);
        goto end;
    }

    /* This must map an 'inode->i_cdev' to a 'void *fusion_nand_device' */
    entry = fusion_nand_get_first();
    while (entry)
    {
        misc = (struct miscdevice *)fusion_nand_get_miscdev(entry);
        if (misc->minor == minor)
            break;

        entry = fusion_nand_get_next(entry);
    }

    if (entry == NULL)
    {
        errprint("Can't map misc device minor %d to fusion_nand_device\n", minor);
        goto end;
    }

    file->private_data = entry;
    rv = 0;

end:
    return rv;
}

static int fusion_control_release(struct inode *inode, struct file *filep)
{
    return 0;
}

static long fusion_control_ioctl_internal(struct file *file, unsigned int cmd, fio_uintptr_t arg)
{
    void *nand_dev = (struct fusion_nand_device *) kfio_fs_private_data(file);

    return fusion_control_ioctl( nand_dev, cmd, arg );
}

static long fusion_control_compat_ioctl_internal(struct file *file, unsigned int cmd, fio_uintptr_t arg)
{
    void *nand_dev = (struct fusion_nand_device *) kfio_fs_private_data(file);
    return fusion_control_ioctl( nand_dev, cmd, arg );
}

static struct file_operations fusion_control_ops =
{
    open:    fusion_control_open,
    release: fusion_control_release,
    unlocked_ioctl: fusion_control_ioctl_internal,
    compat_ioctl: fusion_control_compat_ioctl_internal,
    poll:    fusion_control_ioctl_poll,
};

/**
 * Initialize the kfio_poll_struct
 * Linux - wait_queue_head_t
 * Solaris - struct pollhead
 * FreeBSD - struct selinfo
 */
void kfio_poll_init(kfio_poll_struct *p)
{
    init_waitqueue_head((wait_queue_head_t *) p);
}

/**
 * Called when events of interest have occurred
 */
void kfio_poll_wake(kfio_poll_struct *p)
{
    wake_up_all((wait_queue_head_t *) p);
}

/******************************************************************************
* OS-specific interfaces for char device functions that call into the
* core fucntions.
*/

/**
* @brief OS-specific char device initialization.
*/
static void misc_dev_init(struct miscdevice *md, char *dev_name)
{
    fusion_control_ops.owner = THIS_MODULE;
    kfio_memset(md, 0, sizeof(*md));
    md->minor = MISC_DYNAMIC_MINOR;
    md->name  = dev_name;
    md->fops  = &fusion_control_ops;
}

/// @brief OS-specific init for char device.
int fusion_create_control_device(struct fusion_nand_device *nand_dev)
{
    struct miscdevice *misc;
    int result;

    misc = (struct miscdevice *)fusion_nand_get_miscdev(nand_dev);

    misc_dev_init(misc, fusion_nand_get_dev_name(nand_dev));

    result = misc_register(misc);
    if(result < 0)
    {
        errprint("%s Unable to initialize misc device '%s'\n",
                fusion_nand_get_bus_name(nand_dev),
                fusion_nand_get_dev_name(nand_dev));
    }

// TODO: TENCENT_KERNEL heh
#if !defined(__TENCENT_KERNEL__)
    fio_wait_for_dev(fusion_nand_get_dev_name(nand_dev));
#endif

    return result;
}

int fusion_destroy_control_device(struct fusion_nand_device *nand_dev)
{
    struct miscdevice *misc;

    misc = (struct miscdevice *)fusion_nand_get_miscdev(nand_dev);

    infprint("%s: destroying device %d\n",
            fusion_nand_get_bus_name(nand_dev),
            fusion_nand_get_devnum(nand_dev));

    misc_deregister(misc);

    return 0;
}

/**
 * @}
 */
