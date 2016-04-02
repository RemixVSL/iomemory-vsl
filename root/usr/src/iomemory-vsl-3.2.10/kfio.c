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
//-----------------------------------------------------------------------------

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/types.h>
#include <linux/module.h>
#include <fio/port/dbgset.h>
#include <fio/port/fio-port.h>
#include <linux/slab.h>
#if !defined(__VMKLNX__)
#include <linux/buffer_head.h>
#endif
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/kthread.h>
#include <linux/reboot.h> // register_reboot_notifier unregister_reboot_notifier
#include <linux/stddef.h>
#if !defined(__VMKLNX__)
#include <linux/delay.h>
#include <linux/namei.h>
#include <linux/sysrq.h>
#endif
#include <linux/blkdev.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/random.h>
#include <linux/cpumask.h>

#include <fio/port/kfio_config.h>
#if KFIOC_HAS_LINUX_SCATTERLIST_H
#include <linux/scatterlist.h>
#endif

kfio_cpu_t kfio_current_cpu(void)
{
    return raw_smp_processor_id();
}

kfio_cpu_t kfio_get_cpu(kfio_get_cpu_t *flags)
{
    local_irq_save(*flags);
    return smp_processor_id();
}

void kfio_put_cpu(kfio_get_cpu_t *flags)
{
    local_irq_restore(*flags);
}

unsigned int kfio_max_cpus(void)
{
#if !defined(__VMKLNX__)
    return num_possible_cpus();
#else
    return 1;
#endif
}

int kfio_cpu_online(kfio_cpu_t cpu)
{
#if !defined(__VMKLNX__)
    return cpu_online(cpu);
#else
    return 1;
#endif
}

typedef struct FUSION_STRUCT_ALIGN(8) _linux_spinlock
{
    spinlock_t lock;
    unsigned long flags;
} linux_spinlock_t;
#define FUSION_SPINLOCK_NOT_IRQSAVED ~0UL

void kfio_free_irq(kfio_pci_dev_t *pd, void *dev)
{
    unsigned int irqn = ((struct pci_dev *)pd)->irq;
    free_irq(irqn, dev);
}

C_ASSERT(sizeof(kfio_msix_t) >= sizeof(struct msix_entry));

void kfio_free_msix(kfio_msix_t *msix, unsigned int vector, void *dev)
{
    struct msix_entry *msi = (struct msix_entry *) msix;

    free_irq(msi[vector].vector, dev);
}

irqreturn_t kfio_handle_irq_wrapper(int irq, void *dev_id
#if !KFIOC_HAS_GLOBAL_REGS_POINTER
                                    , struct pt_regs *regs
#endif
    )
{
    (void)iodrive_intr_fast(irq, dev_id);

    // Because hardware cannot lower the IRQ line fast enough
    // We often get called with nothing to do.
    // So, we have to lie to Linux so it doesn't shut off the IRQ
    return FIO_IRQ_HANDLED;
}

irqreturn_t kfio_handle_irqx_wrapper(int irq, void *dev_id
#if !KFIOC_HAS_GLOBAL_REGS_POINTER
                                    , struct pt_regs *regs
#endif
    )
{
    return iodrive_intr_fast_pipeline(irq, dev_id);
}

int kfio_request_msix(kfio_pci_dev_t *pd, const char *devname, void *dev_id,
                      kfio_msix_t *msix, unsigned int vector)
{
    struct msix_entry *msi = (struct msix_entry *) msix;

    return request_irq(msi[vector].vector, kfio_handle_irqx_wrapper, 0, devname, dev_id);
}

int kfio_request_irq(kfio_pci_dev_t *pd, const char *devname, void *dev_id,
                     int msi_enabled)
{
    unsigned long interrupt_flags = 0;
    unsigned int irqn = ((struct pci_dev *)pd)->irq;

#ifdef IRQF_SHARED
    interrupt_flags |= IRQF_SHARED;
#else
    interrupt_flags |= SA_SHIRQ;
#endif

    return request_irq(irqn, kfio_handle_irq_wrapper, interrupt_flags, devname, dev_id);
}

