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

#include <linux/module.h>

#include <fio/port/fio-port.h>
#include <fio/port/kinfo.h>
#include <fio/port/common-linux/kfile.h>

static fusion_file_operations_t kfio_info_type_fops;
static fusion_file_operations_t kfio_info_text_fops;
static fusion_file_operations_t kfio_info_seqf_fops;
static fusion_seq_operations_t  kfio_info_linux_seq_ops;

static fio_ssize_t fio_proc_read_buffer(const char *local_buf, fio_ssize_t len,
                                        char __user *user_buf, fio_size_t count,
                                        fio_loff_t *ppos)
{
    len -= *ppos;

    if (len > count)
        len = count;

    if (len > 0)
    {
        int result = kfio_copy_to_user(user_buf, local_buf + *ppos, len);

        if (result == 0)
            *ppos += len;
        else
            len = result;
    }

    return len;
}

static fio_ssize_t kfio_info_os_type_read(fusion_file *file, char __user *buf,
                                          fio_size_t count, fio_loff_t *ppos)
{
    kfio_info_node_t *nodep;
    kfio_info_val_t   value;
    fio_size_t        size = 0;
    char local_buf[32];
    int  rc = -EINVAL;

    nodep = kfio_inode_data(kfio_fs_inode(file));

    value.type = kfio_info_node_get_type(nodep);
    value.size = kfio_info_node_get_size(nodep);

    if (value.type == KFIO_INFO_STRING)
    {
        value.data = local_buf;

        if (value.size > sizeof(local_buf))
        {
            size = value.size;
            value.data = kfio_vmalloc(size);
            if (value.data == NULL)
            {
                return -ENOMEM;
            }
        }

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_READ, &value, NULL);
        if (rc == 0)
        {
            rc = fio_proc_read_buffer(value.data, value.size, buf, count, ppos);
        }

        if (value.data != local_buf)
        {
            kfio_vfree(value.data, size);
        }
    }

    if (value.type == KFIO_INFO_UINT32)
    {
        fio_ssize_t len;
        uint32_t    val;

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_READ, &value, NULL);

        if (rc == 0)
        {
            len = kfio_snprintf(local_buf, sizeof(local_buf), "%lu\n",
                                (unsigned long)val);

            rc = fio_proc_read_buffer(local_buf, len, buf, count, ppos);
        }
    }

    if (value.type == KFIO_INFO_UINT64)
    {
        fio_ssize_t len;
        uint64_t    val;

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_READ, &value, NULL);
        if (rc == 0)
        {
            len = kfio_snprintf(local_buf, sizeof(local_buf), "%llu\n",
                                (unsigned long long)val);

            rc = fio_proc_read_buffer(local_buf, len, buf, count, ppos);
        }
    }

    if (value.type == KFIO_INFO_INT32)
    {
        fio_ssize_t len;
        int         val;

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_READ, &value, NULL);
        if (rc == 0)
        {
            len = kfio_snprintf(local_buf, sizeof(local_buf), "%d\n", val);

            rc = fio_proc_read_buffer(local_buf, len, buf, count, ppos);
        }
    }
    return rc;
}

static fio_ssize_t kfio_info_os_type_write(fusion_file *file, const char __user *buf,
                                           fio_size_t count, fio_loff_t *ppos)
{
    kfio_info_node_t *nodep;
    kfio_info_val_t   value;
    fio_size_t size = 0;
    char local_buf[32], *endp;
    int  rc = -EINVAL;

    nodep = kfio_inode_data(kfio_fs_inode(file));

    value.type = kfio_info_node_get_type(nodep);
    value.size = kfio_info_node_get_size(nodep);
    value.data = local_buf;

    if (value.type == KFIO_INFO_STRING)
    {
        if (value.size > sizeof(local_buf))
        {
            size = value.size;
            value.data = kfio_vmalloc(size);
            if (value.data == NULL)
            {
                return -ENOMEM;
            }
        }

        if (count > value.size)
        {
            return -EINVAL;
        }
    }
    else
    {
        if (count > sizeof(local_buf))
        {
            return -EINVAL;
        }
    }

    rc = kfio_copy_from_user(value.data, buf, count);
    if (rc != 0)
    {
        if (value.data != local_buf)
        {
            kfio_vfree(value.data, size);
        }
        return rc;
    }

    *ppos += count;

    if (value.type == KFIO_INFO_STRING)
    {
        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_WRITE, NULL, &value);

        if (value.data != local_buf)
        {
            kfio_vfree(value.data, size);
        }
    }

    if (value.type == KFIO_INFO_UINT32)
    {
        uint32_t val;

        val = kfio_strtoul(local_buf, &endp, 10);

        if (endp - local_buf < count - 1)
        {
            return -EINVAL;
        }

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_WRITE, NULL, &value);
    }

    if (value.type == KFIO_INFO_UINT64)
    {
        uint64_t val;

        val = kfio_strtoull(local_buf, &endp, 10);

        if (endp - local_buf < count - 1)
        {
            return -EINVAL;
        }

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_WRITE, NULL, &value);
    }

    if (value.type == KFIO_INFO_INT32)
    {
        int val;

        val = (int)kfio_strtol(local_buf, &endp, 10);

        if (endp - local_buf < count - 1)
        {
            return -EINVAL;
        }

        value.data = &val;

        rc = kfio_info_generic_type_handler(nodep, KFIO_INFO_WRITE, NULL, &value);
    }

    if (rc != 0)
    {
        return -EIO;
    }
    return count;
}

