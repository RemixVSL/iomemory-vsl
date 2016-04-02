%define fio_version        3.2.10.1509
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


Summary: Driver for ioMemory devices from Fusion-io
Name: iomemory-vsl
Vendor: Fusion-io
Version: %{fio_version}
Release: %{fio_release}
Obsoletes: iodrive-driver-kmod, iodrive-driver, fio-driver
License: Proprietary
Group: System Environment/Kernel
URL: http://support.fusionio.com/
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
Drive for ioMemory devices from Fusion-io


%prep
%setup -q -n %{name}-%{fio_tar_version}


%build
rsync -rv root/usr/src/iomemory-vsl-3.2.10/ driver_source/
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

mkdir -p "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.10/include/fio/port/linux"
touch -a "driver_source/Module.symvers"
cp "driver_source/Module.symvers" "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.10/"
cp "driver_source/include/fio/port/linux/kfio_config.h" "${RPM_BUILD_ROOT}/usr/src/iomemory-vsl-3.2.10/include/fio/port/linux/"


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
Summary: Driver for ioMemory devices from Fusion-io
Group: System Environment/Kernel
Provides: iomemory-vsl
Provides: iomemory-vsl-%{fio_version}
Obsoletes: iodrive-driver, fio-driver

%description -n %{name}-%{rpm_kernel_version}
Driver for fio devices


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
if hash dracut
then
    dracut -f
fi



%postun -n %{name}-%{rpm_kernel_version}
if [ -x "/sbin/weak-modules" ]; then
    echo "/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko" \
        | /sbin/weak-modules --remove-modules
fi
/sbin/depmod -a %{rpm_kernel_version}
if hash dracut
then
    dracut -f
fi


%files -n %{name}-%{rpm_kernel_version}
%defattr(-, root, root)
/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko
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
/usr/src/iomemory-vsl-3.2.10/Module.symvers
/usr/src/iomemory-vsl-3.2.10/include/fio/port/linux/kfio_config.h


%package source
Summary: Source to build driver for ioMemory devices from Fusion-io
Group: System Environment/Kernel
Release: %{fio_release}
Obsoletes: iodrive-driver-source, fio-driver-source
Provides: iomemory-vsl, iomemory-vsl-%{fio_version}, libvsl
Obsoletes: iodrive-driver, fio-driver

%description source
Source to build driver for ioMemory devices from Fusion-io


%post source
if [ -x "/usr/sbin/dkms" ]; then
    /usr/sbin/dkms install -m iomemory-vsl -v 3.2.10
fi
ldconfig


%postun source
if [ -x "/usr/sbin/dkms" ]; then
    /usr/sbin/dkms uninstall -m iomemory-vsl -v 3.2.10
fi
ldconfig


