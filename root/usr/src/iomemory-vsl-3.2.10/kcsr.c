//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates. All rights reserved.
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
#if defined(__KERNEL__)
#include "port-internal.h"
#include <asm/io.h>
#else
#include <fio/port/utypes.h>
#include <fio/port/ufio.h>

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
    uint64_t v = __raw_readq(addr);
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

