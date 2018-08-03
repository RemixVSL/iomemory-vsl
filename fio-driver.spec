%define fio_version        3.2.15.1700
%define fio_sysconfdir     /etc
%define fio_oem_name       fusionio
%define fio_oem_name_short fio
%define fio_sourcedir      /usr/src
%define fio_driver_name    iomemory-vsl
%define kfio_scsi_device   False


%define has_kver %{?rpm_kernel_version: 1} %{?!rpm_kernel_version: 0}
%if !%{has_kver}
%define rpm_kernel_version %(uname -r)
%endif

%define has_nice_kver %{?rpm_nice_kernel_version: 1} %{?!rpm_nice_kernel_version: 0}
%if !%{has_nice_kver}
%define rpm_nice_kernel_version %(echo %{rpm_kernel_version} | sed -e 's/-/_/g')
%endif

%define fio_tar_version %{fio_version}
%{!?dist:%define dist %nil}
%{!?kernel_module_package:%define kernel_module_package %nil}
%{!?kernel_module_package_buildreqs:%define kernel_module_package_buildreqs %nil}

%define firehose_shipped_object %{?fio_shipped_object: FIREHOSE_SHIPPED_OBJECT=%{fio_shipped_object}}
%define fio_release 1.0%{dist}


# Turn off silly debug packages
%define debug_package %{nil}


