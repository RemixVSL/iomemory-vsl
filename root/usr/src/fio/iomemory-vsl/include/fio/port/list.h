//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FUSION_LIST_H__
#define __FUSION_LIST_H__

#if defined(WINNT) || defined(WIN32)
#include <fio/port/windows/liststubs.h>
#else

/*****************************************************
***** fusion_list_t
*****************************************************/
struct fusion_list_entry {
    struct fusion_list_entry *next, *prev;
};

typedef struct fusion_list_entry fusion_list_entry_t; ///< an individual list element.
typedef struct fusion_list_entry fusion_list_t;       ///< a list.

/// @brief initialize a list to the empty state.
///
/// @param e the list to initialize.
static inline void fusion_init_list(fusion_list_entry_t *e) {
    e->next = e;
    e->prev = e;
}

/// @brief initialize a list element to the unlinked state.
///
/// @param e the element to initialize.
static inline void fusion_init_list_entry(fusion_list_entry_t *e) {
    e->next = e;
    e->prev = e;
}

/// @brief insert a list element between two pre-existing list elements.
///
/// @param e the element to insert.
/// @param p the preceding element.
/// @param n the succeeding element.
static inline void fusion_list_add(fusion_list_entry_t *e, fusion_list_entry_t *p, fusion_list_entry_t *n) {
    n->prev = e;
    e->next = n;
    e->prev = p;
    p->next = e;
}

/// @brief add an element as the head of a list.
///
/// This function initializes the given element. It is *not* necessary that the element
/// be previously initialized by fusion_init_list_entry.
///
/// It is a serious error if the element is already on the list. This will lead to list
/// corruption. Consider using fusion_list_move_head instead.
///
/// @param e the element to add.
/// @param l the list to add to.
static inline void fusion_list_add_head(fusion_list_entry_t *e, fusion_list_entry_t *l) {
        fusion_list_add(e,l,l->next);
    }

/// @brief add an element as the tail of a list.
///
/// This function initializes the given element. It is *not* necessary that the element
/// be previously initialized by fusion_init_list_entry.
///
/// It is a serious error if the element is already on the list. This will lead to list
/// corruption. Consider using fusion_list_move_tail instead.
///
/// @param e the element to add.
/// @param l the list to add to.
static inline void fusion_list_add_tail(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    fusion_list_add(e,l->prev,l);
}

/// @brief unlink the given element from a list, leaving the element in an undefined state.
///
/// This function is harmless if the element is not actually on a list, but it must have been
/// initialized.
///
/// This function leaves the element in an undefined state. Use with care.
///
/// @param e the element to unlink.
static inline void __fusion_list_del(fusion_list_entry_t *e) {
    e->next->prev = e->prev;
    e->prev->next = e->next;
}

/// @brief unlink the given element from a list, leaving the element in the unlinked state.
///
/// This function is harmless if the element is not actually on a list, but it must have been
/// initialized.
///
/// @param e the element to unlink.
static inline void fusion_list_del(fusion_list_entry_t *e) {
    __fusion_list_del(e);
    fusion_init_list(e);
}

/// @brief move the given element to the head of the given list.
///
/// This function works correctly whether or not the element is currently on the given list.
/// The element must have been initialized.
///
/// @param e the element to place on the head of the list.
/// @param l the list to modify.
static inline void fusion_list_move_head(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    __fusion_list_del(e);
    fusion_list_add_head(e,l);
}

/// @brief move the given element to the tail of the given list.
///
/// This function works correctly whether or not the element is currently on the given list.
/// The element must have been initialized.
///
/// @param e the element to place on the tail of the list.
/// @param l the list to modify.
static inline void fusion_list_move_tail(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    __fusion_list_del(e);
    fusion_list_add_tail(e,l);
}

/// @brief test if a given list is empty.
///
/// When called on a list element this function checks if the element is in the unlinked state.
///
/// @param e the list to test.
static inline int fusion_list_empty(fusion_list_entry_t *e) { return e->next == e; }

/// @brief move the elements in src to the list in dst
///
/// Removes the elements from src and puts them in dst
/// 
/// @param src  The source list of elements
/// @param dst  The destination list of elements
static inline void fusion_list_add_list(fusion_list_t *src, fusion_list_t *dst)
{
    fusion_list_entry_t *new_head, *old_head;

    if (src->next == src)
    {
        return;
    }

    new_head = src->next;
    old_head = dst->next;

    // Link the back link of the list from the head to the tail 
    // of the list we're adding
    old_head->prev = src->prev; 
    // And reciprocate
    src->prev->next = old_head;

    // Now make the new head the head
    new_head->prev = dst;
    dst->next = new_head;

    // And empty out the old list
    src->next = src->prev = src;
}

