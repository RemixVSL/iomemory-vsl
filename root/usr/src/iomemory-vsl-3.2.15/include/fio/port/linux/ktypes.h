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

#ifndef __FUSION_LINUX_KTYPES_H__
#define __FUSION_LINUX_KTYPES_H__

#ifndef __FIO_PORT_KTYPES_H__
#error Do not include this file directly - instead include <fio/port/ktypes.h>
#endif

#include <fio/port/common-linux/kassert.h>
#ifndef BUILDING_MODULE
#include <fio/port/stdint.h>
#else
#include <linux/types.h>
#if defined(__VMKLNX__)
#include <linux/string.h>
#include <linux/mutex.h>
#endif
#endif
#include <fio/port/align.h>
#include <fio/port/bitops.h>
#include <fio/port/common-linux/commontypes.h>

/**
 * brief @struct __fusion_poll_struct
 * OS-independent structure for poll support:
 * Linux wait_queue_head_t
 * Solaris struct pollhead
 * FreeBSD struct selinfo
 *
 */
struct FUSION_STRUCT_ALIGN(8) __fusion_poll_struct {
    DECLARE_RESERVE(196);
};

#define KERNEL_SECTOR_SIZE   512 /* ARGH */

typedef struct __kfio_pci_bus kfio_pci_bus_t;
typedef struct __kfio_pci_dev kfio_pci_dev_t;

typedef struct kfio_pci_dev_id
{
    uint16_t vendor;
    uint16_t device;
} kfio_pci_dev_id_t;

typedef struct kfio_disk kfio_disk_t;

/** XXX should wrap this in \#if KFIO_IRQRETURNS_INT */
typedef int            kfio_irqreturn_t;
#define FIO_IRQ_NONE        (0)
#define FIO_IRQ_HANDLED     (1)
#define FIO_IRQ_RETVAL(x)   ((x) != 0)

// We just need 2x4 bytes on Linux
#if defined(__KERNEL__) && !defined(USERSPACE_KERNEL)
typedef struct kfio_msix {
    DECLARE_RESERVE(8);
} kfio_msix_t;
#define PORT_SUPPORTS_MSIX
#endif

typedef unsigned long kfio_get_cpu_t;
typedef int kfio_numa_node_t;

#define FIO_NUMA_NODE_NONE      -1

/* FIXME - isn't there something like __WORDSIZE that can be used? */
#if defined(__x86_64__) || defined(__PPC64__) || defined(__mips64)
#define FIO_WORD_SIZE 8
#define FIO_BITS_PER_LONG 64
#elif defined(__i386__) || defined(__PPC__)
#define FIO_WORD_SIZE 4
#define FIO_BITS_PER_LONG 32
#else
#error Unsupported wordsize!
#endif

//#if ! KFIOC_HAS_INTPTR_T
//typedef          long  intptr_t;
//typedef unsigned long uintptr_t;
//#endif

#define LODWORD(quad)            ( (uint32_t)(quad) )
#define HIDWORD(quad)            ( (uint32_t)(((quad) >> 32) & 0x00000000ffffffffULL) )

#define ASSERT(x)                ((void)0)

#include <fio/port/list.h>

/** XXX the following need to come thru the porting layer */
#if defined(__mips64)
#define FUSION_PAGE_SHIFT           16
#elif defined(__PPC64__)
#define FUSION_PAGE_SHIFT           16
#else
#define FUSION_PAGE_SHIFT           12
#endif
#define FUSION_PAGE_SIZE            (1UL<<FUSION_PAGE_SHIFT)
#define FUSION_PAGE_MASK            (~(FUSION_PAGE_SIZE-1))
#define FUSION_PAGE_ALIGN_UP(addr)  ((char*)((((fio_uintptr_t)(addr))+FUSION_PAGE_SIZE-1)&FUSION_PAGE_MASK))
#define FUSION_ROUND_TO_PAGES(size) ((fio_uintptr_t)FUSION_PAGE_ALIGN_UP(size))

#if defined(__KERNEL__)
#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define S_IRUGO         (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO         (S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO         (S_IXUSR|S_IXGRP|S_IXOTH)

