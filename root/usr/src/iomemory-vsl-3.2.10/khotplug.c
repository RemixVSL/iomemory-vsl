//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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

#include <linux/types.h>
#include <linux/module.h>
#include <fio/port/dbgset.h>
#include <fio/port/fio-port.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/list.h>
#include <linux/notifier.h>

#if defined CONFIG_SMP && PORT_SUPPORTS_PER_CPU

#ifdef DEFINE_SPINLOCK
static DEFINE_SPINLOCK(hotplug_lock);
#else
static spinlock_t hotplug_lock = SPIN_LOCK_UNLOCKED;
#endif
static int hotplug_initialized;
static LIST_HEAD(notify_list);

struct kfio_cpu_notify {
    struct list_head list;
    kfio_cpu_notify_fn *func;
};

static void send_event_all(int online_flag, kfio_cpu_t cpu)
{
        struct kfio_cpu_notify *kcn;

        spin_lock_irq(&hotplug_lock);

        list_for_each_entry(kcn, &notify_list, list)
            kcn->func(online_flag, cpu);

        spin_unlock_irq(&hotplug_lock);
}

static int notify_fn(struct notifier_block *self, unsigned long action,
                     void *hcpu)
{
#ifdef CPU_DEAD_FROZEN
    if (action == CPU_DEAD || action == CPU_DEAD_FROZEN)
#else
    if (action == CPU_DEAD)
#endif
    {
        send_event_all(0, (kfio_cpu_t) (unsigned long) hcpu);
    }
#ifdef CPU_ONLINE_FROZEN
    else if (action == CPU_ONLINE || action == CPU_ONLINE_FROZEN)
#else
    else if (action == CPU_ONLINE)
#endif
    {
        send_event_all(1, (kfio_cpu_t) (unsigned long) hcpu);
    }

    return NOTIFY_OK;
}

static struct notifier_block kfio_linux_cpu_notifier = {
    .notifier_call = notify_fn,
};
#endif

#if PORT_SUPPORTS_PER_CPU
int kfio_register_cpu_notifier(kfio_cpu_notify_fn *func)
{
#ifdef CONFIG_SMP
    struct kfio_cpu_notify *kcn;
    int do_register = 0;

    kcn = kmalloc(sizeof(*kcn), GFP_KERNEL);
    if (!kcn)
        return -ENOMEM;

    spin_lock(&hotplug_lock);
    kcn->func = func;
    list_add_tail(&kcn->list, &notify_list);

    if (!hotplug_initialized)
    {
        do_register = 1;
        hotplug_initialized = 1;
    }
    spin_unlock(&hotplug_lock);

    if (do_register)
    {
        register_cpu_notifier(&kfio_linux_cpu_notifier);
    }
#endif
    return 0;
}

void kfio_unregister_cpu_notifier(kfio_cpu_notify_fn *func)
{
#ifdef CONFIG_SMP
    struct kfio_cpu_notify *kcn;
    int do_unregister = 0;

    spin_lock(&hotplug_lock);

    list_for_each_entry(kcn, &notify_list, list)
    {
        if (kcn->func == func)
        {
            list_del(&kcn->list);
            kfree(kcn);
            break;
        }
    }

    if (list_empty(&notify_list))
    {
        hotplug_initialized = 0;
        do_unregister = 1;
    }
    spin_unlock(&hotplug_lock);

    /*
     * The call below aquires sleep mutex, so we need to
     * call it after unlocking hotplug_lock spinlock.
     */
    if (do_unregister != 0)
    {
        unregister_cpu_notifier(&kfio_linux_cpu_notifier);
    }

#endif
}
#endif
