dnl ######################################################################
dnl gaul/gaul-devel.spec
dnl ######################################################################
dnl
dnl GAUL - Genetic Algorithm Utility Library
dnl Copyright ©2001-2002, Stewart Adcock <stewart@linux-domain.com>
dnl
dnl The latest version of this program should be available at:
dnl http://www.stewart-adcock.co.uk/
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.  Alternatively, if your project
dnl is incompatible with the GPL, I will probably agree to requests
dnl for permission to use the terms of any other license.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY WHATSOEVER.
dnl
dnl A full copy of the GNU General Public License should be in the file
dnl "COPYING" provided with this distribution; if not, see:
dnl http://www.gnu.org/
dnl
dnl ######################################################################

%define PACKAGE_VERSION 	0
%define RELEASE			1842
%define PATCH			2
%define prefix			/usr/local/

Name:		gaul-devel
Summary:	Genetic Algorithm Utility Library
Version:	%{PACKAGE_VERSION}
Release:	%{RELEASE}
License:	(c) 2000-2002 Stewart Adcock.  Licensed under the terms of the GPL.
Vendor:		"Stewart Adcock" <gaul@linux-domain.com>
Group:		Scientific/Engineering
Source:		gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}.tar.gz
URL:		http://gaul.sourceforge.net/
Requires:	slang-devel
BuildRoot:	%{_tmppath}/gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}-buildroot

%description
The Genetic Algorithm Utility Library (GAUL) is an open source programming library providing genetic algorithms.  Both steady-state and generation based evolution is supported, together with the island model.  GAUL supports the Darwinian, Lamarckian and Baldwininan evolutionary schemes.  Standard mutation, crossover and selection operators are provided, while code hooks additionally allow custom operators.  Several non-evolutionary search heuristics are provided for comparison and local search purposes, including simplex method, hill climbing, simulated annealling and steepest ascent.  Much of the functionality is also available through a simple S-Lang interface.

%prep
%setup -n gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}

%build
dnl./configure --includedir=%{prefix}/include/gaul --libdir=%{prefix}/lib/gaul --bindir=%{prefix}/bin/gaul
./configure
make

%install
rm -rf ${RPM_BUILD_ROOT}
%makeinstall

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-, root, root)
%{_bindir}/*
%{_libdir}/*
%{_includedir}/*
%doc AUTHORS COPYING ChangeLog NEWS README

%changelog
* Tue Dec 24 2002 Stewart Adcock <stewart@linux-domain.com>
- 0.1842-2
- Use 'dnl' not '#' for comments.

* Fri Dec 20 2002 Stewart Adcock <stewart@linux-domain.com>
- 0.1842-0
- Improved description.

* Tue Oct 15 2002 Stewart Adcock <stewart@linux-domain.com>
- 0.1841-0
- Added some extra info to the description.
- Copyright -> License field.

* Thu Oct 03 2002 Stewart Adcock <stewart@linux-domain.com>
- 0.1840-1
- Tidied spec file.

