#!/bin/sh

#-----------------------------------------------------------------------------
# Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
# Copyright (c) 2014-2018 SanDisk Corp. and/or all its affiliates. (acquired by Western Digital Corp. 2016)
# Copyright (c) 2016-2019 Western Digital Technologies, Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#  * Neither the name of the SanDisk Corp. nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
#  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
NCPUS=$(grep -c ^processor /proc/cpuinfo)
TEST_RATE=$(expr $NCPUS "*" 2)

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

# Several of the compile time tests include linux/slab.h, which eventually include
# linux/types.h.  This has the side effect of including kernel space definitions
# of integral types which may cause issues with print format specifiers.  Later
# Linux kernels include asm-generic/int-ll64.h unconditionally for kernel space
# integral type definitions, while the Linux porting layer stdint.h conditionally
# defines 64 bit integral types with one or two "long" statements dependent on
# architecture.

KFIOC_TEST_LIST="
KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
KFIOC_X_TASK_HAS_CPUS_MASK
KFIOC_X_LINUX_HAS_PART_STAT_H
KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS
KFIOC_X_BLK_ALLOC_QUEUE_EXISTS
KFIOC_X_BLK_ALLOC_DISK_EXISTS
KFIOC_X_HAS_MAKE_REQUEST_FN
KFIOC_X_GENHD_PART0_IS_A_POINTER
KFIOC_X_BIO_HAS_BI_BDEV
KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T
KFIOC_X_VOID_ADD_DISK
KFIOC_X_DISK_HAS_OPEN_MUTEX
"


#
# Experimenting with the removal of tests, hard code the exit to 0....
#
KFIOC_REMOVE_TESTS=""

for remove in $KFIOC_REMOVE_TESTS; do
    echo "Hardcode $remove result to 0"
    eval "${remove}() {
      set_kfioc_status $remove 0 exit
      set_kfioc_status $remove 0 result
    }"
done

#
# Actual test procedures for determining Kernel capabilities
#
##
## Newly added tests HAVE to contain the kernel version it appeared in and an
## LWN reference where the change in the kernel is and some form of reference
## to documentation describing the change in the kernel.
##
####
# flag:            KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T
# usage:           1   Kernels that return blk_qc_t
#                  0   Kernels that return nothing:w

# kernel_version:  In 5.16 moves to bio_submit without a return
KFIOC_X_SUBMIT_BIO_RETURNS_BLK_QC_T()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>
void kfioc_check_submit_bio_returns_blk_qc_t(void)
{
  struct bio *bio = NULL;
  blk_qc_t rc;
  rc = submit_bio(bio);
}

'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_VOID_ADD_DISK
# usage:           1   Kernels that return void
#                  0   Kernels that require checkig the rc
# kernel_version:  In 5.16 wants add_disk to be checked
KFIOC_X_VOID_ADD_DISK()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_check_void_add_disk(void)
{
  struct gendisk *gd = NULL;
  add_disk(gd);
}