/// @brief pops the head element off of a list.
///
/// @param list  the list to pop.
static inline fusion_list_entry_t *fusion_list_pop_head(fusion_list_t *list)
{
    fusion_list_entry_t *retval = NULL;

    if (!fusion_list_empty(list))
    {
        retval = list->next;

        fusion_list_del(retval);
    }

    return retval;
}

/// @brief test if a given element is the head of the list.
///
/// @param e  the entry to test.
/// @param l  the list
static inline int fusion_list_is_head(fusion_list_entry_t *e, fusion_list_t *l) 
{
    return l->next == e; 
}

/// @brief test if a given element is the tail of the list.
///
/// @param e  the entry to test.
/// @param l  the list
static inline int fusion_list_is_tail(fusion_list_entry_t *e, fusion_list_t *l) 
{
    return l->prev == e; 
}


#ifndef BUILDING_MODULE
#ifndef container_of

#define container_of(e, t, f) (t*)(((fio_uintptr_t)(t*)((char *)(e))-((fio_uintptr_t)&((t*)0)->f)))

#endif
#endif

#define fusion_list_entry(e, t, f) container_of(e, t, f)

#define fusion_list_for_each_entry(e, l, f, t)              \
    for (e = fusion_list_entry((l)->next, t, f); &e->f != (l); e = fusion_list_entry(e->f.next, t, f))

#define fusion_list_for_each_entry_safe(e, n, l, f, t)                  \
    for (e = fusion_list_entry((l)->next, t, f), n = fusion_list_entry(e->f.next, t, f); \
         &e->f != (l); e = n, n = fusion_list_entry(n->f.next, t, f))

#define fusion_list_for_each(e,l)   for (e = (l)->next; e != (l); e = e->next)

#define fusion_list_for_each_safe(e,n,l)    for (e = (l)->next, n = e->next; e != (l); e = n, n = e->next)

#define fusion_list_get_next(e)     (e).next

#define fusion_list_get_prev(e)     (e).prev

#define fusion_list_get_head(x, y, z)  (fusion_list_empty(x) ? NULL : fusion_list_entry(((x)->next), y, z))

#define fusion_list_get_tail(x, y, z)  (fusion_list_empty(x) ? NULL : fusion_list_entry(((x)->prev), y, z))

/*****************************************************
 ***** fusion_hlist_t
 *****************************************************/
typedef struct fusion_hlist_t
{
    struct  fusion_hlist_entry_t*     first;
}
fusion_hlist_t;

typedef struct  fusion_hlist_entry_t
{
    struct  fusion_hlist_entry_t*     next, **pprev;
}
fusion_hlist_entry_t;

static inline void fusion_init_hlist( struct fusion_hlist_t* head )
{
    head->first = NULL;
}

static inline void fusion_init_hlist_entry( struct fusion_hlist_entry_t* node )
{
    node->next  = NULL;
    node->pprev = NULL;
}

#define fusion_hlist_entry(addr, type, field)  fusion_list_entry(addr, type, field)

#define fusion_hlist_for_each(pos, head)                       \
    for ( pos = (head)->first; pos; pos = pos->next )

static inline int __hlist_assign_next( fusion_hlist_entry_t* entry, fusion_hlist_entry_t** next )
{
    *next   = entry->next;
    return 1;
}

#define fusion_hlist_for_each_safe(pos, n, head) \
        for (pos = (head)->first; pos && __hlist_assign_next(pos,&(n)); pos = n)

static inline void fusion_hlist_add_head( struct fusion_hlist_entry_t* node, struct fusion_hlist_t* head )
{
    struct  fusion_hlist_entry_t* first   = head->first;

    node->next  = first;
    if ( first != NULL )
    {
        first->pprev    = &node->next;
    }

    head->first     = node;
    node->pprev     = &head->first;
}

static inline void fusion_hlist_del( struct fusion_hlist_entry_t* node )
{
    struct fusion_hlist_entry_t*  next    = node->next;
    struct fusion_hlist_entry_t** pprev   = node->pprev;

    *pprev  = next;

    if ( next != NULL )
    {
        next->pprev = pprev;
    }

    node->next  = (struct fusion_hlist_entry_t*)(fio_uintptr_t)0xC001C0DEUL;
    node->pprev = (struct fusion_hlist_entry_t**)(fio_uintptr_t)0xCAFEC0DEUL;
}

#endif // !WINNT

// Common functions
#define fusion_list_get_next_entry(e, t, m)  fusion_list_entry(fusion_list_get_next(e), t, m)
#define fusion_list_get_prev_entry(e, t, m)  fusion_list_entry(fusion_list_get_prev(e), t, m)

#endif
