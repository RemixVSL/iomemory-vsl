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
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/reboot.h>
#if defined(__x86_64__) && !defined(__VMKLNX__)
#include <linux/acpi.h>
#endif
#include <fio/port/fio-port.h>
#include <fio/port/kfio_config.h>
#include <fio/port/dbgset.h>
#include <fio/port/port_config.h>
#include <fio/port/kfio.h>

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
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
    unsigned long sun = 0;
#else
    unsigned long long sun = 0;
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

#if (FUSION_STATIC_KERNEL==0)
extern int
iodrive_pci_probe (struct pci_dev *pci_dev, const struct pci_device_id *id);

extern void
iodrive_pci_remove (struct pci_dev *dev);

extern int fusion_do_shutdown(void);

static void iodrive_pci_on_shutdown(struct pci_dev *linux_pci_dev)
{
    (void) fusion_do_shutdown();
}

// ! If this is changed, make sure and update the NTS shim !
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
// ! If these error handling functions are ever made to do something, update the NTS shim. !
static pci_ers_result_t
iodrive_pci_error_detected (struct pci_dev *dev, enum pci_channel_state error)
{
    errprint("%s iodrive: PCI Error detected: %d\n", pci_name(dev), error);
    return PCI_ERS_RESULT_DISCONNECT;
}

static pci_ers_result_t
iodrive_pcie_link_reset (struct pci_dev *dev)
{
    errprint("%s iodrive: PCIe link reset\n", pci_name(dev));
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
  link_reset:iodrive_pcie_link_reset,
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
