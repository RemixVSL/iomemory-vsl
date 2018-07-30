//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2017 SanDisk Corp. and/or all its affiliates. (acquired by Western Digital Corp. 2016)
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

#include <linux/types.h>
#include <linux/module.h>
#include <fio/port/dbgset.h>
#include <fio/port/fio-port.h>
#include <fio/port/kfio_config.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/list.h>
#include <linux/notifier.h>
#include <linux/version.h>

#if !defined KFIOC_DISCARD
#error kfioconfig not included
#endif


/**
 * @ingroup PORT_LINUX
 * @{
 */

#if defined CONFIG_SMP && PORT_SUPPORTS_PER_CPU

#ifdef DEFINE_SPINLOCK
static DEFINE_SPINLOCK(hotplug_lock);
#else
static spinlock_t hotplug_lock = SPIN_LOCK_UNLOCKED;
#endif
static int hotplug_initialized;
static LIST_HEAD(notify_list);

struct kfio_cpu_notify
{
    struct list_head list;
    kfio_cpu_notify_fn *func;
};

static void send_event_all(int online_flag, kfio_cpu_t cpu)
{
    struct kfio_cpu_notify *kcn;

    spin_lock_irq(&hotplug_lock);

    list_for_each_entry(kcn, &notify_list, list)
    {
        kcn->func(online_flag, cpu);
    }

    spin_unlock_irq(&hotplug_lock);
}

// Kernel 4.8 introduced the hotplug state machine, but without the callback state for offline,
// Kernel 4.10 has hotplug state machine with callback states for both online and offline.
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,10,0)
static int notify_fn(struct notifier_block *self, unsigned long action, void *hcpu)
{
    switch (action)
    {
        case CPU_DEAD:
#ifdef CPU_DEAD_FROZEN
        case CPU_DEAD_FROZEN:
#endif
            send_event_all(0, (kfio_cpu_t)(unsigned long)hcpu);
            break;

        case CPU_ONLINE:
#ifdef CPU_ONLINE_FROZEN
        case CPU_ONLINE_FROZEN:
#endif
            send_event_all(1, (kfio_cpu_t)(unsigned long)hcpu);
            break;
    }

    return NOTIFY_OK;
}

static struct notifier_block kfio_linux_cpu_notifier =
{
    .notifier_call = notify_fn,
};
#else
static int kfio_cpu_notify_offline(unsigned int cpu)
{
    send_event_all(0, (kfio_cpu_t)cpu);
    return 0;
}
static int kfio_cpu_notify_online(unsigned int cpu)
{
    send_event_all(1, (kfio_cpu_t)cpu);
    return 0;
}
#endif
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
        // Fancy footwork to handle various kernel idiosyncracies...
        // Kernels less than 4.8 only use register_cpu_notifier().
        // Kernel 4.8 provides both register_cpu_notifier() and a new function cpuhp_setup_state() for setting
        //  a callback for a cpu coming online. However, our notifier/callback function requires a lower hotplug state
        //  than the provided CPUHP_AP_ONLINE_DYN to work without modification (log messages are wrong).
        //  So we use cpuhp_setup_state() for setting an 'online' callback but use the old register_cpu_notifier()
        //  for 'offline'.
        // Kernel 4.10 and above have symmetrical CPUHP_..._DYN states for online and offline callbacks.


        // Unfortunately, in kernels 4.8 through 4.12 the cpuhp_remove_state() function does not properly remove the
        //  first callback state in either DYN callback array. We apparently are the only one using the
        //  CPUHP_BP_PREPARE_DYN array, and so attempting to remove our offline callback failed silently, and removing
        //  our driver subsequently left the callbacks in the array pointing to bogus functions, causing page faults.
        // To work around this, set up a state with NULL callback functions and if it is _not_ the first state in
        //  the array then remove that state and set the real state with real callbacks.
        //  If our state _is_ the first item in the array and this kernel has the bug then leave the NULL callbacks
        //  entry in the array and _also_ add another state with the real callbacks.
        // When removing the callbacks states just remove the non-NULL state. The NULL callbacks simply won't be called.

        // Whether kernel has states removal bug or not, now setup our real callback.

        // My (enb) testing didn't show the AP state having the same last-state-removal problem...but that's because
        //  my AP state wasn't the first AP state in the array.
        // So for safety we must do the same dance as with the BP DYN.


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)

        cpuhp_setup_state(CPUHP_AP_ONLINE_DYN,
                          "kfio/sandisk:online",
                          kfio_cpu_notify_online,
                          kfio_cpu_notify_offline);
        
        // If kernel is < 4.8 then must always use notifer.
        // If kernel == 4.8 then our functions require use of notifier only for offline.
#else 
        register_cpu_notifier(&kfio_linux_cpu_notifier);
#endif
    }
#endif  /* CONFIG_SMP */
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
        cpuhp_remove_state_nocalls(CPUHP_AP_ONLINE_DYN);
#else
        unregister_cpu_notifier(&kfio_linux_cpu_notifier);
#endif
    }

#endif  /* CONFIG_SMP */
}

#endif

/**
 * @}
 */
