## Process this file with automake to produce Makefile.in
#######################################################################
# methods Makefile.am
#######################################################################
#
# methods - General collection of c-programming utitily routines.
# Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
#
# The latest version of this program should be available at:
# http://www.stewart-adcock.co.uk/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.  Alternatively, if your project
# is incompatiable with the GPL, I will probably agree to requests
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
# Updated:	17/05/01 SAA	This is a highly cut-down version for use in the GAUL CVS archive.
#

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

# CFLAGS = -g -Wall -O

INCLUDES =

lib_LIBRARIES = libmethods.a libstr_util.a librandom.a

libmethods_a_SOURCES = \
	avltree.c \
	compatibility.c \
	log_util.c \
	linkedlist.c \
	memory_util.c \
	mpi_util.c \
	table.c

libstr_util_a_SOURCES = \
	str_util.c \
	dstr_util.c \
	str_match.c

librandom_a_SOURCES = \
	random_util.c

include_HEADERS = \
	SAA_header.h \
	avltree.h \
	compatibility.h \
	log_util.h \
	linkedlist.h \
	memory_util.h \
	mpi_util.h \
	random_util.h \
	table.h

libmethods_a_LIBFLAGS = -lm

librandom_a_LIBFLAGS = -lm

libstr_util_a_LIBFLAGS =

