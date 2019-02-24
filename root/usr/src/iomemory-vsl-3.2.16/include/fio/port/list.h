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

#ifndef __FUSION_LIST_H__
#define __FUSION_LIST_H__

#include <fio/port/kglobal.h> // fio_uintptr_t
#include <fio/port/compiler.h>

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
FIO_NONNULL_PARAMS static inline void fusion_init_list(fusion_list_entry_t *e) {
    e->next = e;
    e->prev = e;
}

/// @brief initialize a list element to the unlinked state.
///
/// @param e the element to initialize.
FIO_NONNULL_PARAMS static inline void fusion_init_list_entry(fusion_list_entry_t *e) {
    e->next = e;
    e->prev = e;
}

/// @brief insert a list element between two pre-existing list elements.
///
/// @param e the element to insert.
/// @param p the preceding element.
/// @param n the succeeding element.
FIO_NONNULL_PARAMS static inline void fusion_list_add(fusion_list_entry_t *e, fusion_list_entry_t *p, fusion_list_entry_t *n) {
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
FIO_NONNULL_PARAMS static inline void fusion_list_add_head(fusion_list_entry_t *e, fusion_list_entry_t *l) {
        fusion_list_add(e, l, l->next);
    }

/// @brief add an element as the tail of a list.
///
/// It is a serious error if the element is already on the list. This will lead to list
/// corruption. Consider using fusion_list_move_tail instead.
///
/// @param e the element to add.
/// @param l the list to add to.
FIO_NONNULL_PARAMS static inline void fusion_list_add_tail(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    fusion_list_add(e, l->prev, l);
}

/// @brief unlink the given element from a list, leaving the element in an undefined state.
///
/// This function is harmless if the element is not actually on a list, but it must have been
/// initialized.
///
/// This function leaves the element in an undefined state. Use with care.
///
/// @param e the element to unlink.
FIO_NONNULL_PARAMS static inline void __fusion_list_del(fusion_list_entry_t *e) {
    if (e->next && e->prev)
    {
        e->next->prev = e->prev;
        e->prev->next = e->next;
    }
}

/// @brief unlink the given element from a list, leaving the element in the unlinked state.
///
/// This function is harmless if the element is not actually on a list, but it must have been
/// initialized.
///
/// @param e the element to unlink.
FIO_NONNULL_PARAMS static inline void fusion_list_del(fusion_list_entry_t *e) {
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
FIO_NONNULL_PARAMS static inline void fusion_list_move_head(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    __fusion_list_del(e);
    fusion_list_add_head(e, l);
}

/// @brief move the given element to the tail of the given list.
///
/// This function works correctly whether or not the element is currently on the given list.
/// The element must have been initialized.
///
/// @param e the element to place on the tail of the list.
/// @param l the list to modify.
FIO_NONNULL_PARAMS static inline void fusion_list_move_tail(fusion_list_entry_t *e, fusion_list_entry_t *l) {
    __fusion_list_del(e);
    fusion_list_add_tail(e, l);
}

/// @brief test if a given list is empty.
///
/// When called on a list element this function checks if the element is in the unlinked state.
///
/// @param e the list to test.
FIO_NONNULL_PARAMS static inline int fusion_list_empty(fusion_list_entry_t *e) { return e->next == e; }

/// @brief move the elements in src to the list in dst
///
/// Removes the elements from src and puts them in dst
///
/// @param src  The source list of elements
/// @param dst  The destination list of elements
FIO_NONNULL_PARAMS static inline void fusion_list_add_list(fusion_list_t *src, fusion_list_t *dst)
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
FIO_NONNULL_PARAMS static inline fusion_list_entry_t *fusion_list_pop_head(fusion_list_t *list)
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
FIO_NONNULL_PARAMS static inline int fusion_list_is_head(fusion_list_entry_t *e, fusion_list_t *l)
{
    return l->next == e;
}

/// @brief test if a given element is the tail of the list.
///
/// @param e  the entry to test.
/// @param l  the list
FIO_NONNULL_PARAMS static inline int fusion_list_is_tail(fusion_list_entry_t *e, fusion_list_t *l)
{
    return l->prev == e;
}

FIO_NONNULL_PARAMS static inline void __fusion_list_splice(fusion_list_t *list,
                                                           fusion_list_t *prv, fusion_list_t *nxt)
{
    fusion_list_t *first = list->next;
    fusion_list_t *last = list->prev;

    first->prev = prv;
    prv->next = first;
    last->next = nxt;
    nxt->prev = last;
}

FIO_NONNULL_PARAMS static inline void fusion_list_splice(fusion_list_t *list, fusion_list_t *head)
{
    if (!fusion_list_empty(list))
    {
        __fusion_list_splice(list, head, head->next);
        fusion_init_list(list);
    }
}

FIO_NONNULL_PARAMS static inline void fusion_list_splice_tail(fusion_list_t *list, fusion_list_t *head)
{
    if (!fusion_list_empty(list))
    {
        __fusion_list_splice(list, head->prev, head);
        fusion_init_list(list);
    }
}

#ifndef BUILDING_MODULE
#ifndef container_of

#define container_of(entry, type, field) (type*)(((fio_uintptr_t)(type*)((char *)(entry))-((fio_uintptr_t)&((type*)0)->field)))

#endif
#endif

#define fusion_list_entry(entry, type, field) container_of(entry, type, field)

#define fusion_list_for_each_entry(entry, list, field, type)      \
    for (entry = fusion_list_entry((list)->next, type, field);    \
     &entry->field != (list);                                     \
     entry = fusion_list_entry(entry->field.next, type, field))

#define fusion_list_for_each_entry_safe(entry, next_entry, list, field, type) \
    for (entry = fusion_list_entry((list)->next, type, field), next_entry = fusion_list_entry(entry->field.next, type, field);                                               \
         &entry->field != (list);                    \
     entry = next_entry, next_entry = fusion_list_entry(next_entry->field.next, type, field))

#define fusion_list_for_each(entry, list)            \
    for (entry = (list)->next;                       \
     entry != (list);                                \
     entry = entry->next)

#define fusion_list_for_each_safe(entry, next_entry, list)        \
    for (entry = (list)->next, next_entry = entry->next;          \
     entry != (list);                                             \
     entry = next_entry, next_entry = entry->next)

#define fusion_list_get_next(entry)     (entry).next

#define fusion_list_get_prev(entry)     (entry).prev

#define fusion_list_get_head(list, type, field)  (fusion_list_empty(list) ? NULL : fusion_list_entry(((list)->next), type, field))

#define fusion_list_get_tail(list, type, field)  (fusion_list_empty(list) ? NULL : fusion_list_entry(((list)->prev), type, field))

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

#define fusion_hlist_for_each_safe(pos, next, head) \
        for (pos = (head)->first; pos && __hlist_assign_next(pos, &(next)); pos = next)

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

// Common functions
#define fusion_list_get_next_entry(entry, type, field)  fusion_list_entry(fusion_list_get_next(entry), type, field)
#define fusion_list_get_prev_entry(entry, type, field)  fusion_list_entry(fusion_list_get_prev(entry), type, field)

#endif
