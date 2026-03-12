Submitted by [@bekiroz](https://github.com/bekiroz) (please contact them with questions, or visit our [Discord](https://discord.gg/VxDvVy3EdS))

# Summary

Successfully built and loaded iomemory-vsl on TrueNAS SCALE 25.04.2.6 with kernel 6.12.15-production+truenas. Sharing the steps for other TrueNAS users.

# System Info

## Component	Details
|Item|Description|
|---|---|
|OS	|TrueNAS SCALE 25.04.2.6 (Fangtooth)|
|Kernel	|6.12.15-production+truenas|
|Hardware	|Dell PowerEdge R530|
|Card	|SanDisk ioDrive2 (1.2TB)|
|Driver	|iomemory-vsl 3.2.16.1732|
|Use case	|ZFS SLOG (64GB partition) + fast pool (1TB partition)|

# Build Steps on TrueNAS SCALE

TrueNAS SCALE locks apt by default and uses a read-only squashfs root. The key is using install-dev-tools to unlock the build environment:

```bash
# 1. Enable development tools (unlocks apt and makes rootfs writable)

install-dev-tools

# 2. Install build dependencies

apt-get install -y build-essential git dkms dpkg-dev fakeroot debhelper

# 3. Clone and build

cd /mnt/<your-pool>/
git clone https://github.com/RemixVSL/iomemory-vsl.git
cd iomemory-vsl
make dpkg

# 4. Install the generated .deb

dpkg -i ../iomemory-vsl-$(uname -r)_*_amd64.deb

# 5. Load the module

modprobe iomemory_vsl

# 6. Verify

lsmod | grep iomemory
ls -la /dev/fio*
```

## Result
```
$ lsmod | grep iomemory
iomemory_vsl         1302528  1

$ lsblk /dev/fioa
NAME    MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
fioa    254:0    0  1.1T  0 disk
├─fioa1 254:1    0   64G  0 part
└─fioa2 254:2    0    1T  0 part

$ zpool status pool_main | grep -E 'state:|fioa|nvme'
 state: ONLINE
    fioa1                                   ONLINE       0     0     0
    nvme0n1                                 ONLINE       0     0     0
```
The ioDrive2 is used as ZFS SLOG (fioa1, 64GB) and the pool is fully ONLINE.

## Important Notes for TrueNAS Users

install-dev-tools is the key command - it disables rootfs protection and enables apt
make dpkg is the correct build method (not make module - that bypasses config detection)
Store the .deb on your ZFS pool (e.g., /mnt/pool_main/) - it survives TrueNAS upgrades
After each TrueNAS upgrade, you need to re-run install-dev-tools, rebuild, and reinstall (or just dpkg -i + modprobe if the kernel version hasn't changed)
Docker container builds do NOT work because TrueNAS kernel headers are not available via standard apt repos
Suggestion
