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
%define RELEASE 1838
%define PATCH	2

Summary: Genetic Algorithm Utility Library
Name: gaul-devel
Version: %{PACKAGE_VERSION}
Release: %{RELEASE}
Copyright: Copyright: (c) 2000-2002 Stewart Adcock, released under GPL.  See COPYING.
Vendor: "Stewart Adcock" <gaul@linux-domain.com>
Group: Scientific/Engineering
Source: gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}.tar.gz
URL: http://gaul.sourceforge.net/
Requires: slang-devel

%description
GAUL: Genetic Algorithm Utility Library.  GAUL is a flexible programming library designed to aid in the development of applications that require the use of genetic algorithms.   It provides data structures and functions for handling and manipulation of the data required for a genetic algorithm. Additional stochastic algorithms are provided for comparison to the genetic algorithms. Much of the functionality is also available through a simple SLang interface.

%prep

%setup -n gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}

%build
./configure
make

%install
make install

%files
%defattr(-, root, root)
/usr/local/lib/libstuff.a
/usr/local/lib/libnn_util.a
/usr/local/lib/librandom.a
/usr/local/lib/libstr_util.a
/usr/local/lib/libtimer_util.a
/usr/local/lib/liblog_util.a
/usr/local/lib/liblog_util_parallel.a
/usr/local/lib/libmpi_util.a
/usr/local/lib/libmpi_util_fake.a
/usr/local/lib/libbtree.a
/usr/local/lib/libstuff.a
/usr/local/lib/libnn_util.a
/usr/local/lib/librandom.a
/usr/local/lib/libstr_util.a
/usr/local/lib/libtimer_util.a
/usr/local/lib/liblog_util.a
/usr/local/lib/liblog_util_parallel.a
/usr/local/lib/libmpi_util.a
/usr/local/lib/libmpi_util_fake.a
/usr/local/include/ga_bitstring.h
/usr/local/include/ga_chromo.h
/usr/local/include/ga_core.h
/usr/local/include/ga_intrinsics.h
/usr/local/include/ga_optim.h
/usr/local/include/ga_qsort.h
/usr/local/include/ga_similarity.h
/usr/local/include/ga_utility.h
/usr/local/include/gaul.h
/usr/local/include/SAA_header.h
/usr/local/include/avltree.h
/usr/local/include/compatibility.h
/usr/local/include/dstr_util.h
/usr/local/include/linkedlist.h
/usr/local/include/log_util.h
/usr/local/include/memory_chunks.h
/usr/local/include/memory_util.h
/usr/local/include/mpi_util.h
/usr/local/include/nn_util.h
/usr/local/include/random_util.h
/usr/local/include/str_match.h
/usr/local/include/str_util.h
/usr/local/include/table_util.h
/usr/local/include/memory_chunks.c
/usr/local/include/timer_util.h
/usr/local/bin/diagnostics
/usr/local/bin/pingpong
/usr/local/bin/wildfire
/usr/local/bin/goldberg1
/usr/local/bin/goldberg2
/usr/local/bin/royalroad
/usr/local/bin/royalroad_ss
/usr/local/bin/struggle
/usr/local/bin/struggle2
/usr/local/bin/struggle3
/usr/local/bin/struggle4
/usr/local/bin/struggle5
/usr/local/bin/struggle5_mp
/usr/local/bin/struggle_ss
/usr/local/bin/nnevolve

%doc AUTHORS COPYING ChangeLog NEWS README

