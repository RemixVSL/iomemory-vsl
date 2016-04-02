/* Machine generated file - edits will be overwritten.
 * Please use 3.2.0-23-generic.config to change these values.
 * If the file does not exist in the base directory, make it.
 * Entries are of the format NAME=VALUE. The name
 * of the file is determined by the --variant=foobar arg to scons,
 * which has a default value dependent on the machine type,
 * e.g. 'linux2-x86_amd64' or 'freebsd8-amd64' , leading to a config
 * filename of 'foobar.config' or 'linux2-x86_amd64.config'
 * Generated files will be created, and the build will take place
 * in a directory named to that value. e.g. 'linux2-x86_amd64/' or 'foobar/'.
 * Default values come from site_scons/fio_symbols.py and fio_options.py
 * but you probably should not change them.
 */

#include <fio/port/config_vars_externs.h>
#ifndef __FIO_PORT_CONFIG_H__
#define __FIO_PORT_CONFIG_H__


/* N/A */
#define ALWAYS_REBUILD_MD 0

/* N/A */
#define ARCHIVE_NAND_COMMANDS 0

/* Automatically attach drive during driver initializaiton: 0 = disable, 1 = enable (default), 2 = enable, but abort if there was an unclean shutdown, skipping a full rescan */
#define AUTO_ATTACH 1

/* Controls directCache behavior after an unclean shutdown: 0 = disable (cache is discarded and manual rebinding is necessary), 1 = enable (default). */
#define AUTO_ATTACH_CACHE 1

/* N/A */
#define BYPASS_ECC 0

/* This totally bypasses all whitening on the card */
#define BYPASS_WHITENING 0

/* Enables Directcache linking Support */
#define CACHEBITS_DIFF2 0

/* Timeout for data log compaction while shutting down. */
#define COMPACTION_TIMEOUT_MS 600000

/* Turn off all debug settings - they won't be available to turn on at run-time. */
#define DBGSET_COMPILE_DBGPRINT_OUT 1

/* N/A */
#define DBGSET_LOCATIONS 0

/* N/A */
#define DBGS_DIGEST 0

/* N/A */
#define DBGS_DMA 0

/* N/A */
#define DBGS_ECC 0

/* General debug messages */
#define DBGS_GENERAL 0

/* N/A */
#define DBGS_GROOM 0

/* N/A */
#define DBGS_INJECT 0

/* N/A */
#define DBGS_LOG_FILE 0

/* N/A */
#define DBGS_LRBTREE 0

/* Debugs regarding expected media errors such as failed programs or erases. */
#define DBGS_MEDIA_ERROR 1

/* General debug messages */
#define DBGS_METADATA 0

/* N/A */
#define DBGS_PCIE 0

/* Persitent Trim Messages */
#define DBGS_PTRIM 0

/* N/A */
#define DBGS_REQUEST 0

/* N/A */
#define DBGS_SHOW_INJECT 0

/* N/A */
#define DEEXTREMIFY 0

/* Action to take on entering flashback mode: 0 = disable flashback, 1 = no action, 2 = limit writes, 3 = read only, 4 = fail device */
#define DEFAULT_FLASHBACK_MODE_ACTION 1

/* Default number of blocks allowed for bad blocks, in hundredths of percent of total blocks. */
#define DEFAULT_MFR_BB_ALLOWANCE_HPCNT 200

/* The default packet size used for fio_formatting a device. */
#define DEFAULT_PACKET_SIZE 512

/* N/A */
#define DISABLE_MSI 1

/* N/A */
#define DISABLE_RLE 0

/* This causes the card to not whiten EBs as it writes */
#define DONT_WHITEN 0

/* On ECC correction events, fail all chips that equal or exceed this threshold. */
#define ECC_CORRECTION_PER_PAD_RETIRE -1

/* On ECC correction events, any chunk experiencing this many or more parts will cause chips to be failed. */
#define ECC_CORRECTION_RETIRE_THRESHOLD -1

/* On ECC correction events, fail chips until the remaining error bits are below or equal to this threshold. */
#define ECC_CORRECTION_TARGET_THRESHOLD -1

/* N/A */
#define ENABLE_CACHE_FILE 1

/* N/A */
#define ENABLE_DISCARD 1

/* N/A */
#define ENABLE_ECC 1

/* N/A */
#define ENABLE_ECC_CHUNK_SHARE 1

/* Compile-in and enable the error injection subsystem. Use fio-error-inject utility to cause errors. */
#define ENABLE_ERROR_INJECTION 0

