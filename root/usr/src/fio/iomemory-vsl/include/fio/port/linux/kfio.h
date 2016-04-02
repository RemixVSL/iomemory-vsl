
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_LINUX_KFIO_H__
#define __FIO_PORT_LINUX_KFIO_H__

#ifndef __FIO_PORT_KFIO_H__
#error Do not include this file directly - instead include <fio/port/kfio.h>
#endif

/* The strategy is to migrate to a more general portability layer.  There
 * should be very few OS-specifics in this file.  
 */

#if defined(__KERNEL__)

#include <fio/port/ktypes.h>

#else

#include <fio/port/linux/commontypes.h>

#endif


// this include comes from gcc, not from linux kernel
#include <stdarg.h>

#define KERN_EMERG       "<0>"       // System is unusable
#define KERN_ALERT       "<1>"       // Action must be taken immediately
#define KERN_CRIT        "<2>"       // Critical conditions
#define KERN_ERR         "<3>"       // Error conditions
#define KERN_WARNING     "<4>"       // Warning conditions
#define KERN_NOTICE      "<5>"       // Normal but significant condition
#define KERN_INFO        "<6>"       // Informational
#define KERN_DEBUG       "<7>"       // Debug-level messages

/* Linux-only PCI functions */
extern uint8_t kfio_find_slot_number(uint8_t *slot_number, void *dev);
extern int kfio_pci_first(kfio_pci_dev_id_t *d);
extern int kfio_pci_next(kfio_pci_dev_id_t *d, int id);

extern kfio_pci_bus_t *kfio_bus_from_pci_dev(kfio_pci_dev_t *pdev);
extern kfio_pci_bus_t *kfio_pci_bus_parent(kfio_pci_bus_t *bus);
extern int kfio_pci_bus_istop(kfio_pci_bus_t *bus);
extern kfio_pci_dev_t *kfio_pci_bus_self(kfio_pci_bus_t *bus);
extern uint8_t kfio_pci_bus_number(kfio_pci_bus_t *bus);
extern void kfio_pci_dev_put(kfio_pci_dev_t *pdev);
extern kfio_pci_dev_t *kfio_pci_get_device(unsigned int vendor, unsigned int device, kfio_pci_dev_t *from);

extern uint64_t kfio_pci_resource_start(kfio_pci_dev_t *pdev, uint16_t bar);
extern uint32_t kfio_pci_resource_len(kfio_pci_dev_t *pdev, uint16_t bar);

extern void kfio_pci_disable_relaxed_ordering(kfio_pci_dev_t *pdev);

extern void *kfio_ioremap_nocache (unsigned long offset, unsigned long size);
extern void  kfio_iounmap(void *addr);

/* disk / block stuff */

// Function to wait for device file creation
extern int fio_wait_for_dev(const char *devname);


// Newer Linux kernels put_user macro use sizeof on arg in the put_user macro.
// Thusly we need to call the proper wrapper function to use the macro without
// errors.

#define kfio_get_user(x, arg)                                 \
    ({                                                        \
        int pu_ret;                                           \
                                                              \
        switch (sizeof(*(arg)))                               \
        {                                                     \
        case 1:                                               \
            pu_ret = kfio_get_user_8(&x, (uint8_t *)arg);     \
            break;                                            \
        case 2:                                               \
            pu_ret = kfio_get_user_16(&x, (uint16_t *)arg);   \
            break;                                            \
        case 4:                                               \
            pu_ret = kfio_get_user_32(&x, (uint32_t *)arg);   \
            break;                                            \
        case 8:                                               \
            pu_ret = kfio_get_user_64(&x, (uint64_t *)arg);   \
            break;                                            \
        }                                                     \
                                                              \
        pu_ret;                                               \
    })

extern void kfio_register_reboot_notifier(void);
extern void kfio_unregister_reboot_notifier(void);

#if defined(__KERNEL__)

#include <fio/port/linux/div64.h>

# define kfio_do_div(n, rem, base) do {                       \
            uint32_t __base = (base);                         \
            (rem) = kfio_mod64_64((uint64_t)(n), __base);     \
            (n) = kfio_div64_64((uint64_t)(n), __base);       \
        } while (0)
#endif

#endif /* __FIO_PORT_LINUX_KFIO_H__ */
