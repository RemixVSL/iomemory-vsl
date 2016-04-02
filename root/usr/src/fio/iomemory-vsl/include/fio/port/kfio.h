//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_KFIO_H__
#define __FIO_PORT_KFIO_H__

#include <fio/port/config.h>
#include <fio/port/ktypes.h>
#include <fio/port/kmem.h>

#ifdef __GNUC__
#if defined(__mips64)
#include <fio/port/arch/mips_atomic.h>
#include <fio/port/arch/mips_cache.h>
#elif defined(_AIX)
#include <fio/port/arch/ppc/ppc_atomic.h>
#include <fio/port/arch/ppc/ppc_cache.h>
#elif defined(__PPC__) || defined(__PPC64__)
#include <fio/port/arch/ppc_atomic.h>
#include <fio/port/arch/ppc_cache.h>
#elif !defined(__SVR4) || !defined(__sun)
#include <fio/port/arch/x86_atomic.h>
#include <fio/port/arch/x86_cache.h>
#endif
#endif

#if defined(USERSPACE_KERNEL)
#include <fio/port/userspace/kfio.h>
#elif defined(__ESXI41__)
#include <fio/port/esxi41/kfio.h>
#elif defined(__ESXI5__)
#include <fio/port/esxi5/kfio.h>
#elif defined(__linux__)
#include <fio/port/linux/kfio.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/bitops.h>
#include <fio/port/solaris/kfio.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/kfio.h>
#elif defined(_AIX)
#include <fio/port/aix/kfio.h>
#elif defined(__OSX__)
#include <fio/port/osx/kfio.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/kfio.h>
#elif defined(EXTERNAL_OS)
#include "fio/port/external_os_kfio.h"
#else
#error Unsupported OS - if you are porting, try starting with a copy of stubs
#endif

/**
 * OS-independent declarations follow:
 */

kfio_irqreturn_t iodrive_intr_fast(int irq, void *dev_id);
extern const char *fio_device_get_bus_name(struct fio_device *dev);

/* Memory Functions */
extern int kfio_print(const char *format, ...)
#ifdef __GNUC__
__attribute__((format(printf,1,2)))
#endif
;
extern int kfio_snprintf(char *buffer, fio_size_t n, const char *format, ...)
#ifdef __GNUC__
#if defined(_AIX)  // The kernel doesn't have snprintf so we have #define-d it to sprintf
__attribute__((format(printf,2,3)))
#else
__attribute__((format(printf,3,4)))
#endif
#endif
;

extern int kfio_get_cpu(void);
extern int kfio_vprint(const char *format, va_list args);
extern int kfio_vsnprintf(char *buffer, fio_size_t n, const char *format, va_list ap);
extern int kfio_strncmp(const char *s1, const char *s2, fio_size_t n);
extern int kfio_strcmp(const char *s1, const char *s2);
extern char *kfio_strncpy(char *dst, const char *src, fio_size_t n);
extern char *kfio_strcpy(char *dst, const char *src);
extern char *kfio_strcat(char *dst, const char *src);
extern char *kfio_strncat(char *dst, const char *src, int size);
extern fio_size_t kfio_strlen(const char *s);
extern void *kfio_memset(void *dst, int c, fio_size_t n);
extern int kfio_memcmp(const void *m1, const void *m2, fio_size_t n);
extern void *kfio_memcpy(void *dst, const void *src, fio_size_t n);
extern void *kfio_memmove(void *dst, const void *src, fio_size_t n);
extern int kfio_stoi(const char *p, int *nchar);
extern long int kfio_strtol(const char *nptr, char **endptr, int base);
extern unsigned long int kfio_strtoul(const char *nptr, char **endptr, int base);
extern unsigned long long int kfio_strtoull(const char *nptr, char **endptr, int base);
extern int kfio_strspn (const char *s, const char *accept);
extern char *kfio_strpbrk(const char *cs, const char *ct);
extern char *kfio_strchr(const char *s, int c);
extern char *kfio_strtok_r(char *s, const char *del, char **sv_ptr);

extern int kfio_copy_from_user(void *to, const void *from, unsigned len);
extern int kfio_copy_to_user(void *to, const void *from, unsigned len);

extern int kfio_put_user_8(int x, uint8_t *arg);
extern int kfio_put_user_16(int x, uint16_t *arg);
extern int kfio_put_user_32(int x, uint32_t *arg);
extern int kfio_put_user_64(int x, uint64_t *arg);

extern int kfio_checkstructs(void);

extern void kfio_set_state_running(struct fio_device *dev);

