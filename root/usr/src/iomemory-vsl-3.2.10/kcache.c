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
#include <linux/hardirq.h>  // in_atomic()

#if KFIOC_HAS_KMEM_CACHE
#define KCACHE_PTR (struct kmem_cache *)
#else
#define KCACHE_PTR (struct kmem_cache_s *)
#endif

/**
 *
 */
int noinline __kfio_create_cache(fusion_mem_cache_t *pcache, char *name, uint32_t size, uint32_t align)
{
#if defined(__VMKLNX__)
    align = 0;
#endif
    dbgprint(DBGS_GENERAL, "Creating cache %s size: %d  align: %d\n", name, size, align);

//    kassert(pcache->name);
//    kassert(name);
    strncpy(pcache->name, name, 39);

#if KFIOC_KMEM_CACHE_CREATE_REMOVED_DTOR
    pcache->p = kmem_cache_create(pcache->name, size, align, 0, NULL);
#else
    pcache->p = kmem_cache_create(pcache->name, size, align, 0, NULL, NULL);
#endif

#if FUSION_DEBUG_CACHE
    fusion_atomic_set(&pcache->count, 0);
#endif

    return pcache->p ? 0 : -ENOMEM;
}

/**
 * @brief allocate a chunk from the given cache, possibly sleeping
 * but never doing file IO.
 *
 * This function checks if it is in interrupt context and if so will never sleep.
 * In non-interrupt context it may sleep.
 */
void *kfio_cache_alloc(fusion_mem_cache_t *cache, int can_wait)
{
    void *p;

    FUSION_ALLOCATION_TRIPWIRE_TEST();

    p = kmem_cache_alloc(KCACHE_PTR (cache->p),
                               (!can_wait || in_atomic() || irqs_disabled()) ? GFP_NOWAIT : GFP_NOIO);
#if FUSION_DEBUG_CACHE
    if (p)
        fusion_atomic_incr(&cache->count);
#endif
    return p;
}

void *kfio_cache_alloc_node(fusion_mem_cache_t *cache, int can_wait,
                            kfio_numa_node_t node)
{
    void *p;

    FUSION_ALLOCATION_TRIPWIRE_TEST();

#if KFIOC_CACHE_ALLOC_NODE_TAKES_FLAGS
    p = kmem_cache_alloc_node(KCACHE_PTR(cache->p),
                               (!can_wait || in_atomic() || irqs_disabled()) ? GFP_NOWAIT : GFP_NOIO, node);
#else
    // On ancient kernel where kmem_cache_alloc_node always uses GFP_KERNEL.
    // Since we need to pass different GFP flags and cannot using the node allocator, use the
    // generic allocator.
    p = kmem_cache_alloc(KCACHE_PTR(cache->p), (!can_wait || in_atomic() || irqs_disabled()) ? GFP_NOWAIT : GFP_NOIO);
#endif

#if FUSION_DEBUG_CACHE
    if (p)
        fusion_atomic_incr(&cache->count);
#endif
    return p;
}

/**
 *
 */
void kfio_cache_free(fusion_mem_cache_t *cache, void *p)
{
#if FUSION_DEBUG_MEMORY
    if(!kmem_ptr_validate(KCACHE_PTR (cache->p), p))
        kfail();
#endif

    kmem_cache_free(KCACHE_PTR (cache->p), p);
#if FUSION_DEBUG_CACHE
    {
        int count;

        count = fusion_atomic_decr(&cache->count);

        if (! count)
            kfio_print("cache %s count decremented to zero\n", cache->name);

        kassert(count >= 0);
    }
#endif
}

/**
 *
 */
void kfio_cache_destroy(fusion_mem_cache_t *cache)
{
#if FUSION_DEBUG_CACHE
    kfio_print("%d objects remaining in cache %s.\n",
               fusion_atomic_read(&cache->count), cache->name);
#endif
    kmem_cache_destroy(KCACHE_PTR (cache->p));

    cache->p = NULL;
}

#undef KCACHE_PTR
