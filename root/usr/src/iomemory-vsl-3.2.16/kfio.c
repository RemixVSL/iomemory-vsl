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

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/types.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/buffer_head.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/kthread.h>
#include <linux/stddef.h>
#include <linux/delay.h>
#include <linux/namei.h>
#include <linux/sysrq.h>
#include <linux/blkdev.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/random.h>
#include <linux/cpumask.h>

#include <fio/port/dbgset.h>
#include <fio/port/ktime.h>
#include <fio/port/cdev.h>
#include <fio/port/fio-port.h>
#include <fio/port/sched.h>
#include <fio/port/kfio_config.h>

/**
 * @ingroup PORT_LINUX
 * @{
 */

typedef struct FUSION_STRUCT_ALIGN(8) _linux_spinlock
{
    spinlock_t lock;
    unsigned long flags;
} linux_spinlock_t;
#define FUSION_SPINLOCK_NOT_IRQSAVED ~0UL

void *kfio_ioremap_nocache (unsigned long offset, unsigned long size)
{
    return ioremap(offset, size);
}

void  kfio_iounmap(void *addr)
{
    iounmap(addr);
}

void kfio_pci_dev_put(kfio_pci_dev_t *pdev)
{
    pci_dev_put((struct pci_dev *)pdev);
}

kfio_pci_dev_t *kfio_pci_get_device(unsigned int vendor, unsigned int device, kfio_pci_dev_t *from)
{
    return (kfio_pci_dev_t *)pci_get_device(vendor, device, (struct pci_dev *)from);
}

#if IODRIVE_DISABLE_PCIE_RELAXED_ORDERING
void kfio_pci_disable_relaxed_ordering(kfio_pci_dev_t *pdev)
{
    int pos;
    uint16_t cap;

    dbgprint(DBGS_GENERAL, "iodrive_pci_probe: disabling relaxed ordering...\n");

    pos = pci_find_capability ((struct pci_dev *)pdev, PCI_CAP_ID_EXP);

    pci_read_config_word ((struct pci_dev *)pdev, pos + PCI_EXP_DEVCTL, &cap);

    if (cap & PCI_EXP_DEVCTL_RELAX_EN)
    {
        cap &= ~PCI_EXP_DEVCTL_RELAX_EN;

        pci_write_config_word ((struct pci_dev *)pdev, pos + PCI_EXP_DEVCTL,
                               cap);
    }
}
#endif

/*
 * Spinlock wrappers
 *
 * If the flags member of the linuc_spinlock_t structure is set to
 *  FUSION_SPINLOCK_NOT_IRQSAVED then the spinlock is held without
 *  IRQ saving.
 */
void noinline fusion_init_spin(fusion_spinlock_t *s, const char *name)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;

    spin_lock_init(&ps->lock);
}
KFIO_EXPORT_SYMBOL(fusion_init_spin);

// spin_lock must always be called in a consistent context, i.e. always in
// user context or always in interrupt context. The following two macros
// make this distinction. Use fusion_spin_lock() if your lock is always
// obtained in user context; use fusion_spin_lock_irqdisabled if your
// lock is always obtained in interrupt contect (OR with interrupts
// disabled); and use fusion_spin_trylock_irqsave if it can be called in
// both or if you don't know.
void noinline fusion_spin_lock(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
#if FUSION_DEBUG && !defined(CONFIG_PREEMPT_RT)
    kassert(!irqs_disabled());
#endif
    spin_lock(&ps->lock);
    ps->flags = FUSION_SPINLOCK_NOT_IRQSAVED;
}
KFIO_EXPORT_SYMBOL(fusion_spin_lock);

void fusion_destroy_spin(fusion_spinlock_t *s)
{
}
KFIO_EXPORT_SYMBOL(fusion_destroy_spin);

int noinline fusion_spin_is_locked(fusion_spinlock_t *s)
{
#if defined(CONFIG_SMP) || defined(CONFIG_DEBUG_SPINLOCK)
    return spin_is_locked(&((linux_spinlock_t *)s)->lock);
#else
    return 1;
#endif
}

void noinline fusion_spin_lock_irqdisabled(fusion_spinlock_t *s)
{
#if FUSION_DEBUG && !defined(CONFIG_PREEMPT_RT)
    kassert(irqs_disabled());
#endif
    spin_lock(&((linux_spinlock_t *)s)->lock);
}

