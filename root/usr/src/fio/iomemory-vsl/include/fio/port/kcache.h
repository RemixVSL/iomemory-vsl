//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
 * Linux - #if KFIOC_HAS_KMEM_CACHE p -> struct kmem_cache 
 *      #else   p-> struct kmem_cache_s
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

/** 
 * @brief The trunk uses fusion_cache*, and the porting branch uses
 *  kfio_cache* for the same function.  Until the porting branch is
 *  merged into the trucnk and we can re-prefix the function names 
 *  as wanted, we'll stick to the old names.
 */

static inline int kfio_create_cache(fusion_mem_cache_t *pcache, char *name, uint32_t size, uint32_t align)
{
    fusion_init_spin(&pcache->lock, "mem-cache-lock");
    pcache->reserved_count = 0;
    fusion_init_list(&pcache->reserved_list);

    return __kfio_create_cache(pcache, name, size, align);
}

#define fusion_create_cache(c, t)           kfio_create_cache(c, #t, sizeof(t), __alignof__(t))
#define fusion_create_struct_cache(c, t)    kfio_create_cache(c, #t, sizeof(struct t), __alignof__(struct t))
#define fusion_create_struct_cache_unique(c, t, n)    kfio_create_cache(c, n, sizeof(struct t), __alignof__(struct t))


extern void *kfio_cache_alloc(fusion_mem_cache_t *cache, int can_wait);
extern void kfio_cache_free(fusion_mem_cache_t *cache, void *p);
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

    kfio_cache_destroy(cache);
}


#endif //__FIO_PORT_KCACHE_H__

