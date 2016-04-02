
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_ARCH_PPC_ATOMIC_H
#define _FIO_PORT_ARCH_PPC_ATOMIC_H

// Use the sync builtins.  If they're unavailable then we'll need
// to use ppc assembly, see x86_atomic.h for more info

#define fusion_atomic_set(x,y)         *(x) = (y)
#define fusion_atomic_read(x)          *(x)

#define fusion_atomic_add(y,x)                  \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_add_and_fetch(x, y);         \
    } while(0);

#define fusion_atomic_inc(x)                    \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_add_and_fetch(x, 1);         \
    } while(0);

#define fusion_atomic_incr(x) __sync_add_and_fetch(x, 1)

#define fusion_atomic_add_return(y,x)           \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_add_and_fetch(x, y);         \
    } while(0);

#define fusion_atomic_dec(x)                    \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_sub_and_fetch(x, 1);         \
    } while(0);

#define fusion_atomic_decr(x) __sync_sub_and_fetch(x, 1)

#define fusion_atomic_sub(y, x)                 \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_sub_and_fetch(x, y);         \
    } while(0);

#define fusion_atomic_sub_return(y, x)          \
    do {                                        \
        fusion_atomic_t z;                      \
        z = __sync_sub_and_fetch(x, y);         \
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
#define fusion_atomic_exchange(ptr, val)   __sync_lock_test_and_set(ptr, val)

#endif /* _FIO_PORT_ARCH_PPC_ATOMIC_H */
