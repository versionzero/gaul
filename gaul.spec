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

%define PACKAGE_VERSION 0
%define RELEASE 1836

Summary: Genetic Algorithm Utility Library
Name: GAUL
Version: %{PACKAGE_VERSION}
Release: %{RELEASE}
Copyright: Copyright: (c) 2000-2002 Stewart Adcock, released under GPL.  See COPYING.
Group: Math
Source: gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}.tar.gz
URL: http://gaul.sourceforge.net/

%description
GAUL: Genetic Algorithm Utility Library.  GAUL is a flexible programming library designed to aid in the development of applications that require the use of genetic algorithms.   It provides data structures and functions for handling and manipulation of the data required for a genetic algorithm. Additional stochastic algorithms are provided for comparison to the genetic algorithms. Much of the functionality is also available through a simple SLang interface.

%prep
%setup

%build
make

%install
make install

%files
%defattr(-, root, root)

%doc AUTHORS COPYING ChangeLog NEWS README
%{prefix}/gaul

#%files devel
#%defattr(-, root, root)

#%{prefix}/tests/*
#%{prefix}/docs/*