Summary: Driver for SanDisk Fusion ioMemory devices
Name: iomemory-vsl
Vendor: SanDisk
Version: %{fio_version}
Release: %{fio_release}
Obsoletes: iodrive-driver-kmod, iodrive-driver, fio-driver
License: Proprietary
Group: System Environment/Kernel
URL: https://link.sandisk.com/commercialsupport.html
Source0: %{name}-%{fio_tar_version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildArch: x86_64
%if "%{_vendor}" == "suse"
BuildRequires: %kernel_module_package_buildreqs rsync tar gcc make kernel-source
%else
%if "%{_vendor}" == "redhat"
BuildRequires: %kernel_module_package_buildreqs rsync tar gcc make kernel-devel rpm-build
# Add the below macro later (causes build failures on too many systems)
#kernel_module_package
%endif
%endif


%description
Driver for SanDisk Fusion ioMemory devices


%prep
%setup -q -n %{name}-%{fio_tar_version}


%build
rsync -rv root/usr/src/iomemory-vsl-3.2.15/ driver_source/
if ! %{__make} \
        -C driver_source \
        KERNELVER=%{rpm_kernel_version} \
        FIO_DRIVER_NAME=%{fio_driver_name} \
        FIO_SCSI_DEVICE=%{kfio_scsi_device} \
        %{firehose_shipped_object} \
        modules
        then
        set +x
        cat <<EOF >&2
ERROR:

EOF
                exit 1
fi


%install
[ "$(cd "${RPM_BUILD_ROOT}" && pwd)" != "/" ] && rm -rf ${RPM_BUILD_ROOT}
rsync -a root/ "${RPM_BUILD_ROOT}/"
mkdir -p "${RPM_BUILD_ROOT}/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}"
cp driver_source/iomemory-vsl.ko \
    "${RPM_BUILD_ROOT}/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}"

# Ensure the docdir has the driver version (prevents collisions when multiple drivers are installed)
mv "${RPM_BUILD_ROOT}/usr/share/doc/iomemory-vsl" \
    "${RPM_BUILD_ROOT}/usr/share/doc/iomemory-vsl-%{rpm_kernel_version}"

mkdir -p "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.15/include/fio/port/linux"
touch -a "driver_source/Module.symvers"
cp "driver_source/Module.symvers" "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.15/"
cp "driver_source/include/fio/port/linux/kfio_config.h" "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.15/include/fio/port/linux/"


%pre
%ifarch i386
wrong_version() {
	echo "iomemory-vsl requires the i686 kernel to be installed.  Due to problems with architecture detection in anaconda, "
	echo "it appears that the wrong kernel is installed.  Please see"
	echo
	echo "http://fedoraproject.org/wiki/Bugs/FC6Common#head-e0676100ebd965b92fbaa7111097983a3822f143"
	echo
	echo "for more information."
	echo
	exit -1
}

echo "Checking kernel version..."

if [ `uname -m` != 'i686' ] ; then
	wrong_version
fi

if [ `rpm -q --queryformat="%{ARCH}\\n" kernel | sort -u` == 'i586' ] ; then
	echo "Found via rpm (uname reports i686):"
	echo
	wrong_version
fi

%endif


%package -n %{name}-%{rpm_kernel_version}
Summary: Driver for SanDisk Fusion ioMemory devices
Group: System Environment/Kernel
Provides: iomemory-vsl, libvsl
Provides: iomemory-vsl-%{fio_version}
Obsoletes: iodrive-driver, fio-driver

%description -n %{name}-%{rpm_kernel_version}
Driver for SanDisk Fusion ioMemory devices


%post -n %{name}-%{rpm_kernel_version}
if [ -x "/sbin/weak-modules" ]; then
    echo "/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko" \
        | /sbin/weak-modules --add-modules
fi
/sbin/depmod -a %{rpm_kernel_version}
if [ -a /etc/init.d/fio-agent ]; then #does fio-agent start script exist
	if [ "$(pidof fio-agent)" ] # fio-agent running
	then
		/etc/init.d/fio-agent restart
	fi
fi
if hash dracut &> /dev/null
then
    dracut -f
fi
ldconfig

%preun -n %{name}-%{rpm_kernel_version}
cp /usr/lib/fio/libvsl.so /usr/lib/fio/libvsl-prev.so

%postun -n %{name}-%{rpm_kernel_version}
if [ -x "/sbin/weak-modules" ]; then
    echo "/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko" \
        | /sbin/weak-modules --remove-modules
fi
/sbin/depmod -a %{rpm_kernel_version}
if hash dracut &> /dev/null
then
    dracut -f
fi
if [ "$1" -eq 0 ]; then
    /sbin/ldconfig
fi

%files -n %{name}-%{rpm_kernel_version}
%defattr(-, root, root)
/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko
/usr/lib/fio/libvsl.so
/usr/share/doc/fio/NOTICE.libvsl
/etc/ld.so.conf.d/fio.conf
/usr/share/doc/iomemory-vsl-%{rpm_kernel_version}/License
/usr/share/doc/iomemory-vsl-%{rpm_kernel_version}/NOTICE.iomemory-vsl


%package -n %{name}-config-%{rpm_kernel_version}
Summary: Configuration of %{name} for FIO drivers %{rpm_kernel_version}
Group: System Environment/Kernel
Provides: iomemory-vsl-config
Provides: iomemory-vsl-config-%{fio_version}
Requires: %{name}-source

%description -n %{name}-config-%{rpm_kernel_version}
Configuration of %{name} for FIO drivers %{rpm_kernel_version}


%files -n %{name}-config-%{rpm_kernel_version}
%defattr(-, root, root)
/usr/src/iomemory-vsl-3.2.15/Module.symvers
/usr/src/iomemory-vsl-3.2.15/include/fio/port/linux/kfio_config.h


%package source
Summary: Source to build driver for SanDisk Fusion ioMemory devices
Group: System Environment/Kernel
Release: %{fio_release}
Obsoletes: iodrive-driver-source, fio-driver-source
Provides: iomemory-vsl, iomemory-vsl-%{fio_version}, libvsl
Obsoletes: iodrive-driver, fio-driver

%description source
Source to build driver for SanDisk Fusion ioMemory devices


%files source
%defattr(-, root, root)
/usr/lib/fio/libvsl.so
/usr/share/doc/fio/NOTICE.libvsl
%config /etc/ld.so.conf.d/fio.conf
/usr/src/iomemory-vsl-3.2.15/Kbuild
/usr/src/iomemory-vsl-3.2.15/Makefile
/usr/src/iomemory-vsl-3.2.15/dkms.conf.example
/usr/src/iomemory-vsl-3.2.15/cdev.c
/usr/src/iomemory-vsl-3.2.15/driver_init.c
/usr/src/iomemory-vsl-3.2.15/check_target_kernel.sh
/usr/src/iomemory-vsl-3.2.15/errno.c
/usr/src/iomemory-vsl-3.2.15/iomemory-vsl.mod.c
/usr/src/iomemory-vsl-3.2.15/katomic.c
/usr/src/iomemory-vsl-3.2.15/kbitops.c
/usr/src/iomemory-vsl-3.2.15/kblock.c
/usr/src/iomemory-vsl-3.2.15/kscsi.c
/usr/src/iomemory-vsl-3.2.15/kscsi_host.c
/usr/src/iomemory-vsl-3.2.15/kcache.c
/usr/src/iomemory-vsl-3.2.15/kcondvar.c
/usr/src/iomemory-vsl-3.2.15/kcsr.c
/usr/src/iomemory-vsl-3.2.15/kexports.c
/usr/src/iomemory-vsl-3.2.15/kfile.c
/usr/src/iomemory-vsl-3.2.15/kfio.c
/usr/src/iomemory-vsl-3.2.15/kfio_config.sh
/usr/src/iomemory-vsl-3.2.15/khotplug.c
/usr/src/iomemory-vsl-3.2.15/kinfo.c
/usr/src/iomemory-vsl-3.2.15/kmem.c
/usr/src/iomemory-vsl-3.2.15/kscatter.c
/usr/src/iomemory-vsl-3.2.15/ktime.c
/usr/src/iomemory-vsl-3.2.15/main.c
/usr/src/iomemory-vsl-3.2.15/module_param.c
/usr/src/iomemory-vsl-3.2.15/pci.c
/usr/src/iomemory-vsl-3.2.15/port-internal.h
/usr/src/iomemory-vsl-3.2.15/sched.c
/usr/src/iomemory-vsl-3.2.15/state.c
/usr/src/iomemory-vsl-3.2.15/sysrq.c
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/bits.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/mips_atomic.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/mips_cache.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/ppc_atomic.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/ppc_cache.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/x86_atomic.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/x86_cache.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/arch/x86_64/bits.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/align.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/atomic_list.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/bitops.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/byteswap.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/cdev.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/commontypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/compiler.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/port_config.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/port_config_macros.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/port_config_macros_clear.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/port_config_vars_externs.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/csr_simulator.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/dbgset.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/errno.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/fio-poppack.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/fio-port.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/fio-pshpack1.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/fio-stat.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/fiostring.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ifio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ioctl.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kbio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kbitops.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kblock.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kbmp.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kcache.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kchunk.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kcondvar.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kcpu.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kcsr.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kdebug.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kfio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kfio_config.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kglobal.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kinfo.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kmem.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kscatter.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/kscsi.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ktime.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ktypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ktypes_32.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ktypes_64.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/libgen.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/list.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/pci.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/sched.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/state.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/stdint.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/trim_completion.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/trimmon_attach.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/unaligned.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/ufio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/utypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/vararg.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/vectored.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/commontypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/compiler.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/div64.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/errno.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kassert.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kblock.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kcondvar.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kfile.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kfio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kscsi_config.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/kscsi_host.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/stdint.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/common-linux/ufio.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/linux/ktypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/linux/utypes.h
/usr/src/iomemory-vsl-3.2.15/include/fio/port/gcc/align.h
/usr/src/iomemory-vsl-3.2.15/kfio/.x86_64_cc*
/usr/src/iomemory-vsl-3.2.15/kfio/x86_64_cc*


%changelog


* Mon Apr 10 2017 18:04:35 +0000  Support <support@fusionio.com> 

SanDisk VSL Pinnacles 3.2.4 to 3.2.14 Change Log

3.2.4 CHANGE LOG
3.2.4.1089 CHANGE LOG (Windows Only)
The Windows installer is now updated to version 3.2.4.1089. This latest version of the ioMemory VSL Windows installer no longer installs the Fusion-io TRIM service as a Windows service. For more information see KB950 in the http://support.fusionio.com Knowledge Base.
The installer does copy over the Fusion-io TRIM service file, but it does not install it as a Windows service.

3.2.4.1086 CHANGE LOG
In addition to various improvements, the following are changes made to the ioMemory VSL software since the last release, including:

General Changes
General Improvements and Features
-- General performance improvements.
-- New supported devices: 1.6TB MLC ioFX device and 825GB MLC ioCache device.
-- Updated list of supported operating systems.
-- New fio-pci-check -n option allows you to return just a status report without clearing any errors or making other changes.
-- New fio-pci-check -o option allows you to optimize the ioMemory device PCIe link settings by increasing the maximum read request size if it is too low.
-- Increased the number of PCI devices that the fio-pci-check utility can detect.
-- Improved PCIe link reporting and error handling.
-- The fio-status utility now reports the NAND flash board temperature and related thermal thresholds. See the ioMemory Hardware Installation Guide for more information.
-- Improved mixed traffic I/O scheduling.
-- Improved handling of errors caused by poorly seated NAND modules.
-- Improved failure handling for some  ioDrive devices.

Fixed General Issues
-- Problems attaching Virtual Controller devices using the fio-attach utility
Issue:
When an ioMemory device is configured with two Virtual Controller devices, one or more of the Virtual Controller devices may fail to attach.
Resolution:
The fio-attach utility now attaches these devices.

-- Format statistics not showing new formatting results
Issue:
After completing a new format of the device using the fio-format utility, the statistics printed in the system logs reflected the previous formatting conditions.

Resolution:
The system logs now show the statistics for the current formatting conditions.

-- Invalid fio-update-iodrive error message
Issue:
The fio-update-iodrive utility returned the following error (which was no longer a valid error):
    Error: unlock failed addr <address> status 0xff
Resolution:
The utility no longer returns the invalid message

-- The fio-bugreport utility fails to complete
Issue:
The fio-bugreport utility fails to complete when one or more ioMemory devices are in minimal mode.
Resolution:
The utility now completes while in minimal mode.

-- Issues with larger capacity devices and 512B sectors
Issue:
Devices with capacities greater than 2TB had an issue with 512B sector sizes.

Resolution:
These devices no longer have an issue with 512B sector sizes.

-- Device signal and noise values incorrect
Issue:
The signal and noise values for ioMemory devices returned by lspci were incorrect.

Resolution:
The devices now show accurate signal and noise values in the PCIe capabilities listed by lspci.

Windows Changes
Windows Improvements and Features

-- Changed the default value for the WIN_LOG_VERBOSE module parameter to 1 (enabled). This does increase the size of the system log, and the parameter can be disabled using the fio-config utility, see the ioMemory VSL User Guide  for Windows for more information.

Fixed Windows Issues

-- fio-config not working when the ioMemory VSL software is not loaded
Issue:
The fio-config utility would not work without the ioMemory VSL driver loaded.Resolution:
The utility now works whether the software is loaded or not.

-- The fio-trim-config utility returned an error when run on operating systems with native TRIM
Issue:
When the fio-trim-config parameter was run on operating systems that had native TRIM support (such as Windows Server 2012), the utility simply printed out an Unknown condition.

Resolution:
The utility now reports that it is Stopped on operating systems that support native TRIM.

Linux Changes
Fixed Linux Issues

-- Mount points with similar names do not mount using the init script
Issue:
When using the init script to load the VSL driver, some mount points with very similar names did not mount.
Resolution:
All mount points mount as expected.

-- Change to default setting of the use_workqueue parameter
Issue:
When the use_workqueue parameter was changed to a default value of 0 in VSL version 3.2.2, it exposed an issue with certain Linux kernels that resulted in decreased write performance with file systems and other operations that use asynchronous IO or libaio and performed flush functions. Resolution:
The issue in these kernels no longer impacts write performance.

VMware Changes
VMware Improvements and Features

-- New SCSI block device version of the ioMemory VSL software is available for ESXi 5.x hypervisors.
-- Improvements in the amount of information collected by the fio-bugreport utility in ESXi.

Fixed VMware Issues

-- fio-status message is not informative
Issue:
When the ioMemory VSL software was not loaded, the fio-status utility returned the following message:
    No Fusion-io drives were found in this system.

Resolution:
The utility now returns the following more informative message:
    No devices were found in this system. Please check if the iomemory-vsl driver is loaded.

-- VMware hypervisors don't support devices greater than 2TB
Issue:
Because the VMFS in VMware hypervisors does not directly support devices with capacities greater than 2TB, you cannot use all of the capacity of a 3.0TB MLC ioDrive2 or a 3.2TB MLC ioScale device when using the device as a LUN.

Resolution:
The issue is resolved with the SCSI device version of the ioMemory VSL software. This issue is still seen in the raw block device version of the software. For more information on this issue, see knowledge base article KB926 on http://support.fusionio.com.

-- Unaligned DMA Failure on VMware with VSL 3.2.3
Issue:
When running ioMemory VSL 3.2.3.950 software on VMware hypervisors an unaligned IO request may cause an error in the VSL software. This could cause an ioMemory device to go offline and may also result in data loss.Resolution:
This issue is now fixed. Upgrade to this version or later if you are running the VSL software on a VMware hypervisor. For more information, see knowledge base article KB829 on http://support.fusionio.com.

Solaris Changes
Solaris Improvements
-- Improvements in the amount of information collected by the fio-bugreport utility in Solaris.

Fixed Solaris Issue
-- Newer ioMemory devices unusable with the ioMemory VSL software in Solaris


Issue:
The ioMemory VSL software  ioMemory device could not attach and use newer ioMemory devices in Solaris.Resolution:
The ioMemory VSL software now attaches and uses all supported devices.

OS X Changes
OS X Improvements and Features
-- Man pages are now included with the ioMemory VSL software for OS X.
-- The ioMemory VSL software now allows 1MB IOs in OS X.

FreeBSD Changes
FreeBSD Improvements and Features
-- Man pages are now included with the ioMemory VSL software for FreeBSD.
-- The install script now includes the option to create an installation package for later use.

Fixed FreeBSD Issues
-- The ioMemory VSL software installation file referenced a missing file

Issue
: When the ioMemory VSL software was installed, the installation file referenced a README file that did not exist in the software package.Resolution:
The README file is now included in the software package.

3.2.6 CHANGE LOG

In addition to various improvements, the following are changes made to the ioMemory VSL software since version 3.2.4, including:
General Changes
General Improvements and Features
-- General performance improvements.
-- Updated supported operating systems.
-- Improved performance on systems with the Intel IOMMU (Input/Output Memory Management Unit) enabled. This enhancement improves how efficiently the VSL software handles DMA mappings, and it therefore helps minimize soft lockup warnings on systems with IOMMU enabled.
-- The fio-status utility now reports the alert values that triggered an error. For example, if voltage spikes out of range, then the utility will report the peak voltage that caused the error.
-- The fio-sure-erase utility will now stop and report an error if the device is attached during the erase process.
-- Improved fio-status processing times.

Fixed General Issues
-- Incorrect time value in log message
Issue:
In a system log error message indicating a watchdog was fired, the time was erroneously given in ms.
Resolution:
The time is now given in us (microseconds).

-- Invalid device information in system logs
Issue:
When an ioMemory device was no longer enumerated on the PCIe bus, the ioMemory VSL software would log device information that was no longer valid.
Resolution:
The VSL software no longer attempts to add invalid information to the system logs.

-- Large misaligned IOs
Issue:
Submitting maximum-sized IOs (1MB) on a non-page aligned boundary could cause IO failure and immediate channel failure.

Resolution:
The ioMemory VSL now adequately handles large misaligned IOs.

Windows Changes
-- Due to a known issue with the Fusion-io TRIM service on Windows Server 2003 and Windows Server 2008 R1, this release of the ioMemory VSL installer does not install the Fusion-io TRIM service. For more information see KB950 in the http://support.fusionio.com Knowledge Base.

Fixed Windows Issues
-- Potential Windows installation issue
Issue:
The ioMemory VSL software installer may fail to install with this error message:
    There is a problem with this Windows Installer package. A DLL required for this install to complete could not be run. Contact your support personnel or package vendor.

Resolution:
The installer package no longer fails with this error.

-- System crashes during format
Issue:
Under certain circumstances formatting a device using the Disk Manager utility may crash the Windows system.

Resolution:
The system will no longer crash while formatting ioMemory devices.

Linux Changes

Fixed Linux Issues
-- Issue with Veritas Cluster Service and fio-status
Issue:
The fio-status utility could hang when the Veritas Cluster Server configuration performed a check on the clusters.
Resolution:
The utility will no longer hang when this check is performed.

-- Continued issues with write performance that involve FLUSH functions.
Issue:
Decreased write performance with file systems and other operations that use asynchronous IO or libaio and performed FLUSH functions on certain Linux kernels (for example, 2.6.32 and 2.6.38).
Resolution:
The issue in these kernels no longer impacts write performance.

-- FLUSH watchdog error
Issue
: A watchdog error was erroneously reporting a stuck flush while the flush was still completing. Resolution:
The ioMemory VSL software now makes sure all current processes are completed before declaring a watchdog error.

-- Incompatible kernel
Issue:
The 3.10 Linux kernel is incompatible with previous versions of the ioMemory VSL software. This is a known issue with some newer kernel updates for Fedora 18. Resolution:
This release of the ioMemory VSL software is compatible with the 3.10 kernel.

3.2.7 CHANGE LOG
The following are changes made to the ioMemory VSL software since version 3.2.6, including:

-- The ioMemory VSL software now prints in the kernel logs the formatted size of each device
as it attaches.
-- The fio-status utility now prints out a warning if the ioMemory VSL driver is not loaded.

Fixed Issues
-- Upgrade Issue
Issue:
After certain ioMemory devices were upgraded to ioMemory VSL software version 3.2.x the devices were no longer recognized by management tools (such as SNMP).

Resolution:
The ioMemory devices now work with the management tools.

-- Invalid device information in fio-status output
Issue:
When an ioMemory device was no longer enumerated on the PCIe bus, the fio-status utility would return device information that was no longer valid.
Resolution
: The utility now returns that the information is unavailable.

-- Unmounting with the init script
Issue:
The init script for unmount would unmount all volumes with similar names.

Resolution:
The script now only unmounts the specified volume.

-- Filesystem errors
Issue:
Under certain kernel conditions the ioMemory VSL software would improperly reorder requests from the kernel, potentially resulting in filesystem errors.
Resolution:
The ioMemory VSL software no longer reorders these requests.

3.2.8 CHANGE LOG
In addition to various improvements, the following are changes made to the ioMemory VSL software since version 3.2.6, including:

General Changes
General Improvements and Features
o Updated supported operating systems

o Newly Supported Operating Systems:
Linux

-- RHEL 5.10
-- RHEL 6.5
-- Oracle Linux 5.10
-- Oracle Linux 6.5
-- CentOS 5.10
-- CentOS 6.5
-- Fedora 20
-- Ubuntu 14.04


OS X
-- OS X 10.9

o Discontinued Operating System Support:
Microsoft Windows
-- Microsoft Windows Server 2003
-- Microsoft Windows Server 2008 (R1)


Linux
-- Debian Squeeze
-- OpenSUSE 12
-- Fedora 18
-- Ubuntu 10.04
-- Ubuntu 12.10
-- Ubuntu 13.04

VMware

-- ESX 4.x
-- ESXi 4.x

Solaris
-- Solaris 10 U9
-- Solaris 10 U10


OS X

-- OS X 10.6
-- OS X 10.7

FreeBSD

-- FreeBSD 8

-- The ioMemory VSL software now prints in the kernel logs the formatted size of each device as it attaches.

-- Documentation for the  preallocate_mb parameter for controlling the amount of memory that the ioMemory VSL software preallocates for using devices as SWAP (also known as paging files in Windows). See the ioMemory VSL User Guide for Windows or Linux for more information.

Fixed General Issues

-- Issue with the fio-status utility
Issue:
When an ioMemory device was no longer enumerated on the PCIe bus, the fio-status utility would return device information that was no longer valid. For example, the firmware version was reported as v1023.1023.1023
Resolution:
The utility now returns valid information.

-- Issues when interrupting the fio-format utility
Issue:
If you interrupted the fio-format utility while it was formatting an ioMemory device, the device would become unusable, and you would need to use the fio-sure-erase utility to recover use of the device.
Resolution:
 You can now interrupt the fio-format utility and then recover the device by formatting it again using the fio-format utility.

-- Delayed startup with preallocated memory
Issue:
Preallocating memory to use ioMemory devices for swap (page files) could result in startup times that were delayed by approximately 20 - 25 minutes.

Resolution
: Preallocating memory no longer results in delayed startup times.

-- Running fio-status with the ioMemory VSL software unloaded causes NMIs
Issue:
Running fio-status without the ioMemory VSL driver loaded would cause an NMI on an HP DL580Gen8, resulting in a system crash.
Resolution:
Running fio-status on that machine without the ioMemory VSL driver loaded no longer crashes the system, and the fio-status utility now prints out a warning if the ioMemory VSL driver is not loaded.

-- Certain devices not recognized by management software
Issue
: After certain first generation devices (such as the ioDrive Duo 1280) were upgraded to ioMemory VSL software 3.2.x the devices were no longer recognized by management tools (such as SNMP).

Resolution:
The ioMemory devices now work with the management tools.

Windows Changes
Windows Improvements and Features
-- The ioMemory VSL software installer for Windows will cease installation if another version is previously installed. You must remove other versions before you install this version.

-- The ioMemory VSL installer no longer presents tree-view options when installing the software. The ioMemory VSL utilities are now installed by default.

Fixed Windows Issues

-- Issues with multiple PCI domains
Issue:
Not all ioMemory devices would attach in a system configured with multiple PCI domains and many ioMemory devices installed (for example, more than 32 devices). This was due to PCI address collisions.
Resolution:
The ioMemory VSL software now detects the domain number to avoid address collisions.

-- Utilities still installed
Issue:
When installing the VSL software the installer may not uninstall the utilities.

Resolution:
When you uninstall the ioMemory VSL software, the installer will now also uninstall the ioMemory VSL utilities.

-- fio-config incorrectly reports values
Issue:
The fio-config utility would incorrectly show values for parameters that were not set.
Resolution:
The utility now displays the correct values for all parameters.

Linux Changes
Linux Improvements and Features
-- ioMemory devices are no longer limited to 15 partitions per device in Linux operating systems using kernel 2.6.28 or newer. Any limitations on the number of partitions are now due to system constraints.

Fixed Linux Issues
-- Issue with init scripts on SLES 10
Issue:
init scripts failed to load the ioMemory VSL software on SLES 10 with the following warning:
    Service udev has to be enabled to start service iomemory-vsl

Resolution:
 The init script now loads the ioMemory VSL software on all supported Linux distributions.

-- Issue with unmounting similarly named volumes
Issue
: The init script for unmount would unmount all volumes with similar names. For example, it would unmount both /volume1 and /volume1-new.
Resolution:
The script now only unmounts the specified volume.

-- Flushed system signals
Issue:
The ioMemory VSL software would issue a system call that flushed pending signals. This prevented some applications from receiving the signals and the applications would hang.

Resolution:
The ioMemory VSL software no longer issues this system call.

-- Filesystem barriers
Issue:
Previous versions of the 3.x ioMemory VSL software do not implement barriers on Linux. Because of this, requests can be reordered in a way that file systems requiring barriers do not expect. File systems that rely on barriers for correct operation may, in rare instances, report inconsistency.
Resolution:
The ioMemory VSL software has been changed to implement barriers within Linux filesystems. If you are using a previous 3.x version of the ioMemory VSL software on Linux, we recommend upgrading to this version (3.2.8) of the ioMemory VSL software. For more information, see KB1310 on our Support site at http://support.fusionio.com.

VMware Changes
VMware Improvements and Features

-- The SCSI unmap command is now supported with ESXi 5.5.

-- vSAN is now supported with the ESXi 5.5 SCSI version of the ioMemory VSL software.

-- In order to minimize issues while detaching, the fio-detach utility now issues the following warning:
WARNING: Please verify that the device is no longer in use, and unmounted before continuing. A device in use, on forced detach, can cause system instability and kernel crashes. Do you wish to continue [y/n]?

-- In order to minimize issues while detaching, the fio-detach utility now makes additional attempts to unclaim devices while detaching.

-- ioMemory VSL software utilities now support remote ESXCLI commands for remote management. See the ioMemory VSL User Guide  for Vmware for more info.

Fixed VMware Issues
-- System crash while attaching
Issue:
A PSOD (VMware system crash) would result when an ioMemory device failed to attach and the ioMemory VSL software would immediately attempt another attach (before verifying the failed attach). The software would go into an invalid state and cause the PSOD.

Resolution
: The ioMemory VSL software now verifies the failed attach to prevent the invalid state and the PSOD.

-- Dead paths reported
Issue:
VMware kernel logs reported dead paths because the ioMemory VSL software recognized some invalid multipaths to ioMemory devices as valid. This issue did not affect driver functionality. Resolution:
The ioMemory VSL software no longer recognizes these invalid paths nor does it print the errors in the kernel logs.

-- Read/write errors
Issue:
Infrequent read/writer errors would result from the ioMemory VSL software not responding quickly enough to read/write requests. The following is an example error in the vmkernel.log file:

 2014Y03Y03T20:10:39.353Z$cpu2:2050)NMP:$nmp_ThrottleLogForDevice:2318:$Cmd$0x2a$ (0x412400754e80)$to$dev$"eui.a6e07c87573e4e8a002471ccd0b2f712"$on$path$"fioiom0:C0:T0:L0"$ Failed:$H:0x7$D:0x0$P:0x0$Possible$sense$data:$0x0$0x0$0x0.Act:EVAL
