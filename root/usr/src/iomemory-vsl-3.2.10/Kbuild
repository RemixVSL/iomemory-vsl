# -*- makefile -*-
ifndef FIO_DRIVER_NAME
$(error FIO_DRIVER_NAME is not defined)
endif

ifndef KFIO_LIB
$(error KFIO_LIB is not defined - this is the path to the libkfio.o_shipped file)
endif


$(src)/license.c: $(src)/Kbuild
	printf '#include "linux/module.h"\nMODULE_LICENSE("Proprietary");\n' >$@


obj-m := $(FIO_DRIVER_NAME).o

$(FIO_DRIVER_NAME)-y := main.o
$(FIO_DRIVER_NAME)-y += license.o
$(FIO_DRIVER_NAME)-y += pci.o
$(FIO_DRIVER_NAME)-y += sysrq.o
$(FIO_DRIVER_NAME)-y += driver_init.o
$(FIO_DRIVER_NAME)-y += kfio.o
$(FIO_DRIVER_NAME)-y += errno.o
$(FIO_DRIVER_NAME)-y += state.o
$(FIO_DRIVER_NAME)-y += kcache.o
$(FIO_DRIVER_NAME)-y += kfile.o
$(FIO_DRIVER_NAME)-y += kmem.o
$(FIO_DRIVER_NAME)-y += kmisc.o
$(FIO_DRIVER_NAME)-y += kscatter.o
$(FIO_DRIVER_NAME)-y += ktime.o
$(FIO_DRIVER_NAME)-y += sched.o
$(FIO_DRIVER_NAME)-y += cdev.o

ifeq ($(FIO_SCSI_DEVICE),True)
$(FIO_DRIVER_NAME)-y += kscsi.o
$(FIO_DRIVER_NAME)-y += kscsi_host.o
else
$(FIO_DRIVER_NAME)-y += kblock.o
endif

$(FIO_DRIVER_NAME)-y += kcondvar.o
$(FIO_DRIVER_NAME)-y += kinfo.o
$(FIO_DRIVER_NAME)-y += kexports.o
$(FIO_DRIVER_NAME)-y += khotplug.o

ifeq ($(ARCH),powerpc)
$(FIO_DRIVER_NAME)-y += kbitops.o
endif

ifeq ($(CC),mips64el-gentoo-linux-gnu-gcc)
$(FIO_DRIVER_NAME)-y += kbitops.o
$(FIO_DRIVER_NAME)-y += katomic.o
endif

$(FIO_DRIVER_NAME)-y += kcsr.o
$(FIO_DRIVER_NAME)-y += $(patsubst %_shipped,%,$(KFIO_LIB))
$(FIO_DRIVER_NAME)-y += module_param.o

EXTRA_CFLAGS:= -I$(INCLUDE_DIR)

# This includes a kernel-specific config file (if one exists).
# The file should be put in
# include/fio/port/linux/kfio_config-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
# and #defines the macros that are enumerated in
# include/fio/port/linux/kfio_config.h.
KFIOC_H-VER := fio/port/linux/kfio_config-$(KERNELRELEASE).h
KFIOC_H := $(wildcard $(INCLUDE_DIR)/$(KFIOC_H-VER))
EXTRA_CFLAGS += $(if $(KFIOC_H),-include "$(KFIOC_H-VER)")

INSTALL_MOD_DIR ?= extra/fio

ifneq ($(FIREHOSE_INSTALL_ROOT),)
EXTRA_ARGS=INSTALL_MOD_PATH="$(FIREHOSE_INSTALL_ROOT)"
endif
