Name:		gaul-devel
Summary:	Genetic Algorithm Utility Library
Version:	0.1846
Release:	0
License:	Copyright (c) 2000-2003, Stewart Adcock.  Licensed under terms of the GNU GPL.
Vendor:		Stewart Adcock
Packager:	"Stewart Adcock" <gaul@linux-domain.com>
Group:		Scientific/Engineering
Source:		gaul-devel-%{PACKAGE_VERSION}-%{PACKAGE_RELEASE}.tar.gz
URL:		http://gaul.sourceforge.net/
Requires:	slang-devel
BuildRoot:	%{_tmppath}/gaul-devel-%{PACKAGE_VERSION}-%{PACKAGE_RELEASE}-buildroot

########################################################################
# gaul/gaul-devel.spec
########################################################################
#
# GAUL - Genetic Algorithm Utility Library
# Copyright ©2001-2003, Stewart Adcock <stewart@linux-domain.com>
# All rights reserved.
#
# The latest version of this program should be available at:
# http://gaul.sourceforge.net/
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
########################################################################

%description
The Genetic Algorithm Utility Library (GAUL) is an open source programming library providing genetic algorithms.  Both steady-state and generation based evolution is supported, together with the island model.  GAUL supports the Darwinian, Lamarckian and Baldwininan evolutionary schemes.  Standard mutation, crossover and selection operators are provided, while code hooks additionally allow custom operators.  Several non-evolutionary search heuristics are provided for comparison and local search purposes, including simplex method, hill climbing, simulated annealling and steepest ascent.  Much of the functionality is also available through a simple S-Lang interface.

%prep
%setup -n gaul-devel-%{PACKAGE_VERSION}-%{PACKAGE_RELEASE}

%build
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
* Wed May 28 2003 Stewart Adcock <stewart@linux-domain.com>
- 0.1843-3
- Tidied, removed dodgy prefix stuff.
- Adjusted to give more appropriate package numbering.

* Thu Jan 09 2003 Stewart Adcock <stewart@linux-domain.com>
- 0.1842-3
- Added packager tag.

* Wed Jan 08 2003 Stewart Adcock <stewart@linux-domain.com>
- That 'dnl' change was stupid because it broke the spec file!
- Copyright notices changed to 2003.

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

