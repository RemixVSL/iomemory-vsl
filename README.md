<br />
<p align="center">

  <!-- <a href="https://github.com/snuf/iomemory-docs">
    <img src="https://github.com/snuf/iomemory-docs/images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

  <h3 align="center"></h3>

  <p align="center">
    <a href="../../wiki">Wiki</a>
    .
    <a href="../../issues">Report Bug</a>
    ·
    <a href="../../issues">Request Feature</a>
  </p>
</p>

# IOMemory-VSL (Driver version 3.x)
This is an unsupported update of the original driver source for FusionIO cards. It comes with no warranty, it may cause DATA LOSS or CORRUPTION. It is designed for "VSL3" i.e. the first few generations of cards. Newer cards would require VSL4 (see below). You cannot use VSL4 on the older cards, and vice versa. 

# IOMemory-VSL4 (Driver version 4.x)
For the iomemory-vsl4 driver please go to the [iomemory-vsl4](https://github.com/RemixVSL/iomemory-vsl4) repo.

# How to Identify your Fusion-io Card (including VSL version)
 Please refer to the [Fusion-io and OEM Part Numbers Matrix](https://docs.google.com/spreadsheets/d/e/2PACX-1vQMd40liekOCeftUYQx6GeofHgjU5SSDT-jHWid03JCfswQxHAhVee3rW-04baqKg1qN2fp7wEzuFm6/pubhtml).

## Background
Driver support for FusionIO cards has been lagging behind kernel releases, effectively making these cards an expensive paperweight when running a distribution like Ubuntu / Arch / Fedora / ProxMox which all supply newer kernels than supported by the original drivers.

## Releases
We've abandoned the notion of releases, Generally `main` should be checked out. `main` is completely backwards compatible for all **5**, and **6** kernels. The latest working tested kernel is **6.9**.

Historically releases were tagged, and were be checked out by their tag. The release tags follow Linux Kernel versions. E.g. **v5.12.1 (A Fine Boi)** will work on all 5.x kernels that are 5.12 and lower, but is not guaranteed to work on 5.13. **v4.20.2 - Grey Dawn** supports most kernels that pre-date 5.0. Again this way of following kernel releases has been abandoned. Please follow **main**.

| Tag | Codename |
| --- | --- |
| v5.12.1 | [A Fine Boi](https://i.imgur.com/SrtYVIr.jpeg) |
| v5.10.0 | [Fatto Catto](https://www.youtube.com/watch?v=1S69FTdTS8g) |
| v5.6.0 | [MEGACHONKER](https://www.reddit.com/r/Chonkers/) |
| v4.20.3 | [Grey Dawn](https://southpark.cc.com/clips/154175/crazy-old-drivers) |

## Important Note for newer Linux Kernels
Starting with Linux kernel 5.4.0, significant changes to the kernel were made that require additional boot time kernel flags for this driver to work. These affect AMD CPUs starting with 5.4.0, and Intel CPUs after about kernel 5.8.0. 

### Grub
When using grub as your boot loader, add the following to your /etc/default/grub by looking for `GRUB_CMDLINE_LINUX_DEFAULT=""` and adding additional parameters inside the quotes.

For AMD systems:
```
amd_iommu=on iommu=pt
```
For Intel system:
```
iommu=pt
```

Example:
```GRUB_CMDLINE_LINUX_DEFAULT="quiet iommu=pt"```

### Systemd-boot
When using `systemd-boot` add the above options to the `options` section of the `.conf` file you're using as your loader. Typically the loader root will be `/EFI`, `/efi`, or `/boot`. Which will have a `loader/entries/` directory that contains your specific configuration files. Use your favourite editor. More information on `systemd-boot` can be found [here](https://wiki.archlinux.org/title/systemd-boot#Adding_loaders).

## Versions
The driver is derived from the original `iomemory-vsl-3.2.16.1732`, but has several fixes and gone through rigorous cleaning of redundant unused and old code. Active development is done on branches and merged back to `main` (formerly master) when stable.

The modified source fixes several known bugs in the driver:
- silent IO drops
- crashes during driver unload
- SCSI queue settings
- BIO status handling

## Important note!!!
PRs and commits to `main` have gone through testing with Flexible I/O Tester. Testing for page_cache errors, and generic FIO checksumming on read and write and different block sizes. Non `main` branches are often in process of verification and under active development, and guaranteed not stable.

## Building
Note! For many systems, **the best option is to use DKMS**, using the [instructions below](https://github.com/RemixVSL/iomemory-vsl/blob/main/README.md#dkms). If you prefer to build the module directly, or to create a `dpkg` or `rpm` package, you can proceed with these options below. 

Please make sure that the required dependencies are installed, as mentioned in this [README](https://github.com/RemixVSL/iomemory-vsl/blob/main/README)

## Installation
DKMS is recommended, but installation can also be done with packages or from source.

### DKMS
Dynamic Kernel Module Support automates away the requirement of having to repackage the kernel module with every kernel and headers update that takes place on the system. This mechanism also makes sure that the driver for the new kernel actually works, or if the source needs refreshing.

Try building from `main` first as it works with most modern kernels:
```
git clone https://github.com/RemixVSL/iomemory-vsl
cd iomemory-vsl/
sudo make dkms
```

If you know you need to build a specific branch based on a specific recommendation, use:
```
git clone https://github.com/RemixVSL/iomemory-vsl
cd iomemory-vsl/
# OPTIONAL: Checkout a specific release. Usually using main branch is correct.
# DO THIS ONLY IF YOU'VE BEEN TOLD IT'S REQUIRED!
git checkout <release-tag>
sudo make dkms
```

### Source
```
git clone https://github.com/RemixVSL/iomemory-vsl
cd iomemory-vsl/
# OPTIONAL: Checkout a specific release. Usually using main branch is correct.
# DO THIS ONLY IF YOU'VE BEEN TOLD IT'S REQUIRED!
# git checkout <release-tag>
make module
sudo insmod root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko
```
### .deb Ubuntu / Debian
```
git clone https://github.com/RemixVSL/iomemory-vsl
cd iomemory-vsl
# OPTIONAL: Checkout a specific release. Usually using main branch is correct.
# DO THIS ONLY IF YOU'VE BEEN TOLD IT'S REQUIRED!
# git checkout <release-tag>
make dpkg
```

### .rpm CentOS / RHEL
```
git clone https://github.com/RemixVSL/iomemory-vsl
cd iomemory-vsl/
# OPTIONAL: Checkout a specific release. Usually using main branch is correct.
# DO THIS ONLY IF YOU'VE BEEN TOLD IT'S REQUIRED!
# git checkout <release-tag>
make rpm
```

# Utils
The utilities to query and manage the drive(s) can be found in the [vsl_downloads](https://github.com/RemixVSL/vsl_downloads) repo.

After installing the utilities and using `fio-status -a` something as follows should be printed:
```
Found 1 ioMemory device in this system
Driver version: 3.2.16 build 1731

Adapter: Single Controller Adapter
	Fusion-io ioDrive2 1.205TB, Product Number:F00-001-1T20-CS-0001, SN:1241D0259, FIO SN:1241D0259
	ioDrive2 Adapter Controller, PN:PA004137009
	External Power: NOT connected
	PCIe Bus voltage: avg 12.02V
	PCIe Bus current: avg 0.77A
	PCIe Bus power: avg 9.21W
	PCIe Power limit threshold: 24.75W
	PCIe slot available power: unavailable
	Connected ioMemory modules:
	  fct0:	Product Number:F00-001-1T20-CS-0001, SN:1241D0259

fct0	Attached
	ioDrive2 Adapter Controller, Product Number:F00-001-1T20-CS-0001, SN:1241D0259
	ioDrive2 Adapter Controller, PN:PA004137009
	SMP(AVR) Versions: App Version: 1.0.29.0, Boot Version: 0.0.8.1
	Located in slot 0 Center of ioDrive2 Adapter Controller SN:1241D0259
	Powerloss protection: protected
	PCI:00:04.0, Slot Number:3
	Vendor:1aed, Device:2001, Sub vendor:1aed, Sub device:2001
	Firmware v7.1.15, rev 110356 Public
	UEFI Option ROM v3.2.6, rev 1212 Enabled
	1205.00 GBytes device size
	Format: v500, 2353515625 sectors of 512 bytes
	PCIe slot available power: unavailable
	PCIe negotiated link: 4 lanes at 5.0 Gt/sec each, 2000.00 MBytes/sec total
	Internal temperature: 64.47 degC, max 64.97 degC
	Internal voltage: avg 1.02V, max 1.02V
	Aux voltage: avg 2.49V, max 2.49V
	Reserve space status: Healthy; Reserves: 100.00%, warn at 10.00%
	Active media: 100.00%
	Rated PBW: 17.00 PB, 99.46% remaining
	Lifetime data volumes:
	   Physical bytes written: 92,553,058,889,120
	   Physical bytes read   : 82,741,942,333,288
	RAM usage:
	   Current: 389,636,800 bytes
	   Peak   : 389,636,800 bytes
	Contained VSUs:
	  fioa:	ID:0, UUID:0f402a7e-9948-7a4f-a2d0-7b0cb4d1ecda

fioa	State: Online, Type: block device
	ID:0, UUID:0f402a7e-9948-7a4f-a2d0-7b0cb4d1ecda
	1205.00 GBytes device size
	Format: 2353515625 sectors of 512 bytes
```
Where `dmesg` contains the actual driver version tag of the running driver
```
vagrant@fio:~/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16$ dmesg | grep "ioDrive driver"
[    3.122269] <6>fioinf ioDrive driver 5.11.10-b29020f-3.2.16.1731        loading...
```

# How to Get Help
- Open an issue in this Github repo
- Join our Discord server at https://discord.gg/VxDvVy3EdS

# Acknowledgements
The support and maintenance of this driver is made possible by the people that actively contribute or contributed to its code base or by supporting the project in other ways.

| Name | Discord |  |
|---|---|---|
| Demetrius Cassidy | @Tourman | Long nights of cleaning the codebase, setting up the Discord channel and guiding people through firmware upgrades |
| Vince Fleming |  | Donating a 1.2TB IoDrive2 for debugging problems |

Obviously all the regulars on the Discord channel, notably @bplein, @AcquaCow and @Tear.

Oh yes and the folks that were at <b>Fusion IO</b> for creating this product that was way ahead of its time and delivering the integration shim sources with the driver core.

## Resources
Tools and resources often used to figure out what changed, and why things are not working as they are supposed to.
| Source | |
| --- | --- |
| [Elixir](https://elixir.bootlin.com/linux/latest/source) | Making the Linux source code browsable and easy to compare changes over time | 
| [ZOL](https://zfsonlinux.org/) |  A source of inspiration for changes in the block layers of the linux kernel that we get to deal with | 
| [LKML](https://lkml.org/) | Sometimes the first or last resort to figure out why something changed in the kernel, or where |
| [The Nvidia Forum](https://forums.developer.nvidia.com/c/gpu-unix-graphics/linux/148) | Because they are hot to trot they encounter problems before we do, so we get to ride on their coat tails.... sometimes....though  often not | 
| [Ghidra](https://ghidra-sre.org/) | The Ghidra project from the NSA that allows a look under the covers to figure out things inside non-sourcy libs|


## Notes
Installing the fio-util, fio-common, fio-preinstall and fio-sysvinit are recommended.
When moving from source 2.3.11 to source 3.2.10 the firmware HAS TO BE UPDATED to 3.2.10.20150212. For the move to 3.2.15, and 3.2.16 from 3.2.10 we've not seen any issues.

## Firmware
Though the newer the firmware often the better. If your card is not supported with the latest firmware drop by the Discord channel and someone might be able to help you.
