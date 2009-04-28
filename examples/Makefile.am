## Process this file with automake to produce Makefile.in
#######################################################################
# gaul-devel/examples/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
# Copyright ©2005-2009, Stewart Adcock (http://saa.dyndns.org/)
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

bin_PROGRAMS = \
	polynomial_ga polynomial_moga \
	polynomial_simplex polynomial_sd polynomial_de polynomial_sa \
	fitting fitting_simplex fitting_sd \
	pingpong pingpong9 \
	pingpong_tabu pingpong_tabu2 pingpong_sa \
	wildfire wildfire_forked wildfire_loadbalancing \
	goldberg1 goldberg2 \
	royalroad royalroad_ss royalroad_bitstring \
	royalroad_mutation_prob_demo \
	onemax \
	all5s all5s_allele_ranges \
	struggle struggle_cpp \
	struggle_forked struggle_threaded \
	struggle_mp struggle_mpi \
	struggle_dc struggle_randomsearch \
	struggle_systematicsearch struggle_ss \
	struggle2 struggle3 struggle4 \
	struggle5 struggle5_mp struggle5_mpi struggle5_forked struggle5_threaded \
	saveload \
	nnevolve \
	mixed

polynomial_ga_SOURCES = polynomial_ga.c
polynomial_moga_SOURCES = polynomial_moga.c
polynomial_simplex_SOURCES = polynomial_simplex.c
polynomial_sd_SOURCES = polynomial_sd.c
polynomial_de_SOURCES = polynomial_de.c
polynomial_sa_SOURCES = polynomial_sa.c
fitting_SOURCES = fitting.c
fitting_simplex_SOURCES = fitting_simplex.c
fitting_sd_SOURCES = fitting_sd.c
pingpong_SOURCES = pingpong.c
pingpong9_SOURCES = pingpong9.c
pingpong_tabu_SOURCES = pingpong_tabu.c
pingpong_tabu2_SOURCES = pingpong_tabu2.c
pingpong_sa_SOURCES = pingpong_sa.c
wildfire_SOURCES = wildfire.c
wildfire_forked_SOURCES = wildfire_forked.c
wildfire_loadbalancing_SOURCES = wildfire_loadbalancing.c
goldberg1_SOURCES = goldberg1.c
goldberg2_SOURCES = goldberg2.c
royalroad_SOURCES = royalroad.c
royalroad_bitstring_SOURCES = royalroad_bitstring.c
royalroad_ss_SOURCES = royalroad_ss.c
royalroad_mutation_prob_demo_SOURCES = royalroad_mutation_prob_demo.c
onemax_SOURCES = onemax.c
all5s_SOURCES = all5s.c
all5s_allele_ranges_SOURCES = all5s_allele_ranges.c
struggle_SOURCES = struggle.c
struggle_mp_SOURCES = struggle_mp.c
struggle_mpi_SOURCES = struggle_mpi.c
struggle_dc_SOURCES = struggle_dc.c
struggle_randomsearch_SOURCES = struggle_randomsearch.c
struggle_systematicsearch_SOURCES = struggle_systematicsearch.c
struggle_forked_SOURCES = struggle_forked.c
struggle_threaded_SOURCES = struggle_threaded.c
struggle2_SOURCES = struggle2.c
struggle3_SOURCES = struggle3.c
struggle4_SOURCES = struggle4.c
struggle5_SOURCES = struggle5.c
struggle5_forked_SOURCES = struggle5_forked.c
struggle5_threaded_SOURCES = struggle5_threaded.c
struggle5_mp_SOURCES = struggle5_mp.c
struggle5_mpi_SOURCES = struggle5_mpi.c
struggle_ss_SOURCES = struggle_ss.c
saveload_SOURCES = saveload.c
nnevolve_SOURCES = nnevolve.c
mixed_SOURCES = mixed.c

struggle_cpp_SOURCES = struggle_cpp.cpp

noinst_HEADERS = \
	goldberg1.h goldberg2.h pingpong.h wildfire.h

EXTRA_DIST = \
	chromostubs.c \
	chromostubs.h \
	data/wine.data data/wine.names

