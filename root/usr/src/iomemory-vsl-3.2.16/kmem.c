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
// -----------------------------------------------------------------------------

#if !defined (__linux__)
#error This file supports linux only
#endif

#include "port-internal.h"
#include <fio/port/kfio_config.h>
#include <fio/port/ktypes.h>
#include <fio/port/dbgset.h>
#include <linux/vmalloc.h>
#include <linux/version.h>

/**
 * @ingroup PORT_LINUX
 * @{
 */
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
    return vmalloc(size);
}
KFIO_EXPORT_SYMBOL(kfio_vmalloc);

/**
 *
 */
void noinline kfio_vfree(void *ptr, fio_size_t sz)
{
    (void) sz;
    vfree(ptr);
}
KFIO_EXPORT_SYMBOL(kfio_vfree);

/*---------------------------------------------------------------------------*/

void noinline kfio_free_page(fusion_page_t pg)
{
    __free_page((struct page *) pg);
}

/** @brief returns the page for a virtual address
 */
fusion_page_t noinline kfio_page_from_virt(void *vaddr)
{
    kassert(!is_vmalloc_addr(vaddr));
    return (fusion_page_t)virt_to_page(vaddr);
}

/** @brief returns a virtual address given a page
 */
void * noinline kfio_page_address(fusion_page_t pg)
{
    return page_address((struct page *) pg);
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
        lflags |= (GFP_NOWAIT | __GFP_NOWARN);
/** linux-2.6.11 introduced __GFP_ZERO
 * We don't have a KFIOC_ for this because defines are trivially detectable.
 */

#if defined(__GFP_ZERO)
    page = (fusion_page_t) alloc_page(lflags | __GFP_ZERO);
#else
    page = (fusion_page_t) alloc_page(lflags);
#endif

#if !defined(__GFP_ZERO)
    if (page)
        memset(kfio_page_address(page), 0, FUSION_PAGE_SIZE);
#endif

    return page;
}

#define GET_USER_PAGES_FLAGS(write, force) (write? FOLL_WRITE : 0 | force? FOLL_FORCE : 0)

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
    retval =  get_user_pages(start, nr_pages, GET_USER_PAGES_FLAGS(write, 0), (struct page **) pages, NULL);
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
        put_page((struct page *)pages[i]);
    }
}

KFIO_EXPORT_SYMBOL(kfio_get_user_pages);
KFIO_EXPORT_SYMBOL(kfio_put_user_pages);
#endif

/**
 * @}
 */
