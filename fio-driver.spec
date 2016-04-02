%define fio_version        2.3.11.183
%define fio_sysconfdir     /etc
%define fio_oem_name       fusionio
%define fio_oem_name_short fio
%define fio_sourcedir      /usr/src
%define fio_driver_name    iomemory-vsl


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


# Older kernels don't build Module.symvers, rhel4, sles10, centos4
%define has_module_symvers 1

# http://en.opensuse.org/openSUSE:Build_Service_cross_distribution_howto
# Suse is nice
%if 0%{?sles_version} == 10
%define has_module_symvers 0
%endif
# wish RHEL had something similar, hack around it
%if "%{_vendor}" == "redhat"
%define has_module_symvers %( if [ -n "$( sed -n -e '/Red.*Enterprise .* release 4/p' -e '/CentOS release 4/p' /etc/redhat-release)" ] ; then echo 0 ; else echo 1 ; fi )
%endif


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
rsync -rv root/usr/src/fio/iomemory-vsl/ driver_source/
if ! %{__make} \
        -C driver_source \
        KERNELVER=%{rpm_kernel_version} \
        FIO_DRIVER_NAME=%{fio_driver_name} \
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

mkdir -p "${RPM_BUILD_ROOT}/usr/src/fio/iomemory-vsl/include/fio/port/linux"
%if "%{?has_module_symvers}" == "1"
    cp "driver_source/Module.symvers" "${RPM_BUILD_ROOT}/usr/src/fio/iomemory-vsl/"
%endif
cp "driver_source/include/fio/port/linux/kfio_config.h" "${RPM_BUILD_ROOT}/usr/src/fio/iomemory-vsl/include/fio/port/linux/"


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
/sbin/depmod -a %{rpm_kernel_version}


%postun -n %{name}-%{rpm_kernel_version}
/sbin/depmod -a %{rpm_kernel_version}


%files -n %{name}-%{rpm_kernel_version}
%defattr(-, root, root)
/lib/modules/%{rpm_kernel_version}/extra/%{fio_oem_name_short}/iomemory-vsl.ko
/usr/share/doc/iomemory-vsl-%{rpm_kernel_version}/License


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
%if "%{?has_module_symvers}" == "1"
/usr/src/fio/iomemory-vsl/Module.symvers
%endif
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kfio_config.h


%package source
Summary: Source to build driver for ioMemory devices from Fusion-io
Group: System Environment/Kernel
Release: %{fio_release}
Obsoletes: iodrive-driver-source, fio-driver-source

%description source
Source to build driver for ioMemory devices from Fusion-io


