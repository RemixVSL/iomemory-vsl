//-----------------------------------------------------------------------------
// Copyright (c) 2012-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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
// -----------------------------------------------------------------------------
#ifndef __FIO_PORT_VECTORED_H__
#define __FIO_PORT_VECTORED_H__

// Flags that determine the type of atomic operation. Currently, only
// atomic write and trim are supported.
#define FIOV_ATOMIC_WRITE 0x01
#define FIOV_ATOMIC_TRIM  0x02

/*
 * Struct used for atomics operations through the kernel api. This struct
 * is declared here rather than in another header so that external kernel
 * modules can directly include this header to get the struct definition.
 */
struct kfio_iovec
{
    void      *iov_base;       ///< The starting address of user buffer
    size_t     iov_len;        ///< The length of the data in user buffer
    uint64_t   iov_sector;     ///< Sector base
    uint32_t   iov_flag;       ///< Extra flags defining operation type
    uint32_t   iov_reserved;   ///< Reserved for alignment purpose

};
#endif /*__FIO_PORT_VECTORED_H__ */


