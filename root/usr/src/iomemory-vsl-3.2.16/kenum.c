/*
  Device enumeration for a namespace shared with another ioMemory driver.
  See include/fio/port/common-linux/kenum.h for what this is for.

  The driver object numbers cards from zero and formats the names from that
  number before handing them to this porting layer.  Nothing here changes a
  name: it only adds a base to the number inside one, and only when it can
  reproduce the name it was given from the number the object also supplied.
  Anything it does not recognise is passed through untouched, so an
  unrecognised naming scheme costs coexistence, never a wrong device name.
 */

#include "port-internal.h"

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/kernel.h>
#include <linux/namei.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/version.h>
#if __has_include(<linux/kstrtox.h>)  /* split out of linux/kernel.h in 5.18 */
#include <linux/kstrtox.h>
#endif

#include <fio/port/dbgset.h>
#include <fio/port/common-linux/kenum.h>

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

int fio_dev_index_base = -1;

/* How far to look for a free number in the shared namespace. */
#define FIO_ENUM_MAX_PROBE     64

static int fio_enum_base = 0;
static int fio_enum_base_done = 0;

/*
 * misc_register() keeps the name pointer it is given for the lifetime of the
 * device, so a renumbered control device name cannot be built on the stack.
 * One slot per card this driver owns, indexed by the number the driver object
 * assigned it.
 */
static char fio_enum_control_names[FIO_ENUM_MAX_DEVICES][FIO_ENUM_NAME_MAX];

int fio_enum_path_exists(const char *path)
{
    struct path p;

    if (kern_path(path, 0, &p) != 0)
    {
        return 0;
    }

    path_put(&p);
    return 1;
}

/*
 * filldir_t returned int until 6.1 and bool from 6.1 on.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
# define FIO_FILLDIR_RET   bool
# define FIO_FILLDIR_CONT  true
#else
# define FIO_FILLDIR_RET   int
# define FIO_FILLDIR_CONT  0
#endif

struct fio_enum_dir_count
{
    struct dir_context ctx;
    int                entries;
};

static FIO_FILLDIR_RET fio_enum_count_one(struct dir_context *ctx, const char *name,
                                          int len, loff_t off, u64 ino,
                                          unsigned int d_type)
{
    struct fio_enum_dir_count *c = container_of(ctx, struct fio_enum_dir_count, ctx);

    (void)off; (void)ino; (void)d_type;

    if (!(len == 1 && name[0] == '.') &&
        !(len == 2 && name[0] == '.' && name[1] == '.'))
    {
        c->entries++;
    }
    return FIO_FILLDIR_CONT;
}

/**
 * @brief 1 if @path is an empty directory, 0 if it has entries, -1 if unknown.
 *
 * procfs children exist as proc_dir_entry objects whether or not a dentry has
 * been instantiated, so this reads the directory rather than inspecting
 * dentries — which would wrongly report empty.
 */
int fio_enum_dir_is_empty(const char *path)
{
    struct fio_enum_dir_count c;
    struct file *fp;
    int rc;

    memset(&c, 0, sizeof(c));
    c.ctx.actor = fio_enum_count_one;
    c.ctx.pos   = 0;
    c.entries   = 0;

    fp = filp_open(path, O_RDONLY | O_DIRECTORY, 0);
    if (IS_ERR(fp))
    {
        return -1;
    }

    rc = iterate_dir(fp, &c.ctx);
    filp_close(fp, NULL);

    if (rc < 0)
    {
        return -1;
    }

    return c.entries == 0 ? 1 : 0;
}

int fio_enum_root_removable(const char *proc_root)
{
    char path[FIO_ENUM_NAME_MAX * 2];
    int  empty;

    snprintf(path, sizeof(path), "/proc/%s", proc_root);
    empty = fio_enum_dir_is_empty(path);

    if (empty == 1)
    {
        return 1;
    }

    /*
     * ⛔ Removing a non-empty procfs directory leaks its children ("removing
     * non-empty directory ... leaking at least ...") and the next load then
     * collides creating it again.  When in doubt, leave it: an empty directory
     * left behind is harmless, and a later load simply shares it.
     */
    if (empty == 0)
    {
        infprint("leaving /proc/%s in place: another ioMemory driver still has"
                 " entries there\n", proc_root);
    }
    else
    {
        infprint("cannot determine whether /proc/%s is empty; leaving it in"
                 " place\n", proc_root);
    }
    return 0;
}

/**
 * @brief Render a disk index the way the driver object does: a, b, .. z, aa, ..
 *
 * Returns 0 on success.
 */
static int fio_enum_disk_letters(int index, char *out, size_t outlen)
{
    char tmp[8];
    int  n = 0;
    int  i;

    if (index < 0)
    {
        return -1;
    }

    do
    {
        tmp[n++] = 'a' + (index % 26);
        index = index / 26 - 1;
    }
    while (index >= 0 && n < (int)sizeof(tmp));

    if (index >= 0 || (size_t)n >= outlen)
    {
        return -1;
    }

    for (i = 0; i < n; i++)
    {
        out[i] = tmp[n - 1 - i];
    }
    out[n] = '\0';
    return 0;
}

