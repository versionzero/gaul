## Process this file with automake to produce Makefile.in
#######################################################################
# gaul/tests/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
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
# Last Updated:	11/05/01 SAA	Added wildfire stuff.
#		23/04/01 SAA	From ga/Makefile.am
#

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = pingpong wildfire

CFLAGS = -g -Wall -O

INCLUDES = -I../util/ -I../src/ -I /usr/include/slang/

pingpong_SOURCES = pingpong.c
wildfire_SOURCES = wildfire.c

include_HEADERS = pingpong.h wildfire.h

pingpong_DEPENDENCIES = \
	../util/libmethods.a \
	../util/librandom.a \
	../util/libstr_util.a

wildfire_DEPENDENCIES = \
	../util/libmethods.a \
	../util/librandom.a \
	../util/libstr_util.a

pingpong_LDADD = -L../src/ -L../util/ -lm -lga -lmethods -lrandom -lstr_util
wildfire_LDADD = -L../src/ -L../util/ -lm -lga -lmethods -lrandom -lstr_util

