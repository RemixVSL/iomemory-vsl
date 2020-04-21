all: help

.PHONY: dkms
dkms:
	cd root/usr/src/$(shell ls root/usr/src) && \
		$(MAKE) dkms
	# ./module_operations.sh -n "$(FIO_DRIVER_NAME)" -v "$(MODULE_VERSION)" -d

.PHONY: dpkg
dpkg:
	# patch fio_version, fio_short_version in debian/fio_values
	cd $(shell git rev-parse --show-toplevel) && \
		dpkg-buildpackage -rfakeroot --no-check-builddeps --no-sign

.PHONY: module
module:
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) gpl

clean:
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) clean

define usage
@echo Stub for making dkms, dpkg, the module and clean
@echo usage: make "(dkms|dpkg|module|clean)"
endef
help:
	$(usage)
