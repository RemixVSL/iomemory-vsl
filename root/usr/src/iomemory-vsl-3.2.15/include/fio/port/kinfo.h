//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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

#ifndef FIO_PORT_KINFO_H
#define FIO_PORT_KINFO_H

typedef struct kfio_info_val     kfio_info_val_t;
typedef struct kfio_info_data    kfio_info_data_t;
typedef struct kfio_info_node    kfio_info_node_t;
typedef struct kfio_info_seq_ops kfio_info_seq_ops_t;

typedef int    kfio_info_type_handler_t(void *param, int cmd, kfio_info_val_t *oldval,
                                        kfio_info_val_t *newval);
typedef int    kfio_info_text_handler_t(void *param, int cmd, kfio_info_data_t *handle);

#define KFIO_INFO_DIR      0x01
#define KFIO_INFO_INT32    0x02
#define KFIO_INFO_UINT32   0x03
#define KFIO_INFO_UINT64   0x04
#define KFIO_INFO_STRING   0x05
#define KFIO_INFO_TEXT     0x06
#define KFIO_INFO_SEQFILE  0x07

#define KFIO_INFO_READ     0x0100
#define KFIO_INFO_WRITE    0x0080
#define KFIO_INFO_RDWR     (KFIO_INFO_READ | KFIO_INFO_WRITE)

extern kfio_info_node_t *fusion_info_root;

extern int  kfio_info_create_dir(kfio_info_node_t *parent, const char *name, kfio_info_node_t **newdir);
extern int  kfio_info_create_type(kfio_info_node_t *parent, const char *name, int type, fio_mode_t mode,
                                  void *data, fio_size_t size);
extern int  kfio_info_create_proc(kfio_info_node_t *parent, const char *name, int type, fio_mode_t mode,
                                  kfio_info_type_handler_t *handler, void *param, fio_size_t size);
extern int  kfio_info_create_seqf(kfio_info_node_t *parent, const char *name, fio_mode_t mode,
                                  kfio_info_seq_ops_t *ops, void *param);
extern int  kfio_info_create_text(kfio_info_node_t *parent, const char *name, fio_mode_t mode,
                                  kfio_info_text_handler_t *handler, void *param);
extern void kfio_info_remove_node(kfio_info_node_t **dirp);

extern int    kfio_info_node_get_type(kfio_info_node_t *node);
extern fio_ssize_t kfio_info_node_get_size(kfio_info_node_t *node);
extern fio_mode_t  kfio_info_node_get_mode(kfio_info_node_t *node);
extern const char *kfio_info_node_get_name(kfio_info_node_t *node);
extern void       *kfio_info_node_get_data(kfio_info_node_t *node);

extern void *kfio_info_node_get_os_private(kfio_info_node_t *node);
extern void  kfio_info_node_set_os_private(kfio_info_node_t *node, void *priv);

/*
 * Helpers to manipulate trasparent data handle
 */
extern int  kfio_info_alloc_data_handle(kfio_info_node_t *node, void *data, fio_size_t size,
                                        kfio_info_data_t **dbhp);
extern void kfio_info_reset_data_handle(kfio_info_node_t *node, void *data, fio_size_t size,
                                        kfio_info_data_t *dbh);
extern void kfio_info_free_data_handle(kfio_info_data_t *dbh);

extern fio_ssize_t kfio_info_printf(kfio_info_data_t *dbh, const char *fmt, ...)
#ifdef __GNUC__
__attribute__((format(printf,2,3)))
#endif
;
extern fio_ssize_t kfio_info_write(kfio_info_data_t *dbh, const void *data, fio_size_t size);

extern int  kfio_info_data_overflow(kfio_info_data_t *handle);
kfio_info_node_t *kfio_info_data_node(kfio_info_data_t *dbh);

extern fio_size_t kfio_info_data_size_limit(kfio_info_data_t *handle);
extern fio_size_t kfio_info_data_size_valid(kfio_info_data_t *handle);
extern fio_size_t kfio_info_data_size_written(kfio_info_data_t *handle);
extern fio_size_t kfio_info_data_size_free(kfio_info_data_t *handle);
extern void kfio_info_need_buffer_size(kfio_info_data_t *dbh, fio_size_t size);

extern int  kfio_info_data_get_eof(kfio_info_data_t *handle);
extern void kfio_info_data_set_eof(kfio_info_data_t *handle, int flag);

/*
 * Handle data exchange.
 */
struct kfio_info_val
{
    int         type;
    fio_ssize_t size;
    void       *data;
};

extern int kfio_info_handle_cmd(int cmd, kfio_info_val_t *oldval, kfio_info_val_t *newval, void *data);
extern int kfio_info_generic_type_handler(kfio_info_node_t *nodep, int cmd,
                                          kfio_info_val_t *oldval, kfio_info_val_t *newval);
extern int kfio_info_generic_text_handler(kfio_info_node_t *nodep, kfio_info_data_t *dbh);


/*
 * Wrappers around sequential operations. Patterned after corresponding sequential_file operations
 * in Linux.
 */
typedef void *(*kfio_info_seq_op_init)(void *param, fio_loff_t *pos, kfio_info_data_t *dbh);
typedef void *(*kfio_info_seq_op_next)(void *param, void *cookie, fio_loff_t *pos);
typedef void  (*kfio_info_seq_op_stop)(void *param, void *cookie);
typedef  int  (*kfio_info_seq_op_show)(void *param, void *cookie, kfio_info_data_t *dbh);

struct kfio_info_seq_ops
{
    kfio_info_seq_op_init seq_init;
    kfio_info_seq_op_next seq_next;
    kfio_info_seq_op_stop seq_stop;
    kfio_info_seq_op_show seq_show;
};

extern void *kfio_info_seq_init(kfio_info_node_t *node, fio_loff_t *pos, kfio_info_data_t *dbh);
extern void *kfio_info_seq_next(kfio_info_node_t *node, void *cookie, fio_loff_t *pos);
extern void  kfio_info_seq_stop(kfio_info_node_t *node, void *cookie);
extern  int  kfio_info_seq_show(kfio_info_node_t *node, void *cookie, kfio_info_data_t *dbh);

/*
 * OS-specific hooks.
 */
extern int  kfio_info_os_create_node(kfio_info_node_t *parent, kfio_info_node_t *nodep);
extern void kfio_info_os_remove_node(kfio_info_node_t *parent, kfio_info_node_t *nodep);

struct fusion_rw_proc;

/*
 * IOCTL interface.
 */
extern int kfio_info_ioctl_proc(struct fusion_rw_proc *procp, void *user_ptr);

#endif /* FIO_PORT_KINFO_H */
