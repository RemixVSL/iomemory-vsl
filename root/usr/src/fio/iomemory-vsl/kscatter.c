//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#define KSCATTER_IMPL
#include "port-internal.h"
#include <fio/port/dbgset.h>

struct linux_sgentry
{
    fusion_page_t       page;      /* pointer to struct page */
    uint32_t            offset;    /* offset from above page */
    uint32_t            length;    /* length of memory to be mapped */
    dma_addr_t          dma_addr;  /* bus address for programming card */
#if defined(__VMKLNX__)
    fusion_page_t       orig_page; /* pointer to original page in case of double buffering */
    uint32_t            orig_off;  /* offset from above page */
    uint32_t            flags;
#endif
};

#define SGE_BUFFERED    0x1

struct kfio_dma_map
{
    uint32_t              map_offset;
    uint32_t              map_length;
    uint32_t              sge_count;
    uint32_t              sge_skip;
    uint32_t              sge_trunc;
    struct linux_sgentry *sge_first;
    struct linux_sgentry *sge_last;
};

struct linux_sgl
{
    uint32_t            max_entries;
    uint32_t            num_entries;
    uint32_t            num_mapped;
    uint32_t            sgl_size;
    struct pci_dev     *pci_dev;
    int                 pci_dir;
    struct linux_sgentry sge[1];
};

int kfio_sgl_alloc_nvec(kfio_pci_dev_t *pcidev, kfio_sg_list_t **sgl, int nvecs)
{
    struct linux_sgl *lsg;

#if defined(__VMKLNX__)
    // kmalloc pool is very limited on ESX and gets depleted with more than 7 cards or so.
    lsg = kfio_vmalloc(sizeof(*lsg) + (nvecs - 1) * sizeof(struct linux_sgentry));
#else
    lsg = kfio_malloc(sizeof(*lsg) + (nvecs - 1) * sizeof(struct linux_sgentry));
#endif
    if (NULL == lsg)
    {
        return -ENOMEM;
    }

    lsg->max_entries = nvecs;
    lsg->num_entries = 0;
    lsg->num_mapped  = 0;
    lsg->sgl_size    = 0;

    lsg->pci_dev = (struct pci_dev *)pcidev;
    lsg->pci_dir = 0;

    *sgl = lsg;
    return 0;
}

void kfio_sgl_destroy(kfio_sg_list_t *sgl)
{
    struct linux_sgl *lsg = sgl;

    if (lsg->num_mapped != 0)
    {
       kfio_sgl_dma_unmap(lsg);
    }

#if defined(__VMKLNX__)
    kfio_vfree(lsg, sizeof(*lsg) +
                    (lsg->max_entries - 1) * sizeof(struct linux_sgentry));
#else
    kfio_free(lsg, sizeof(*lsg) +
                    (lsg->max_entries - 1) * sizeof(struct linux_sgentry));
#endif
}

void kfio_sgl_reset(kfio_sg_list_t *sgl)
{
    struct linux_sgl *lsg = sgl;

    if (lsg->num_mapped != 0)
    {
       kfio_sgl_dma_unmap(lsg);
    }

    lsg->sgl_size    = 0;
    lsg->num_mapped  = 0;
    lsg->num_entries = 0;
}

uint32_t kfio_sgl_size(kfio_sg_list_t *sgl)
{
    struct linux_sgl *lsg = sgl;

    return lsg->sgl_size;
}

/**
 * returns virtual address
 */
static void *kfio_sgl_get_vaddr(struct linux_sgentry *sge)
{
    return page_address((struct page *) (sge->page)) + sge->offset;
}