polynomial_ga_DEPENDENCIES = ${DEPENDENCIES}
polynomial_moga_DEPENDENCIES = ${DEPENDENCIES}
polynomial_simplex_DEPENDENCIES = ${DEPENDENCIES}
polynomial_sd_DEPENDENCIES = ${DEPENDENCIES}
polynomial_de_DEPENDENCIES = ${DEPENDENCIES}
polynomial_sa_DEPENDENCIES = ${DEPENDENCIES}
fitting_DEPENDENCIES = ${DEPENDENCIES}
fitting_simplex_DEPENDENCIES = ${DEPENDENCIES}
fitting_sd_DEPENDENCIES = ${DEPENDENCIES}
pingpong_DEPENDENCIES = ${DEPENDENCIES}
pingpong9_DEPENDENCIES = ${DEPENDENCIES}
pingpong_tabu_DEPENDENCIES = ${DEPENDENCIES}
pingpong_tabu2_DEPENDENCIES = ${DEPENDENCIES}
pingpong_sa_DEPENDENCIES = ${DEPENDENCIES}
wildfire_DEPENDENCIES = ${DEPENDENCIES}
wildfire_forked_DEPENDENCIES = ${DEPENDENCIES}
wildfire_loadbalancing_DEPENDENCIES = ${DEPENDENCIES}
goldberg1_DEPENDENCIES = ${DEPENDENCIES}
goldberg2_DEPENDENCIES = ${DEPENDENCIES}
royalroad_DEPENDENCIES = ${DEPENDENCIES}
royalroad_bitstring_DEPENDENCIES = ${DEPENDENCIES}
royalroad_ss_DEPENDENCIES = ${DEPENDENCIES}
royalroad_mutation_prob_demo_DEPENDENCIES = ${DEPENDENCIES}
onemax_DEPENDENCIES = ${DEPENDENCIES}
all5s_DEPENDENCIES = ${DEPENDENCIES}
all5s_allele_ranges_DEPENDENCIES = ${DEPENDENCIES}
struggle_DEPENDENCIES = ${DEPENDENCIES}
struggle_mp_DEPENDENCIES = ${DEPENDENCIES}
struggle_dc_DEPENDENCIES = ${DEPENDENCIES}
struggle_randomsearch_DEPENDENCIES = ${DEPENDENCIES}
struggle_systematicsearch_DEPENDENCIES = ${DEPENDENCIES}
struggle_forked_DEPENDENCIES = ${DEPENDENCIES}
struggle_threaded_DEPENDENCIES = ${DEPENDENCIES}
struggle2_DEPENDENCIES = ${DEPENDENCIES}
struggle3_DEPENDENCIES = ${DEPENDENCIES}
struggle4_DEPENDENCIES = ${DEPENDENCIES}
struggle5_DEPENDENCIES = ${DEPENDENCIES}
struggle5_forked_DEPENDENCIES = ${DEPENDENCIES}
struggle5_threaded_DEPENDENCIES = ${DEPENDENCIES}
struggle5_mp_DEPENDENCIES = ${DEPENDENCIES}
struggle5_mpi_DEPENDENCIES = ${DEPENDENCIES}
struggle_ss_DEPENDENCIES = ${DEPENDENCIES}
saveload_DEPENDENCIES = ${DEPENDENCIES}
nnevolve_DEPENDENCIES = ${DEPENDENCIES}
mixed_DEPENDENCIES = ${DEPENDENCIES}

struggle_cpp_DEPENDENCIES = ${DEPENDENCIES}

# Serial code:
polynomial_ga_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
polynomial_moga_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
polynomial_simplex_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
polynomial_sd_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
polynomial_de_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
polynomial_sa_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
fitting_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
fitting_simplex_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
fitting_sd_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
pingpong_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
pingpong9_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
pingpong_tabu_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
pingpong_tabu2_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
pingpong_sa_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
wildfire_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
wildfire_forked_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
wildfire_loadbalancing_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
goldberg1_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
goldberg2_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
royalroad_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
royalroad_bitstring_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
royalroad_ss_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
royalroad_mutation_prob_demo_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
onemax_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
all5s_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
all5s_allele_ranges_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_dc_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_randomsearch_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_systematicsearch_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_forked_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_threaded_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle2_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle3_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle4_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_forked_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_threaded_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_ss_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
saveload_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
nnevolve_LDADD = -lgaul -lnn_util -lgaul_util -lm @MPILIBS@
mixed_LDADD = -lgaul -lnn_util -lgaul_util -lm @MPILIBS@

struggle_cpp_LDADD = -lgaul -lgaul_util -lm @MPILIBS@

# Parallel code:
struggle_mp_CFLAGS = -lgaul -lgaul_util -lm @MPILIBS@
struggle_mp_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle_mpi_CFLAGS = -lgaul -lgaul_util -lm @MPILIBS@
struggle_mpi_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_mp_CFLAGS = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_mp_LDADD = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_mpi_CFLAGS = -lgaul -lgaul_util -lm @MPILIBS@
struggle5_mpi_LDADD = -lgaul -lgaul_util -lm @MPILIBS@

