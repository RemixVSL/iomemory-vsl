// --------------------------------------------------------------------------
// Copyright (c) 2013-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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
// --------------------------------------------------------------------------

// This file contains a shim between the following layers for use with linux variants:
// 1) the fio-scsi layer
// 1) the linux SCSI mid-layer
//    - The scsi_host_template contains the function pointers that we implement.
//    - Our fio_port_scsi_host is our companion to the mid-layer's Scsi_Host.
//    - We create one Scsi_Host for each unique target id.

#include <scsi/scsi_host.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi.h>
#include <fio/port/kscsi.h>
#include <fio/port/ktypes.h>
#include <fio/port/dbgset.h>
#include <fio/port/common-linux/kscsi_config.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/blkdev.h>   // for BLK_EH_NOT_HANDLED

// See CRT-23. ESX asserts on DID_BAD_TARGET; it expects DID_NO_CONNECT.
#if defined(__VMKLNX__)
#undef DID_BAD_TARGET
#define DID_BAD_TARGET DID_NO_CONNECT
#endif

static void fio_init_scsi_host_template(void);

// ------------------------------------
// structure definitions

/// @brief SCSI midlayer interaction statistics
struct fio_port_scsi_host_stats
{
    uint64_t tot_cmds_completed;           ///< total number of SCSI commands completed back to the midlayer
    uint64_t tot_cmds_completed_err;       ///< total number of SCSI commands with an error back to the midlayer
    uint64_t tot_aborts;                   ///< total number of SCSI aborts from the midlayer
    uint64_t tot_device_resets;            ///< total number of device resets from the midlayer
    uint64_t tot_bus_resets;               ///< total number of bus resets from the midlayer
    uint64_t tot_host_resets;              ///< total number of host resets from the midlayer
};

/// @brief Internal representation of the linux midlayer scsi host.
struct fio_port_scsi_host
{
    struct Scsi_Host *shost;               ///< Pointer to midlayer companion structure
    fusion_spinlock_t shost_lk;            ///< Lock for modifying some midlayer fields
    fusion_list_entry_t port_hosts_node;   ///< port_hosts list node
    fusion_atomic_t ref_count;             ///< How many LUs this host has
    uint32_t target_id;                    ///< The target id for this host
    struct fio_port_scsi_host_stats stats; ///< Statistics
};

static fusion_list_t port_hosts;                   ///< the list of port_hosts
static fusion_spinlock_t port_hosts_lk;            ///< the protector of the port_hosts list
static int port_hosts_initialized = 0;

static void fio_port_scsi_host_destroy(struct fio_port_scsi_host* port_host);

// ------------------------------------
// init & teardown

/// @brief One-time initialization of the SCSI driver.
int kfio_init_storage_interface(void)
{
    int ret;

    fio_init_scsi_host_template();

    fusion_init_spin(&port_hosts_lk, "scsi_port_hosts_lock");
    fusion_init_list(&port_hosts);
    port_hosts_initialized = 1;

    ret = kfio_scsi_init();

    if (ret != 0)
    {
        kfio_teardown_storage_interface();
        return ret;
    }

    return 0;
}

/// @brief One-time cleanup of the SCSI driver.
int kfio_teardown_storage_interface(void)
{
    kfio_scsi_cleanup();

    if (port_hosts_initialized)
    {
        fusion_destroy_spin(&port_hosts_lk);
    }
    return 0;
}

// ------------------------------------
// Linux SCSI Mid Layer interfaces

#if KFIOC_HAS_NEW_QUEUECOMMAND_SIGNATURE
static int fio_shost_queuecommand(struct Scsi_Host *, struct scsi_cmnd *);
#else
static int fio_shost_queuecommand(struct scsi_cmnd *, void (*completor)(struct scsi_cmnd *));
#endif
static int fio_shost_abort(struct scsi_cmnd *);
static int fio_shost_device_reset(struct scsi_cmnd *);
static int fio_shost_bus_reset(struct scsi_cmnd *);
static int fio_shost_host_reset(struct scsi_cmnd *);

