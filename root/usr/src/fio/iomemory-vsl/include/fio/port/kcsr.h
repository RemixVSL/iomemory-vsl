//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

extern uint32_t kfio_csr_read_direct(volatile void *addr, void *hdl);
extern uint64_t kfio_csr_read_direct_64(volatile void *addr, void *hdl);
extern void kfio_csr_write_nobarrier(uint32_t val, volatile void *addr, void *hdl);
extern void kfio_csr_write(uint32_t val, volatile void *addr, void *hdl);
extern void kfio_csr_write_64(uint64_t val, volatile void *addr, void *hdl);

