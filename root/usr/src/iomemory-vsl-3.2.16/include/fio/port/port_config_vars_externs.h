/* -----------------------------------------------------------------------------
 * Copyright (c) 2018, Fusion-io, Inc. (acquired by SanDisk Corp. 2014)
 * Copyright (c) 2014-2015 SanDisk Corp. and/or all its affiliates.
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

#ifndef  __GENERATED_PORT_CONFIG_VARS_EXTERNS_H__
#define  __GENERATED_PORT_CONFIG_VARS_EXTERNS_H__

// The maximum length of a string parameter                            
// NOTE: we must make the string long enough to accommodate 384 ints   
// separated by commas and terminated with a nul char.                 
// NOTE2: 384 is the max number of PCIE devices on a bus.              
// Sounds crazy, but you never know when someone is going to push      
// the limit.                                                          
#define PARAM_STRING_VAL_MAX_LEN    11*384    

// Integer array                              
// Note: If you change this you must change it in ioctl-types.h
#ifndef INT_ARRAY_MAX_ELEMENTS                
#define INT_ARRAY_MAX_ELEMENTS      256       
typedef struct _int_array_s_                  
{                                             
    int     num_elements_used;                
    int     pad;                              
    int     array[INT_ARRAY_MAX_ELEMENTS];    
} int_array_t;                                
#endif // not defined INT_ARRAY_MAX_ELEMENTS  

#if defined(__KERNEL__)

extern int auto_attach ; //AUTO_ATTACH
extern int compaction_timeout_ms ; //COMPACTION_TIMEOUT_MS
extern int strict_sync ; //STRICT_SYNC
extern const int port_supports_pci_numa_info ; //PORT_SUPPORTS_PCI_NUMA_INFO
extern int disable_msi ; //DISABLE_MSI
extern int disable_msix ; //DISABLE_MSIX
extern int disable_scanner ; //DISABLE_SCANNER
extern int enable_discard ; //ENABLE_DISCARD
extern int bypass_ecc ; //BYPASS_ECC
extern char exclude_devices[PARAM_STRING_VAL_MAX_LEN] ; //EXCLUDE_DEVICES
extern int expected_io_size ; //EXPECTED_IO_SIZE
extern char fio_affinity[PARAM_STRING_VAL_MAX_LEN] ; //FIO_AFFINITY
extern int fio_dev_optimal_blk_size ; //FIO_DEV_BLK_OPTIMAL_BLK_SIZE
extern int fio_dev_wait_timeout_secs ; //FIO_DEV_WAIT_TIMEOUT_SECS
extern char external_power_override[PARAM_STRING_VAL_MAX_LEN] ; //FIO_EXTERNAL_POWER_OVERRIDE
extern int groom_harder_threshold ; //FIO_GROOM_HARDER_THRESHOLD
extern char preallocate_memory[PARAM_STRING_VAL_MAX_LEN] ; //FIO_PREALLOCATE_MEMORY
extern int enable_unmap ; //FIO_SCSI_UNMAP_SUPPORTED
extern int force_minimal_mode ; //FORCE_MINIMAL_MODE
extern int disable_groomer ; //GROOMER_DISABLE
extern int groomer_high_water_delta_hpcnt ; //GROOMER_HIGH_WATER_DELTA_HPCNT
extern int groomer_low_water_delta_hpcnt ; //GROOMER_LOW_WATER_DELTA_HPCNT
extern char include_devices[PARAM_STRING_VAL_MAX_LEN] ; //INCLUDE_DEVICES
extern int iodrive_load_eb_map ; //IODRIVE_LOAD_EB_MAP
extern int make_assert_nonfatal ; //IODRIVE_MAKE_ASSERT_NONFATAL
extern const int iodrive_max_ebmap_probe ; //IODRIVE_MAX_EBMAP_PROBE
extern int max_requests ; //IODRIVE_MAX_NUM_REQUEST_STRUCTURES
extern int capacity_warning_threshold ; //IODRIVE_THROTTLE_CAPACITY_WARNING_THRESHOLD
extern int use_command_timeouts ; //IODRIVE_USE_COMMAND_TIMEOUTS
extern int scsi_queue_depth ; //KFIO_SCSI_QUEUE_DEPTH
extern int max_md_blocks_per_device ; //MAX_MD_BLOCKS_PER_DEVICE
extern int parallel_attach ; //PARALLEL_ATTACH
extern int global_slot_power_limit_mw ; //PCIe_GLOBAL_SLOT_POWER_LIMIT
extern int port_supports_numa_node_cache_allocations ; //PORT_SUPPORTS_NUMA_NODE_CACHE_ALLOCATIONS
extern const int port_supports_numa_node_override ; //PORT_SUPPORTS_NUMA_NODE_OVERRIDE
extern const int port_supports_per_cpu ; //PORT_SUPPORTS_PER_CPU
extern const int port_supports_sglist_copy ; //PORT_SUPPORTS_SGLIST_COPY
extern const int port_supports_user_pages ; //PORT_SUPPORTS_USER_PAGES
extern int preallocate_mb ; //PREALLOCATE_MB
extern int rmap_memory_limit_MiB ; //RMAP_MEMORY_LIMIT_MiB
extern int rsort_memory_limit_MiB ; //RSORT_MEMORY_LIMIT_MiB
extern int use_large_pcie_rx_buffer ; //USE_LARGE_PCIE_RX_BUFFER
extern int force_enable_pcie_device_control_fix ; //FORCE_ENABLE_PCIE_DEVICE_CONTROL_FIX
extern const int log_scan_yield_interval_ms ; //LOG_SCAN_YIELD_INTERVAL_MSECS
extern int fio_qos_enable ; //FIO_QOS_ENABLE
extern int use_new_io_sched ; //USE_NEW_IO_SCHED
extern int enable_ecc ; //ENABLE_ECC
extern int tintr_hw_wait ; //IODRIVE_TINTR_HW_WAIT
extern int iodrive_load_midprom ; //IODRIVE_LOAD_MIDPROM
extern int auto_attach_cache ; //AUTO_ATTACH_CACHE
extern int retire_on_psub ; //RETIRE_ON_PSUB
extern int use_modules ; //USE_MODULES
extern int enable_pcie_device_control_fix ; //ENABLE_PCIE_DEVICE_CONTROL_FIX
extern int force_soft_ecc ; //FIO_FORCE_SOFT_ECC
extern int read_pipe_depth ; //READ_PIPE_DEPTH

#endif /* __KERNEL__ */
#endif //  __GENERATED_PORT_CONFIG_VARS_EXTERNS_H__
