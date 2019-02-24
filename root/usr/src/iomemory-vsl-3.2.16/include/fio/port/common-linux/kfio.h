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

#include <fio/port/common-linux/commontypes.h>

#endif


// this include comes from gcc, not from linux kernel
#include <stdarg.h>

#ifndef KERN_EMERG
#define KERN_EMERG       "<0>"       // System is unusable
#define KERN_ALERT       "<1>"       // Action must be taken immediately
#define KERN_CRIT        "<2>"       // Critical conditions
#define KERN_ERR         "<3>"       // Error conditions
#define KERN_WARNING     "<4>"       // Warning conditions
#define KERN_NOTICE      "<5>"       // Normal but significant condition
#define KERN_INFO        "<6>"       // Informational
#define KERN_DEBUG       "<7>"       // Debug-level messages
#endif

/* Linux-only PCI functions */
extern uint8_t kfio_find_slot_number(uint8_t *slot_number, void *dev);

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

#define kfio_cmpxchg(dst, old, newv)    cmpxchg((dst), (old), (newv))
#define kfio_xchg(dst, value)           xchg((dst), (value))

#if defined(__KERNEL__)

#include <fio/port/common-linux/div64.h>

# define kfio_do_div(n, rem, base) do {                       \
            uint32_t __base = (base);                         \
            (rem) = kfio_mod64_64((uint64_t)(n), __base);     \
            (n) = kfio_div64_64((uint64_t)(n), __base);       \
        } while (0)

// ESX-specific block-mode limit; capped at 2TB
# if defined(__VMKLNX__) && (KFIO_SCSI_DEVICE==0)
#  define PORT_FORMATTED_CAPACITY_BYTES_MAX (1ULL<<41)
# endif

#endif

#endif /* __FIO_PORT_LINUX_KFIO_H__ */

#if (DISABLE_MODULE_PARAM_EXPORT==0)
#define KFIO_MODULE_PARAM(variable,type,perm) module_param(variable,type,perm)
#define KFIO_MODULE_PARAM_DESC(parm,desc) MODULE_PARM_DESC(parm,desc)
#define KFIO_MODULE_PARAM_ARRAY(name,type,nump,perm) module_param_array(name,type,nump,perm)
#define KFIO_MODULE_PARAM_STRING(name,string,len,perm) module_param_string(name,string,len,perm)
#else
#define KFIO_MODULE_PARAM(variable,type,perm)
#define KFIO_MODULE_PARAM_DESC(parm,desc)
#define KFIO_MODULE_PARAM_ARRAY(name,type,nump,perm)
#define KFIO_MODULE_PARAM_STRING(name,string,len,perm)
#endif

#if (DISABLE_EXPORTED_SYMBOLS==0)
#define KFIO_EXPORT_SYMBOL(sym) EXPORT_SYMBOL(sym)
#else
#define KFIO_EXPORT_SYMBOL(sym)
#endif