int kfio_sgl_map_bytes(kfio_sg_list_t *sgl, const void *buffer, uint32_t size)
{
    const uint8_t *bp = buffer;
    struct linux_sgl *lsg = sgl;
#if !defined(__VMKLNX__)
    bool vmalloc_buffer;

#if defined(is_vmalloc_addr)
    vmalloc_buffer = is_vmalloc_addr(buffer);
# else
    vmalloc_buffer = (((fio_uintptr_t)buffer) >= VMALLOC_START &&
                      ((fio_uintptr_t)buffer) < VMALLOC_END);
#endif /* is_vmalloc_addr */
#endif

    while (size)
    {
        fusion_page_t page;
        uint32_t     mapped_bytes;
        uint32_t     page_offset, page_remainder;
        struct linux_sgentry *sge;

        sge = &lsg->sge[lsg->num_entries];

#if defined(__VMKLNX__)
        sge->flags     = 0;
        sge->orig_page = NULL;
        sge->orig_off  = 0;
#endif

        if (lsg->num_entries >= lsg->max_entries)
        {
            dbgprint(DBGS_GENERAL, "%s: too few sg entries (cnt: %d nvec: %d size: %d)\n",
                   __func__, lsg->num_entries, lsg->max_entries, size);

            return -ENOMEM;
        }

        page_offset    = (uint32_t)((fio_uintptr_t)bp % FUSION_PAGE_SIZE);
        page_remainder = FUSION_PAGE_SIZE - page_offset;
        mapped_bytes   = page_remainder > size ? size : page_remainder;

#if !defined(__VMKLNX__)
        if (vmalloc_buffer)
        {
            /*
             * Do extra casting to get rid of const not expected by
             * vmalloc_to_page on older Linux kernels.
             */
            page = (fusion_page_t) vmalloc_to_page((void *)(fio_uintptr_t)bp);
        }
        else
#endif
        {
            page = (fusion_page_t) virt_to_page((void *)(fio_uintptr_t)bp);
        }

        kassert_release(page != NULL);

        sge->page   = page;
        sge->offset = page_offset;
        sge->length = mapped_bytes;

        size -= mapped_bytes;
        bp   += mapped_bytes;

        lsg->num_entries++;
        lsg->sgl_size += mapped_bytes;
    }

    return 0;
}

/**
 * @brief Inserts page into the provided kfio_sg_list_t, and updates
 *   sgl->num_entries. Does not actually perform DMA mapping.
 * Called by kfio_sgl_map_bio(..) q.v., and by
 * fio_request_map_metadata_block(..)
 *
 * @return 0 on success
 */
int kfio_sgl_map_page(kfio_sg_list_t *sgl, fusion_page_t page,
                      uint32_t offset, uint32_t size)
{
    struct linux_sgl *lsg = sgl;
    struct linux_sgentry *sge;

    if (unlikely(lsg->num_entries >= lsg->max_entries))
    {
        dbgprint(DBGS_GENERAL, "No room to map request_page\n");
        return -EINVAL;
    }

    if (unlikely(offset + size > FUSION_PAGE_SIZE))
    {
#if defined(__VMKLNX__)
        vmk_MachAddr maddr;
        vmk_MachPage this_page = (vmk_MachPage)(uint64_t)page, page_plus_one;
        vmk_VirtAddr vaddr;

        maddr = vmk_MachPageToMachAddr(this_page);
        vmk_MachAddrToVirtAddr(maddr, &vaddr);
        vmk_VirtAddrToMachAddr(vaddr + FUSION_PAGE_SIZE, &maddr);
        page_plus_one = vmk_MachAddrToMachPage(maddr);

        if (this_page + 1 != page_plus_one)
        {
            errprint("Attempt to map too great a span: this_page=%llx page_plus_one=%llx\n",
                     (uint64_t)this_page, (uint64_t)page_plus_one);
            return -EINVAL;
        }
#elif !defined(DMA_X_PAGE_BOUNDARY)
        dbgprint(DBGS_GENERAL, "Attempt to map too great a span\n");
        return -EINVAL;
#endif
    }

    sge = &lsg->sge[lsg->num_entries];

#if defined(__VMKLNX__)
    // Special handling for unaligned buffers on ESX
    sge->flags     = 0;
    sge->orig_page = NULL;
    sge->orig_off  = offset;

    // We are thoroughly screwed if we get a size that is not an 8-byte multiple.
    // This being ESX we can't be too cautious.
    if (size & 0x7)
    {
        errprint("%s: Invalid size: 0x%x page=0x%llx offset=0x%x\n",
                 __FUNCTION__, size, (uint64_t)page, offset);
        return -EINVAL;
    }

    if (unlikely(offset & 7))
    {
        /* Firmware can't handle nonzero offsets unless they are 8-byte aligned.
         * This operation will require double buffering.
         */

        if (offset + size > FUSION_PAGE_SIZE)
        {
            dbgprint(DBGS_DMA,
                     "Handling unaligned contiguous-page DMA: page=%llx offset=%x size=%x\n",
                     (uint64_t)page, offset, size);
        }

        // We can get unaligned multi-page (contiguous) DMA requests in ESX,
        // so we need to loop and create more sg entries as needed.
        while (size)
        {
            fusion_page_t buffered_page;
            uint32_t mapped_bytes;

            if (lsg->num_entries >= lsg->max_entries)
            {
                errprint("%s: too few sg entries (cnt=%d nvec=%d size=0x%x)\n",
                       __FUNCTION__, lsg->num_entries, lsg->max_entries, size);
                return -ENOMEM;
            }

            sge->flags     = 0;
            sge->orig_page = page;
            sge->orig_off  = offset;

            if (size + offset > FUSION_PAGE_SIZE)
            {
                mapped_bytes = FUSION_PAGE_SIZE - (offset & ~0x7);
                offset = offset & 0x7;
            }
            else
            {
                mapped_bytes = size;
                offset = 0;
            }

            buffered_page = kfio_alloc_0_page(KFIO_MAA_NORMAL);
            if (buffered_page == NULL)
            {
                return -ENOMEM;
            }
            sge->page = buffered_page;
            sge->flags |= SGE_BUFFERED;
            sge->offset = 0;
            sge->length = mapped_bytes;

            size -= mapped_bytes;
            page = (fusion_page_t)((uint64_t)page + 1);

            lsg->num_entries++;
            lsg->sgl_size += mapped_bytes;
            sge = &lsg->sge[lsg->num_entries];
        }
    }
    else
#endif
    {
        sge->page = page;
        sge->offset = offset;
        sge->length = size;

        lsg->num_entries++;
        lsg->sgl_size += size;
    }

    return 0;
}

