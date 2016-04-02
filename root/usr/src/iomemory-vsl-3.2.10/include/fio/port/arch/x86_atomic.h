
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
#ifndef _FIO_PORT_ARCH_X86_ATOMIC_H
#define _FIO_PORT_ARCH_X86_ATOMIC_H

typedef struct fusion_atomic {
    volatile int value;
} fusion_atomic_t;

// Use the sync builtins if our gcc version ( >= 4.0 ) supports it.  If not
// the following functions will be used instead.
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2

static inline void fusion_atomic_set(fusion_atomic_t *atomp, int32_t val)
{
    atomp->value = val;
}

static inline int32_t fusion_atomic_read(fusion_atomic_t *atomp)
{
    return atomp->value;
}

static inline void fusion_atomic_add(int32_t val, fusion_atomic_t *atomp)
{
    (void)__sync_add_and_fetch(&atomp->value, val);
}

static inline void fusion_atomic_inc(fusion_atomic_t *atomp)
{
    (void)__sync_add_and_fetch(&atomp->value, 1);
}

static inline int32_t fusion_atomic_incr(fusion_atomic_t *atomp)
{
    return __sync_add_and_fetch(&atomp->value, 1);
}

static inline int32_t fusion_atomic_add_return(int32_t val, fusion_atomic_t *atomp)
{
    return __sync_add_and_fetch(&atomp->value, val);
}

static inline void fusion_atomic_dec(fusion_atomic_t *atomp)
{
    (void)__sync_sub_and_fetch(&atomp->value, 1);
}

static inline int32_t fusion_atomic_decr(fusion_atomic_t *atomp)
{
    return __sync_sub_and_fetch(&atomp->value, 1);
}

static inline void fusion_atomic_sub(int32_t val, fusion_atomic_t *atomp)
{
    (void)__sync_sub_and_fetch(&atomp->value, val);
}

static inline int32_t fusion_atomic_sub_return(int32_t val, fusion_atomic_t *atomp)
{
    return __sync_sub_and_fetch(&atomp->value, val);
}


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
static inline int32_t fusion_atomic_exchange(fusion_atomic_t *atomp, int32_t val)
{
    return __sync_lock_test_and_set(&atomp->value, val);
}

#else
// Older gcc compiler here.  Hopefully the following ugly assembly code will someday disappear.
// ___ This is INTEL/AMD specific and requires 32-bit integers ONLY ___

/*

Since I'm very likely to forget how this inline assembly syntax is formatted, heres
a quick explanation for those who are unfortunate enough to have to look at it.
Inline assembly is not typical assembly syntax because most of this stuff gives the compiler a
idea of what variables change, which ones to use where, and how to protect them.
I don't use real registers in my inline so the compiler can better optimize their
use.

__asm__ __volatile__ ( "<instruction> <src reg, ie. %n>, <dst reg %n>"
: output reg/addr (1st parameter), %2, .. %n
: input reg/addr %n+1, %n+2, ... (last parameter)
: clobber list register/memory/etc. );

The output/input contents are specified with "constraint", (c code).
I used the following contrains:
"m"  memory, implied "&" (reference)
"r" register
"0" same as first output variable
"q" only registers a,b,c,d (intel only)

The following are constraint modifiers:
"=" write only operand
"i" integer type
"+" read and write operand
"memory" code changes the contents of memory

__volatile__ to make sure the compiler doesn't move the generated code
out of order in an attempt to optimize it.  With atomics we very much
need them modified _only_ when we want them modified.

- Jer

PS:  The "lock" instruction works, but is pointless on a uniprocessor machine.
So it takes another instruction, I didn't think it was worth optimizing
out for UP kernels.

*/

// Assignments and reads don't need atomic specific instructions
static inline void fusion_atomic_set(fusion_atomic_t *atomp, int32_t val)
{
    atomp->value = val;
}

static inline int32_t fusion_atomic_read(fusion_atomic_t *atomp)
{
    return atomp->value;
}

static inline void fusion_atomic_add(int32_t val, fusion_atomic_t *atomp)
{
    __asm__ __volatile__("\n\tlock; addl %1, %0"
                         :"=m"(atomp->value)
                         :"ir"(val), "m"(atomp->value));
}

static inline void fusion_atomic_inc(fusion_atomic_t *x)
{
    __asm__ __volatile__("\n\tlock; incl %0"
                         :"=m"(x->value) :"m"(x->value));
}

static inline int32_t fusion_atomic_add_return(int32_t val, fusion_atomic_t *atomp)
{
    int i = val;

    __asm__ __volatile__("\n\tlock; xaddl %0, %1"
                         :"+r"(val), "+m"(atomp->value)
                         :
                         :"memory");
    return val + i;
}

static inline void fusion_atomic_dec(fusion_atomic_t *x)
{
    __asm__ __volatile__("\n\tlock; decl %0"
                         :"=m"(x->value) :"m"(x->value));
}

#define fusion_atomic_decr(x)         fusion_atomic_add_return(-1, x)
#define fusion_atomic_incr(x)         fusion_atomic_add_return(1, x)

static inline void fusion_atomic_sub(int32_t val, fusion_atomic_t *atomp)
{
    __asm__ __volatile__("\n\tlock; subl %1, %0"
                         :"=m"(atomp->value)
                         :"ir"(val), "m"(atomp->value));
}

#define fusion_atomic_sub_return(val,atomp)  fusion_atomic_add_return(-val,atomp)

static inline int32_t fusion_atomic_exchange(fusion_atomic_t *atomp, volatile int32_t val)
{
    __asm__ __volatile__("xchgl %k0,%1"
                         :"=r" (val)
                         :"m" (atomp->value), "0" (val)
                         :"memory");
    return val;
}

#endif /* __GNUC__ */

#endif /* _FIO_PORT_ARCH_X86_ATOMIC_H */
