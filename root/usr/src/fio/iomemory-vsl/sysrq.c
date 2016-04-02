//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#include <linux/stddef.h>
#include <linux/errno.h>
#include <linux/sysrq.h>
#include <linux/list.h>

#include <fio/port/linux/kfio_config.h>
#include <fio/port/ktypes.h>
#include <fio/port/dbgset.h>

extern void fio_pcicheck_handle_sysrq(void);
extern void iodrive_dump_all_csr(void);
extern void iodrive_dump_all_software_state(void);

#define SYSRQ_SOFTWARE  'l'
#define SYSRQ_CSR       'z'

#if SPINLOCK_STATS
#define SYSRQ_SPINSTATS 'g'
extern void dump_spinlock_stats(void);
#endif

void iodrive_handle_sysrq(int key
#if ! KFIOC_HAS_GLOBAL_REGS_POINTER
                          ,struct pt_regs *pt_regs
#endif
#if KFIOC_SYSRQ_HANDLER_NEEDS_TTY_STRUCT
                          ,struct tty_struct *tty
#endif
                          )
{
    switch (key)
    {
    case SYSRQ_CSR:
        iodrive_dump_all_csr();
        break;
    case SYSRQ_SOFTWARE:
        iodrive_dump_all_software_state();
        break;

#if SPINLOCK_STATS
    case SYSRQ_SPINSTATS:
        dump_spinlock_stats();
        break;
#endif
    }
}

struct sysrq_key_op csr_key_op = {
    .handler = iodrive_handle_sysrq,
    .help_msg = "ioDrive CSR (z)",
    .action_msg = "ioDrive CSR status:",
#if KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK
    .enable_mask = 0,
#endif
};

struct sysrq_key_op software_key_op = {
    .handler = iodrive_handle_sysrq,
    .help_msg = "ioDrive software (l)",
    .action_msg = "ioDrive Software status:",
#if KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK
    .enable_mask = 0,
#endif
};

#if SPINLOCK_STATS
struct sysrq_key_op spinlock_stats_key_op = {
    .handler = iodrive_handle_sysrq,
    .help_msg = "ioDrive spinlock stats (g)",
    .action_msg = "ioDrive spinlock stats:",
#if KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK
    .enable_mask = 0,
#endif
};

#endif

void kfio_iodrive_sysrq_keys(void)
{
    register_sysrq_key(SYSRQ_CSR, &csr_key_op);
    register_sysrq_key(SYSRQ_SOFTWARE, &software_key_op);
#if SPINLOCK_STATS
    register_sysrq_key(SYSRQ_SPINSTATS, &spinlock_stats_key_op);
#endif
}

void kfio_iodrive_unreg_sysrq_keys(void)
{
    unregister_sysrq_key(SYSRQ_CSR, &csr_key_op);
    unregister_sysrq_key(SYSRQ_SOFTWARE, &software_key_op);
#if SPINLOCK_STATS
    unregister_sysrq_key(SYSRQ_SPINSTATS, &spinlock_stats_key_op);
#endif
}

