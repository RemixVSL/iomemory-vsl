#!/bin/bash
#
# 1) Make sure BIOS is set to UEFI boot mode
# 2) Make sure FIO device has UEFI enabled
# 3) Boot ubuntu install medium
# 4) Choose "Try Ubuntu"
# 5) Open a terminal
# 6) wget https://192.168.122.1:8080/install.sh && bash install.sh
# 7) Follow the instructions on screen.
# 8) DON'T REBOOT BEFORE THE SCRIPT FINISHES!!!

mod=vsl
sp="/-\|"
if [ ! -d "/sys/firmware/efi" ]; then
    echo "You have to be running EFI mode and not BIOS mode to do this."
    exit 1
fi

sudo apt-get update
sudo apt-get install -y git build-essential

git clone https://github.com/RemixVSL/iomemory-${mod}.git
cd iomemory-${mod}
make module
module=$(find . -type f -name iomemory-${mod}.ko)
echo "Inserting module, this may take some time."
sudo insmod $module

# should we add fio-tools for debugging?
echo "Module loaded!!"
echo "Install the OS to your fio device now. Don't reboot till we are Done."

waitforit=true
while $waitforit; do
    if [ -e "/target/etc/passwd" ]; then
        grep -q 1000 /target/etc/passwd && grep -q 1000 /target/etc/group
        if [ "$?" == "0" ]; then
            waitforit=false
        fi
    fi
    printf "\rwaiting for signs of life  \b${sp:i++%${#sp}:1}"
    sleep .2
done
echo
waitforit=true
while $waitforit; do
    grep -q "finish-install:" /var/log/syslog
    if [ "$?" == "0" ]; then
        waitforit=false
    else
        printf "\rwaiting for installer to finish  \b${sp:i++%${#sp}:1}"
    fi
    sleep .2
done
echo
waitforit=true
while $waitforit; do
    tail -100 /var/log/syslog | grep -q "Reverting lockdown of the desktop environment."
    if [ "$?" == "0" ]; then
        waitforit=false
    else
        printf "\rwaiting for lockdown to finish  \b${sp:i++%${#sp}:1}"
    fi
    sleep .2
done
echo

echo ""
echo "Copy module to target env root."
sudo cp -pr ~/iomemory-${mod} /target/root

sudo cp /etc/resolv.conf /target/etc/resolv.conf
for m in proc sys dev; do
    sudo mount -o bind /$m /target/$m
done
# use "EOT" to stomp interpration of stuff
echo "Going into chroot to move things around."
sudo chroot /target /bin/bash <<"EOT"
apt-get install -y git build-essential dkms grub-efi-amd64
echo iomemory_vsl >> /etc/initramfs-tools/modules
cd /root/iomemory-vsl
make dkms
release=$(uname -r)
nrelease=$(ls -1 /lib/modules | grep -v $release)
dkms_ver=$(dkms status | grep ^iomemory-vsl | awk -F', ' '{ print $2 }')
dkms build -m iomemory-vsl -v $dkms_ver -k $nrelease
dkms install -m iomemory-vsl -v $dkms_ver -k $nrelease
EOT

### NON LVM installs!!!
## until we fix udev do this...
sudo cp /target/etc/fstab /target/etc/fstab.org
UUIDs=$(cat /target/etc/fstab | perl -ne 'print $1."\n" if /UUID=([\w\d\-]+)/')
for UUID in $UUIDs; do
    blkdev=$(blkid | grep $UUID | awk -F: '{print $1}')
    line=$(grep $UUID /target/etc/fstab)
    sudo perl -pi -e "s#UUID=$UUID#$blkdev#" /target/etc/fstab
    echo "# $line" | sudo tee -a /target/etc/fstab
done
echo "DONE!!!, you can restart now."
echo exit