/// @brief Midlayer template where we tell it how to interact with us.
static struct scsi_host_template fio_templ =
{
    .module = THIS_MODULE,
    .name = "fio",
    .proc_name = "fio",
    .queuecommand = fio_shost_queuecommand,       ///< Call this to send us a SCSI command
    .eh_abort_handler = fio_shost_abort,          ///< Call this to abort an outstanding command
    .eh_device_reset_handler = fio_shost_device_reset,
    .eh_bus_reset_handler = fio_shost_bus_reset,
    .eh_host_reset_handler = fio_shost_host_reset,
    .this_id = -1,
    .sg_tablesize = SG_ALL,
    .max_sectors = FUSION_MAX_SECTORS_PER_OS_RW_REQUEST,      ///< Max IO size in sectors
    .use_clustering = DISABLE_CLUSTERING,
    // .ordered_tag = ?,
};

static struct fio_scsi_lu *fio_scsi_unhooked_lu = NULL;                        ///< What the midlayer will have in hostdata before we hook it up.

static void fio_init_scsi_host_template(void)
{
    fio_templ.can_queue = scsi_queue_depth;     // Max # SCSI commands driver can handle at a time
    fio_templ.cmd_per_lun = scsi_queue_depth;   // Max # SCSI commands each LUN can handle at a time
}

/// @brief Lookup our companion fio_port_scsi_host to the midlayer Scsi_Host.
static inline struct fio_port_scsi_host *fio_port_scsi_host_from_shost(struct Scsi_Host *shost)
{
    struct fio_port_scsi_host *port_host;

    port_host = (struct fio_port_scsi_host *)shost_priv(shost);
    return port_host->shost == shost ? port_host : NULL;
}

/// @brief Lookup our companion fio_port_scsi_host to the midlayer scsi_cmnd.
static inline struct fio_port_scsi_host *fio_port_scsi_host_from_scmd(struct scsi_cmnd *scmd)
{
    return fio_port_scsi_host_from_shost(scmd->device->host);
}

/// @brief Lookup our companion fio_scsi_lu to the midlayer scsi_device.
/// @note Assumes that the shost_lk is locked.
static inline struct fio_scsi_lu *fio_scsi_lu_from_sdev_locked(struct scsi_device *sdev)
{
    return (struct fio_scsi_lu *)sdev->hostdata;
}

/// @brief Lookup our companion fio_scsi_lu to the midlayer scsi_cmnd.
static inline struct fio_scsi_lu *fio_scsi_lu_from_scmd(struct scsi_cmnd *scmd)
{
    struct fio_port_scsi_host *port_host = fio_port_scsi_host_from_scmd(scmd);
    struct fio_scsi_lu *lu;

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    lu = fio_scsi_lu_from_sdev_locked(scmd->device);
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    return lu;
}

/// @brief Get the bus name from the scsi_cmnd.
static inline const char *fio_scsi_bus_name_from_scmd(struct scsi_cmnd *scmd)
{
    struct fio_scsi_lu *lu = fio_scsi_lu_from_scmd(scmd);

    return lu != fio_scsi_unhooked_lu ? kfio_scsi_lu_get_bus_name(lu) : "unknown";
}

/// @brief Unhook all of the scsi_devices from their attached fio_scsi_lu structures for a particular Scsi_Host.
/// @note This is mainly used to cause them to get re-hooked-up when the next SCSI command is sent.
static void fio_scsi_shost_unhook_devices_from_lus(struct Scsi_Host *shost)
{
    struct scsi_device *sdev;
    struct fio_port_scsi_host *port_host;

    port_host = fio_port_scsi_host_from_shost(shost);
    kassert(port_host != NULL);

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    shost_for_each_device(sdev, shost)
    {
        sdev->hostdata = fio_scsi_unhooked_lu;
    }
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);
}

/// @brief Lookup our companion fio_scsi_cmd to the midlayer scsi_cmnd.
static inline struct fio_scsi_cmd *fio_scsi_cmd_from_scmd(struct scsi_cmnd *scmd)
{
    return (struct fio_scsi_cmd *)scmd->host_scribble;
}

/// @brief Lookup a mid-layer scsi_device from our stuff. This keeps a ref.
static struct scsi_device *fio_port_scsi_device_get(struct fio_port_scsi_host *port_host, struct fio_scsi_lu *lu)
{
    return scsi_device_lookup(port_host->shost, 0, kfio_scsi_lu_get_target_id(lu), kfio_scsi_lu_get_lun(lu));
}