int kfio_get_irq_number(kfio_pci_dev_t *pd, uint32_t *irq)
{
    *irq = (uint32_t)((struct pci_dev *)pd)->irq;
    return 0;
}

int kfio_get_msix_number(void *msix, uint32_t vec_ix, uint32_t *irq)
{
    struct msix_entry *linux_msix = msix;
    *irq = linux_msix[vec_ix].vector;
    return 0;
}

void *kfio_ioremap_nocache (unsigned long offset, unsigned long size)
{
    return ioremap_nocache(offset, size);
}

void  kfio_iounmap(void *addr)
{
    iounmap(addr);
}

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
#if !defined(__VMKLNX__)
    return vprintk(format, ap);
#else
    va_list argsCopy;
    int printedLen;

    va_copy(argsCopy, ap);

    vmk_vLogNoLevel(VMK_LOG_URGENCY_NORMAL, format, ap);
    printedLen = vmk_Vsnprintf(NULL, 0, format, argsCopy);

    va_end(argsCopy);
    va_end(ap);

    return printedLen;
#endif
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

//int kfio_sscanf(const char *buf, const char *fmt, ...)
//{
//    va_list args;
//    int i;

//    va_start(args,fmt);
//    i = vsscanf(buf,fmt,args);
//    va_end(args);
//    return i;
//}

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
#if !defined(__VMKLNX__)
    return memmove(dst, src, n);
#else
    char *tmp;
    const char *s;
    char *dest = dst;
    size_t count = n;

    if (dest <= (char *)src) {
        tmp = dest;
        s = src;
        while (count--)
            *tmp++ = *s++;
    } else {
        tmp = dest;
        tmp += count;
        s = src;
        s += count;
        while (count--)
            *--tmp = *--s;
    }
    return dest;
#endif
}

int kfio_stoi(const char *p, int *nchar)
{
    int n, c;


    for (n=0, *nchar=0; ;(*nchar)++){
        c = *p++;
        if ((c < '0') || (c > '9'))
            break;
        n = (n * 10) + (c - '0');
    }
    return n;
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

kfio_pci_bus_t *kfio_bus_from_pci_dev(kfio_pci_dev_t *pdev)
{
    return (kfio_pci_bus_t *)((struct pci_dev *)pdev)->bus;
}

kfio_pci_bus_t *kfio_pci_bus_parent(kfio_pci_bus_t *bus)
{
    return (kfio_pci_bus_t *)((struct pci_bus *)bus)->parent;
}

int kfio_pci_bus_istop(kfio_pci_bus_t *bus)
{
    // The root device doesn't have a self link
    return !((struct pci_bus *)bus)->self;
}

kfio_pci_dev_t *kfio_pci_bus_self(kfio_pci_bus_t *bus)
{
    return (kfio_pci_dev_t *)((struct pci_bus *)bus)->self;
}

uint8_t kfio_pci_bus_number(kfio_pci_bus_t *bus)
{
    return ((struct pci_bus *)bus)->number;
}

#if PORT_SUPPORTS_PCI_NUMA_INFO
kfio_numa_node_t kfio_pci_get_node(kfio_pci_dev_t *pci_dev)
{
#if KFIOC_PCI_HAS_NUMA_INFO
    struct pci_dev *pdev = (struct pci_dev *) pci_dev;

    return dev_to_node(&pdev->dev);
#else
    return FIO_NUMA_NODE_NONE;
#endif
}
#endif

int kfio_pci_read_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t *val)
{
    return pci_read_config_byte((struct pci_dev *)pdev, where, val);
}


int kfio_pci_read_config_word(kfio_pci_dev_t *pdev, int where, uint16_t *val)
{
    return pci_read_config_word((struct pci_dev *)pdev, where, val);
}


