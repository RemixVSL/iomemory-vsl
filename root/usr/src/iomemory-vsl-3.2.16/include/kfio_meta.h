/*
  This "meta" file is supposed to abstract things that change in kblock.c
  and should provide simplified defines that are named after their function.
  Both aimed at making the code cleaner, more readable and perhaps more
  maintainable. Blocks should be isolated and only cover one item.
 */
#include <linux/string.h>

#ifndef __FIO_KFIO_META_H__
#define __FIO_KFIO_META_H__

/*
 * kfio_strscpy_pad() - copy a string into a sized buffer, NULL-padding the
 * tail. Uses strscpy_pad() where available (5.2+); falls back to strncpy()
 * on older kernels, which has the same padding semantics and is still
 * present there (it was only removed in 7.2).
 */
#if KFIOC_X_HAVE_STRSCPY_PAD
#define KFIO_STRSCPY_PAD(dst, src, count) strscpy_pad((dst), (src), (count))
#else
#define KFIO_STRSCPY_PAD(dst, src, count) strncpy((dst), (src), (count))
#endif

#endif /* __FIO_KFIO_META_H__ */
