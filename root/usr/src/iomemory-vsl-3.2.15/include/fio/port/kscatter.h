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

/** @file include/fio/port/kscatter.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#include <fio/port/errno.h>
#ifndef __FIO_PORT_KSCATTER_H__
#define __FIO_PORT_KSCATTER_H__

/// The max number of sectors the OS is allowed to ask for in a single request.
///
/// This value was previously hardcoded in kfio_setup_gendisk(); its derivation is
/// unknown. It is likely it was made up at random.
///
/// Note that in linux, this value is allowed to differ from the maximum scatter-gather
/// list we will accept from the OS. That value is determined by the
/// ioctx_nvec setting, derived from FUSION_IOCTX_MIN_VECS.
/// It is likely other OSes may have to cap FUSION_MAX_SECTORS_PER_OS_RW_REQUEST to
/// prevent overrunning the scatter-gather list.
/// 2048 allows 1MB IO at 512B sectors
#define FUSION_MAX_SECTORS_PER_OS_RW_REQUEST (2048)  ///< Trims can be larger than this!

/* Transparent sglist handle. */
#ifndef KSCATTER_IMPL
typedef struct  __kfio_port_sg_list kfio_sg_list_t;
#else
typedef void    kfio_sg_list_t;
#endif
typedef struct  kfio_dma_map kfio_dma_map_t;

extern int      kfio_sgl_alloc_nvec(kfio_pci_dev_t *pcidev, kfio_numa_node_t node, kfio_sg_list_t **sgl, int nvecs);
extern void     kfio_sgl_destroy(kfio_sg_list_t *sgl);
extern void     kfio_sgl_reset(kfio_sg_list_t *sgl);
extern uint32_t kfio_sgl_size(kfio_sg_list_t *sgl);
extern int      kfio_sgl_map_page(kfio_sg_list_t *sgl, fusion_page_t page,
                                  uint32_t offset, uint32_t size);
extern int      kfio_sgl_map_bytes(kfio_sg_list_t *sgl, const void *buffer, uint32_t size);

extern void     kfio_sgl_dump(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, const char *prefix, unsigned dump_contents);
extern int      kfio_sgl_num_mapped(kfio_sg_list_t *sgl);

struct kfio_sgl_phys
{
    uint64_t addr;
    uint32_t len;
};

typedef struct kfio_sgl_phys kfio_sgl_phys_t;
typedef void  *kfio_sgl_iter_t;

/* Physical segment enumeration interface. */
extern kfio_sgl_iter_t kfio_sgl_phys_first(kfio_sg_list_t *sgl, kfio_sgl_phys_t *segp);
extern kfio_sgl_iter_t kfio_sgl_phys_next(kfio_sg_list_t *sgl, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp);


extern kfio_dma_map_t *kfio_dma_map_alloc(int may_sleep, kfio_numa_node_t node);
extern void kfio_dma_map_free(kfio_dma_map_t *dmap);

extern kfio_sgl_iter_t kfio_dma_map_first(kfio_dma_map_t *dmap, kfio_sgl_phys_t *segp);
extern kfio_sgl_iter_t kfio_dma_map_next(kfio_dma_map_t *dmap, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp);
extern int kfio_dma_map_nvecs(kfio_dma_map_t *dmap);
extern uint32_t kfio_dma_map_size(kfio_dma_map_t *dmap);

extern int kfio_sgl_dma_map(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, int direction);
extern int kfio_sgl_dma_unmap(kfio_sg_list_t *sgl);
extern int kfio_sgl_dma_slice(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, uint32_t offset, uint32_t length);
#if PORT_SUPPORTS_USER_PAGES
extern int kfio_sgl_map_user_pages(kfio_sg_list_t *sgl, fusion_user_page_t *pages, uint32_t size, uint32_t offset);
#else
static inline int kfio_sgl_map_user_pages(kfio_sg_list_t *sgl, fusion_user_page_t *pages, uint32_t size, uint32_t offset)
{
    return -ENODEV;
}
#endif

#if PORT_SUPPORTS_SGLIST_COPY
extern int kfio_sgl_copy_data(kfio_sg_list_t *dst, kfio_sg_list_t *src, uint32_t length);
#endif

/* Debug interface used only in error injection. */
#if ENABLE_ERROR_INJECTION
# if defined(__linux__) && !defined(__VMKLNX__) || (defined(__SVR4) && defined(__sun)) || defined (__OSX__) || defined(USERSPACE_KERNEL)
extern void *kfio_sgl_get_byte_pointer(kfio_sg_list_t *sgl, uint32_t offset);
# else
#  define kfio_sgl_get_byte_pointer(sql, offset) 0
# endif
#endif

#endif  /* __FIO_PORT_KSCATTER_H__ */
