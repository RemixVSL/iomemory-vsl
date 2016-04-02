
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

#ifndef __FIO_PORT_KCONDVAR_H__
#error Please include <fio/port/kcondvar.h> rather than this file directly.
#endif
#ifndef __FIO_PORT_LINUX_KCONDVAR_H__
#define __FIO_PORT_LINUX_KCONDVAR_H__

#if !defined(__linux__)
#error This file supports only Linux
#endif

typedef fusion_spinlock_t fusion_cv_lock_t;

#define fusion_cv_lock_init(lk, n) fusion_init_spin(lk, n)
#define fusion_cv_lock_destroy(lk) fusion_destroy_spin(lk)
#define fusion_cv_lock(lk)         fusion_spin_lock(lk)
#define fusion_cv_trylock(lk)      fusion_spin_trylock(lk)
#define fusion_cv_unlock(lk)       fusion_spin_unlock(lk)
#define fusion_cv_lock_irq(lk)     fusion_spin_lock_irqsave(lk)
#define fusion_cv_trylock_irq(lk)  fusion_spin_trylock_irqsave(lk)
#define fusion_cv_unlock_irq(lk)   fusion_spin_unlock_irqrestore(lk)
#define fusion_cv_lock_is_irqsaved(lk)  fusion_spin_is_irqsaved(lk)

#endif