int kfio_pci_read_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t *val)
{
    return pci_read_config_dword((struct pci_dev *)pdev, where, val);
}


int kfio_pci_write_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t val)
{
    return pci_write_config_byte((struct pci_dev *)pdev, where, val);
}


int kfio_pci_write_config_word(kfio_pci_dev_t *pdev, int where, uint16_t val)
{
    return pci_write_config_word((struct pci_dev *)pdev, where, val);
}


int kfio_pci_write_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t val)
{
    return pci_write_config_dword((struct pci_dev *)pdev, where, val);
}


const char *kfio_pci_name(kfio_pci_dev_t *pdev)
{
    return (char *)pci_name((struct pci_dev *)pdev);
}

void kfio_pci_dev_put(kfio_pci_dev_t *pdev)
{
    pci_dev_put((struct pci_dev *)pdev);
}


kfio_pci_dev_t *kfio_pci_get_device(unsigned int vendor, unsigned int device, kfio_pci_dev_t *from)
{
    return (kfio_pci_dev_t *)pci_get_device(vendor, device, (struct pci_dev *)from);
}


uint16_t kfio_pci_get_vendor(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->vendor;
}

uint32_t kfio_pci_get_devnum(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->device;
}

void kfio_pci_disable_device(kfio_pci_dev_t *pdev)
{
    struct pci_dev *dev = (struct pci_dev *)pdev;
    u16 cmd, old;

    /* Save old  BAR decode enable flags. */
    pci_read_config_word(dev, PCI_COMMAND, &old);
    old &= (PCI_COMMAND_IO | PCI_COMMAND_MEMORY);

    pci_disable_device(dev);

    /*
     * Older Linux kernel prior to 2.6.19 used to disable IO and MEM
     * BAR decode on disable, which later was reverted due to it being
     * just as bad idea as it sounds. The code below is a NOOP for
     * newer kernels, but undoes the mistake on older kernels.
     */
    pci_read_config_word(dev, PCI_COMMAND, &cmd);
    if ((cmd & old) != old)
    {
        cmd |= old;
        pci_write_config_word(dev, PCI_COMMAND, cmd);
    }
}

void kfio_pci_disable_msix(kfio_pci_dev_t *pdev)
{
    pci_disable_msix((struct pci_dev *)pdev);
}

void kfio_pci_disable_msi(kfio_pci_dev_t *pdev)
{
    pci_disable_msi((struct pci_dev *)pdev);
}

int kfio_pci_enable_device(kfio_pci_dev_t *pdev)
{
    return pci_enable_device((struct pci_dev *)pdev);
}

unsigned int kfio_pci_enable_msix(kfio_pci_dev_t *__pdev,
                                  kfio_msix_t *msix, unsigned int nr_vecs)
{
    struct pci_dev *pdev = (struct pci_dev *) __pdev;
    struct msix_entry *msi = (struct msix_entry *) msix;
    int err, i;

    if (!pci_find_capability(pdev, PCI_CAP_ID_MSIX))
    {
        return 0;
    }

    for (i = 0; i < nr_vecs; i++)
    {
        msi[i].vector = 0;
        msi[i].entry = i;
    }

    err = pci_enable_msix(pdev, msi, nr_vecs);
    if (err)
    {
        return 0;
    }

    return nr_vecs;
}

int kfio_pci_enable_msi(kfio_pci_dev_t *pdev)
{
    return pci_enable_msi((struct pci_dev *)pdev);
}

void kfio_pci_release_regions(kfio_pci_dev_t *pdev)
{
    pci_release_regions((struct pci_dev *)pdev);
}

int kfio_pci_request_regions(kfio_pci_dev_t *pdev, const char *res_name)
{
    return pci_request_regions((struct pci_dev *)pdev,
#if KFIOC_PCI_REQUEST_REGIONS_CONST_CHAR
        res_name);
#else
        (char *)res_name);
