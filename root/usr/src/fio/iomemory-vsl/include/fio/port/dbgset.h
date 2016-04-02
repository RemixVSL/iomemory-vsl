//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _DBGSET_H_
#define _DBGSET_H_

#if defined(__KERNEL__)
#include <fio/port/kfio.h>
#include <fio/port/config.h>
#endif

#if defined(WINNT) || defined(WIN32)
#include "../../../src/kernel/fio-port/windows/ntdrv/winlogmsg.h"
#endif // WINNT

/* FIXME - put this in a central location - maybe global.h? */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#if defined(EXTERNAL_OS)
#include "fio/port/external_os_dbgset.h"
#endif

enum _DBGSET_FLAGS {
#include <fio/port/config_macros_clear.h>
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(cfg) _DF_ ## cfg,
#include <fio/port/config_macros.h>
    _DF_END
};

#undef FIO_CONFIG_MACRO

#if defined(__KERNEL__)

struct dbgflag {
    const char            *name;
    fio_mode_t             mode;
    unsigned int           value;
};

struct dbgset {
    kfio_info_node_t      *flag_dir;
    const char            *flag_dir_name;
    struct dbgflag        *flags;
};

extern struct dbgset _dbgset;

extern int dbgs_create_flags_dir(struct dbgset *ds);
extern int dbgs_delete_flags_dir(struct dbgset *ds);

#endif

#if defined(DBGSET_LOCATIONS) && (DBGSET_LOCATIONS==1)
#define AT __FILE__ ":" TOSTRING(__LINE__) " "
#else
#define AT
#endif


/* convert DBGSET_COMPILE_DBGPRINT_OUT to xDBGSET_COMPILE_DBGPRINT_OUT for
 * a quick disable of the section.
 */
#define xDBGSET_COMPILE_DBGPRINT_OUT 1


#if DBGSET_COMPILE_DBGPRINT_OUT

#define dbgflag_state(x)                   (0)
#define dbgprint(flag_def, ...)            do {} while(0)
#define dbgprint_cond(flag_def, cond, ...) do {} while(0)

#elif defined(__KERNEL__)

/* Get the state of a flag from a dbgset */
#define dbgflag_state(df_flag)    \
    (_dbgset.flags[_DF_ ## df_flag].value)

#define dbgflag_set_state(df_flag, x)             \
    (_dbgset.flags[_DF_ ## df_flag].value = (x))

/* Print output if <flag_def> is set to 1, don't print if <flag_def> is set
 * to 0.
 */
#define dbgprint(df_flag, ...)                              \
    do {                                                    \
        if (unlikely(_dbgset.flags[_DF_ ## df_flag].value)) \
            dbgkprint(__VA_ARGS__);                         \
    } while (0)


/* A version of dbgprint() that adds an additional, run-time condition
 * <cond>. */
#define dbgprint_cond(df_flag, cond, ...)                         \
    do {                                                          \
    if (unlikely(_dbgset.flags[_DF_ ## df_flag].value && (cond))) \
        dbgkprint(__VA_ARGS__);                                   \
    } while (0)

#else

#define dbgprint(df_flag, ...)                              \
    do {                                                    \
        kfio_print("fiodbg " AT __VA_ARGS__);               \
    } while (0)


/* A version of dbgprint() that adds an additional, run-time condition
 * <cond>. */
#define dbgprint_cond(df_flag, cond, ...)                          \
    do {                                                           \
        if (cond)                                                  \
            kfio_print("fiodbg " AT __VA_ARGS__);                  \
    } while (0)

#endif /* DBGSET_COMPILE_DBGPRINT_OUT */

#ifndef __KERNEL__

#define errprint(...)  printf("fioerr " AT __VA_ARGS__)
#define infprint(...)  printf("fioinf " AT __VA_ARGS__)
#define dbgkprint(...) printf("fiodbg " AT __VA_ARGS__)
#define cinfprint(...) printf(__VA_ARGS__)

#elif defined(USERSPACE_KERNEL)

#include <stdio.h>

#define errprint(...)  do { printf("fioerr " AT __VA_ARGS__); fflush(stdout); } while(0)
#define infprint(...)  printf("fioinf " AT __VA_ARGS__)
#define dbgkprint(...) printf("fiodbg " AT __VA_ARGS__)
#define cinfprint(...) printf(__VA_ARGS__)

#elif defined(WINNT) || defined(WIN32)

#define errprint(base_str, ...) WinLogMessage(base_str, WIN_LOG_MESSAGE_ERROR, __VA_ARGS__)
#define wrnprint(base_str, ...) WinLogMessage(base_str, WIN_LOG_MESSAGE_WARNING, __VA_ARGS__)
// Use winfprint in Windows specific code for info messages that we want in the
// System Event Log. Change this to infprint when core infprint's are cleaned up
// and routed to the win event log.
#define winfprint(base_str, ...) WinLogMessage(base_str, WIN_LOG_MESSAGE_WINFO, __VA_ARGS__)
// infprint should go to the windows log - but there are simply too many extraneous
// info messages currently in the base driver. However, if the config param WIN_LOG_VERBOSE
// is set, we will allow the spew to go to the system log.  WIN_LOG_VERBOSE is false by
// default.
#define infprint(base_str, ...) WinLogMessage(base_str, WIN_LOG_MESSAGE_INFO, __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define dbgkprint(...) kfio_print(KERN_DEBUG "fiodbg " AT __VA_ARGS__)

#elif defined(__linux__)
#define errprint(...)  kfio_print(KERN_ERR  "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_print(KERN_INFO "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define dbgkprint(...) kfio_print(KERN_DEBUG "fiodbg " AT __VA_ARGS__)

#elif defined (__SVR4) && defined (__sun)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define solprint(...)  kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)

#elif defined (__FreeBSD__)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define bsdprint(...)  kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)

#elif defined (_AIX)
#define errprint(...)  printf(__VA_ARGS__)
#define infprint(...)  printf(__VA_ARGS__)
#define cinfprint(...) printf(__VA_ARGS__)
#define dbgkprint(...) printf(__VA_ARGS__)
#define     va_copy(save,ap)    save = ap

#elif defined (__OSX__)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#endif

#define xerrprint(format, ...) do {} while(0)
#define xinfprint(format, ...) do {} while(0)
#define xdbgprint(flag_def, ...) do {} while(0)
#define xdbgprint_cond(flag_def, cond, ...) do {} while(0)

#endif /* _DBGSET_H_ */

