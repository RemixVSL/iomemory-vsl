# KERNELRELEASE is set by the kernel build system.  This is used
# as a test to know if the build is being driven by the kernel.
ifneq ($(KERNELRELEASE),)
# Kernel build

# Older kernel scripts/Makefile.build only process Makefile so
# we include the Kbuild file here.  Newer kernel scripts/Makefile.build
# include Kbuild directly and never process Makefile (this file).
include $(FUSION_DRIVER_DIR)/Kbuild

else

ifneq ($(CROSS_COMPILE),)
FIOCC=$(CROSS_COMPILE)gcc
else
FIOCC=$(CC)
endif
$(info    FIOCC is $(FIOCC))

FIO_DRIVER_NAME ?= $(shell cat dkms.conf | grep BUILT_MODULE_NAME | awk -F= '{ print $$2 }')
FIO_DRIVER_SUBDIR ?= extra/fio
FUSION_DEBUG ?= 0
FIO_SCSI_DEVICE ?= 0

ifneq ($(DKMS_KERNEL_VERSION),)
KERNELVER = $(DKMS_KERNEL_VERSION)
else
KERNELVER ?= $(shell uname -r)
endif
KERNEL_BUILD = /lib/modules/$(KERNELVER)/build
CFLAGS = -I$(shell pwd)/include -DBUILDING_MODULE -DLINUX_IO_SCHED -Wall

# Older systems (Debian 6) have the kernel headers in a different directory than the kernel build directory.
# Start in this source directory, and fix it up in the config shell script if it isn't right.
KERNEL_SRC = /lib/modules/$(KERNELVER)/source

# Set PORT_NO_WERROR=0 to compile without -Werror compilation flag
ifneq ($(PORT_NO_WERROR),1)
    CFLAGS += -Werror
endif

# Set FUSION_DEBUG=0 to compile without debugging symbols
ifeq ($(FUSION_DEBUG),1)
    CFLAGS += -g -ggdb
endif
# Target should be a mix of compiler version, platform and cpu this will allow
# us to build multiple targets in the same tree without overlapping objects
# and libs.  We will need to build multiple objects where kernel ifdefs
# differ.
FIOARCH ?= $(shell uname -m)
CCMAJOR ?= $(shell echo __GNUC__ | $(FIOCC) -E -xc - | tail -n 1)
CCMINOR ?= $(shell echo __GNUC_MINOR__ | $(FIOCC) -E -xc - | tail -n 1)
CCVER=$(CCMAJOR)$(CCMINOR)
LAST_CCVER := $(shell ls -1 kfio | sed -n 's/.*_cc\([0-9][0-9]*\)_.*/\1/p' | tail -1)
# Get the tag and push it in here too.... if not too long...
MODULE_VERSION ?= $(shell git rev-parse --short HEAD)

TARGET ?= $(FIOARCH)_cc$(CCMAJOR)$(CCMINOR)

KFIO_LIB ?= kfio/$(TARGET)_libkfio.o
LAST_KFIO_LIB := x86_64_latest_libkfio.o_shipped
KFIO_LIB_CMD ?= kfio/.$(TARGET)_libkfio.o.cmd

NCPUS:=$(shell grep -c ^processor /proc/cpuinfo)

all: fake_license modules patch_module_version

gpl: fake_license modules patch_module_version

.PHONY: fake_license
fake_license:
	sed -i 's/Proprietary/GPL/g' Kbuild

.PHONY: check_n_fix_kfio_ccver
check_n_fix_kfio_ccver:
	if [ ! -f "${KFIO_LIB}" ];then \
		cp kfio/${LAST_KFIO_LIB} ${KFIO_LIB}; \
	fi \
	$(shell touch ${KFIO_LIB_CMD})


clean modules_clean:
	$(MAKE) \
		-j$(NCPUS) \
		-C $(KERNEL_BUILD) \
		FIO_DRIVER_NAME=$(FIO_DRIVER_NAME) \
		FUSION_DRIVER_DIR=$(shell pwd) \
		M=$(shell pwd) \
		ccflags-y+="$(CFLAGS)" \
		EXTRA_CFLAGS+="$(CFLAGS)" \
		KFIO_LIB=$(KFIO_LIB) \
		clean
	rm -rf include/fio/port/linux/kfio_config.h kfio_config

include/fio/port/linux/kfio_config.h: kfio_config.sh
	./$< -a $(FIOARCH) -o $@ -k $(KERNEL_BUILD) -p -d $(CURDIR)/kfio_config -l $(FUSION_DEBUG) -s $(KERNEL_SRC)

.PHONY: check_target_kernel
check_target_kernel:
	@ret="$$(./check_target_kernel.sh "$(KERNELVER)" "$(KERNEL_BUILD)")" ; \
	if [ -n "$$ret" ] ; then \
		echo "" ; \
		echo "Change found in target kernel: $$ret" ; \
		echo "Running clean before building driver" ; \
		echo "" ; \
		$(MAKE) clean ; \
	fi

.PHONY: add_module_version patch_module_version
add_module_version patch_module_version:
	@ret="$$("$(shell pwd)"/module_operations.sh -n "$(FIO_DRIVER_NAME)" -v "$(MODULE_VERSION)")" ; \
	if [ -n "$$ret" ] ; then \
		echo "" ; \
		echo ""$$ret"" ; \
		echo "" ; \
	fi

.PHONY: dkms
dkms:
	sudo ./module_operations.sh -n "$(FIO_DRIVER_NAME)" -v "$(MODULE_VERSION)" -l ${LAST_KFIO_LIB} -d

.PHONY: dpkg
dpkg:
	# patch fio_version, fio_short_version in debian/fio_values
	cd $(shell git rev-parse --show-toplevel) && \
			$(MAKE) dpkg

.PHONY: dpkg
rpm:
	# patch fio_version, fio_short_version in debian/fio_values
	cd $(shell git rev-parse --show-toplevel) && \
			$(MAKE) rpm

modules modules_install: check_target_kernel check_n_fix_kfio_ccver include/fio/port/linux/kfio_config.h add_module_version
	$(MAKE) \
	-j$(NCPUS) \
	-C $(KERNEL_BUILD) \
	FIO_DRIVER_NAME=$(FIO_DRIVER_NAME) \
	FUSION_DRIVER_DIR=$(shell pwd) \
	M=$(shell pwd) \
	EXTRA_CFLAGS+="$(CFLAGS)" \
	ccflags-y+="$(CFLAGS)" \
	INSTALL_MOD_DIR=$(FIO_DRIVER_SUBDIR) \
	INSTALL_MOD_PATH=$(INSTALL_ROOT) \
	KFIO_LIB=$(KFIO_LIB) \
	objtool=$(PWD)/objtool_wrapper \
	$@

endif

# 4.3.7.1205 bryce@a7e621e6fb50 -tag=7bd0632
# 3.2.16.1731 pinnacles@4870ad45b7ea
