//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2016 SanDisk Corp. and/or all its affiliates. All rights reserved.
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

#ifndef __FIO_PORT_KPCI_H__
#define __FIO_PORT_KPCI_H__

#include <fio/port/ktypes.h>

#if !defined(USERSPACE_KERNEL)
# define __FIO_PORT_PCI_DEV_T_DEFINED
# if defined(UEFI)
#  include <fio/port/uefi/kpci.h>
# elif defined(__VMKAPI__)
#  include <fio/port/esxi6/kpci.h>
# elif defined(__linux__) || defined(__VMKLNX__)
#  include <fio/port/common-linux/kpci.h>
# elif defined(__FreeBSD__)
#  include <fio/port/freebsd/kpci.h>
# elif defined(__OSX__)
#  include <fio/port/osx/kpci.h>
# elif defined(__sun__)
#  include <fio/port/solaris/kpci.h>
# elif defined(WINNT) || defined(WIN32)
#  include <fio/port/windows/kpci.h>
# else
#  undef __FIO_PORT_PCI_DEV_T_DEFINED
# endif
#else
# undef __FIO_PORT_PCI_DEV_T_DEFINED
#endif

extern int kfio_pci_read_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t *val);
extern int kfio_pci_read_config_word(kfio_pci_dev_t *pdev, int where, uint16_t *val);
extern int kfio_pci_read_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t *val);
extern int kfio_pci_write_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t val);
extern int kfio_pci_write_config_word(kfio_pci_dev_t *pdev, int where, uint16_t val);
extern int kfio_pci_write_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t val);

extern uint8_t kfio_pci_get_bus(kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_devicenum(kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_function(kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_slot(kfio_pci_dev_t *pdev);

extern uint16_t kfio_pci_get_vendor(kfio_pci_dev_t *pdev);
extern uint32_t kfio_pci_get_devnum(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_vendor(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_device(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_domain(kfio_pci_dev_t *pdev);

/**
 * @struct fusion_dma_t
 * @brief placeholder struct that is large enough for the @e real OS-specific
 * structure:
 * Linux => dma_addr_t (size 4 for x86, size 8 for x64)
 * Solaris => struct of ddi_dma_handle_t & ddi_acc_handle_t
 * FreeBSD => struct of bus_dma_map_t (8 bytes) & bus_dmamap_t (8 bytes)
 * OSX     => IOBufferMemoryDescriptor * (8)
 * UEFI    => Mapping & EFI_PCI_IO_PROTOCOL
 */
struct fusion_dma_t
{
    uint64_t phys_addr;
    uint64_t _private[96/sizeof(uint64_t)];
};

extern void *kfio_dma_alloc_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                     struct fusion_dma_t *dma_handle);

extern void kfio_dma_free_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                   void *vaddr, struct fusion_dma_t *dma_handle);

#define KFIO_DMA_SYNC_FOR_DRIVER 1
#define KFIO_DMA_SYNC_FOR_DEVICE 2
extern int kfio_dma_sync(struct fusion_dma_t *dma_hdl, uint64_t offset, size_t length, unsigned type);

#if !defined(WIN32) && !defined(WINNT)

extern int  kfio_request_irq(kfio_pci_dev_t *pdev, const char *devname,
                             void *iodrive_dev, int msi_enabled);
extern void kfio_free_irq(kfio_pci_dev_t *pdev, void *dev);

#endif /* !defined(WIN32) */

#if defined(PORT_SUPPORTS_MSIX)

#if (!defined(WIN32) && !defined(WINNT)) || defined(UEFI)
extern int  kfio_request_msix(kfio_pci_dev_t *pdev, const char *devname, void *iodrive_dev,
                              kfio_msix_t *msix, unsigned int vector);
extern void kfio_free_msix(kfio_msix_t *msix, unsigned int vector, void *dev);
extern unsigned int kfio_pci_enable_msix(kfio_pci_dev_t *pdev, kfio_msix_t *msix, unsigned int nr_vecs);
extern void kfio_pci_disable_msix(kfio_pci_dev_t *pdev);
#endif
extern int  kfio_get_msix_number(kfio_msix_t *msix, uint32_t vec_ix, uint32_t *irq);

#else

typedef struct kfio_msix
{
    DECLARE_RESERVE(1);
} kfio_msix_t;

#if !defined(WIN32) && !defined(WINNT)
static inline int kfio_request_msix(kfio_pci_dev_t *pdev, const char *devname, void *iodrive_dev,
                                    kfio_msix_t *msix, unsigned int vector)
{
    return -EINVAL;
}

static inline int kfio_pci_enable_msix(kfio_pci_dev_t *pdev, kfio_msix_t *msix, unsigned int nr_vecs)
{
    return 0;
}

static inline void kfio_pci_disable_msix(kfio_pci_dev_t *pdev)
{
}

static inline void kfio_free_msix(kfio_msix_t *msix, unsigned int vector, void *dev)
{
}

#endif /* !defined(WIN32) */

static inline int kfio_get_msix_number(kfio_msix_t *msix, uint32_t vec_ix, uint32_t *irq)
{

    return -ENOENT;
}
#endif /* PORT_SUPPORTS_MSIX */

#endif /* __FIO_PORT_KPCI_H__ */