%files source
%defattr(-, root, root)
/usr/src/fio/iomemory-vsl/Kbuild
/usr/src/fio/iomemory-vsl/Makefile
/usr/src/fio/iomemory-vsl/cdev.c
/usr/src/fio/iomemory-vsl/check_target_kernel.sh
/usr/src/fio/iomemory-vsl/driver_init.c
/usr/src/fio/iomemory-vsl/errno.c
/usr/src/fio/iomemory-vsl/iomemory-vsl.mod.c
/usr/src/fio/iomemory-vsl/katomic.c
/usr/src/fio/iomemory-vsl/kbitops.c
/usr/src/fio/iomemory-vsl/kblock.c
/usr/src/fio/iomemory-vsl/kcache.c
/usr/src/fio/iomemory-vsl/kcondvar.c
/usr/src/fio/iomemory-vsl/kcsr.c
/usr/src/fio/iomemory-vsl/kexports.c
/usr/src/fio/iomemory-vsl/kfile.c
/usr/src/fio/iomemory-vsl/kfio.c
/usr/src/fio/iomemory-vsl/kfio_config.sh
/usr/src/fio/iomemory-vsl/kinfo.c
/usr/src/fio/iomemory-vsl/kmem.c
/usr/src/fio/iomemory-vsl/kmisc.c
/usr/src/fio/iomemory-vsl/kscatter.c
/usr/src/fio/iomemory-vsl/ksimple_event.c
/usr/src/fio/iomemory-vsl/ktime.c
/usr/src/fio/iomemory-vsl/main.c
/usr/src/fio/iomemory-vsl/pci.c
/usr/src/fio/iomemory-vsl/port-internal.h
/usr/src/fio/iomemory-vsl/sched.c
/usr/src/fio/iomemory-vsl/sysrq.c
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc34_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc41_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc42_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc43_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc44_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc45_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc46_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/kfio/x86_64_cc47_libkfio.o_shipped
/usr/src/fio/iomemory-vsl/include/fio/port/arch/bits.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/mips_atomic.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/mips_cache.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/ppc_atomic.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/ppc_cache.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/sparc_cache.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/x86_atomic.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/x86_cache.h
/usr/src/fio/iomemory-vsl/include/fio/port/arch/x86_64/bits.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/kassert.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/ktypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/commontypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/kfio.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/compiler.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/ufio.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/stdint.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/utypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/kcondvar.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/kfile.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/div64.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/errno.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/bdev_us_common.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/nbd-compat.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/nbd_funcs.h
/usr/src/fio/iomemory-vsl/include/fio/port/userspace/blkdev.h
/usr/src/fio/iomemory-vsl/include/fio/port/bitops.h
/usr/src/fio/iomemory-vsl/include/fio/port/cdev.h
/usr/src/fio/iomemory-vsl/include/fio/port/commontypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/compiler.h
/usr/src/fio/iomemory-vsl/include/fio/port/config.h
/usr/src/fio/iomemory-vsl/include/fio/port/config_macros.h
/usr/src/fio/iomemory-vsl/include/fio/port/config_macros_clear.h
/usr/src/fio/iomemory-vsl/include/fio/port/config_vars_externs.h
/usr/src/fio/iomemory-vsl/include/fio/port/csr_simulator.h
/usr/src/fio/iomemory-vsl/include/fio/port/dbgset.h
/usr/src/fio/iomemory-vsl/include/fio/port/errno.h
/usr/src/fio/iomemory-vsl/include/fio/port/fio-poppack.h
/usr/src/fio/iomemory-vsl/include/fio/port/fio-port.h
/usr/src/fio/iomemory-vsl/include/fio/port/fio-pshpack1.h
/usr/src/fio/iomemory-vsl/include/fio/port/fio-stat.h
/usr/src/fio/iomemory-vsl/include/fio/port/fiostring.h
/usr/src/fio/iomemory-vsl/include/fio/port/ifio.h
/usr/src/fio/iomemory-vsl/include/fio/port/ioctl.h
/usr/src/fio/iomemory-vsl/include/fio/port/kbio.h
/usr/src/fio/iomemory-vsl/include/fio/port/kbitops.h
/usr/src/fio/iomemory-vsl/include/fio/port/kblock.h
/usr/src/fio/iomemory-vsl/include/fio/port/kbmp.h
/usr/src/fio/iomemory-vsl/include/fio/port/kcache.h
/usr/src/fio/iomemory-vsl/include/fio/port/kchunk.h
/usr/src/fio/iomemory-vsl/include/fio/port/kcondvar.h
/usr/src/fio/iomemory-vsl/include/fio/port/kcsr.h
/usr/src/fio/iomemory-vsl/include/fio/port/kdebug.h
/usr/src/fio/iomemory-vsl/include/fio/port/kfio.h
/usr/src/fio/iomemory-vsl/include/fio/port/kfio_config.h
/usr/src/fio/iomemory-vsl/include/fio/port/kglobal.h
/usr/src/fio/iomemory-vsl/include/fio/port/kinfo.h
/usr/src/fio/iomemory-vsl/include/fio/port/kmem.h
/usr/src/fio/iomemory-vsl/include/fio/port/kmisc.h
/usr/src/fio/iomemory-vsl/include/fio/port/kscatter.h
/usr/src/fio/iomemory-vsl/include/fio/port/ksimple_event.h
/usr/src/fio/iomemory-vsl/include/fio/port/ktime.h
/usr/src/fio/iomemory-vsl/include/fio/port/ktypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/ktypes_32.h
/usr/src/fio/iomemory-vsl/include/fio/port/ktypes_64.h
/usr/src/fio/iomemory-vsl/include/fio/port/libgen.h
/usr/src/fio/iomemory-vsl/include/fio/port/list.h
/usr/src/fio/iomemory-vsl/include/fio/port/pci.h
/usr/src/fio/iomemory-vsl/include/fio/port/sched.h
/usr/src/fio/iomemory-vsl/include/fio/port/stdint.h
/usr/src/fio/iomemory-vsl/include/fio/port/tree.h
/usr/src/fio/iomemory-vsl/include/fio/port/trim_completion.h
/usr/src/fio/iomemory-vsl/include/fio/port/trimmon_attach.h
/usr/src/fio/iomemory-vsl/include/fio/port/ufio.h
/usr/src/fio/iomemory-vsl/include/fio/port/utypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/commontypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/compiler.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/div64.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/errno.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kassert.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kblock.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kcondvar.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kfile.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/kfio.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/ktypes.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/stdint.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/ufio.h
/usr/src/fio/iomemory-vsl/include/fio/port/linux/utypes.h


%changelog
* Mon Dec 07 2009 23:19:12 -0700 Support <support@fusionio.com>
- Initial build.
