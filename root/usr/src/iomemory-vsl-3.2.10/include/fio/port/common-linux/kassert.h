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

#ifndef __FIO_PORT_LINUX_KASSERT_H__
#define __FIO_PORT_LINUX_KASSERT_H__

#include <fio/port/port_config.h>

#if defined(__KERNEL__)
extern void __kassert_fail(const char *expr, const char *file, int line, const char *func, int good, int bad) ;

#if FUSION_DEBUG

#define kbreakpoint()   do { } while(0)

#define kassert(expr)                                   \
    do {                                                \
        static int good=0;                              \
        static int bad=0;                               \
        if(!(expr)) {                                   \
            __kassert_fail (#expr, __FILE__, __LINE__,  \
                            __func__, good, bad++);     \
        } else good++;                                  \
    } while(0)

#define kassert_once(expr)                              \
    do {                                                \
        static int good=0;                              \
        static int asserted__=0;                        \
        if(!asserted__ && !(expr)) {                    \
            __kassert_fail (#expr, __FILE__, __LINE__,  \
                            __func__, good, 0);         \
            asserted__=1;                               \
        } else good++;                                  \
    } while(0)

#define kfail()                                                     \
    do {                                                            \
        __kassert_fail("FAILURE: See above\n", __FILE__, __LINE__,  \
                       __func__, 0, 1);                             \
    } while(0)

#define kassert_msg(test, y, args...)           \
    do                                          \
    {                                           \
        if(!(test))                             \
        {                                       \
            errprint(y, ## args);               \
            kfail();                            \
        }                                       \
    }                                           \
    while(0)

#define kwarn(x, args...)                       \
    do {                                        \
        kfio_print(x, ## args);                   \
        kfio_dump_stack();                        \
    } while(0)

#define kdebug(expression) expression

#else

#define kbreakpoint()                  do { } while(0)
#define kassert(expr)                  do { } while(0)
#define kassert_once(expr)             do { } while(0)
#define kwarn(x, args...)              do { } while(0)
#define kfail()                        do { } while(0)
#define kdebug(expression)             do { } while(0)
#define kassert_msg(test, y, args...)  do { } while(0)

#endif

#define kassert_release(expr)                           \
    do {                                                \
        static int good=0;                              \
        static int bad=0;                               \
        if(!(expr)) {                                   \
            __kassert_fail (#expr, __FILE__, __LINE__,  \
                            __func__, good, bad);       \
        } else good++;                                  \
    } while(0)

#endif  // __KERNEL__
#endif

