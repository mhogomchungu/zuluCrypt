#
# Spec file for package Dolphin Emulator
#
# Copyright © 2011-2015 Francis Banyikwa <mhogomchungu@gmail.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

Name:           zuluCrypt
Version:        4.7.5
Release:        0
Summary:        Qt GUI front end to cryptsetup
License:        GPL-2.0+
Group:          Productivity/Security
Source:         %{name}-%{version}.tar.xz
Source100:      zuluCrypt-rpmlint
URL:            https://github.com/mhogomchungu/zuluCrypt

%define libversion 3_2_0
%define srcname zulucrypt
%define libname %srcname%libversion
%define libnamedev %{srcname}%{libversion}-devel

#You may want to add dependencies for kwallet,gnome-keyring and pwquality below
#if you want to include optional functionality they provide.

BuildRequires: cmake
BuildRequires: device-mapper-devel
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: glibc-devel
BuildRequires: libblkid-devel
BuildRequires: libmount-devel
BuildRequires: libgcrypt-devel
BuildRequires: libsecret-devel
BuildRequires: libpwquality-devel
BuildRequires: pkgconfig(QtCore)
BuildRequires: pkgconfig(QtGui)

%if 0%{?fedora} 
BuildRequires: cryptsetup-luks-devel
%endif

%if 0%{?suse_version}
BuildRequires: libcryptsetup-devel
%endif

%description
zuluCrypt is a front end to cryptsetup.
It makes it easier to use cryptsetup by providing a Qt-based GUI and a simpler to use CLI front end to cryptsetup.
It does the same thing truecrypt does but without licensing problems or requiring a user to setup sudo for it or presenting root's password.
This package contains the applications.

%package -n %{libnamedev}
Requires:       lib%{libname} = %{version}
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

%prep
%setup -q

%build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/usr -DUDEVSUPPORT=true -DCMAKE_BUILD_TYPE=Release

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
rm -rf $RPM_BUILD_DIR/zuluCrypt

%files 
%defattr(0755,root,root)
%{_bindir}/zuluMount-gui
%{_bindir}/zuluMount-cli
%{_bindir}/zuluCrypt-gui
%{_bindir}/zuluCrypt-cli
%{_bindir}/zuluSafe-cli

#It is necessary to comment in the below two lines if you want to build these optional features
#and all dependencies are met.
#%%{_libdir}/zuluCrypt/kwallet
%{_libdir}/zuluCrypt/keyring
%{_libdir}/zuluCrypt/zuluCrypt-testKey
%{_libdir}/zuluCrypt/keykeyfile
%{_libdir}/zuluCrypt/keydialog-qt
%{_libdir}/zuluCrypt/luks
%{_libdir}/zuluCrypt/steghide
%{_libdir}/zuluCrypt/tomb
%{_libdir}/zuluCrypt/gpg
%{_libdir}/zuluCrypt/hmac

%{_datadir}/applications/zuluCrypt.desktop
%{_datadir}/applications/zuluMount.desktop
%defattr(0644,root,root)
%{_datadir}/icons/*
%{_datadir}/pixmaps/*
%{_mandir}/man1/*
%defattr(0644,root,root)

%files -n %{libname}
%defattr(0644,root,root)
%dir %{_libdir}/zuluCrypt
%{_libdir}/libzuluCrypt.so.*
%{_libdir}/libzuluCrypt-exe.so.*
%{_libdir}/libzuluCryptPluginManager.so.*

%files -n %{libnamedev}
%defattr(0644,root,root)
%dir %{_includedir}/zuluCrypt
%{_includedir}/zuluCrypt/libzuluCrypt.h
%{_includedir}/zuluCrypt/libzuluCrypt-exe.h
%{_includedir}/zuluCrypt/libzuluCryptPluginManager.h
%{_libdir}/libzuluCryptPluginManager.so
#%%{_libdir}/libzuluCryptPluginManager-static.a
%{_libdir}/libzuluCrypt.so
%{_libdir}/libzuluCrypt-exe.so
#%%{_libdir}/libzuluCrypt-static.a
#%%{_libdir}/libzuluCrypt-exe-static.a
%{_libdir}/pkgconfig/libzuluCrypt.pc

%changelog

# openSUSE is strict about the date format used.
# To do: Fix date format.

#* Thu May  1 2014 mhogomchungu@gmail.com
#* Fri Mar 15 2013 mhogomchungu@gmail.com
#- upate to version 4.6.2
#* Mon Jan 14 2012 mhogomchungu@gmail.com
#- update to version 4.6.0
