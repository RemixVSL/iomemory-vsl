//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------
#if !defined (__linux__)
#error This file supports linux only
#endif

#include <linux/module.h>

#include <fio/port/linux/kfio_config.h>

#include "fio/port/kfio.h"

#define AUTO_ATTACH_DEFAULT 1

// Global driver options
const char *exclude_devices[MAX_PCI_DEVICES] = {0};
const char *include_devices[MAX_PCI_DEVICES] = {0};

#if KFIOC_USE_IO_SCHED
int use_workqueue = USE_QUEUE_RQ;
#else
int use_workqueue = USE_QUEUE_NONE; ///< if non-zero will queue all bio requests and return as quickly as possible.
#endif

int fio_dont_init = 0;

module_param (enable_ecc, int, S_IRUGO | S_IWUSR);
module_param (enable_two_plane, int, S_IRUGO | S_IWUSR);
module_param (disable_msi, int, S_IRUGO | S_IWUSR);
module_param (bypass_ecc, int, S_IRUGO | S_IWUSR);
module_param (force_soft_ecc, int, S_IRUGO | S_IWUSR);
module_param (bypass_whitening, int, S_IRUGO | S_IWUSR);
module_param (dont_whiten, int, S_IRUGO | S_IWUSR);
module_param (force_sw_read_completions, int, S_IRUGO | S_IWUSR);
module_param (ecc_correction_target_threshold, int, S_IRUGO | S_IWUSR);
module_param (ecc_correction_per_pad_retire, int, S_IRUGO | S_IWUSR);
module_param (ecc_correction_retire_threshold, int, S_IRUGO | S_IWUSR);

int preallocate_memory_count = 0;                       // HACK: Because the module_param_array
char *preallocate_memory[INT_ARRAY_MAX_ELEMENTS] = {0}; // does not allow structure members to be passed due
                                                        // to token concatination and token expansion to string literals.
                                                        // Possibly change later.
#if !defined(__VMKLNX__) && !defined(__TENCENT__)
#if KFIOC_MODULE_PARAM_ARRAY_NUMP
module_param_array (preallocate_memory, charp, &preallocate_memory_count, S_IRUGO | S_IWUSR);
#else
module_param_array (preallocate_memory, charp, preallocate_memory_count, S_IRUGO | S_IWUSR);
#endif
MODULE_PARM_DESC(preallocate_memory, "Cards for which to preallocate memory\n(card <serial> serial number required)");
#endif

extern int preallocate_mb;
module_param (preallocate_mb, int, S_IRUGO | S_IWUSR);

extern int expected_io_size;
module_param (expected_io_size, int, S_IRUGO | S_IWUSR);

module_param (iodrive_load_midprom, int, S_IRUGO | S_IWUSR);

module_param (iodrive_load_eb_map, int, S_IRUGO | S_IWUSR);

module_param (iodrive_scan_nv_data, int, S_IRUGO | S_IWUSR);

module_param (tcmd0, int, S_IRUGO | S_IWUSR);
module_param (tcmd1, int, S_IRUGO | S_IWUSR);
module_param (tread0, int, S_IRUGO | S_IWUSR);
module_param (tread1, int, S_IRUGO | S_IWUSR);
module_param (twrite0, int, S_IRUGO | S_IWUSR);
module_param (twrite1, int, S_IRUGO | S_IWUSR);
module_param (taddr0, int, S_IRUGO | S_IWUSR);
module_param (taddr1, int, S_IRUGO | S_IWUSR);
module_param (tintr_hw_wait, int, S_IRUGO | S_IWUSR);

module_param (iodrive_tread_hw_wait , int, S_IRUGO | S_IWUSR);
module_param (iodrive_tread_hw_wait_mlc , int, S_IRUGO | S_IWUSR);
module_param (iodrive_tstat_hw_wait , int, S_IRUGO | S_IWUSR);
module_param (iodrive_tstat_hw_wait_mlc , int, S_IRUGO | S_IWUSR);

