## Process this file with automake to produce Makefile.in
#######################################################################
# util/Makefile.am
#######################################################################
#
# util - General collection of C-programming utility routines.
# Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
# All rights reserved.
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
#

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

EXTRA_DIST = gaul_config.h.in

lib_LTLIBRARIES = libgaul_util.la \
	libnn_util.la

libgaul_util_la_SOURCES = \
	avltree.c \
	compatibility.c \
	linkedlist.c \
	memory_util.c \
	table_util.c \
	random_util.c \
	str_util.c \
	dstr_util.c \
	str_match.c \
	timer_util.c \
	log_util.c \
	mpi_util.c 

libnn_util_la_SOURCES = \
	nn_util.c

include_HEADERS = \
	SAA_header.h \
	gaul_config.h \
	avltree.h \
	compatibility.h \
	dstr_util.h \
	linkedlist.h \
	log_util.h \
	memory_chunks.h \
	memory_util.h \
	mpi_util.h \
	nn_util.h \
	random_util.h \
	str_match.h \
	str_util.h \
	table_util.h \
	memory_chunks.c \
	timer_util.h

libgaul_util_a_LIBFLAGS =
libgaul_util_la_LDFLAGS = -version-info ${GA_MAJOR_VERSION}:${GA_MINOR_VERSION}:0

libnn_util_a_LIBFLAGS =
libnn_util_la_LDFLAGS = -version-info ${GA_MAJOR_VERSION}:${GA_MINOR_VERSION}:0

# Regenerate gaul_config.h with config.status whenever gaul_config.h.in changes.
gaul_config.h: stamp-gaul_config
	@:
stamp-common: $(srcdir)/gaul_config.h.in $(top_builddir)/config.status
	cd $(top_builddir) \
	&& CONFIG_FILES= CONFIG_HEADERS= CONFIG_OTHER=gaul_config.h \
	$(SHELL) ./config.status
	echo timestamp > $@

# Don't distribute gaul_config.h, since it is build host dependent!
dist-hook:
	rm -f $(distdir)/gaul_config.h

