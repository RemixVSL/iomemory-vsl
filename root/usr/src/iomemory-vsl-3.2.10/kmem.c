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

#if !defined (__linux__)
#error This file supports linux only
#endif

#include "port-internal.h"
#include <fio/port/dbgset.h>

#include <linux/version.h>
#if defined(__PPC64__) || defined(__mips64)
#include <linux/vmalloc.h>
#endif

#if defined(__VMKLNX__)
extern void *dlmalloc(size_t bytes);
extern void dlfree(void *mem);

static vmk_MemPool g_vmklnx_mem_pool = NULL;
#if defined(VMK_TRACK_MEMPOOL_STAT)
static uint64_t g_num_allocs=0, g_num_frees=0, g_max_outstanding=0;
#endif
static fusion_spinlock_t g_memory_lock; /* lock that guard the globals defined below */
static int64_t g_pages_reserved = 0;
static int64_t g_pages_used = 0;

// 105k pages reserved per 80G ==> 410MB
#define PAGES_PER_80G  105000

#define GROW_PAGES     105000
#define FIO_MEMORY_PAGE_RESERVE 512  /* from src/kernel/fio-obj/memory.c */

/**
 * ESX 4.0 (and ESXi 4.1, 5.0) imposes a hard limit on the heap size of 500 MB
 *  which limits the number (not the capacity which uses the page allocator) of
 *  cards we can support to empirically 5.  The nuber of physical pages we can
 *  allocate is not limited (expcept by installed memory) but the number of pages
 *  we can pass top vmap(..) is limited to 63K or ESX will return a failure.
 *  I have successfully allocated and vmapped 10 ranges each of 63 K pages on ESX
 *  4.0 with 24 GB installed RAM.  Based on the kernel; messages and the virtual
 *  address ranges passed back this may be a hard limit in ESX 4.0.
 * The following manages these vmapped page ranges and provides them to a
 *  version of Dog Lea's malloc.  A call to kfio_vmalloc(..) will acquire the
 *  global allocator lock and pass the call to dl_malloc(..) which will call
 *  ifio_more_core(..) when it needs more memory.
 * Since any call to ifio_more_core may result in additional allocation of pages
 *  and vmapping them, contiguous memory is not guaranteed.
 * Calculations indicate this will allow ~2520 MB of heap which does not include
 *  overhead from the allocator.
 *
 */
/** Replacement vmalloc for ESX **/
#define MAX_SIZE_T           (~(size_t)0)
#define MFAIL                ((void*)(MAX_SIZE_T))

/* 64K pages do not succeed a vmap() in ESX 4.0, but 63K pages do */
#define VM_PAGES_PER_HEAP (63*1024)

static vmk_MemPool g_vmem_pool = NULL;
static fusion_mutex_t g_vmemory_mtx;
static int64_t g_vm_pages_reserved = 0;
typedef struct ifio_vmem_struct
{
    struct ifio_vmem_struct *prev;
    char                    *base_ptr;
    char                    *cur_ptr;
    uint64_t                length;

} ifio_vmem_node_t;
ifio_vmem_node_t *p_tail = NULL;

static vmk_VirtAddr vmklnx_page_address(vmk_MachPage page)
{
    vmk_MachAddr     maddr;
    vmk_VirtAddr     vaddr;
    VMK_ReturnStatus retval;
    if (page == VMK_INVALID_MACH_PAGE)
        return (vmk_VirtAddr)NULL;
    maddr  = vmk_MachPageToMachAddr(page);
    retval = vmk_MachAddrToVirtAddr(maddr, &vaddr);
    kassert(retval == VMK_OK);
    return vaddr;
}

static vmk_MachPage vmklnx_virt_to_page(vmk_VirtAddr vaddr)
{
    vmk_MachAddr     maddr;
    vmk_MachPage     mpage;
    VMK_ReturnStatus retval;
    if (vaddr == (vmk_VirtAddr)NULL)
        return (vmk_MachPage)NULL;
    retval = vmk_VirtAddrToMachAddr(vaddr, &maddr);
    kassert(retval == VMK_OK);
    mpage  = vmk_MachAddrToMachPage(maddr);
    return mpage;
}

/**
 * Create or resize the page pool.  This does not allocate any pages.
 */