int noinline fusion_spin_trylock(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    int ret;

    ret = spin_trylock(&ps->lock);
    if (ret) /* we have the lock */
    {
        ps->flags = FUSION_SPINLOCK_NOT_IRQSAVED;
    }
    return ret ? 1 : 0;
}

void noinline fusion_spin_unlock(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    spin_unlock(&ps->lock);
}
KFIO_EXPORT_SYMBOL(fusion_spin_unlock);

/* The flags parameter is modified before the lock is locked
 * so we only modify the linux_spinlock_t.flags after acquiring
 * the spinlock.
 */
void noinline fusion_spin_lock_irqsave(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    unsigned long flags;

    spin_lock_irqsave(&ps->lock, flags);
    ps->flags = flags;
}
KFIO_EXPORT_SYMBOL(fusion_spin_lock_irqsave);

void noinline fusion_spin_lock_irq(fusion_spinlock_t *s)
{
    spin_lock_irq(&((linux_spinlock_t *)s)->lock);
}

/* The flags parameter is modified before the lock is tried
 * so we only modify the linux_spinlock_t.flags if we have
 * the spinlock held.
 */
int noinline fusion_spin_trylock_irqsave(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    unsigned long flags;
    int ret ;

    ret = spin_trylock_irqsave(&ps->lock, flags);
    if (ret) /* lock is held */
    {
        ps->flags = flags;
    }
    return ret ? 1 : 0;
}

void noinline fusion_spin_unlock_irqrestore(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    spin_unlock_irqrestore(&ps->lock, ps->flags);
}
KFIO_EXPORT_SYMBOL(fusion_spin_unlock_irqrestore);

void noinline fusion_spin_unlock_irq(fusion_spinlock_t *s)
{
    spin_unlock_irq(&((linux_spinlock_t *)s)->lock);
}

/**
 * If the lock is not held the following function's return is meaningless.
 */
int noinline fusion_spin_is_irqsaved(fusion_spinlock_t *s)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    return (ps->flags == FUSION_SPINLOCK_NOT_IRQSAVED ? 0 : 1);
}
/***
 * @brief nested spinlock support
 * use @f fusion_spin_lock_irqrestore(..) to unlock it.
 * is2.c needs it
 */
void noinline fusion_spin_lock_irqsave_nested(fusion_spinlock_t *s, int subclass)
{
    linux_spinlock_t *ps = (linux_spinlock_t *) s;
    spin_lock_irqsave_nested(&ps->lock, ps->flags, subclass);
}

/*
 * Mutex wrappers
 */

void fusion_mutex_init(fusion_mutex_t *lock, const char *name)
{
    mutex_init((struct mutex *)lock);
}
KFIO_EXPORT_SYMBOL(fusion_mutex_init);

void fusion_mutex_destroy(fusion_mutex_t *lock)
{
}
KFIO_EXPORT_SYMBOL(fusion_mutex_destroy);

int fusion_mutex_trylock(fusion_mutex_t *lock)
{
    return mutex_trylock((struct mutex *)lock) ? 1 : 0;
}
void fusion_mutex_lock(fusion_mutex_t *lock)
{
    mutex_lock((struct mutex *)lock);
}
KFIO_EXPORT_SYMBOL(fusion_mutex_lock);

void fusion_mutex_unlock(fusion_mutex_t *lock)
{
    mutex_unlock((struct mutex *)lock);
}
KFIO_EXPORT_SYMBOL(fusion_mutex_unlock);

/*
 * Sempahore wrappers
 */
void fusion_rwsem_init(fusion_rwsem_t *x, const char *name)
{
    init_rwsem((struct rw_semaphore *)x);
}
void fusion_rwsem_delete(fusion_rwsem_t *x)
{
    (void) x;
}
void fusion_rwsem_down_read(fusion_rwsem_t *x)
{
    down_read((struct rw_semaphore *)x);
}
/* Returns 1 if we got the lock */
int fusion_rwsem_down_read_trylock(fusion_rwsem_t *x)
{
    return down_read_trylock((struct rw_semaphore *)x);
}
void fusion_rwsem_up_read(fusion_rwsem_t *x)
{
    up_read((struct rw_semaphore *)x);
}
void fusion_rwsem_down_write(fusion_rwsem_t *x)
{
    down_write((struct rw_semaphore *)x);
}
/* Returns 1 if we got the lock */
int fusion_rwsem_down_write_trylock(fusion_rwsem_t *x)
{
    return down_write_trylock((struct rw_semaphore *)x);
}
void fusion_rwsem_up_write(fusion_rwsem_t *x)
{
    up_write((struct rw_semaphore *)x);
}