/* N/A */
#define ENABLE_PCICHECK 0

/* Sets PCIe device control register bits to prevent hang on device disable non-Windows 7/2008 OS's. */
#define ENABLE_PCIE_DEVICE_CONTROL_FIX 1

/* N/A */
#define ENABLE_RANDOM_READ 1

/* N/A */
#define ENABLE_TWO_PLANE 1

/* N/A */
#define FIND_BUG_DA 0

/* Binary installation directory */
#define FIO_BINDIR "/usr/bin"

/* Threshhold 1 for ditching boring packets in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_1 15

/* Threshhold 2 for ditching intersting in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_2 5

/* Threshhold 3 for ditching very intersting in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_3 2

/* Threshhold 4 for ditching all clean packets in cache mode */
#define FIO_CACHE_DITCH_THRESHOLD_4 1

/* Turns on raw request statistics gathering */
#define FIO_COLLECT_RAW_STATS 1

/* Datadir (platform independent data) installation directory */
#define FIO_DATADIR "/usr/share"

/* Under what rate (MB/s) do we consider the system idle */
#define FIO_DATA_GROOM_CONSIDER_IDLE_RATE 1

/* When idle at what rate (MB/s) do we groom */
#define FIO_DATA_IDLE_GROOM_RATE 1

/* Optimal block size hint for the linux block layer. */
#define FIO_DEV_BLK_OPTIMAL_BLK_SIZE 32768

/* Number of seconds to wait for device file creation before continuing. */
#define FIO_DEV_WAIT_TIMEOUT_SECS 30

/* Documentation installation directory */
#define FIO_DOCDIR "/usr/share/doc"

/* The name of the driver */
#define FIO_DRIVER_NAME "iomemory-vsl"

/* Causes the driver to suspend operation on failure */
#define FIO_FAIL_FAST 1

/* Forces software ECC in all cases */
#define FIO_FORCE_SOFT_ECC 0

/* The number of free LEBs that stops grooming */
#define FIO_GC_DEFAULT_HIGH_WATER 30

/* The number of free LEBs that triggers grooming */
#define FIO_GC_DEFAULT_LOW_WATER 29

/* Maximum number of blocks to do on each pass */
#define FIO_GROOMER_DEFAULT_BLOCKS_PER 1

/* Default amount of data (MB) to do on each pass */
#define FIO_GROOMER_DEFAULT_DATA_PER 1

/* How many messages max to copy at once */
#define FIO_GROOMER_DEFAULT_MSG_BATCH_SIZE 512

/* How many requests max to use copying */
#define FIO_GROOMER_DEFAULT_REQUEST_BATCH_SIZE 512

/* Maximum number of blocks to do on each pass */
#define FIO_GROOMER_MAX_BLOCKS_PER 10

/* Maximum amount of data (MB) to do on each pass */
#define FIO_GROOMER_MAX_DATA_PER 1000

/* How many times we try regroom at the append point */
#define FIO_GROOM_APPEND_POINT_RETRIES 25

/* Size in KB of groomer buffer */
#define FIO_GROOM_BUFFER_SIZE 2048

/* N/A */
#define FIO_GROOM_FAILING 0

/* Percentage free LEBs below which the idle groomer is more aggressive. */
#define FIO_GROOM_HARDER_THRESHOLD 5

/* How many ebs can avoid grooming after groom failure before we disable the device */
#define FIO_GROOM_HARD_MAX_EXEMPT_BLOCKS 8

/* Number of EBs the oldest groomer gets at a time. */
#define FIO_GROOM_OLDEST_LEB_COUNT 1

/* How many ms between grooming the oldest LEB */
#define FIO_GROOM_OLDEST_LEB_INTERVAL 30000

/* How often do we wake up to check if we are idle */
#define FIO_GROOM_POLL_INTERVAL 5000

/* How many ebs can avoid grooming after groom failure before we slow down writes */
#define FIO_GROOM_SOFT_MAX_EXEMPT_BLOCKS 4

/* Turns on extra veification in the groomer for ensuring everything is correct */
#define FIO_GROOM_VERIFY 0

/* Include installation directory */
#define FIO_INCLUDEDIR "/usr/include"

/* Causes a panic to be injected in a reclaim */
#define FIO_INJECT_RECLAIM_PANIC 0

/* Library installation directory */
#define FIO_LIBDIR "/usr/lib"

/* How many active read-ahead pages to keep in a log file */
#define FIO_LOG_FILE_PAGES 4

/* Maximum number of interpolation probes before falling back to binary search */
#define FIO_LRBTREE_ISEARCH_CUTOFF 6

