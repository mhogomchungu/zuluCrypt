Name:           zuluCrypt
Version: 4.7.4
Release: 0
Summary:        Qt GUI front end to cryptsetup
License:        GPL-2.0+
Group:          Productivity/Security
Source:         zuluCrypt-%{version}.tar.bz2
Source100:      zuluCrypt-rpmlint
URL:            http://code.google.com/p/zulucrypt/
BuildRoot:      %{_topdir}/%{name}-%{version}

%define zuluCrypt_plugins zuluCrypt_plugins
%define libversion 3.2.0
%define srcname zuluCrypt
%define libname lib%srcname%libversion
%define libnamedev lib%{srcname}%{libversion}-devel

#You may want to add dependencies for kwallet,gnome-keyring and pwquality below
#if you want to include optional functionality they provide.
%if 0%{?fedora}
BuildRequires:  cmake gcc gcc-c++ qt-devel glibc-devel libblkid-devel cryptsetup-luks-devel
#BuildRequires:  cmake gcc gcc-c++ qt-devel gnome-keyring-devel glibc-devel libmount-devel libblkid-devel cryptsetup-luks-devel
%else
BuildRequires:  cmake gcc gcc-c++ libqt4-devel glibc-devel libblkid-devel libcryptsetup-devel libtcplay-devel
%endif

%description
zuluCrypt is a front end to cryptsetup.
It makes it easier to use cryptsetup by providing a Qt based GUI and a simpler to use cli front end to cryptsetup and tcplay.
It does the same thing truecrypt does but without licensing problems or requiring a user to setup sudo for it or presenting root's password.
This package contains the applications.

%package -n %{libnamedev}
Requires:       %{libname} = %{version}
Summary:        Development library package
Group:          Development/Libraries/C and C++

%description -n %{libnamedev}
This package contains development files and libraries necessary to build programs around zulucrypt

%package -n %{libname}
Summary:        Library for %{name}
Group:          Productivity/Security

%description -n %{libname}
This package contains libraries that provide higher level access to cryptsetup API and provide mounting/unmounting API
to easy opening and closing of volumes

%package -n %{zuluCrypt_plugins}
Requires:       %{libname}
Summary:        various zuluCrypt plugins

%description -n %{zuluCrypt_plugins}
zuluCrypt plugins

%prep
%setup -q -n zuluCrypt-%{version}

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ../

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install

%post
chmod 4755 %{_bindir}/zuluCrypt-cli
chmod 4755 %{_bindir}/zuluMount-cli

%post -n %{libnamedev}
sbin/ldconfig

%postun -n %{libnamedev}
sbin/ldconfig

%post -n %{libname}
sbin/ldconfig

%postun -n %{libname}
sbin/ldconfig

%clean
rm -rf %{buildroot}
rm -rf $RPM_BUILD_DIR/%{name}-%{version}

%files
%defattr(0755,root,root)
%{_bindir}/zuluMount-gui
%{_bindir}/zuluMount-cli
%{_bindir}/zuluCrypt-gui
%{_bindir}/zuluCrypt-cli
%{_bindir}/zuluSafe-cli
%{_libdir}/zuluCrypt/zuluCrypt-testKey

%{_datadir}/applications/zuluCrypt.desktop
%{_datadir}/applications/zuluMount.desktop
%defattr(0644,root,root)
%{_datadir}/icons/*
%{_datadir}/pixmaps/*
%{_mandir}/man1/*
%defattr(0644,root,root)

%files -n %{zuluCrypt_plugins}
%defattr(0755,root,root)
#
# you may need to comment plugins you dont want to build
#
%{_libdir}/zuluCrypt/gpg
%{_libdir}/zuluCrypt/keydialog-qt
%{_libdir}/zuluCrypt/keyring
%{_libdir}/zuluCrypt/keykeyfile
# %{_libdir}/zuluCrypt/kwallet
%{_libdir}/zuluCrypt/luks
%{_libdir}/zuluCrypt/steghide
%{_libdir}/zuluCrypt/tomb

%files -n %{libname}
%defattr(0755,root,root)
%dir %{_libdir}/zuluCrypt
%defattr(0644,root,root)
%{_libdir}/libzuluCrypt.so.*
%{_libdir}/libzuluCrypt-exe.so.*
%{_libdir}/libzuluCryptPluginManager.so.*

%files -n %{libnamedev}
%defattr(0755,root,root)
%dir %{_includedir}/zuluCrypt
%defattr(0644,root,root)
%{_includedir}/zuluCrypt/libzuluCrypt.h
%{_includedir}/zuluCrypt/libzuluCrypt-exe.h
%{_includedir}/zuluCrypt/libzuluCryptPluginManager.h
%{_libdir}/libzuluCryptPluginManager.so
%{_libdir}/libzuluCrypt.so
%{_libdir}/libzuluCrypt-exe.so
%{_libdir}/pkgconfig/libzuluCrypt.pc

%changelog
* Thu May 1 2014 mhogomchungu@gmail.com
 - update to version 4.7.0
* Fri Jul 19 2013 mhogomchungu@gmail.com
 - update to version 4.6.5
* Sun Jun  2 2013 mhogomchungu@gmail.com
 - update to version 4.6.4
* Thu May 30 2013 mhogomchungu@gmail.com
 - upate to version 4.6.3
* Fri Mar 15 2013 mhogomchungu@gmail.com
 - upate to version 4.6.2
* Mon Jan 14 2012 mhogomchungu@gmail.com
 - update to version 4.6.0
