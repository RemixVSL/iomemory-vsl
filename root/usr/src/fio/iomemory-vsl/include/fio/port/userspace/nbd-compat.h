#ifndef NBD_COMPAT_H_
# define NBD_COMPAT_H_

# define CLISERV_MAGIC 0x00420281861253LL
# define INIT_PASSWD "NBDMAGIC" 
# define NBD_FLAG_HAS_FLAGS  (1 << 0)    /* Flags are there */                                     
# define NBD_FLAG_READ_ONLY  (1 << 1)    /* Device is read-only */

# define NBD_OPT_EXPORT_NAME (1 << 0)

#define NBD_NUM_ZEROS 124

#endif
