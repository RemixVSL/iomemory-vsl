#!/bin/sh
#
# Extra tests that are used for compatibility with newer kernels.
# This file gets sourced by kfio_config.sh, to keep changes to a minimum
#
NCPUS=$(grep -c ^processor /proc/cpuinfo)
TEST_RATE=$(expr $NCPUS "*" 2)

KFIOC_TEST_LIST="${KFIOC_TEST_LIST}
KFIOC_X_HAS_COARSE_REAL_TS
KFIOC_X_PROC_CREATE_DATA_WANTS_PROC_OPS
"

##
# Override start_tests with out more efficient
##
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
            sleep .1
            KFIOC_COUNT=$( pgrep -fc "kfio_config.sh -a" )
        done
    done

    printf "Started tests, waiting for completions...\n"

    # We want more time for ourselves than the child tests
    TIMEOUT_DELTA=$(($TIMEOUT_DELTA+$TIMEOUT_DELTA/2))
    update_timeout
}

# flag:           KFIOC_X_REQUEST_QUEUE_HAS_QUEUE_LOCK_POINTER
# values:
#                 0     starting 5.0 the request_queue has a spinlock_t for queue_lock
#                 1     pre 5.0 kernels have a spinlock_t *queue_lock in request_queue.
# git commit:     NA
# comments:
KFIOC_X_REQUEST_QUEUE_HAS_QUEUE_LOCK_POINTER()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_test_request_queue_has_queue_lock_pointer(void) {
    spinlock_t *l;
    struct request_queue *q;
    q->queue_lock = l;
}
'

    kfioc_test "$test_code" "$test_flag" 1
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
    const struct proc_ops *pops;
    return proc_create_data(NULL, 0, NULL, pops, NULL);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_X_PART_STAT_REQUIRES_CPU
# values:
#                 0     newer kernels don't need the cpu for stats
#                 1     older kernels need the cpu for stats
# git commit:
# comments:       in newer
KFIOC_X_PART_STAT_REQUIRES_CPU()
{
    local test_flag="$1"
    local test_code='
#include <linux/genhd.h>
struct gendisk *gd;
void kfioc_test_part_stat_requires_cpu(void) {
  part_stat_inc(1, &gd->part0, ios[0]);
}
'

    kfioc_test "$test_code" "$test_flag" 1
}
# flag:           KFIOC_REQUEST_QUEUE_HAS_REQUEST_FN
# values:
#                 0     for newer kernels that don't have request_fn member in struct request_queue.
#                 1     for older kernels that have request_fn member in struct request_queue.
# git commit:     NA
# comments:
KFIOC_X_REQUEST_QUEUE_HAS_REQUEST_FN()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_test_request_queue_request_fn(void) {
    struct request_queue q = { .request_fn = NULL };
    (void)q;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}

# flag:          KFIOC_TASK_HAS_CPUS_ALLOWED
# usage:         1   Task struct has CPUs allowed as mask
#                0   It does not
KFIOC_X_TASK_HAS_CPUS_ALLOWED()
{
    local test_flag="$1"
    local test_code='
#include <linux/sched.h>
void kfioc_check_task_has_cpus_allowed(void)
{
    cpumask_t *cpu_mask = NULL;
    struct task_struct *tsk = NULL;
    tsk->cpus_allowed = *cpu_mask;
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror
}

# flag:           KFIOC_X_BIO_HAS_BIO_SEGMENTS
# usage:          0     if kernel has no bio_segments
#                 1     if kernel has bio_segments
KFIOC_X_BIO_HAS_BIO_SEGMENTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>

void kfioc_test_bio_has_bio_segments(void) {
    struct bio *bio = NULL;
    unsigned segs;
    segs = bio_segments(bio);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:           KFIOC_BIO_HAS_BI_PHYS_SEGMENTS
# usage:          0     if bio has no bi_phys_segments
#                 1     if bio has bi_phys_segments
KFIOC_X_BIO_HAS_BI_PHYS_SEGMENTS()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>

void kfioc_test_bio_has_bi_phys_segments(void) {
	struct bio bio;
	void *test = &(bio.bi_phys_segments);
}
'
    kfioc_test "$test_code" "$test_flag" 1 -Werror-implicit-function-declaration
}

# flag:          KFIOC_X_HAS_BLK_QUEUE_FLAG_OPS
# usage:         1   request queue limits structure has 'queue_flag_clear_unlocked' function.
#                0   It does not
KFIOC_X_HAS_BLK_QUEUE_FLAG_OPS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>
void has_queue_blk_queue_flag_ops(void)
{
     struct request_queue q;
     blk_queue_flag_clear(0, &q);
}
'
    kfioc_test "$test_code" "$test_flag" 1 "-Werror -Werror=frame-larger-than=4096"
}

