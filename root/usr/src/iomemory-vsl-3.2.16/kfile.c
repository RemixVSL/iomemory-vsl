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
// -----------------------------------------------------------------------------

#if !defined (__linux__)
#error This file supports linux only
#endif

#include "port-internal.h"

#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/signal.h>
#include <linux/poll.h>
#if KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
#include <linux/proc_fs.h>
#endif /* KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS */
#include <fio/port/common-linux/kfile.h>

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

C_ASSERT(sizeof(fusion_seq_operations_t) >= sizeof(struct seq_operations));
C_ASSERT(sizeof(fusion_file_operations_t) >= sizeof(struct file_operations));

/**
* @brief given a pointer to struct file returns a pointer to
* the associated struct inode of the dentry member
*/
fusion_inode * noinline kfio_fs_inode(fusion_file *fp)
{
    return (fusion_inode *) file_inode(fp);
}

/**
* @brief given a pointer to struct file returns a pointer to
* the private_data member of the file structure
*/
void * noinline kfio_fs_private_data(fusion_file *fp)
{
    return ((struct file *)fp)->private_data;
}

/**
* @brief given a pointer to struct seq_file returns a pointer to
* the private member of the file structure
*/
void * noinline kfio_fseq_private(fusion_seq_file *fsp)
{
    return ((struct seq_file *)fsp)->private;
}

/**
* @brief given a pointer to struct seq_file sets the private member
* to the passed in void pointer
*/
void noinline kfio_fseq_set_private(fusion_seq_file *sp, void *pv)
{
    ((struct seq_file *)sp)->private = pv;
}

/**
*
*/
int noinline kfio_seq_printf(fusion_seq_file *sp, const char *f, ...)
{
    struct seq_file *m = (struct seq_file *) sp;
    va_list args;
    int len;

    if (m->count < m->size) {
        va_start(args, f);
        len = vsnprintf(m->buf + m->count, m->size - m->count, f, args);
        va_end(args);
        if (m->count + len < m->size) {
            m->count += len;
            return 0;
        }
    }
    m->count = m->size;
    return -1;
}

fio_size_t noinline kfio_seq_get_buf(fusion_seq_file *sp, char **bufp)
{
    struct seq_file *m = (struct seq_file *) sp;

    if (m->size > m->count)
    {
        *bufp = m->buf + m->count;
        return m->size - m->count;
    }
    else
    {
        *bufp = NULL;
        return 0;
    }
}

void noinline kfio_seq_commit(fusion_seq_file *sp, int num)
{
    struct seq_file *m = (struct seq_file *) sp;

    if (num < 0)
    {
        m->count = m->size;
    }
    else
    {
        kassert(m->count + num <= m->size);
        m->count += num;
    }
}

/**
*
*/
void *noinline kfio_inode_data(fusion_inode *ip)
{
#if KFIOC_HAS_PROCFS_PDE_DATA
    return PDE_DATA(ip);
#else
    /* get proc_dir_entry that contains this inode */
    struct proc_dir_entry *pe = PDE((struct inode *) ip);
    return pe->data;
#endif
}

/**
* @brief linux: calls single_open(..)
*/
int noinline kfio_single_open(fusion_file *fp, int (*show_proc)(fusion_seq_file *, void *), void *data)
{
    return single_open(fp, (int (*)(struct seq_file *, void *))show_proc, data);
}

/**
* @brief linux: calls single_release(..)
*/
int noinline kfio_single_release(fusion_inode *inode, fusion_file *fp)
{
    return single_release((struct inode *) inode, (struct file *) fp);
}

/**
* @brief linux: calls seq_open(..)
*/
int noinline kfio_seq_open(fusion_file *fp, fusion_seq_operations_t *pop)
{
    kassert(sizeof (struct seq_operations) <= sizeof (fusion_seq_operations_t) );
    return seq_open((struct file *) fp, (struct seq_operations *) pop);
}

/**
* @brief linux: calls seq_read(..)
*/
fio_ssize_t noinline kfio_seq_read(fusion_file *fp, char *p, fio_size_t sz, fio_loff_t *poff)
{
    return seq_read((struct file *) fp, p, sz, poff);
}

/**
* @brief linux: calls seq_lseek(..)
*/
fio_loff_t noinline kfio_seq_lseek(fusion_file *fp, fio_loff_t off, int flg)
{
    return seq_lseek((struct file *) fp, off, flg);
}


/**
* @brief linux: calls seq_release(..)
*/
int noinline kfio_seq_release(fusion_inode *inode, fusion_file *fp)
{
    return seq_release((struct inode *) inode, (struct file *) fp);
}

/***** struct proc_dir_entry functions ***/

/**
* @brief linux: calls proc_mkdir(..)
*/
fusion_proc_dir_entry * noinline kfio_proc_mkdir(const char *name, fusion_proc_dir_entry *parent)
{
    return proc_mkdir(name, (struct proc_dir_entry *) parent);
}

