//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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
#define KSCATTER_IMPL
#include "port-internal.h"
#include <fio/port/dbgset.h>
#include <linux/version.h>

#ifndef MIN
#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#endif

struct linux_sgentry
{
#if defined(__VMKLNX__)
    fusion_page_t       orig_page; /* pointer to original page in case of double buffering */
    uint32_t            orig_off;  /* offset from above page */
    uint32_t            flags;
#endif
    char                *dummy; /* make the struct non-empty */
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
    struct linux_sgl     *lsg;
};

struct linux_sgl
{
    uint32_t            max_entries;
    uint32_t            num_entries;
    uint32_t            num_mapped;
    uint32_t            sgl_size;
    struct pci_dev     *pci_dev;
    int                 pci_dir;
    struct linux_sgentry *sge;
    struct scatterlist *sl;
};
#define sge_to_sl(lsg, sge) ((lsg)->sl + ((sge) - (lsg)->sge))

#if KFIOC_SGLIST_NEW_API == 0
static void sg_init_table(struct scatterlist *sgl, unsigned int nents)
{
    memset(sgl, 0, sizeof(*sgl) * nents);
}

static inline struct page *sg_page(struct scatterlist *sg)
{
    return sg->page;
}

#if !defined(__VMKLNX__)
/* ESX4 doesn't define above two APIs but this one */
static inline void sg_set_page(struct scatterlist *sg, struct page *page,
                               unsigned int len, unsigned int offset)
{
    sg->page = page;
    sg->offset = offset;
    sg->length = len;
}
#endif
#endif

int kfio_sgl_alloc_nvec(kfio_pci_dev_t *pcidev, kfio_numa_node_t node, kfio_sg_list_t **sgl, int nvecs)
{
    struct linux_sgl *lsg;

#if defined(__VMKLNX__)
    // kmalloc pool is very limited on ESX and gets depleted with more
    // than 7 or so cards.
    lsg = kfio_vmalloc(sizeof(*lsg) + nvecs * (sizeof(struct linux_sgentry) + sizeof(struct scatterlist)));
#else
    lsg = kfio_malloc_node(sizeof(*lsg) + nvecs * (sizeof(struct linux_sgentry) + sizeof(struct scatterlist)), node);
#endif
    if (NULL == lsg)
    {
        return -ENOMEM;
    }

    lsg->max_entries = nvecs;
    lsg->num_entries = 0;
    lsg->num_mapped  = 0;
    lsg->sgl_size    = 0;
    lsg->sge = (struct linux_sgentry *)(lsg + 1);
    lsg->sl = (struct scatterlist *)(lsg->sge + nvecs);
    sg_init_table(lsg->sl, nvecs);

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
                    lsg->max_entries * (sizeof(struct linux_sgentry) + sizeof(struct scatterlist)));
#else
    kfio_free(lsg, sizeof(*lsg) +
                    lsg->max_entries * (sizeof(struct linux_sgentry) + sizeof(struct scatterlist)));
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
static void *kfio_sgl_get_vaddr(struct scatterlist *sl)
{
    return page_address(sg_page(sl)) + sl->offset;
}

