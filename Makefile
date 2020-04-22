all: help

.PHONY: dkms
dkms: clean
	cd root/usr/src/$(shell ls root/usr/src) && \
		$(MAKE) dkms

.PHONY: dpkg
dpkg: clean
	# patch fio_version, fio_short_version in debian/fio_values
	cd $(shell git rev-parse --show-toplevel) && \
		dpkg-buildpackage -rfakeroot --no-check-builddeps --no-sign

.PHONY: rpm
rpm: clean
	#	patch fio_version in fio-driver.spec
	mkdir -p ~/rpmbuild/SOURCES && \
	tar -zcvf ~/rpmbuild/SOURCES/iomemory-vsl-3.2.16.1732.tar.gz \
		--transform s/iomemory-vsl/iomemory-vsl-3.2.16.1732/ \
		../iomemory-vsl && \
	cd $(shell git rev-parse --show-toplevel) && \
			rpmbuild -ba fio-driver.spec

.PHONY: module
module: clean
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) gpl

clean:
	cd root/usr/src/$(shell ls root/usr/src) && \
  	$(MAKE) clean

define usage
@echo Stub for making dkms, dpkg, the module and clean
@echo usage: make "(dkms|dpkg|rpm|module|clean)"
endef
help:
	$(usage)
