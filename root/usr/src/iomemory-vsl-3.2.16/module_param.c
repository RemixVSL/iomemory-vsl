
//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc.
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the Fusion-io, Inc. nor the names of its contributors
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
#include <linux/module.h>

#include <fio/port/kfio_config.h>

#include "fio/port/kfio.h"

KFIO_MODULE_PARAM(strict_sync, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(strict_sync, "Force strict write flushing on early non-powercut safe cards. (1=enable, 0=disable, -1=auto) Do not change for newer cards.");
KFIO_MODULE_PARAM(disable_msix, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(disable_msix, "N/A");
KFIO_MODULE_PARAM(bypass_ecc, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(bypass_ecc, "N/A");
KFIO_MODULE_PARAM(groomer_low_water_delta_hpcnt, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(groomer_low_water_delta_hpcnt, "The proportion of logical space free over the runway that represents 'the wall'");
KFIO_MODULE_PARAM(disable_scanner, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(disable_scanner, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(use_command_timeouts, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(use_command_timeouts, "Use the command timeout registers");
KFIO_MODULE_PARAM(use_large_pcie_rx_buffer, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(use_large_pcie_rx_buffer, "If true, use 1024 byte PCIe rx buffer. This improves performance but causes NMIs on some specific hardware.");
KFIO_MODULE_PARAM(rsort_memory_limit_MiB, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(rsort_memory_limit_MiB, "Memory limit in MiBytes for rsort rescan.");
KFIO_MODULE_PARAM(capacity_warning_threshold, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(capacity_warning_threshold, "If the reserve space is below this threshold (in hundredths of percent), warnings will be issued.");
KFIO_MODULE_PARAM(enable_unmap, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(enable_unmap, "Enable UNMAP support.");
KFIO_MODULE_PARAM(fio_qos_enable, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(fio_qos_enable, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(fio_dev_wait_timeout_secs, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(fio_dev_wait_timeout_secs, "Number of seconds to wait for device file creation before continuing.");
KFIO_MODULE_PARAM(iodrive_load_eb_map, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(iodrive_load_eb_map, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(use_new_io_sched, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(use_new_io_sched, "N/A");
KFIO_MODULE_PARAM(groomer_high_water_delta_hpcnt, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(groomer_high_water_delta_hpcnt, "The proportion of logical space over the low watermark where grooming starts (in ten-thousandths)");
KFIO_MODULE_PARAM(preallocate_mb, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(preallocate_mb, "The megabyte limit for FIO_PREALLOCATE_MEMORY. This will prevent the driver from potentially using all of the system's non-paged memory.");
KFIO_MODULE_PARAM_STRING(exclude_devices, exclude_devices, sizeof(exclude_devices), S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(exclude_devices, "List of cards to exclude from driver initialization (comma separated list of <domain>:<bus>:<slot>.<func>)");
KFIO_MODULE_PARAM(parallel_attach, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(parallel_attach, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(enable_ecc, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(enable_ecc, "N/A");
KFIO_MODULE_PARAM(rmap_memory_limit_MiB, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(rmap_memory_limit_MiB, "Memory limit in MiBytes for rmap rescan.");
KFIO_MODULE_PARAM(expected_io_size, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(expected_io_size, "Timeout for data log compaction while shutting down.");
KFIO_MODULE_PARAM(tintr_hw_wait, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(tintr_hw_wait, "N/A");
KFIO_MODULE_PARAM(iodrive_load_midprom, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(iodrive_load_midprom, "Load the midprom");
KFIO_MODULE_PARAM(auto_attach_cache, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(auto_attach_cache, "Controls directCache behavior after an unclean shutdown: 0 = disable (cache is discarded and manual rebinding is necessary), 1 = enable (default).");
KFIO_MODULE_PARAM(retire_on_psub, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(retire_on_psub, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(use_modules, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(use_modules, "Number of NAND modules to use");
KFIO_MODULE_PARAM(groom_harder_threshold, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(groom_harder_threshold, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(make_assert_nonfatal, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(make_assert_nonfatal, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(enable_discard, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(enable_discard, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(max_md_blocks_per_device, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(max_md_blocks_per_device, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM(force_soft_ecc, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(force_soft_ecc, "Forces software ECC in all cases");
KFIO_MODULE_PARAM(max_requests, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(max_requests, "How many requests pending in iodrive");
KFIO_MODULE_PARAM(global_slot_power_limit_mw, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(global_slot_power_limit_mw, "Global PCIe slot power limit in milliwatts. Performance will be throttled to not exceed this limit in any PCIe slot.");
KFIO_MODULE_PARAM(read_pipe_depth, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(read_pipe_depth, "Max number of read requests outstanding in hardware.");
KFIO_MODULE_PARAM(force_minimal_mode, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(force_minimal_mode, "N/A");
KFIO_MODULE_PARAM(auto_attach, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(auto_attach, "Automatically attach drive during driver initialization: 0 = disable attach, 1 = enable attach (default). Note for Windows only: The driver will only attach if there was a clean shutdown, otherwise the fiochkdrv utility will perform the full scan attach, 2 = Windows only: Forces the driver to do a full rescan (if needed).");
KFIO_MODULE_PARAM_STRING(external_power_override, external_power_override, sizeof(external_power_override), S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(external_power_override, "Override external power requirement on boards that normally require it. (comma-separated list of adapter serial numbers)");
KFIO_MODULE_PARAM(compaction_timeout_ms, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(compaction_timeout_ms, "Timeout in ms for data log compaction while shutting down.");
KFIO_MODULE_PARAM(disable_groomer, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(disable_groomer, "For use only under the direction of Customer Support.");
KFIO_MODULE_PARAM_STRING(include_devices, include_devices, sizeof(include_devices), S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(include_devices, "Whitelist of cards to include in driver initialization (comma separated list of <domain>:<bus>:<slot>.<func>)");
KFIO_MODULE_PARAM_STRING(preallocate_memory, preallocate_memory, sizeof(preallocate_memory), S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(preallocate_memory, "Causes the driver to pre-allocate the RAM it needs");
KFIO_MODULE_PARAM(fio_dev_optimal_blk_size, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(fio_dev_optimal_blk_size, "Optimal block size hint for the linux block layer.");
KFIO_MODULE_PARAM(disable_msi, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(disable_msi, "N/A");
KFIO_MODULE_PARAM(scsi_queue_depth, int, S_IRUGO | S_IWUSR);
KFIO_MODULE_PARAM_DESC(scsi_queue_depth, "The queue depth that is advertised to the OS SCSI interface.");
