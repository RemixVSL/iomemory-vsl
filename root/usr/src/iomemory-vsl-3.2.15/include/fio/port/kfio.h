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

#ifndef __FIO_PORT_KFIO_H__
#define __FIO_PORT_KFIO_H__

#include <fio/port/port_config.h>
#include <fio/port/align.h>
#if (FUSION_INTERNAL==1)
#include <fio/internal/config.h>
#endif
#include <fio/port/ktypes.h>
#include <fio/port/kcpu.h>
#include <fio/port/kmem.h>

#if defined(USERSPACE_KERNEL)
#include <fio/port/userspace/kfio.h>
#elif defined(__linux__) || defined(__VMKLNX__)
#include <fio/port/common-linux/kfio.h>
#elif defined(__SVR4) && defined(__sun)
#include <fio/port/bitops.h>
#include <fio/port/solaris/kfio.h>
#elif defined(__hpux__)
#include <fio/port/hpux/kfio.h>
#elif defined(__FreeBSD__)
#include <fio/port/freebsd/kfio.h>
#elif defined(_AIX)
#include <fio/port/aix/kfio.h>
#elif defined(__OSX__)
#include <fio/port/osx/kfio.h>
#elif defined(WINNT) || defined(WIN32)
#include <fio/port/windows/kfio.h>
#elif defined(UEFI)
#include <fio/port/uefi/kfio.h>
#else
#error Unsupported OS - if you are porting, try starting with a copy of stubs
#endif

/**
 * OS-independent declarations follow:
 */

kfio_irqreturn_t iodrive_intr_fast(int irq, void *dev_id);
kfio_irqreturn_t iodrive_intr_fast_pipeline(int irq, void *dev_id);
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

extern int kfio_vprint(const char *format, va_list args);
extern int kfio_vsnprintf(char *buffer, fio_size_t n, const char *format, va_list ap);
extern int kfio_strncmp(const char *s1, const char *s2, fio_size_t n);
extern int kfio_strcmp(const char *s1, const char *s2);
extern char *kfio_strncpy(char *dst, const char *src, fio_size_t n);
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
extern char *kfio_strpbrk(char *cs, const char *ct);
extern char *kfio_strchr(const char *s, int c);
extern char *kfio_strtok_r(char *s, const char *del, char **sv_ptr);

extern int kfio_copy_from_user(void *to, const void *from, unsigned len);
extern int kfio_copy_to_user(void *to, const void *from, unsigned len);

extern int kfio_put_user_8(int x, uint8_t *arg);
extern int kfio_put_user_16(int x, uint16_t *arg);
extern int kfio_put_user_32(int x, uint32_t *arg);
extern int kfio_put_user_64(int x, uint64_t *arg);

extern int kfio_checkstructs(void);

extern void kfio_set_dev_state_running(struct fio_device *dev);
extern void kfio_set_dev_state_quiescing(struct fio_device *dev);

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
extern uint8_t kfio_pci_get_slot(kfio_pci_dev_t *pdev);

extern uint16_t kfio_pci_get_vendor(kfio_pci_dev_t *pdev);
extern uint32_t kfio_pci_get_devnum(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_vendor(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_subsystem_device(kfio_pci_dev_t *pdev);
extern uint16_t kfio_pci_get_domain(kfio_pci_dev_t *pdev);

struct fusion_nand_channel;
struct kfio_pci_csr_handle
{
    void *csr_virt;
    void *csr_hdl;
};

extern int  kfio_pci_map_csr(kfio_pci_dev_t *pdev, const char *device_label, struct kfio_pci_csr_handle *csr);
extern void kfio_pci_unmap_csr(kfio_pci_dev_t *pdev, struct kfio_pci_csr_handle *csr);

#if !defined(WIN32) && !defined(WINNT)

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

#if defined(PORT_SUPPORTS_MSIX) && defined(__KERNEL__) && !defined(USERSPACE_KERNEL)
extern int kfio_request_msix(kfio_pci_dev_t *pd, const char *devname,
                             void *iodrive_dev, kfio_msix_t *msix,
                             unsigned int vector);
extern void kfio_free_msix(kfio_msix_t *msix, unsigned int vector, void *dev);
extern unsigned int kfio_pci_enable_msix(kfio_pci_dev_t *pdev, kfio_msix_t *msix, unsigned int nr_vecs);
extern void kfio_pci_disable_msix(kfio_pci_dev_t *pdev);
#else
typedef struct kfio_msix {
    DECLARE_RESERVE(1); // UEFI compiler does not like arrays of zero sized objects
} kfio_msix_t;

static inline int kfio_request_msix(kfio_pci_dev_t *pd, const char *devname,
                                    void *iodrive_dev, kfio_msix_t *msix,
                                    unsigned int vector)
{
    return -EINVAL;
}
static inline int kfio_pci_enable_msix(kfio_pci_dev_t *pdev, kfio_msix_t *msix,
                                       unsigned int nr_vecs)
{
    return 0;
}
static inline void kfio_pci_disable_msix(kfio_pci_dev_t *pdev)
{
}
static inline void kfio_free_msix(kfio_msix_t *msix, unsigned int vector,
                                  void *dev)
{
}
#endif /* PORT_SUPPORTS_MSIX */

extern void kfio_dump_stack(void);
extern uint32_t kfio_random_seed(void);

#endif /* !defined(WIN32) */

extern int kfio_get_next_device_number(kfio_pci_dev_t *pci_dev);

extern int kfio_get_irq_number(kfio_pci_dev_t *pd, uint32_t *irq);

#if defined(PORT_SUPPORTS_MSIX)
extern int kfio_get_msix_number(void *msix, uint32_t vec_ix, uint32_t *irq);
#else
static inline int kfio_get_msix_number(void *msix, uint32_t vec_ix, uint32_t *irq)
{
    return -ENOENT;
}
#endif

/* Core entry points. */
int  iodrive_pci_attach(kfio_pci_dev_t *pci_dev, int device_num);
void iodrive_pci_detach(kfio_pci_dev_t *pci_dev);

extern void iodrive_pci_clear_errors(kfio_pci_dev_t *pci_dev);

// Default maximum formatted volume capacity in bytes
#ifndef PORT_FORMATTED_CAPACITY_BYTES_MAX
#define PORT_FORMATTED_CAPACITY_BYTES_MAX KFIO_UINT64_MAX
#endif

#endif /* __FIO_PORT_KFIO_H__ */

