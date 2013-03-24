Name:           zuluCrypt
Version: 4.6.2
Release: 0
Summary:        Qt GUI front end to cryptsetup
License:        GPL-2.0+
Group:          Productivity/Security
Source:         zulucrypt-%{version}.tar.bz2
Source100:      zuluCrypt-rpmlint
URL:            http://code.google.com/p/zulucrypt/
BuildRoot:      %{_topdir}/%{name}-%{version}

%define libversion 3_2_0
%define srcname zulucrypt
%define libname %srcname%libversion
%define libnamedev %{srcname}%{libversion}-devel

#You may want to add dependencies for kwallet,gnome-keyring and pwquality below
#if you want to include optional functionality they provide.
%if 0%{?fedora} 
BuildRequires:  cmake gcc gcc-c++ qt-devel glibc-devel libmount-devel libblkid-devel cryptsetup-luks-devel
#BuildRequires:  cmake gcc gcc-c++ qt-devel gnome-keyring-devel glibc-devel libmount-devel libblkid-devel cryptsetup-luks-devel
%else
BuildRequires:  cmake gcc gcc-c++ libqt4-devel glibc-devel libmount-devel libblkid-devel libcryptsetup-devel
%endif

%description
zuluCrypt is a front end to cryptsetup.
It makes it easier to use cryptsetup by providing a Qt based GUI and a simpler to use cli front end to cryptsetup.
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
%setup -q -n zulucrypt-%{version}

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
%{_bindir}/zuluCrypt-test
#It is necessary to comment in the below two lines if you want to build these optional features
#and all dependencies are met.
%{_libdir}/zuluCrypt/kwallet
%{_libdir}/zuluCrypt/keyring
%{_libdir}/zuluCrypt/zuluCrypt-testKey
%{_libdir}/zuluCrypt/keykeyfile
%{_libdir}/zuluCrypt/keydialog-qt
%{_libdir}/zuluCrypt/gpg
%{_datadir}/applications/zuluCrypt.desktop
%{_datadir}/applications/zuluMount.desktop
%defattr(0644,root,root)
%{_datadir}/icons/*
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
* Fri Mar 15 2013 mhogomchungu@gmail.com
 - upate to version 4.6.2
* Mon Jan 14 2012 mhogomchungu@gmail.com
 - update to version 4.6.0