/// @brief Return a scsi_cmnd to the mid-layer before a fio_scsi_cmd was allocated.
static void fio_scmd_immediate_error(struct scsi_cmnd *scmd, uint8_t host_byte, const char *why)
{
    dbgprint(DBGS_SCSI_ERROR, "%s: MidLayer <=E->fio-port: SCSI cmd [hid %u, tid %u, lun %u, opcode %02x, host byte %02x] was immediately failed because %s\n",
            fio_scsi_bus_name_from_scmd(scmd), (scmd->device && scmd->device->host)? scmd->device->host->unique_id : 999,
            scmd->device ? scmd->device->id : 999, scmd->device ? scmd->device->lun : 999, scmd->cmnd[0], host_byte, why);

    scmd->result = 0;
    set_host_byte(scmd, host_byte);
    scmd->scsi_done(scmd);
}

#if KFIOC_HAS_NEW_QUEUECOMMAND_SIGNATURE
static int fio_shost_queuecommand(struct Scsi_Host *shost, struct scsi_cmnd *scmd)
#else
static int fio_shost_queuecommand(struct scsi_cmnd *scmd, void (*completor)(struct scsi_cmnd *))
#endif
{
    struct fio_scsi_lu *lu;
    struct fio_scsi_cmd *cmd;
    struct scsi_device *sdev;
    struct fio_port_scsi_host *port_host;
    struct Scsi_Host *sh;
    int cmd_status;
    int ret;

    kassert(scmd != NULL);

#if KFIOC_HAS_NEW_QUEUECOMMAND_SIGNATURE==0
    scmd->scsi_done = completor;
#endif
    kassert(scmd->scsi_done != NULL);

    sdev = scmd->device;
    kassert(sdev != NULL);

    sh = sdev->host;
    kassert(sh != NULL);

    port_host = fio_port_scsi_host_from_shost(sh);
    if (port_host == NULL)
    {
        fio_scmd_immediate_error(scmd, DID_NO_CONNECT, "host is unregistered");
        return 0;
    }

    if (sdev->id != port_host->target_id)
    {
        fio_scmd_immediate_error(scmd, DID_NO_CONNECT, "invalid target for host");
        return 0;
    }

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    lu = fio_scsi_lu_from_sdev_locked(sdev);
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    if (lu == fio_scsi_unhooked_lu)
    {
        // This is the first time we have had an IO on this sdev.
        // Set it up, & make sure we don't have to do this again.
        uint32_t num_luns;

        lu = kfio_scsi_lu_lookup_from_tid_lun(sdev->id, sdev->lun);
        if (lu == NULL)
        {
            fio_scmd_immediate_error(scmd, DID_TIME_OUT, "LU is not attached");
            return 0;
        }

        // Divide all the requests amoung the LUNs evenly.
        // FIXME This is fine for now because there is only 1 LUN per host (aka 1 VSU/device). However, I wonder if a more
        //       sophisticated mechanism would not yield better performance when there are more VSUs.
        num_luns = (uint32_t)fusion_atomic_read(&port_host->ref_count);
        scsi_adjust_queue_depth(sdev, 0, scsi_queue_depth/num_luns); // Max # SCSI commands this LUN can handle at a time.

        dbgprint(DBGS_SCSI, "%s: SCSI hooked up sdev to LU [tid %u, lun %u]\n",
            kfio_scsi_lu_get_bus_name(lu), sdev->id, sdev->lun);
        fusion_spin_lock_irqsave(&port_host->shost_lk);
        sdev->hostdata = lu;
        kfio_scsi_lu_set_port_lu(lu, sdev);
        fusion_spin_unlock_irqrestore(&port_host->shost_lk);
    }

    ret = kfio_scsi_cmd_alloc(&cmd, scmd->cmnd, lu, scmd);
    if (ret == -ENOMEM)
    {
        infprint("%s: scsi command %02x was not be handled due to lack of resources\n",
            fio_scsi_bus_name_from_scmd(scmd), scmd->cmnd[0]);
        return SCSI_MLQUEUE_HOST_BUSY;
    }
    if (ret == -ENOENT)
    {
        fio_scmd_immediate_error(scmd, DID_TIME_OUT, "LU just became detached");
        return 0;
    }

    dbgprint_cdb(DBGS_SCSI_IO2, scmd->cmnd,
        "%s: MidLayer ---> fio-port: SCSI cmd #%u [tid %u, lun %u, cdb %s]\n",
        kfio_scsi_cmd_get_bus_name(cmd), kfio_scsi_cmd_get_global_seq_num(cmd), sdev->id, sdev->lun, _cdb_str);

    scmd->host_scribble = (unsigned char *)cmd;

    cmd_status = kfio_scsi_cmd_rcvd(cmd);

    if (cmd_status == -ENOMEM)
    {
        return SCSI_MLQUEUE_HOST_BUSY;
    }

    if (cmd_status == 1)    // completion is pending
    {
        scmd->result = 0;
    }

    return 0;
}

