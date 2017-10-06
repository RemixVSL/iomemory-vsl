//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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

#ifndef __FIO_PORT_ALIGN_H__
#define __FIO_PORT_ALIGN_H__

/*
 * This header is for the defintion of FUSION_STRUCT_ALIGN
 * and the verification macro FUSION_STRUCT_ALIGN_VERIFY.
 *
 * The proper way to use FUSION_STRUCT_ALIGN is thus:
 * struct FUSION_STRUCT_ALIGN(8) my_struct
 * {
 *     uint32_t my_32_member;
 * };
 * FUSION_STRUCT_ALIGN_VERIFY(8, struct my_struct);
 *
 * or
 * typedef struct FUSION_STRUCT_ALIGN(8) my_struct
 * {
 *     uint32_t my_32_member;
 * } my_struct_t;
 * FUSION_STRUCT_ALIGN_VERIFY(8, my_struct_t);
 *
 * Since some incorrect uses of FUSION_STRUCT_ALIGN don't produce a compile
 * error nor do they align, all common code uses of FUSION_STRUCT_ALIGN
 * should verify proper alignment by calling FUSION_STRUCT_ALIGN_VERIFY.
 *
 * As well we add DECLARE_RESERVE
 */

#if defined(__GNUC__)
#include <fio/port/gcc/align.h>
#elif defined(WINNT) || defined(WIN32) || defined(_MSC_VER)
#include <fio/port/windows/align.h>
#else
#error Unsupported compiler - needs alignment macros defined
#endif

#endif