int kfio_sgl_map_bytes(kfio_sg_list_t *sgl, const void *buffer, uint32_t size)
{
    const uint8_t *bp = buffer;
    struct linux_sgl *lsg = sgl;
#if !defined(__VMKLNX__)
    bool vmalloc_buffer;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
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
        struct scatterlist *sl;

        sge = &lsg->sge[lsg->num_entries];
        sl = &lsg->sl[lsg->num_entries];

#if defined(__VMKLNX__)
        sge->flags     = 0;
        sge->orig_page = NULL;
        sge->orig_off  = 0;
#endif

        if (lsg->num_entries >= lsg->max_entries)
        {
            engprint("%s: too few sg entries (cnt: %d nvec: %d size: %d)\n",
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

        sg_set_page(sl, (struct page *)page, mapped_bytes, page_offset);

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
    struct scatterlist *sl;

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
    sl = &lsg->sl[lsg->num_entries];

#if defined(__VMKLNX__)
    // Special handling for unaligned buffers on ESX
    sge->flags     = 0;
    sge->orig_page = NULL;
    sge->orig_off  = offset;

    // Reject any unaligned DMA with size not multiple of 8.
    // The block layer constraint sgElemSizeMult=8 should prevent this,
    // but better be safe here.
    if (unlikely(size & 0x7))
    {
        errprint("%s: Unaligned DMA size: 0x%x page=0x%llx offset=0x%x, rejecting I/O\n",
                 __FUNCTION__, size, (uint64_t)page, offset);
        return -EINVAL;
    }

    // Check for unaligned addresses.
    // The block layer constraint sgElemAlignment=8 should prevent this,
    // but this old code is left here as a backup.
    if (unlikely(offset & 0x7))
    {
        /* Firmware can't handle nonzero offsets unless they are double-word aligned.
         * This operation will require double buffering.
         */

        errprint("Handling unaligned DMA: page=%llx offset=%x size=%x\n",
                 (uint64_t)page, offset, size);

        // We can get unaligned multi-page DMA requests in ESX, so we need to loop and create
        // more sg entries as needed.
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
            sge->flags |= SGE_BUFFERED;
            sg_set_page(sl, (struct page *)buffered_page, mapped_bytes, 0);

            size -= mapped_bytes;
            page = (fusion_page_t)((uint64_t)page + 1);

            lsg->num_entries++;
            lsg->sgl_size += mapped_bytes;
            sge = &lsg->sge[lsg->num_entries];
            sl = &lsg->sl[lsg->num_entries];
        }
    }
    else
#endif
    {
        sg_set_page(sl, (struct page *)page, size, offset);

        lsg->num_entries++;
        lsg->sgl_size += size;
    }

    return 0;
}

int kfio_sgl_map_bio(kfio_sg_list_t *sgl, struct bio *pbio)
{
    struct linux_sgl *lsg = sgl;
    struct scatterlist *sl, *old_sl;

#if KFIOC_HAS_BIOVEC_ITERATORS
    struct bio_vec vec;
    struct bvec_iter bv_i;
#define BIOV_MEMBER(vec, member) (vec.member)
#else
    struct bio_vec *vec;
    int bv_i;
#define BIOV_MEMBER(vec, member) (vec->member)
#endif

    uint32_t old_len, old_sgl_size, old_sgl_len;
    int rval = 0;

    /* Remember SGL vitals in case we need to back out. */
    if (lsg->num_entries > 0)
    {
        old_sl = &lsg->sl[lsg->num_entries - 1];
        old_len = old_sl->length;
    }
    else
    {
        old_sl = NULL;
        old_len = 0;
    }
    old_sgl_len  = lsg->num_entries;
    old_sgl_size = lsg->sgl_size;

    bio_for_each_segment(vec, pbio, bv_i)
    {
        if (lsg->num_entries > 0)
        {
            /*
             * This iovec shares page with previos one. Merge them together. The biggest
             * entry we can get this way is limited by page size.
             */
            sl = &lsg->sl[lsg->num_entries - 1];

            if (sg_page(sl) == BIOV_MEMBER(vec, bv_page) &&
                sl->offset + sl->length == BIOV_MEMBER(vec, bv_offset))
            {
                sl->length   += BIOV_MEMBER(vec, bv_len);
                lsg->sgl_size += BIOV_MEMBER(vec, bv_len);
                continue;
            }

            /* The list will grow: check for overflow here. */
            if (unlikely(lsg->num_entries >= lsg->max_entries))
            {
                rval = -EINVAL;
                break;
            }
        }

        rval = kfio_sgl_map_page(sgl,
                                 (fusion_page_t) BIOV_MEMBER(vec, bv_page),
                                 BIOV_MEMBER(vec, bv_offset),
                                 BIOV_MEMBER(vec, bv_len));
        if (rval)
        {
            dbgprint(DBGS_GENERAL, "Failed to map bio_vec\n" );
            break;
        }
    }

    if (rval)
    {
        /*
         * Roll back all of the updates made so far. Unfortunately our
         * callers expect bio mapping to be either all successful or
         * not, with no partial result left in.
         */
#if defined(__VMKLNX__)
        while (lsg->num_entries > old_sgl_len)
        {
            struct linux_sgentry *sge;
            lsg->num_entries--;
            sge = &lsg->sge[lsg->num_entries];
            sl = &lsg->sl[lsg->num_entries];
            if (sge->flags & SGE_BUFFERED)
            {
                kfio_free_page((fusion_page_t)sg_page(sl));
            }
        }
#else
        lsg->num_entries = old_sgl_len;
#endif
        lsg->sgl_size = old_sgl_size;
        if (old_sl != NULL)
        {
            old_sl->length = old_len;
        }
    }
    return rval;
}
KFIO_EXPORT_SYMBOL(kfio_sgl_map_bio);

int kfio_sgl_dma_map(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, int dir)
{
    struct linux_sgl *lsg = sgl;
    int i;

    lsg->pci_dir = dir;

    for (i = 0; i < lsg->num_entries; i++)
    {
        struct linux_sgentry *sge;
        struct scatterlist *sl;

        sge = &lsg->sge[i];
        sl = &lsg->sl[i];

#if defined(__VMKLNX__)
        /* ESX 4 doesn't do anything in pci_map_sg if dma_address isn't 0 */
        sl->dma_address = 0;
        if (sge->flags & SGE_BUFFERED && dir == IODRIVE_DMA_DIR_WRITE)
        {
            kfio_memcpy(((char *)kfio_page_address((fusion_page_t)sg_page(sl))) + sl->offset,
                        ((char *)kfio_page_address(sge->orig_page)) + sge->orig_off,
                        sl->length);
        }
#endif
    }

    i = pci_map_sg(lsg->pci_dev, lsg->sl, lsg->num_entries,
                    dir == IODRIVE_DMA_DIR_READ ? PCI_DMA_FROMDEVICE : PCI_DMA_TODEVICE);
    lsg->num_mapped = i;
    if (i < lsg->num_entries)
    {
        goto bail;
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
        dmap->lsg        = lsg;
    }
    return lsg->num_mapped;

bail:
    kfio_sgl_dma_unmap(sgl);
    return -EINVAL;
}
KFIO_EXPORT_SYMBOL(kfio_sgl_dma_map);

int kfio_sgl_dma_unmap(kfio_sg_list_t *sgl)
{
    struct linux_sgl *lsg = sgl;
    int i;

    for (i = 0; i < lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge;
        struct scatterlist *sl;

        sge = &lsg->sge[i];
        sl = &lsg->sl[i];
#if defined(__VMKLNX__)
        if (sge->flags & SGE_BUFFERED)
        {
            if (lsg->pci_dir == IODRIVE_DMA_DIR_READ)
            {
                kfio_memcpy(((char *)kfio_page_address(sge->orig_page)) + sge->orig_off,
                                ((char *)kfio_page_address((fusion_page_t)sg_page(sl))) + sl->offset, sl->length);
            }
            kfio_free_page((fusion_page_t)sg_page(sl));
        }
#endif
    }
    if (lsg->num_mapped)
    {
        pci_unmap_sg(lsg->pci_dev, lsg->sl, lsg->num_mapped,
                 lsg->pci_dir == IODRIVE_DMA_DIR_READ ? PCI_DMA_FROMDEVICE : PCI_DMA_TODEVICE);
    }
    lsg->num_mapped = 0;
    lsg->pci_dir = 0;
    return 0;
}
KFIO_EXPORT_SYMBOL(kfio_sgl_dma_unmap);

#if defined(DMA64_OS32)
#define PRIdma "llx"
#define DMA_PR_T long long
#else
#define PRIdma "p"
#define DMA_PR_T void *
#endif

void kfio_sgl_dump(kfio_sg_list_t *sgl, kfio_dma_map_t *dmap, const char *prefix, unsigned dump_contents)
{
    struct linux_sgl *lsg = sgl;
    uint32_t i, j;

    infprint("%s sglist %p num %u max %u size %u\n",
             prefix, lsg, lsg->num_entries, lsg->max_entries, lsg->sgl_size);

    if (dmap != NULL)
    {
        infprint("%s map %p offset %u size %u count %u\n",
                 prefix, dmap, dmap->map_offset, dmap->map_length, dmap->sge_count);
    }

    for (i = 0; i < lsg->num_entries; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];
        struct scatterlist *sl = &lsg->sl[i];
        const uint8_t *bp = kfio_sgl_get_vaddr(sl);

        infprint("%s        mvec %d: vaddr: %p offset: %u size: %d paddr: %" PRIdma"\n",
                 prefix, i, bp, sl->offset, sl->length, (DMA_PR_T)sg_dma_address(sl));

        if (dmap != NULL)
        {
            if (sge == dmap->sge_first)
            {
                infprint("%s        map first skip %u\n",
                         prefix, dmap->sge_skip);
            }

            if (sge == dmap->sge_last)
            {
                infprint("%s        map last trunc %u\n",
                         prefix, dmap->sge_trunc);
            }
        }

        if (dump_contents)
        {
            for (j = 0; j < sl->length; j++)
            {
                if (j % 16 == 0)
                {
                    kfio_print("\n%04d:", j);
                }
                kfio_print(" %02x", bp[j]);
            }
            kfio_print("\n");

            // Give the linux print FIFO time to catch up
            // Yes, I know this ain't pretty, but neither is
            // an unreliable print buffer
            kfio_msleep(10);
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
            segp->addr = sg_dma_address(&lsg->sl[0]);
            segp->len  = lsg->sl[0].length;
        }
        return &lsg->sge[0];
    }
    return NULL;
}