module_param (iodrive_dma_delay, int, S_IRUGO | S_IWUSR);

module_param (use_large_pcie_rx_buffer, int, S_IRUGO | S_IWUSR);

module_param (use_workqueue, int, S_IRUGO | S_IWUSR);

module_param(default_gc_low_water, int, S_IRUGO | S_IWUSR);
module_param(default_gc_high_water, int, S_IRUGO | S_IWUSR);

module_param(always_rebuild_md, int, S_IRUGO | S_IWUSR);

module_param(auto_attach, int, S_IRUGO | S_IWUSR);
module_param(auto_attach_cache, int, S_IRUGO | S_IWUSR);
module_param(parallel_attach, int, S_IRUGO | S_IWUSR);
module_param(disable_groomer, int, S_IRUGO | S_IWUSR);
module_param(groomer_backoff, int, S_IRUGO | S_IWUSR);

module_param(use_new_io_sched, int, S_IRUGO | S_IWUSR);

module_param(max_md_blocks_per_device, int, S_IRUGO | S_IWUSR);

module_param(strict_sync, int, S_IRUGO | S_IWUSR);

#if FUSION_SUPPORT_SIMULATED_DMA
module_param(iodrive_simulate_command_queuing_dma, int, S_IRUGO | S_IWUSR);
#endif

module_param(use_command_timeouts, int, S_IRUGO | S_IWUSR);

module_param(fio_dont_init, int, S_IRUGO | S_IWUSR);
module_param(max_requests, int, S_IRUGO | S_IWUSR);
module_param(reduced_write_threshold, int,  S_IRUGO | S_IWUSR);
module_param(capacity_warning_threshold, int,  S_IRUGO | S_IWUSR);
module_param(read_only_threshold, int,  S_IRUGO | S_IWUSR);
module_param(iodrive_read_retire_threshold, int,  S_IRUGO | S_IWUSR);
module_param(disable_rle, int, S_IRUGO | S_IWUSR);

module_param(force_minimal_mode, int, S_IRUGO | S_IWUSR);

module_param(flashback_mode, int, S_IRUGO | S_IWUSR);
module_param(flashback_warning_as_failure, int, S_IRUGO | S_IWUSR);

#if !defined(__VMKLNX__) && !defined(__TENCENT__)
MODULE_PARM_DESC(exclude_devices, "PCI addresses of devices to exclude during initialization\n(full <domain>:<bus>:<slot>.<func> address required)");
MODULE_PARM_DESC(include_devices, "Only initialize devices with these PCI addresses\n(full <domain>:<bus>:<slot>.<func> address required) (precedence over exclude_devices)");

#if KFIOC_MODULE_PARAM_ARRAY_NUMP
module_param_array (exclude_devices, charp, &num_exclude_devices, S_IRUGO | S_IWUSR);
module_param_array (include_devices, charp, &num_include_devices, S_IRUGO | S_IWUSR);
#else
module_param_array (exclude_devices, charp, num_exclude_devices, S_IRUGO | S_IWUSR);
module_param_array (include_devices, charp, num_include_devices, S_IRUGO | S_IWUSR);
#endif
#endif

#if !defined(__VMKLNX__)
module_param (fio_dev_wait_timeout_secs, int, S_IRUGO | S_IWUSR);
module_param (fio_dev_optimal_blk_size, int, S_IRUGO | S_IWUSR);
#endif

module_param(thermal_throttle_disable, int, S_IRUGO | S_IWUSR);
module_param(compaction_timeout_ms, int, S_IRUGO | S_IWUSR);

module_param(persistent_discard, int, S_IRUGO | S_IWUSR);

#if defined(__VMKLNX__)
// Option for ESX internal testing only.
// The IOVP test tool complains about /dev/fio node entries, so we use this
// option to register as cciss and create /dev/cciss entries instead.
module_param(register_as_cciss, int, S_IRUGO | S_IWUSR);
#endif
