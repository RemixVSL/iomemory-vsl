
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#ifndef _FIO_PORT_ARCH_X86_ATOMIC_H
#define _FIO_PORT_ARCH_X86_ATOMIC_H

// Use the sync builtins if our gcc version ( >= 4.0 ) supports it.  If not
// the following functions will be used instead.
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2

static inline void fusion_atomic_set(volatile int32_t *atomp, int32_t val)
{
    *atomp = val;
}

static inline int32_t fusion_atomic_read(volatile int32_t *atomp)
{
    return *atomp;
}

static inline void fusion_atomic_add(int32_t val, volatile int32_t *atomp)
{
    (void)__sync_add_and_fetch(atomp, val);
}

static inline void fusion_atomic_inc(volatile int32_t *atomp)
{
    (void)__sync_add_and_fetch(atomp, 1);
}

static inline int32_t fusion_atomic_incr(volatile int32_t *atomp)
{
    return __sync_add_and_fetch(atomp, 1);
}

static inline int32_t fusion_atomic_add_return(int32_t val, volatile int32_t *atomp)
{
    return __sync_add_and_fetch(atomp, val);
}

static inline void fusion_atomic_dec(volatile int32_t *atomp)
{
    (void)__sync_sub_and_fetch(atomp, 1);
}

static inline int32_t fusion_atomic_decr(volatile int32_t *atomp)
{
    return __sync_sub_and_fetch(atomp, 1);
}

static inline void fusion_atomic_sub(int32_t val, volatile int32_t *atomp)
{
    (void)__sync_sub_and_fetch(atomp, val);
}

static inline int32_t fusion_atomic_sub_return(int32_t val, volatile int32_t *atomp)
{
    return __sync_sub_and_fetch(atomp, val);
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
static inline int32_t fusion_atomic_exchange(volatile int32_t *atomp, int32_t val)
{
    return __sync_lock_test_and_set(atomp, val);
}

#else
// Older gcc compiler here.  Hopefully the following ugly assembly code will someday dissapear.
// ___ This is INTEL/AMD specific and requires 32-bit integers ONLY ___

/*

Since I'm very likely to forget how this inline assembly syntax is formatted, heres
a quick explanation for those who are unfortunate enough to have to look at it.  
Inline assembly is not typical assembly syntax becuase most of this stuff gives the compiler a
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
static inline void fusion_atomic_set(volatile int32_t *atomp, int32_t val)
{
    *atomp = val;
}

static inline int32_t fusion_atomic_read(volatile int32_t *atomp)
{
    return *atomp;
}

static inline void fusion_atomic_add(int32_t val, volatile int32_t *atomp){
    __asm__ __volatile__("\n\tlock; addl %1, %0"
                         :"=m"(*atomp)
                         :"ir"(val), "m"(*atomp));
}

static inline void fusion_atomic_inc(volatile int32_t *x)
{
    __asm__ __volatile__("\n\tlock; incl %0"
                         :"=m"(*x) :"m"(*x));
}

static inline int32_t fusion_atomic_add_return(int32_t val, volatile int32_t *atomp)
{
    int i = val;

    __asm__ __volatile__("\n\tlock; xaddl %0, %1"
                         :"+r"(val), "+m"(*atomp)
                         : 
                         :"memory");
    return val + i;
}

static inline void fusion_atomic_dec(volatile int32_t *x)
{
    __asm__ __volatile__("\n\tlock; decl %0"
                         :"=m"(*x) :"m"(*x));
}

#define fusion_atomic_decr(x)         fusion_atomic_add_return(-1, x)
#define fusion_atomic_incr(x)         fusion_atomic_add_return(1, x)

static inline int32_t fusion_atomic_dec_is_zero(volatile int32_t *x)
{
    unsigned char n;
    __asm__ __volatile__("\n\tlock; decl %0; sete %1"
                         :"=m"(*x), "=qm"(n)
                         :"m"(*x)
                         :"memory");

    // n should now contain the result of the decl instruction
    // sete puts the zero flag in a register (a,b,c,d) which
    // will end up as n
    if(n)
        return 0;

    return 1;
}

static inline void fusion_atomic_sub(int32_t val, volatile int32_t *atomp)
{
    __asm__ __volatile__("\n\tlock; subl %1, %0"
                         :"=m"(*atomp)
                         :"ir"(val), "m"(*atomp));
}

#define fusion_atomic_sub_return(val,atomp)  fusion_atomic_add_return(-val,atomp)

static inline int32_t fusion_atomic_exchange(volatile int32_t *atomp, volatile int32_t val) 
{
    __asm__ __volatile__("xchgl %k0,%1"
                         :"=r" (val)
                         :"m" (*atomp), "0" (val)
                         :"memory");
    return val;
}

#endif /* __GNUC__ */

#endif /* _FIO_PORT_ARCH_X86_ATOMIC_H */
