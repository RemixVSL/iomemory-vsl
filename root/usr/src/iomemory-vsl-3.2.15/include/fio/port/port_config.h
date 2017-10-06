/* -----------------------------------------------------------------------------
 * Copyright (c) 2017, Fusion-io, Inc. (acquired by SanDisk Corp. 2014)
 * Copyright (c) 2014 SanDisk Corp. and/or all its affiliates.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the SanDisk Corp. nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------*/

#ifndef __GENERATED_PORT_CONFIG_H__
#define __GENERATED_PORT_CONFIG_H__
#include <fio/port/port_config_vars_externs.h>


/* Threshold 1 for ditching boring packets in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_1 15

/* Threshold 2 for ditching intersting in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_2 5

/* Threshold 3 for ditching very intersting in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_3 2

/* Threshold 4 for ditching all clean packets in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_4 1

/* Source installation directory */
#define FIO_OEM_SOURCEDIR "/usr/src/fio"

/* Build version of VSL to present SCSI LUNs. */
#define KFIO_SCSI_DEVICE 0

/* Datadir (platform independent data) installation directory */
#define FIO_DATADIR "/usr/share"

/* The major version of this build */
#define FUSION_MAJOR_VERSION "3"

/* Whether symbols are exported to the linux kernel table. */
#define DISABLE_EXPORTED_SYMBOLS 0

/* Force strict write flushing on early non-powercut safe cards. (1=enable, 0=disable, -1=auto) Do not change for newer cards. */
#define STRICT_SYNC -1

/* Internal */
#define PORT_SUPPORTS_PCI_NUMA_INFO 1

/* N/A */
#define DISABLE_MSIX 0

/* Size allocated in KB of groomer buffer */
#define FIO_GROOM_MAX_BUFFER_SIZE 16384

/* N/A */
#define BYPASS_ECC 0

/* Datadir (platform independent data) installation directory */
#define FIO_OEM_DATADIR "/usr/share/fio"

/* Under what rate (MB/s) do we consider the system idle */
#define FIO_DATA_GROOM_CONSIDER_IDLE_RATE 1

/* The name of the driver */
#define FIO_DRIVER_NAME "iomemory-vsl"

/* N/A */
#define IODRIVE_SYNC_WAIT_US 100000

/* Something to do with debugging memory */
#define FUSION_DEBUG_MEMORY 0

/* Internal */
#define PORT_SUPPORTS_NUMA_NODE_OVERRIDE 1

/* The proportion of logical space free over the runway that represents 'the wall' */
#define GROOMER_LOW_WATER_DELTA_HPCNT 1

/* For use only under the direction of Customer Support. */
#define DISABLE_SCANNER 0

/* Manual installation directory */
#define FIO_MANDIR "/usr/share/man"

/* Use the command timeout registers */
#define IODRIVE_USE_COMMAND_TIMEOUTS 1

/* Turn on code coverage */
#define FUSION_COVERAGE 0

/* If true, use 1024 byte PCIe rx buffer. This improves performance but causes NMIs on some specific hardware. */
#define USE_LARGE_PCIE_RX_BUFFER 0

/* Documentation installation directory */
#define FIO_DOCDIR "/usr/share/doc"

/* Turn on internal-only features */
#define FUSION_INTERNAL 0

/* Force setting PCIe device control register bits to prevent hang on device disable. */
#define FORCE_ENABLE_PCIE_DEVICE_CONTROL_FIX 0

/* Memory limit in MiBytes for rsort rescan. */
#define RSORT_MEMORY_LIMIT_MiB 0

/* The max number of requests the Linux request queue will accept. OS exposes as /sys/block/fioa/queue/nr_requests. */
#define DEFAULT_LINUX_MAX_NR_REQUESTS 1024

/* Causes the driver to suspend operation on failure */
#define FIO_FAIL_FAST 1

/* Set up affinity options */
#define FIO_AFFINITY ""

/* If the reserve space is below this threshold (in hundredths of percent), warnings will be issued. */
#define IODRIVE_THROTTLE_CAPACITY_WARNING_THRESHOLD 1000

