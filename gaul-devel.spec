#######################################################################
# gaul/gaul-devel.spec
#######################################################################
#
# GAUL - Genetic Algorithm Utility Library
# Copyright ©2001-2002, Stewart Adcock <stewart@linux-domain.com>
#
# The latest version of this program should be available at:
# http://www.stewart-adcock.co.uk/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.  Alternatively, if your project
# is incompatible with the GPL, I will probably agree to requests
# for permission to use the terms of any other license.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY WHATSOEVER.
#
# A full copy of the GNU General Public License should be in the file
# "COPYING" provided with this distribution; if not, see:
# http://www.gnu.org/
#
#######################################################################

%define PACKAGE_VERSION 	0
%define RELEASE			1840
%define PATCH			2
%define prefix			/usr/local/

Summary:	Genetic Algorithm Utility Library
Name:		gaul-devel
Version:	%{PACKAGE_VERSION}
Release:	%{RELEASE}
Copyright:	(c) 2000-2002 Stewart Adcock.  Licensed under the terms of the GPL.
Vendor:		"Stewart Adcock" <gaul@linux-domain.com>
# License:	GPL
Group:		Scientific/Engineering
Source:		gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}.tar.gz
URL:		http://gaul.sourceforge.net/
Requires:	slang-devel
BuildRoot:	%{_tmppath}/gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}-buildroot

%description
The Genetic Algorithm Utility Library (GAUL) is an open source programming library designed to assist in the development of code using genetic algorithms.  Both steady-state and generation based evolution is supported, together with the island model.  GAUL supports the Darwinian, Lamarckian and Baldwininan evolutionary schemes.  Standard mutation, crossover and selection operators are provided, while code hooks additionally allow custom operators.  Much of the functionality is also available through a simple S-Lang interface.

%prep
%setup -n gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}

%build
#./configure --includedir=%{prefix}/include/gaul --libdir=%{prefix}/lib/gaul --bindir=%{prefix}/bin/gaul
./configure
make

%install
rm -rf ${RPM_BUILD_ROOT}
%makeinstall

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog NEWS README

%changelog
* Thu Oct 03 2002 Stewart Adcock <stewart@linux-domain.com>
- 0.1840-1
- Tidied spec file.