# flag:           KFIOC_X_BIO_HAS_ERROR
# usage:          1 if bio.bi_error does not exist, 0 if instead
# git commit:
# kernel version: v4.14-rc4
KFIOC_X_BIO_HAS_ERROR()
{
    local test_flag="$1"
    local test_code='
#include <linux/bio.h>

void kfioc_bio_has_error(void) {
    struct bio test_bio;
    (void) test_bio.error;
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_X_REQ_HAS_ERRORS
# usage:          1 if req.errors does not exist, 0 if instead
# git commit:
# kernel version: v4.10
KFIOC_X_REQ_HAS_ERRORS()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_req_has_errors(void) {
    struct request *req;
    req->errors = 1;
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_X_REQ_HAS_ERROR_COUNT
# usage:          1 if req.error_count does not exist, 0 if instead
# git commit:
# kernel version: v4.14-rc4
KFIOC_X_REQ_HAS_ERROR_COUNT()
{
    local test_flag="$1"
    local test_code='
#include <linux/blkdev.h>

void kfioc_req_has_error_count(void) {
    struct request *req;
    req->error_count = 1;
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_X_BOUNCE_H
# usage:          1 if no bounce.h, 0 has bounce.h
#                 bounce was seperated out from highmem
# git commit:
# kernel version: v4.14-rc4
KFIOC_X_BOUNCE_H()
{
    local test_flag="$1"
    local test_code='
#include <linux/bounce.h>

void kfioc_bio_has_error(void) {
    struct bio test_bio;
    (void) test_bio.error;
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_X_HAS_TIMER_SETUP
# usage:          1   linux/time.h has `timer_setup((struct timer_list *)  timer, fusion_timer_callback, 0)
#                 0   not timer_setup, use function and data of timer instead
# git commit:
# kernel version: v4.15
# iomemory-vsl:   3
KFIOC_X_HAS_TIMER_SETUP() {
    local test_flag="$1"
    local test_code='
#include <linux/time.h>

static void timer_callback(struct timer_list *t) {
}

void kfioc_has_timer_setup(void) {
    struct timer_list *timer;
    timer_setup((struct timer_list *)  timer, timer_callback, 0);
}
'
    kfioc_test "$test_code" "$test_flag" 1

}

# flag:            KFIOC_X_HAS_DISK_STATS_NSECS
# usage:           1 struct disk_stats has nsecs member
#                  0 struct is still uses ticks member
# kernel version:  Added in 4.19 to log disk stats with nanoseconds
#                  commit "block: use nanosecond resolution for iostat"
KFIOC_X_HAS_DISK_STATS_NSECS()
{
    local test_flag="$1"
    local test_code='
#include <linux/genhd.h>

void test_has_disk_stats_nsecs(void)
{
    struct disk_stats stat = { .nsecs = 0 };
    (void)stat;
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:            KFIOC_X_HAS_COARSE_REAL_TS
# usage:           1 kernel exports ktime_get_coarse_real_ts64()
#                  0 old kernel with current_kernel_time()
# kernel version:  Added in 4.18 to provide a 64 bit time interface
#                  commit: "timekeeping: Standardize on ktime_get_*() naming"
KFIOC_X_HAS_COARSE_REAL_TS()
{
    local test_flag="$1"
    local test_code='
#include <linux/timekeeping.h>

void test_has_coarse_real_ts(void)
{
    struct timespec64 ts;
    ktime_get_coarse_real_ts64(&ts);
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:            KFIOC_X_HAS_ELEVATOR_INIT
# usage:           1 kernel has 2 parameter elevator_init()
#                  0 newer kernel without that function
# kernel version:  Symbol export removed in 4.18. Since then only internal
#                  commit: "block: unexport elevator_init/exit"
KFIOC_X_HAS_ELEVATOR_INIT()
{
    local test_flag="$1"
    local test_code='
#include <linux/elevator.h>

test_has_elevator_init(void)
{
    struct request_queue *q;
    elevator_init(q, "noop");
}
'
    kfioc_test "$test_code" "$test_flag" 1
}

# flag:           KFIOC_X_PART0_HAS_IN_FLIGHT
# values:
#                 0     beyond 5 the struct has no in_flight
#                 1     older kernels do have in_flight
# git commit:
# comments:       yada
KFIOC_X_PART0_HAS_IN_FLIGHT()
{
    local test_flag="$1"
    local test_code='
#include <linux/genhd.h>
struct gendisk *gd;
void kfioc_test_part0_has_in_flight(void) {
  return gd->part0.in_flight;
}
'

    kfioc_test "$test_code" "$test_flag" 1
}
