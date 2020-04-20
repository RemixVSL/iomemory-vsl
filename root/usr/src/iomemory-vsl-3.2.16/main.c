//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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
#include <fio/port/compiler.h>
#include "port-internal.h"
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <fio/port/fio-port.h>
#include <linux/module.h>
#include <fio/port/port_config.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config.h>
#endif
#include <fio/port/dbgset.h>
#include <fio/port/ifio.h>
#if (KFIO_SCSI_DEVICE==1)
#   include <fio/port/kscsi.h>
#else
#   include <fio/port/common-linux/kblock.h>
#endif
#include <fio/port/port_config_vars_externs.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config_vars_externs.h>
#endif

/* Create storage for the debug flags */
/// @cond GENERATED_CODE
#include <fio/port/port_config_macros_clear.h>
/// @endcond
#define FIO_PORT_CONFIG_SET_DBGFLAG
#define FIO_PORT_CONFIG_MACRO(dbgf) [_DF_ ## dbgf] = { .name = #dbgf, .mode = S_IRUGO | S_IWUSR, .value = dbgf },

#if (FUSION_INTERNAL==1)
/// @cond GENERATED_CODE
#include <fio/internal/config_macros_clear.h>
/// @endcond
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(dbgf) [_DF_ ## dbgf] = { .name = #dbgf, .mode = S_IRUGO | S_IWUSR, .value = dbgf },
#endif

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */
extern int use_workqueue;

static struct dbgflag _dbgflags[] = {
/// @cond GENERATED_CODE
#include <fio/port/port_config_macros.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config_macros.h>
#endif
/// @endcond
    [_DF_END] = { .name = NULL, .mode = 0, .value =0 }
};

struct dbgset _dbgset = {
    .flag_dir_name = "debug",
    .flags         = _dbgflags
};

/* Make debug flags module parameters so they can be set at module load-time */
/// @cond GENERATED_CODE
#include <fio/port/port_config_macros_clear.h>
/// @endcond
#define FIO_PORT_CONFIG_SET_DBGFLAG
#if (DISABLE_MODULE_PARAM_EXPORT==0)
#define FIO_PORT_CONFIG_MACRO(dbgf) module_param_named(debug_ ## dbgf, _dbgflags[_DF_ ## dbgf].value, uint, S_IRUGO | S_IWUSR);
#else
#define FIO_PORT_CONFIG_MACRO(dbgf)
#endif

/// @cond GENERATED_CODE
#include <fio/port/port_config_macros.h>
/// @endcond

#if (FUSION_INTERNAL==1 && DISABLE_MODULE_PARAM_EXPORT==0)
/// @cond GENERATED_CODE
#include <fio/internal/config_macros_clear.h>
/// @endcond
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(dbgf) module_param_named(debug_ ## dbgf, _dbgflags[_DF_ ## dbgf].value, uint, S_IRUGO | S_IWUSR);
/// @cond GENERATED_CODE
#include <fio/internal/config_macros.h>
/// @endcond
#endif

extern int init_fio_dev(void);
extern void cleanup_fio_dev(void);

#if (FUSION_STATIC_KERNEL==0)
extern int kfio_pci_register_driver(void);
extern void kfio_pci_unregister_driver(void);
#endif

extern int init_fio_iodrive(void);
extern void cleanup_fio_iodrive(void);

extern int init_fio_obj(void);
extern void cleanup_fio_obj(void);

extern int init_fio_blk(void);
extern void cleanup_fio_blk(void);

extern void fio_auto_attach_wait(void);

extern void kfio_iodrive_sysrq_keys(void);
extern void kfio_iodrive_unreg_sysrq_keys(void);

/// @brief Driver initialization for parts that do not require cleanup if initialization fails.
/// @note  The companion fio_do_exit() function may be called at any time, regardless of the
///        status of fio_do_init(). However, on error, a call to fio_do_exit() is unnecessary
///        because everything is automatically cleaned up.
static int fio_do_init(void)
{
    int rc;

    if (((rc = init_fio_iodrive()) != 0)
        || ((rc = init_fio_obj()) != 0)
        || ((rc = init_fio_blk()) != 0)
        || ((rc = kfio_init_storage_interface()) != 0))
    {
        errprint("Failed to load " FIO_DRIVER_NAME " error %d: %s\n",
                 rc, ifio_strerror(rc));

        if (rc > 0)
        {
            rc = -rc; // follow convention of 0/-error
        }

        return rc;
    }

#if (FUSION_STATIC_KERNEL==0)
    if (0 >= kfio_pci_register_driver()) // >0 => success
    {
        kfio_pci_unregister_driver();
        rc = -ENODEV;
    }
#endif

    return rc;
}

static int __init init_fio_driver(void)
{
    int rc = 0;

#if (FUSION_STATIC_KERNEL==1)
    auto_attach = 0;
#endif

    if(use_workqueue == USE_QUEUE_RQ)
    {
        infprint("Using Linux I/O Scheduler\n");
    }

    /* If the LEB map isn't loaded then don't bother trying to auto attach */
    if (!iodrive_load_eb_map)
    {
        auto_attach = 0;
    }

    if ((rc = kfio_checkstructs()) ||
        (rc = init_fio_dev()))
    {
        return 0;
    }

    /* Call swiss knife of all init functions. */
    rc = fio_do_init();
    fio_auto_attach_wait();
    kfio_iodrive_sysrq_keys();

    if ((rc = dbgs_create_flags_dir(&_dbgset)))
    {
        errprint("Failed to create debug flags directory %d: %s\n",
                 rc, ifio_strerror(rc));
    }

    return rc;
}

void __exit exit_fio_driver(void)
{
    dbgs_delete_flags_dir(&_dbgset);
    kfio_iodrive_unreg_sysrq_keys();

    cleanup_fio_blk();
    cleanup_fio_obj();

    kfio_teardown_storage_interface();

#if (FUSION_STATIC_KERNEL==0)
    kfio_pci_unregister_driver();
#endif
    cleanup_fio_iodrive();
    cleanup_fio_dev();
}

/**
 * @}
 */

#if (FUSION_STATIC_KERNEL==0)
module_init(init_fio_driver);
module_exit(exit_fio_driver);
#endif

// MODULE_LICENSE is found in license.c - which is auto-generated
// Module metadata parameters will be found in license.c - which is auto-generated
