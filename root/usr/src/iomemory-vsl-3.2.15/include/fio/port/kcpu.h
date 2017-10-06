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

#ifndef __FIO_PORT_KTYPES_H__
#error "Please include <fio/port/ktypes.h> rather than this file directly."
#endif
#ifndef __FIO_PORT_KCPU_H__
#define __FIO_PORT_KCPU_H__

extern kfio_cpu_t kfio_current_cpu(void);

#if defined(PORT_SUPPORTS_PER_CPU) && PORT_SUPPORTS_PER_CPU
extern kfio_cpu_t kfio_get_cpu(kfio_get_cpu_t *flags);
extern void kfio_put_cpu(kfio_get_cpu_t *flags);
extern unsigned int kfio_max_cpus(void);
extern int kfio_cpu_online(kfio_cpu_t cpu);

void kfio_create_kthread_on_cpu(fusion_kthread_func_t func, void *data,
                            void *fusion_nand_device, kfio_cpu_t cpu,
                            const char *fmt, ...);

typedef void (kfio_cpu_notify_fn)(int new_cpu_flag, kfio_cpu_t cpu);
extern void kfio_unregister_cpu_notifier(kfio_cpu_notify_fn *func);
extern int kfio_register_cpu_notifier(kfio_cpu_notify_fn *func);
#endif

#if defined(PORT_SUPPORTS_PCI_NUMA_INFO) && PORT_SUPPORTS_PCI_NUMA_INFO
extern kfio_numa_node_t kfio_pci_get_node(kfio_pci_dev_t *pdev);
void kfio_bind_kthread_to_node(kfio_numa_node_t node);
#else
static inline kfio_numa_node_t kfio_pci_get_node(kfio_pci_dev_t *pdev)
{
    return 0;
}
static inline void kfio_bind_kthread_to_node(kfio_numa_node_t node)
{
}
#endif

#if defined(PORT_SUPPORTS_NUMA_NODE_OVERRIDE) && PORT_SUPPORTS_NUMA_NODE_OVERRIDE
extern int kfio_get_numa_node_override(kfio_pci_dev_t *pdev, const char *name,
                                       kfio_numa_node_t *nodep);
#else
static inline int kfio_get_numa_node_override(kfio_pci_dev_t *pdev, const char *name,
                                              kfio_numa_node_t *nodep)
{
    return -ENOENT;
}
#endif

extern int numa_node_forced_local;
extern kfio_cpu_t kfio_next_cpu_in_node(kfio_cpu_t last_cpu, kfio_numa_node_t node);

#endif