static int ifio_expand_pool()
{
    vmk_MemPoolProps create_props;
    vmk_MemPoolProps mod_props;
    int rc;

    if (!g_vmem_pool) /* needs to be initialized */
    {
        create_props.reservation = VM_PAGES_PER_HEAP;
        create_props.limit = VM_PAGES_PER_HEAP;

        rc = vmk_MemPoolCreate("fioVM", &create_props, &g_vmem_pool);
        if (rc != VMK_OK)
        {
            errprint("Error %d in create VM pool\n", rc);
            return -1;
        }
        g_vm_pages_reserved = VM_PAGES_PER_HEAP;
        return 0;
    }
    /* expand existing mem pool */
    g_vm_pages_reserved += VM_PAGES_PER_HEAP;

    mod_props.reservation = g_vm_pages_reserved;
    mod_props.limit = g_vm_pages_reserved;
    rc = vmk_MemPoolSetProps(g_vmem_pool, &mod_props);
    if (rc != VMK_OK)
    {
        g_vm_pages_reserved -= VM_PAGES_PER_HEAP;
        errprint("Error %d in mod mem pool size.\n", rc);
        return -1;
    }
    return 0;
}

/**
 * Acquires VM_PAGES_PER_HEAP or fewer pages and vmaps into a returned pointer.
 * @return pointer to vmapped memory on success else NULL.
 */
static ifio_vmem_node_t *ifio_new_heap()
{
    ifio_vmem_node_t *pn;
    vmk_MachPageRange *prange;
    vmk_MemPoolAllocProps alloc_props;
    vmk_MachPage page;
    int j;
    int rc;
    void *vaddr = NULL;

    rc = ifio_expand_pool();
    if (rc)
    {
        errprint("Error %d in expand pool.\n", rc);
        return NULL;
    }

    pn = vmalloc(sizeof(*pn));
    if (NULL == pn)
    {
        return NULL;
    }
    prange = vmalloc(sizeof (*prange) * VM_PAGES_PER_HEAP);
    if (NULL == prange)
    {
        vfree(pn);
        return NULL;
    }

    alloc_props.alignment = 0;
    alloc_props.maxPage = VMK_MEMPOOL_MAXPAGE_ANY;

    for (j=0; j<VM_PAGES_PER_HEAP; j++)
    {
        rc = vmk_MemPoolAlloc(g_vmem_pool, &alloc_props, 1, VMK_TRUE, &page);
        if (rc != VMK_OK)
        {
            break;
        }
        prange[j].numPages = 1;
        prange[j].startPage = page;
    }

    if (j == 0)
    {
        errprint("Could not allocate pages for new vm heap.\n");
        vfree(prange);
        vfree(pn);
        return NULL;
    }

    rc = vmk_MapVA(prange, (vmk_uint32) j, (vmk_VirtAddr *) &vaddr);
    if (rc != VMK_OK)
    {
        errprint("Error %d from mapVA()\n", rc);
        for (j--; j >= 0; j--)
        {
            vmk_MemPoolFree(&prange[j].startPage);
        }
        vfree(prange);
        vfree(pn);
        return NULL;
    }

    vfree(prange);

    pn->base_ptr = vaddr;
    pn->cur_ptr = vaddr;
    pn->length = PAGE_SIZE * j;
    pn->prev = NULL;

    infprint("Successfully created new heap of %u MB.\n",
        (unsigned) ((PAGE_SIZE * j) / (1024 * 1024)) );

    return pn;
}

/**
 * Cleans up mutex, unmaps vmem in list and frees page pool.
 */
void ifio_cleanup_vmalloc()
{

    fusion_mutex_lock(&g_vmemory_mtx);

    for (; p_tail;)
    {
        struct ifio_vmem_struct *p;
        char *vaddr;

        for(vaddr = p_tail->base_ptr;
            vaddr < p_tail->base_ptr + p_tail->length;
            vaddr += PAGE_SIZE)
        {
            vmk_MachPage page = vmklnx_virt_to_page((vmk_VirtAddr) vaddr);
            vmk_MemPoolFree(&page);
        }

        vmk_UnmapVA((vmk_VirtAddr) p_tail->base_ptr);
        p = p_tail->prev;
        vfree(p_tail);
        p_tail = p;
    }

    if (g_vmem_pool != NULL)
    {
        vmk_MemPoolDestroy(g_vmem_pool);
    }
    fusion_mutex_unlock(&g_vmemory_mtx);
    fusion_mutex_destroy(&g_vmemory_mtx);
}

