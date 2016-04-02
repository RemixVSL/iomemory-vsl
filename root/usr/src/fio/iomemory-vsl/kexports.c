//-----------------------------------------------------------------------------
// Copyright 2010-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
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
#include <fio/port/linux/kblock.h>

// Export symbols from internal fio-dircache.h functions.
typedef struct dc_stats dircache_stats;
typedef struct fio_block_range fio_block_range_t;
typedef struct fio_persistent_data_key fio_persistent_data_key_t;
typedef struct dircache_callbacks dircache_callbacks_t;
typedef struct fio_version fio_version_t;

int kfio_dircache_get_vsl_callback_version(fio_version_t *callback_version);
EXPORT_SYMBOL(kfio_dircache_get_vsl_callback_version);

int kfio_dircache_register_handlers(dircache_callbacks_t *callbacks);
EXPORT_SYMBOL(kfio_dircache_register_handlers);

void kfio_dircache_unregister_handlers(void);
EXPORT_SYMBOL(kfio_dircache_unregister_handlers);

// Kernelspace versions of the UUID family functions.
typedef unsigned char* fio_uuid_t;
void fio_uuid_copy(fio_uuid_t dst, fio_uuid_t src);
void fio_uuid_unparse(fio_uuid_t uu, char* dst);
int fio_uuid_compare(fio_uuid_t u1, fio_uuid_t u2);
bool fio_uuid_is_null(fio_uuid_t uu);
EXPORT_SYMBOL(fio_uuid_copy);
EXPORT_SYMBOL(fio_uuid_unparse);
EXPORT_SYMBOL(fio_uuid_compare);
EXPORT_SYMBOL(fio_uuid_is_null);


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
EXPORT_SYMBOL(fio_pps_is_started);
EXPORT_SYMBOL(fio_pps_format);
EXPORT_SYMBOL(fio_pps_get_value);
EXPORT_SYMBOL(fio_pps_set_value);
EXPORT_SYMBOL(fio_pps_delete_value);

typedef struct fusion_config_set fusion_config_set_t;
typedef struct fusion_config_get fusion_config_get_t;
typedef struct fusion_config_enum_size fusion_config_enum_size_t;
int fiocfg_set(fusion_config_set_t * const set_p);
int fiocfg_get(fusion_config_get_t * const get_p);
int fiocfg_enumerate(uint32_t *enum_table_size_p, char* const multi_string_table_p);
int fiocfg_enum_get_size(fusion_config_enum_size_t * const enum_size_p);
extern unsigned int config_table_count;

EXPORT_SYMBOL(fiocfg_set);
EXPORT_SYMBOL(fiocfg_get);
EXPORT_SYMBOL(fiocfg_enumerate);
EXPORT_SYMBOL(fiocfg_enum_get_size);
EXPORT_SYMBOL(config_table_count);

EXPORT_SYMBOL(kfio_bio_submit);
EXPORT_SYMBOL(kfio_bio_alloc);
EXPORT_SYMBOL(kfio_bio_free);

struct fusion_nand_device;
struct fio_device *kfio_fio_device_from_nand_dev(struct fusion_nand_device *nand_dev);
EXPORT_SYMBOL(kfio_fio_device_from_nand_dev);

