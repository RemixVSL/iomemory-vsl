# IOMemory-VSL

This is an unsupported update of the original driver source for FusionIO
cards. It comes with no warranty, it may cause DATA LOSS or CORRUPTION.
Therefore it is NOT meant for production use, just for testing purposes.

## Background
Driver support for FusionIO cards has been lagging behind kernel
releases, effectively making the cards an expensive paperweight
when running a distribution like Ubuntu which supplies newer kernels.
Deemed a trivial task to update the drivers and actually make them work
with said newer kernels, and putting the expensive paperweight to use again
so I could access my data..., set forking and fixing the code in motion quite
a while ago.

## Important note!!!
Only the version branches are tested, nothing else. The driver may
in fact work with older or newer kernels but none have been tested. The
latest reliable tested versions are from 3.19 and upwards. Testing and
running with the kernel module has solely been done on Ubuntu.
*** the untested branch has only been compiled and loaded not device tested ***

## Building
Module building can be done according to the original README.

## Installation
Installation can be done according to the original README.

## DKMS
A dkms.conf file is supplied, so it should be plug and play:
```
sudo cp -r iomemory-vsl/root/usr/src/iomemory-vsl-3.2.10 /usr/src/
sudo mkdir -p /var/lib/dkms/iomemory-vsl/3.2.10/build
sudo ln -s /usr/src/iomemory-vsl-3.2.10 /var/lib/dkms/iomemory-vsl/3.2.10/source
sudo dkms build -m iomemory-vsl -v 3.2.10
sudo dkms install -m iomemory-vsl -v 3.2.10
sudo modprobe iomemory-vsl
```
With fio-utils installed you should see the following:
```
[11:31:43] funs@morbo:/var/lib/dkms/iomemory-vsl/3.2.10$ sudo fio-status
[sudo] password for funs:

Found 1 ioMemory device in this system
Driver version: 3.2.10 build 1509

Adapter: Single Adapter
        HP 320GB MLC PCIe ioDrive for ProLiant Servers, Product Number:600279-B21, SN:459457
        External Power: NOT connected
        PCIe Power limit threshold: 24.75W
        Connected ioMemory modules:
          fct0: Product Number:600279-B21, SN:459457

fct0    Attached
        HP ioDrive 320GB, Product Number:600279-B21, SN:459457
        PCI:05:00.0, Slot Number:10
        Firmware v7.1.15, rev 110356 Public
        320.00 GBytes device size
        Internal temperature: 42.33 degC, max 42.82 degC
        Reserve space status: Healthy; Reserves: 100.00%, warn at 10.00%
        Contained VSUs:
          fioa: ID:0, UUID:13a294d6-3d8f-48b0-a528-1120a628d5e4

fioa    State: Online, Type: block device
        ID:0, UUID:13a294d6-3d8f-48b0-a528-1120a628d5e4
        320.00 GBytes device size
```

## Other notes
Installing the fio-util, fio-common, fio-preinstall and fio-sysvinit are
recommended.
When moving from source 2.3.11 to source 3.2.10 the firmware HAS TO BE UPDATED
to 3.2.10.20150212.
