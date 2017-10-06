//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015 SanDisk Corp. and/or all its affiliates. All rights reserved.
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

#ifndef _DBGSET_H_
#define _DBGSET_H_

#if defined(__KERNEL__)
#include <fio/port/kfio.h>
#endif

/// @cond GENERATED_CODE
#include <fio/port/port_config.h>
/// @endcond

/* FIXME - put this in a central location - maybe global.h? */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

enum _DBGSET_FLAGS {

/// @cond GENERATED_CODE
#include <fio/port/port_config_macros_clear.h>
/// @endcond

#define FIO_PORT_CONFIG_SET_DBGFLAG
#define FIO_PORT_CONFIG_MACRO(cfg) _DF_ ## cfg,

/// @cond GENERATED_CODE
#include <fio/port/port_config_macros.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config_macros_clear.h>
#define FIO_CONFIG_SET_DBGFLAG
#define FIO_CONFIG_MACRO(cfg) _DF_ ## cfg,
#include <fio/internal/config_macros.h>
#endif
/// @endcond

    _DF_END
};

#undef FIO_PORT_CONFIG_MACRO

#if (FUSION_INTERNAL==1)
#undef FIO_CONFIG_MACRO
#endif

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


#if (defined(DBGSET_COMPILE_DBGPRINT_OUT) && DBGSET_COMPILE_DBGPRINT_OUT) || \
       FUSION_INTERNAL==0

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

#if FUSION_INTERNAL == 1
#define engprint(...)  printf("fioeng " AT __VA_ARGS__)
#endif

#elif defined(USERSPACE_KERNEL)

#include <stdio.h>

#define errprint(...)  do { printf("fioerr " AT __VA_ARGS__); fflush(stdout); } while(0)
#define infprint(...)  printf("fioinf " AT __VA_ARGS__)
#define dbgkprint(...) printf("fiodbg " AT __VA_ARGS__)
#define cinfprint(...) printf(__VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  printf("fioeng " AT __VA_ARGS__)
#endif

#elif defined(WINNT) || defined(WIN32)

#define errprint(base_str, ...) WinLogMessage(WIN_LOG_MESSAGE_ERROR, base_str, __VA_ARGS__)
#define wrnprint(base_str, ...) WinLogMessage(WIN_LOG_MESSAGE_WARNING, base_str, __VA_ARGS__)
// Use winfprint in Windows specific code for info messages that we want in the
// System Event Log. Change this to infprint when core infprint's are cleaned up
// and routed to the win event log.
#define winfprint(base_str, ...) WinLogMessage(WIN_LOG_MESSAGE_WINFO, base_str, __VA_ARGS__)
// infprint will go to the system log by default. If you wish to avoid spew in the system log,
// set the config param WIN_LOG_VERBOSE to 0 via fio-config.
#define infprint(base_str, ...) WinLogMessage(WIN_LOG_MESSAGE_INFO, base_str, __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#ifndef dbgkprint
#define dbgkprint(...) kfio_print(KERN_DEBUG "fiodbg " AT __VA_ARGS__)
#endif
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_print(__VA_ARGS__)
#endif

#elif defined(__linux__)
#define errprint(...)  kfio_print(KERN_ERR  "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_print(KERN_INFO "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define dbgkprint(...) kfio_print(KERN_DEBUG "fiodbg " AT __VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_print(KERN_INFO "fioeng " AT __VA_ARGS__)
#endif

#elif defined (__SVR4) && defined (__sun)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define solprint(...)  kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioeng " AT __VA_ARGS__)
#endif

#elif defined(__hpux__)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioeng " AT __VA_ARGS__)
#endif

#elif defined (__FreeBSD__)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_print(__VA_ARGS__)
#define bsdprint(...)  kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioeng " AT __VA_ARGS__)
#endif

#elif defined (_AIX)
#define errprint(...)  printf(__VA_ARGS__)
#define infprint(...)  printf(__VA_ARGS__)
#define cinfprint(...) printf(__VA_ARGS__)
#define dbgkprint(...) printf(__VA_ARGS__)
#define     va_copy(save,ap)    save = ap
#if FUSION_INTERNAL == 1
#define engprint(...)  printf(__VA_ARGS__)
#endif

#elif defined (__OSX__)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioeng " AT __VA_ARGS__)
#endif

#elif defined(UEFI)
#define errprint(...)  kfio_kprint(KFIO_PRINT_WARN, "fioerr " AT __VA_ARGS__)
#define infprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioinf " AT __VA_ARGS__)
#define cinfprint(...) kfio_kprint(KFIO_PRINT_NOTE, __VA_ARGS__)
#define dbgkprint(...) kfio_kprint(KFIO_PRINT_DEBUG, "fiodbg " AT __VA_ARGS__)
#define     va_copy(save,ap)    save = ap
#if FUSION_INTERNAL == 1
#define engprint(...)  kfio_kprint(KFIO_PRINT_NOTE, "fioeng " AT __VA_ARGS__)
#endif
#endif

#if FUSION_INTERNAL != 1
#define engprint(...)   do {} while(0)
#endif

#if FUSION_INTERNAL && FUSION_DEBUG

/// @brief Nucleon triage codes.
///
/// These are intended to provide an unambiguous reason for driver failure.
/// i.e. why did we call fusion_channel_bad(). Please do not renumber!
/// External tools use these codes.
typedef enum
{
    NUCT_INCOMPLETE_COMPLETION = 1,
    NUCT_WRITE_PATH_PARITY_ERROR = 2,
    NUCT_READ_PATH_PARITY_ERROR = 3,
    NUCT_DCM_LOCK_LOST = 4,
    NUCT_POISON_PKT = 5,
    NUCT_LOST_BAR = 6,
    NUCT_PAGE_BUFFER_PARITY = 7,
    NUCT_SBEU = 8,
    NUCT_PCIE_FATAL_INT = 9,
    NUCT_FELL_OFF_BUS = 10,
    NUCT_WATCHDOG_POWER_GOOD_0 = 11,
    NUCT_WATCHDOG_PCIE_FATAL = 12,
    NUCT_WATCHDOG_HIGH_PCIE_ERR = 13,
    NUCT_WATCHDOG_UNDIAGNOSED = 14
} nucleon_triage_code_e;

const char *nucleon_triage_code_to_string(nucleon_triage_code_e code);

# define NUCLEON_TRIAGE_FMT(chan, code, fmt, ...) \
    errprint("NUC-TRIAGE: SN %s CTL %s PCI %s SEQ %u @ %" PRIu64 ": %u %s " fmt, \
            chan->board_serial_number_string[0] ? chan->board_serial_number_string : "n/a", \
            chan->nand_channel.device ? ((struct iodrive_dev *)chan->nand_channel.device)->nand_dev.dev_name : "n/a", \
            chan->nand_channel.device ? ((struct iodrive_dev *)chan->nand_channel.device)->nand_dev.device_label : "n/a", \
            chan->nucleon_triage_counter++, \
            fusion_getmicrotime(), \
            code, nucleon_triage_code_to_string(code), \
            __VA_ARGS__)

#define NUCLEON_TRIAGE(chan, code) \
        NUCLEON_TRIAGE_FMT(chan, code, "%s", "\n")  /// have to have at least one arg after format so VA_ARGS is not empty.

#else // INTERNAL && DEBUG

# define NUCLEON_TRIAGE_FMT(chan, code, fmt, ...) do {} while(0)
# define NUCLEON_TRIAGE(chan, code) do {} while(0)

#endif

#endif /* _DBGSET_H_ */

