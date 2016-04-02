//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if defined(__KERNEL__)
#include "port-internal.h"
#include <asm/io.h>
#else
#include <fio/port/utypes.h>

#define __raw_readl(addr) (*(volatile uint32_t *)(addr))
#define __raw_writel(val, addr) (*(volatile uint32_t *)(addr) = (val))

#define __raw_readq(addr) (*(volatile uint64_t *)(addr))
#define __raw_writeq(val, addr) (*(volatile uint64_t *)(addr) = (val))

#endif // __KERNEL__

/** @brief in kernel space, the hdl is the handle to access the 
 * mapped PCI memory, (can be NULL in Linux).  
 * In user space Linux allow direct access to mapped PCI memory and so
 * perfroms like in kernel space.  For other OSes where such access is 
 * not allowed, the hdl is a pointer to a file handle and the 
 * kfio_csr_read / write() calls perform an ioctl to the required driver
 * (which may be in minimal mode).
 */

// We have these functions because htonl can repeatedly evaluate its argument, which is very, very bad
uint32_t kfio_csr_read_direct(volatile void *addr, void *hdl)
{
    uint32_t v = __raw_readl(addr);
    v = fusion_card_to_host32(v);

    return v;
}
uint64_t kfio_csr_read_direct_64(volatile void *addr, void *hdl)
{
    uint64_t v;

    v = __raw_readq(addr);
    v = fusion_card_to_host64(v);

    return v;
}

void kfio_csr_write_nobarrier(uint32_t val, volatile void *addr, void *hdl)
{
    val = host_to_fusion_card32(val);

    __raw_writel(val, addr);
}

void kfio_csr_write(uint32_t val, volatile void *addr, void *hdl)
{
    kfio_csr_write_nobarrier(val, addr, hdl);
#if defined(__KERNEL__)
    kfio_barrier();
#endif
}

void kfio_csr_write_64(uint64_t val, volatile void *addr, void *hdl)
{
    val = host_to_fusion_card64(val);

    __raw_writeq(val, addr);

#if defined(__KERNEL__)
    kfio_barrier();
#endif
}

