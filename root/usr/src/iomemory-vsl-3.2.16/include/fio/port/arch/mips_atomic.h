
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

#ifndef _FIO_PORT_ARCH_MIPS_ATOMIC_H
#define _FIO_PORT_ARCH_MIPS_ATOMIC_H

// Use the sync builtins.  If they're unavailable then we'll need
// to use mips assembly, see x86_atomic.h for more info

#define fusion_atomic_set(x,y)         ((x)->value = (y))
#define fusion_atomic_read(x)          (x)->value

int kfio_atomic_sub(int val, fusion_atomic_t *p);
int kfio_atomic_add(int val, fusion_atomic_t *p);

#define fusion_atomic_add(y,x)                  \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_add(y, x);              \
    } while(0);

#define fusion_atomic_inc(x)                    \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_add(1, x);              \
    } while(0);

#define fusion_atomic_incr(x) kfio_atomic_add(1, x)

#define fusion_atomic_add_return(y,x)           \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_add(y, x);              \
    } while(0);

#define fusion_atomic_dec(x)                    \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_sub(1, x);              \
    } while(0);

#define fusion_atomic_decr(x) kfio_atomic_sub(1, x)

#define fusion_atomic_sub(y, x)                 \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_sub(y, x);              \
    } while(0);

#define fusion_atomic_sub_return(y, x)          \
    do {                                        \
        fusion_atomic_t z;                      \
        z = kfio_atomic_sub(y, x);              \
    } while(0);

/*
  Taken from http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html

  This builtin (__sync_lock_test_and_set), as described by Intel, is not a
  traditional test-and-set operation, but rather an atomic exchange operation.
  It writes value into *ptr, and returns the previous contents of *ptr.

  Many targets have only minimal support for such locks, and do not support a full
  exchange operation. In this case, a target may support reduced functionality here
  by which the only valid value to store is the immediate constant 1. The exact value
  actually stored in *ptr is implementation defined.

  This builtin is not a full barrier, but rather an acquire barrier. This means that
  references after the builtin cannot move to (or be speculated to) before the builtin,
  but previous memory stores may not be globally visible yet, and previous memory loads
  may not yet be satisfied.
*/
#define fusion_atomic_exchange(ptr, val)    \
    __sync_lock_test_and_set(&(ptr)->value, val)

#endif /* _FIO_PORT_ARCH_MIPS_ATOMIC_H */
