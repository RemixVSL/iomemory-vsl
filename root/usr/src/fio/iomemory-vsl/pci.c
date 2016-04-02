//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if defined (__linux__)
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/module.h>
#endif
#include <fio/port/fio-port.h>
#include <fio/port/linux/kfio_config.h>
#include <fio/port/dbgset.h>
#include <fio/port/config.h>
#include <fio/port/linux/kfio.h>

#define PCI_IRQ_ROUTING_RESERVED_LEN          11
#define PCI_IRQ_ROUTING_SLOT_COUNT            16
#define SYSTEM_REQUIRED_VERSION            0x100
#define SYSTEM_REQUIRED_MIN_SIZE              32
#define SYSTEM_REQUIRED_SIZE_ALIGNMENT        16
#define SIGNATURE_OFFSET                      16
#define PCI_IRQ_ROUTING_TABLE_SIGNATURE   "$PIR"
#define BIOS_MEMORY_ADDRESS              0xe0000
#define BIOS_MEMORY_LEN                  0x20000

extern const char *fio_default_device_label;

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

#define PCI_ADDRESS_LEN 12 /* <domain>:<bus>:<slot>.<func> => 4:2:2.1 => 12 */

extern const char *exclude_devices[MAX_PCI_DEVICES];
extern const char *include_devices[MAX_PCI_DEVICES];

int kfio_ignore_device(const kfio_pci_dev_t *fusion_pdev)
{
    struct pci_dev *pci_dev = (struct pci_dev *)fusion_pdev;
    int idx;
    int result = 0;

    /* Check to see if this device has been explicitly listed in 'exclude_devices' or 'include_devices'.
     * 'exclude_devices' has precedence if the device is specified in both.
     */
    if (num_exclude_devices)
    {
        result = 0;
        for (idx = 0; idx < num_exclude_devices; ++idx)
        {
            if (strncmp(pci_name(pci_dev), exclude_devices[idx], PCI_ADDRESS_LEN) == 0)
            {
                /* Found in exclude devices so don't initialize this device. */
                result = -ENODEV;
                break;
            }
        }
    }
    else if (num_include_devices)
    {
        result = -ENODEV;
        for (idx = 0; idx < num_include_devices; ++idx)
        {
            if (strncmp(pci_name(pci_dev), include_devices[idx], PCI_ADDRESS_LEN) == 0)
            {
                result = 0;
                break;
            }
        }
    }

    return result;
}

uint8_t kfio_pci_get_slot(kfio_pci_dev_t *pdev)
{
#if defined(__i386__) || defined(__x84_64__)
    const uint8_t *next_addr;
    const uint8_t *stop_addr;
    struct pci_dev *dev = (struct pci_dev *)pdev;
    void *bios_addr;

    bios_addr = ioremap_nocache(BIOS_MEMORY_ADDRESS, BIOS_MEMORY_LEN);
    if (bios_addr == NULL)
    {
        errprint("%s: could not map bios\n", pci_name(dev));
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
#endif
    return 0;
}

extern int
iodrive_pci_probe (struct pci_dev *pci_dev, const struct pci_device_id *id);

extern void
iodrive_pci_remove (struct pci_dev *dev);

struct pci_device_id iodrive_ids[] = {
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE_PCI_DEVICE)       },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE1_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_OLD_PCI_DEVICE)  },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_BOOT_PCI_DEVICE) },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IODRIVE3_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREME_PCI_DEVICE)      },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREMEPRO_PCI_DEVICE)   },
    {PCI_DEVICE (PCI_VENDOR_ID_FUSION, IOXTREME2_PCI_DEVICE)     },
    {0,}
};

int kfio_pci_first(kfio_pci_dev_id_t *d)
{
    int i = 0;

    d->vendor = iodrive_ids[i].vendor;
    d->device = iodrive_ids[i].device;

    return i;
}

int kfio_pci_next(kfio_pci_dev_id_t *d, int i)
{
    i++;

    d->vendor = iodrive_ids[i].vendor;
    d->device = iodrive_ids[i].device;

    return i;
}

#if KFIOC_HAS_PCI_ERROR_HANDLERS
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
    if (0 <= retval) return 1;
    return retval;
}

void kfio_pci_unregister_driver(void)
{
    pci_unregister_driver(&iodrive_pci_driver);
}

