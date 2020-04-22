//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2016 SanDisk Corp. and/or all its affiliates. (acquired by Western Digital Corp. 2016)
// Copyright (c) 2016-2017 Western Digital Technologies, Inc. All rights reserved.
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

#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/types.h>
#include <linux/module.h>
#include <fio/port/dbgset.h>
#include <fio/port/ktypes.h>
#include <fio/port/kcpu.h>
#include <linux/stddef.h>
#include <linux/cpumask.h>
#include <linux/kthread.h>

#include <fio/port/kfio_config.h>
#include <linux/kallsyms.h>

/**
 * @ingroup PORT_COMMON_LINUX
 * @{
 */

/**
 *  @brief returns current CPU.
 *  @note Some core code assumes that returning a non-zero CPU number implies support for
 *    per CPU completions.
 */
kfio_cpu_t kfio_current_cpu(void)
{
    return raw_smp_processor_id();
}

kfio_cpu_t kfio_get_cpu(kfio_get_cpu_t *flags)
{
    local_irq_save(*flags);
    return smp_processor_id();
}

void kfio_put_cpu(kfio_get_cpu_t *flags)
{
    local_irq_restore(*flags);
}

/**
 *  @brief returns maximum number of CPUs.
 *  @note Some code assumes a return value greater than 1 implies support for per
 *    CPU completions.
 */
unsigned int kfio_max_cpus(void)
{
    return num_possible_cpus();
}

/**
 *  @brief returns nonzero if indicated CPU is online.
 */
int kfio_cpu_online(kfio_cpu_t cpu)
{
    return cpu_online(cpu);
}

#if PORT_SUPPORTS_PER_CPU
void kfio_create_kthread_on_cpu(fusion_kthread_func_t func, void *data,
                                void *fusion_nand_device, kfio_cpu_t cpu,
                                const char *fmt, ...)
{
    struct task_struct *task;
    va_list ap;
    char buffer[40];

    if (!cpu_online(cpu))
    {
        return;
    }

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    task = kthread_create(func, data, "%s", buffer);
    if (IS_ERR(task))
    {
        return;
    }

    kthread_bind(task, cpu);
    wake_up_process(task);
}
#endif

// #if PORT_SUPPORTS_PCI_NUMA_INFO
#if KFIOC_NUMA_MAPS
static void __kfio_bind_task_to_cpumask(struct task_struct *tsk, cpumask_t *mask)
{
#if KFIOC_X_TASK_HAS_CPUS_MASK
    tsk->cpus_mask = *mask;
#else
    tsk->cpus_allowed = *mask;
#endif
}
#endif
/*
 * Will take effect on next schedule event
 */
void kfio_bind_kthread_to_node(kfio_numa_node_t node)
{
#if KFIOC_NUMA_MAPS
    if (node != FIO_NUMA_NODE_NONE)
    {
        cpumask_t *cpumask = (cpumask_t *) cpumask_of_node(node);

        if (cpumask &&
            cpumask_weight(cpumask)
            )
            __kfio_bind_task_to_cpumask(current, cpumask);
    }
#endif
}

/**
 * @}
 */
