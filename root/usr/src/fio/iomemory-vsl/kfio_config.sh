#!/bin/sh

#-----------------------------------------------------------------------------
# Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
#
# No use, or distribution, of this source code is permitted in any form or
# means without a valid, written license agreement with Fusion-io. Please
# refer to the included "License" or "License.txt" file for terms and
# conditions regarding the use and redistribution of this software.
#-----------------------------------------------------------------------------

set -e
set -u


VERBOSE=0
PRESERVE=0
FIOARCH="$(uname -m)"
KERNELDIR="/lib/modules/$(uname -r)/build"
KERNELSOURCEDIR=""
OUTPUTFILE=""
CONFIGDIR=""
CREATED_CONFIGDIR=0
TMP_OUTPUTFILE=""
KFIOC_PROCS=""
FAILED_TESTS=""
TIMEOUT_DELTA=${TIMEOUT_DELTA:-120}
TIMEOUT_TIME=$(($(date "+%s")+$TIMEOUT_DELTA))
FUSION_DEBUG=0


# These are exit codes from include/sysexits.h
EX_OK=0
EX_USAGE=64
EX_SOFTWARE=70
EX_OSFILE=72


# NOTE: test_list is obviously the names of the flags that must be
# tested and set.  Even though it's poor bourne shell style to use
# capitalized function names, the test flags and the test functions
# have identical names so that a look-up-table is not needed (it just
# makes a few things simpler).

KFIOC_TEST_LIST="
KFIOC_MISSING_WORK_FUNC_T
KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
KFIOC_HAS_PCI_ERROR_HANDLERS
KFIOC_HAS_GLOBAL_REGS_POINTER
KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK
KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS
KFIOC_HAS_LINUX_SCATTERLIST_H
KFIOC_KMEM_CACHE_CREATE_REMOVED_DTOR
KFIOC_BIO_ENDIO_REMOVED_BYTES_DONE
KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS
KFIOC_NEEDS_VIRT_TO_PHYS
KFIOC_HAS_BLK_UNPLUG
KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN
KFIOC_HAS_KMEM_CACHE
KFIOC_HAS_NOTIFIER_FROM_ERRNO
KFIOC_HAS_MUTEX_SUBSYSTEM
KFIOC_STRUCT_FILE_HAS_PATH
KFIOC_HAS_PATH_LOOKUP
KFIOC_UNREGISTER_BLKDEV_RETURNS_VOID
KFIOC_PARTITION_STATS
KFIOC_HAS_NEW_BLOCK_METHODS
KFIOC_HAS_COMPAT_IOCTL_METHOD
KFIOC_COMPAT_IOCTL_RETURNS_LONG
KFIOC_DISCARD
KFIOC_DISCARD_GRANULARITY_IN_LIMITS
KFIOC_BARRIER
KFIOC_USE_LINUX_UACCESS_H
KFIOC_HAS_SPIN_LOCK_IRQSAVE_NESTED
KFIOC_MODULE_PARAM_ARRAY_NUMP
KFIOC_OWNER_IN_STRUCT_PROC_DIR_ENTRY
KFIOC_CONFIG_PREEMPT_RT
KFIOC_CONFIG_TREE_PREEMPT_RCU
KFIOC_HAS_BLK_QUEUE_HARDSECT_SIZE
KFIOC_HAS_END_REQUEST
KFIOC_USE_IO_SCHED
KFIOC_USE_NEW_IO_SCHED
KFIOC_HAS_ELV_DEQUEUE_REQUEST
KFIOC_HAS_BIO_RW_FLAGGED
KFIOC_HAS_INFLIGHT_RW
KFIOC_HAS_INFLIGHT_RW_ATOMIC
KFIOC_HAS_DMA_ERROR_CODE
KFIOC_HAS_BLK_LIMITS_IO_MIN
KFIOC_HAS_BLK_LIMITS_IO_OPT
KFIOC_HAS_BLK_QUEUE_MAX_SEGMENTS
KFIOC_HAS_UNIFIED_BLKTYPES
KFIOC_HAS_BIO_RW_DISCARD
KFIOC_SYSRQ_HANDLER_NEEDS_TTY_STRUCT
KFIOC_PCI_REQUEST_REGIONS_CONST_CHAR
KFIOC_FOPS_USE_LOCKED_IOCTL
KFIOC_HAS_RQ_POS_BYTES
KFIOC_HAS_RQ_IS_SYNC
KFIOC_HAS_RQ_FOR_EACH_BIO
KFIOC_HAS_REQ_RW_SYNC
KFIOC_BIOSET_CREATE_HAS_THIRD_ARG
KFIOC_NEW_BARRIER_SCHEME
KFIOC_HAS_QUEUE_FLAG_CLUSTER
KFIOC_HAS_QUEUE_LIMITS_CLUSTER
KFIOC_HAS_QUEUE_FLAG_CLEAR_UNLOCKED
KFIOC_MAKE_REQUEST_FN_VOID
"


#
# General functions
#

# Pass an argument as a delta from "now" time for the timeout.  If an
# argument is not passed then default to adding TIMEOUT_DELTA to "now".
update_timeout()
{
    TIMEOUT_TIME=$(($(date "+%s")+${1:-$TIMEOUT_DELTA}))
}


# This is a way to log the output of this script in a merged stdout+stderr log file
# while still having stdout and stderr sent to . . . stdout and stderr!
open_log()
{
    # The tee processes will die when this process exits.
    rm -f "$CONFIGDIR/kfio_config.stdout" "$CONFIGDIR/kfio_config.stderr" "$CONFIGDIR/kfio_config.log"
    exec 3>&1 4>&2
    touch "$CONFIGDIR/kfio_config.log"
    mkfifo "$CONFIGDIR/kfio_config.stdout"
    tee -a "$CONFIGDIR/kfio_config.log" <"$CONFIGDIR/kfio_config.stdout" >&3 &
    mkfifo "$CONFIGDIR/kfio_config.stderr"
    tee -a "$CONFIGDIR/kfio_config.log" <"$CONFIGDIR/kfio_config.stderr" >&4 &
    exec >"$CONFIGDIR/kfio_config.stdout"  2>"$CONFIGDIR/kfio_config.stderr"
    # Don't need these now that everything is connected
    rm -f "$CONFIGDIR/kfio_config.stdout" "$CONFIGDIR/kfio_config.stderr"
}