/* Manual installation directory */
#define FIO_MANDIR "/usr/share/man"

/* N/A */
#define FIO_METADATA_DEBUG_LRBTREE 0

/* N/A */
#define FIO_METADATA_SWAP 0

/* Datadir (platform independent data) installation directory */
#define FIO_OEM_DATADIR "/usr/share/fio"

/* Library installation directory */
#define FIO_OEM_LIBDIR "/usr/lib/fio"

/* OEM name simplified for file naming */
#define FIO_OEM_NAME "fusionio"

/* OEM formal name */
#define FIO_OEM_NAME_FORMAL "Fusion-io"

/* OEM short name simplified for file naming */
#define FIO_OEM_NAME_SHORT "fio"

/* Source installation directory */
#define FIO_OEM_SOURCEDIR "/usr/src/fio"

/* Causes the driver to issue a message on detecting powercut */
#define FIO_POWERCUT_DETECT 1

/* Causes the driver to pre-allocate the RAM it needs */
#define FIO_PREALLOCATE_MEMORY 0

/* installation prefix */
#define FIO_PREFIX "/usr"

/* N/A */
#define FIO_PRINT_DANGLING 0

/* N/A */
#define FIO_PRINT_DIGEST 0

/* N/A */
#define FIO_PRINT_GROOM_STATS 0

/* N/A */
#define FIO_PRINT_OBJECT 0

/* The number of free LEBs before we hold off writes */
#define FIO_RESERVED_LEBS 25

/* Source installation directory */
#define FIO_SOURCEDIR "/usr/src"

/* Sysconfig installation directory */
#define FIO_SYSCONFDIR "/etc"

/* true: report a flashback substitution warning as a failure. */
#define FLASHBACK_WARNING_AS_FAILURE 0

/* Force setting PCIe device control register bits to prevent hang on device disable. */
#define FORCE_ENABLE_PCIE_DEVICE_CONTROL_FIX 0

/* N/A */
#define FORCE_MINIMAL_MODE 0

/* Force all read completions to occur in worker thread context, even if IODRIVE_ISR_READ_COMPLETIONS is set. */
#define FORCE_SW_READ_COMPLETIONS 0

/* Enable the memory allocation trip wire (for testing preallocation) */
#define FUSION_ALLOCATION_TRIPWIRE 0

/* The build version of this build */
#define FUSION_BUILD_VERSION "183"

/* Turn on debugging */
#define FUSION_DEBUG 0

/* Counts allocs and frees in cache pools */
#define FUSION_DEBUG_CACHE 0

/* Something to do with debugging memory */
#define FUSION_DEBUG_MEMORY 0

/* Debug metadata */
#define FUSION_DEBUG_METADATA 0

/* Enables Directfs Support */
#define FUSION_DIRECTFS 0

/* Enable dumping command queue on device failure. */
#define FUSION_DUMP_CMD_Q_ON_FAILURE 8

/* Compile in the event logger */
#define FUSION_INCLUDE_KTR 0

/* The major version of this build */
#define FUSION_MAJOR_VERSION "2"

/* Assume worst-case big LRBTree while groming space so save metadata. */
#define FUSION_METADATA_SAVE_WORST_CASE 0

/* The micro version of this build */
#define FUSION_MICRO_VERSION "11"

/* The minor version of this build */
#define FUSION_MINOR_VERSION "3"

/* The maximim packet size supported for fio_formatting a device. */
#define FUSION_PACKET_SIZE_MAX 4096

/* The minimim packet size supported for fio_formatting a device. */
#define FUSION_PACKET_SIZE_MIN 512

/* Require packet size to be power of two. */
#define FUSION_PACKET_SIZE_MUST_BE_POWER_OF_TWO 1

/* N/A */
#define FUSION_SPARSEFS_SUPPORT 0

/* Support the simulated DMA command interface */
#define FUSION_SUPPORT_SIMULATED_DMA 0

/* Use software resource accounting to prevent overflowing the hardware. */
#define FUSION_USE_SW_RESOURCE_ACCOUNTING 1

/* Validate data written by the frob utility (svn://svn/main/kvigor/scratchbox/frob) */
#define FUSION_VERIFY_FROB 0

/* Make i/o buffers readonly as part of from testing (requires kernel mods) */
#define FUSION_VERIFY_FROB_READONLY 0

/* The version of this build */
#define FUSION_VERSION_STRING "2.3.11.183 acadia@8c2abaa3355d"

/* Strategy number for mitigating groomer system impact */
#define GROOMER_BACKOFF 1

