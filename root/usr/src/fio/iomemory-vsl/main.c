//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if defined (__linux__)
#include <linux/compiler.h>
#include "port-internal.h"
#if KFIOC_USE_LINUX_UACCESS_H && !defined(__VMKLNX__)
# include <linux/uaccess.h>
#else
# include <asm/uaccess.h>
#endif
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/sched.h>
#endif
#include <fio/port/fio-port.h>
#if defined (__linux__)
#include <linux/module.h>
#endif
#include <fio/port/dbgset.h>
#include <fio/port/config.h>
#include <fio/port/ifio.h>
#include <fio/port/linux/kblock.h>
#include <fio/port/config_vars_externs.h>

/* Create storage for the debug flags */
#include <fio/port/config_macros_clear.h>
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(dbgf) [_DF_ ## dbgf] = { .name = #dbgf, .mode = S_IRUGO | S_IWUSR, .value = dbgf },

extern int use_workqueue;
extern int preallocate_memory_count;
extern char *preallocate_memory[];
extern int_array_t _preallocate_memory;


static struct dbgflag _dbgflags[] = {
#include <fio/port/config_macros.h>
    [_DF_END] = { .name = NULL, .mode = 0, .value =0 }
};

struct dbgset _dbgset = {
    .flag_dir_name = "debug",
    .flags         = _dbgflags
};

void
__kassert_fail (const char *expr, const char *file, int line,
                const char *func, int good, int bad)
{
    errprint("iodrive: assertion failed %s:%d:%s: %s",
             file, line, func, expr);

#if !FIO_FAIL_FAST
    errprint("ASSERT STATISTICS: %d bad, %d good\n",
             bad, good);

    dump_stack ();
#else
    BUG();
#endif
}
EXPORT_SYMBOL(__kassert_fail);

/* Make debug flags module parameters so they can be set at module load-time */
#include <fio/port/config_macros_clear.h>
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(dbgf) module_param_named(debug_ ## dbgf, _dbgflags[_DF_ ## dbgf].value, uint, S_IRUGO | S_IWUSR);
#include <fio/port/config_macros.h>

extern int init_fio_dev(void);
extern void cleanup_fio_dev(void);

extern int kfio_pci_register_driver(void);
extern void kfio_pci_unregister_driver(void);

extern int init_fio_iodrive(void);
extern void cleanup_fio_iodrive(void);

extern int init_fio_obj(void);
extern void cleanup_fio_obj(void);

extern int init_fio_blk(void);
extern void cleanup_fio_blk(void);

extern void fio_auto_attach_wait(void);

extern void kfio_iodrive_sysrq_keys(void);
extern void kfio_iodrive_unreg_sysrq_keys(void);
#if defined (__VMKLNX__)
extern int  ifio_init_memory();
extern void ifio_cleanup_memory();
#endif

extern int fio_dont_init;

#define FIO_ABORT_STRING  "iodrive="

/* Create the config_string so that it can be dumped in the load messages. */
#include <fio/port/config_macros_clear.h>
#define FIO_CONFIG_SET_ALL
#define xstr(s) #s
#define FIO_CONFIG_MACRO(cfg) #cfg "=" xstr(cfg) "\n"

const char *__fusion_config_string =
    "***BEGIN CONFIGURATION***\n"
#include <fio/port/config_macros.h>
    "***END CONFIGURATION***\n"
    ;

int fio_do_init(void)
{
    int rc;

    if ((rc = init_fio_iodrive() != 0)
        || (rc = init_fio_obj()  != 0)
        || (rc = init_fio_blk()  != 0)) 
    {
        errprint("%s(): Failed to load " FIO_DRIVER_NAME " error %d: %s\n", __func__,
                 rc, ifio_strerror(rc));

        if (rc > 0)
        {
            rc = -rc; // follow convention of 0/-error
        }

        return rc;
    } 

    rc = kfio_register_blkdev("fio");
    if (rc <= 0)
    {
        return -EBUSY;
    }

    if (0 >= kfio_pci_register_driver()) // >0 => success
    {
        kfio_pci_unregister_driver();
        rc = -ENODEV;
    }

    return rc;
}

static int __init init_fio_driver(void)
{
    int i, rc = 0;

#if defined(__TENCENT__)
    auto_attach = 0;
#endif

    _preallocate_memory.num_elements_used = preallocate_memory_count;
    for (i = 0; i < preallocate_memory_count; i++)
    {
        _preallocate_memory.array[i] = kfio_strtoul(preallocate_memory[i], NULL, 10);
    }

#if FUSION_DEBUG
    infprint("%s", __fusion_config_string);
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

#if defined(__VMKLNX__)
    rc = ifio_init_memory();
    if (rc)
    {
        return rc;
    }
#endif

    if ((rc = kfio_checkstructs()) ||
        (rc = init_fio_dev()))
    {
        return 0;
    }

    kfio_register_reboot_notifier();

    if (!fio_dont_init)
    {
        rc = fio_do_init();

        fio_auto_attach_wait();

#if ENABLE_PCICHECK
        fio_pcicheck_init();
        fio_pcicheck_runcheck();
#endif
    }

#if !defined(__VMKLNX__)
    kfio_iodrive_sysrq_keys();
#endif

    if ((rc = dbgs_create_flags_dir(&_dbgset)))
    {
        errprint("%s(): Failed to create debug flags dir %d: %s\n",
                 __func__, rc, ifio_strerror(rc));
    }

#if FUSION_DIRECTFS
    infprint("***   KFIOC_USE_IO_SCHED = %d\n", KFIOC_USE_IO_SCHED );
    infprint("***   "
#if !defined(__VMKLNX__)
                    "not-"
#endif
                         "defined __FMKLNX__\n");
    infprint("***   KFIOC_DISCARD is %d\n", KFIOC_DISCARD );
    infprint("***   KFIOC_BARRIER is %d\n", KFIOC_BARRIER );
    infprint("***   use_workqueue = %d\n", use_workqueue );
#endif

    return rc;
}

static void __exit cleanup_fio_driver(void)
{
    dbgs_delete_flags_dir(&_dbgset);

#if !defined(__VMKLNX__)
    kfio_iodrive_unreg_sysrq_keys();
#endif

    cleanup_fio_blk();
    cleanup_fio_obj();
    kfio_unregister_blkdev("fio");
    kfio_pci_unregister_driver();
    kfio_unregister_reboot_notifier();
    cleanup_fio_iodrive();
    cleanup_fio_dev();
#if defined(__VMKLNX__)
    ifio_cleanup_memory();
#endif
}

module_init(init_fio_driver);
module_exit(cleanup_fio_driver);

// MODULE_LICENSE is found in license.c - which is auto-generated
MODULE_AUTHOR("Fusion-io");
MODULE_DESCRIPTION("Fusion-io ioMemory-VSL (Virtual Storage Layer)");
