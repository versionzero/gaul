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
%define RELEASE		1840
%define PATCH		1
%define prefix		/usr/local/

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
The Genetic Algorithm Utility Library (GAUL) is an open source programming library designed to assist in the development of code using genetic algorithms.  Both steady-state and generation based evolution is supported, together with the island model.  GAUL supports the Darwinian, Lamarckian and Baldwininan evolutionary schemes.  Standard mutation, crossover and selection operators are provided, while code hooks additionally allow custom operators.  Much of the functionality is also available through a simple S-Lang interface.

%prep

%setup -n gaul-devel-%{PACKAGE_VERSION}.%{RELEASE}-%{PATCH}

%build
./configure --includedir=%{prefix}/include/gaul --libdir=%{prefix}/lib/gaul
make

%install
make install

%files
%defattr(-, root, root)
%{prefix}/lib/gaul/libgaul.so.*
%{prefix}/lib/gaul/libstuff.so.*
%{prefix}/lib/gaul/libnn_util.so.*
%{prefix}/lib/gaul/liblog_util.so.*
%{prefix}/lib/gaul/liblog_util_parallel.so.*
%{prefix}/lib/gaul/libmpi_util.so.*
%{prefix}/lib/gaul/libmpi_util_fake.so.*
#%{prefix}/lib/gaul/libbtree.so.*
%{prefix}/lib/gaul/libgaul.la
%{prefix}/lib/gaul/libstuff.la
%{prefix}/lib/gaul/libnn_util.la
%{prefix}/lib/gaul/liblog_util.la
%{prefix}/lib/gaul/liblog_util_parallel.la
%{prefix}/lib/gaul/libmpi_util.la
%{prefix}/lib/gaul/libmpi_util_fake.la
#%{prefix}/lib/gaul/libbtree.la
%{prefix}/lib/gaul/libgaul.a
%{prefix}/lib/gaul/libstuff.a
%{prefix}/lib/gaul/libnn_util.a
%{prefix}/lib/gaul/liblog_util.a
%{prefix}/lib/gaul/liblog_util_parallel.a
%{prefix}/lib/gaul/libmpi_util.a
%{prefix}/lib/gaul/libmpi_util_fake.a
#%{prefix}/lib/gaul/libbtree.a
%{prefix}/lib/gaul/
%{prefix}/include/gaul/ga_bitstring.h
%{prefix}/include/gaul/ga_chromo.h
%{prefix}/include/gaul/ga_core.h
%{prefix}/include/gaul/ga_intrinsics.h
%{prefix}/include/gaul/ga_optim.h
%{prefix}/include/gaul/ga_qsort.h
%{prefix}/include/gaul/ga_similarity.h
%{prefix}/include/gaul/ga_utility.h
%{prefix}/include/gaul/gaul.h
%{prefix}/include/gaul/SAA_header.h
%{prefix}/include/gaul/avltree.h
%{prefix}/include/gaul/compatibility.h
%{prefix}/include/gaul/dstr_util.h
%{prefix}/include/gaul/linkedlist.h
%{prefix}/include/gaul/log_util.h
%{prefix}/include/gaul/memory_chunks.h
%{prefix}/include/gaul/memory_util.h
%{prefix}/include/gaul/mpi_util.h
%{prefix}/include/gaul/nn_util.h
%{prefix}/include/gaul/random_util.h
%{prefix}/include/gaul/str_match.h
%{prefix}/include/gaul/str_util.h
%{prefix}/include/gaul/table_util.h
%{prefix}/include/gaul/memory_chunks.c
%{prefix}/include/gaul/timer_util.h
%{prefix}/include/gaul/
%{prefix}/bin/diagnostics
%{prefix}/bin/pingpong
%{prefix}/bin/wildfire
%{prefix}/bin/goldberg1
%{prefix}/bin/goldberg2
%{prefix}/bin/royalroad
%{prefix}/bin/royalroad_ss
%{prefix}/bin/struggle
%{prefix}/bin/struggle_forked
%{prefix}/bin/struggle2
%{prefix}/bin/struggle3
%{prefix}/bin/struggle4
%{prefix}/bin/struggle5
#%{prefix}/bin/struggle5_mp
%{prefix}/bin/struggle5_forked
%{prefix}/bin/struggle_ss
%{prefix}/bin/nn
%{prefix}/bin/nnevolve
%{prefix}/bin/saveload

%doc AUTHORS COPYING ChangeLog NEWS README

