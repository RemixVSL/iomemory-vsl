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

#ifndef __FIO_PORT_BITOPS_H__
# define __FIO_PORT_BITOPS_H__

# if defined(__linux__) && !defined(__PPC64__) && !defined(__PPC__) && !defined(__mips64) && !defined(__ia64__)
#  include <fio/port/arch/bits.h>
#  if defined(__x86_64__)
#   include <fio/port/arch/x86_64/bits.h>
#  endif
# elif defined(__ia64__)
#  include <fio/port/arch/ia64_bits.h>
# elif defined(__PPC64__) || defined(__PPC__) || defined(__mips64)
typedef uint64_t fusion_bits_t;
#  define fio_test_bit           kfio_test_bit
#  define fio_set_bit            kfio_set_bit
#  define fio_clear_bit          kfio_clear_bit
#  define fio_test_and_set_bit   kfio_test_and_set_bit
#  define fio_test_and_clear_bit kfio_test_and_clear_bit
# elif defined(__SVR4) && defined(__sun)
#  if defined(__x86_64__)
#   include <fio/port/arch/bits.h>
#   include <fio/port/arch/x86_64/bits.h>
#   include <fio/port/arch/x86_atomic.h>
#   include <fio/port/arch/x86_cache.h>
#  elif defined (__i386__)
#   include <fio/port/arch/bits.h>
#   include <fio/port/arch/i386/bits.h>
#   include <fio/port/arch/x86_atomic.h>
#   include <fio/port/arch/x86_cache.h>
#  elif defined(__sparc__)
#   define fio_test_bit           kfio_test_bit
#   define fio_set_bit            kfio_set_bit
#   define fio_clear_bit          kfio_clear_bit
#   define fio_test_and_set_bit   kfio_test_and_set_bit
#   define fio_test_and_clear_bit kfio_test_and_clear_bit

typedef struct fusion_atomic {
    volatile int value;
} fusion_atomic_t;

extern void fusion_atomic_set(fusion_atomic_t *x, int y);
extern int  fusion_atomic_read(fusion_atomic_t *x);

extern void fusion_atomic_inc(fusion_atomic_t *x);
extern int  fusion_atomic_incr(fusion_atomic_t *x);
extern void fusion_atomic_dec(fusion_atomic_t *x);
extern int  fusion_atomic_decr(fusion_atomic_t *x);

extern void fusion_atomic_add(int val, fusion_atomic_t *atomp);
extern int  fusion_atomic_add_return(int val, fusion_atomic_t *atomp);
extern void fusion_atomic_sub(int val, fusion_atomic_t *atomp);
extern int  fusion_atomic_sub_return(int val, fusion_atomic_t *atomp);
extern int  fusion_atomic_exchange(fusion_atomic_t *atomp, volatile int val);

#  else
#   error Unknown Solaris Architecture
#  endif

# elif defined(__FreeBSD__)
#  include <fio/port/arch/bits.h>
#  include <fio/port/arch/x86_64/bits.h>
#  include <fio/port/arch/x86_atomic.h>

# elif defined(_AIX)
#  include <fio/port/arch/bits.h>

# elif defined(__OSX__)
#  include <fio/port/arch/bits.h>
#  include <fio/port/arch/x86_64/bits.h>
#  include <fio/port/arch/x86_atomic.h>

# elif defined(WIN32) || defined(WINNT)
//#define fio_test_bit           test_bit
//#define fio_set_bit            set_bit
//#define fio_clear_bit          clear_bit
//#define fio_test_and_set_bit   test_and_set_bit
//#define fio_test_and_clear_bit test_and_clear_bit
# elif defined(UEFI)
#   include <fio/port/uefi/bits.h>
# elif defined(USERSPACE_KERNEL)
#  if  defined(__x86_64__)
#   include <fio/port/arch/bits.h>
#   include <fio/port/arch/x86_64/bits.h>
#  elif defined(__i386__)
#   include <fio/port/arch/bits.h>
#   include <fio/port/arch/i386/bits.h>
#  else
#   error USERSPACE_KERNEL unsupported bitops
#  endif

# else

# error You must define bit operations for your platform.

# endif

// If we are using GCC, there are builtins for bitcounting which can exploit
// magic processor instructions. However these require library support which
// is not available in-kernel (or anywhere at all in AIX).
# if defined(__GNUC__) && !defined(__KERNEL__) && !defined(_AIX)

/// @brief return number of bits set in the given 8 bit quantity.
static inline unsigned fio_bitcount8(uint8_t val)
{
    return (uint32_t)__builtin_popcount(val);
}

/// @brief return number of bits set in the given 32 bit quantity.
static inline unsigned fio_bitcount32(uint32_t val)
{
    return (uint32_t)__builtin_popcount(val);
}

/// @brief return number of bits set in the given 64 bit quantity.
static inline unsigned fio_bitcount64(int64_t val)
{
    return (uint32_t)__builtin_popcountl(val);
}

# else // Not using GCC bitcount builtins.

/// @brief return number of bits set in the given 8 bit quantity.
///
/// Algorithm available in the public domain, see http://www-graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel.
static inline unsigned fio_bitcount8(uint8_t val)
{
    val = val - ((val >> 1) & 0x55);
    val = (val & 0x33) + ((val >> 2) & 0x33);
    return (val + (val >> 4)) & 0x0F;
}
/// @brief return number of bits set in the given 32 bit quantity.
///
/// Algorithm available in the public domain, see http://www-graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel.
static inline unsigned fio_bitcount32(uint32_t val)
{
    val = val - ((val >> 1) & 0x55555555);
    val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
    return (((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

/// @brief return number of bits set in the given 64 bit quantity.
///
/// Algorithm available in the public domain, see http://www-graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel.
static inline unsigned fio_bitcount64(uint64_t val)
{
    val = val - ((val >> 1) & 0x5555555555555555ULL);
    val = (val & 0x3333333333333333ULL) + ((val >> 2) & 0x3333333333333333ULL);
    return (((val + (val >> 4)) & 0xf0f0f0f0f0f0f0fULL) * 0x101010101010101ULL) >> 56;
}
# endif // __GNUC__

/// @brief return the index of the 'n'th set bit in the given 32-bit value.
///
/// 'n' is one based and index is zero based, i.e. fio_get_nth_bit_set_index32(x, 1)
/// returns the zero-based index of the first set bit in x. It is an error to invoke
/// with n less than 1 or greater than 32.
///
/// Returns 32 if no such bit found.
static inline uint32_t fio_get_nth_bit_set_index32(const uint32_t val, uint32_t n)
{
    uint32_t found = 0;
    uint32_t index = 0;

    while (index < 32)
    {
        if (val & (1 << index))
        {
            if (++found == n)
            {
                break;
            }
        }
        index++;
    }

    return index;
}

/// @brief return the index of the top set bit in the given 32-bit value.
///
/// Returns -1 if no such bit found.
static inline int32_t fio_get_top_bit_set_index32(const uint32_t val)
{
    int32_t index = 31;

    while (index >= 0)
    {
        if (val & (1 << index))
        {
            break;
        }
        --index;
    }

    return index;
}

#endif /* __FIO_PORT_BITOPS_H__  */