Resolution:
The ioMemory VSL software no longer has these errors. The software now has improved response to abort handling due to read/write timeouts.

3.2.9 CHANGE LOG
In addition to various improvements, the following are changes made to the Fusion ioMemory VSL software since version 3.2.8, including:
General Changes
General Improvements and Features
o Updated supported operating systems.

o Newly Supported Operating Systems:
Linux
-- RHEL 7.0
-- RHEL 5.11
-- Oracle Linux 7.0
-- CentOS 7.0
-- Oracle VM 3.2.7, 3.3.1

-- The fio-sure-erase utility now reports the device serial number upon completion.

-- The fio-status utility now provides the thresholds for when performance throttling engages due to high temperature conditions at the controller or NAND memory devices. These values can be used by customer management processes to predict when high temperature conditions will impact device performance or when additional cooling measures need to be taken. Example output:

fio-status -fj
...
  "controller_throttle_start_deg_c" : "78",
  "nand_throttle_start_deg_c" : "78",

Fixed General Issues

-- Latency issues
Issue:
Under specific workloads and in specific system environments a customer may experience longer than expected latencies on Fusion ioMemory devices. The behavior has been reported with Fusion ioMemory VSL software versions 3.2.x. The frequency of occurrence depends on the workload and system environment.
Resolution:
This release includes fixes that reduce the frequency of latency spikes in many scenarios.