static int kfio_info_linux_text_show(fusion_seq_file *sfile, void *cookie)
{
    kfio_info_node_t *nodep = kfio_fseq_private(sfile);
    kfio_info_data_t *dbh;
    char             *buf;
    fio_size_t        size;
    int               rc;

    size = kfio_seq_get_buf(sfile, &buf);

    rc = kfio_info_alloc_data_handle(nodep, buf, size, &dbh);
    if (rc != 0)
    {
        return -EIO;
    }

    rc = kfio_info_generic_text_handler(nodep, dbh);
    if (rc < 0)
    {
        /* Propagate error to seq file. */
        kfio_seq_commit(sfile, rc);
    }
    else
    {
        /* Just commit the data written. */
        kfio_seq_commit(sfile, kfio_info_data_size_valid(dbh));
    }

    kfio_info_free_data_handle(dbh);

    return rc;
}

static int kfio_info_text_open(fusion_inode *inode, fusion_file *file)
{
    return kfio_single_open(file, kfio_info_linux_text_show, kfio_inode_data(inode));
}

static void *kfio_info_linux_seq_start(fusion_seq_file *sfile, fio_loff_t *pos)
{
    kfio_info_node_t *nodep = kfio_fseq_private(sfile);
    kfio_info_data_t *dbh;
    char             *buf;
    fio_size_t        size;
    int               rc;

    size = kfio_seq_get_buf(sfile, &buf);

    rc = kfio_info_alloc_data_handle(nodep, buf, size, &dbh);
    if (rc != 0)
    {
        return NULL;
    }
    kfio_fseq_set_private(sfile, dbh);

    return kfio_info_seq_init(nodep, pos, dbh);
}

static void *kfio_info_linux_seq_next(fusion_seq_file *sfile, void *cookie, fio_loff_t *pos)
{
    kfio_info_data_t *dbh   = kfio_fseq_private(sfile);
    kfio_info_node_t *nodep = kfio_info_data_node(dbh);

    return kfio_info_seq_next(nodep, cookie, pos);
}

static void kfio_info_linux_seq_stop(fusion_seq_file *sfile, void *cookie)
{
    kfio_info_data_t *dbh   = kfio_fseq_private(sfile);
    kfio_info_node_t *nodep = kfio_info_data_node(dbh);

    kfio_info_seq_stop(nodep, cookie);
    kfio_fseq_set_private(sfile, nodep);
    kfio_info_free_data_handle(dbh);
}

static int kfio_info_linux_seq_show(fusion_seq_file *sfile, void *cookie)
{
    kfio_info_data_t *dbh   = kfio_fseq_private(sfile);
    kfio_info_node_t *nodep = kfio_info_data_node(dbh);
    fio_size_t        size  = kfio_info_data_size_valid(dbh);
    int               rc;

    rc = kfio_info_seq_show(nodep, cookie, dbh);
    if (rc < 0)
    {
        /* Propagate error to seq file. */
        kfio_seq_commit(sfile, rc);
    }
    else
    {
        /* Just commit the data written. */
        kfio_seq_commit(sfile, kfio_info_data_size_valid(dbh) - size);
    }
    return rc;
}

static int kfio_info_seqf_open(fusion_inode *inode, fusion_file *file)
{
    fusion_seq_file *sfile;
    int retval;

    retval = kfio_seq_open(file, &kfio_info_linux_seq_ops);

    if (retval == 0)
    {
        sfile = kfio_fs_private_data(file);
        kfio_fseq_set_private(sfile, kfio_inode_data(inode));
    }

    return retval;
}

