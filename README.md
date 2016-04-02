# IOMemory-VSL

This is an unsoported update of the original driver source for FusionIO 
cards. It comes with no waranty, it may in cause DATA LOSS or CORRUPTION.
Therefore it is NOT meant for production use, just for testing purposes.

## Background
Driver support for FusionIO cards has been lagging behind kernel 
development, effectively making the cards an expensive paperweight when
running newer kernels.
Deemed a trivial task to update the drivers and acutally make them work
with newer kernels, and make the expensive papaerwight usable again, set
forking the code in motion.

## Important note!!!
Only the version branches are supported, nothing else. The driver may
in fact work with older or newer kernels but none have been tested. The
latest reliable tested versions are from 3.19 and upwards.

## Building
Module building can be done according to the original README.

## Installation
Installation can be done according to the orignal README.

## DKMS
sudo cp -r iomemory-vsl/root/usr/src/iomemory-vsl-3.2.10 /usr/src/
sudo mkdir -p /var/lib/dkms/iomemory-vsl/3.2.10/build
sudo ln -s /usr/src/iomemory-vsl-3.2.10 /var/lib/dkms/iomemory-vsl/3.2.10/source
sudo dkms build -m iomemory-vsl -v 3.2.10
sudo dkms install -m iomemory-vsl -v 3.2.10