'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_DISK_HAS_OPEN_MUTEX
# usage:           1   Kernels that have open_mutex on disks
#                  0   Kernels that have bd_mutex on the bd
# kernel_version:  In 5.14 introduces open_mutex on disks, and removes bd_mutex
#                  from a block device
# driver:          vsl4
KFIOC_X_DISK_HAS_OPEN_MUTEX()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_check_disk_open_mutex(void)
{
  struct gendisk *gd = NULL;
  struct mutex open_mutex;

  open_mutex = gd->open_mutex;
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_BLK_ALLOC_DISK_EXISTS
# usage:           1   Kernels that do have blk_alloc_disk
#                  0   Kernels that don't have blk_alloc_disk
# kernel_version:  In 5.14 blk_alloc_queue got removed and introduced blk_alloc_disk
#                  which replaces alloc_disk and adds the blk_alloq_queue implicit
KFIOC_X_BLK_ALLOC_DISK_EXISTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_check_blk_alloc_disk(void)
{
  struct gendisk *gd;
  int node = 1;

  gd = blk_alloc_disk(node);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_BIO_HAS_BI_BDEV
# usage:           1   bio->bi_bdev exists
#                  0   it doesn't and we assume bio->bi_disk is a thing
# kernel_version:  5.12
# description:     bio->bi_disk was dropped and was moved to
#                  bio->bi_bdev->bd_disk
KFIOC_X_BIO_HAS_BI_BDEV()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_bio_has_bi_bdev(void)
 {
  struct bio *bio = NULL;
  struct gendisk *disk = bio->bi_bdev->bd_disk;
  set_disk_ro(disk, 1);
 }
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_GENHD_PART0_IS_A_POINTER
# usage:           1   genhd disk part is a pointer
#                  0   genhd disk part is not a pointer
# kernel_version:  In 5.11 the gendisk part becomes a pointer and also a
# block_device, which causes two changes in the code. These are reflected by
# two defines in block_meta.h
KFIOC_X_GENHD_PART0_IS_A_POINTER()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
#include <linux/part_stat.h>
void kfioc_genhd_part0_is_a_pointer(void)
{
  struct gendisk *gd = NULL;
  part_stat_set_all(gd->part0, 0);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_HAS_MAKE_REQUEST_FN
# usage:           1   Kernels that do have make_request_fn
#                  0   Kernels that don't have make_request_fn
# kernel_version:  In 5.9 make_request_fn got removed and we move to bio_submit
KFIOC_X_HAS_MAKE_REQUEST_FN()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_has_make_request_fn(void)
{
  struct kfio_disk
  {
      make_request_fn       *make_request_fn;
  };
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS
# usage:           1   Kernels that do have blk_alloc_queue_node
#                  0   Kernels that don't have blk_alloc_queue_node
# kernel_version:  In 5.7 blk_alloc_queue_node got removed and introduced block_alloc_queue,
 #                 which simplifies things
KFIOC_X_BLK_ALLOC_QUEUE_NODE_EXISTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_check_blk_alloc_queue_node(void)
{
  struct request_queue *rq;
  int node = 1;

  rq = blk_alloc_queue_node(GFP_NOIO, node);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_BLK_ALLOC_QUEUE_EXISTS
# usage:           1   Kernels that do have blk_alloc_queue
#                  0   Kernels that don't have blk_alloc_queue
# kernel_version:  In 5.7 blk_alloc_queue node got removed and introduced block_alloc_queue,
#                  which simplifies things
KFIOC_X_BLK_ALLOC_QUEUE_EXISTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void kfioc_check_blk_alloc_queue(void)
{
  struct request_queue *rq;
  int node = 1;
  rq = blk_alloc_queue(node);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:            KFIOC_X_LINUX_HAS_PART_STAT_H
# usage:           1   Kernels that have linux/part_stats.h
#                  0   No kernel before 5.7 has linux/part_stat.h
# kernel_version:  Partition statistics got their own header file in 5.7 and
#                  onwards
KFIOC_X_LINUX_HAS_PART_STAT_H()
{
    local test_flag="$1"
    local test_code='
#include <linux/part_stat.h>
void kfioc_check_linux_has_part_stats_h(void)
{
    part_stat_lock();
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

####
# flag:          KFIOC_X_TASK_HAS_CPUS_MASK
# usage:         1   Task struct has CPUs allowed as mask 5.2 and up
#                0   5.0, 5.1 and 5.2 don't have this
KFIOC_X_TASK_HAS_CPUS_MASK()
{
    local test_flag="$1"
    local test_code='
#include <linux/sched.h>
void kfioc_check_task_has_cpus_mask(void)
{
    cpumask_t *cpu_mask = NULL;
    struct task_struct *tsk = NULL;
    tsk->cpus_mask = *cpu_mask;
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:           KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
# usage:          undef for automatic selection by kernel version
#                 0     if the kernel does not have the proc_create_data function
#                 1     if the kernel has the function
# description:    Between 5.3 and 5.6 the 4th option for proc_create_data changes.
#                 It went from a "const struct file_operations *" to a
#                 const struct proc_ops *.
#                 https://elixir.bootlin.com/linux/v5.3/source/include/linux/proc_fs.h#L44
#                 https://elixir.bootlin.com/linux/v5.6.3/source/include/linux/proc_fs.h#L59
KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS()
{
    local test_flag="$1"
    local test_code='
#include <linux/proc_fs.h>

void *kfioc_has_proc_create_data(struct inode *inode)
{
    const struct proc_ops *pops = NULL;
    return proc_create_data(NULL, 0, NULL, pops, NULL);
}
'
    kfioc_test "$test_code" "$test_flag" 1 "-Werror-implicit-function-declaration -Werror"
}

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
    FIFO_DIR=${FIFO_DIR:=$CONFIGDIR}
    # FIFO_DIR=/tmp
    # The tee processes will die when this process exits.
    rm -f "$FIFO_DIR/kfio_config.stdout" "$FIFO_DIR/kfio_config.stderr" "$FIFO_DIR/kfio_config.log"
    exec 3>&1 4>&2
    touch "$FIFO_DIR/kfio_config.log"
    mkfifo "$FIFO_DIR/kfio_config.stdout"
    tee -a "$FIFO_DIR/kfio_config.log" <"$FIFO_DIR/kfio_config.stdout" >&3 &
    mkfifo "$FIFO_DIR/kfio_config.stderr"
    tee -a "$FIFO_DIR/kfio_config.log" <"$FIFO_DIR/kfio_config.stderr" >&4 &
    exec >"$FIFO_DIR/kfio_config.stdout"  2>"$FIFO_DIR/kfio_config.stderr"
    # Don't need these now that everything is connected
    rm -f "$FIFO_DIR/kfio_config.stdout" "$FIFO_DIR/kfio_config.stderr"
}


kfioc_open_config()
{
    cat <<EOF > "${TMP_OUTPUTFILE}"
//-----------------------------------------------------------------------------
// Copyright (c) 2011-2014, Fusion-io, Inc. (acquired by SanDisk Corp. 2014)
// Copyright (c) 2014-2015, SanDisk Corp. and/or all its affiliates.
// All rights reserved.
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
        start_test $kfioc_test &
        KFIOC_PROCS="$KFIOC_PROCS $kfioc_test:$!"
        KFIOC_COUNT=$( pgrep -fc "kfio_config.sh -a" )
        while [ $KFIOC_COUNT -gt $TEST_RATE ]
        do
            sleep .01
            KFIOC_COUNT=$( pgrep -fc "kfio_config.sh -a" )
        done
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
    # POSITIVE_RESULT is what should be returned when a test compiles successfully.
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

###############################################################################

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
EOF

    # Check to make sure the kernel build directory exists
    if [ ! -d "$KERNELDIR" ]; then
        echo "Unable to locate the kernel build dir '$KERNELDIR'"
        echo "Please make sure your kernel development package is installed."
        exit $EX_OSFILE;
    fi

    # For some reason we used to unilaterally set KERNELSOURCEDIR to KERNELDIR. Maybe because they were the same in many
    #  cases and it didn't seem to matter, the way we used them.
    # With CRT-1114 however, we had to change the way we detect the API changes to get_user_pages(), which requires
    #  that the KERNELDIR point to the kernel build directory and the KERNELSOURCEDIR point to...well, the kernel source.
    # Attempt to correctly set the KERNELSOURCEDIR by checking for the existence of a file. (mm.h because, why not?).
    INCLUDEFILEPATH="include/linux/mm.h"
    if [ ! -f "$KERNELSOURCEDIR/$INCLUDEFILEPATH" ]; then
        KERNELSOURCEDIR=${KERNELDIR}
    fi

    cat <<EOF
Kernel source dir  : ${KERNELSOURCEDIR}
EOF
    # Check again to make sure the kernel souce directory and a file exists
    if [ ! -f "$KERNELSOURCEDIR/$INCLUDEFILEPATH" ]; then
        echo "Unable to locate the kernel source in '$KERNELSOURCEDIR'"
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
