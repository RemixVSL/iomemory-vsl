//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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
#include <linux/stddef.h>
#include <linux/errno.h>
#include <linux/sysrq.h>
#include <linux/list.h>

#include <fio/port/kfio_config.h>
#include <fio/port/ktypes.h>
#include <fio/port/dbgset.h>

extern void fio_pcicheck_handle_sysrq(void);
extern void iodrive_dump_all(void);
extern void iodrive_dump_all_software_state(void);

#define SYSRQ_SOFTWARE  'l'
#define SYSRQ_CSR       'z'

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
        iodrive_dump_all();
        break;
    case SYSRQ_SOFTWARE:
        iodrive_dump_all_software_state();
        break;
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

void kfio_iodrive_sysrq_keys(void)
{
    register_sysrq_key(SYSRQ_CSR, &csr_key_op);
    register_sysrq_key(SYSRQ_SOFTWARE, &software_key_op);
}

void kfio_iodrive_unreg_sysrq_keys(void)
{
    unregister_sysrq_key(SYSRQ_CSR, &csr_key_op);
    unregister_sysrq_key(SYSRQ_SOFTWARE, &software_key_op);
}

