## Process this file with automake to produce Makefile.in
#######################################################################
# methods Makefile.am
#######################################################################
#
# methods - General collection of c-programming utitily routines.
# Copyright ©2000-2001, Stewart Adcock <stewart@linux-domain.com>
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

lib_LIBRARIES = libstuff.a \
	libmpi_util.a libnn_util.a librandom.a libstr_util.a libtimer_util.a

bin_PROGRAMS = nn

nn_SOURCES = \
	nn_main.c

libstuff_a_SOURCES = \
	avltree.c \
	compatibility.c \
	log_util.c \
	linkedlist.c \
	memory_util.c \
	table_util.c

libmpi_util_a_SOURCES = \
	mpi_util.c 

libnn_util_a_SOURCES = \
	nn_util.c

librandom_a_SOURCES = \
	random_util.c

libstr_util_a_SOURCES = \
	str_util.c \
	dstr_util.c \
	str_match.c

libtimer_util_a_SOURCES = \
	timer_util.c

include_HEADERS = \
     SAA_header.h \
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

nn_LDADD = -L../src/ -lnn_util -lmethods -lrandom -lstr_util -lm

libstuff_a_LIBFLAGS = -lm @MPIFLAGS@

libnn_util_a_LIBFLAGS = @MPIFLAGS@

libmpi_util_a_LIBFLAGS = @MPIFLAGS@

librandom_a_LIBFLAGS = -lm @MPIFLAGS@

libstr_util_a_LIBFLAGS = @MPIFLAGS@

libtimer_util_a_LIBFLAGS = @MPIFLAGS@