/// @brief Call from midlayer to abort an outstanding SCSI command.
/// @note The application cannot assume anything about whether the command was or was not completed.
/// @return SUCCESS (which is not 0) or FAILED.
static int fio_shost_abort(struct scsi_cmnd *scmd)
{
    struct fio_port_scsi_host *port_host;
    struct fio_scsi_cmd *cmd;
    struct fio_scsi_lu *lu;
    int ret;

    dbgprint(DBGS_SCSI_ERROR, "%s: MidLayer -E-> fio-port: SCSI host [tid %u] abort\n",
        fio_scsi_bus_name_from_scmd(scmd), scmd->device->id);

    port_host = fio_port_scsi_host_from_scmd(scmd);
    cmd = fio_scsi_cmd_from_scmd(scmd);
    lu = fio_scsi_lu_from_scmd(scmd);

    // Some of this is paranoia checking to make sure we are aborting something reasonable.
    if (port_host == NULL || cmd == NULL || lu == fio_scsi_unhooked_lu ||
        lu != kfio_scsi_cmd_get_lu(cmd) || fio_port_scsi_host_from_scmd(scmd) == NULL)
    {
        ret = FAILED;
    }
    else
    {
        fusion_spin_lock_irqsave(&port_host->shost_lk);
        port_host->stats.tot_aborts++;
        fusion_spin_unlock_irqrestore(&port_host->shost_lk);

        ret = kfio_scsi_lu_abort_cmd(lu, cmd) == 0 ? SUCCESS : FAILED;
    }

    dbgprint(DBGS_SCSI_ERROR, "%s: MidLayer <=E= fio-port: SCSI host [tid %u] abort cmd #%u %s\n",
        fio_scsi_bus_name_from_scmd(scmd), scmd->device->id,
        cmd ? kfio_scsi_cmd_get_global_seq_num(cmd) : 0, ret == SUCCESS ? "SUCCESS" : "FAILED");

    return ret;
}

/// @brief Reset all LUs associated with the associated scsi_device (aka, target).
static int fio_shost_reset_all_lus(struct scsi_cmnd *scmd, const char *reason)
{
    dbgprint(DBGS_SCSI_ERROR, "%s: MidLayer ---> fio-port: SCSI host [tid %u] %s\n",
        fio_scsi_bus_name_from_scmd(scmd), scmd->device->id, reason);

    return kfio_scsi_lu_reset_all_in_target(scmd->device->id);
}

/// @brief Call from midlayer to reset the device. This should clear SCSI reservations.
/// @note Since we don't implement SCSI reservations, there is nothing extra to do.
/// @return SUCCESS (which is not 0) or FAILED.
static int fio_shost_device_reset(struct scsi_cmnd *scmd)
{
    struct fio_port_scsi_host *port_host = fio_port_scsi_host_from_scmd(scmd);

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    port_host->stats.tot_device_resets++;
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    return fio_shost_reset_all_lus(scmd, "device reset") == 0 ? SUCCESS : FAILED;
}

/// @brief Call from midlayer to reset the bus.
/// @return SUCCESS (which is not 0) or FAILED.
static int fio_shost_bus_reset(struct scsi_cmnd *scmd)
{
    struct fio_port_scsi_host *port_host = fio_port_scsi_host_from_scmd(scmd);

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    port_host->stats.tot_bus_resets++;
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    return fio_shost_reset_all_lus(scmd, "bus reset") == 0 ? SUCCESS : FAILED;
}

/// @brief Call from midlayer to reset the host.
/// @return SUCCESS (which is not 0) or FAILED.
static int fio_shost_host_reset(struct scsi_cmnd *scmd)
{
    struct fio_port_scsi_host *port_host = fio_port_scsi_host_from_scmd(scmd);

    fusion_spin_lock_irqsave(&port_host->shost_lk);
    port_host->stats.tot_host_resets++;
    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    return fio_shost_reset_all_lus(scmd, "host reset") == 0 ? SUCCESS : FAILED;
}

