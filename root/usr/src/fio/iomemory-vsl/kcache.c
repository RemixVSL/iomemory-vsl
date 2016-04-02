//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