/* Enable UNMAP support. */
#define FIO_SCSI_UNMAP_SUPPORTED 0

/* Allows allocation of NUMA node cache memory for requests. */
#define PORT_SUPPORTS_NUMA_NODE_CACHE_ALLOCATIONS 1

/* Yield CPU at least once every specified interval while scanning the log. */
#define LOG_SCAN_YIELD_INTERVAL_MSECS 2000

/* For use only under the direction of Customer Support. */
#define FIO_QOS_ENABLE 0

/* Binary installation directory */
#define FIO_BINDIR "/usr/bin"

/* Number of seconds to wait for device file creation before continuing. */
#define FIO_DEV_WAIT_TIMEOUT_SECS 30

/* For use only under the direction of Customer Support. */
#define IODRIVE_LOAD_EB_MAP 1

/* N/A */
#define USE_NEW_IO_SCHED 1

/* Library installation directory */
#define FIO_LIBDIR "/usr/lib"

/* The proportion of logical space over the low watermark where grooming starts (in ten-thousandths) */
#define GROOMER_HIGH_WATER_DELTA_HPCNT 160

/* How often do we wake up to check if we are idle */
#define FIO_GROOM_POLL_INTERVAL 5000

/* The megabyte limit for FIO_PREALLOCATE_MEMORY. This will prevent the driver from potentially using all of the system's non-paged memory. */
#define PREALLOCATE_MB 0

/* List of cards to exclude from driver initialization (comma separated list of <domain>:<bus>:<slot>.<func>) */
#define EXCLUDE_DEVICES ""

/* Internal */
#define PORT_SUPPORTS_USER_PAGES 1

/* For use only under the direction of Customer Support. */
#define PARALLEL_ATTACH 1

/* Counts allocs and frees in cache pools */
#define FUSION_DEBUG_CACHE 0

/* N/A */
#define ENABLE_ECC 1

/* The full version of this release */
#define FUSION_VERSION "3.2.15"

/* Memory limit in MiBytes for rmap rescan. */
#define RMAP_MEMORY_LIMIT_MiB 3100

/* Compile-in and enable the error injection subsystem. Use fio-error-inject utility to cause errors. */
#define ENABLE_ERROR_INJECTION 0

/* Timeout for data log compaction while shutting down. */
#define EXPECTED_IO_SIZE 0

/* When idle at what rate (MB/s) do we groom */
#define FIO_DATA_IDLE_GROOM_RATE 1

/* N/A */
#define IODRIVE_TINTR_HW_WAIT 0

/* Load the midprom */
#define IODRIVE_LOAD_MIDPROM 1

/* Controls directCache behavior after an unclean shutdown: 0 = disable (cache is discarded and manual rebinding is necessary), 1 = enable (default). */
#define AUTO_ATTACH_CACHE 1

/* For use only under the direction of Customer Support. */
#define RETIRE_ON_PSUB 1

/* N/A */
#define IODRIVE_DISABLE_PCIE_RELAXED_ORDERING 0

/* Number of NAND modules to use */
#define USE_MODULES -1

/* Source installation directory */
#define FIO_SOURCEDIR "/usr/src"

/* Compile the driver suitable for linking directly into the linux kernel (not a kernel module). */
#define FUSION_STATIC_KERNEL 0

/* For use only under the direction of Customer Support. */
#define FIO_GROOM_HARDER_THRESHOLD 5

/* For use only under the direction of Customer Support. */
#define IODRIVE_MAKE_ASSERT_NONFATAL 0

/* Internal */
#define PORT_SUPPORTS_SGLIST_COPY 1

/* For use only under the direction of Customer Support. */
#define ENABLE_DISCARD 1

/* Sysconfig installation directory */
#define FIO_SYSCONFDIR "/etc"

/* For use only under the direction of Customer Support. */
#define MAX_MD_BLOCKS_PER_DEVICE 0

/* Sets PCIe device control register bits to prevent hang on device disable non-Windows 7/2008 OS's. */
#define ENABLE_PCIE_DEVICE_CONTROL_FIX 1