/* N/A */
#define GROOMER_DISABLE 0

/* Allow for 1.2.7 Firmware to be used with >= 2.0 software */
#define IODRIVE_ALLOW_1_2_7_FW 0

/* Percentage of non-failed LEBs below which we start checking for a dead pad. */
#define IODRIVE_DEAD_PAD_DETECTION_THRESHOLD 99

/* If >= this percentage of total failures are contributed by a single pad, consider it dead. */
#define IODRIVE_DEAD_PAD_THRESHOLD 90

/* Turns on assertion checking */
#define IODRIVE_DEBUG 0

/* Tracks all requests in one list */
#define IODRIVE_DEBUG_TRACK_ALL_REQUESTS 0

/* disable the use of append reservations */
#define IODRIVE_DEFEAT_APPEND_RESERVATIONS 0

/* N/A */
#define IODRIVE_DISABLE_PCIE_RELAXED_ORDERING 0

/* Delay value for dma writes */
#define IODRIVE_DMA_DELAY 16

/* N/A */
#define IODRIVE_GATHER_NAND_STATS 0

/* N/A */
#define IODRIVE_GATHER_STATS 0

/* How many reads until we inject a pad failure (0=don't inject failures */
#define IODRIVE_INJECT_PAD_FAILURE_FREQUENCY 0

/* Allow completion of read requests in ISR context. */
#define IODRIVE_ISR_READ_COMPLETIONS 0

/* Turns KTR on in the driver */
#define IODRIVE_KTR_ENABLED 0

/* N/A */
#define IODRIVE_LARGE_PCIE_MTU 1

/* N/A */
#define IODRIVE_LOAD_EB_MAP 1

/* N/A */
#define IODRIVE_LOAD_MIDPROM 1

/* Maximum number of erase blocks to search for the LEB map. */
#define IODRIVE_MAX_EBMAP_PROBE 100

/* How many times an EB may be read before it is scheduled for grooming (to mitigate read disturbs) */
#define IODRIVE_MAX_READ_THRESHOLD 15000

/* How many requests pending in iodrive */
#define IODRIVE_NUM_REQUEST_STRUCTURES 5000

/* How many packetizer commands before we require an interrupt */
#define IODRIVE_PACKETIZER_INTERRUPT_THRESHOLD 10

/* Print log messages on ECC correction events */
#define IODRIVE_PRINT_CORRECTABLE_GROOMS 1

/* How many read DMA's before we require an interrupt */
#define IODRIVE_READ_DMA_INTERRUPT_THRESHOLD 4

/* ECC failures below this read threshold will be not be considered a read disturb and will result in the faulty part being failed.  */
#define IODRIVE_READ_RETIRE_THRESHOLD 20000

/* Scaling factor for IODRIVE_MAX_READ_THRESHOLD on MLC parts. */
#define IODRIVE_READ_THRESHOLD_MLC_SCALING 2

/* N/A */
#define IODRIVE_SCAN_NV_DATA 1

/* command queue uses real DMA: 0=yes 1=no */
#define IODRIVE_SIMULATE_COMMAND_QUEUING_DMA 0

/* N/A */
#define IODRIVE_SYNC_DELAY_US 2000

/* N/A */
#define IODRIVE_SYNC_REQUEST_DATA_DELAY_US 50000

/* N/A */
#define IODRIVE_SYNC_REQUEST_DATA_WAIT_US 50000

/* N/A */
#define IODRIVE_SYNC_WAIT_US 100000

/* N/A */
#define IODRIVE_TADDR0 4

/* N/A */
#define IODRIVE_TADDR1 4

/* N/A */
#define IODRIVE_TCMD0 4

/* N/A */
#define IODRIVE_TCMD1 4

/* 'Percent capacity remaining before reduced write mode' warning threshold in hundredths of a percent */
#define IODRIVE_THROTTLE_CAPACITY_WARNING_THRESHOLD 1000

/* Hundredths of a percent of non-failed LEBs below which we enter read-only mode. */
#define IODRIVE_THROTTLE_READ_ONLY_THRESHOLD 9400

/* Hundredths of a percent of non-failed LEBs below which we enter reduced write mode. */
#define IODRIVE_THROTTLE_REDUCED_WRITE_THRESHOLD 9600

/* N/A */
#define IODRIVE_TINTR_HW_WAIT 0

/* N/A */
#define IODRIVE_TREAD0 3

/* N/A */
#define IODRIVE_TREAD1 1

/* N/A */
#define IODRIVE_TREAD_HW_WAIT 1800