/// @brief Inform the midlayer about how much data was transferred.
/// @note The midlayer's residual (which tracks how much data remains to be transferred) is decremented.
static void fio_scmd_update_resid(struct scsi_cmnd *cmnd, uint64_t bytes_xfer)
{
    uint64_t resid;

    resid = (uint64_t)scsi_get_resid(cmnd);

    // We may have to determine the residual from the buffer length for the very 1st data transfer.
    if (resid == 0)
    {
        resid = scsi_bufflen(cmnd);
    }

    scsi_set_resid(cmnd, resid - bytes_xfer);
}

// ------------------------------------
// fio_port_scsi_host functions

/// @brief Find a fio_port_scsi_host from the target_id.
/// @note It is up to the caller to protect the list.
static struct fio_port_scsi_host *fio_port_scsi_host_lookup_from_tid(uint32_t tid)
{
    struct fio_port_scsi_host *port_host_iter;
    struct fio_port_scsi_host *port_host = NULL;

    fusion_list_for_each_entry(port_host_iter, &port_hosts, port_hosts_node, struct fio_port_scsi_host)
    {
        if (port_host_iter->target_id == tid)
        {
            port_host = port_host_iter;
            break;
        }
    }

    return port_host;
}

/// @brief Create a new fio_port_scsi_host and register it with the midlayer.
static struct fio_port_scsi_host *fio_port_scsi_host_create(uint32_t target_id, struct pci_dev *pdev)
{
    struct Scsi_Host *shost;
    struct fio_port_scsi_host *port_host;
    int ret;

    // Scsi_Host will contain our fio_port_scsi_host struct.
    if ((shost = scsi_host_alloc(&fio_templ, sizeof(*port_host))) == NULL)
    {
        return NULL;
    }
    port_host = (struct fio_port_scsi_host *)shost_priv(shost);

    kfio_memset(port_host, 0, sizeof(*port_host));
    fusion_init_spin(&port_host->shost_lk, "scsi_port_host");
    port_host->target_id = target_id;
    fusion_atomic_set(&port_host->ref_count, 0);

    shost->max_id = 64;         // This is really the max number of target ids
    shost->max_channel = 0;
    shost->max_lun = 1;
    shost->max_cmd_len = 16;
    shost->unique_id = target_id;

    ret = fio_port_scsi_host_register_host(shost, pdev);
    if (ret != 0)
    {
        fio_port_scsi_host_destroy(port_host);
        return NULL;
    }
    port_host->shost = shost;

    dbgprint(DBGS_SCSI, "SCSI host [tid %u] created.\n", target_id);

    return port_host;
}

/// @brief Unregister our fio_port_scsi_host from the midlayer and destroy it.
static void fio_port_scsi_host_destroy(struct fio_port_scsi_host* port_host)
{
    if (port_host)
    {
        struct Scsi_Host *shost = port_host->shost;

        if (shost)
        {
            port_host->shost = NULL;
            fio_port_scsi_host_unregister_host(shost);
        }

        fusion_destroy_spin(&port_host->shost_lk);
        dbgprint(DBGS_SCSI, "SCSI host [tid %u] destroyed.\n", port_host->target_id);

        if (shost)
        {
            scsi_host_put(shost);
        }
    }
}

/// @brief Tell the midlayer about a new logical unit.
static int fio_port_scsi_host_add_lu(struct fio_port_scsi_host *port_host, struct fio_scsi_lu *lu)
{
    return scsi_add_device(port_host->shost, 0, kfio_scsi_lu_get_target_id(lu), kfio_scsi_lu_get_lun(lu));
}

/// @brief Tell the midlayer that a logical unit is being removed.
static void fio_port_scsi_host_remove_lu(struct fio_port_scsi_host *port_host, struct fio_scsi_lu *lu)
{
    struct scsi_device *sdev;

    sdev = fio_port_scsi_device_get(port_host, lu);
    if (sdev)
    {
        scsi_remove_device(sdev);
        scsi_device_put(sdev);
    }
}