Windows Changes
Fixed Windows Issues

-- Conflicting SCSI IDs
Issue:
If your system is using a storage device in addition to an Fusion ioMemory device, the system may show duplicate SCSI IDs causing conflicts.
Resolution
: Use the fio-config utility and the WIN_SCSI_BUS_ID module parameter to set the device ID for all ioMemory devices to a SCSI ID number that doesn't conflict with other SCSI devices. See the fio-config section in the Command Utilities section of the ioMemory VSL User Guide  for Windows for more information.

-- Multiple processes accessing PCI device information
Issue:
When more than one process tries to access PCI device information (for example running multiple instances of fio-pci-check in parallel), the first process will erroneously return information for a later process.

Resolution:
The configuration information is locked so only one process can access it at a time.

Linux Changes
Linux Improvements and Features

-- Added DKMS support for Linux source packages. When you update the system kernel and DKMS is installed and configured correctly, the Fusion ioMemory VSL software will be configured for the new kernel. This includes updates to Fusion ioMemory ioMemory VSL driver loading configuration.

-- Improved kernel configuration code for building from source.

Fixed Linux Issues
-- Invalid OS requests
Issue:
When the Linux operating system issued an incorrectly formed request the Fusion ioMemory VSL software would crash.

Resolution:
The Fusion ioMemory VSL software now rejects the invalid request.