%files source
%defattr(-, root, root)
/usr/src/iomemory-vsl-3.2.10/Kbuild
/usr/src/iomemory-vsl-3.2.10/Makefile
/usr/src/iomemory-vsl-3.2.10/dkms.conf
/usr/src/iomemory-vsl-3.2.10/cdev.c
/usr/src/iomemory-vsl-3.2.10/driver_init.c
/usr/src/iomemory-vsl-3.2.10/check_target_kernel.sh
/usr/src/iomemory-vsl-3.2.10/errno.c
/usr/src/iomemory-vsl-3.2.10/iomemory-vsl.mod.c
/usr/src/iomemory-vsl-3.2.10/katomic.c
/usr/src/iomemory-vsl-3.2.10/kbitops.c
/usr/src/iomemory-vsl-3.2.10/kblock.c
/usr/src/iomemory-vsl-3.2.10/kscsi.c
/usr/src/iomemory-vsl-3.2.10/kscsi_host.c
/usr/src/iomemory-vsl-3.2.10/kcache.c
/usr/src/iomemory-vsl-3.2.10/kcondvar.c
/usr/src/iomemory-vsl-3.2.10/kcsr.c
/usr/src/iomemory-vsl-3.2.10/kexports.c
/usr/src/iomemory-vsl-3.2.10/kfile.c
/usr/src/iomemory-vsl-3.2.10/kfio.c
/usr/src/iomemory-vsl-3.2.10/kfio_config.sh
/usr/src/iomemory-vsl-3.2.10/khotplug.c
/usr/src/iomemory-vsl-3.2.10/kinfo.c
/usr/src/iomemory-vsl-3.2.10/kmem.c
/usr/src/iomemory-vsl-3.2.10/kmisc.c
/usr/src/iomemory-vsl-3.2.10/kscatter.c
/usr/src/iomemory-vsl-3.2.10/ktime.c
/usr/src/iomemory-vsl-3.2.10/main.c
/usr/src/iomemory-vsl-3.2.10/module_param.c
/usr/src/iomemory-vsl-3.2.10/pci.c
/usr/src/iomemory-vsl-3.2.10/port-internal.h
/usr/src/iomemory-vsl-3.2.10/sched.c
/usr/src/iomemory-vsl-3.2.10/state.c
/usr/src/iomemory-vsl-3.2.10/sysrq.c
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/bits.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/mips_atomic.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/mips_cache.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/ppc_atomic.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/ppc_cache.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/x86_atomic.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/x86_cache.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/arch/x86_64/bits.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/align.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/atomic_list.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/bitops.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/byteswap.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/cdev.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/commontypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/compiler.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/port_config.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/port_config_macros.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/port_config_macros_clear.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/port_config_vars_externs.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/csr_simulator.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/dbgset.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/errno.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/fio-poppack.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/fio-port.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/fio-pshpack1.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/fio-stat.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/fiostring.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ifio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ioctl.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kbio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kbitops.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kblock.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kbmp.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kcache.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kchunk.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kcondvar.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kcpu.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kcsr.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kdebug.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kfio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kfio_config.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kglobal.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kinfo.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kmem.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kmisc.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kscatter.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/kscsi.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ktime.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ktypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ktypes_32.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ktypes_64.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/libgen.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/list.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/pci.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/sched.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/state.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/stdint.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/trim_completion.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/trimmon_attach.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/unaligned.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/ufio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/utypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/vararg.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/vectored.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/commontypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/compiler.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/div64.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/errno.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kassert.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kblock.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kcondvar.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kfile.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kfio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kscsi_config.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/kscsi_host.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/stdint.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/common-linux/ufio.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/linux/ktypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/linux/utypes.h
/usr/src/iomemory-vsl-3.2.10/include/fio/port/gcc/align.h
/usr/src/iomemory-vsl-3.2.10/Kbuild
/usr/src/iomemory-vsl-3.2.10/Makefile
/usr/src/iomemory-vsl-3.2.10/cdev.c
/usr/src/iomemory-vsl-3.2.10/check_target_kernel.sh
/usr/src/iomemory-vsl-3.2.10/driver_init.c
/usr/src/iomemory-vsl-3.2.10/errno.c
/usr/src/iomemory-vsl-3.2.10/iomemory-vsl.mod.c
/usr/src/iomemory-vsl-3.2.10/katomic.c
/usr/src/iomemory-vsl-3.2.10/kbitops.c
/usr/src/iomemory-vsl-3.2.10/kblock.c
/usr/src/iomemory-vsl-3.2.10/kcache.c
/usr/src/iomemory-vsl-3.2.10/kcondvar.c
/usr/src/iomemory-vsl-3.2.10/kcsr.c
/usr/src/iomemory-vsl-3.2.10/kexports.c
/usr/src/iomemory-vsl-3.2.10/kfile.c
/usr/src/iomemory-vsl-3.2.10/kfio.c
/usr/src/iomemory-vsl-3.2.10/kfio_config.sh
/usr/src/iomemory-vsl-3.2.10/khotplug.c
/usr/src/iomemory-vsl-3.2.10/kinfo.c
/usr/src/iomemory-vsl-3.2.10/kmem.c
/usr/src/iomemory-vsl-3.2.10/kmisc.c
/usr/src/iomemory-vsl-3.2.10/kscatter.c
/usr/src/iomemory-vsl-3.2.10/kscsi.c
/usr/src/iomemory-vsl-3.2.10/kscsi_host.c
/usr/src/iomemory-vsl-3.2.10/ktime.c
/usr/src/iomemory-vsl-3.2.10/license.c
/usr/src/iomemory-vsl-3.2.10/main.c
/usr/src/iomemory-vsl-3.2.10/module_param.c
/usr/src/iomemory-vsl-3.2.10/pci.c
/usr/src/iomemory-vsl-3.2.10/port-internal.h
/usr/src/iomemory-vsl-3.2.10/sched.c
/usr/src/iomemory-vsl-3.2.10/state.c
/usr/src/iomemory-vsl-3.2.10/sysrq.c
/usr/src/iomemory-vsl-3.2.10/kfio/.x86_64_cc48_libkfio.o.cmd
/usr/src/iomemory-vsl-3.2.10/kfio/x86_64_cc48_libkfio.o_shipped