/// @brief Call from the fio-scsi layer that a new logical unit has been exposed.
int kfio_port_scsi_new_lu(struct fio_scsi_lu *lu)
{
    uint32_t target_id = kfio_scsi_lu_get_target_id(lu);
    struct fio_port_scsi_host *port_host;
    struct fio_port_scsi_host *other_thread_port_host;

    fusion_spin_lock(&port_hosts_lk);
    port_host = fio_port_scsi_host_lookup_from_tid(target_id);
    fusion_spin_unlock(&port_hosts_lk);

    if (port_host == NULL)
    {
        // Don't call out while locked because we cannot enforce non-blocking behavior.
        port_host = fio_port_scsi_host_create(target_id, (struct pci_dev *)kfio_scsi_lu_get_pci_dev(lu));
        if (port_host == NULL)
        {
            return -ENOMEM;
        }

        fusion_spin_lock(&port_hosts_lk);
        if ((other_thread_port_host = fio_port_scsi_host_lookup_from_tid(target_id)) != NULL)
        {
            // Another thread created this same host while we were unlocked. Destroy ours. Use his.
            fio_port_scsi_host_destroy(port_host);
            port_host = other_thread_port_host;
        }
        else
        {
            fusion_list_add_tail(&port_host->port_hosts_node, &port_hosts);
        }
        fusion_spin_unlock(&port_hosts_lk);
    }

    fusion_atomic_inc(&port_host->ref_count);

    // Force existing scsi_devices to recalculate their queue depth on the next command.
    fio_scsi_shost_unhook_devices_from_lus(port_host->shost);

    return fio_port_scsi_host_add_lu(port_host, lu);
}

/// @brief Call from the fio-scsi layer that one of the logical units has been hidden.
/// @note This should cause all incoming commands to fail while outstanding commands are drained.
void kfio_port_scsi_lu_hidden(struct fio_scsi_lu *lu)
{
    struct fio_port_scsi_host *port_host;
    struct scsi_device *sdev;

    fusion_spin_lock(&port_hosts_lk);
    port_host = fio_port_scsi_host_lookup_from_tid(kfio_scsi_lu_get_target_id(lu));
    fusion_spin_unlock(&port_hosts_lk);

    sdev = fio_port_scsi_device_get(port_host, lu);
    if (sdev)
    {
        fusion_spin_lock_irqsave(&port_host->shost_lk);
        sdev->hostdata = fio_scsi_unhooked_lu;
        kfio_scsi_lu_set_port_lu(lu, NULL);
        fusion_spin_unlock_irqrestore(&port_host->shost_lk);

        scsi_device_put(sdev);
    }
}

/// @brief Call from the fio-scsi layer that one of the logical units has been removed,
/// and if this is the last one, destroy the host.
void kfio_port_scsi_lu_gone(struct fio_scsi_lu *lu)
{
    struct fio_port_scsi_host *port_host;

    fusion_spin_lock(&port_hosts_lk);
    port_host = fio_port_scsi_host_lookup_from_tid(kfio_scsi_lu_get_target_id(lu));
    fusion_spin_unlock(&port_hosts_lk);

    if (port_host)
    {
        fio_port_scsi_host_remove_lu(port_host, lu);

        if (fusion_atomic_decr(&port_host->ref_count) == 0)
        {
            fusion_spin_lock(&port_hosts_lk);
            fusion_list_del(&port_host->port_hosts_node);
            fusion_spin_unlock(&port_hosts_lk);

            fio_port_scsi_host_destroy(port_host);
        }
        else
        {
            // Force remaining scsi_devices to recalculate their queue depth on the next command.
            fio_scsi_shost_unhook_devices_from_lus(port_host->shost);
        }
    }
}

/// @brief Call from fio-scsi layer allowing us to modify an inquiry response.
void kfio_port_scsi_cmd_tweak_inq_response(void *port_cmd, const struct fio_scsi_lu *lu, const uint8_t *inq_cdb,
                                          uint8_t *response, uint32_t response_len)
{
    // Nothing required here. We are satisfied with fio-scsi's responses.
}

/// @brief Call from fio-scsi layer asking where to put SCSI sense data.
void kfio_port_scsi_cmd_get_sense_buffer(void *port_cmd, uint8_t **buffer, uint32_t *buffer_length)
{
    struct scsi_cmnd *scmd = (struct scsi_cmnd *)port_cmd;

    if (buffer)
    {
        *buffer = scmd->sense_buffer;
    }

    if (buffer_length)
    {
        *buffer_length = SCSI_SENSE_BUFFERSIZE;
    }
}

