%define name    MihPhoto
%define version 1.04
%define release opensuse11.4

Name:           %{name}
Version:        %{version}
Release:        %{release}
Summary:        Photo viewer for touchscreen devices
Group:          Applications/Text
License:        GPLv3
Packager:       Mihai Paslariu
URL:            http://mihplaesu.home.ro
BuildArch:      x86_64
Source:         MihPhoto.tgz
#BuildRequires: qt-devel
Requires:       qt
BuildRoot:      %{_tmppath}/%{name}

%description 
A simple image viewer for touchscreens, based on QT.

%prep
%setup -n MihPhoto

%build
pwd
cd dev/
qmake
make %{?_smp_mflags}

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/opt
mkdir -p $RPM_BUILD_ROOT/opt/%{name}
mkdir -p $RPM_BUILD_ROOT/opt/%{name}/bin
cp bin/MihPhoto $RPM_BUILD_ROOT/opt/%{name}/bin/
cp -r icons $RPM_BUILD_ROOT/opt/%{name}/
cp -r images $RPM_BUILD_ROOT/opt/%{name}/
cp AUTHORS $RPM_BUILD_ROOT/opt/%{name}/
cp COPYING $RPM_BUILD_ROOT/opt/%{name}/
cp CREDITS $RPM_BUILD_ROOT/opt/%{name}/

%post
ln -s /opt/%{name}/bin/MihPhoto /%{_bindir}/MihPhoto

%preun
rm -f /%{_bindir}/MihPhoto

%clean
#if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
rm -rd $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/%{name}

%files 
%defattr(-,root,root,-)
%doc 
/opt/%{name}

%changelog
* Sun Oct 10 2010 Mihai Paslariu <>
- Initial version