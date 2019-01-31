# IOMemory-VSL

This is an unsupported update of the original driver source for FusionIO
cards. It comes with no warranty, it may cause DATA LOSS or CORRUPTION.
Therefore it is NOT meant for production use, just for testing purposes.

## Current version
At the moment there is a lot of code redesign ongoing. You can find the
bleeding edge development in the next_generation branch. Despite its name
it is more mature than the master. Especially it fixes several known bugs:
- silent IO drops
- crashes during driver unload
- SCSI queue settings
- BIO status handling

## Important note!!!
Only the master branch is tested, nothing else. It is currently running in a 
CEPH cluster with official LTS kernel 4.14 and works quite well. Testing and 
running with the kernel module has been done on Ubuntu and Redhat.
*** the untested branch has only been compiled and loaded not device tested ***

## Background
Driver support for FusionIO cards has been lagging behind kernel
releases, effectively making the cards an expensive paperweight
when running a distribution like Ubuntu which supplies newer kernels.
Deemed a trivial task to update the drivers and actually make them work
with said newer kernels, and putting the expensive paperweight to use again
so I could access my data..., set forking and fixing the code in motion 
quite a while ago.

## Building
If you are on CentOS or similiar distribution simply run
```
git clone https://github.com/snuf/iomemory-vsl
cd iomemory-vsl/
rpmbuild -ba fio-driver.spec
```
Otherwise module building can be done according to the original README.

## Installation
Installation can be done according to the original README.

## DKMS
A dkms.conf file is supplied, so it should be plug and play:
```
sudo cp -r iomemory-vsl/root/usr/src/iomemory-vsl-3.2.15 /usr/src/
sudo mkdir -p /var/lib/dkms/iomemory-vsl/3.2.15/build
sudo ln -s /usr/src/iomemory-vsl-3.2.15 /var/lib/dkms/iomemory-vsl/3.2.15/source
sudo dkms build -m iomemory-vsl -v 3.2.15
sudo dkms install -m iomemory-vsl -v 3.2.15
sudo modprobe iomemory-vsl
```
With fio-utils installed you should see the following kind of...:
```
snuf@scipio:~/Downloads/ark/usr/bin$ sudo ./fio-status 
[sudo] password for snuf: 

Found 2 ioMemory devices in this system with 1 ioDrive Duo
Driver version: 3.2.15 build 1700

Adapter: Dual Adapter
    Fusion-io ioDrive Duo 640GB, Product Number:FS3-204-320-CS, SN:440178, FIO SN:440178
    External Power: NOT connected
    PCIe Power limit threshold: 24.75W
    Connected ioMemory modules:
      fct0: SN:443248
      fct1: SN:443247

fct0    Attached
    ioDIMM3 320G MLC, SN:443248
    Located in slot 0 Upper of ioDrive Duo HL SN:440178
    Last Power Monitor Incident: 693 sec
    PCI:0b:00.0, Slot Number:17
    Firmware v7.1.13, rev 109322 Public
    320.00 GBytes device size
    Internal temperature: 49.71 degC, max 51.68 degC
    Reserve space status: Healthy; Reserves: 100.00%, warn at 10.00%
    Contained VSUs:
      fioa: ID:0, UUID:43836c20-2782-4279-91a3-25ac72c1a270

fioa    State: Online, Type: block device
    ID:0, UUID:43836c20-2782-4279-91a3-25ac72c1a270
    320.00 GBytes device size

fct1    Attached
    ioDIMM3 320G MLC, SN:443247
    Located in slot 1 Lower of ioDrive Duo HL SN:440178
    Last Power Monitor Incident: 693 sec
    PCI:0c:00.0, Slot Number:10
    Firmware v7.1.13, rev 109322 Public
    320.00 GBytes device size
    Internal temperature: 52.17 degC, max 53.65 degC
    Reserve space status: Healthy; Reserves: 100.00%, warn at 10.00%
    Contained VSUs:
      fiob: ID:0, UUID:e6cf3046-bf97-49c2-810e-81e14620b1d8

fiob    State: Online, Type: block device
    ID:0, UUID:e6cf3046-bf97-49c2-810e-81e14620b1d8
    320.00 GBytes device size
```

## Other notes
Installing the fio-util, fio-common, fio-preinstall and fio-sysvinit are
recommended.
When moving from source 2.3.11 to source 3.2.10 the firmware HAS TO BE UPDATED
to 3.2.10.20150212. For the move to 3.2.15 from 3.2.10 I've not seen any issues
with my card.