kfio_sgl_iter_t kfio_sgl_phys_next(kfio_sg_list_t *sgl, kfio_sgl_iter_t iter, kfio_sgl_phys_t *segp)
{
    struct linux_sgl *lsg = sgl;
    struct linux_sgentry *sge = iter;
    struct scatterlist *sl;

    if (lsg->num_mapped != 0)
    {
        if (sge >= &lsg->sge[0] && (sge - &lsg->sge[0] + 1) < lsg->num_mapped)
        {
            sge++;

            if (segp != NULL)
            {
                sl = sge_to_sl(lsg, sge);
                segp->addr = sg_dma_address(sl);
                segp->len  = sl->length;
            }
            return sge;
        }
    }
    return NULL;
}

#if !defined(__VMKLNX__)
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
        struct scatterlist *sl = &lsg->sl[i];

        if (offset < total_offset + sl->length)
        {
            uint8_t *bp = kfio_sgl_get_vaddr(sl);

            bp += (offset - total_offset);
            return bp;
        }

        total_offset += sl->length;
   }

    return 0;
}
#endif // !defined(__VMKLNX__)

kfio_dma_map_t *kfio_dma_map_alloc(int may_sleep, kfio_numa_node_t node)
{
    kfio_dma_map_t *dmap;

    if (may_sleep)
    {
        dmap = kfio_malloc_node(sizeof(*dmap), node);
    }
    else
    {
        dmap = kfio_malloc_atomic_node(sizeof(*dmap), node);
    }
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
    dmap->lsg = lsg;

    for (i = 0; i < lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];
        struct scatterlist *sl = &lsg->sl[i];

        if (offset < sl->length)
        {
            dmap->sge_first = sge;
            dmap->sge_last  = sge;

            dmap->map_length = sl->length - offset;
            dmap->sge_skip   = offset;
            dmap->sge_count  = 1;
            break;
        }

        offset -= sl->length;
    }

    kassert(i < lsg->num_entries);
    kassert(dmap->sge_first != NULL);

    for (i = i + 1; i <= lsg->num_mapped; i++)
    {
        struct linux_sgentry *sge = &lsg->sge[i];
        struct scatterlist *sl = &lsg->sl[i];

        if (dmap->map_length >= length)
        {
            dmap->sge_trunc   = dmap->map_length - length;
            dmap->map_length -= dmap->sge_trunc;
            break;
        }
        kassert(i < lsg->num_entries);
        dmap->map_length += sl->length;
        dmap->sge_last    = sge;
        dmap->sge_count++;
    }

    return 0;
}