#if PORT_SUPPORTS_PER_CPU
kfio_cpu_t kfio_next_cpu_in_node(kfio_cpu_t last_cpu, kfio_numa_node_t node)
{
    if (node != FIO_NUMA_NODE_NONE)
    {
        cpumask_t *cpumask = (cpumask_t *) cpumask_of_node(node);
        kfio_cpu_t ret;

        do {
            ret = cpumask_next(last_cpu, cpumask);
            if (ret < nr_cpu_ids)
            {
                return ret;
            }
            last_cpu = -1;
        } while (1);
    }
    return -1;
}
#endif  /* PORT_SUPPORTS_PER_CPU */

int kfio_put_user_8(int x, uint8_t *arg)
{
    return put_user(x, arg);
}

int kfio_put_user_16(int x, uint16_t *arg)
{
    return put_user(x, arg);
}

int kfio_put_user_32(int x, uint32_t *arg)
{
    return put_user(x, arg);
}

int kfio_put_user_64(int x, uint64_t *arg)
{
    return put_user(x, arg);
}

/*
 * @brief OS-specific init for char device.
 *
 * Wait for the device to be created by udev before we
 * continue on and allow modprobe to exit.  This is because on
 * some older Linux distros, there is a problem where the devices
 * aren't yet created by the time they are needed by init scripts
 * and bad stuff happens.
 *
 */
int fio_wait_for_dev(const char *devname)
{
    char abs_filename[UFIO_DEVICE_FILE_MAX_LEN];
    struct path path;
    int i = 0;

    snprintf(abs_filename, sizeof(abs_filename) - 1,
            "%s%s", UFIO_CONTROL_DEVICE_PATH, devname);

    while ((kern_path(abs_filename, LOOKUP_PARENT, &path) != 0) &&
          (i < fio_dev_wait_timeout_secs * 10))
    {
        if (i == 0)
        {
            infprint("Waiting for %s to be created\n", abs_filename);
        }
        msleep(100);  // Sleep 100msecs
        i++;
    }

    if(i >= fio_dev_wait_timeout_secs * 10)
    {
        infprint("warning: timeout on device %s creation\n", devname);
    }
    return 0;
}
KFIO_EXPORT_SYMBOL(fio_wait_for_dev);


/*
 * Platform specific allocations and structure size checks
 */

// A compile time assertion routine suitable for use in a function body. Unlike C_ASSERT,
// does not throw unused variable warnings. Uses duplicate switch labels to cause compile
// time error.
#define C_ASSERT_IN_FN_BODY(cond)               \
    switch (0) { case 0: case (cond): ; }

/**
 * We detect size issues at compile time but print out the failure at
 * runtime so that we get the output from the customers machine that
 * is intelligible.
 */
#define TEST_SIZE(str, our_struct, kernel_struct, extra_needed)         \
    do {                                                                \
        if (sizeof(our_struct) < sizeof(kernel_struct) + extra_needed)  \
        {                                                               \
            errprint("%s%s (%u, %u)\n", str, #our_struct,               \
                     (unsigned)sizeof(kernel_struct), (unsigned)sizeof(our_struct)); \
            rc = -1;                                                    \
        }                                                               \
    } while(0)


/**
 * @brief checks size of abstracted fusion_* structures against the size of the
 * OS-specific structure they wrap.
 */
int __init kfio_checkstructs(void)
{
    int rc = 0;
    char *errstr = "\nSize mismatch between the following kernel and shipped object structs:\n";

    TEST_SIZE(errstr, kfio_misc_device_t, struct miscdevice, 0);

    TEST_SIZE(errstr, fusion_spinlock_t, linux_spinlock_t, 0);
    TEST_SIZE(errstr, fusion_rw_spinlock_t, rwlock_t, 0);

    TEST_SIZE(errstr, fusion_rwsem_t, struct rw_semaphore, 0);

    TEST_SIZE(errstr, struct fusion_timer_list, struct timer_list, 0);

    TEST_SIZE(errstr, struct fusion_work_struct, struct delayed_work, 0);
    TEST_SIZE(errstr, struct fusion_work_struct, struct work_struct, 0);

    TEST_SIZE(errstr, fusion_mutex_t, struct mutex, 0);

    TEST_SIZE(errstr, kfio_poll_struct, wait_queue_head_t, 0);
    TEST_SIZE(errstr, fusion_condvar_t, wait_queue_head_t, 0);

    return rc;
}

/**
 * @}
 */
