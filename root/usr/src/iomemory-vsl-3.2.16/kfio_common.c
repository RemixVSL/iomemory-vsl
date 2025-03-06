//-----------------------------------------------------------------------------
// Copyright (c) 2014 Fusion-io, Inc. (acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2016 SanDisk Corp. and/or all its affiliates. (acquired by Western Digital Corp. 2016)
// Copyright (c) 2016-2017 Western Digital Technologies, Inc. All rights reserved.
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

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>
#include <fio/port/dbgset.h>
#include <fio/port/kfio_config.h>

/**
 * @ingroup PORT_LINUX
 * @{
 */

void
__kassert_fail (const char *expr, const char *file, int line,
                const char *func, int good, int bad)
{
    errprint("iodrive: assertion failed %s:%d:%s: %s",
             file, line, func, expr);

    if (make_assert_nonfatal)
    {
        errprint("ASSERT STATISTICS: %d bad, %d good\n",
                 bad, good);
        dump_stack ();
    }
    else
    {
        BUG();
    }
}
KFIO_EXPORT_SYMBOL(__kassert_fail);

int kfio_print(const char *format, ...)
{
    va_list ap;
    int rc;

    va_start(ap, format);
    rc = kfio_vprint(format, ap);
    va_end(ap);

    return rc;
}
KFIO_EXPORT_SYMBOL(kfio_print);

int kfio_vprint(const char *format, va_list ap)
{
    return vprintk(format, ap);
}

int kfio_snprintf(char *buffer, fio_size_t n, const char *format, ...)
{
    va_list ap;
    int rc;

    va_start(ap, format);
    rc = kfio_vsnprintf(buffer, n, format, ap);
    va_end(ap);

    return rc;
}
KFIO_EXPORT_SYMBOL(kfio_snprintf);

int kfio_vsnprintf(char *buffer, fio_size_t n, const char *format, va_list ap)
{
    return vsnprintf(buffer, n, format, ap);
}
KFIO_EXPORT_SYMBOL(kfio_vsnprintf);

int kfio_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

fio_size_t kfio_strlen(const char *s1)
{
    return strlen(s1);
}
KFIO_EXPORT_SYMBOL(kfio_strlen);

int kfio_strncmp(const char *s1, const char *s2, fio_size_t n)
{
    return strncmp(s1, s2, n);
}
KFIO_EXPORT_SYMBOL(kfio_strncmp);

char *kfio_strncpy(char *dst, const char *src, fio_size_t n)
{
    return strncpy(dst, src, n);
}
KFIO_EXPORT_SYMBOL(kfio_strncpy);

char *kfio_strcat(char *dst, const char *src)
{
    return strcat(dst, src);
}

char *kfio_strncat(char *dst, const char *src, int size)
{
    return strncat(dst, src, size);
}

void *kfio_memset(void *dst, int c, fio_size_t n)
{
    return memset(dst, c, n);
}
KFIO_EXPORT_SYMBOL(kfio_memset);

int kfio_memcmp(const void *m1, const void *m2, fio_size_t n)
{
    return memcmp(m1, m2, n);
}
KFIO_EXPORT_SYMBOL(kfio_memcmp);

void *kfio_memcpy(void *dst, const void *src, fio_size_t n)
{
    return memcpy(dst, src, n);
}
KFIO_EXPORT_SYMBOL(kfio_memcpy);

void *kfio_memmove(void *dst, const void *src, fio_size_t n)
{
    return memmove(dst, src, n);
}

unsigned long long kfio_strtoull(const char *nptr, char **endptr, int base)
{
    return simple_strtoull(nptr, endptr, base);
}
KFIO_EXPORT_SYMBOL(kfio_strtoull);

unsigned long int kfio_strtoul(const char *nptr, char **endptr, int base)
{
    return simple_strtoul(nptr, endptr, base);
}
KFIO_EXPORT_SYMBOL(kfio_strtoul);

long int kfio_strtol(const char *nptr, char **endptr, int base)
{
    return simple_strtol(nptr, endptr, base);
}
KFIO_EXPORT_SYMBOL(kfio_strtol);

/*
 * RW spinlock wrappers
 */

void fusion_init_rw_spin(fusion_rw_spinlock_t *s, const char *name)
{
    rwlock_init((rwlock_t *)s);
}

void fusion_destroy_rw_spin(fusion_rw_spinlock_t *s)
{
    (void)s;
}

void fusion_spin_read_lock(fusion_rw_spinlock_t *s)
{
#if FUSION_DEBUG
    kassert(!irqs_disabled());
#endif
    read_lock((rwlock_t *)s);
}
void fusion_spin_write_lock(fusion_rw_spinlock_t *s)
{
#if FUSION_DEBUG
    kassert(!irqs_disabled());
#endif
    write_lock((rwlock_t *)s);
}

void fusion_spin_read_unlock(fusion_rw_spinlock_t *s)
{
    read_unlock((rwlock_t *)s);
}
void fusion_spin_write_unlock(fusion_rw_spinlock_t *s)
{
    write_unlock((rwlock_t *)s);
}

void fusion_create_kthread(fusion_kthread_func_t func, void *data, void *fusion_nand_device,
                           const char *fmt, ...)
{
    va_list ap;
    char buffer[MAX_KTHREAD_NAME_LENGTH];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    kthread_run(func, data, "%s", buffer);
}

/*
 * Userspace <-> Kernelspace routines
 * XXX: all the kfio_[put\get]_user_*() routines are superfluous
 * and ought be replaced with kfio_copy_[to|from]_user()
 */
/*
 8140 - 8170 is also valid.... if we deny the 8140 fio-sure-erase bombs on
 not being able to backup the lebmap...

*/
#define MAX_USER_BUFFER_SIZE 9216
int kfio_copy_from_user(void *to, const void *from, unsigned len)
{
    infprint("Copying %u bytes from user space to kernel space\n", len);
    if (!from || !to) {
        infprint("NULL pointer passed to kfio_copy_from_user\n");
      return -EINVAL;
    }
    if (len > MAX_USER_BUFFER_SIZE) {
        infprint("Invalid user buffer size: %u\n", len);
        return -EINVAL;
    }
    if (len == 3960) {
        infprint("hit the weird one");
        return -EINVAL;
    }
    int result = copy_from_user(to, from, len);
    if (result) {
        infprint("Failed to copy %u bytes from user space, %d bytes not copied\n", len, result);
        return -EFAULT;
    }
    return result;
    // return copy_from_user(to, from, len);
}
KFIO_EXPORT_SYMBOL(kfio_copy_from_user);

int kfio_copy_to_user(void *to, const void *from, unsigned len)
{
    infprint("Copying %u bytes to user space from kernel space\n", len);
    if (!from || !to) {
        infprint("NULL pointer passed to kfio_copy_from_user\n");
      return -EINVAL;
    }
    if (len > MAX_USER_BUFFER_SIZE) {
        infprint("Invalid user buffer size: %u\n", len);
        return -EINVAL;
    }
    int result = copy_to_user(to, from, len);
    if (result) {
        infprint("Failed to copy %u bytes from user space, %d bytes not copied\n", len, result);
        return -EFAULT;
    }
    return result;
    // return copy_to_user(to, from, len);
}
KFIO_EXPORT_SYMBOL(kfio_copy_to_user);

void kfio_dump_stack()
{
    dump_stack();
}

/**
 * @}
 */
