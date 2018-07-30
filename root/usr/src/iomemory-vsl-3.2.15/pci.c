//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/reboot.h>
#if defined(__x86_64__) && !defined(__VMKLNX__)
#include <linux/acpi.h>
#endif
#include <fio/port/fio-port.h>
#include <fio/port/pci.h>
#include <fio/port/kfio_config.h>
#include <fio/port/dbgset.h>
#include <fio/port/port_config.h>
#include <fio/port/kfio.h>

#if !defined KFIOC_DISCARD
#error kfioconfig not included
#endif


/*************************************************************************************/
/*   Legacy and MSI interrupts.                                                      */
/*************************************************************************************/
/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

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

void kfio_free_irq(kfio_pci_dev_t *pd, void *dev)
{
    unsigned int irqn = ((struct pci_dev *)pd)->irq;
    free_irq(irqn, dev);
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

int kfio_pci_enable_msi(kfio_pci_dev_t *pdev)
{
    return pci_enable_msi((struct pci_dev *)pdev);
}

void kfio_pci_disable_msi(kfio_pci_dev_t *pdev)
{
    pci_disable_msi((struct pci_dev *)pdev);
}

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

/*************************************************************************************/
/*   Legacy and MSI interrupts.                                                      */
/*************************************************************************************/

C_ASSERT(sizeof(kfio_msix_t) >= sizeof(struct msix_entry));

void kfio_free_msix(kfio_msix_t *msix, unsigned int vector, void *dev)
{
    struct msix_entry *msi = (struct msix_entry *) msix;

    free_irq(msi[vector].vector, dev);
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

int kfio_get_msix_number(void *msix, uint32_t vec_ix, uint32_t *irq)
{
    struct msix_entry *linux_msix = msix;
    *irq = linux_msix[vec_ix].vector;
    return 0;
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

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
    err = pci_enable_msix_exact(pdev, msi, nr_vecs);
#else
    err = pci_enable_msix(pdev, msi, nr_vecs);
#endif
    if (err)
    {
        return 0;
    }

    return nr_vecs;
}

void kfio_pci_disable_msix(kfio_pci_dev_t *pdev)
{
    pci_disable_msix((struct pci_dev *)pdev);
}


/*************************************************************************************/
/*   PCI Bus hierarchy traversal                                                     */
/*************************************************************************************/

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

/*************************************************************************************/
/*   NUMA node management for PCI devices.                                           */
/*************************************************************************************/

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

#if PORT_SUPPORTS_NUMA_NODE_OVERRIDE

extern char *numa_node_override[MAX_PCI_DEVICES];
extern int   num_numa_node_override;

/**
 * Look for override string in a form of 'fctX:Y' in module parameters.
 * Return node to the caller in *nodep if parameter has been found.
 * Return error code and leave *nodep untouched otherwise.
 */
int kfio_get_numa_node_override(kfio_pci_dev_t *fusion_pdev, const char *name, kfio_numa_node_t *nodep)
{
    int name_len, i;

    name_len = kfio_strlen(name);

    for (i = 0; i < num_numa_node_override; i++)
    {
        if (strncmp(name, numa_node_override[i], name_len) == 0 &&
            numa_node_override[i][name_len] == ':')
        {
            kfio_numa_node_t node;
            char *endptr;

            node = (kfio_numa_node_t)kfio_strtoul(&numa_node_override[i][name_len + 1], &endptr, 10);

            if (endptr == &numa_node_override[i][name_len + 1] || *endptr != '\0')
            {
                errprint("%s: malformed numa_node_override parameter ignored.\n", name);
                break;
            }

            *nodep = node;
            return 0;
        }
    }

    return -ENOENT;
}

#endif /* PORT_SUPPORTS_NUMA_NODE_OVERRIDE */

/*************************************************************************************/
/*   PCI configuration space access.                                                 */
/*************************************************************************************/

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

/*************************************************************************************/
/*   PCI device properties.                                                          */
/*************************************************************************************/

uint16_t kfio_pci_get_vendor(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->vendor;
}

uint32_t kfio_pci_get_devnum(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->device;
}

uint8_t kfio_pci_get_bus(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->bus->number;
}

uint16_t kfio_pci_get_domain(kfio_pci_dev_t *pdev)
{
    return pci_domain_nr(((struct pci_dev *)pdev)->bus);
}

uint8_t kfio_pci_get_devicenum(kfio_pci_dev_t *pdev)
{
    return (PCI_SLOT(((struct pci_dev *)pdev)->devfn));
}

uint8_t kfio_pci_get_function(kfio_pci_dev_t *pdev)
{
    return (PCI_FUNC(((struct pci_dev *)pdev)->devfn));
}

uint16_t kfio_pci_get_subsystem_vendor(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->subsystem_vendor;
}

uint16_t kfio_pci_get_subsystem_device(kfio_pci_dev_t *pdev)
{
    return ((struct pci_dev *)pdev)->subsystem_device;
}

uint64_t kfio_pci_resource_start(kfio_pci_dev_t *pdev, uint16_t bar)
{
    return pci_resource_start((struct pci_dev *)pdev, bar);
}

uint32_t kfio_pci_resource_len(kfio_pci_dev_t *pdev, uint16_t bar)
{
    return pci_resource_len((struct pci_dev *)pdev, bar);
}

const char *kfio_pci_name(kfio_pci_dev_t *pdev)
{
    return (const char *)pci_name((struct pci_dev *)pdev);
}

#define PCI_IRQ_ROUTING_RESERVED_LEN          11
#define PCI_IRQ_ROUTING_SLOT_COUNT            16
#define SYSTEM_REQUIRED_VERSION            0x100
#define SYSTEM_REQUIRED_MIN_SIZE              32
#define SYSTEM_REQUIRED_SIZE_ALIGNMENT        16
#define SIGNATURE_OFFSET                      16
#define PCI_IRQ_ROUTING_TABLE_SIGNATURE   "$PIR"
#define BIOS_MEMORY_ADDRESS              0xe0000
#define BIOS_MEMORY_LEN                  0x20000

typedef struct pci_slot_entry
{
    uint8_t     pci_slot_bus_number;
    uint8_t     pci_slot_device_number;
    uint8_t     pci_slot_link_value_inta;
    uint16_t    pci_slot_irq_bitmap_inta;
    uint8_t     pci_slot_link_value_intb;
    uint16_t    pci_slot_irq_bitmap_intb;
    uint8_t     pci_slot_link_value_intc;
    uint16_t    pci_slot_irq_bitmap_intc;
    uint8_t     pci_slot_link_value_intd;
    uint16_t    pci_slot_irq_bitmap_intd;
    uint8_t     pci_slot_number;
    uint8_t     pci_slot_resolved;
} __attribute__ ((aligned(1),packed)) pci_slot_entry_t;

typedef struct pci_irq_routing
{
    uint32_t            pci_irqr_signature;
    uint16_t            pci_irqr_version;
    uint16_t            pci_irqr_table_size;
    uint8_t             pci_irqr_interrupt_routers_bus;
    uint8_t             pci_irqr_interrupt_routers_dev_func;
    uint16_t            pci_irqr_exclusive_irqs;
    uint32_t            pci_irqr_compatible_pci_interrupt_router;
    uint32_t            pci_irqr_miniport_data;
    uint8_t             pci_irqr_reserved[PCI_IRQ_ROUTING_RESERVED_LEN];
    uint8_t             pci_irqr_checksum;
    pci_slot_entry_t    pci_irqr_slot[];
} __attribute__ ((aligned(1),packed)) pci_irq_routing_t;

static uint8_t find_slot_number_bios(const struct pci_dev *dev)
{
    const uint8_t *next_addr;
    const uint8_t *stop_addr;
    void *bios_addr;

    bios_addr = ioremap_nocache(BIOS_MEMORY_ADDRESS, BIOS_MEMORY_LEN);
    if (bios_addr == NULL)
    {
        errprint("%s: could not map bios\n", pci_name((struct pci_dev*)dev));
        return 0;
    }

    stop_addr = bios_addr + BIOS_MEMORY_LEN;

    for (next_addr = (const uint8_t *)bios_addr;
         next_addr < stop_addr;
         next_addr += SIGNATURE_OFFSET)
    {
        char signature[sizeof(PCI_IRQ_ROUTING_TABLE_SIGNATURE)];
        memcpy_fromio(signature, next_addr, sizeof(PCI_IRQ_ROUTING_TABLE_SIGNATURE));

        if (memcmp(signature, PCI_IRQ_ROUTING_TABLE_SIGNATURE, sizeof(PCI_IRQ_ROUTING_TABLE_SIGNATURE)) == 0)
        {
            pci_irq_routing_t pir_entry;
            memcpy_fromio(&pir_entry, next_addr, sizeof(pci_irq_routing_t));

            // Validate basic required fields, excluding checksum.
            if (   pir_entry.pci_irqr_version == SYSTEM_REQUIRED_VERSION
                && pir_entry.pci_irqr_table_size > SYSTEM_REQUIRED_MIN_SIZE
                && (pir_entry.pci_irqr_table_size % SYSTEM_REQUIRED_SIZE_ALIGNMENT) == 0)
            {
                stop_addr = next_addr + pir_entry.pci_irqr_table_size;
                for (; next_addr < stop_addr; next_addr += sizeof(pci_slot_entry_t))
                {
                    pci_slot_entry_t slot_entry;
                    memcpy_fromio(&slot_entry, next_addr, sizeof(pci_slot_entry_t));

                    if (   slot_entry.pci_slot_device_number == dev->devfn
                        && slot_entry.pci_slot_bus_number == dev->bus->number
                        && slot_entry.pci_slot_number > 0)
                    {
                        iounmap(bios_addr);
                        return slot_entry.pci_slot_number;
                    }
                }
                break; // only one table
            }
            break; // table did not validate
        }
    }
    iounmap(bios_addr);

    return 0;
}

#if defined(__x86_64__) && !defined(__VMKLNX__)
static uint8_t find_slot_number_acpi(const struct pci_dev *pcidev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
    unsigned long long sun = 0;
#else
    unsigned long sun = 0;
#endif

#ifdef DEVICE_ACPI_HANDLE
    const struct device *dev = &pcidev->dev;

    // Walk the tree to get a slot number, just in case we are behind a bridge. (ie ioDUO)
    while (dev && !sun)
    {
        acpi_handle handle = DEVICE_ACPI_HANDLE(dev);
        if (handle)
        {
            // Use the ACPI method _SUN to get the slot number. See ACPI spec.
            acpi_evaluate_integer(handle, "_SUN", NULL, &sun);
        }
        dev = dev->parent;
    }
#endif

    return (uint8_t)sun;
}
#endif

uint8_t kfio_pci_get_slot(kfio_pci_dev_t *pdev)
{
#if !defined (__x86_64__) && !defined(__i386__)
    return 0;
#else
    uint32_t slot_number = 0;
    struct pci_dev *dev = (struct pci_dev *)pdev;

    slot_number = find_slot_number_bios(dev);

    /* If behind a bridge, need to check parent or grandparent */
    if (!slot_number && dev->bus && dev->bus->self)
    {
        slot_number = find_slot_number_bios(dev->bus->self);
        if (!slot_number && dev->bus->parent && dev->bus->parent->self)
        {
            slot_number = find_slot_number_bios(dev->bus->parent->self);
        }
    }
#if defined(__x86_64__) && !defined(__VMKLNX__)
    if (!slot_number)
    {
        slot_number = find_slot_number_acpi(dev);
    }
#endif

    return slot_number;
#endif
}

/*************************************************************************************/
/*   PCI device helper methods.                                                      */
/*************************************************************************************/

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

int kfio_pci_enable_device(kfio_pci_dev_t *pdev)
{
    return pci_enable_device((struct pci_dev *)pdev);
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

void kfio_pci_set_drvdata(kfio_pci_dev_t *pdev, void *data)
{
    pci_set_drvdata((struct pci_dev *)pdev, data);
}

void *kfio_pci_get_drvdata(kfio_pci_dev_t *pdev)
{
    return pci_get_drvdata((struct pci_dev *)pdev);
}

/**
 *  @brief allocates locked down memory suitable for DMA transfers.
 *  @param pdev - pointer to device handle
 *      Solaris  (struct _kfio_solaris_pci_dev *)
 *  @param size - size of allocation
 *  @param dma_handle - member .phys_addr of struct will be set to the
 *  physical addr on successful return.  The remainder of the structure is opaque.
 *  @return virtual address of allocated memory or NULL on failure.
 *
 *  The current io-drive has 64 bit restrictions on alignment and buffer length,
 *  and no restrictions on address ranges with DMA.
 */
void noinline *kfio_dma_alloc_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                       struct fusion_dma_t *dma_handle)
{
    FUSION_ALLOCATION_TRIPWIRE_TEST();

    return dma_alloc_coherent(&((struct pci_dev *)pdev)->dev, size,
                              (dma_addr_t *) &dma_handle->phys_addr, GFP_KERNEL);
}

/**
 * @brief frees memory allocated by kfio_dma_alloc_coherent.
 */
void noinline kfio_dma_free_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                     void *vaddr, struct fusion_dma_t *dma_handle)
{
    dma_free_coherent(&((struct pci_dev *)pdev)->dev, size, vaddr,
                      (dma_addr_t) dma_handle->phys_addr);
}

/*
 * @brief Sync the DMA memory before accessing it from device or host.
 */
int kfio_dma_sync(struct fusion_dma_t *dma_hdl, uint64_t offset, size_t length, unsigned type)
{
    return 0;
}

/*************************************************************************************/
/*   PCI device driver module glue.                                                  */
/*************************************************************************************/

#if (FUSION_STATIC_KERNEL==0)
extern int fusion_do_shutdown(void);

extern void iodrive_pci_remove(struct pci_dev *dev);

extern int iodrive_pci_probe(struct pci_dev *pci_dev, const struct pci_device_id *id);

static void iodrive_pci_on_shutdown(struct pci_dev *linux_pci_dev)
{
    (void) fusion_do_shutdown();
}

struct pci_device_id iodrive_ids[] = {
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE_PCI_DEVICE)       },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE1_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_OLD_PCI_DEVICE)  },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_BOOT_PCI_DEVICE) },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREME_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREMEPRO_PCI_DEVICE)   },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREME2_PCI_DEVICE)     },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE4_PCI_DEVICE)      },
    {0,}
};

