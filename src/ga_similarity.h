/**********************************************************************
  ga_similarity.h
 **********************************************************************

  ga_similarity - Genetic algorithm genome comparison routines.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatible with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:     Routines for comparing genomes/chromosomes.

  Updated:      13/02/01 SAA	First version.

 **********************************************************************/

#ifndef GA_SIMILARITY_H_INCLUDED
#define GA_SIMILARITY_H_INCLUDED

/*
 * Includes
 */
#include "SAA_header.h"

#include "methods/memory_util.h"	/* Memory handling. */
#include "methods/log_util.h"		/* For logging facilities. */
#include "methods/mpi_util.h"		/* For multiprocessing facilities. */

#include "ga_util.h"			/* For PRNGs. */

/*
 * Prototypes
 */
boolean	ga_compare_genome(population *pop, entity *alpha, entity *beta);
double	ga_genome_euclidean_similarity(population *pop, entity *alpha, entity *beta);
double	ga_genome_hamming_similarity(population *pop, entity *alpha, entity *beta);

#endif	/* GA_SIMILARITY_H_INCLUDED */
