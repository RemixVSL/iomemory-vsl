//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_USERSPACE_KASSERT_H__
#define __FIO_PORT_USERSPACE_KASSERT_H__

#include <fio/port/config.h>

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

#else // FUSION_DEBUG

#define kbreakpoint()                  do { } while(0)
#define kassert(expr)                  do { } while(0)
#define kassert_once(expr)             do { } while(0)
#define kwarn(x, args...)              do { } while(0)
#define kfail()                        do { } while(0)
#define kdebug(expression)             do { } while(0)
#define kassert_msg(test, y, args...)  do { } while(0)

#endif // FUSION_DEBUG

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