int fio_enum_get_base(void)
{
    return fio_enum_base;
}

int fio_enum_resolve_base(const char *proc_root, int shared)
{
    char path[FIO_ENUM_NAME_MAX * 2];
    int  i;

    if (fio_enum_base_done)
    {
        return fio_enum_base;
    }
    fio_enum_base_done = 1;

    if (fio_dev_index_base >= 0)
    {
        fio_enum_base = fio_dev_index_base;
        infprint("enumerating devices from %s%d (fio_dev_index_base=%d)\n",
                 UFIO_CONTROL_DEVICE_PREFIX, fio_enum_base, fio_dev_index_base);
        return fio_enum_base;
    }

    if (!shared)
    {
        /* We created /proc/<root>, so no other driver has registered there. */
        return fio_enum_base;
    }

    /*
     * Another driver already owns the directory.  Its cards are the ones
     * numbered there now, because ours have not registered yet.
     */
    for (i = 0; i < FIO_ENUM_MAX_PROBE; i++)
    {
        snprintf(path, sizeof(path), "/proc/%s/%s%d", proc_root,
                 UFIO_CONTROL_DEVICE_PREFIX, i);
        if (!fio_enum_path_exists(path))
        {
            break;
        }
    }

    if (i >= FIO_ENUM_MAX_PROBE)
    {
        errprint("no free device number below %s%d in /proc/%s, enumerating from"
                 " %s0 and expecting a collision\n", UFIO_CONTROL_DEVICE_PREFIX,
                 FIO_ENUM_MAX_PROBE, proc_root, UFIO_CONTROL_DEVICE_PREFIX);
        return fio_enum_base;
    }

    fio_enum_base = i;

    if (fio_enum_base == 0)
    {
        infprint("sharing /proc/%s with an already loaded ioMemory driver, which"
                 " has no device registered yet; enumerating from %s0\n",
                 proc_root, UFIO_CONTROL_DEVICE_PREFIX);
    }
    else
    {
        infprint("sharing /proc/%s with an already loaded ioMemory driver;"
                 " enumerating this driver's devices from %s%d\n",
                 proc_root, UFIO_CONTROL_DEVICE_PREFIX, fio_enum_base);
    }

    return fio_enum_base;
}

const char *fio_enum_control_name(unsigned int devnum, const char *stock)
{
    char expect[FIO_ENUM_NAME_MAX];

    if (fio_enum_base == 0 || stock == NULL || devnum >= FIO_ENUM_MAX_DEVICES)
    {
        return stock;
    }

    snprintf(expect, sizeof(expect), "%s%u", UFIO_CONTROL_DEVICE_PREFIX, devnum);
    if (strcmp(expect, stock) != 0)
    {
        return stock;
    }

    snprintf(fio_enum_control_names[devnum], FIO_ENUM_NAME_MAX, "%s%u",
             UFIO_CONTROL_DEVICE_PREFIX, devnum + fio_enum_base);

    return fio_enum_control_names[devnum];
}

const char *fio_enum_block_name(int disk_index, const char *stock,
                                char *buf, size_t buflen)
{
    char letters[8];
    char expect[FIO_ENUM_NAME_MAX];

    if (fio_enum_base == 0 || stock == NULL)
    {
        return stock;
    }

    if (fio_enum_disk_letters(disk_index, letters, sizeof(letters)) != 0)
    {
        return stock;
    }

    snprintf(expect, sizeof(expect), "%s%s", UFIO_BLOCK_DEVICE_PREFIX, letters);
    if (strcmp(expect, stock) != 0)
    {
        return stock;
    }

    if (fio_enum_disk_letters(disk_index + fio_enum_base, letters,
                              sizeof(letters)) != 0)
    {
        return stock;
    }

    snprintf(buf, buflen, "%s%s", UFIO_BLOCK_DEVICE_PREFIX, letters);
    return buf;
}

const char *fio_enum_proc_name(const char *stock, char *buf, size_t buflen)
{
    size_t       prefix_len = strlen(UFIO_CONTROL_DEVICE_PREFIX);
    unsigned int devnum;

    if (fio_enum_base == 0 || stock == NULL)
    {
        return stock;
    }

    if (strncmp(stock, UFIO_CONTROL_DEVICE_PREFIX, prefix_len) != 0)
    {
        return stock;
    }

    /* Rejects a trailing or embedded non-digit, so "fio" and "version" and the
     * like are left alone. */
    if (kstrtouint(stock + prefix_len, 10, &devnum) != 0)
    {
        return stock;
    }

    snprintf(buf, buflen, "%s%u", UFIO_CONTROL_DEVICE_PREFIX,
             devnum + fio_enum_base);
    return buf;
}

/**
 * @}
 */
