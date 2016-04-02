//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OSX v 10.6+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KMEM_H__
#define __FIO_PORT_KMEM_H__

/** @brief these are arbitrary and do @e not correspond to the linux flags
 */
/* general memory allocation with no special context */
#define KFIO_MAA_NORMAL         1
/* IO isn't allowed during allocation */
#define KFIO_MAA_NOIO           2
/* Atomic and high priority/emergency pool access */
#define KFIO_MAA_NOWAIT         4

/** DMA defines **/
#define IODRIVE_DMA_DIR_WRITE      0
#define IODRIVE_DMA_DIR_READ       1

/**
 * @struct fusion_dma_t
 * @brief placeholder struct that is large enough for the @e real OS-specific
 * structure:
 * Linux => dma_addr_t (size 4 for x86, size 8 for x64)
 * Solaris => struct of ddi_dma_handle_t & ddi_acc_handle_t
 * FreeBSD => struct of bus_dma_map_t (8 bytes) & bus_dmamap_t (8 bytes)
 * OSX     => IOBufferMemoryDescriptor * (8)
 */
struct fusion_dma_t {
    uint64_t phys_addr;
    char _private[64];
};

typedef struct __fusion_page *fusion_page_t;

#if FUSION_DEBUG && FUSION_ALLOCATION_TRIPWIRE
# define FUSION_ALLOCATION_TRIPWIRE_TEST() \
    kassert(g_fusion_allocation_tripwire == 0)
#else
# define FUSION_ALLOCATION_TRIPWIRE_TEST() ;
#endif

extern void *kfio_dma_alloc_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                     struct fusion_dma_t *dma_handle);

extern void kfio_dma_free_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                   void *vaddr, struct fusion_dma_t *dma_handle);


extern void *kfio_malloc(fio_size_t size);
extern void *kfio_malloc_atomic(fio_size_t size);
extern void  kfio_free(void *ptr, fio_size_t size);

extern void *kfio_vmalloc(fio_size_t size);
extern void kfio_vfree(void *ptr, fio_size_t size);
extern void  kfio_free_page(fusion_page_t pg );
extern void *kfio_page_address(fusion_page_t pg);

extern fusion_page_t kfio_alloc_0_page(kfio_maa_t flags);

#define KFIO_DMA_SYNC_FOR_DRIVER 1
#define KFIO_DMA_SYNC_FOR_DEVICE 2
extern int kfio_dma_sync(struct fusion_dma_t *dma_hdl, uint64_t offset, size_t length, unsigned type);

#endif //__FIO_PORT_KMEM_H__
