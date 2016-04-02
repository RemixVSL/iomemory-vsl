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

/** @file
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KFILE_H__
#define __FIO_PORT_KFILE_H__

typedef void fusion_proc_dir_entry;
typedef void fusion_file;
typedef void fusion_seq_file;
typedef void fusion_module_t;
typedef void fusion_inode;

/**
 * brief @struct __fusion_seq_operations
 * OS-independent structure for seq file operations.
 */
struct __fusion_seq_operations
{
    DECLARE_RESERVE(64);
};

typedef struct __fusion_seq_operations fusion_seq_operations_t;

/**
 * brief @struct __fusion_file_operations
 * OS-independent structure for file operations.
 */
struct __fusion_file_operations
{
    DECLARE_RESERVE(256);
};

typedef struct __fusion_file_operations fusion_file_operations_t;

extern void * kfio_fs_private_data(fusion_file *fp);
extern fusion_inode * kfio_fs_inode(fusion_file *fp);
extern void * kfio_fseq_private(fusion_seq_file *sp);
extern void kfio_fseq_set_private(fusion_seq_file *sp, void *pv);
extern int kfio_seq_printf(fusion_seq_file *sp, const char *f, ...);
extern fio_size_t kfio_seq_get_buf(fusion_seq_file *sp, char **bufp);
extern void kfio_seq_commit(fusion_seq_file *sp, int num);

extern fusion_proc_dir_entry * kfio_inode_proc_dir(fusion_inode *ip);
extern void * kfio_inode_data(fusion_inode *ip);

extern int kfio_single_open(fusion_file *fp, int (*show_proc)(fusion_seq_file *, void *), void *data);

extern int kfio_seq_open(fusion_file *fp, fusion_seq_operations_t *pop);
extern fio_ssize_t kfio_seq_read(fusion_file *fp, char *p, fio_size_t sz, fio_loff_t *poff);
extern fio_loff_t kfio_seq_lseek(fusion_file *fp, fio_loff_t off, int flg);
extern int kfio_seq_release(fusion_inode *inode, fusion_file *fp);
/****/
extern fusion_proc_dir_entry * kfio_proc_mkdir(const char *name, fusion_proc_dir_entry *parent);
extern void kfio_remove_proc_entry(const char *name, fusion_proc_dir_entry *parent);
extern fusion_proc_dir_entry * kfio_create_proc_fops_entry(const char *name,
    fio_mode_t mode, fusion_proc_dir_entry *base,
    fusion_file_operations_t *fops, void *data);

extern void kfio_set_file_ops_owner(fusion_file_operations_t *fops, void *owner);
extern void kfio_set_file_ops_llseek_handler(fusion_file_operations_t *fops, void *llseek);
extern void kfio_set_file_ops_read_handler(fusion_file_operations_t *fops, void *read);
extern void kfio_set_file_ops_write_handler(fusion_file_operations_t *fops, void *write);
extern void kfio_set_file_ops_open_handler(fusion_file_operations_t *fops, void *open);
extern void kfio_set_file_ops_release_handler(fusion_file_operations_t *fops, void *release);

extern void kfio_set_seq_ops_start_handler(fusion_seq_operations_t *sops, void *start);
extern void kfio_set_seq_ops_stop_handler(fusion_seq_operations_t *sops, void *stop);
extern void kfio_set_seq_ops_next_handler(fusion_seq_operations_t *sops, void *next);
extern void kfio_set_seq_ops_show_handler(fusion_seq_operations_t *sops, void *show);

#endif //__FIO_PORT_KFILE_H__
