## Process this file with automake to produce Makefile.in
#######################################################################
# gaul/src/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
# Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
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
#######################################################################

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

# Kludge:
INCLUDES = -I../util/ -I/usr/include/slang/

lib_LTLIBRARIES = libgaul.la

libgaul_la_DEPENDENCIES = gaul.h

libgaul_la_SOURCES = \
    ga_bitstring.c \
    ga_chromo.c \
    ga_climbing.c \
    ga_compare.c \
    ga_core.c \
    ga_crossover.c \
    ga_deterministiccrowding.c \
    ga_intrinsics.c \
    ga_io.c \
    ga_gradient.c \
    ga_mutate.c \
    ga_optim.c \
    ga_qsort.c \
    ga_replace.c \
    ga_randomsearch.c \
    ga_seed.c \
    ga_select.c \
    ga_sa.c \
    ga_similarity.c \
    ga_simplex.c \
    ga_stats.c \
    ga_systematicsearch.c \
    ga_tabu.c \
    ga_utility.c

nobase_include_HEADERS = \
    gaul/ga_bitstring.h \
    gaul/ga_chromo.h \
    gaul/ga_climbing.h \
    gaul/ga_core.h \
    gaul/ga_deterministiccrowding.h \
    gaul/ga_intrinsics.h \
    gaul/ga_gradient.h \
    gaul/ga_optim.h \
    gaul/ga_qsort.h \
    gaul/ga_randomsearch.h \
    gaul/ga_sa.h \
    gaul/ga_similarity.h \
    gaul/ga_simplex.h \
    gaul/ga_systematicsearch.h \
    gaul/ga_tabu.h \
    gaul/ga_utility.h \
    gaul.h

libgaul_la_LIBFLAGS = -lm
libgaul_la_LDFLAGS = -version-info ${GA_MAJOR_VERSION}:${GA_MINOR_VERSION}:0


