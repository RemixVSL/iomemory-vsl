// --------------------------------------------------------------------------
// Copyright (c) 2013-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
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
// --------------------------------------------------------------------------

// This file provides working or nonworking definitions for distros that are missing
// some midlayer stuff.

#ifndef _FIO_PORT_COMMON_LINUX_KSCSI_CONFIG
#define _FIO_PORT_COMMON_LINUX_KSCSI_CONFIG

#include <fio/port/kfio_config.h>
#include <fio/port/dbgset.h>
#include <fio/port/ktypes.h>
#include <linux/scatterlist.h>
#include <scsi/scsi_cmnd.h>

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

// Set to 0 to cause an error if SCSI is enabled and distro does not support SCSI.
// Set to 1 to allow compiling all distros with SCSI enabled
// (even if they might not function until the missing interfaces are created).
#define FIO_PORT_SCSI_ALLOW_COMPILE_BAD_DISTRO 1

struct scsi_cmnd;
struct Scsi_Host;
struct scatterlist;
struct page;

// Some linux variants don't define these macros
#ifndef set_driver_byte
#define set_driver_byte(cmnd, hsts) ((cmnd)->result |= (hsts) << 24)
#endif
#ifndef set_host_byte
#define set_host_byte(cmnd, dsts) ((cmnd)->result |= (dsts) << 16)
#endif

#if KFIOC_HAS_SCSI_SG_FNS && KFIOC_HAS_SCSI_RESID_FNS
#define KFIOC_PORT_SUPPORTS_SCSI 1
#else
#define KFIOC_PORT_SUPPORTS_SCSI 0
#endif

static inline void fio_port_scsi_unsupported_distro(void)
{
#if FIO_PORT_SCSI_ALLOW_COMPILE_BAD_DISTRO
    kassert_msg(false, "This distro does not support SCSI");
#else
#error ERROR: This distro does not support SCSI
#endif
}

#define UNSUP     { fio_port_scsi_unsupported_distro(); }
#define UNSUPR(r) { fio_port_scsi_unsupported_distro(); return r; }

// Distros that do not define for_each_sg don't have sg iteration.
// The only user is kfio_sgl_map_scatter().
// Define what is needed here in a way that avoids unused variables.
#ifndef for_each_sg
static inline struct page *sg_page(struct scatterlist *sg) UNSUPR(NULL)
#define for_each_sg(sglist, sg, nr, __i)    \
        for (__i = 0, sg = (sglist); __i < (nr); )
#endif

#if KFIOC_HAS_SCSI_SG_FNS==0
static inline unsigned scsi_sg_count(struct scsi_cmnd *scmd) UNSUPR(0)
static inline struct scatterlist *scsi_sglist(struct scsi_cmnd *scmd) UNSUPR(NULL)
static inline unsigned scsi_bufflen(struct scsi_cmnd *scmd) UNSUPR(0)
static inline void *shost_priv(struct Scsi_Host *shost) UNSUPR(NULL)
#endif

#if KFIOC_HAS_SCSI_SG_COPY_FNS==0
// implementation for 2.6.18
static inline int scsi_sg_copy_from_buffer(struct scsi_cmnd *cmd, void *buf, int buflen)
{
    kassert(cmd->scatterlen == 0);
    memcpy(cmd->request_buffer, buf, buflen);
    return 0;
}

static inline int scsi_sg_copy_to_buffer(struct scsi_cmnd *cmd, void *buf, int buflen)
{
    kassert(cmd->scatterlen == 0);
    memcpy(buf, cmd->request_buffer, buflen);
    return 0;
}
#endif

#if KFIOC_HAS_SCSI_RESID_FNS==0
static inline void scsi_set_resid(struct scsi_cmnd *scmd, int resid) UNSUP
static inline int scsi_get_resid(struct scsi_cmnd *scmd) UNSUPR(0)
#endif

/**
 * @}
 */

#endif // _FIO_PORT_COMMON_LINUX_KSCSI_CONFIG