-- make clean failed
Issue:
When building the software package from source, the 'make clean' option would fail.

Resolution:
This option now works as expected.

-- Driver load issues after updates
Issue:
Updating the system kernel or Fusion ioMemory VSL software would insert the Fusion ioMemory VSL software into the initrd and ignore the software configuration file (/etc/modprobe.d/iomemory-vsl.conf). This issue created software loading issues that required dracut to fix. Resolution:
Multiple solutions to this configuration issue:
    1. If DKMS is installed and configured the Fusion ioMemory VSL software is configured for the new kernel.
    2. If dracut is installed, the Fusion ioMemory VSL software uses that program to fix the configuration issue.
    3. If both the fio-sysvinit and fio-common packages are installed, the Fusion ioMemory VSL software is excluded from the initrd.

VMware Changes

Fixed VMware Issues

-- Caching stopped due to internal errors
Issue:
An imbalance in process queue sizes would cause the Fusion ioMemory VSL software for ESXi to become unresponsive and log errors that the SCSILinuxQueueCommand had failed. This could cause caching software to stop "due to internal errors."

Resolution:
The process queues for the Fusion ioMemory VSL software are now balanced.

Solaris Changes

Fixed Solaris Issues

-- Fixed reported CPU usage
Issue:
The reported VSL software CPU consumption on Solaris was 100%.

