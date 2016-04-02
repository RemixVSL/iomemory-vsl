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

#ifndef __FIO_PORT_KCSR_H__
#define __FIO_PORT_KCSR_H__

struct kfio_csr_handle
{
    volatile void *bar_virt;
    volatile struct iodrive_CSR *csr_virt;
    volatile struct iodrive_nwlogic_s2c_registers *dma_s2c_virt;
    volatile struct iodrive_nwlogic_c2s_registers *dma_c2s_virt;
    volatile struct iodrive_common_DMA_CSR *dma_common_virt;
    void *csr_hdl;
    uint8_t indirect_access;
};

extern uint32_t kfio_csr_read_direct(volatile void *addr, void *hdl);
extern uint64_t kfio_csr_read_direct_64(volatile void *addr, void *hdl);
extern void kfio_csr_write_nobarrier(uint32_t val, volatile void *addr, void *hdl);
extern void kfio_csr_write(uint32_t val, volatile void *addr, void *hdl);
extern void kfio_csr_write_64(uint64_t val, volatile void *addr, void *hdl);

#endif /* __FIO_PORT_KCSR_H__ */