%changelog


* Wed Feb 04 2015 17:42:11 +0000  Support <support@fusionio.com> 

GENERAL CHANGES
---------------

General Improvements and Features

        * The fio-sure-erase utility now reports the device serial number upon
          completion.
        * The fio-status utility now provides the thresholds for when
          performance throttling engages due to high temperature conditions at
          the controller or NAND memory devices. These values can be used by
          customer management processes to predict when high temperature
          conditions will impact device performance or when additional cooling
          measures need to be taken. Example
          output:
                fio-status -fj

                        "controller_throttle_start_deg_c" : "78",
                        "nand_throttle_start_deg_c" : "78",

Fixed General Issues

        ISSUE: Under specific workloads and in specific system environments a
               customer may experience longer than expected latencies on
               ioMemory devices.
               The behavior has been reported with ioMemory VSL 3.2.x. The
               frequency of occurrence is dependent on the workload and system
               environment.
        RESOLUTION: This release includes fixes that reduce the frequency of
                    latency spikes in many scenarios.


WINDOWS CHANGES
---------------

Fixed Windows Issues

        ISSUE: If your system is using a storage device in addition to an
               ioMemory device, the system may show duplicate SCSI IDs causing
               conflicts.
        RESOLUTION: Use the fio-config utility and the WIN_SCSI_BUS_ID module
                    parameter to set the device ID for all ioMemory devices to
                    a SCSI ID number that doesn't conflict with other SCSI
                    devices. See the fio-config section in the Command Utilities
                    section of the ioMemory VSL User Guide for Windows for more
                    information.

        ISSUE: When more than one process tries to access PCI device information
               (for example running multiple instances of fio-pci-check in
               parallel), the first process will erroneously return information
               for a later process.
        RESOLUTION: The configuration information is locked so only one process
                    can access it at a time.


LINUX CHANGES
-------------

Linux Improvements and Features

        * Added DKMS support for Linux source packages. When you update the
        system kernel and DKMS is installed and configured correctly, the
        ioMemory VSL software will be configured for the new kernel. This
        includes updates to ioMemory VSL driver loading configuration.
        * Improved kernel configuration code for building from source.

Fixed Linux Issues

        ISSUE: When the Linux operating system issued an incorrectly formed
               request the ioMemory VSL software would crash.
        RESOLUTION: The ioMemory VSL software now rejects the invalid request.

        ISSUE: When building the software package from source, the 'make clean'
               option would fail.
        RESOLUTION: This option now works as expected.

        ISSUE: Updating the system kernel or ioMemory VSL software would insert
               the ioMemory VSL software into the initrd and ignore the software
               configuration file (/etc/modprobe.d/iomemory-vsl.conf). This
               issue created software loading issues that required dracut to
               fix.
        RESOLUTION: Multiple solutions to this configuration issue:
                1) If DKMS is installed and configured the ioMemory VSL software
                   is configured for the new kernel.
                2) If dracut is installed, the ioMemory VSL software uses that
                   program to fix the configuration issue.
                3) If both the fio-sysvinit and fio-common packages are
                   installed, the ioMemory VSL software is excluded from the
                   initrd.


VMWARE CHANGES
--------------

Fixed VMware Issues

        ISSUE: An imbalance in process queue sizes would cause the ioMemory VSL
               software for ESXi to become unresponsive and log errors that the
               SCSILinuxQueueCommand had failed. This could cause caching
               software to stop "due to internal errors."
        RESOLUTION: The process queues for the ioMemory VSL software are now
                    balanced.


SOLARIS CHANGES
---------------

Fixed Solaris Issues

        ISSUE: The reported VSL software CPU consumption on Solaris was 100%.
        RESOLUTION: CPU consumption on Solaris is now decreased.


Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
