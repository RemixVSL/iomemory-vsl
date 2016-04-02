//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

/** @file include/fio/port/kscatter.h 
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KSCATTER_H__
#define __FIO_PORT_KSCATTER_H__


/**
 * @brief Hack to limit number of SG vectors to a sane number
 * It should be ((FIO_MSG_MAX_PACKETS * PACKET_SIZE) / PAGESIZE)
 * The extra SG list entry (+1) allows non-page-aligned buffers
 * unfortunatiely packet size is run-time decided so this is
 * an approximation and a 'worst case'. For larger packet sizes it really
 * results in less than optimal number of packets per request, but we can live 
 * with it.
 */
#define FUSION_BUFFER_NUM_VECS             ((2048 / 8) + 1)

/// The max number of sectors the OS is allowed to ask for in a single request.
///
/// This value was previously hardcoded in kfio_setup_gendisk(); its derivation is
/// unknown. It is likely it was made up at random.
///
/// Note that in linux, this value is allowed to differ from the maximum scatter-gather
/// list we will accept from the OS. That value is determined by FUSION_BUFFER_NUM_VECS.
/// It is likely other OSes may have to cap FUSION_MAX_SECTORS_PER_OS_REQUEST to
/// prevent overrunning the scatter-gather list.
/// 2048 allows 1MB IO at 512B sectors.  This must not be > FIO_MSG_MAX_PACKETS
#define FUSION_MAX_SECTORS_PER_OS_REQUEST 2048

/* Transparent sglist handle. */
#ifndef KSCATTER_IMPL
typedef struct  __kfio_port_sg_list kfio_sg_list_t;
#else
typedef void    kfio_sg_list_t;
#endif

extern int      kfio_sgl_alloc_nvec(kfio_pci_dev_t *pcidev, kfio_sg_list_t **sgl, int nvecs);
extern void     kfio_sgl_destroy(kfio_sg_list_t *sgl);
extern void     kfio_sgl_reset(kfio_sg_list_t *sgl);
extern uint32_t kfio_sgl_size(kfio_sg_list_t *sgl);
extern int      kfio_sgl_map_page(kfio_sg_list_t *sgl, fusion_page_t page,
                                  uint32_t offset, uint32_t size);
extern int      kfio_sgl_map_bytes(kfio_sg_list_t *sgl, const void *buffer, uint32_t size);

extern void     kfio_sgl_dump(kfio_sg_list_t *sgl, const char *prefix, unsigned dump_contents);
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

typedef struct kfio_dma_map kfio_dma_map_t;

extern kfio_dma_map_t *kfio_dma_map_alloc(void);
extern void kfio_dma_map_free(kfio_dma_map_t *dmap);

extern kfio_sgl_iter_t kfio_dma_map_first(kfio_dma_map_t *dmap, kfio_sgl_phys_t *segp);
extern kfio_sgl_iter_t kfio_dma_map_next(kfio_dma_map_t *dmap, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp);
extern int kfio_dma_map_nvecs(kfio_dma_map_t *dmap);

extern int kfio_sgl_dma_map(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, int direction);
extern int kfio_sgl_dma_unmap(kfio_sg_list_t *sgl);
extern int kfio_sgl_dma_slice(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, uint32_t offset, uint32_t length);

/* Debug interface used only in error injection. */
#if ENABLE_ERROR_INJECTION
# if defined(__linux__) && !defined(EXTERNAL_OS) && !defined(__VMKLNX__) || (defined(__SVR4) && defined(__sun))
extern void *kfio_sgl_get_byte_pointer(kfio_sg_list_t *sgl, uint32_t offset);
# else
#  define kfio_sgl_get_byte_pointer(sql, offset) 0
# endif
#endif

#endif  /* __FIO_PORT_KSCATTER_H__ */