/* N/A */
#define IODRIVE_TREAD_HW_WAIT_MLC 2000

/* N/A */
#define IODRIVE_TSTAT_HW_WAIT 400

/* N/A */
#define IODRIVE_TSTAT_HW_WAIT_MLC 100

/* N/A */
#define IODRIVE_TWRITE0 2

/* N/A */
#define IODRIVE_TWRITE1 2

/* Use the command timeout registers */
#define IODRIVE_USE_COMMAND_TIMEOUTS 1

/* Turns on the DPC based completion */
#define IODRIVE_USE_DPC 0

/* Use the ability to substitute the parity chip */
#define IODRIVE_USE_PAD_REMAP 1

/* Max number of IOs of one type (read or write) which may be consecutively issued before switching to the other type. */
#define IO_SCHEDULER_SWITCH_THRESHOLD 50000

/* Yield CPU at least once every specified interval while scanning the log. */
#define LOG_SCAN_YIELD_INTERVAL_MSECS 2000

/* Enable to profile the LRBTree rather than auto-attach */
#define LRBTREE_PROFILE 0

/* N/A */
#define LRB_DEBUG_MESSAGE 0

/* N/A */
#define LRB_STATISTICS 0

/* N/A */
#define MAX_MD_BLOCKS_PER_DEVICE 0

/* The number of objects to be preallocated if preallocation is on. */
#define MEMOS_PREALLOCATED_OBJECTS 32768

/* Special behaviors for Nucleon */
#define NUCLEON 0

/* N/A */
#define NUM_EXCLUDE_DEVICES 0

/* N/A */
#define NUM_INCLUDE_DEVICES 0

/* Override fio_ktr_mask to filter KTR events at run-time */
#define OVERRIDE_KTR_MASK 0

/* N/A */
#define PARALLEL_ATTACH 1

/* Global PCIe slot power limit in milliwatts. Performance will be throttled to not exceed this limit in any PCIe slot. */
#define PCIe_GLOBAL_SLOT_POWER_LIMIT 24750

/* N/A */
#define PERSISTENT_DISCARD 0

/* Turns on accounting for the various stages of the pipeline */
#define PIPELINE_ACCOUNTING 0

/* N/A */
#define PRINT_APPEND_FAILURES 0

/* N/A */
#define PRINT_APPEND_POINT 0

/* N/A */
#define PRINT_DEPACKETIZER 0

/* N/A */
#define PRINT_DMA 0

/* N/A */
#define PRINT_DMA_READ 0

/* N/A */
#define PRINT_DMA_WRITE 0

/* N/A */
#define PRINT_ECC_READ 0

/* N/A */
#define PRINT_ECC_WRITE 0

/* N/A */
#define PRINT_GROOM_RANGES 0

/* N/A */
#define PRINT_GROOM_SCAN 0

/* N/A */
#define PRINT_IDLE_GROOM 0

/* N/A */
#define PRINT_LEB_OPERATIONS 0

/* N/A */
#define PRINT_NAND_READ 0

/* N/A */
#define PRINT_NAND_READ_DETAILS 0

/* N/A */
#define PRINT_NAND_WRITE 0

/* N/A */
#define PRINT_PACKETIZER 0

/* N/A */
#define PRINT_PARITY_OPERATIONS 0

/* N/A */
#define PRINT_PROGRESS 0

/* N/A */
#define PRINT_REATTACH 0

/* N/A */
#define PRINT_REQUESTS 0

/* N/A */
#define PRINT_REQUEST_LIST 0

/* N/A */
#define PRINT_RESOURCES 0

/* N/A */
#define PRINT_SCAN 0

/* N/A */
#define PRINT_SCAN_ENTRIES 0

/* N/A */
#define REGISTER_AS_CCISS 0

/* Gather spinlock statistics: currently only implemented on linux. */
#define SPINLOCK_STATS 0

/* Force strict write flushing on early non-powercut safe cards. (1=enable, 0=disable, -1=auto) Do not change for newer cards. */
#define STRICT_SYNC -1

/* N/A */
#define SUPPORT_DUMP 0

/* N/A */
#define SUPPORT_HIBERNATION 0

/* N/A */
#define THERMAL_THROTTLE_DISABLE 0

/* If true, use 1024 byte PCIe rx buffer. This improves performance but causes NMIs on some specific hardware. */
#define USE_LARGE_PCIE_RX_BUFFER 0

/* N/A */
#define USE_NEW_IO_SCHED 0

#endif /* __FIO_PORT_CONFIG_H__ */
