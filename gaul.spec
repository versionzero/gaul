# Note that this is NOT a relocatable package
%define ver      0.1-010516
%define  RELEASE 1
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix   /usr

Summary: Genetic Algorithm Utility Library
Name: GAUL
Version: %ver
Release: %rel
Copyright: GPL
Group: Math
Source: none
BuildRoot: /var/tmp/gaul-%{PACKAGE_VERSION}-root
URL: http://www.stewart-adcock.co.uk/
Docdir: %{prefix}/doc

%description
Genetic Algorithm Utility Library.

%package devel
Summary: Genetic Algorithm Utility Library developers stuff.
Group: Math

%description devel
Genetic Algorithm Utility Library.  Development stuff.

%changelog

* Thu Jan 04 2001 Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
- First spec file.

* Fri Feb 09 2001 Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
- Added documentation directory.
- Fixed typos.

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%prefix
make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)

%doc AUTHORS COPYING ChangeLog NEWS README
%{prefix}/gaul

%files devel
%defattr(-, root, root)

%{prefix}/methods/*
%{prefix}/molstruct/*
%{prefix}/tests/*
%{prefix}/Docs/*
%{prefix}/src/*.c
%{prefix}/src/*.h
