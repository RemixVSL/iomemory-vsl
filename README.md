# IOMemory-VSL
This is an unsupported update of the original driver source for FusionIO
cards. It comes with no warranty, it may cause DATA LOSS or CORRUPTION.
Therefore it is NOT meant for production use, just for testing purposes.

## Background
Driver support for FusionIO cards has been lagging behind kernel releases, effectively making these cards an expensive paperweight when running a distribution like Ubuntu / Arch / Fedora / ProxMox which all supply newer kernels than supported.

## Releases
Releases are tagged, and should be checked out by their tag. The release tags follow Linux Kernel versions. E.g. **v5.6.0 (MEGACHONKER)** will work on all 5.x kernels that are 5.6 and lower, but is not guaranteed to work on 5.7. **v4.20.0 - Grey Dawn** supports most kernels that pre-date 5.3.

| Tag | Codename |
| --- | --- |
| v5.6.0 | [MEGACHONKER](https://www.reddit.com/r/Chonkers/) |
| v4.20.0 | Grey Dawn |

## Versions
The driver is derived from the original iomemory-vsl-3.2.16, but has several fixes and gone through rigorous cleaning of redundant unused and old code. Active development is done on master, while *tags* are used for releases. The new releases are all tagged from master, whereas the v4.20 releases are tagged from fio-3.2.16.1732.

The modified source fixes several known bugs in the driver:
- silent IO drops
- crashes during driver unload
- SCSI queue settings
- BIO status handling

## Important note!!!
Commits to master are not "always" write tested, just compile tested. Releases have gone through testing with Flexible I/O Tester. Testing for page_cache errors, and generic FIO checksumming on read and write and different block sizes.

## Building
### Source
```
git clone https://github.com/snuf/iomemory-vsl
cd iomemory-vsl/
git checkout <release-tag>
cd root/usr/src/iomemory-vsl-3.2.16
make gpl
sudo insmod iomemory-vsl.ko
```
### .deb Ubuntu / Debian
```
git clone https://github.com/snuf/iomemory-vsl
cd iomemory-vsl
git checkout <release-tag>
make dpkg
```

### .rpm CentOS / RHEL
```
git clone https://github.com/snuf/iomemory-vsl
cd iomemory-vsl/
git checkout <release-tag>
make rpm
```

## Installation
Installation can be done with created packages, DKMS or other options described in the original README.


## DKMS
Dynamic Kernel Module Support automates away the requirement of having to repackage the kernel module with every kernel and headers update that takes place on the system.
```
git clone https://github.com/snuf/iomemory-vsl
cd iomemory-vsl/
git checkout <release-tag>
make dkms
```

# Utils
With fio-utils installed and passing a single device through to a VM  you should see the following kind of...:
```
vagrant@fio:~/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16$ sudo fio-status -a

Found 1 ioMemory device in this system with 1 ioDrive Duo
Driver version: 3.2.16 build 1731

Adapter: Dual Adapter
	Fusion-io ioDrive Duo 640GB, Product Number:FS3-204-320-CS, SN:440178, FIO SN:440178
	ioDrive Duo HL, PN:00190000108
	External Power: NOT connected
	PCIe Bus voltage: avg 11.93V min 11.88V max 11.94V
	PCIe Bus current: avg 0.89A max 1.28A
	PCIe Bus power: avg 10.83W max 15.23W
	PCIe Power limit threshold: 24.75W
	PCIe slot available power: unavailable
	Connected ioMemory modules:
	  fct0:	SN:443248

fct0	Attached
	ioDIMM3 320G MLC, SN:443248
	ioDIMM3 320G MLC, PN:00276700903
	Located in slot 0 Upper of ioDrive Duo HL SN:440178
	Powerloss protection: protected
	PCI:00:04.0, Slot Number:3
	Vendor:1aed, Device:1005, Sub vendor:1aed, Sub device:1010
	Firmware v7.1.13, rev 109322 Public
	320.00 GBytes device size
	Format: v500, 625000000 sectors of 512 bytes
	PCIe slot available power: 25.00W
	PCIe negotiated link: 4 lanes at 2.5 Gt/sec each, 1000.00 MBytes/sec total
	Internal temperature: 63.98 degC, max 64.47 degC
	Internal voltage: avg 1.02V, max 1.02V
	Aux voltage: avg 2.48V, max 2.48V
	Reserve space status: Healthy; Reserves: 100.00%, warn at 10.00%
	Active media: 100.00%
	Rated PBW: 5.00 PB, 99.41% remaining
	Lifetime data volumes:
	   Physical bytes written: 29,684,767,106,744
	   Physical bytes read   : 34,299,628,056,072
	RAM usage:
	   Current: 136,650,432 bytes
	   Peak   : 2,621,233,792 bytes
	Contained VSUs:
	  fioa:	ID:0, UUID:43836c20-2782-4279-91a3-25ac72c1a270

fioa	State: Online, Type: block device
	ID:0, UUID:43836c20-2782-4279-91a3-25ac72c1a270
	320.00 GBytes device size
	Format: 625000000 sectors of 512 bytes
```
Where `dmesg` contains the actual driver version tag of the running driver
```
vagrant@fio:~/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16$ dmesg | grep "ioDrive driver"
[91439.667645] <6>fioinf ioDrive driver 745cc88-3.2.16.1731                loading...
```

# Support
Join us on the Discord Server in the Wiki, or create a bug report

## Other notes
Installing the fio-util, fio-common, fio-preinstall and fio-sysvinit are
recommended.
When moving from source 2.3.11 to source 3.2.10 the firmware HAS TO BE UPDATED
to 3.2.10.20150212. For the move to 3.2.15, and 3.2.16 from 3.2.10 I've not seen any issues with my card.
