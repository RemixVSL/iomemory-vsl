/*
  This "meta" file is supposed to abstract things that change in kfile.c
  and should provide simplified defines that are named after their function.
  Both aimed at making the code cleaner, more readable and perhaps more
  maintainable. Blocks should be isolated and only cover one item.
 */
#ifndef __FIO_KFILE_META_H__
#define __FIO_KFILE_META_H__

#include <linux/version.h>

#if KFIOC_X_CAPS_PDE_DATA
#define KFIO_PDE_DATA PDE_DATA(ip)
#else
#define KFIO_PDE_DATA pde_data(ip)
#endif /* PDE_DATA */

#endif /* __FIO_FILE_META_H__ */

