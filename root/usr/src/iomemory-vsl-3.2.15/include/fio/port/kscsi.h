//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

// This file contains the SCSI driver interfaces between the fio-scsi and fio-port layers.
// It is where everything that must be shared between the layers has to live.

#ifndef __FIO_PORT_KSCSI_H__
#define __FIO_PORT_KSCSI_H__

#include <fio/port/ktypes.h>
#include <fio/port/dbgset.h>

#if defined(__linux__)
#include <fio/port/common-linux/kscsi_host.h>
#elif !defined(USERSPACE_KERNEL)
#error Unsupported OS - if you are porting, try starting with a copy of stubs
#endif

/// @defgroup SCSIAPI VSL SCSI API

/// @defgroup SCSIAPI-core port => scsi interfaces
/// @ingroup SCSIAPI
/// @{

// global definitions & interfaces

extern void kfio_scsi_cdb_tostring(char *s, unsigned s_len, const uint8_t *cdb);
extern void dbgkprint_hex(const char *prefix, const uint8_t *hex, unsigned len);

#define FIO_SCSI_CMD_LEN_MAX 16    ///< The max CDB size we support.

/* Versions of dbgprint() specifically for SCSI, requiring CDB-related routines
 * and structure definitions.
 */
#if (defined(DBGSET_COMPILE_DBGPRINT_OUT) && DBGSET_COMPILE_DBGPRINT_OUT) || \
       FUSION_INTERNAL==0

#define dbgprint_cdb(_df_flag, _cdb, ...)  do {} while(0)

#define errprint_cdb(_cdb, ...)            do {} while(0)

#else