Resolution:
The reported/actual CPU consumption on Solaris is now decreased.


3.2.10 CHANGE LOG

In addition to various improvements, the following are changes made to the Fusion ioMemory VSL software since version 3.2.9, including:
General Changes
General Improvements and Features
-- Removed misleading "Suspect Pad" debug message. (FH-22575)
-- Updated supported operating systems.

o Newly Supported Operating Systems:
Linux

-- RHEL 6.6
-- SLES 12
-- Oracle Linux 6.6
-- CentOS 6.6

Windows Changes
Windows Improvements and Features
-- Fixed Windows Installer prevents Blue screen of death with "BAD_POOL_CALLER" and code "C2"

Issue:
Running VSL 3.2.10 on Windows could result in a blue screen of death (BSOD) following an unclean shutdown, which may occur following the driver upgrade to 3.2.10. After reboot, the blue screen of death displayed "BAD_POOL_CALLER" and code "C2". This condition persists across subsequent reboots.
Resolution:
A new installer is available that prevents this issue from occurring. You can download and use the new installer named "Fusion_ioMemory_VSL_3.2.10.1509-2_x64.exe" https://link.sandisk.com/commercialsupport .
NOTE: If a user has already encountered this issue, please see the "Known Issues - Windows Specific" section of this document for Recovery information. (FH-23156)

