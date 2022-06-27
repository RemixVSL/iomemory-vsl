FIO_DRIVER = $(shell grep fio_driver_name fio-driver.spec | awk '{print $$3}' | head -1)
FIO_VERSION = $(shell grep fio_version fio-driver.spec | awk '{print $$3}' | head -1)
FIO_SRC_DIR = root/usr/src/$(shell ls root/usr/src)
FIO_DIR = $(shell git rev-parse --show-toplevel)

all: help

.PHONY: dkms
dkms: clean
	cd ${FIO_SRC_DIR} && \
		$(MAKE) dkms

.PHONY: dpkg
dpkg: clean patch_module_version
	# patch fio_version, fio_short_version in debian/fio_values
	cd ${FIO_DIR} && \
		dpkg-buildpackage -rfakeroot --no-check-builddeps --no-sign

.PHONY: rpm
rpm: clean patch_module_version
	#	patch fio_version in fio-driver.spec
	mkdir -p ~/rpmbuild/SOURCES && \
	tar -zcvf ~/rpmbuild/SOURCES/${FIO_DRIVER}-${FIO_VERSION}.tar.gz \
		--transform s/${FIO_DRIVER}/${FIO_DRIVER}-${FIO_VERSION}/ \
		../${FIO_DRIVER} && \
	cd ${FIO_DIR} && \
			rpmbuild -ba fio-driver.spec

.PHONY: module
module: clean
	cd ${FIO_SRC_DIR} && \
  		$(MAKE) gpl

clean:
	cd ${FIO_SRC_DIR} && \
  		$(MAKE) clean

patch_module_version:
	cd ${FIO_SRC_DIR} && \
		$(MAKE) patch_module_version


define usage
@echo Stub for making dkms, dpkg, the module and clean
@echo usage: make "(dkms|dpkg|rpm|module|clean)"
endef
help:
	$(usage)