#endif // __KERNEL

// Valid values for use_workqueue.
#define USE_QUEUE_NONE   0 ///< directly submit requests.
#define USE_QUEUE_WQ     1 ///< use kernel work queue.
#define USE_QUEUE_SINGLE 2 ///< use single kernel thread.
#define USE_QUEUE_RQ     3 ///< use single kernel thread and blk_init_queue style queueing.

struct __fusion_sema {
// This is horribly ugly, but this needs to be large enough
// to acomodate any platforms semaphore structure.  We cast it
// in the wrapper layer to whatever it needs to be in the
// host OS.  In this case, the Linux kernel uses rw_sema structs
// that are 24 bytes long, and normal semaphore structs that are
// 32 bytes long, so we will use the maximum.
    DECLARE_RESERVE(180); // linux 2.6.29 kernel with mutex debugging on: 152 bytes...
};

typedef volatile struct __fusion_sema     fusion_rwsem_t;

/* DO NOT USE INTERRUPTIBLE FUNCTIONS */
#define __int_compile_error() C_ASSERT(0)

extern void fusion_rwsem_init(fusion_rwsem_t *x, const char *);
extern void fusion_rwsem_delete(fusion_rwsem_t *x);
extern void fusion_rwsem_down_read(fusion_rwsem_t *x);
extern int fusion_rwsem_down_read_trylock(fusion_rwsem_t *x);
extern void fusion_rwsem_up_read(fusion_rwsem_t *x);
extern void fusion_rwsem_down_write(fusion_rwsem_t *x);
extern void fusion_rwsem_up_write(fusion_rwsem_t *x);
extern int fusion_rwsem_down_write_trylock(fusion_rwsem_t *x);

#define PORT_SUPPORTS_TRYLOCK_RWSEM

//#define fusion_set_relative_timer(o,t)   mod_timer((o),jiffies + (t))
//#define fusion_del_timer(t)              del_timer_sync(t)

struct __fusion_spinlock
{
// This must be large enough to handle all spinlock port
// implementations.
    DECLARE_RESERVE(192);

};

typedef volatile struct __fusion_spinlock  fusion_spinlock_t;
typedef volatile struct __fusion_spinlock  fusion_rw_spinlock_t;

// spin_lock must always be called in a consistent context, i.e. always in
// user context or always in interrupt context. The following two macros
// make this distinction. Use fusion_spin_lock() if your lock is always
// obtained in user context; use fusion_spin_lock_irqdisabled if your
// lock is always obtained in interrupt contect (OR with interrupts
// disabled); and use fusion_spin_trylock_irqsave if it can be called in
// both or if you don't know.
extern void fusion_init_spin(fusion_spinlock_t *s, const char *name);
extern void fusion_init_rw_spin(fusion_rw_spinlock_t *s, const char *name);
extern void fusion_destroy_spin(fusion_spinlock_t *s);
extern void fusion_spin_lock(fusion_spinlock_t *s);
extern int  fusion_spin_is_locked(fusion_spinlock_t *s);
extern int  fusion_spin_trylock(fusion_spinlock_t *s);
extern void fusion_spin_unlock(fusion_spinlock_t *s);
extern void fusion_spin_lock_irqdisabled(fusion_spinlock_t *s);
extern void fusion_spin_lock_irqsave(fusion_spinlock_t *s);
extern int  fusion_spin_trylock_irqsave(fusion_spinlock_t *s);
extern void fusion_spin_unlock_irqrestore(fusion_spinlock_t *s);
extern int  fusion_spin_is_irqsaved(fusion_spinlock_t *s);
extern void fusion_spin_lock_irqsave_nested(fusion_spinlock_t *s, int subclass);

extern void fusion_destroy_rw_spin(fusion_rw_spinlock_t *s);
extern void fusion_spin_read_lock(fusion_rw_spinlock_t *s);
extern void fusion_spin_write_lock(fusion_rw_spinlock_t *s);
extern void fusion_spin_read_unlock(fusion_rw_spinlock_t *s);
extern void fusion_spin_write_unlock(fusion_rw_spinlock_t *s);

