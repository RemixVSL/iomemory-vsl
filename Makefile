FIO_DRIVER = $(shell grep fio_driver_name fio-driver.spec | awk '{print $$3}' | head -1)
FIO_VERSION = $(shell grep fio_version fio-driver.spec | awk '{print $$3}' | head -1)

all: help

.PHONY: dkms
dkms: clean
	cd root/usr/src/$(shell ls root/usr/src) && \
		$(MAKE) dkms

.PHONY: dpkg
dpkg: clean patch_module_version
	# patch fio_version, fio_short_version in debian/fio_values
	cd $(shell git rev-parse --show-toplevel) && \
		dpkg-buildpackage -rfakeroot --no-check-builddeps --no-sign

.PHONY: rpm
rpm: clean patch_module_version
	#	patch fio_version in fio-driver.spec
	mkdir -p ~/rpmbuild/SOURCES && \
	tar -zcvf ~/rpmbuild/SOURCES/${FIO_DRIVER}-${FIO_VERSION}.tar.gz \
		--transform s/${FIO_DRIVER}/${FIO_DRIVER}-${FIO_VERSION}/ \
		../${FIO_DRIVER} && \
	cd $(shell git rev-parse --show-toplevel) && \
			rpmbuild -ba fio-driver.spec

.PHONY: module
module: clean
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) gpl

clean:
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) clean

patch_module_version:
	cd root/usr/src/$(shell ls root/usr/src) && \
		$(MAKE) patch_module_version


define usage
@echo Stub for making dkms, dpkg, the module and clean
@echo usage: make "(dkms|dpkg|rpm|module|clean)"
endef
help:
	$(usage)