/* How many ms between grooming the oldest LEB */
#define FIO_GROOM_OLDEST_LEB_INTERVAL 30000

/* Number of EBs the oldest groomer gets at a time (0 is disable). */
#define FIO_GROOM_OLDEST_LEB_COUNT 0

/* Forces software ECC in all cases */
#define FIO_FORCE_SOFT_ECC 0

/* Internal */
#define PORT_SUPPORTS_PER_CPU 1

/* How many requests pending in iodrive */
#define IODRIVE_NUM_REQUEST_STRUCTURES 5000

/* Global PCIe slot power limit in milliwatts. Performance will be throttled to not exceed this limit in any PCIe slot. */
#define PCIe_GLOBAL_SLOT_POWER_LIMIT 24750

/* Max number of read requests outstanding in hardware. */
#define READ_PIPE_DEPTH 32

/* How many ebs can avoid grooming after groom failure before we disable the device */
#define FIO_GROOM_HARD_MAX_EXEMPT_BLOCKS 8

/* N/A */
#define FORCE_MINIMAL_MODE 0

/* The minor version of this build */
#define FUSION_MINOR_VERSION "2"

/* Library installation directory */
#define FIO_OEM_LIBDIR "/usr/lib/fio"

/* Automatically attach drive during driver initialization: 0 = disable attach, 1 = enable attach (default). Note for Windows only: The driver will only attach if there was a clean shutdown, otherwise the fiochkdrv utility will perform the full scan attach, 2 = Windows only: Forces the driver to do a full rescan (if needed). */
#define AUTO_ATTACH 1

/* N/A */
#define IODRIVE_SYNC_DELAY_US 2000

/* Override external power requirement on boards that normally require it. (comma-separated list of adapter serial numbers) */
#define FIO_EXTERNAL_POWER_OVERRIDE ""

/* Timeout in ms for data log compaction while shutting down. */
#define COMPACTION_TIMEOUT_MS 600000

/* For use only under the direction of Customer Support. */
#define GROOMER_DISABLE 0

/* Turn on release symbol mangling */
#define FUSION_MANGLE_RELEASE_SYMBOLS 1

/* Size used in KB of groomer buffer */
#define FIO_GROOM_BUFFER_SIZE 16384

/* OEM name simplified for file naming */
#define FIO_OEM_NAME "fusionio"

/* Include installation directory */
#define FIO_INCLUDEDIR "/usr/include"

/* Turn on debugging */
#define FUSION_DEBUG 0

/* Whitelist of cards to include in driver initialization (comma separated list of <domain>:<bus>:<slot>.<func>) */
#define INCLUDE_DEVICES ""

/* Causes the driver to pre-allocate the RAM it needs */
#define FIO_PREALLOCATE_MEMORY ""

/* installation prefix */
#define FIO_PREFIX "/usr"

/* OEM short name simplified for file naming */
#define FIO_OEM_NAME_SHORT "fio"

/* Enables logging of LBA(s) resident in an LEB that fails grooming */
#define FIO_GROOM_LOG_LBA_IN_FAILED_LEB 0

/* Optimal block size hint for the linux block layer. */
#define FIO_DEV_BLK_OPTIMAL_BLK_SIZE 32768

/* Maximum number of erase blocks to search for the LEB map. */
#define IODRIVE_MAX_EBMAP_PROBE 100

/* N/A */
#define DISABLE_MSI 0

/* The queue depth that is advertised to the OS SCSI interface. */
#define KFIO_SCSI_QUEUE_DEPTH 256

/* How many ebs can avoid grooming after groom failure before we slow down writes */
#define FIO_GROOM_SOFT_MAX_EXEMPT_BLOCKS 4

/* If True, hides the module parameters from use from the Linux kernel. */
#define DISABLE_MODULE_PARAM_EXPORT 0

/* The micro version of this build */
#define FUSION_MICRO_VERSION "15"

/* OEM formal name */
#define FIO_OEM_NAME_FORMAL "Fusion-io"

#endif /* __GENERATED_PORT_CONFIG_H__ */