typedef struct __fusion_sema fusion_mutex_t;

extern void fusion_mutex_init(fusion_mutex_t *lock, const char *);
extern void fusion_mutex_destroy(fusion_mutex_t *lock);
extern int  fusion_mutex_trylock(fusion_mutex_t *lock);
extern void fusion_mutex_lock(fusion_mutex_t *lock);
extern void fusion_mutex_unlock(fusion_mutex_t *lock);

#define fusion_mutex_lock_interruptible(x)  __int_compile_error()

typedef void    *fusion_kthread_t;

#define MAX_KTHREAD_NAME_LENGTH 40
typedef int (*fusion_kthread_func_t)(void *);

//nand_device can be NULL in the following fxn call
void fusion_create_kthread(fusion_kthread_func_t func, void *data, void *fusion_nand_device,
                           const char *fmt, ...);
#if defined(__VMKLNX__)
struct task_struct *fusion_esx_create_kthread(fusion_kthread_func_t func, void *data,
                                              void *fusion_nand_device, const char *fmt, ...);
void fusion_esx_wakeup_thread(struct task_struct *ts);
#endif

////#define kfio_page_address(pg)          page_address(pg)

#if defined(__x86_64__)
#define kfio_barrier()          asm volatile("mfence":::"memory")
#define kfio_store_barrier()    asm volatile("sfence":::"memory")
#define kfio_load_barrier()     asm volatile("lfence":::"memory")

#else
#define kfio_barrier()          __sync_synchronize()
#endif

#if 0
#if KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT

typedef struct work_struct fusion_dpc_t;
typedef struct delayed_work fusion_delayed_dpc_t;

#define fusion_setup_dpc(wq, func)           INIT_WORK(wq, func)
#define fusion_setup_delayed_dpc(wq, func)   INIT_DELAYED_WORK(wq, func)

#else

typedef struct work_struct fusion_dpc_t;
typedef struct work_struct fusion_delayed_dpc_t;

#define fusion_setup_dpc(wq, func)           INIT_WORK(wq, func, wq)
#define fusion_setup_delayed_dpc(wq, func)           INIT_WORK(wq, func, wq)

#endif

#define fusion_schedule_dpc(dpc)             schedule_work(dpc)
#define fusion_schedule_delayed_dpc(dpc,t)   schedule_delayed_work(dpc,t)

#endif

// Returns true if we're running on what is considered a server OS
#if defined(__VMKLNX__)
#define fusion_is_server_os()     1
#else
#define fusion_is_server_os()     0
#endif

#define fusion_divmod(quotient, remainder, dividend, divisor)  do { quotient = dividend / divisor;  remainder = dividend % divisor; } while(0)

#if defined(__i386__)

static inline uint64_t kfio_rdtsc(void)
{
    uint64_t x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static inline uint64_t kfio_rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
}

#elif defined(__powerpc__)

static inline unsigned long long kfio_rdtsc(void)
{
    unsigned long long int result=0;
    unsigned long int upper, lower,tmp;
    __asm__ volatile(
        "0:                  \n"
        "\tmftbu   %0           \n"
        "\tmftb    %1           \n"
        "\tmftbu   %2           \n"
        "\tcmpw    %2,%0        \n"
        "\tbne     0b         \n"
        : "=r"(upper),"=r"(lower),"=r"(tmp)
        );
    result = upper;
    result = result<<32;
    result = result|lower;

    return(result);
}

#endif

#if defined(__VMKLNX__)
/* FH-11140: ESX can crash under certain proc operations, using sysctl kinfo
 *           backend.  Thus, we use the agnostic backend instead.
 */
#define KFIO_INFO_USE_OS_BACKEND 0
#define KFIO_SUPPORTS_DUAL_PIPE 0
#else
/* Linux port implements kinfo backend using sysctl. */
#define KFIO_INFO_USE_OS_BACKEND 1
#define KFIO_SUPPORTS_DUAL_PIPE 1
#endif

#endif