int kfio_dma_map_nvecs(kfio_dma_map_t *dmap)
{
    return dmap->sge_count;
}

uint32_t kfio_dma_map_size(kfio_dma_map_t *dmap)
{
    return dmap->map_length;
}

/* Physical segment enumeration interface. */
kfio_sgl_iter_t kfio_dma_map_first(kfio_dma_map_t *dmap, kfio_sgl_phys_t *segp)
{
    struct linux_sgentry *sge = dmap->sge_first;
    struct scatterlist *sl;

    if (sge != NULL)
    {
        if (segp != NULL)
        {
            sl = sge_to_sl(dmap->lsg, sge);
            segp->addr = sg_dma_address(sl) + dmap->sge_skip;
            segp->len  = sl->length - dmap->sge_skip;

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
    struct scatterlist *sl;

    if (sge >= dmap->sge_first && sge < dmap->sge_last)
    {
        sge++;

        sl = sge_to_sl(dmap->lsg, sge);
        if (segp != NULL)
        {
            segp->addr = sg_dma_address(sl);
            segp->len  = sl->length;

            if (sge == dmap->sge_last)
                segp->len -= dmap->sge_trunc;
        }
        return sge;
    }
    return NULL;
}

#if PORT_SUPPORTS_USER_PAGES
/// @brief Map one or more pages into SGL
///
/// @param sgl         the scatter-gather list.
/// @param pages       An array of pages.
/// @param size        Total size of the buffer
/// @param offset      offset of the starting address from the page boundary
///
/// @return none
int kfio_sgl_map_user_pages(kfio_sg_list_t *sgl, fusion_user_page_t *pages,
                            uint32_t size, uint32_t offset)
{
    struct linux_sgl *lsg = sgl;
    int i = 0;


    if (unlikely(offset >= FUSION_PAGE_SIZE))
    {
        dbgprint(DBGS_GENERAL, "%s: offset %d > FUSION_PAGE_SIZE\n",
                     __func__, offset);
        return -EINVAL;
    }

    while (size)
    {
        uint32_t     mapped_bytes;
        uint32_t     page_offset, page_remainder;
        struct       scatterlist *sl;

        if (unlikely(lsg->num_entries >= lsg->max_entries))
        {
            dbgprint(DBGS_GENERAL, "%s: too few sg entries (cnt: %d nvec: %d size: %d)\n",
                     __func__, lsg->num_entries, lsg->max_entries, size);
            return -ENOMEM;
        }

        sl = &lsg->sl[lsg->num_entries];

        page_offset    = (uint32_t)((fio_uintptr_t)offset % FUSION_PAGE_SIZE);
        page_remainder = FUSION_PAGE_SIZE - page_offset;
        mapped_bytes   = page_remainder > size ? size : page_remainder;

        sg_set_page(sl, (struct page *)pages[i++], mapped_bytes, offset);
        offset = 0;

        size -= mapped_bytes;
        lsg->num_entries++;
        lsg->sgl_size += mapped_bytes;
    }
    return 0;
}
#endif

#if PORT_SUPPORTS_SGLIST_COPY
/// @brief Copy data between scatter gather lists.
///
/// @param src     the data source
/// @param dst     the destination scatter-gather list.
/// @param length  number of bytes to copy from source to destination
///                scatter-gather list.
/// Note: the entries in linux_sgl for src and dst should be updated by the
///       caller (such as num_entries, sgl->size, etc.)
/// @return 0, -EFAULT on error.
int kfio_sgl_copy_data(kfio_sg_list_t *dst, kfio_sg_list_t *src, uint32_t length)
{
    struct linux_sgl *ldst = dst;
    struct linux_sgl *lsrc = src;

    struct scatterlist   *ldst_sl = &ldst->sl[0];
    struct scatterlist   *lsrc_sl = &lsrc->sl[0];

    uint32_t ldst_sge_off = ldst_sl->offset;
    uint32_t lsrc_sge_off = lsrc_sl->offset;

    if (length > ldst->sgl_size || length > lsrc->sgl_size)
    {
        return -EFAULT;
    }

    while (length)
    {
        int sub_length = length;
        void *pdst, *psrc;

        sub_length = MIN(sub_length, ldst_sl->length - ldst_sge_off);
        sub_length = MIN(sub_length, lsrc_sl->length - lsrc_sge_off);

        preempt_disable();

#if KFIOC_KMAP_ATOMIC_NEEDS_TYPE
        pdst = kmap_atomic(sg_page(ldst_sl), KM_USER0);
        psrc = kmap_atomic(sg_page(lsrc_sl), KM_USER1);
#else
        // newer linux kernels do not require kmem_type_t argument
        pdst = kmap_atomic(sg_page(ldst_sl));
        psrc = kmap_atomic(sg_page(lsrc_sl));
#endif

        kfio_memcpy(pdst + ldst_sge_off, psrc + lsrc_sge_off, sub_length);

#if KFIOC_KMAP_ATOMIC_NEEDS_TYPE
        kunmap_atomic(psrc, KM_USER1);
        kunmap_atomic(pdst, KM_USER0);
#else
        kunmap_atomic(psrc);
        kunmap_atomic(pdst);
#endif

        preempt_enable();

        ldst_sge_off += sub_length;
        lsrc_sge_off += sub_length;

        if (ldst_sge_off == ldst_sl->length)
        {
            ldst_sl++;
            ldst_sge_off = ldst_sl->offset;
        }

        if (lsrc_sge_off == lsrc_sl->length)
        {
            lsrc_sl++;
            lsrc_sge_off = lsrc_sl->offset;
        }

        length -= sub_length;
    }

    return 0;
}
#endif
