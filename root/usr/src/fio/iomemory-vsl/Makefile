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

FIO_DRIVER_NAME ?= iomemory-vsl
FIO_DRIVER_SUBDIR ?= extra/fio
FUSION_DEBUG ?= 0

KERNELVER ?= $(shell uname -r)
KERNEL_SRC = /lib/modules/$(KERNELVER)/build
CFLAGS="-I$(shell pwd)/include -DBUILDING_MODULE -DLINUX_IO_SCHED"

# Target should be a mix of compiler version, platform and cpu this will allow
# us to build multiple targets in the same tree without overlapping objects
# and libs.  We will need to build multiple objects where kernel ifdefs
# differ.
FIOARCH ?= $(shell uname -m)
CCMAJOR ?= $(shell echo __GNUC__ | $(FIOCC) -E -xc - | tail -n 1)
CCMINOR ?= $(shell echo __GNUC_MINOR__ | $(FIOCC) -E -xc - | tail -n 1)
CCVER=$(CCMAJOR)$(CCMINOR)
TARGET ?= $(FIOARCH)_cc$(CCMAJOR)$(CCMINOR)

KFIO_LIB ?= kfio/$(TARGET)_libkfio.o_shipped

all: modules

clean modules_clean:
	$(MAKE) \
		-C $(KERNEL_SRC) \
		FIO_DRIVER_NAME=$(FIO_DRIVER_NAME) \
		FUSION_DRIVER_DIR=$(shell pwd) \
		SUBDIRS=$(shell pwd) \
		EXTRA_CFLAGS+=$(CFLAGS) \
		KFIO_LIB=$(KFIO_LIB) \
		clean
	rm -rf include/fio/port/linux/kfio_config.h kfio_config

include/fio/port/linux/kfio_config.h: kfio_config.sh
	./$< -a $(FIOARCH) -o $@ -k $(KERNEL_SRC) -p -d $(CURDIR)/kfio_config -l $(FUSION_DEBUG)

.PHONY: check_target_kernel
check_target_kernel:
	@ret="$$(./check_target_kernel.sh "$(KERNELVER)" "$(KERNEL_SRC)")" ; \
	if [ -n "$$ret" ] ; then \
		echo "" ; \
		echo "Change found in target kernel: $$ret" ; \
		echo "Running clean before building driver" ; \
		echo "" ; \
		$(MAKE) clean ; \
	fi

modules modules_install: check_target_kernel include/fio/port/linux/kfio_config.h
	$(MAKE) \
	-C $(KERNEL_SRC) \
	FIO_DRIVER_NAME=$(FIO_DRIVER_NAME) \
	FUSION_DRIVER_DIR=$(shell pwd) \
	SUBDIRS=$(shell pwd) \
	EXTRA_CFLAGS+=$(CFLAGS) \
	INSTALL_MOD_DIR=$(FIO_DRIVER_SUBDIR) \
	INSTALL_MOD_PATH=$(INSTALL_ROOT) \
	KFIO_LIB=$(KFIO_LIB) \
	$@

endif