/* PCI */
extern void kfio_pci_set_master(kfio_pci_dev_t *pdev);
extern int kfio_pci_request_regions(kfio_pci_dev_t *pdev, const char *res_name);
extern void kfio_pci_release_regions(kfio_pci_dev_t *pdev);
extern int kfio_pci_enable_device(kfio_pci_dev_t *pdev);
extern void kfio_pci_disable_device(kfio_pci_dev_t *pdev);
extern int kfio_pci_set_dma_mask(kfio_pci_dev_t *pdev, uint64_t mask);

extern int kfio_pci_enable_msi(kfio_pci_dev_t *pdev);
extern void kfio_pci_disable_msi(kfio_pci_dev_t *pdev);
extern const char *kfio_pci_name(kfio_pci_dev_t *pdev);

extern void *kfio_pci_get_drvdata(kfio_pci_dev_t *pdev);
extern void kfio_pci_set_drvdata(kfio_pci_dev_t *pdev, void *data);

extern void kfio_iodrive_intx (kfio_pci_dev_t *pci_dev, int enable);

extern int kfio_pci_read_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t *val);
extern int kfio_pci_read_config_word(kfio_pci_dev_t *pdev, int where, uint16_t *val);
extern int kfio_pci_read_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t *val);
extern int kfio_pci_write_config_byte(kfio_pci_dev_t *pdev, int where, uint8_t val);
extern int kfio_pci_write_config_word(kfio_pci_dev_t *pdev, int where, uint16_t val);
extern int kfio_pci_write_config_dword(kfio_pci_dev_t *pdev, int where, uint32_t val);

extern uint8_t kfio_pci_get_bus(kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_devicenum(kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_function(kfio_pci_dev_t *pdev);
extern int     kfio_ignore_device(const kfio_pci_dev_t *pdev);
extern uint8_t kfio_pci_get_slot(kfio_pci_dev_t *pdev);

extern uint16_t kfio_pci_get_vendor(kfio_pci_dev_t *pdev);
extern uint32_t kfio_pci_get_devnum(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_vendor(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_device(kfio_pci_dev_t *pdev);

struct kfio_pci_csr_handle
{
    void *csr_virt;
    void *csr_hdl;
};

extern int  kfio_pci_map_csr(kfio_pci_dev_t *pdev, struct kfio_pci_csr_handle *csr);
extern void kfio_pci_unmap_csr(kfio_pci_dev_t *pdev);

#if defined(WIN32) || defined(WINNT)

#define     KERN_EMERG          "<EMERG>"
#define     KERN_ALERT          "<ALERT>"
#define     KERN_CRIT           "<CRIT>"
#define     KERN_ERR            "<ERR>"
#define     KERN_WARNING        "<WARNING>"
#define     KERN_INFO           "<INFO>"
#define     KERN_NOTICE         "<NOTICE>"
#define     KERN_DEBUG          "<DEBUG>"

#else

#define KERN_EMERG       "<0>"       // System is unusable
#define KERN_ALERT       "<1>"       // Action must be taken immediately
#define KERN_CRIT        "<2>"       // Critical conditions
#define KERN_ERR         "<3>"       // Error conditions
#define KERN_WARNING     "<4>"       // Warning conditions
#define KERN_NOTICE      "<5>"       // Normal but significant condition
#define KERN_INFO        "<6>"       // Informational
#define KERN_DEBUG       "<7>"       // Debug-level messages

#define kfio_put_user(x, arg)                                 \
    ({                                                        \
        int pu_ret;                                           \
                                                              \
        switch (sizeof(*(arg)))                               \
        {                                                     \
        case 1:                                               \
            pu_ret = kfio_put_user_8(x, (uint8_t *)arg);      \
            break;                                            \
        case 2:                                               \
            pu_ret = kfio_put_user_16(x, (uint16_t *)arg);    \
            break;                                            \
        case 4:                                               \
            pu_ret = kfio_put_user_32(x, (uint32_t *)arg);    \
            break;                                            \
        case 8:                                               \
            pu_ret = kfio_put_user_64(x, (uint64_t *)arg);    \
            break;                                            \
        }                                                     \
                                                              \
        pu_ret;                                               \
    })

extern int kfio_request_irq(kfio_pci_dev_t *pd, const char *devname,
                            void *iodrive_dev, int msi_enabled);
extern void kfio_free_irq(kfio_pci_dev_t *pd, void *dev);
extern int kfio_pci_register_return_0_on_success(void);

extern void kfio_dump_stack(void);
extern uint32_t kfio_random_seed(void);

#endif /* !defined(WIN32) */

/* Core entry points. */
int  iodrive_pci_attach(kfio_pci_dev_t *pci_dev, int device_num, int poll);
void iodrive_pci_detach(kfio_pci_dev_t *pci_dev);
int  iodrive_pci_finish_attach(kfio_pci_dev_t *pci_dev);

#endif /* __FIO_PORT_KFIO_H__ */