#endif
}


int kfio_pci_set_dma_mask(kfio_pci_dev_t *pdev, uint64_t mask)
{
    return pci_set_dma_mask((struct pci_dev *)pdev, mask);
}


void kfio_pci_set_master(kfio_pci_dev_t *pdev)
{
    pci_set_master((struct pci_dev *)pdev);
}

uint16_t kfio_pci_get_subsystem_vendor(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->subsystem_vendor;
}

uint16_t kfio_pci_get_subsystem_device(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->subsystem_device;
}

void *kfio_pci_get_drvdata(kfio_pci_dev_t *pdev)
{
    return pci_get_drvdata((struct pci_dev *)pdev);
}

void kfio_pci_set_drvdata(kfio_pci_dev_t *pdev, void *data)
{
    pci_set_drvdata((struct pci_dev *)pdev, data);
}

uint64_t kfio_pci_resource_start(kfio_pci_dev_t *pdev, uint16_t bar)
{
    return pci_resource_start((struct pci_dev *)pdev, bar);
}

uint32_t kfio_pci_resource_len(kfio_pci_dev_t *pdev, uint16_t bar)
{
    return pci_resource_len((struct pci_dev *)pdev, bar);
}

uint8_t kfio_pci_get_bus(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->bus->number;
}

uint8_t kfio_pci_get_devicenum(kfio_pci_dev_t *pdev)
{
    return (PCI_SLOT(((struct pci_dev *)pdev)->devfn));
}

uint8_t kfio_pci_get_function(kfio_pci_dev_t *pdev)
{
    return (PCI_FUNC(((struct pci_dev *)pdev)->devfn));
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

void kfio_iodrive_intx (kfio_pci_dev_t *pci_dev, int enable)
{
    uint16_t c, n;

    kfio_pci_read_config_word (pci_dev, PCI_COMMAND, &c);

    if (enable)
    {
        n = c & ~PCI_COMMAND_INTX_DISABLE;
    }
    else
    {
        n = c | PCI_COMMAND_INTX_DISABLE;
    }

    if (n != c)
    {
        kfio_pci_write_config_word (pci_dev, PCI_COMMAND, n);
    }
}

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
#if FUSION_DEBUG && !KFIOC_CONFIG_PREEMPT_RT && !defined(__VMKLNX__)
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
#if FUSION_DEBUG && !KFIOC_CONFIG_PREEMPT_RT
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

#if defined(__VMKLINUX__)
    /* VMware defines their irqrestore implementation as a macro; acting on the
     * actual flags in the lock, rather than a copy (or one passed-by-value).
     * Here, we correct this manually. VMware fixed this for ESXi 5.0 only.
     */
    unsigned long flags = ps->flags;
    spin_unlock_irqrestore(&ps->lock, flags);
#else
    spin_unlock_irqrestore(&ps->lock, ps->flags);
#endif
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
    /* XXX: old kernels do not have this function : */
#if KFIOC_HAS_SPIN_LOCK_IRQSAVE_NESTED
    spin_lock_irqsave_nested(&ps->lock, ps->flags, subclass);
#else
    spin_lock_irqsave(&ps->lock, ps->flags);
#endif
}

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
#if FUSION_DEBUG && !KFIOC_CONFIG_PREEMPT_RT && !defined(__VMKLNX__)
    kassert(!irqs_disabled());
