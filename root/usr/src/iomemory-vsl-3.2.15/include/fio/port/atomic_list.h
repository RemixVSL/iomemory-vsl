//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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

#ifndef __FUSION_ATOMIC_LIST_H__
#define __FUSION_ATOMIC_LIST_H__

#include <fio/port/kfio.h>

/*
 * Below is an implementation of a singly linked compare-and-exchange (or
 * load and swap) list. It is safe to add to this list without protection
 * of locks.
 */
struct fio_atomic_list
{
    struct fio_atomic_list *next;
};

#define fusion_atomic_list_init(head)   ((head)->next = NULL)

static inline int fusion_atomic_list_empty(struct fio_atomic_list *list)
{
    return list->next == NULL;
}

static inline void fusion_atomic_list_add(struct fio_atomic_list *entry,
                                          struct fio_atomic_list *head)
{
    struct fio_atomic_list *cur;

    do
    {
        cur = head->next;
        entry->next = cur;
    } while (kfio_cmpxchg(&head->next, cur, entry) != cur);
}

static inline void fusion_atomic_list_splice(struct fio_atomic_list *list,
                                             struct fio_atomic_list *head)
{
    head->next = kfio_xchg(&list->next, NULL);
}

#define fusion_atomic_list_for_each(pos, n, head) \
    for (pos = (head)->next, (n = pos ? pos->next : NULL); pos; \
        pos = n, n = pos ? pos->next : NULL)

#endif
