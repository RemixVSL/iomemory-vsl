/*
  Device enumeration for a namespace shared with another ioMemory driver.

  Every generation of this driver enumerates its cards from zero and registers
  them under the same names: /proc/<root>/fct<n>, /dev/fct<n> and /dev/fio<a>.
  Two generations loaded at once therefore collide on the first card, and the
  second driver to load fails to attach it.

  The numbers are generated inside the prebuilt driver object, which hands the
  finished names to this porting layer.  This file is the one place that
  renumbers them, so that the driver occupies a free range of the shared
  namespace instead of insisting on starting at zero.  The names themselves are
  never changed: userspace tools keep working unmodified, and the range in use
  is printed at load time rather than having to be inferred.
 */
#ifndef __FIO_PORT_COMMON_LINUX_KENUM_H__
#define __FIO_PORT_COMMON_LINUX_KENUM_H__

/*
 * Longest name handled here: a device prefix plus its index, or one procfs
 * path component.  The names themselves are far shorter than this.
 */
#define FIO_ENUM_NAME_MAX      32

/* Highest number of cards a single driver can be renumbered around. */
#define FIO_ENUM_MAX_DEVICES   26

/*
 * Module parameter.  -1 detects the first free device number at load time,
 * 0 always enumerates from zero (the behaviour of a driver built without
 * this file), and a positive value forces a starting number.
 */
extern int fio_dev_index_base;

/**
 * @brief Fix the device number this driver enumerates from.
 *
 * Called once, while the top level procfs directory is being set up and before
 * any card has registered, so that whatever is already in @proc_root belongs
 * to another driver.  @shared says whether that directory already existed.
 * Returns the resolved base.
 */
int fio_enum_resolve_base(const char *proc_root, int shared);

/** @brief The resolved base.  Zero means nothing is renumbered. */
int fio_enum_get_base(void);

/** @brief True if @path resolves, used to find occupied names. */
int fio_enum_path_exists(const char *path);

/** @brief 1 if @path is an empty directory, 0 if it has entries, -1 if unknown. */
int fio_enum_dir_is_empty(const char *path);

/**
 * @brief True only if /proc/<proc_root> can safely be removed, i.e. it is
 *        provably empty.
 *
 * Removing a non-empty procfs directory leaks its children and makes the next
 * load collide when it recreates the directory, so anything other than a
 * definite "empty" answer means leave it in place.
 */
int fio_enum_root_removable(const char *proc_root);

/**
 * @brief Renumber a control device name ("fct0").
 *
 * @param devnum  the device number the driver object assigned
 * @param stock   the name it derived from that number
 *
 * Returns @stock unchanged unless the name is one this code recognises as
 * "prefix followed by devnum" and there is a base to add.  The result is
 * stable storage: misc_register() keeps the pointer it is given.
 */
const char *fio_enum_control_name(unsigned int devnum, const char *stock);

/**
 * @brief Renumber a block device name ("fioa") into @buf.
 *
 * @param disk_index  the index the driver object assigned
 * @param stock       the name it derived from that index
 *
 * Returns @stock unchanged unless the name is one this code recognises as
 * "prefix followed by the letters for disk_index".
 */
const char *fio_enum_block_name(int disk_index, const char *stock,
                                char *buf, size_t buflen);

/**
 * @brief Renumber a procfs entry name ("fct0") into @buf.
 *
 * Entries of the top level directory that are not a control device (the "fio"
 * subdirectory, "version", ...) are returned unchanged.
 */
const char *fio_enum_proc_name(const char *stock, char *buf, size_t buflen);

#endif /* __FIO_PORT_COMMON_LINUX_KENUM_H__ */
