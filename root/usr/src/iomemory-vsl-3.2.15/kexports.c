//-----------------------------------------------------------------------------
// Copyright (c) 2010-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
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


#if !defined (__linux__)
#error This file supports linux only
#endif

// TODO:  The following functions are used by directcache and require export
//        from the firehose module.  This needs to be implemented instead
//        using a function pointer table.  For now, I'm putting them in
//        here temporarily.

#include "port-internal.h"
#include <fio/port/kfio.h>
#include <fio/port/common-linux/kblock.h>
#include <fio/port/vectored.h>

// Export symbols from internal fio-dircache.h functions.
typedef struct dc_stats dircache_stats;
typedef struct fio_block_range fio_block_range_t;
typedef struct fio_persistent_data_key fio_persistent_data_key_t;
typedef struct dircache_callbacks dircache_callbacks_t;
typedef struct fio_version fio_version_t;

#if (KFIO_SCSI_DEVICE==0)
int kfio_vectored_atomic(struct block_device *bdev,
                         const struct kfio_iovec *iov,
                         uint32_t iovcnt,
                         bool user_pages);
KFIO_EXPORT_SYMBOL(kfio_vectored_atomic);
int kfio_count_sectors_inuse(struct block_device *bdev,
                           uint64_t base,
                           uint64_t length,
                           uint64_t *count);
KFIO_EXPORT_SYMBOL(kfio_count_sectors_inuse);
#endif

int kfio_dircache_get_vsl_callback_version(fio_version_t *callback_version);
KFIO_EXPORT_SYMBOL(kfio_dircache_get_vsl_callback_version);

int kfio_dircache_register_handlers(dircache_callbacks_t *callbacks);
KFIO_EXPORT_SYMBOL(kfio_dircache_register_handlers);

void kfio_dircache_unregister_handlers(void);
KFIO_EXPORT_SYMBOL(kfio_dircache_unregister_handlers);

// Kernelspace versions of the UUID family functions.
typedef unsigned char* fio_uuid_t;
void fio_uuid_copy(fio_uuid_t dst, const fio_uuid_t src);
void fio_uuid_unparse(const fio_uuid_t uu, char* dst);
int fio_uuid_compare(const fio_uuid_t u1, const fio_uuid_t u2);
bool fio_uuid_is_null(const fio_uuid_t uu);
KFIO_EXPORT_SYMBOL(fio_uuid_copy);
KFIO_EXPORT_SYMBOL(fio_uuid_unparse);
KFIO_EXPORT_SYMBOL(fio_uuid_compare);
KFIO_EXPORT_SYMBOL(fio_uuid_is_null);


// Persistent Parameter Store structures
struct fusion_version;
struct fusion_pps_format;
struct fusion_pps_info;
struct fusion_pps_entry;
struct fusion_pps_enum;

// Persistent Parameter Store potentially exported functions
// Note: Do not export the following two functions. Only the vsl driver should call them.
//int fio_pps_init_store(struct fio_device* dev);   // Do not export.
//void fio_pps_fini_store(struct fio_device* dev);  // Do not export.
void fio_pps_get_api_version(struct fusion_version* api);
bool fio_pps_is_started(struct fio_device* dev);
int fio_pps_format(struct fio_device* dev, struct fusion_pps_format* formatinfo);
int fio_pps_get_pps_info(struct fio_device* dev, struct fusion_pps_info* info);
int fio_pps_get_value(struct fio_device* fdev, struct fusion_pps_entry* entry, uint32_t* stored_size_bytes);
int fio_pps_set_value(struct fio_device* fdev, struct fusion_pps_entry* entry);
int fio_pps_delete_value(struct fio_device* fdev, struct fusion_pps_entry* entry);
int fio_pps_first_key(struct fio_device* fdev, struct fusion_pps_enum* enumerator);
int fio_pps_next_key(struct fusion_pps_enum* enumerator);

// Persistent Parameter Store actually exported functions
KFIO_EXPORT_SYMBOL(fio_pps_is_started);
KFIO_EXPORT_SYMBOL(fio_pps_format);
KFIO_EXPORT_SYMBOL(fio_pps_get_value);
KFIO_EXPORT_SYMBOL(fio_pps_set_value);
KFIO_EXPORT_SYMBOL(fio_pps_delete_value);

typedef struct fusion_config_set fusion_config_set_t;
typedef struct fusion_config_get fusion_config_get_t;
typedef struct fusion_config_enum_size fusion_config_enum_size_t;
int fiocfg_set(fusion_config_set_t * const set_p);
int fiocfg_get(fusion_config_get_t * const get_p);
int fiocfg_enumerate(uint32_t *enum_table_size_p, char* const multi_string_table_p);
int fiocfg_enum_get_size(fusion_config_enum_size_t * const enum_size_p);
extern unsigned int config_table_count;

KFIO_EXPORT_SYMBOL(fiocfg_set);
KFIO_EXPORT_SYMBOL(fiocfg_get);
KFIO_EXPORT_SYMBOL(fiocfg_enumerate);
KFIO_EXPORT_SYMBOL(fiocfg_enum_get_size);
KFIO_EXPORT_SYMBOL(config_table_count);

KFIO_EXPORT_SYMBOL(kfio_bio_submit);
KFIO_EXPORT_SYMBOL(kfio_bio_submit_handle_retryable);
KFIO_EXPORT_SYMBOL(kfio_bio_alloc);
KFIO_EXPORT_SYMBOL(kfio_bio_free);

struct fusion_nand_device;
struct fio_device *kfio_fio_device_from_nand_dev(struct fusion_nand_device *nand_dev);
KFIO_EXPORT_SYMBOL(kfio_fio_device_from_nand_dev);

//Used for cache maintenance
void kfio_cache_free(fusion_mem_cache_t  *cache,void *p);
void kfio_cache_destroy(fusion_mem_cache_t *cache);
int __kfio_create_cache(fusion_mem_cache_t *pcache,char *name,uint32_t size,uint32_t align);
void *kfio_cache_alloc(fusion_mem_cache_t *cache,int can_wait);
KFIO_EXPORT_SYMBOL(kfio_cache_free);
KFIO_EXPORT_SYMBOL(kfio_cache_destroy);
KFIO_EXPORT_SYMBOL(__kfio_create_cache);
KFIO_EXPORT_SYMBOL(kfio_cache_alloc);