Linux Changes
Linux Improvements and Features
-- Improved logging for invalid Linux I/O requests. (FH-22281)

Solaris Changes
Fixed Solaris Issues
-- Fixed Solaris load issue
Issue:
The ioMemory VSL software for Solaris would fail to load due to an installation script error.Resolution:
The ioMemory VSL software now loads properly. (FH-22110)


3.2.11 CHANGE LOG

In addition to various improvements, the following are changes made to the Fusion ioMemory VSL software since version 3.2.10, including:

General Changes
General Improvements and Features
o Updated supported operating systems.
o Newly Supported Operating Systems:
Linux

-- RHEL 7.1
-- Fedora 21
-- Oracle VM 3.2.8, 3.2.9, and 3.3.2
-- Oracle Linux 7.1
-- CentOS 7.1

OS X

-- OSX 10.10

VMware

-- ESXi 6.0 (SCSI only)* libvsl Packaging Change. libvsl for 64-bit management applications is now included with the VSL driver (except for OS X and VMware, which have libvsl provided as a separate package).

Fixed General Issues
-- Rebranded "Fusion-io" to "Fusion" in utilities code. (FH-22751)
-- fio-update-iodrive no longer requires the user to specify a firmware file in order to clear the lock. (FH-23223)
-- Fixed issue upgrading firmware when fio-agent was restarted. In previous VSL versions, fio-update-iodrive may have failed when fio-agent was restarted. (CRT-1)
-- A race condition that occasionally caused latencies up to 1 second has been resolved. This latency potentially affected all VSL 3.2.x drivers prior to VSL 3.2.11. (CRT-24, CRT-25, FH-20639)
-- fio-status now properly reports "Disabled" for the "PCIe Power limit threshold" when using external power or enabling power override. (CRT-41)
-- Fixed a potential infinite loop when reading PCI configuration space. (CRT-40)
-- PCI domain is now available in SDK. Software written using the SDK can access/query the PCI domain. (FH-23067, FH-22740)
-- Fixed memory leak on driver unload. (CRT-29)
-- Fixed incorrect high temperature taking Fusion ioMemory ioDrive device offline. (CRT-15)

