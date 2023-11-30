/*
  This "meta" file is supposed to abstract things that change in sysrq.c
  and should provide simplified defines that are named after their function.
  Both aimed at making the code cleaner, more readable and perhaps more
  maintainable. Blocks should be isolated and only cover one item.
 */
#ifndef __FIO_SYSRQ_META_H__
#define __FIO_SYSRQ_META_H__

#if KFIOC_X_HANDLE_SYSRQ_IS_U8
#define HANDLE_SYSRQ_TYPE u8
#else
#define HANDLE_SYSRQ_TYPE int
#endif /* KFIOC_X_HANDLE_SYSRQ_IS_U8 */

#endif /* __FIO_SYSRQ_META_H__ */
