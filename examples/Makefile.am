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

#EXTRA_PROGRAMS = 

bin_PROGRAMS = diagnostics \
	pingpong pingpong_tabu pingpong_tabu2 pingpong_sa \
	wildfire wildfire_forked wildfire_loadbalancing \
	goldberg1 goldberg2 \
	royalroad royalroad_ss \
	struggle struggle_randomsearch struggle_forked \
        struggle2 struggle3 struggle4 struggle5 struggle5_mp struggle5_forked struggle_ss \
	saveload \
	nnevolve

diagnostics_SOURCES = diagnostics.c
pingpong_SOURCES = pingpong.c
pingpong_tabu_SOURCES = pingpong_tabu.c
pingpong_tabu2_SOURCES = pingpong_tabu2.c
pingpong_sa_SOURCES = pingpong_sa.c
wildfire_SOURCES = wildfire.c
wildfire_forked_SOURCES = wildfire_forked.c
wildfire_loadbalancing_SOURCES = wildfire_loadbalancing.c
goldberg1_SOURCES = goldberg1.c
goldberg2_SOURCES = goldberg2.c
royalroad_SOURCES = royalroad.c
royalroad_ss_SOURCES = royalroad_ss.c
struggle_SOURCES = struggle.c
struggle_randomsearch_SOURCES = struggle_randomsearch.c
struggle_forked_SOURCES = struggle_forked.c
struggle2_SOURCES = struggle2.c
struggle3_SOURCES = struggle3.c
struggle4_SOURCES = struggle4.c
struggle5_SOURCES = struggle5.c
struggle5_forked_SOURCES = struggle5_forked.c
struggle5_mp_SOURCES = struggle5_mp.c
struggle_ss_SOURCES = struggle_ss.c
saveload_SOURCES = saveload.c
nnevolve_SOURCES = nnevolve.c

noinst_LTLIBRARIES = libbtree.la

libbtree_la_SOURCES = btree.c

# Kludge to force compilation of the binaries.
libbtree_la_DEPENDENCIES = ${EXTRA_PROGRAMS}

noinst_HEADERS = \
	btree.h goldberg1.h goldberg2.h pingpong.h wildfire.h

EXTRA_DIST = \
	chromostubs.c \
	chromostubs.h \
	wine.data wine.names

diagnostics_DEPENDENCIES = ${DEPENDENCIES}
pingpong_DEPENDENCIES = ${DEPENDENCIES}
pingpong_tabu_DEPENDENCIES = ${DEPENDENCIES}
pingpong_tabu2_DEPENDENCIES = ${DEPENDENCIES}
pingpong_sa_DEPENDENCIES = ${DEPENDENCIES}
wildfire_DEPENDENCIES = ${DEPENDENCIES}
wildfire_forked_DEPENDENCIES = ${DEPENDENCIES}
wildfire_loadbalancing_DEPENDENCIES = ${DEPENDENCIES}
goldberg1_DEPENDENCIES = ${DEPENDENCIES}
goldberg2_DEPENDENCIES = ${DEPENDENCIES}
royalroad_DEPENDENCIES = ${DEPENDENCIES}
royalroad_ss_DEPENDENCIES = ${DEPENDENCIES}
struggle_DEPENDENCIES = ${DEPENDENCIES}
struggle_randomsearch_DEPENDENCIES = ${DEPENDENCIES}
struggle_forked_DEPENDENCIES = ${DEPENDENCIES}
struggle2_DEPENDENCIES = ${DEPENDENCIES}
struggle3_DEPENDENCIES = ${DEPENDENCIES}
struggle4_DEPENDENCIES = ${DEPENDENCIES}
struggle5_DEPENDENCIES = ${DEPENDENCIES}
struggle5_forked_DEPENDENCIES = ${DEPENDENCIES}
struggle5_mp_DEPENDENCIES = ${DEPENDENCIES}
struggle_ss_DEPENDENCIES = ${DEPENDENCIES}
saveload_DEPENDENCIES = ${DEPENDENCIES}
nnevolve_DEPENDENCIES = ${DEPENDENCIES}

# Serial code:
diagnostics_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
pingpong_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
pingpong_tabu_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
pingpong_tabu2_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
pingpong_sa_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
wildfire_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
wildfire_forked_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
wildfire_loadbalancing_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
goldberg1_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm 
goldberg2_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
royalroad_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
royalroad_ss_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle_randomsearch_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle_forked_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle2_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle3_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle4_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle5_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle5_forked_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
struggle_ss_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm
saveload_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lstuff -lm 
nnevolve_LDADD = -L../src/ -L../util/ -lgaul -llog_util -lmpi_util_fake -lnn_util -lstuff -lm 

# Parallel code:
struggle5_mp_LDADD = -L../src/ -L../util/ -lgaul -llog_util_parallel -lmpi_util -lstuff -lm
struggle5_mp_LINK = @MPICC@ $(struggle5_mp_LDADD) -o struggle5_mp @MPIFLAGS@