/**
 * This must be called serially
 */
int ifio_init_vmalloc()
{
    if (g_vmem_pool) /* already initialized */
    {
        return 0;
    }
    fusion_mutex_init(&g_vmemory_mtx, "vmem_mtx");
    fusion_mutex_lock(&g_vmemory_mtx);

    p_tail = ifio_new_heap();
    if (NULL == p_tail)
    {
        errprint("Unable to create VM pool.\n");
        fusion_mutex_unlock(&g_vmemory_mtx);
        ifio_cleanup_vmalloc();
        return -1;
    }

    fusion_mutex_unlock(&g_vmemory_mtx);
    return 0;
}

/**
 *  This is the memory allocator function called by dlmalloc(), and dlfree().
 *  @return MFAIL on failure else returns ptr to start of memory of the
 *   specified length.
 */
void *ifio_more_core(int sz)
{
    void *p;

    if (p_tail == NULL)
    {
        return MFAIL;
    }
    if (sz == 0)
    {
        return p_tail->cur_ptr;
    }
    if (sz < 0)
    {
        errprint("Cannot handle trim to morecore.\n");
        return MFAIL;
    }

    if (p_tail->cur_ptr + sz > p_tail->base_ptr + p_tail->length)
    {
        ifio_vmem_node_t *pn;

        pn = ifio_new_heap();
        if (NULL == pn)
        {
            return MFAIL;
        }
        pn->prev = p_tail;
        p_tail = pn;

        if (p_tail->cur_ptr + sz > p_tail->base_ptr + p_tail->length)
        {
            return MFAIL;
        }
    }
    p = p_tail->cur_ptr;
    p_tail->cur_ptr += sz;
    return p;
}
#undef VM_PAGES_PER_HEAP
/** End of ESX ivirtual memory allocator **/

/** Page allocator and initializations functions for ESX **/
/**
 * @brief Initialise memory spin lock and create initial memory pool sized for
 *  80 GB drive (the smallest reasonable value).
 */
int ifio_init_memory()
{
    vmk_MemPoolProps create_props;
    uint32_t pages_to_reserve;
    int rc;

    rc = ifio_init_vmalloc();
    if (rc)
    {
        errprint("Failure %d from init_vmalloc().\n", rc);
        return -1;
    }

    fusion_init_spin(&g_memory_lock, "vm_lck");
    fusion_spin_lock(&g_memory_lock);

    /* Create the initial mempool, reserving the appropriate number of pages
     * based on the information we have now.
     */
    pages_to_reserve = PAGES_PER_80G;
#if defined(VMK_TRACK_MEMPOOL_STAT)
    infprint("Creating pool with %u reserved\n", pages_to_reserve);
#endif
    create_props.reservation = pages_to_reserve;
    create_props.limit       = pages_to_reserve;
    rc = vmk_MemPoolCreate("fio", &create_props, &g_vmklnx_mem_pool);
    if (rc != VMK_OK)
    {
        fusion_spin_unlock(&g_memory_lock);
        fusion_destroy_spin(&g_memory_lock);
        return rc; /* cannot continue => return error */
    }
    g_pages_reserved = pages_to_reserve;
    fusion_spin_unlock(&g_memory_lock);

    return 0;
}

void ifio_cleanup_memory()
{
    fusion_spin_lock(&g_memory_lock);
    if (g_pages_reserved > 0)
    {
#if defined(VMK_TRACK_MEMPOOL_STAT)
        infprint("g_num_allocs=%llu g_num_frees=%llu g_max_outstanding=%llu, g_pages_used %llu\n",
                 g_num_allocs, g_num_frees, g_max_outstanding, g_pages_used);
#endif
        vmk_MemPoolDestroy(g_vmklnx_mem_pool);
    }
    fusion_spin_unlock(&g_memory_lock);
    fusion_destroy_spin(&g_memory_lock);
    ifio_cleanup_vmalloc();
}

/**
 * @brief Function to allocate a page for VMWare.  We allocate from a reserved pool
 *  and have seen problems when we attempt to allocate more pages than reserved
 *  (FH-8446 implies allocating two times the reserved pool size causes a problems).
 *  We check the # of outstadning allocated pages and increase the reserved pool size
 *  if necessary.
 *  Due to the fact that fio_allocate_page(..) insists a reserve number of pages
 *  of FIO_MEMORY_PAGE_RESERVE, we oversize our pool by this amount to avoid
 *  having to resize the pool when fio_allocate_page(..) calls us in the spinlocked
 *  loop to re-fill the reserve.
 */
