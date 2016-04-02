
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

#ifndef _FIO_PORT_LINUX_UFIO_H_
# define _FIO_PORT_LINUX_UFIO_H_

# ifndef _FIO_PORT_UFIO_H_
#  error Do not include this file directly - instead include <fio/port/ufio.h>
# endif

# define UFIO_CONTROL_DEVICE_PATH "/dev/"

# define UFIO_KINFO_ROOT "fusion"

# define UFIO_MODPARAMS_DIR "/sys/module/" FIO_DRIVER_NAME "/parameters"

# define _FILE_OFFSET_BITS 64

# if !defined(__KERNEL__)
#  include <stdint.h>
#  include <stdlib.h>

#  if FUSION_DEBUG

#if defined(__x86_64__) || defined(__i386__)
#   define kassert(x)   do { if(!(x)) { fprintf(stderr, "ASSERT FAILED: %s:%d %s\n", __FILE__, __LINE__, #x); asm("int3"); } } while(0)
#else
#   define kassert(x)   do { if(!(x)) { fprintf(stderr, "ASSERT FAILED: %s:%d %s\n", __FILE__, __LINE__, #x); exit(-1); } } while(0)
#endif
#   define kdebug(x) x

#  else

#   define kassert(x)   do { } while(0)
#   define kdebug(x)    do { } while(0)

#  endif

# endif /* !defined(__KERNEL__) */

#endif /* _FIO_PORT_LINUX_UFIO_H_ */