#endif
    read_lock((rwlock_t *)s);
}
void fusion_spin_write_lock(fusion_rw_spinlock_t *s)
{
#if FUSION_DEBUG && !KFIOC_CONFIG_PREEMPT_RT && !defined(__VMKLNX__)
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

/*
 * Mutex wrappers
 */

#if KFIOC_HAS_MUTEX_SUBSYSTEM
#else
#include <asm/semaphore.h>
#endif

void fusion_mutex_init(fusion_mutex_t *lock, const char *name)
{
#if KFIOC_HAS_MUTEX_SUBSYSTEM
    mutex_init((struct mutex *)lock);
#else
    init_MUTEX((struct semaphore *)lock);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_mutex_init);

void fusion_mutex_destroy(fusion_mutex_t *lock)
{
#if KFIOC_HAS_MUTEX_SUBSYSTEM
    //  mutex_destroy((struct mutex *)lock);
#else
    do { } while(0);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_mutex_destroy);

int fusion_mutex_trylock(fusion_mutex_t *lock)
{
#if KFIOC_HAS_MUTEX_SUBSYSTEM
    return mutex_trylock((struct mutex *)lock) ? 1 : 0;
#else
    return !down_trylock((struct semaphore *)lock);
#endif
}
void fusion_mutex_lock(fusion_mutex_t *lock)
{
#if KFIOC_HAS_MUTEX_SUBSYSTEM
    mutex_lock((struct mutex *)lock);
#else
    down((struct semaphore *)lock);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_mutex_lock);

void fusion_mutex_unlock(fusion_mutex_t *lock)
{
#if KFIOC_HAS_MUTEX_SUBSYSTEM
    mutex_unlock((struct mutex *)lock);
#else
    up((struct semaphore *)lock);
#endif
}
KFIO_EXPORT_SYMBOL(fusion_mutex_unlock);

/*
 * Sempahore wrappers
 */

#if !defined(__VMKLNX__)
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
#else  // rw semaphore routines for VMKLNX
void fusion_rwsem_init(fusion_rwsem_t *x, const char *name)
{
    sema_init((struct semaphore *)x, 1);
}
void fusion_rwsem_delete(fusion_rwsem_t *x)
{
    (void) x;
}
void fusion_rwsem_down_read(fusion_rwsem_t *x)
{
    down((struct semaphore *)x);
}
void fusion_rwsem_up_read(fusion_rwsem_t *x)
{
    up((struct semaphore *)x);
}
void fusion_rwsem_down_write(fusion_rwsem_t *x)
{
    down((struct semaphore *)x);
}
void fusion_rwsem_up_write(fusion_rwsem_t *x)
{
    up((struct semaphore *)x);
}
/* Returns 1 if we got the lock */
int fusion_rwsem_down_write_trylock(fusion_rwsem_t *x)
{
    return !down_trylock((struct semaphore *)x);
}
/* Returns 1 if we got the lock */
int fusion_rwsem_down_read_trylock(fusion_rwsem_t *x)
{
    return !down_trylock((struct semaphore *)x);
}
#endif

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

#if defined(__VMKLNX__)
// Version of fusion_create_kthread() that returns the thread task_struct pointer
struct task_struct *fusion_esx_create_kthread(fusion_kthread_func_t func, void *data,
                                              void *fusion_nand_device, const char *fmt, ...)
{
    va_list ap;
    char buffer[MAX_KTHREAD_NAME_LENGTH];
    struct task_struct *ts;

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    ts = kthread_run(func, data, "%s", buffer);

    return IS_ERR(ts)? NULL : ts;
}

void fusion_esx_wakeup_thread(struct task_struct *ts)
{
    wake_up_process(ts);
}
#endif

#if PORT_SUPPORTS_PER_CPU
void kfio_create_kthread_on_cpu(fusion_kthread_func_t func, void *data,
                                void *fusion_nand_device, kfio_cpu_t cpu,
                                const char *fmt, ...)
{
    struct task_struct *task;
    va_list ap;
    char buffer[40];

    if (!cpu_online(cpu))
    {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    task = kthread_create(func, data, "%s", buffer);
    if (!IS_ERR(task) && task)
    {
        kthread_bind(task, cpu);
        wake_up_process(task);
    }
}
#endif

#if PORT_SUPPORTS_PCI_NUMA_INFO
#if KFIOC_NUMA_MAPS
static void __kfio_bind_task_to_cpumask(struct task_struct *tsk, cpumask_t *mask)
{
    tsk->cpus_allowed = *mask;
#if KFIOC_TASK_HAS_NR_CPUS_ALLOWED
    tsk->rt.nr_cpus_allowed = cpus_weight(*mask);
#endif
#if KFIOC_TASK_HAS_BOUND_FLAG
    tsk->flags |= PF_THREAD_BOUND;
#endif
}
#endif

/*
 * Will take effect on next schedule event
 */
void kfio_bind_kthread_to_node(kfio_numa_node_t node)
{
#if KFIOC_NUMA_MAPS
    if (node != FIO_NUMA_NODE_NONE)
    {
        cpumask_t *cpumask = (cpumask_t *) cpumask_of_node(node);

        if (cpumask && cpus_weight(*cpumask))
            __kfio_bind_task_to_cpumask(current, cpumask);
    }
#endif
}
#endif

#if PORT_SUPPORTS_PER_CPU
kfio_cpu_t kfio_next_cpu_in_node(kfio_cpu_t last_cpu, kfio_numa_node_t node)
{
#if KFIOC_NUMA_MAPS
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
#endif

    return -1;
}
#endif  /* PORT_SUPPORTS_PER_CPU */

/*
 * Userspace <-> Kernelspace routines
 * XXX: all the kfio_[put\get]_user_*() routines are superfluous
 * and ought be replaced with kfio_copy_[to|from]_user()
 */
int kfio_copy_from_user(void *to, const void *from, unsigned len)
{
    return copy_from_user(to, from, len);
}
KFIO_EXPORT_SYMBOL(kfio_copy_from_user);

int kfio_copy_to_user(void *to, const void *from, unsigned len)
{
    return copy_to_user(to, from, len);
}
KFIO_EXPORT_SYMBOL(kfio_copy_to_user);

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

void kfio_dump_stack()
{
    dump_stack();
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
#if !defined(__VMKLNX__)
    char abs_filename[UFIO_DEVICE_FILE_MAX_LEN];
#if KFIOC_HAS_PATH_LOOKUP
    struct nameidata nd;
#else
    struct path path;
#endif
    int i = 0;

    snprintf(abs_filename, sizeof(abs_filename) - 1,
            "%s%s", UFIO_CONTROL_DEVICE_PATH, devname);

#if KFIOC_HAS_PATH_LOOKUP
    while ((path_lookup(abs_filename, 0, &nd) != 0) &&
#else
    while ((kern_path(abs_filename, LOOKUP_PARENT, &path) != 0) &&
#endif
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
#endif

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

#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
    TEST_SIZE(errstr, struct fusion_work_struct, struct delayed_work, 0);
#endif
    TEST_SIZE(errstr, struct fusion_work_struct, struct work_struct, 0);

#if KFIOC_HAS_MUTEX_SUBSYSTEM
    TEST_SIZE(errstr, fusion_mutex_t, struct mutex, 0);
#endif

    TEST_SIZE(errstr, kfio_poll_struct, wait_queue_head_t, 0);
    TEST_SIZE(errstr, fusion_condvar_t, wait_queue_head_t, 0);

    return rc;
}

#if FIO_BITS_PER_LONG == 32
/* 64bit divisor, dividend and result. dynamic precision */
uint64_t kfio_div64_64(uint64_t dividend, uint64_t divisor)
{
    uint32_t tmp_divisor = divisor;
    uint32_t upper = divisor >> 32;
    uint32_t shift;

    if(upper)
    {
        shift = fls(upper);
        dividend = dividend >> shift;
        tmp_divisor = divisor >> shift;
    }

    do_div(dividend, tmp_divisor);

    return dividend;
}

uint64_t kfio_mod64_64(uint64_t dividend, uint64_t divisor)
{
    return do_div(dividend, divisor);
}

#endif