static vmk_MachPage vmklnx_alloc_page(gfp_t flags)
{
    vmk_MemPoolAllocProps alloc_props;
    vmk_MachPage          page     = VMK_INVALID_MACH_PAGE;
    VMK_ReturnStatus      retval;
    vmk_Bool              can_wait = VMK_TRUE;
    size_t                size     = FUSION_PAGE_SIZE;
#if defined(VMK_TRACK_MEMPOOL_STAT)
    uint64_t outstanding;
#endif
    uint32_t pages_to_reserve;

    if (flags & (GFP_ATOMIC | GFP_NOWAIT))
    {
        can_wait = VMK_FALSE;
    }

    fusion_spin_lock(&g_memory_lock);

    /* If the allocation is more urgent (can_wait != VMK_TRUE), then we can
       replenish our reserve page pool sooner, else we wait until things
       become more critical
     */
    if (((can_wait == VMK_TRUE) &&
        (g_pages_used + FIO_MEMORY_PAGE_RESERVE >= g_pages_reserved)) ||
        (g_pages_used + (FIO_MEMORY_PAGE_RESERVE >> 1) >= g_pages_reserved))
    {
    /* A subsequent call has indicated that we need to reserve more pages than
     * were previously thought.  Modify the reservation amount, and update
     * pages_reserved to reflect it.  This takes the g_memory_lock so is MP safe.
     */
        vmk_MemPoolProps modify_props;

        pages_to_reserve = g_pages_reserved + GROW_PAGES;
#if defined(VMK_TRACK_MEMPOOL_STAT)
        infprint("modifying pool to %u reserved; %s wait.\n",
            pages_to_reserve, (can_wait == VMK_FALSE)?"cannot":"can");
#endif
        modify_props.reservation = pages_to_reserve;
        modify_props.limit       = pages_to_reserve;
        retval = vmk_MemPoolSetProps(g_vmklnx_mem_pool, &modify_props);

        if (retval != VMK_OK) /* we will still attempt to get a page */
        {
            errprint("Error 0x%x, tried to increase mem pool to %u, g_pages_used=%lld\n",
                 (unsigned) retval, pages_to_reserve, g_pages_used);
        }
        else
        {
            g_pages_reserved = pages_to_reserve;
        }
    }

    alloc_props.alignment = 0;   // num pages allocation will be aligned on??
    alloc_props.maxPage   = VMK_MEMPOOL_MAXPAGE_ANY; // memory can come from anywhere, not limited by 4G boundary

    retval = vmk_MemPoolAlloc(g_vmklnx_mem_pool, &alloc_props, size / PAGE_SIZE,
                              can_wait, &page);
    if (unlikely(retval != VMK_OK))
    {
        errprint("out of pages! g_pages_reserved=%lld g_pages_used=%lld\n",
                 g_pages_reserved, g_pages_used);
        fusion_spin_unlock(&g_memory_lock);
        if (unlikely(retval != VMK_MEM_ADMIT_FAILED))
        {
            errprint("Unexpected vmk_MemPoolAlloc() return code 0x%x\n", (unsigned) retval);
        }
        return VMK_INVALID_MACH_PAGE;
    }
    g_pages_used++;

#if defined(VMK_TRACK_MEMPOOL_STAT)
    g_num_allocs++;
    outstanding = g_num_allocs - g_num_frees;
    if (outstanding > g_max_outstanding)
    {
        g_max_outstanding = outstanding;
        if (!(g_max_outstanding % 100))
        {
            infprint("g_num_allocs=%llu g_num_frees=%llu g_max_outstanding=%llu, g_pages_used %llu\n",
                     g_num_allocs, g_num_frees, g_max_outstanding, g_pages_used);
        }
    }
#endif
    fusion_spin_unlock(&g_memory_lock);
    return page;
}