/// @brief dbgprint a message containing a SCSI CDB
/// @param _df_flag which df_flag to use with dbgprint.
/// @param _cdb     a const uint8_t* which points to the CDB data.
/// @param ...      the format string and arguments
/// @note  Use _cdb_str as the argument which has the ASCII CDB string.
#define dbgprint_cdb(_df_flag, _cdb, ...)                                  \
    do {                                                                   \
        if (_dbgset.flags[_DF_ ## _df_flag].value)                         \
        {                                                                  \
            char _cdb_str[FIO_SCSI_CMD_LEN_MAX * 3 + 1];                   \
            kfio_scsi_cdb_tostring(_cdb_str, sizeof(_cdb_str), _cdb);       \
            dbgkprint(__VA_ARGS__);                                        \
        }                                                                  \
    } while (0)

/// @brief errprint a message containing a SCSI CDB
/// @param _cdb     a const uint8_t* which points to the CDB data.
/// @param ...      the format string and arguments
/// @note  Use _cdb_str as the argument which has the ASCII CDB string.
#define errprint_cdb(_cdb, ...)                                        \
    do {                                                               \
        char _cdb_str[FIO_SCSI_CMD_LEN_MAX * 3 + 1];                   \
        kfio_scsi_cdb_tostring(_cdb_str, sizeof(_cdb_str), _cdb);       \
        errprint(__VA_ARGS__);                                         \
    } while (0)

#endif

/// @brief SCSI status values. SAM3r14 section 5.3.1 table 22.
enum fio_scsi_cmd_status
{
    FIO_SCSI_CMD_STAT_GOOD                  = 0x00,
    FIO_SCSI_CMD_STAT_CHECK                 = 0x02,
    FIO_SCSI_CMD_STAT_COND_MET              = 0x04,
    FIO_SCSI_CMD_STAT_BUSY                  = 0x08,
    FIO_SCSI_CMD_STAT_INTERMEDIATE          = 0x10,
    FIO_SCSI_CMD_STAT_INTERMEDIATE_COND_MET = 0x14,
    FIO_SCSI_CMD_STAT_RES_CONFLICT          = 0x18,
    FIO_SCSI_CMD_STAT_QFULL                 = 0x28,
    FIO_SCSI_CMD_STAT_ACA                   = 0x30,
    FIO_SCSI_CMD_STAT_TASK_ABRT             = 0x40,
};

struct scsi_cmnd;
struct fio_scsi_cmd;
struct fio_scsi_lu;
struct kfio_bio;
extern int kfio_scsi_init(void);
extern void kfio_scsi_cleanup(void);

// SCSI command interfaces
extern int kfio_scsi_cmd_alloc(struct fio_scsi_cmd **cmdr, const uint8_t *cdb, struct fio_scsi_lu *lu, void *port_cmd);
extern int kfio_scsi_cmd_rcvd(struct fio_scsi_cmd *cmd);
extern const char *kfio_scsi_cmd_get_bus_name(const struct fio_scsi_cmd *cmd);
extern uint32_t kfio_scsi_cmd_get_global_seq_num(const struct fio_scsi_cmd *cmd);
extern struct fio_scsi_lu *kfio_scsi_cmd_get_lu(const struct fio_scsi_cmd *cmd);

// SCSI logical unit interfaces
extern uint32_t kfio_scsi_lu_get_target_id(const struct fio_scsi_lu *lu);
extern uint32_t kfio_scsi_lu_get_lun(const struct fio_scsi_lu *lu);
extern const char *kfio_scsi_lu_get_bus_name(const struct fio_scsi_lu *lu);
extern kfio_pci_dev_t *kfio_scsi_lu_get_pci_dev(const struct fio_scsi_lu *lu);
extern void *kfio_scsi_lu_get_port_lu(const struct fio_scsi_lu *lu);
extern void kfio_scsi_lu_set_port_lu(struct fio_scsi_lu *lu, void *port_lu);
extern struct fio_scsi_lu *kfio_scsi_lu_lookup_from_tid_lun(uint32_t tid, uint32_t lun);
extern int kfio_scsi_lu_abort_cmd(struct fio_scsi_lu *lu, struct fio_scsi_cmd *cmd);
extern int kfio_scsi_lu_reset_all_in_target(uint32_t tid);

/// @}

/// @defgroup SCSIAPI-port scsi => port interfaces
/// @ingroup SCSIAPI
/// @{

// SCSI logical unit interfaces

/// @brief A new logical unit has been exposed.
extern int kfio_port_scsi_new_lu(struct fio_scsi_lu *lu);

/// @brief A logical unit has been hidden. This should cause all incoming commands to fail.
extern void kfio_port_scsi_lu_hidden(struct fio_scsi_lu *lu);

/// @brief A logical unit has been removed. If this is the last one, destroy the host.
extern void kfio_port_scsi_lu_gone(struct fio_scsi_lu *lu);

// SCSI command interfaces

/// @brief Allow port to tweak the SCSI inquiry response we just constructed before we respond with it.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param lu               The associated fio_scsi_lu struct, for convenience
/// @param inq_cdb          The incoming inquiry CDB
/// @param response         The buffer where the response has been constructed
/// @param response_len     The response buffer size
extern void kfio_port_scsi_cmd_tweak_inq_response(void *port_cmd, const struct fio_scsi_lu *lu, const uint8_t *inq_cdb,
                                                 uint8_t *response, uint32_t response_len);

/// @brief Where this command's SCSI sense data should be populated.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param buffer           buffer
/// @param buffer_length    buffer size
extern void kfio_port_scsi_cmd_get_sense_buffer(void *port_cmd, uint8_t **buffer, uint32_t *buffer_length);

/// @brief Copy a block of data to this command's scatter-gather buffer.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param src_buffer       Pointer to the data block
/// @param len              The number of bytes in the data block
extern int kfio_port_scsi_cmd_copy_to_sg(void *port_cmd, void *src_buffer, uint32_t len);

/// @brief Copy a block of data from this command's scatter-gather buffer.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param dst_buffer       Pointer to the data block
/// @param len              The number of bytes in the data block
extern int kfio_port_scsi_cmd_copy_from_sg(void *port_cmd, void *dst_buffer, uint32_t len);

/// @brief Map this command's scatter-gather buffer for DMA use.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param fbio             The bio to map
/// @param alignment        The required alignment for DMA buffers
extern int kfio_port_scsi_cmd_map_kfio_bio(void *port_cmd, struct kfio_bio *fbio, uint32_t alignment);

/// @brief This command is complete. Return it to the OS's initiator.
/// @param port_cmd         The port_cmd argument passed into the kfio_scsi_cmd_alloc function call for this command
/// @param bytes_xferred    The number of bytes transferred by this command
/// @param scsi_status      The SCSI status for this command (non-0 indicates an error occurred after this command was queued)
/// @param driver_error     The -errno value for an error which occurred before this command was processed
/// @note There are two cases for aborted commands (we distinguish between them as a debug aid):
///       1) IOs aborted after submitted to the bio engine will have scsi_status check condition with sense data indicating aborted command.
///       2) IOs aborted before submitted to the bio engine will have driver_error -EAGAIN.
extern void kfio_port_scsi_cmd_completor(void *port_cmd, uint64_t bytes_xferred, enum fio_scsi_cmd_status scsi_status, int driver_error);

/// @}

#endif /* __FIO_PORT_KSCSI_H__ */
