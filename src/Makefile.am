## Process this file with automake to produce Makefile.in
#######################################################################
# gaul/src/Makefile.am
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
# Last Updated:	17/05/01 SAA	Split entirely from HelGA for new life on sourceforge.
#		02/02/01 SAA	Seperated from the main helga directory.
#

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

lib_LIBRARIES = libgaul.a

# CFLAGS = -g -Wall -O

INCLUDES = -I../util/

libgaul_a_SOURCES = \
    ga_chromo.c \
    ga_core.c \
    ga_crossover.c \
    ga_intrinsics.c \
    ga_mutate.c \
    ga_optim.c \
    ga_qsort.c \
    ga_replace.c \
    ga_seed.c \
    ga_select.c \
    ga_similarity.c \
    ga_utility.c

include_HEADERS = \
    ga_chromo.h \
    ga_core.h \
    ga_intrinsics.h \
    ga_optim.h \
    ga_qsort.h \
    ga_similarity.h \
    ga_utility.h \
    gaul.h

libgaul_a_DEPENDENCIES = \
	../util/libmethods.a \
	../util/librandom.a \
	../util/libstr_util.a

libgaul_a_LIBFLAGS = -lm