static void kfio_info_os_init(void)
{
    /* Initialize file ops used to handle fixed types. */
    kfio_set_file_ops_owner(&kfio_info_type_fops, fusion_get_this_module());
    kfio_set_file_ops_write_handler(&kfio_info_type_fops, kfio_info_os_type_write);
    kfio_set_file_ops_read_handler (&kfio_info_type_fops, kfio_info_os_type_read);

    kfio_set_file_ops_owner(&kfio_info_seqf_fops, fusion_get_this_module());
    kfio_set_file_ops_open_handler   (&kfio_info_seqf_fops, kfio_info_seqf_open);
    kfio_set_file_ops_read_handler   (&kfio_info_seqf_fops, kfio_seq_read);
    kfio_set_file_ops_llseek_handler (&kfio_info_seqf_fops, kfio_seq_lseek);
    kfio_set_file_ops_release_handler(&kfio_info_seqf_fops, kfio_seq_release);

    kfio_set_file_ops_owner(&kfio_info_text_fops, THIS_MODULE);
    kfio_set_file_ops_open_handler   (&kfio_info_text_fops, kfio_info_text_open);
    kfio_set_file_ops_read_handler   (&kfio_info_text_fops, kfio_seq_read);
    kfio_set_file_ops_llseek_handler (&kfio_info_text_fops, kfio_seq_lseek);
    kfio_set_file_ops_release_handler(&kfio_info_text_fops, kfio_seq_release);

    kfio_set_seq_ops_start_handler(&kfio_info_linux_seq_ops, kfio_info_linux_seq_start);
    kfio_set_seq_ops_next_handler (&kfio_info_linux_seq_ops, kfio_info_linux_seq_next);
    kfio_set_seq_ops_stop_handler (&kfio_info_linux_seq_ops, kfio_info_linux_seq_stop);
    kfio_set_seq_ops_show_handler (&kfio_info_linux_seq_ops, kfio_info_linux_seq_show);
}

int kfio_info_os_create_node(kfio_info_node_t *parent, kfio_info_node_t *nodep)
{
    fusion_proc_dir_entry *parent_dir;
    fusion_proc_dir_entry *node_entry;
    const char *node_name;
    fio_mode_t  node_mode;
    int         node_type;

    node_name  = kfio_info_node_get_name(nodep);
    node_mode  = kfio_info_node_get_mode(nodep);
    node_type  = kfio_info_node_get_type(nodep);

    /*
     * If dir node we are creating has no parent, this is the first
     * time this code is being called. Use the opportunity to initialize
     * few globals.
     */
    if (parent == NULL)
    {
        kfio_info_os_init();

        /* Null parent_dir is legal */
        parent_dir = NULL;

        /* .. while creating top level dir only. */
        if (node_type != KFIO_INFO_DIR)
        {
            return -EIO;
        }
    }
    else
    {
        parent_dir = kfio_info_node_get_os_private(parent);
        if (parent_dir == NULL)
        {
            return -EIO;
        }
    }

    node_entry = NULL;

    if (node_type == KFIO_INFO_DIR)
    {
        node_entry = kfio_proc_mkdir(node_name, parent_dir);
    }

    if (node_type == KFIO_INFO_UINT32 || node_type == KFIO_INFO_UINT64 ||
        node_type == KFIO_INFO_STRING || node_type == KFIO_INFO_INT32)
    {
        node_entry = kfio_create_proc_fops_entry(node_name, node_mode, parent_dir,
                                                 &kfio_info_type_fops, nodep);
    }

    if (node_type == KFIO_INFO_TEXT)
    {
        node_entry = kfio_create_proc_fops_entry(node_name, node_mode, parent_dir,
                                                 &kfio_info_text_fops, nodep);
    }

    if (node_type == KFIO_INFO_SEQFILE)
    {
        node_entry = kfio_create_proc_fops_entry(node_name, node_mode, parent_dir,
                                                 &kfio_info_seqf_fops, nodep);
    }

    if (node_entry == NULL)
    {
        return -EIO;
    }

    kfio_info_node_set_os_private(nodep, node_entry);
    return 0;
}

void kfio_info_os_remove_node(kfio_info_node_t *parent, kfio_info_node_t *nodep)
{
    fusion_proc_dir_entry *parent_dir;
    fusion_proc_dir_entry *node_entry;
    const char *node_name;
    int         node_type;

    node_name  = kfio_info_node_get_name(nodep);
    node_type  = kfio_info_node_get_type(nodep);

    node_entry = kfio_info_node_get_os_private(nodep);

    /* Deleting entry that is not there is always OK. */
    if (node_entry == NULL)
    {
        return;
    }

    kfio_info_node_set_os_private(nodep, NULL);

    /*
     * If dir node we are creating has no parent, this is the first
     * time this code is being called. Use the opportunity to initialize
     * few globals.
     */
    if (parent == NULL)
    {
        parent_dir = NULL;
        kassert (node_type == KFIO_INFO_DIR);
    }
    else
    {
        parent_dir = kfio_info_node_get_os_private(parent);
        kassert (parent_dir != NULL);
    }
    kfio_remove_proc_entry(node_name, parent_dir);
}

KFIO_EXPORT_SYMBOL(kfio_info_create_seqf);
KFIO_EXPORT_SYMBOL(kfio_info_create_dir);
KFIO_EXPORT_SYMBOL(kfio_info_create_text);
KFIO_EXPORT_SYMBOL(kfio_info_remove_node);
KFIO_EXPORT_SYMBOL(kfio_info_handle_cmd);
KFIO_EXPORT_SYMBOL(kfio_info_create_proc);
KFIO_EXPORT_SYMBOL(kfio_info_printf);
KFIO_EXPORT_SYMBOL(fusion_info_root);


