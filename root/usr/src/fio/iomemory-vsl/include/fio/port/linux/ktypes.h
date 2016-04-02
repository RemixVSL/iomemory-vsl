
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FUSION_LINUX_KTYPES_H__
#define __FUSION_LINUX_KTYPES_H__

#ifndef __FIO_PORT_KTYPES_H__
#error Do not include this file directly - instead include <fio/port/ktypes.h>
#endif

#include <fio/port/linux/kassert.h>
#ifndef BUILDING_MODULE
#include <fio/port/stdint.h>
#include <fio/port/bitops.h>
#else
#include <linux/types.h>
#if defined(__VMKLNX__)
#include <linux/string.h>
#include <linux/mutex.h>
#endif
#endif
#include <fio/port/linux/commontypes.h>
#include <fio/port/linux/kfile.h>

#define FUSION_STRUCT_ALIGN(alignment_byte)    __attribute__((aligned(alignment_byte)))

#define DECLARE_RESERVE(x) char foo[x] __attribute__ ((aligned(8)))

/**
 * brief @struct __fusion_seq_operations
 * OS-independent structure for seq file operations.
 */
struct __fusion_seq_operations {
    DECLARE_RESERVE(64);
};

/**
 * brief @struct __fusion_file_operations
 * OS-independent structure for file operations.
 */
struct __fusion_file_operations {
    DECLARE_RESERVE(248);
};

/**
 * brief @struct __fusion_poll_struct
 * OS-independent structure for poll support:
 * Linux wait_queue_head_t
 * Solaris struct pollhead
 * FreeBSD struct selinfo
 *
 */
struct __fusion_poll_struct {
    DECLARE_RESERVE(196);
} FUSION_STRUCT_ALIGN(8);

#define KERNEL_SECTOR_SIZE   512 /* ARGH */

typedef struct __kfio_pci_bus kfio_pci_bus_t;
typedef struct __kfio_pci_dev kfio_pci_dev_t;

typedef struct kfio_pci_dev_id
{
    uint16_t vendor;
    uint16_t device;
} kfio_pci_dev_id_t;

typedef struct kfio_disk kfio_disk_t;

/** XXX should wrap this in #if KFIO_IRQRETURNS_INT */
typedef int            kfio_irqreturn_t;
#define FIO_IRQ_NONE        (0)
#define FIO_IRQ_HANDLED     (1)
#define FIO_IRQ_RETVAL(x)   ((x) != 0)

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
extern void fusion_rwsem_up_read(fusion_rwsem_t *x);
extern void fusion_rwsem_down_write(fusion_rwsem_t *x);
extern void fusion_rwsem_up_write(fusion_rwsem_t *x);

typedef volatile int                fusion_atomic_t;

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
#if SPINLOCK_STATS
extern void fusion_init_spin_real(fusion_spinlock_t *s, const char *name, const char *file, int line);
#define fusion_init_spin(s, n) fusion_init_spin_real(s, n, __FILE__, __LINE__)
extern void fusion_init_rw_spin_real(fusion_rw_spinlock_t *s, const char *name, const char *file, int line);
#define fusion_init_rw_spin(s, n) fusion_init_rw_spin_real(s, n, __FILE__, __LINE__)
#else
extern void fusion_init_spin(fusion_spinlock_t *s, const char *name);
extern void fusion_init_rw_spin(fusion_rw_spinlock_t *s, const char *name);
#endif

extern void fusion_destroy_spin(fusion_spinlock_t *s);
extern void fusion_spin_lock(fusion_spinlock_t *s);
extern void fusion_spin_lock_irq(fusion_spinlock_t *s);
extern int  fusion_spin_is_locked(fusion_spinlock_t *s);
extern int  fusion_spin_trylock(fusion_spinlock_t *s);
extern void fusion_spin_unlock(fusion_spinlock_t *s);
extern void fusion_spin_unlock_irq(fusion_spinlock_t *s);
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

#if defined(__KERNEL__)
#if defined(__x86_64__) || defined(__i386__) || defined(__mips64)
#define fusion_card_to_host64(x) (x)
#define host_to_fusion_card64(x) (x)
#define fusion_card_to_host32(x) (x)
#define host_to_fusion_card32(x) (x)
#define fusion_card_to_host16(x) (x)
#define host_to_fusion_card16(x) (x)
#else // __PPC*__
static inline uint64_t fusion_card_to_host64(uint64_t val)
{
    return (((val &           0xffULL)<<56)|
            ((val &             0xff00ULL)<<40)|
            ((val &           0xff0000ULL)<<24)|
            ((val &         0xff000000ULL)<<8) |
            ((val &       0xff00000000ULL)>>8) |
            ((val &     0xff0000000000ULL)>>24)|
            ((val &   0xff000000000000ULL)>>40)|
            ((val & 0xff00000000000000ULL)>>56)); 
}