kfioc_open_config() 
{
    cat <<EOF > "${TMP_OUTPUTFILE}"
//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef _FIO_PORT_LINUX_KFIO_CONFIG_H_
#define _FIO_PORT_LINUX_KFIO_CONFIG_H_

EOF

}


kfioc_close_config() 
{
    cat <<EOF >> "${TMP_OUTPUTFILE}"
#include <linux/slab.h>
#include <linux/gfp.h>

#ifndef GFP_NOWAIT
#define GFP_NOWAIT  (GFP_ATOMIC & ~__GFP_HIGH)
#endif

#endif /* _FIO_PORT_LINUX_KFIO_CONFIG_H_ */
EOF

}


collect_kfioc_results()
{
    local flag=
    local pid=
    local status=
    local result=
    local test_fail=
    local rc=0

    for flag in $KFIOC_PROCS; do
        pid=${flag##*:}
        flag=${flag%%:*}
        test_fail=
        status=$(get_kfioc_status $flag exit)
        result=$(get_kfioc_status $flag result)

        if [ -z "$status" ]; then
            printf "ERROR: missing test exit status $flag\n" >&2
            test_fail=1
        elif [ "$status" -ge 128 ]; then
            printf "ERROR: test exited by signal $flag $status\n" >&2
            test_fail=1
        elif [ "$status" -gt 2 ]; then
            printf "ERROR: unexpected test exit status $flag $status\n" >&2
            test_fail=1
        fi

        if [ -z "$result" ]; then
            printf "ERROR: missing test result $flag\n" >&2
            test_fail=1
        elif [ "$result" -gt 1 ]; then
            printf "ERROR: unexpected test value $flag $result\n" >&2
            test_fail=1
        fi

        if [ -n "$test_fail" ]; then
            FAILED_TESTS="$FAILED_TESTS $flag"
            rc=1
        else
            printf "  %.14s  $flag=$result\n" $(date "+%s.%N")
            generate_kfioc_define $flag >>"$TMP_OUTPUTFILE"
        fi
    done

    return $rc
}


fio_create_makefile() 
{
    local kfioc_flag="$1"
    local extra_cflags="${2:-}"

    cat <<EOF >"${CONFIGDIR}/${kfioc_flag}/Makefile"
KERNEL_SRC := ${KERNELDIR}

ifneq (\$(KERNELRELEASE),)

obj-m := kfioc_test.o

else

all: modules

modules clean:
	\$(MAKE) -C \$(KERNEL_SRC) M=\$(CURDIR) EXTRA_CFLAGS='-Wall ${extra_cflags}' \$@

endif

EOF
}


sig_exit()
{
    printf "%.14s  Exiting\n" $(date "+%s.%N")
    # Keep the output files if "PRESERVE" is used or if there are failures.
    if [ -d "$CONFIGDIR" -a 0 -eq "$PRESERVE" -a -z "$FAILED_TESTS" ]; then
        printf "Deleting temporary files (use '-p' to preserve temporary files)\n"
        if [ 1 -eq "$CREATED_CONFIGDIR" ]; then
            rm -rf "${CONFIGDIR}"
        else
            rm -rf "$TMP_OUTPUTFILE" "${CONFIGDIR}/kfio"*
        fi
    elif [ -n "$FAILED_TESTS" ]; then
        printf "Preserving configdir for failure analysis: $CONFIGDIR\n" >&2
        tar \
            --exclude="*kfio_config.tar.gz" \
            -C "$(dirname $CONFIGDIR)" \
            -c -z \
            -f "$CONFIGDIR/kfio_config.tar.gz" \
            "$(basename $CONFIGDIR)"
        printf "Submit tar file to support for analysis: '$CONFIGDIR/kfio_config.tar.gz'\n" >&2
    else
        printf "Preserving configdir due to '-p' option: $CONFIGDIR\n"
    fi
}


# start_test() wraps each test so that script debug can be recorded
start_test()
{
    local test_name="$1"

    mkdir -p "$CONFIGDIR/$test_name"
    exec >"$CONFIGDIR/$test_name/kfio_config.log" 2>&1
    set -x
    $test_name $test_name
}


start_tests()
{
    local kfioc_test=

    # Clean out any old cruft in case building in a previously used directory
    (
        cd "$CONFIGDIR"
        rm -rf KFIOC_* kfio_config.h kfio_config.tar.gz
    )

    printf "Starting tests:\n"
    for kfioc_test in $KFIOC_TEST_LIST; do
        printf "  %.14s  $kfioc_test...\n" $(date "+%s.%N")

        # Each test has an absolute time deadline for completion from when it is started.
        # A test can depend on another test so it needs a timeout to decide that the other
        # test may have failed.
        update_timeout
        start_test $kfioc_test &
        KFIOC_PROCS="$KFIOC_PROCS $kfioc_test:$!"
    done

    printf "Started tests, waiting for completions...\n"

    # We want more time for ourselves than the child tests
    TIMEOUT_DELTA=$(($TIMEOUT_DELTA+$TIMEOUT_DELTA/2))
    update_timeout
}


finished()
{
    local rc=0

    kfioc_open_config || rc=1

    collect_kfioc_results || rc=1

    kfioc_close_config || rc=1

    if [ -z "$FAILED_TESTS" -a 0 = "$rc" ]; then
        cp "${TMP_OUTPUTFILE}" "$OUTPUTFILE"
        printf "Finished\n"
    else
        printf "ERROR: Failures detected\n" >&2
        if [ -n "$FAILED_TESTS" ]; then
            printf "Failed tests: $FAILED_TESTS\n" >&2
            rc=1
        fi
    fi

    return $rc
}


set_kfioc_status()
{
    local kfioc_flag="$1"
    local value="$2"
    local file="$3"
    shift 3

    mkdir -p "$CONFIGDIR/$kfioc_flag"
    # It appears there's a nice race condition where a file is created by
    # the test and the parent reads the data from the file before the write
    # is flushed - resulting in an empty read.  Creating the ".tmp" file
    # and then moving it into place should guarantee that the contents are
    # flushed prior to the file existing.
    printf "%s\n" "$value" >"$CONFIGDIR/$kfioc_flag/$file.tmp"
    mv "$CONFIGDIR/$kfioc_flag/$file.tmp" "$CONFIGDIR/$kfioc_flag/$file"
}


get_kfioc_status()
{
    local kfioc_flag="$1"
    local file="$2"
    local last_status_count="$(ls "$CONFIGDIR/"*/* 2>/dev/null | wc -l)"
    local this_status_count=0

    while [ ! -s "$CONFIGDIR/$kfioc_flag/$file" -a $(date "+%s") -lt "$TIMEOUT_TIME" ]; do
        this_status_count="$(ls "$CONFIGDIR/"*/* 2>/dev/null | wc -l)"
        if [ "$this_status_count" -gt "$last_status_count" ]; then
            last_status_count="$this_status_count"
            update_timeout
        fi
        sleep 1
    done

    if [ -s "$CONFIGDIR/$kfioc_flag/$file" ]; then
        local result="$(cat "$CONFIGDIR/$kfioc_flag/$file")"
        if [ -z "$result" ]; then
            printf "ERROR: empty result in $CONFIGDIR/$kfioc_flag/$file\n" >&2
            exit $EX_SOFTWARE
        fi
        printf "$result"
        update_timeout
    else
        printf "%.14s  ERROR: timed out waiting for $kfioc_flag/$file $$ $last_status_count:$this_status_count $TIMEOUT_TIME\n" $(date "+%s.%N") >&2
        exit $EX_SOFTWARE
    fi
}


generate_kfioc_define()
{
    local kfioc_flag="$1"
    local result=$(get_kfioc_status $kfioc_flag result)

    if [ -n "$result" ]; then
        printf "#define $kfioc_flag ($result)\n"
    fi
}


kfioc_test()
{
    local code="
#include <linux/module.h>
$1
"
    local kfioc_flag="$2"
    # POSITIVE_RESULT is what should be returned when a test compiles sucessfully.
    # This value is inverted when the compile test fails
    local positive_result="$3"
    local cflags="${4:-}"
    local test_dir="${CONFIGDIR}/${kfioc_flag}"
    local result=0
    local license="
MODULE_LICENSE(\"Proprietary\");
"

    mkdir -p "$test_dir"
    fio_create_makefile "$kfioc_flag" "$cflags"

    echo "$code" > "$test_dir/kfioc_test.c"

    if [ 1 -eq "$FUSION_DEBUG" ]; then
        license="
MODULE_LICENSE(\"GPL\");
"
    fi
    echo "$license" >> "$test_dir/kfioc_test.c"

    if [ 1 -eq "$VERBOSE" ]; then
        env -i PATH="${PATH}" make -C "$test_dir" V=1 2>&1 | tee "$test_dir/kfioc_test.log" || result=$?
    else
        env -i PATH="${PATH}" make -C "$test_dir" V=1 >"$test_dir/kfioc_test.log" 2>&1 || result=$?
    fi

    # Save the exit status
    set_kfioc_status $kfioc_flag $result exit

    # Interpret the result
    local myflag=$positive_result
    # Return code of 0 indicates success
    if [ $result != 0 ]; then
        myflag=$((! $positive_result))
    fi

    # Save the interpreted result
    set_kfioc_status $kfioc_flag $myflag result
}


kfioc_has_include()
{
    local include_file="$1"
    local kfioc_flag="$2"

    local test_code="
#include <$include_file>
"

    kfioc_test "$test_code" "$kfioc_flag" 1
}


#
# Actual test procedures for determining Kernel capabilities
#


# flag:           KFIOC_MISSING_WORK_FUNC_T
# usage:          undef for automatic selection by kernel version
#                 0     for non stupid kernels
#                 1     for 2.6.18-92.el5.h
# git commit:
# comments:
KFIOC_MISSING_WORK_FUNC_T()
{
    local test_flag="$1"
    local test_code='
#include <linux/workqueue.h>

void kfio_test_work_func_t(void) {
    work_func_t *fn;
    fn = NULL;
}
'
    
    kfioc_test "$test_code" "$test_flag" 0
}


# flag:           KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT
# values:
#                 0     for older kernels that pass void data to the work function
#                 1     if the kernel passes work queue data in the work struct
# git commit:     65f27f38446e1976cc98fd3004b110fedcddd189
# kernel version: >= 2.6.20
# comments:       Possible ABI compatibility if older kernel doesn't pass
#                 workstruct as data - may need individual .o
KFIOC_WORKDATA_PASSED_IN_WORKSTRUCT()
{
    local test_flag="$1"
    local test_code='
#include <linux/workqueue.h>

struct work_struct *wq;
work_func_t func;

void tst(void)
{
    INIT_WORK(wq, func);
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_PCI_ERROR_HANDLERS
# values:
#                 0     for older kernels
#                 1     For kernels that have pci error handlers.
# git commit:     392a1ce761bc3b3a5d642ee341c1ff082cbb71f0
# kernel version: >= 2.6.18
KFIOC_HAS_PCI_ERROR_HANDLERS()
{
    local test_flag="$1"
    local test_code='
#include <linux/pci.h>

void kfioc_test_pci_error_handlers(void) {
    struct pci_error_handlers e;
    (void)e;
}
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_GLOBAL_REGS_POINTER
# values:
#                 0     for older kernel that pass pt_regs arguments to handlers
#                 1     for kernels that have a global regs pointer.
# git commit:     7d12e780e003f93433d49ce78cfedf4b4c52adc5
# comments:       The change in IRQ arguments causes an ABI incompatibility.
KFIOC_HAS_GLOBAL_REGS_POINTER()
{
    local test_flag="$1"
    local test_code='
#include <asm/irq_regs.h>

struct pt_regs *kfioc_set_irq_regs(void) {
    return get_irq_regs();
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK
# values:
#                 0     for older kernels that removed enable_mask in struct sysrq_key_op
#                 1     for kernels that have enable_mask member in struct sysrq_key_op
#                 in struct sysrq_key_op.
# git commit:     NA
# kernel version: >= 2.6.12
# comments:       Structure size change causes ABI incompatibility.
KFIOC_HAS_SYSRQ_KEY_OP_ENABLE_MASK()
{
    local test_flag="$1"
    local test_code='
#include <linux/sysrq.h>

int kfioc_test_sysrq_key_op(void) {
    struct sysrq_key_op op = {};
    return op.enable_mask;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS
# values:
#                 0     for older kernels that have read and write data members in disk stats
#                 1     for kernels that have arrays for disk ops
#                 rather than individual structures for read/write ops.
# git commit:     a362357b6cd62643d4dda3b152639303d78473da
# kernel version: >= 2.6.15
# comments:       While this is an API change, it doesn't appear to be an ABI
#                 incompatibility in the structure.
#                 This changed disk_stats from independent read/write members to arrays length two.
KFIOC_HAS_DISK_STATS_READ_WRITE_ARRAYS()
{
    local test_flag="$1"
    local test_code='
#include <linux/genhd.h>

void kfioc_test_disk_stats(void) {
    struct disk_stats stat = { .sectors = { [0] = 0, [1] = 0 } };
    (void)stat;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_LINUX_SCATTERLIST_H
# values:
#                 0     for older kernels that had asm/scatterlist.h
#                 1     for kernels that have linux/scatterlist.h
# git commit:     NA
# comments:
KFIOC_HAS_LINUX_SCATTERLIST_H()
{
    local test_flag="$1"
    local test_code='
#include <asm/scatterlist.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_KMEM_CACHE_CREATE_REMOVED_DTOR
# values:
#                 0     for older kernels that have the dtor argument to kmem_cache_create
#                 1     for kernels that removed the dtor argument to kmem_cache_create
# git commit:     c59def9f222d44bb7e2f0a559f2906191a0862d7
# comments:       API and ABI incompatible.
KFIOC_KMEM_CACHE_CREATE_REMOVED_DTOR()
{
    local test_flag="$1"
    local test_code='
#include <linux/slab.h>
    
void kfioc_test_kmem_cache_create(void) {
    kmem_cache_create("foo", 0, 0, 0, NULL);
}
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_BIO_ENDIO_REMOVED_BYTES_DONE
# values:
#                 0     for older kernels that have the bytes_done argument to bio_endio
#                 1     for kernels that removed the bytes_done argument to bio_endio
# git commit:     6712ecf8f648118c3363c142196418f89a510b90
# comments:       API and ABI incompatible.
KFIOC_BIO_ENDIO_REMOVED_BYTES_DONE()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>
    
void kfioc_test_bio_endio(void) {
    bio_endio(NULL, 0);
}
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS
# values:
#                 0     for older kernels that have the destroy_dirty_buffers argument to invalidate_bdev()
#                 1     for kernels that removed the destroy_dirty_buffers argument to invalidate_bdev()
# git commit:     f98393a64ca1392130724c3acb4e3f325801d2b6
# comments:       API and ABI incompatible.
KFIOC_INVALIDATE_BDEV_REMOVED_DESTROY_DIRTY_BUFFERS()
{
    local test_flag="$1"
    local test_code='
#include <linux/buffer_head.h>

void kfioc_test_invalidate_bdev(void) {
    invalidate_bdev(NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_NEEDS_VIRT_TO_PHYS
# values:
#                 0     for kernels that provide correct virt_to_phys()
#                 1     for older kernels that incorrectly define virt_to_phys() as extern.
# git commit:
# comments:       There was an error defining virt_to_phys() as "extern inline" and only in
#                 x86_64.  It was later corrected.
KFIOC_NEEDS_VIRT_TO_PHYS()
{
    local test_flag="$1"
    local result=0

    case $FIOARCH in
    "x86_64")
        if [ -d "$KERNELDIR/include/asm-x86_64" ]; then
                grep -q "extern inline unsigned long virt_to_phys(" "$KERNELSOURCEDIR/include/asm-x86_64/io.h" 2>/dev/null || result=$?
                result=$((! $result))
        fi
        ;;
    "mips64")
        if [ -d "$KERNELDIR/include/asm-mips" ]; then
                grep -q "unsigned long virt_to_phys(" "$KERNELSOURCEDIR/include/asm-mips/io.h" 2>/dev/null || result=$?
                result=$((! $result))
        fi
        ;;
    esac
    set_kfioc_status "$test_flag" 0 exit
    set_kfioc_status "$test_flag" "$result" result
}


# flag:           KFIOC_HAS_BLK_UNPLUG
# values:
#                 0     for older kernels that don't have blk_unplug()
#                 1     for kernels that provide blk_unplug()
# git commit:     2ad8b1ef11c98c5603580878aebf9f1bc74129e4
# comments:
KFIOC_HAS_BLK_UNPLUG()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_test_blk_unplug(void) {
    blk_unplug(NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}


# flag:           KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN
# values:
#                 0     for older kernels that don't have unplug_fn member in struct request_queue.
#                 1     for kernels that have unplug_fn memeber in struct request_queue.
# git commit:     NA
# comments:
KFIOC_REQUEST_QUEUE_HAS_UNPLUG_FN()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_test_request_queue_unplug(void) {
    struct request_queue q = { .unplug_fn = NULL };
    (void)q;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_KMEM_CACHE
# values:
#                 0     for older kernels that use kmem_cache_s
#                 1     for kernels that have kmem_cache
# git commit:     2109a2d1b175dfcffbfdac693bdbe4c4ab62f11f
# comments:       minor name change of the structure.
KFIOC_HAS_KMEM_CACHE()
{
    local test_flag="$1"
    local test_code='
#include <linux/slab.h>

struct kfioc_test_kmem_cache {
    int junk;
};

void kfioc_test_kmem_cache(void) {
    struct kfioc_test_kmem_cache c;
    kmem_cache_destroy((struct kmem_cache *) &c);
    return;
}
'

    kfioc_test "$test_code" "$test_flag" 1 "-Werror"
}


# flag:           KFIOC_HAS_NOTIFIER_FROM_ERRNO
# values:
#                 0     for olders kernels that don't pass errnos in notifiers.
#                 1     for kernels that pass errnos in notifiers.
# git commit:     fcc5a03ac42564e9e255c1134dda47442289e466
# comments:
KFIOC_HAS_NOTIFIER_FROM_ERRNO()
{
    local test_flag="$1"
    local test_code='
#include <linux/types.h>

int kfioc_test_notifier_from_errno(void) {
    return notifier_from_errno(0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}


# flag:           KFIOC_HAS_MUTEX_SUBSYSTEM
# values:
#                 0     for older kernels that don't have mutex subsystem (use semaphores)
#                 1     for kernels with the mutex subsystem
# git commit:     6053ee3b32e3437e8c1e72687850f436e779bd49
# comments:       Semaphores can be used in place of mutexs
KFIOC_HAS_MUTEX_SUBSYSTEM()
{
    local test_flag="$1"
    local test_code='
int kfioc_test_mutex_subsystem(void) {
    struct mutex lock;
    mutex_init(&lock);
    return 0;
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}


# flag:           KFIOC_STRUCT_FILE_HAS_PATH
# values:
#                 0     for older kernels that had separate struct dentry and
#                       struct vfsmount.
#                 1     for newer kernels with struct path in struct file.
# git commit:     0f7fc9e4d03987fe29f6dd4aa67e4c56eb7ecb05
# comments:       API portability.  ABI portability problems because struct vfsmount
#                 and struct dentry are reversed in struct path.
KFIOC_STRUCT_FILE_HAS_PATH()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>

void kfioc_test_file_path(void) {
    struct file f = { .f_path = { [0] = 0, [1] = 0 } };
    (void)f;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_HAS_PATH_LOOKUP
# values:
#                 1 If the kernel still has the path_lookup() helper
#                 0 If it does not (use kern_path_parent())
KFIOC_HAS_PATH_LOOKUP()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>
#include <linux/namei.h>
void kfioc_has_path_lookup(void)
{
    (void) path_lookup("foo", 0, NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_UNREGISTER_BLKDEV_RETURNS_VOID
# values:
#                 0     for kernels that return int from unregister_blkdev()
#                 1     for newer kernels that return void from unregister_blkdev()
# git commit:     f4480240f700587c15507b7815e75989b16825b2
# comments:       
KFIOC_UNREGISTER_BLKDEV_RETURNS_VOID()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>

int kfioc_test_unregister_blkdev(void) {
    int rc = 0;
    rc = unregister_blkdev(0, NULL);
    return rc;
}
'

    kfioc_test "$test_code" "$test_flag" 0
}


# flag:           KFIOC_PARTITION_STATS
# values:
#                 0     for kernels that the driver updates stats
#                 1     for kernels where the block layer updates stats
# git commit:     074a7aca7afa6f230104e8e65eba3420263714a5
# comments:       genhd struct no longer tracks stats.  Stats are tracked
#                 automatically when updating the partition stats by the
#                 block layer.
KFIOC_PARTITION_STATS()
{
    local test_flag="$1"
    local test_code='
#include <linux/genhd.h>
#ifndef part_stat_lock
#error part_stat_lock not defined
#endif
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_NEW_BLOCK_METHODS
# values:
#                 0     for old block open release ioctl methods
#                 1     for new block open release ioctl methods
# git commit:     d4430d62fa77208824a37fe6f85ab2831d274769
# comments:
KFIOC_HAS_NEW_BLOCK_METHODS()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>
#include <linux/blkdev.h>

int myfunc(struct block_device *bdev, fmode_t mode){
    return 0;
}

struct block_device_operations testops = {
    .open = myfunc
};
'

    kfioc_test "$test_code" "$test_flag" 1 "-Werror"
}


# flag:           KFIOC_HAS_COMPAT_IOCTL_METHOD
# values:
#                 0     for old file_operations without compat_ioctl
#                 1     for new file_operations with compat_ioctl
# comments:
KFIOC_HAS_COMPAT_IOCTL_METHOD()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>
#include <linux/blkdev.h>

long myfunc(struct file *file, unsigned cmd, unsigned long arg){
    return 0L;
}

struct file_operations testops = {
    .compat_ioctl = myfunc
};
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_COMPAT_IOCTL_RETURNS_LONG
# values:
#                 1     for compat_ioctl returning long
#                 0     for compat_ioctl returning int
# comments:
KFIOC_COMPAT_IOCTL_RETURNS_LONG()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>
#include <linux/blkdev.h>

long myfunc(struct file *file, unsigned cmd, unsigned long arg){
    return 0L;
}

struct file_operations testops = {
    .compat_ioctl = myfunc
};
'

    kfioc_test "$test_code" "$test_flag" 1 "-Werror"
}


# flag:           KFIOC_DISCARD
# values:
#                 0     for kernel doesn't support TRIM/Discard
#                 1     for kernel supports TRIM/Discard
# comments:
KFIOC_DISCARD()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>


void kfioc_test_blk_queue_set_discard(void) {
	queue_flag_set_unlocked(QUEUE_FLAG_DISCARD,NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_DISCARD_GRANULARITY_IN_LIMITS
# values:
#                 1     for queue has q->limits.discard_granularity
#                 0     it does not
# comments:
KFIOC_DISCARD_GRANULARITY_IN_LIMITS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_test_blk_queue_discard_granularity(void)
{
    struct request_queue q;

    q.limits.discard_granularity = 0;
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_BARRIER
# values:
#                 0     for kernel doesn't support our way to do barriers
#                 1     for kernel support it
# comments:
KFIOC_BARRIER()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>


void kfioc_test_blk_queue_set_discard(void) {
	(void) blk_queue_ordered(NULL,QUEUE_ORDERED_NONE, NULL);
	(void) blk_queue_ordered(NULL,QUEUE_ORDERED_TAG, NULL);
	(void) blk_queue_ordered(NULL,QUEUE_ORDERED_TAG_FLUSH, NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}






# flag:           KFIOC_USE_LINUX_UACCESS_H
# values:
#                 0     for kernels that use asm/uaccess.h
#                 1     for kernels that use linux/uaccess.h
KFIOC_USE_LINUX_UACCESS_H()
{
    local test_flag="$1"
    kfioc_has_include "linux/uaccess.h" "$test_flag"
}


# flag:           KFIOC_HAS_SPIN_LOCK_IRQSAVE_NESTED
# values:
#                 0     for kernels that use asm/uaccess.h
#                 1     for kernels that use linux/uaccess.h
KFIOC_HAS_SPIN_LOCK_IRQSAVE_NESTED()
{
    local test_flag="$1"
    local test_code='
#include <linux/spinlock.h>
#ifndef spin_lock_irqsave_nested
#error spin_lock_irqsave_nested is missing
#endif
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_MODULE_PARAM_ARRAY_NUMP
# values:
#                 0     for kernels that don't take a nump pointer as the fourth argument
#                       to the module_param_array_named() macro (and module_param_array())
#                 1     for kernels that take nump pointer parameters.
# git commit:     Pre-dates git
# comments:
KFIOC_MODULE_PARAM_ARRAY_NUMP()
{
    local test_flag="$1"
    local test_code='
const char *test[10];

module_param_array(test, charp, NULL, 0);
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_OWNER_IN_STRUCT_PROC_DIR_ENTRY
# values:
#                 0     if the kernel has an "owner" data member in "struct proc_dir_entry"
#                 1     if the kernel does not have an "owner" data member in "struct proc_dir_entry"
# git commit:     99b76233803beab302123d243eea9e41149804f3
# kernel version: < 2.6.30
KFIOC_OWNER_IN_STRUCT_PROC_DIR_ENTRY() 
{
    local test_flag="$1"
    local test_code='
#include <linux/proc_fs.h>

void kfioc_owner_in_struct_proc_dir_entry(void) {
    struct proc_dir_entry dentry = { .owner = NULL };
    (void)dentry;
}
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_CONFIG_PREEMPT_RT
# usage:          undef for automatic selection by kernel version
#                 0     if using a non realtime patched kernel
#                 1     if we found the CONFIG_PREEMPT_RT kernel
# kernel version: patched RT kernels
KFIOC_CONFIG_PREEMPT_RT()
{
    local test_flag="$1"
    local test_code='
#ifndef CONFIG_PREEMPT_RT
#error Not running a PREEMPT_RT kernel
#endif
'
    
    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_CONFIG_TREE_PREEMPT_RCU
# usage:          undef for automatic selection by kernel version
#                 0     if using kernel with undefined CONFIG_TREE_PREEMPT_RCU
#                 1     if using kernel with defined CONFIG_TREE_PREEMPT_RCU
# kernel version:
KFIOC_CONFIG_TREE_PREEMPT_RCU()
{
    local test_flag="$1"
    local test_code='
#ifndef CONFIG_TREE_PREEMPT_RCU
#error Not running a PREEMPT_TREE_RCU kernel
#endif
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_BLK_QUEUE_HARDSECT_SIZE
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the blk_queue_hardsect_size function
#                 1     if the kernel has the function
# git commit:     
# kernel version: < 2.6.31
KFIOC_HAS_BLK_QUEUE_HARDSECT_SIZE()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_has_blk_queue_hardsect_size(void){
    struct request_queue *rq = NULL;
    blk_queue_hardsect_size(rq, 512);
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_HAS_END_REQUEST
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the blk_queue_hardsect_size function
#                 1     if the kernel has the function
# git commit: f0f0052069989b80d2a3e50c9cd2f2a650bc1aea
#             This interface was changed and no longer exists on 2.6.25 or later
# kernel version: < 2.6.25
KFIOC_HAS_END_REQUEST()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_has_end_request(void){
    end_that_request_first(NULL, 0, 0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_USE_IO_SCHED
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the blk_queue_hardsect_size function
#                 1     if the kernel has the function
# git commit:     
# kernel version: > 2.6.24 and < 2.6.31
KFIOC_USE_IO_SCHED()
{
    local test_flag="$1"
    # FIXME - this is a poor way to test for a GPL symbol since there is no guarantee
    # that a patch, either way, isn't pulled into a vendor-specific kernel.
    local test_code='
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,24)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
#error Cant use elevator due to required GPL symbol
#endif
#endif

#include <linux/blkdev.h>
void kfioc_use_io_sched(void) {
    blk_complete_request(NULL);
}
'

    kfioc_test "$test_code" KFIOC_USE_IO_SCHED 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_USE_NEW_IO_SCHED
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the blk_queue_hardsect_size function
#                 1     if the kernel has the function
# git commit:     2e46e8b27aa57c6bd34b3102b40ee4d0144b4fab
# kernel version: > 2.6.24 and < 2.6.31
KFIOC_USE_NEW_IO_SCHED()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_new_sched(void) {
    blk_fetch_request(NULL);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_ELV_DEQUEUE_REQUEST
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the blk_queue_hardsect_size function
#                 1     if the kernel has the function
KFIOC_HAS_ELV_DEQUEUE_REQUEST()
{
    local test_flag="$1"
    local test_code='
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,24)
#error elv_dequeue_request is either GPL or doesnt exist
#endif
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_HAS_BIO_RW_FLAGGED
#                 0     if the kernel does not have the bio_rw_flagged
#                 1     if the kernel has the function
KFIOC_HAS_BIO_RW_FLAGGED()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_bio_rw_flagged()
{
    (void)bio_rw_flagged(NULL, BIO_RW_SYNCIO);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}
 

# flag:           KFIOC_HAS_INFLIGHT_RW
#                 1     if the kernel has part0.in_flight[rw]
#                 0     if the kernel does not
KFIOC_HAS_INFLIGHT_RW()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_inflight_rw_stats(void)
{
    struct gendisk gd;
    gd.part0.in_flight[0] = 0;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_HAS_INFLIGHT_RW_ATOMIC
#                 1     if the kernel has part0.in_flight[rw] as atomics
#                 0     if not
KFIOC_HAS_INFLIGHT_RW_ATOMIC()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
#include <linux/atomic.h>
void kfioc_has_inflight_rw_atomic(void)
{
    struct gendisk gd;
    atomic_set(&gd.part0.in_flight[0], 0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror
}
 
# flag:           KFIOC_HAS_DMA_ERROR_CODE
#                 0     if the kernel defines DMA_ERROR_CODE
#                 1     if the kernel defines bad_dma_addr
KFIOC_HAS_DMA_ERROR_CODE()
{
    local test_flag="$1"
    local test_code='
#include <linux/dma-mapping.h>
void kfioc_has_dma_error_code()
{
    dma_addr_t foo = DMA_ERROR_CODE;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_HAS_BLK_LIMITS_IO_MIN
#                 1     if the kernel defines blk_limits_io_min
#                 0     if the kernel does not
KFIOC_HAS_BLK_LIMITS_IO_MIN()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_blk_limits_io_min()
{
    blk_limits_io_min(NULL, 0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_BLK_LIMITS_IO_OPT
#                 1     if the kernel defines blk_limits_io_opt
#                 0     if the kernel does not
KFIOC_HAS_BLK_LIMITS_IO_OPT()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_blk_limits_io_opt()
{
    blk_limits_io_opt(NULL, 0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_BLK_QUEUE_MAX_SEGMENTS
#                 1     if the kernel defines blk_queue_max_segments
#                 0     if the kernel does not
# git commit:     8a78362c4eefc1deddbefe2c7f38aabbc2429d6b
#                 086fa5ff0854c676ec333760f4c0154b3b242616
#                 Changed introduced in 2.6.34
KFIOC_HAS_BLK_QUEUE_MAX_SEGMENTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_blk_queue_max_segments(void)
{
    blk_queue_max_segments(NULL, 0);
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_UNIFIED_BLKTYPES
#                 1     if the kernel supports unified bio/rq flags
#                 0     if the kernel does not
# git commit:     7cc015811ef8992dfcce314d0ed9642bc18143d1
#                 Change introduced in 2.6.36
KFIOC_HAS_UNIFIED_BLKTYPES()
{
    local test_flag="$1"
    local test_code='
#include <linux/blk_types.h>
void foo(void)
{
}
'

    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:           KFIOC_HAS_BIO_RW_DISCARD
#                 1     if the kernel supports has a bio DISCARD flag
#                 0     if the kernel does not
KFIOC_HAS_BIO_RW_DISCARD()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>
void foo(void)
{
    unsigned long flags = 1 << BIO_RW_DISCARD;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}


# flag:           KFIOC_SYSRQ_HANDLER_NEEDS_TTY_STRUCT
# usage:          undef for automatic selection by kernel version
#                 0     if the sysrq handler functions take a tty_struct argument.
#                 1     if the kernel has the function
# kernel version: < 2.6.36
KFIOC_SYSRQ_HANDLER_NEEDS_TTY_STRUCT()
{
    local test_flag="$1"
    local test_code='
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#error "sysrq handlers do not need tty_struct."
#endif
'
    kfioc_test "$test_code" KFIOC_SYSRQ_HANDLER_NEEDS_TTY_STRUCT 1 -Werror
}

# flag:           KFIOC_FOPS_USE_LOCKED_IOCTL
#                 1     if the driver should use fops->ioctl()
#                 0     if the driver should use fops->unlocked_ioctl()
#                 Change introduced in 2.6.36
KFIOC_FOPS_USE_LOCKED_IOCTL()
{
    local test_flag="$1"
    local test_code='
#include <linux/fs.h>
void foo(void)
{
    struct file_operations fops;
    fops.ioctl = NULL;
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:           KFIOC_HAS_RQ_POS_BYTES
#                 1     if the driver should use 
#                 0     if the driver should use 
#                 Change introduced in 2e46e8b27aa57c6bd34b3102b40ee4d0144b4fab
KFIOC_HAS_RQ_POS_BYTES()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void foo(void)
{
    struct request *req = NULL;
    sector_t foo;

    foo = blk_rq_pos(req);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_RQ_IS_SYNC
#                 1     if the driver should use 
#                 0     if the driver should use 
#                 Change introduced in 1faa16d22877f4839bd433547d770c676d1d964c
KFIOC_HAS_RQ_IS_SYNC()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void foo(void)
{
    struct request *req = NULL;

    rq_is_sync(req);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_HAS_RQ_FOR_EACH_BIO
#                 1     if the driver should use 
#                 0     if the driver should use 
#                 Change introduced in 5705f7021748a69d84d6567e68e8851dab551464
KFIOC_HAS_RQ_FOR_EACH_BIO()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void foo(void)
{
    struct bio *lbio;
    struct request *req = NULL;

    __rq_for_each_bio(lbio, req)
    {
    }
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:           KFIOC_HAS_REQ_RW_SYNC
#                 1     if the environment has this defined
#                 0     if the environment does not have this defined
KFIOC_HAS_REQ_RW_SYNC()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
int foo(void)
{
    return (REQ_RW_SYNC);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}
# flag:           KFIOC_BIOSET_CREATE_HAS_THIRD_ARG
#                 1     if the driver should use 
#                 0     if the driver should use 
KFIOC_BIOSET_CREATE_HAS_THIRD_ARG()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>
void foo(void)
{
    bioset_create(0,0,0);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:          KFIOC_PCI_REQUEST_REGIONS_CONST_CHAR
# usage:         1   pci_request_regions(..) has a const second parameter.
#                0   pci_request_regions() has no const second parameter.
KFIOC_PCI_REQUEST_REGIONS_CONST_CHAR()
{
    local test_flag="$1"
    local test_code='
#include <linux/pci.h>
int kfioc_pci_request_regions_const_param(void)
{
    return pci_request_regions(NULL, (const char *) "foo");
}
'
    kfioc_test "$test_code" KFIOC_PCI_REQUEST_REGIONS_CONST_CHAR 1 -Werror
}

# flag:          KFIOC_NEW_BARRIER_SCHEME
# usage:         1   Kernel uses the new barrier scheme
#                0   It does not
KFIOC_NEW_BARRIER_SCHEME()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_hew_barrier_scheme(void)
{
    struct request_queue q;

    q.flush_flags = REQ_FLUSH | REQ_FUA;
}
'
    kfioc_test "$test_code" KFIOC_NEW_BARRIER_SCHEME 1 -Werror
}

# flag:          KFIOC_HAS_QUEUE_FLAG_CLUSTER
# usage:         1   Kernel has QUEUE_FLAG_CLUSTER
#                0   It does not
KFIOC_HAS_QUEUE_FLAG_CLUSTER()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
int has_queue_flag_cluster(void)
{
    return QUEUE_FLAG_CLUSTER ? 1 : 0;
}
'
    kfioc_test "$test_code" KFIOC_HAS_QUEUE_FLAG_CLUSTER 1 -Werror
}

# flag:          KFIOC_HAS_QUEUE_LIMITS_CLUSTER
# usage:         1   request queue limits structure has 'cluster' member.
#                0   It does not
KFIOC_HAS_QUEUE_LIMITS_CLUSTER()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void has_queue_limits_cluster(void)
{
     struct request_queue q;
     q.limits.cluster = 0;
     printk("%d", q.limits.cluster);
}
'
    kfioc_test "$test_code" KFIOC_HAS_QUEUE_LIMITS_CLUSTER 1 -Werror
}

# flag:          KFIOC_HAS_QUEUE_FLAG_CLEAR_UNLOCKED
# usage:         1   request queue limits structure has 'queue_flag_clear_unlocked' function.
#                0   It does not
KFIOC_HAS_QUEUE_FLAG_CLEAR_UNLOCKED()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void has_queue_flag_clear_unlocked(void)
{
     struct request_queue q;
     queue_flag_clear_unlocked(0, &q);
}
'
    kfioc_test "$test_code" KFIOC_HAS_QUEUE_FLAG_CLEAR_UNLOCKED 1 -Werror
}

# flag:          KFIOC_MAKE_REQUEST_FN_VOID
# usage:         1   make_request_fn returns void
#                0   It returns 0/1 for done/remap
KFIOC_MAKE_REQUEST_FN_VOID()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
static void my_make_request_fn(struct request_queue *q, struct bio *bio)
{
}
void test_make_request_fn(void)
{
    blk_queue_make_request(NULL, my_make_request_fn);
}
'
    kfioc_test "$test_code" KFIOC_MAKE_REQUEST_FN_VOID 1 -Werror
}

usage()
{
    local prog="$1"
    shift

    cat <<EOF
Usage: ${prog} [ -a <arch> ] [ -h ] [ -k <kernel_dir> ] [ -p ] [ -v ] -o <output_file>

-a <arch>            Architecture to build
-d <tmp_dir>         Temporary directory to use for config tests
-h                   Usage information (yep, you're reading it now)
-k <kernel_dir>      Kernel "build" directory or the "O" output directory
-o <output_file>     Output file for the config settings (REQUIRED)
-p                   Preserve temporary files in the temporary directory
-s <kernel_src_dir>  Kernel "source" directory if it is different then the "-k <kernel_dir>" directory
-v                   Verbose output
EOF
}
 

#
# Main code block
#
main()
{
    local rc=0
    local option=
    while getopts a:d:hk:o:ps:vl: option; do
        case $option in
            (a)
                FIOARCH="$OPTARG"
                ;;
            (d)
                CONFIGDIR="$OPTARG"
                ;;
            (h)
                usage "$0"
                exit $EX_OK
                ;;
            (k)
                KERNELDIR="$OPTARG"
                ;;
            (o)
                OUTPUTFILE="$OPTARG"
                ;;
            (p)
                PRESERVE=1
                ;;
            (s)
                KERNELSOURCEDIR="$OPTARG"
                ;;
            (v)
                VERBOSE=1
                ;;
            (l)
                FUSION_DEBUG="$OPTARG"
                ;;
        esac
    done
    shift $(($OPTIND - 1))

    if [ -z "$OUTPUTFILE" ]; then
        printf "ERROR: '-o' output file argument is required\n\n" >&2
        usage "$0" >&2
        exit $EX_USAGE
    fi

    trap sig_exit EXIT

    if [ -z "$CONFIGDIR" ]; then
        CREATED_CONFIGDIR=1
        : "${CONFIGDIR:=$(mktemp -t -d kfio_config.XXXXXXXX)}"
    elif ! [ -d "$CONFIGDIR" ]; then
        CREATED_CONFIGDIR=1
        mkdir -p "$CONFIGDIR"
    fi

    # This should be opened as soon as the CONFIGDIR is available
    open_log

    : "${TMP_OUTPUTFILE:=${CONFIGDIR}/kfio_config.h}"

    cat <<EOF
Detecting Kernel Flags
Config dir         : ${CONFIGDIR}
Output file        : ${OUTPUTFILE}
Kernel output dir  : ${KERNELDIR}
Kernel source dir  : ${KERNELSOURCEDIR}
EOF

    # Check to make sure the kernel build directory exists
    if [ ! -d "$KERNELDIR" ]; then
        echo "Unable to locate the kernel build dir '$KERNELDIR'"
        echo "Please make sure your kernel development package is installed."
        exit $EX_OSFILE;
    fi

    # Check to make sure the kernel build directory exists
    if [ ! -d "${KERNELSOURCEDIR:=${KERNELDIR}}" ]; then
        echo "Unable to locate the kernel source dir '$KERNELSOURCEDIR'"
        echo "Please make sure your kernel development package is installed."
        exit $EX_OSFILE;
    fi

    start_tests

    finished || rc=$?

    return $rc
}


if [ "$#" -gt 0 ]; then
    main "$@"
else
    main
fi
