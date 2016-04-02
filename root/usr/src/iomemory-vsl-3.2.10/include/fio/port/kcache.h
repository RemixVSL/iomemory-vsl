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

/** @file include/fio/port/kcache.h
 *     NO OS-SPECIFIC REFERENCES ARE TO BE IN THIS FILE
 *
 */

/*******************************************************************************
 * Supported platforms / OS:
 * x86_64 Linux, Solaris, FreeBSD
 * PPC_64 Linux
 * SPARC_64 Solaris
 * Apple OS X v 10.6.2+
 ******************************************************************************/
#ifndef __FIO_PORT_KTYPES_H__
#error Please include <fio/port/ktypes.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_KCACHE_H__
#define __FIO_PORT_KCACHE_H__

#include <fio/port/errno.h>

/**
 * @brief OS-independent structure for cache.
 * Linux - \#if KFIOC_HAS_KMEM_CACHE p -> struct kmem_cache
 *         \#else   p-> struct kmem_cache_s
 * MS Windows p -> NPAGED_LOOKASIDE_LIST
 * FreeBSD p = uma_zone_t (struct uma_zone *)
 * Solaris kmem_cache
 * OS X homegrown double-linked list of free and used elements.
 */
typedef struct
{
/* Rename "struct kmem_cache_s" -> "struct kmem_cache" linux-2.6.15
 * commit: 2109a2d1b175dfcffbfdac693bdbe4c4ab62f11f
 */
/* Remove typedef kmem_cache_t linux-2.6.23
 * commit: 698827fa9f45019df1609bb686bc51c94e127fbc
 */
    void *p;
    char name[40];
#if FUSION_DEBUG_CACHE
    fusion_atomic_t count;
#endif
    uint32_t          size;
    uint32_t          align;
    fusion_spinlock_t lock;
    int               reserved_count;
    fusion_list_t     reserved_list;
} fusion_mem_cache_t;


#define FUSION_DECLARE_CACHE(x)  extern fusion_mem_cache_t x
#if !defined(__GNUC__)
#define FUSION_DEFINE_CACHE(x)  fusion_mem_cache_t x;
#else
#define FUSION_DEFINE_CACHE(x)   fusion_mem_cache_t x = { .p = NULL }
#endif
#define FUSION_CACHE_VALID(c)    ((c).p != NULL)

extern int __kfio_create_cache(fusion_mem_cache_t *pcache, char *name, uint32_t size, uint32_t align);

/// @brief create a memory cache suitable for allocation pool of fixed size objects.
///
/// NEVER CALL THIS FUNCTION DIRECTLY! Use fusion_create_cache_* macro!
///
/// @param pcache pointer to cache structure to initialize (need not be initialized before call).
/// @param name   name of cache (for debugging purposes).
/// @param size   size of cache elements in bytes, Must be non-zero.
/// @param align  alignment of cache elements, Must be >= sizeof void * and a power of 2.
///
/// @returns zero on success,
static inline int kfio_create_cache(fusion_mem_cache_t *pcache, char *name, uint32_t size, uint32_t align)
{
    kassert(size != 0);
    kassert(align != 0);
    kassert((align % sizeof(void *)) == 0);
    kassert((align & (align - 1)) == 0);

    fusion_init_spin(&pcache->lock, "mem-cache-lock");
    pcache->reserved_count = 0;
    pcache->size = size;
    pcache->align = align;
    fusion_init_list(&pcache->reserved_list);

    return __kfio_create_cache(pcache, name, size, align);
}

/// @brief initialize the cache structure 'c' to allocate elements of type 't'
#define fusion_create_cache(c, t) kfio_create_cache(c, #t, sizeof(t), __alignof__(t))

/// @brief initialize the cache structure 'c' to allocate elements of type struct 't'
#define fusion_create_struct_cache(c, t) kfio_create_cache(c, #t, sizeof(struct t), __alignof__(struct t))

extern void *kfio_cache_alloc(fusion_mem_cache_t *cache, int can_wait);
extern void *kfio_cache_alloc_node(fusion_mem_cache_t *cache, int can_wait,
                                    kfio_numa_node_t node);
extern void kfio_cache_free(fusion_mem_cache_t *cache, void *p);
#if defined(PORT_HAS_KFIO_CACHE_FREE_NODE) && PORT_HAS_KFIO_CACHE_FREE_NODE
extern void kfio_cache_free_node(fusion_mem_cache_t *cache, void *p);
#else
#define kfio_cache_free_node kfio_cache_free
#endif
extern void kfio_cache_destroy(fusion_mem_cache_t *cache);

static inline int fusion_cache_reserve(fusion_mem_cache_t *cache, int can_wait, uint32_t n)
{
    int retval = 0;
    uint32_t i;
    fusion_list_t alloc_list;
    fusion_list_entry_t *le, *next;

    fusion_init_list(&alloc_list);

    for (i = 0; i < n; i++)
    {
        le = (fusion_list_entry_t *)kfio_cache_alloc(cache, can_wait);

        if (le == NULL)
        {
            retval = -ENOMEM;
            goto error_exit;
        }

        fusion_list_add_head(le, &alloc_list);
    }

    fusion_spin_lock(&cache->lock);

    fusion_list_add_list(&alloc_list, &cache->reserved_list);
    cache->reserved_count += n;

    fusion_spin_unlock(&cache->lock);

    return retval;

error_exit:
    fusion_list_for_each_safe(le, next, &alloc_list)
    {
        kfio_cache_free(cache, le);
    }

    return retval;
}
static inline int fusion_cache_release(fusion_mem_cache_t *cache, uint32_t n)
{
    int retval = 0;
    uint32_t i;

    fusion_spin_lock(&cache->lock);

    for (i = 0; i < n; i++)
    {
        fusion_list_entry_t *le;

        if (cache->reserved_count == 0)
        {
            retval = -ENOMEM;
            goto exit;
        }

        le = fusion_list_pop_head(&cache->reserved_list);
        cache->reserved_count--;

        kfio_cache_free(cache, le);
    }

exit:
    fusion_spin_unlock(&cache->lock);

    return retval;
}

static inline void *fusion_cache_prealloc(fusion_mem_cache_t *cache)
{
    void *retval = NULL;

    fusion_spin_lock(&cache->lock);

    if (cache->reserved_count)
    {
        retval = fusion_list_pop_head(&cache->reserved_list);
        cache->reserved_count--;
    }

    fusion_spin_unlock(&cache->lock);
    return retval;
}

static inline void *fusion_cache_alloc(fusion_mem_cache_t *cache, int can_wait)
{
    return kfio_cache_alloc(cache, can_wait);
}

static inline void fusion_cache_free(fusion_mem_cache_t *cache, void *p, int prealloc)
{
    if (prealloc)
    {
        fusion_list_entry_t *le;

        fusion_spin_lock(&cache->lock);

        le = (fusion_list_entry_t *)p;

        fusion_list_add_head(le, &cache->reserved_list);
        cache->reserved_count++;

        fusion_spin_unlock(&cache->lock);
    }
    else
    {
        kfio_cache_free(cache, p);
    }
}

static inline void fusion_cache_destroy(fusion_mem_cache_t *cache)
{
    fusion_list_entry_t *e, *n;

    fusion_list_for_each_safe(e, n, &cache->reserved_list)
    {
        fusion_list_del(e);
        kfio_cache_free(cache, e);
    }

    fusion_destroy_spin(&cache->lock);
    kfio_cache_destroy(cache);
}


#endif //__FIO_PORT_KCACHE_H__