static void vmklnx_free_page(vmk_MachPage page)
{
#if FUSION_DEBUG
    VMK_ReturnStatus retval =
#endif
        vmk_MemPoolFree(&page);
    fusion_spin_lock(&g_memory_lock);
    g_pages_used--;
#if FUSION_DEBUG
    kassert(retval == VMK_OK);
#endif
#if defined(VMK_TRACK_MEMPOOL_STAT)
    g_num_frees++;
#endif
    fusion_spin_unlock(&g_memory_lock);
}
#endif /* #if defined(__VMKLNX__) */

/**
 *  @brief allocates locked down memory suitable for DMA transfers.
 *  @param pdev - pointer to device handle
 *      Solaris  (struct _kfio_solaris_pci_dev *)
 *  @param size - size of allocation
 *  @param dma_handle - member .phys_addr of struct will be set to the
 *  physical addr on successful return.  The remainder of the structure is opaque.
 *  @return virtual address of allocated memory or NULL on failure.
 *
 *  The current io-drive has 64 bit restrictions on alignment and buffer length,
 *  and no restrictions on address ranges with DMA.
 */
void noinline *kfio_dma_alloc_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                       struct fusion_dma_t *dma_handle)
{
    FUSION_ALLOCATION_TRIPWIRE_TEST();

    return dma_alloc_coherent(&((struct pci_dev *)pdev)->dev, size,
                              (dma_addr_t *) &dma_handle->phys_addr, GFP_KERNEL);
}

/**
 * @brief frees memory allocated by kfio_dma_alloc_coherent().
 *
 */
void noinline kfio_dma_free_coherent(kfio_pci_dev_t *pdev, unsigned int size,
                                     void *vaddr, struct fusion_dma_t *dma_handle)
{
    dma_free_coherent(&((struct pci_dev *)pdev)->dev, size, vaddr,
                      (dma_addr_t) dma_handle->phys_addr);
}

/**
 * @brief allocates physically contiguous memory.
 * No i/o is allowed to fulfill request, but the allocation may block.
 *
 * Some OSes want the size requested to be passed into the fuction to free
 * the memory.  A quick solution is to allocate (wordsize) more bytes than
 * requested, place the size in the first four bytes, and return the pointer
 * to (ptr_from_alloc + wordsize).
 */
static void *__kfio_malloc(fio_size_t size, gfp_t gfp, kfio_numa_node_t node)
{
    // This has been determined to be a place to put this injector agent, but there are compile problems.
    // Apparently fio-port.ko cannot find the error_injection_agents.o file.
#if 0   //ENABLE_ERROR_INJECTION
    if(inject_hard_alloc_error())
        return NULL;
#endif

    FUSION_ALLOCATION_TRIPWIRE_TEST();

    return kmalloc(size, gfp);
}

/**
 *
 */
void *kfio_malloc(fio_size_t size)
{
    return __kfio_malloc(size, GFP_NOIO, -1);
}
KFIO_EXPORT_SYMBOL(kfio_malloc);

/**
 *
 */
void *kfio_malloc_node(fio_size_t size, kfio_numa_node_t node)
{
    return __kfio_malloc(size, GFP_NOIO, node);
}

/**
 *
 */
void *kfio_malloc_atomic(fio_size_t size)
{
    return __kfio_malloc(size, GFP_NOWAIT, -1);
}
KFIO_EXPORT_SYMBOL(kfio_malloc_atomic);

/**
 *
 */
void *kfio_malloc_atomic_node(fio_size_t size, kfio_numa_node_t node)
{
    return __kfio_malloc(size, GFP_NOWAIT, node);
}

/**
 *
 */
void noinline kfio_free(void *ptr, fio_size_t size)
{
    (void) size;

    kfree(ptr);
}
KFIO_EXPORT_SYMBOL(kfio_free);

/**
 * @brief allocates virtual memory mapped into kernel space that is
 * not assumed to be contiguous.
 */
void *noinline kfio_vmalloc(fio_size_t size)
{
    FUSION_ALLOCATION_TRIPWIRE_TEST();

#if defined(__VMKLNX__)
    {
        void *p;

        fusion_mutex_lock(&g_vmemory_mtx);
        p = dlmalloc(size);
        fusion_mutex_unlock(&g_vmemory_mtx);
        return p;
    }
#else

    return vmalloc(size);
#endif
}
KFIO_EXPORT_SYMBOL(kfio_vmalloc);

/**
 *
 */