int kfio_sgl_map_bio(kfio_sg_list_t *sgl, struct bio *pbio)
{
    struct linux_sgl *lsg = sgl;
    struct linux_sgentry *sge;
    struct bio_vec *vec;
    int i, rval;

    bio_for_each_segment(vec, pbio, i)
    {
        if (lsg->num_entries > 0)
        {
            /*
             * This iovec shares page with previos one. Merge them together. The biggest
             * entry we can get this way is limited by page size.
             */
            sge = &lsg->sge[lsg->num_entries - 1];

            if (sge->page == (fusion_page_t)vec->bv_page &&
                sge->offset + sge->length == vec->bv_offset)
            {
                sge->length   += vec->bv_len;
                lsg->sgl_size += vec->bv_len;
                continue;
            }
        }

        rval = kfio_sgl_map_page(sgl, (fusion_page_t) vec->bv_page,
                                 vec->bv_offset, vec->bv_len);
        if (rval)
        {
            dbgprint(DBGS_GENERAL, "Failed to map bio_vec\n" );
            return rval;
        }
    }
    return 0;
}
EXPORT_SYMBOL(kfio_sgl_map_bio);

int kfio_sgl_dma_map(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, int dir)
{
    struct linux_sgl *lsg = sgl;

    lsg->pci_dir = dir;

    for (lsg->num_mapped = 0; lsg->num_mapped < lsg->num_entries; lsg->num_mapped++)
    {
        struct linux_sgentry *sge;

        sge = &lsg->sge[lsg->num_mapped];

#if defined(__VMKLNX__)
        if (sge->flags & SGE_BUFFERED && dir == IODRIVE_DMA_DIR_WRITE)
        {
            kfio_memcpy(((char *)kfio_page_address(sge->page)) + sge->offset,
                        ((char *)kfio_page_address(sge->orig_page)) + sge->orig_off,
                        sge->length);
        }
#endif

        sge->dma_addr = pci_map_page(lsg->pci_dev, (struct page *)sge->page,
                                     sge->offset, sge->length,
                                     dir == IODRIVE_DMA_DIR_READ ? PCI_DMA_FROMDEVICE : PCI_DMA_TODEVICE);

#if !defined (__PPC64__) && !defined(__PPC__) && !defined (__mips64) && (KFIOC_HAS_DMA_ERROR_CODE == 0)
        if (!sge->dma_addr || bad_dma_address == sge->dma_addr)
#else
        if (!sge->dma_addr || DMA_ERROR_CODE == sge->dma_addr)
#endif
        {
            goto bail;
        }
    }

    /*
     * Fill in a map covering the whole scatter-gather list if caller is
     * interested in the information.
     */
    if (dmap != NULL)
    {
        dmap->map_offset = 0;
        dmap->map_length = lsg->sgl_size;
        dmap->sge_count  = lsg->num_mapped;
        dmap->sge_skip   = 0;
        dmap->sge_trunc  = 0;
        dmap->sge_first  = &lsg->sge[0];
        dmap->sge_last   = &lsg->sge[lsg->num_mapped - 1];
    }
    return lsg->num_mapped;

bail:
    kfio_sgl_dma_unmap(sgl);
    return -EINVAL;
}
EXPORT_SYMBOL(kfio_sgl_dma_map);