#if KFIOC_HAS_PCI_ERROR_HANDLERS
static pci_ers_result_t
iodrive_pci_error_detected (struct pci_dev *dev, enum pci_channel_state error)
{
    errprint("%s iodrive: PCI Error detected: %d\n", pci_name(dev), error);
    return PCI_ERS_RESULT_DISCONNECT;
}

static pci_ers_result_t
iodrive_pci_slot_reset (struct pci_dev *dev)
{
    errprint("%s iodrive: PCI slot reset\n", pci_name(dev));
    return PCI_ERS_RESULT_DISCONNECT;
}

static struct pci_error_handlers iodrive_pci_error_handlers = {
  error_detected:iodrive_pci_error_detected,
  slot_reset:iodrive_pci_slot_reset,
};
#endif

static struct pci_driver iodrive_pci_driver = {
    .name = "iodrive",
    .id_table = iodrive_ids,
    .probe = iodrive_pci_probe,
    .remove = iodrive_pci_remove,
    .shutdown = iodrive_pci_on_shutdown,
#if KFIOC_HAS_PCI_ERROR_HANDLERS
    .err_handler = &iodrive_pci_error_handlers,
#endif
};

MODULE_DEVICE_TABLE (pci, iodrive_ids);

/**
 * @brief registers pci driver.  Returns 1 on success
 * independent of Linux kernel obstacles.
 *
 */
int kfio_pci_register_driver(void)
{
    int retval;

    retval = pci_register_driver(&iodrive_pci_driver);
    if (0 <= retval)
    {
        return 1;
    }
    return retval;
}

void kfio_pci_unregister_driver(void)
{
    pci_unregister_driver(&iodrive_pci_driver);
}
#endif
/**
 * @}
 */