/// @brief Call from fio-scsi layer when a command is complete.
void kfio_port_scsi_cmd_completor(void *port_cmd, uint64_t bytes_xferred, enum fio_scsi_cmd_status scsi_status,
                                 int driver_error)
{
    struct scsi_cmnd *scmd;
    struct fio_port_scsi_host *port_host;
    struct fio_scsi_cmd *cmd;
    char host_status;

    scmd = (struct scsi_cmnd *)port_cmd;
    kassert(scmd);

    port_host = fio_port_scsi_host_from_scmd(scmd);
    if (port_host == NULL)
    {
        return;
    }

    fusion_spin_lock_irqsave(&port_host->shost_lk);

    cmd = fio_scsi_cmd_from_scmd(scmd);
    if (cmd == NULL)
    {
        // This should not happen
        fusion_spin_unlock_irqrestore(&port_host->shost_lk);
        return;
    }
    scmd->host_scribble = NULL;

    port_host->stats.tot_cmds_completed++;

    scmd->result = scsi_status;     // set the status_byte

    // Determine the host_status
    if (driver_error)
    {
        if (driver_error == -EAGAIN)
        {
            // An unsubmitted cmd that was aborted.
            host_status = DID_TIME_OUT;     // transient failure
        }
        else
        {
            host_status = DID_NO_CONNECT;   // permanent OS failure
        }
    }
    else if (scsi_status == FIO_SCSI_CMD_STAT_CHECK)
    {
        // We are returning a SCSI Check Condition.
        // This always comes with "sense data", as defined in SPC-3.
        // Here, we look at the sense data that was generated by fio-scsi to
        // determine an appropriate host_status byte.
        // We know fio-scsi always generates "fixed format" sense data, and there
        // are no deferred errors.

        uint8_t response_code = scmd->sense_buffer[0] & 0x7f;   // as defined in SPC-3

        if (response_code == 0x70)  // valid fixed-format (current errors) sense data
        {
            uint8_t sense_key = scmd->sense_buffer[2] & 0xf;    // as defined in SPC-3

            if (sense_key == HARDWARE_ERROR)
            {
                host_status = DID_BAD_TARGET;   // permanent device failure
            }
            else
            {
                // A cmd that had a transient error.
                // A submitted cmd that was aborted will be here with a sense key of aborted command.
                host_status = DID_OK;
            }
        }
        else
        {
            // A check condition with no valid sense data. Should not happen.
            host_status = DID_NO_CONNECT;       // permanent OS failure
        }
    }
    else
    {
        host_status = DID_OK;
    }

    if (unlikely(host_status != DID_OK || scsi_status != FIO_SCSI_CMD_STAT_GOOD))
    {
        port_host->stats.tot_cmds_completed_err++;
    }

    fusion_spin_unlock_irqrestore(&port_host->shost_lk);

    if (host_status != DID_OK)
    {
        set_host_byte(scmd, host_status);
    }
    else if (scsi_status == FIO_SCSI_CMD_STAT_CHECK)
    {
        set_driver_byte(scmd, DRIVER_SENSE);    // I'm not sure the LLD is supposed to be doing this, but should be harmless
    }

    if (likely(bytes_xferred > 0))
    {
        fio_scmd_update_resid(scmd, bytes_xferred);
    }

    // log the completion
    if (unlikely(scmd->result))
    {
        dbgprint_cdb(DBGS_SCSI_ERROR, scmd->cmnd,
            "%s: MidLayer <=E= fio-port: SCSI cmd #%u [tid %u, lun %u, cdb %s] result %x\n",
            kfio_scsi_cmd_get_bus_name(cmd), kfio_scsi_cmd_get_global_seq_num(cmd),
            scmd->device->id, scmd->device->lun, _cdb_str, scmd->result);
    }
    else
    {
        dbgprint_cdb(DBGS_SCSI_IO2, scmd->cmnd,
            "%s: MidLayer <=== fio-port: SCSI cmd #%u [tid %u, lun %u, cdb %s] %lld/%llu bytes xferred\n",
            kfio_scsi_cmd_get_bus_name(cmd), kfio_scsi_cmd_get_global_seq_num(cmd),
            scmd->device->id, scmd->device->lun, _cdb_str, bytes_xferred, (uint64_t)scsi_bufflen(scmd));
    }

    // We handle out of memory conditions in queuecommand.
    if (driver_error != -ENOMEM)
    {
        scmd->scsi_done(scmd);
    }
}