Utilities Changes
-- Added support for Unified Utilities on VSL 3.2.11. You can now use a common set of standard 4.2.x utilities to manage and report on multiple generations of Fusion ioMemory devices within your installation. Unified Utilities is compatible between systems running Fusion ioMemory VSL software versions VSL 3.2.11 and 4.2.1. However, only certain operating systems installed with VSL 3.2.11 can use the 4.2.1 utilities. For more information on Unified Utilities, including a list of compatible operating systems on a VSL 3.2.11 system, please see the Support for Unified Utilities section in the 3.2.11 Release Notes.

-- In the 4.2.1 utilities, the fio-kinfo command replaces the fio-proctl command; therefore, if you are running 4.2.1 utilities on a 3.2.11 server, any scripts using the fio-proctl command must be updated to use the new 4.2.1 fio-kinfo command.
Note: If you are using the 3.2.11 utilities on a VSL 3.2.11 server, the fio-proctl command is still supported as in previous releases.

-- Two new commands that were added to the 4.2.1. utilities - fio-control and fio-odometer - are not supported on systems running VSL 3.2.11 (even when using the 4.2.1 Unified Utilities). If a user tries to run them on VSL 3.2.11, they fail and an error message displays.

-- All utilities now display in the header "Fusion utility..." instead of "Fusion-io utility...". (FH-22751)

Linux Changes
Linux Improvements and Features
-- libvsl Packaging Change. libvsl (libvsl.so) for 64-bit management applications is now included with the VSL driver.
    o This means you must now load both the VSL driver and utilities before running command-line utilities such as fio-status.
    o Any procedure that referenced the libvsl package must now assume that the libvsl library is installed with the VSL driver.
-- Fedora 21 is now supported. Support is provided for 3.17.1 and newer kernels. (FH-22893)
-- Support is provided for Linux 3.19 Kernels. (FH-22925)

Fixed Linux Issues
-- The fio-util package now contains a udev rule to create a device alias using the serial number, which enables persistent naming for Block devices. (CRT-2)
-- RPM Build Warning no longer displays. Files were listed twice in the rpm spec file, which generated a warning. The extra lines were removed, and the warning is no longer generated. (FH-22984)
-- Dynamic Kernel Module Support (DKMS) improved. A dkms.conf file was added to make DKMS support complete. (CRT-9, FH-22679)

Windows Changes
Fixed Windows Issues
-- The fio-config -f (force) parameter has been removed. This option previously allowed setting driver load options even if the driver was not installed. On a Windows system with VSL 3.2.11, if a user needs to set configuration parameters before the VSL driver loads, the user can manually edit the registry directly. (FH-23126)
-- Affinity log messages now print the correct device. The VSL affinity log message may have mistakenly displayed fct0 for all device names instead of their correct device name. (FH-22691)
-- Running utilities concurrently will no longer cause a bugcheck blue-screen-of-death under Windows. (FH-23068)
-- Windows Installer repair option was not restoring fio utilities. The Repair option has been removed. (To fix a broken installation of the VSL, users need to follow the instructions in the VSL User Guide to uninstall and re-install the VSL.) (FH-22932)
-- Some VSL driver messages that were not being logged are now being printed to the kernel log. (FH-23167)

-- Fixed blue screen of death after unclean shutdown
Issue:
On systems running VSL 3.2.10 with Windows, after an unclean shutdown and reboot, the Fusion ioMemory device generates a blue screen of death.
Resolution
: On systems running VSL 3.2.11 with Windows, this problem is fixed and no longer occurs. (FH-23156)
Note: If AUTO_ATTACH was set to 2 to avoid this issue, the user may change it back to 1.


VMware Changes
VMware Improvements and Features
-- Support for the block-labelled driver has been removed.
-- Added ESXi 6 support.

Fixed VMware Issues
-- With ESXi, fio-bugreport now removes from the system the vm-support bundle it gathers. (CRT-18)
-- With the VMware SCSI driver, certain I/O errors are now correctly reported as DID_NO_CONNECT instead of DID_BAD_TARGET. (CRT-23)


3.2.13 CHANGE LOG
In addition to various improvements, the following are changes made to the Fusion ioMemory VSL software since version 3.2.11, including:

Windows Changes
Fixed Windows Issues
-- Fixed premature retirement of devices issue
Issue:
Under specific workloads at high temperatures, devices are incorrectly reading NAND, resulting in premature retirement of the devices.

Resolution: Improvements made to ensure NAND output is correctly read, preventing premature retirement of devices (CRT-84).

3.2.14 CHANGE LOG
In addition to various improvements, the following are changes made to the Fusion ioMemory VSL software since version 3.2.13, including:

Linux Fix
Fixed a resource leak which would lead to soft lockups and reduced performance, when a Linux server was configured to use the intel_iommu. (CRT-100)
