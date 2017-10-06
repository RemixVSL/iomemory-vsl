//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
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
#include <fio/port/errno.h>
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
#define IODRIVE_DMA_DIR_INVALID   -1

/**
 * @struct fusion_dma_t
 * @brief placeholder struct that is large enough for the @e real OS-specific
 * structure:
 * Linux => dma_addr_t (size 4 for x86, size 8 for x64)
 * Solaris => struct of ddi_dma_handle_t & ddi_acc_handle_t
 * FreeBSD => struct of bus_dma_map_t (8 bytes) & bus_dmamap_t (8 bytes)
 * OSX     => IOBufferMemoryDescriptor * (8)
 * HPUX    => various mapping structures
 */
struct fusion_dma_t {
    uint64_t phys_addr;
    uint64_t _private[96/sizeof(uint64_t)];
};

typedef struct __fusion_page *fusion_page_t;
typedef struct __fusion_user_page *fusion_user_page_t;

#if FUSION_DEBUG && defined(FUSION_ALLOCATION_TRIPWIRE) && FUSION_ALLOCATION_TRIPWIRE
# define FUSION_ALLOCATION_TRIPWIRE_TEST() \
    kassert(g_fusion_allocation_tripwire == 0)
#else
# define FUSION_ALLOCATION_TRIPWIRE_TEST() ;
#endif

extern void *kfio_dma_alloc_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                     struct fusion_dma_t *dma_handle);

extern void kfio_dma_free_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                   void *vaddr, struct fusion_dma_t *dma_handle);


// Limit to the size that can be allocated via kfio_malloc()
// This limitation is imposed by Linux
#define KFIO_MALLOC_SIZE_MAX (128 * 1024)

extern void *kfio_malloc(fio_size_t size);
extern void *kfio_malloc_node(fio_size_t size, kfio_numa_node_t node);
extern void *kfio_malloc_atomic(fio_size_t size);
extern void *kfio_malloc_atomic_node(fio_size_t size, kfio_numa_node_t node);
extern void  kfio_free(void *ptr, fio_size_t size);

/********************************************************************************
 * All OSes are different, when applying this rule, fine-tuning may be desireable
 *
 * When determining kfio_malloc vs kfio_vmalloc, the general recommendation is:
 * 1. cut-off line is set to 4K at this time.
 * 2. if memory size can be determined during compile time, such as one or more structs,
 *    total size is less than the cut-off line, use kfio_malloc.
 *    Otherwise, use kfio_vmalloc
 * 3. if memory size is determined during run-time, but unlikely being greater than the cut-off line,
 *    such as a path name in kinfo.c, use kfio_malloc.
 * 4. if memory size is determined during run-time (such as user input, num of blocks, etc)
 *    and if it is hard to predict the size, use kfio_vmalloc
 ********************************************************************************/
extern void *kfio_vmalloc(fio_size_t size);
extern void  kfio_vfree(void *ptr, fio_size_t size);

extern void  kfio_free_page(fusion_page_t pg );
extern fusion_page_t kfio_page_from_virt(void *vaddr);
extern void *kfio_page_address(fusion_page_t pg);
extern fusion_page_t kfio_alloc_0_page(kfio_maa_t flags);

#if PORT_SUPPORTS_USER_PAGES
/* This API needs to be called from within a process context (i.e. ioctl or other syscall)
 * Passed-in address is user space address for the context from which you are called.
 */
extern int kfio_get_user_pages(fusion_user_page_t *pages, int nr_pages, fio_uintptr_t start, int write);

/* This API needs to be called from within the same process context (i.e. ioctl or other syscall)
 * where kfio_get_user_pages was called.
 */
extern void kfio_put_user_pages(fusion_user_page_t *pages, int nr_pages);
#else
static inline int kfio_get_user_pages(fusion_user_page_t *pages, int nr_pages, fio_uintptr_t start, int write)
{
    return -ENODEV;
}

static inline void kfio_put_user_pages(fusion_user_page_t *pages, int nr_pages)
{
}
#endif

#define KFIO_DMA_SYNC_FOR_DRIVER 1
#define KFIO_DMA_SYNC_FOR_DEVICE 2
extern int kfio_dma_sync(struct fusion_dma_t *dma_hdl, uint64_t offset, size_t length, unsigned type);

#endif //__FIO_PORT_KMEM_H__
