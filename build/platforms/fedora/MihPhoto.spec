%define name    DiffImg
%define version 1.0
%define release fedora13

Name:           %{name}
Version:        %{version}
Release:        %{release}
Summary:        image comparison tool
Group:          Applications/Text
License:        GPLv2
Packager:       xbee
URL:            http://http://thehive.xbee.net/
BuildArch:      i686
Source:         MihPhoto.tgz
#BuildRequires: qt-devel
Requires:       qt
BuildRoot:      %{_tmppath}/%{name}

%description 
DiffImg is a simple image comparison tool which take two RGB images with the same size as input.

%prep
%setup -n MihPhoto

%build
pwd
cd dev/
qmake-qt4
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