# Note that this is NOT a relocatable package
%define ver      0.1-010516
%define  RELEASE 1
%define  rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix   /usr

Summary: HelGA - Helix Packing by a Genetic Algorithm
Name: helga
Version: %ver
Release: %rel
Copyright: GPL
Group: Math
Source: none
BuildRoot: /var/tmp/helga-%{PACKAGE_VERSION}-root
URL: http://www.stewart-adcock.co.uk/
Docdir: %{prefix}/doc

%description
Helix Packing by a Genetic Algorithm.

%package devel
Summary: Helix Packing by a Genetic Algorithm developers stuff.
Group: Math

%description devel
Helix Packing by a Genetic Algorithm.  Development stuff.

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
%{prefix}/helga

%files devel
%defattr(-, root, root)

%{prefix}/methods/*
%{prefix}/molstruct/*
%{prefix}/tests/*
%{prefix}/Docs/*
%{prefix}/src/*.c
%{prefix}/src/*.h
