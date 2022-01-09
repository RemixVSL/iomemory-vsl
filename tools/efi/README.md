# EFI Boot
* NOT ALL VSL DRIVES SUPPORT UEFI, VSL4 DRIVES DO,
check your device documentation and firmware to make sure yours does.

> snippet from <b>lspci -nnnvvv</b>
>
> 04:00.0 Mass storage controller [0180]: SanDisk ioDrive2 [1aed:2001] (rev 04)<br>
>	Subsystem: SanDisk ioDrive2 [1aed:2001]

Booting from a Fusion-IO device comes with its own challenges. Firstly
making sure the device supports the EFI boot ROM, and secondly installing
the module, and in the case of Linux making the module available to initramfs
when installing the OS so it will actually boot.

The script in this directory is meant to help achieve and document the
EFI booting from a Fusion-IO device on a new install of Ubuntu.

## install-ubuntu.sh
Support for Ubuntu Desktop installation to a Fusion-IO drive as a boot drive.

1) Make sure BIOS is set to UEFI boot mode
2) Make sure FIO device has UEFI enabled
3) Boot your Ubuntu Desktop install medium
4) Choose "Try Ubuntu"
5) Open a terminal
6) `wget https://raw.githubusercontent.com/RemixVSL/iomemory-vsl/main/tools/efi/install-ubuntu.sh && bash install-ubuntu.sh`
7) Follow the instructions from the terminal
8) Proceed to install Ubuntu to the Fusion-IO drive
9) DON'T REBOOT BEFORE THE SCRIPT FINISHES!!!