int kfio_sgl_dma_unmap(kfio_sg_list_t *sgl)
{
    struct linux_sgl *lsg = sgl;
    int i;

    for (i = 0; i < lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge;

        sge = &lsg->sge[i];
#if !defined (__PPC64__) && !defined(__PPC__) && !defined (__mips64) && (KFIOC_HAS_DMA_ERROR_CODE == 0)
        if (sge->dma_addr && bad_dma_address != sge->dma_addr)
#else
        if (sge->dma_addr && DMA_ERROR_CODE != sge->dma_addr)
#endif
        {
#if defined(__VMKLNX__)
            if (sge->flags & SGE_BUFFERED)
            {
                if (lsg->pci_dir == IODRIVE_DMA_DIR_READ)
                {
                    kfio_memcpy(((char *)kfio_page_address(sge->orig_page)) + sge->orig_off,
                                ((char *)kfio_page_address(sge->page)) + sge->offset, sge->length);
                }
                kfio_free_page(sge->page);
            }
#endif

            pci_unmap_page(lsg->pci_dev,
                           sge->dma_addr, sge->length,
                           lsg->pci_dir == IODRIVE_DMA_DIR_READ ? PCI_DMA_FROMDEVICE : PCI_DMA_TODEVICE);
            sge->dma_addr = 0;
        }
    }
    lsg->num_mapped = 0;
    lsg->pci_dir = 0;
    return 0;
}
EXPORT_SYMBOL(kfio_sgl_dma_unmap);

#if defined(DMA64_OS32)
#define PRIdma "llx"
#define DMA_PR_T long long
#else
#define PRIdma "p"
#define DMA_PR_T void *
#endif

void kfio_sgl_dump(kfio_sg_list_t *sgl, const char *prefix, unsigned dump_contents)
{
    struct linux_sgl *lsg = sgl;
    uint32_t i, j;

    infprint("%s sglist %p num %u max %u size %u\n",
             prefix, lsg, lsg->num_entries, lsg->max_entries, lsg->sgl_size);

    for (i = 0; i < lsg->num_entries; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];
        const uint8_t *bp = kfio_sgl_get_vaddr(sge);

        infprint("%s        mvec %d: vaddr: %p offset: %u size: %d paddr: %" PRIdma"\n",
                 prefix, i, bp, sge->offset, sge->length, (DMA_PR_T)sge->dma_addr);

        if (dump_contents)
        {
            for (j = 0; j < sge->length; j++)
            {
                if (j % 16 == 0)
                {
                    kfio_print("\n%04d:", j);
                }
                kfio_print(" %02x", bp[j]);
            }
            kfio_print("\n");
        }
    }
}

/* Physical segment enumeration interface. */
kfio_sgl_iter_t kfio_sgl_phys_first(kfio_sg_list_t *sgl, kfio_sgl_phys_t *segp)
{
    struct linux_sgl *lsg = sgl;

    if (lsg->num_mapped != 0)
    {
        if (segp != NULL)
        {
            segp->addr = lsg->sge[0].dma_addr;
            segp->len  = lsg->sge[0].length;
        }
        return &lsg->sge[0];
    }
    return NULL;
}

