//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/** @file include/fio/port/kfile.h
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

typedef struct __fusion_seq_operations fusion_seq_operations_t;
typedef struct __fusion_file_operations fusion_file_operations_t;

typedef int (fusion_read_proc_t) (char *page, char **start, fio_off_t off, int count, int *eof, void *data);
typedef int (fusion_write_proc_t) (fusion_file *file, const char *buffer, unsigned long count, void *data);

extern fusion_inode * kfio_fs_inode(fusion_file *fp);
extern void * kfio_fs_private_data(fusion_file *fp);
extern void * kfio_fseq_private(fusion_seq_file *sp);
extern void kfio_fseq_set_private(fusion_seq_file *sp, void *pv);
extern int kfio_seq_printf(fusion_seq_file *sp, const char *f, ...);
extern fio_size_t kfio_seq_get_buf(fusion_seq_file *sp, char **bufp);
extern void kfio_seq_commit(fusion_seq_file *sp, int num);

extern fusion_proc_dir_entry * kfio_inode_proc_dir(fusion_inode *ip);
extern void * kfio_inode_data(fusion_inode *ip);

/****/
extern fusion_proc_dir_entry * kfio_proc_mkdir(const char *name, fusion_proc_dir_entry *parent);
extern void kfio_remove_proc_entry(const char *name, fusion_proc_dir_entry *parent);
extern fusion_proc_dir_entry * kfio_create_proc_read_entry(const char *name,
    fio_mode_t mode, fusion_proc_dir_entry *base, 
    fusion_read_proc_t *read_proc, void *data);
extern fusion_proc_dir_entry * kfio_create_proc_write_entry(const char *name,
    fio_mode_t mode, fusion_proc_dir_entry *base, 
    fusion_write_proc_t *write_proc, void *data);

extern fusion_proc_dir_entry * kfio_create_proc_fops_entry(const char *name,
    fio_mode_t mode, fusion_proc_dir_entry *base, 
    fusion_file_operations_t *fops, void *data);


extern void kfio_set_write_proc_dir_entry(fusion_proc_dir_entry *entry, fusion_write_proc_t *write_proc);
extern void kfio_set_owner_dir_entry(fusion_proc_dir_entry *entry, void *owner);

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

