## Process this file with automake to produce Makefile.in
#######################################################################
# gaul/tests/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
# Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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
INCLUDES = -I../util/ -I../src/ -I/usr/include/slang/

bin_PROGRAMS = diagnostics

noinst_PROGRAMS = test_prng test_utils test_bitstrings test_slang

diagnostics_LDADD = -L../src/ -L../util/ -lgaul -lgaul_util -lm
test_prng_LDADD = -L../src/ -L../util/ -lgaul -lgaul_util -lm
test_utils_LDADD = -L../src/ -L../util/ -lgaul -lgaul_util -lm
test_bitstrings_LDADD = -L../src/ -L../util/ -lgaul -lgaul_util -lm
test_slang_LDADD = -L../src/ -L../util/ -lgaul -lgaul_util -lm