kfio_sgl_iter_t kfio_sgl_phys_next(kfio_sg_list_t *sgl, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp)
{
    struct linux_sgl *lsg = sgl;
    struct linux_sgentry *sge = iter;

    if (lsg->num_mapped != 0)
    {
        if (sge >= &lsg->sge[0] && (sge - &lsg->sge[0] + 1) < lsg->num_mapped)
        {
            sge++;

            if (segp != NULL)
            {
                segp->addr = sge->dma_addr;
                segp->len  = sge->length;
            }
            return sge;
        }
    }
    return NULL;
}

#if !defined(EXTERNAL_OS) && !defined(__VMKLNX__)
/// @brief return a kernel virtual address for the byte 'offset' bytes into the given SGL.
///
/// Note that only one byte is guaranteed valid!
///
/// @param sgl     the scatter-gather list.
/// @param offset  byte offset within the SGL.
///
/// @return pointer, null on error.
void *kfio_sgl_get_byte_pointer(kfio_sg_list_t *sgl, uint32_t offset)
{
    struct linux_sgl *lsg = sgl;
    uint32_t i;
    uint32_t total_offset = 0;

    for (i = 0; i < lsg->num_entries; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];

        if (offset < total_offset + sge->length)
        {
            uint8_t *bp = kfio_sgl_get_vaddr(sge);

            bp += (offset - total_offset);
            return bp;
        }

        total_offset += sge->length;
   }

    return 0;
}
#endif // !defined(EXTERNAL_OS) && !defined(__VMKLNX__)

kfio_dma_map_t *kfio_dma_map_alloc(void)
{
    kfio_dma_map_t *dmap;

    dmap = kfio_malloc(sizeof(*dmap));
    if (dmap != NULL)
    {
        kfio_memset(dmap, 0, sizeof(*dmap));
    }
    return dmap;
}

void kfio_dma_map_free(kfio_dma_map_t *dmap)
{
    if (dmap != NULL)
    {
        kfio_free(dmap, sizeof(*dmap));
    }
}

int kfio_sgl_dma_slice(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, uint32_t offset, uint32_t length)
{
    struct linux_sgl *lsg = sgl;
    uint32_t          i;

    dmap->map_offset = offset;

    for (i = 0; i < lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];

        if (offset < sge->length)
        {
            dmap->sge_first = sge;
            dmap->sge_last  = sge;

            dmap->map_length = sge->length - offset;
            dmap->sge_skip   = offset;
            dmap->sge_count  = 1;
            break;
        }

        offset -= sge->length;
    }

    kassert(i < lsg->num_entries);
    kassert(dmap->sge_first != NULL);

    for (i = i + 1; i <= lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];

        if (dmap->map_length >= length)
        {
            dmap->sge_trunc   = dmap->map_length - length;
            dmap->map_length -= dmap->sge_trunc;
            break;
        }
        kassert(i < lsg->num_entries);
        dmap->map_length += sge->length;
        dmap->sge_last    = sge;
        dmap->sge_count++;
    }

    return 0;
}

int kfio_dma_map_nvecs(kfio_dma_map_t *dmap)
{
    return dmap->sge_count;
}

/* Physical segment enumeration interface. */
kfio_sgl_iter_t kfio_dma_map_first(kfio_dma_map_t *dmap, kfio_sgl_phys_t *segp)
{
    struct linux_sgentry *sge = dmap->sge_first;

    if (sge != NULL)
    {
        if (segp != NULL)
        {
            segp->addr = sge->dma_addr + dmap->sge_skip;
            segp->len  = sge->length - dmap->sge_skip;

            if (sge == dmap->sge_last)
                segp->len -= dmap->sge_trunc;
        }
        return sge;
    }
    return NULL;
}

kfio_sgl_iter_t kfio_dma_map_next(kfio_dma_map_t *dmap, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp)
{
    struct linux_sgentry *sge = iter;

    if (sge >= dmap->sge_first && sge < dmap->sge_last)
    {
        sge++;

        if (segp != NULL)
        {
            segp->addr = sge->dma_addr;
            segp->len  = sge->length;

            if (sge == dmap->sge_last)
                segp->len -= dmap->sge_trunc;
        }
        return sge;
    }
    return NULL;
}