// the following is probably more efficient, but has not been plessed by Jer yet
#if 0
static inline uint64_t fusion_card_to_host64(uint64_t val)
{
    uint64_t tmp;
    tmp = ((val << 8) & 0xff00ff00ff00ff00ULL) | ((val >> 8) & 0x00ff00ff00ff00ffULL);
    tmp = ((tmp << 16) & 0xffff0000ffff0000ULL) | ((tmp >> 16) & 0x0000ffff0000ffffULL);
    return (((tmp << 32) & 0xffffffff00000000ULL) | ((tmp >> 32) & 0x00000000ffffffffULL));
}
#endif


static inline uint32_t fusion_card_to_host32(uint32_t val)
{
    return (((val &   0xff)<<24) |
            ((val &     0xff00)<<8)  |
            ((val &   0xff0000)>>8)  |
            ((val & 0xff000000)>>24)); 
}

static inline uint16_t fusion_card_to_host16(uint16_t val)
{
    return (((val &   0xff)<<8) |
            ((val &     0xff00)>>8)); 
}

#define host_to_fusion_card64(x) fusion_card_to_host64(x)
#define host_to_fusion_card32(x) fusion_card_to_host32(x)
#define host_to_fusion_card16(x) fusion_card_to_host16(x)
#endif // __PPC*__
#endif // __KERNEL__

#define MD_SETTER_NOCHK(field,p,v,ww)   (p)->field = host_to_fusion_card##ww(v)
#define MD_SETTER(field,p,v,bits,ww)            \
    do{                                         \
        kassert((uint64_t)v<(1ULL<<(bits)));    \
        MD_SETTER_NOCHK(field,p,v,ww);          \
    } while(0)
#define MD_GETTER(field,p,t,ww)         ((t)fusion_card_to_host##ww((p)->field))

#define NMASK(n) ((1ULL<<(n))-1)

#define MD_SETTER_BF(field,p,v,bits,ww,off,len)         \
    do{                                                 \
        uint64_t val = MD_GETTER(field,p,uint64_t,ww);  \
        kassert((uint64_t)v<(1ULL<<(bits)));            \
        val &= ~(NMASK(len) << (off));                  \
        val |= ((uint64_t)(v) << (off));                \
        MD_SETTER_NOCHK(field,p,val,ww);                \
    } while(0)
#define MD_GETTER_BF(field,p,t,ww,off,len)                          \
    (t)((MD_GETTER(field,p,uint64_t,ww) >> (off)) & (NMASK(len)))

typedef void    *fusion_kthread_t;

typedef int (*fusion_kthread_func_t)(void *);

//nand_device can be NULL in the following fxn call
void fusion_create_kthread(fusion_kthread_func_t func, void *data, void *fusion_nand_device,
                           const char *fmt, ...);

////#define kfio_page_address(pg)          page_address(pg)

#if defined(__x86_64__)
/** XXX this is mb */
#define kfio_barrier()          asm volatile("mfence":::"memory")
#else
#define kfio_barrier()          __sync_synchronize()
#endif

typedef struct
{
#if FUSION_DEBUG_MEMORY
#define REF_COUNT_MAGIC    0xCAFEF00D
    uint32_t magic;
#endif
    int count;
} fusion_ref_count_t;

// runtime initialize to 1
static inline void fusion_init_ref_count(fusion_ref_count_t *cnt)
{
#if FUSION_DEBUG_MEMORY
    cnt->magic = REF_COUNT_MAGIC;
#endif
    fusion_atomic_set(&cnt->count, 1);
}

static inline int fusion_inc_ref_count(fusion_ref_count_t *cnt)
{
    return fusion_atomic_incr(&cnt->count);
}

static inline int fusion_get_ref_count(fusion_ref_count_t *cnt)
{
    return fusion_atomic_read(&cnt->count);
}

// returns 1 if decremented to zero, returns 0 otherwise
static inline int fusion_dec_ref_count_is_zero(fusion_ref_count_t *cnt)
{
    int count;

    count =  fusion_atomic_decr(&cnt->count);
    //Reference count must never decrement past zero
    kassert(count >= 0);
    return (0 == count);
}

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

#if !defined(__PPC64__) && !defined(__mips64)
typedef unsigned int fusion_bits_t;
#else
typedef unsigned long fusion_bits_t;
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
#else
/* Linux port implements kinfo backend using sysctl. */
#define KFIO_INFO_USE_OS_BACKEND 1
#endif

#endif