void noinline kfio_vfree(void *ptr, fio_size_t sz)
{
    (void) sz;
#if defined(__VMKLNX__)
    fusion_mutex_lock(&g_vmemory_mtx);
    dlfree(ptr);
    fusion_mutex_unlock(&g_vmemory_mtx);
#else
    vfree(ptr);
#endif
}
KFIO_EXPORT_SYMBOL(kfio_vfree);

/*---------------------------------------------------------------------------*/

void noinline kfio_free_page(fusion_page_t pg)
{
#if !defined(__VMKLNX__)
    __free_page((struct page *) pg);
#else
    vmklnx_free_page((vmk_MachPage)(uint64_t)pg);
#endif
}

/** @brief returns the page for a virtual address
 */
fusion_page_t noinline kfio_page_from_virt(void *vaddr)
{
#if defined(__VMKLNX__)
    return (fusion_page_t)(fio_uintptr_t)vmklnx_virt_to_page((vmk_VirtAddr)vaddr);
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
    kassert(!is_vmalloc_addr(vaddr));
#endif
    return (fusion_page_t)virt_to_page(vaddr);
#endif
}

/** @brief returns a virtual address given a page
 */
void * noinline kfio_page_address(fusion_page_t pg)
{
#if defined(__VMKLNX__)
    return (void *)vmklnx_page_address((vmk_MachPage)(uint64_t)pg);
#else
    return page_address((struct page *) pg);
#endif
}

/**
 * The flags passed in are @e not compatible with the linux flags.
 */
fusion_page_t noinline kfio_alloc_0_page(kfio_maa_t flags)
{
    gfp_t lflags = 0;
    fusion_page_t page = NULL;

    if (flags & KFIO_MAA_NORMAL)
        lflags |= GFP_KERNEL;
    if (flags & KFIO_MAA_NOIO)
        lflags |= GFP_NOIO;
    if (flags & KFIO_MAA_NOWAIT)
        lflags |= GFP_NOWAIT;
/** linux-2.6.11 introduced __GFP_ZERO
 * We don't have a KFIOC_ for this because defines are trivially detectable.
 */

#if defined(__VMKLNX__)
    page = (fusion_page_t)(uint64_t)vmklnx_alloc_page(lflags);
    if ((uint64_t)page == VMK_INVALID_MACH_PAGE)
        page = NULL;
#elif defined(__GFP_ZERO)
    page = (fusion_page_t) alloc_page(lflags | __GFP_ZERO);
#else
    page = (fusion_page_t) alloc_page(lflags);
#endif

#if !defined(__GFP_ZERO) || defined(__VMKLNX__)
    if (page)
        memset(kfio_page_address(page), 0, FUSION_PAGE_SIZE);
#endif

    return page;
}

#if PORT_SUPPORTS_USER_PAGES
/// @brief Pin the user pages in memory.
/// Note:  This needs to be called from within a process
///        context (i.e. ioctl or other syscall) for the user address passed in
/// @param pages     array that receives pointers to the pages pinned. Should be at least nr_pages long
/// @param nr_pages  number of pages from start to pin
/// @param start     starting userspace address
/// @param write     whether pages will be written to by the caller
///
/// @return          number of pages pinned
int kfio_get_user_pages(fusion_user_page_t *pages, int nr_pages, fio_uintptr_t start, int write)
{
    int retval;

    down_read(&current->mm->mmap_sem);
    retval =  get_user_pages(current, current->mm, start, nr_pages, write, 0, (struct page **) pages, NULL);
    up_read(&current->mm->mmap_sem);
    return retval;
}

/// @brief Release the mapped pages from the page cache
/// Note:  This needs to be called from within the same process
///        context (i.e. ioctl or other syscall) kfio_get_user_pages
///        is called.
/// @param pages     the array of pages to be released
/// @param nr_pages  number of pages to be released
///
/// @return          none
void kfio_put_user_pages(fusion_user_page_t *pages, int nr_pages)
{
    int i;
    for (i = 0; i < nr_pages; i++)
    {
        if (!pages[i])
        {
            break;
        }
        page_cache_release((struct page *)pages[i]);
    }
}

KFIO_EXPORT_SYMBOL(kfio_get_user_pages);
KFIO_EXPORT_SYMBOL(kfio_put_user_pages);
#endif

/*---------------------------------------------------------------------------*/

int kfio_dma_sync(struct fusion_dma_t *dma_hdl, uint64_t offset, size_t length, unsigned type)
{
    return 0;
}
