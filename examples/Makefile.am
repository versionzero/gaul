## Process this file with automake to produce Makefile.in
#######################################################################
# gaul/examples/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
# Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
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

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = diagnostics \
	pingpong wildfire \
	goldberg1 goldberg2 \
	royalroad royalroad_ss \
	struggle struggle2 struggle3 struggle4 struggle5 struggle5_mp struggle_ss \
	nnevolve

diagnostics_SOURCES = diagnostics.c
pingpong_SOURCES = pingpong.c
wildfire_SOURCES = wildfire.c
goldberg1_SOURCES = goldberg1.c
goldberg2_SOURCES = goldberg2.c
royalroad_SOURCES = royalroad.c
royalroad_ss_SOURCES = royalroad_ss.c
struggle_SOURCES = struggle.c
struggle2_SOURCES = struggle2.c
struggle3_SOURCES = struggle3.c
struggle4_SOURCES = struggle4.c
struggle5_SOURCES = struggle5.c
struggle5_mp_SOURCES = struggle5_mp.c
struggle_ss_SOURCES = struggle_ss.c
nnevolve_SOURCES = nnevolve.c

lib_LIBRARIES = libbtree.a

libbtree_a_SOURCES = btree.c

noinst_HEADERS = \
	btree.h goldberg1.h goldberg2.h pingpong.h wildfire.h

DEPENDENCIES = \
	../util/librandom.a \
	../util/libstuff.a \
	../util/libmpi_util.a \
	../util/libnn_util.a \
	../util/libstr_util.a \
	../util/libtimer_util.a \
	../src/libgaul.la

EXTRA_DIST = \
	chromostubs.c \
	chromostubs.h \
	wine.data wine.names

diagnostics_DEPENDENCIES = ${DEPENDENCIES}
pingpong_DEPENDENCIES = ${DEPENDENCIES}
wildfire_DEPENDENCIES = ${DEPENDENCIES}
goldberg1_DEPENDENCIES = ${DEPENDENCIES}
goldberg2_DEPENDENCIES = ${DEPENDENCIES}
royalroad_DEPENDENCIES = ${DEPENDENCIES}
royalroad_ss_DEPENDENCIES = ${DEPENDENCIES}
struggle_DEPENDENCIES = ${DEPENDENCIES}
struggle2_DEPENDENCIES = ${DEPENDENCIES}
struggle3_DEPENDENCIES = ${DEPENDENCIES}
struggle4_DEPENDENCIES = ${DEPENDENCIES}
struggle5_DEPENDENCIES = ${DEPENDENCIES}
struggle5_mp_DEPENDENCIES = ${DEPENDENCIES}
struggle_ss_DEPENDENCIES = ${DEPENDENCIES}
nnevolve_DEPENDENCIES = ${DEPENDENCIES}

diagnostics_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
pingpong_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
wildfire_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
goldberg1_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm 
goldberg2_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
royalroad_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
royalroad_ss_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle2_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle3_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle4_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle5_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
struggle5_mp_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm @MPIFLAGS@
struggle_ss_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lrandom -lstr_util -lm
nnevolve_LDADD = -L../src/ -L../util/ -lgaul -lstuff -lmpi_util -lnn_util -lrandom -lstr_util -ltimer_util -lm 