/**
* @brief linux: calls remove_proc_entry(..)
*/
void noinline kfio_remove_proc_entry(const char *name, fusion_proc_dir_entry *parent)
{
    remove_proc_entry(name, (struct proc_dir_entry *) parent);
}

/**
* @brief linux: calls create_proc_fops_entry(..)
*/
fusion_proc_dir_entry * noinline kfio_create_proc_fops_entry(const char *name,
    fio_mode_t mode, fusion_proc_dir_entry *base, fusion_file_operations_t *fops, void *data)
{
# if ! KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
    return proc_create_data(name, mode, base, (struct file_operations *)fops, data);
# elif KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
    return proc_create_data(name, mode, base, (struct proc_ops *)fops, data);
# endif /* KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS */
}

typedef fio_loff_t  (*file_ops_llseek_fn)  (struct file *, fio_loff_t, int);
typedef fio_ssize_t (*file_ops_read_fn)    (struct file *, char *, fio_size_t, fio_loff_t *);
typedef fio_ssize_t (*file_ops_write_fn)   (struct file *, const char *, fio_size_t, fio_loff_t *);
typedef int     (*file_ops_open_fn)    (struct inode *, struct file *);
typedef int     (*file_ops_release_fn) (struct inode *, struct file *);

typedef void * (*seq_ops_start_fn) (struct seq_file *m, fio_loff_t *pos);
typedef void   (*seq_ops_stop_fn)  (struct seq_file *m, void *v);
typedef void * (*seq_ops_next_fn)  (struct seq_file *m, void *v, fio_loff_t *pos);
typedef int    (*seq_ops_show_fn)  (struct seq_file *m, void *v);

void kfio_set_seq_ops_start_handler(fusion_seq_operations_t *sops, void *start)
{
    ((struct seq_operations *)sops)->start = (seq_ops_start_fn)start;
}

void kfio_set_seq_ops_stop_handler(fusion_seq_operations_t *sops, void *stop)
{
    ((struct seq_operations *)sops)->stop = (seq_ops_stop_fn)stop;
}

void kfio_set_seq_ops_next_handler(fusion_seq_operations_t *sops, void *next)
{
    ((struct seq_operations *)sops)->next = (seq_ops_next_fn)next;
}

void kfio_set_seq_ops_show_handler(fusion_seq_operations_t *sops, void *show)
{
    ((struct seq_operations *)sops)->show = (seq_ops_show_fn)show;
}

#if ! KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
void kfio_set_file_ops_owner(fusion_file_operations_t *fops, void *owner)
{
    ((struct file_operations *)fops)->owner = (struct module *)owner;
}

void kfio_set_file_ops_llseek_handler(fusion_file_operations_t *fops, void *llseek)
{
    ((struct file_operations *)fops)->llseek = (file_ops_llseek_fn)llseek;
}

void kfio_set_file_ops_read_handler(fusion_file_operations_t *fops, void *read)
{
    ((struct file_operations *)fops)->read = (file_ops_read_fn)read;
}

void kfio_set_file_ops_write_handler(fusion_file_operations_t *fops, void *write)
{
    ((struct file_operations *)fops)->write = (file_ops_write_fn)write;
}

void kfio_set_file_ops_open_handler(fusion_file_operations_t *fops, void *open)
{
    ((struct file_operations *)fops)->open = (file_ops_open_fn)open;
}

void kfio_set_file_ops_release_handler(fusion_file_operations_t *fops, void *release)
{
    ((struct file_operations *)fops)->release = (file_ops_release_fn)release;
}
#elif KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
/* TODO: check if we need to make the full conversion to proc_ops, now we cheat
   our way through it by just pretending we're dumb deaf and blind, and reuse
   the botched allocated memory that was there before
*/
void kfio_set_file_ops_owner(fusion_file_operations_t *fops, void *owner)
{
}

void kfio_set_file_ops_llseek_handler(fusion_file_operations_t *pops, void *llseek)
{
    ((struct proc_ops *)pops)->proc_lseek = (file_ops_llseek_fn)llseek;
}

void kfio_set_file_ops_read_handler(fusion_file_operations_t *pops, void *read)
{
    ((struct proc_ops *)pops)->proc_read = (file_ops_read_fn)read;
}

void kfio_set_file_ops_write_handler(fusion_file_operations_t *pops, void *write)
{
    ((struct proc_ops *)pops)->proc_write = (file_ops_write_fn)write;
}

void kfio_set_file_ops_open_handler(fusion_file_operations_t *pops, void *open)
{
    ((struct proc_ops *)pops)->proc_open = (file_ops_open_fn)open;
}

void kfio_set_file_ops_release_handler(fusion_file_operations_t *pops, void *release)
{
    ((struct proc_ops *)pops)->proc_release = (file_ops_release_fn)release;
}

#endif /* ! KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS */

/**
 * @}
 */
