# Note that this is NOT a relocatable package
%define RELEASE 1
%define prefix  /usr/local

Summary: Genetic Algorithm Utility Library
Name: GAUL
Version: %{PACKAGE_VERSION}
Release: %{RELEASE}
Copyright: GPL
Group: Math
Source: none
BuildRoot: /var/tmp/gaul-%{PACKAGE_VERSION}-root
URL: http://gaul.sourceforge.net/
Docdir: %{prefix}/doc

%description
Genetic Algorithm Utility Library.

%package devel
Summary: Genetic Algorithm Utility Library developers stuff.
Group: Math

%description devel
Genetic Algorithm Utility Library.  Development stuff.

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

%{prefix}/tests/*
%{prefix}/docs/*
