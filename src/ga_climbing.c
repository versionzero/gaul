/**********************************************************************
  ga_climbing.c
 **********************************************************************

  ga_climbing - Hill climbing algorithms for comparison and search.
  Copyright ©2002-2003, Stewart Adcock <stewart@linux-domain.com>
  All rights reserved.

  The latest version of this program should be available at:
  http://gaul.sourceforge.net/

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

  Synopsis:     Hill climbing algorithms for comparison and local
		search.  The available functions are
		ga_next_ascent_hillclimbing(), which modifies each
		allele in turn until the termination criteria are
		achieve, and ga_random_ascent_hillclimbing(), which
		modifies randomly selected alleles.

 **********************************************************************/

#include "gaul/ga_climbing.h"

/**********************************************************************
  ga_population_set_hillclimbing_parameters()
  synopsis:     Sets the hill climbing parameters for a population.
  parameters:
  return:
  last updated: 17 Oct 2002
 **********************************************************************/

void ga_population_set_hillclimbing_parameters( population              *pop,
                                      GAmutate_allele		mutate_allele)
  {

  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !mutate_allele ) die("Null pointer to GAmutate_allele callback passed.");

  plog( LOG_VERBOSE,
        "Population's hill-climbing parameters: " );

  if (pop->climbing_params == NULL)
    pop->climbing_params = s_malloc(sizeof(ga_climbing_t));

  pop->climbing_params->mutate_allele = mutate_allele;

  return;
  }


/**********************************************************************
  ga_random_ascent_hillclimbing()
  synopsis:	Performs optimisation on the passed entity by using the
  		random ascent hill climbing protocol.  The fitness
		evaluations are performed using the standard evaluation
		callback mechanism.
		The passed entity will have it's data overwritten.  The
		remainder of the population will be let untouched.
		Note that it is safe to pass a NULL initial structure,
		in which case a random starting structure wil be
		generated, however the final solution will not be
		available to the caller in any obvious way.
  parameters:
  return:
  last updated:	17 Oct 2002
 **********************************************************************/


int ga_random_ascent_hillclimbing(	population		*pop,
					entity			*best,
					const int		max_iterations )
  {
  int		iteration=0;		/* Current iteration number. */
  entity	*putative;		/* Current solution. */
  entity	*tmp;			/* Used to swap working solutions. */
  int		chromo_id;		/* Chromosome number. */
  int		allele_id;		/* Allele number. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->climbing_params)
    die("ga_population_set_hillclimbing_params(), or similar, must be used prior to ga_random_ascent_hillclimbing().");
  if (!pop->climbing_params->mutate_allele)
    die("Population's allele mutation callback is undefined.");

/* Prepare working entity. */
  putative = ga_get_free_entity(pop);

/* Do we need to generate a random starting solution? */
  if (!best)
    {
    plog(LOG_VERBOSE, "Will perform hill climbing with random starting solution.");

    best = ga_get_free_entity(pop);
    ga_entity_seed(pop, best);
    }
  else
    {   
    plog(LOG_VERBOSE, "Will perform hill climbing with specified starting solution.");
    }

/*
 * Ensure that initial solution is scored.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, the current solution has fitness score of %f",
        best->fitness );

/*
 * Do all the iterations:
 *
 * Stop when (a) max_iterations reached, or
 *           (b) "pop->iteration_hook" returns FALSE.
 */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, best):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

/*
 * Generate and score a new solution.
 */
    chromo_id = random_int(pop->num_chromosomes);
    allele_id = random_int(pop->len_chromosomes);
    pop->climbing_params->mutate_allele(pop, best, putative, chromo_id, allele_id);
    pop->mutate(pop, best, putative);
    pop->evaluate(pop, putative);

/*
 * Decide whether this new solution should be selected or discarded based
 * on the relative fitnesses.
 */
  if ( putative->fitness > best->fitness )
    {
    tmp = best;
    best = putative;
    putative = tmp;
    }

/*
 * Use the iteration callback.
 */
    plog( LOG_VERBOSE,
          "After iteration %d, the current solution has fitness score of %f",
          iteration,
          best->fitness );

    }	/* Iteration loop. */

/*
 * Cleanup.
 */
  ga_entity_dereference(pop, putative);

  return iteration;
  }


/**********************************************************************
  ga_next_ascent_hillclimbing()
  synopsis:	Performs optimisation on the passed entity by using the
  		next ascent hill climbing protocol.  The fitness
		evaluations are performed using the standard evaluation
		callback mechanism.
		The passed entity will have it's data overwritten.  The
		remainder of the population will be let untouched.
		Note that it is safe to pass a NULL initial structure,
		in which case a random starting structure wil be
		generated, however the final solution will not be
		available to the caller in any obvious way.
  parameters:
  return:
  last updated:	17 Oct 2002
 **********************************************************************/


int ga_next_ascent_hillclimbing(	population		*pop,
					entity			*best,
					const int		max_iterations )
  {
  int		iteration=0;		/* Current iteration number. */
  entity	*putative;		/* Current solution. */
  entity	*tmp;			/* Used to swap working solutions. */
  int		chromo_id=0;		/* Chromosome number. */
  int		allele_id=0;		/* Allele number. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->climbing_params)
    die("ga_population_set_hillclimbing_params(), or similar, must be used prior to ga_next_ascent_hillclimbing().");
  if (!pop->climbing_params->mutate_allele)
    die("Population's allele mutation callback is undefined.");

/* Prepare working entity. */
  putative = ga_get_free_entity(pop);

/* Do we need to generate a random starting solution? */
  if (!best)
    {
    plog(LOG_VERBOSE, "Will perform hill climbing with random starting solution.");

    best = ga_get_free_entity(pop);
    ga_entity_seed(pop, best);
    }
  else
    {   
    plog(LOG_VERBOSE, "Will perform hill climbing with specified starting solution.");
    }

/*
 * Ensure that initial solution is scored.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, the current solution has fitness score of %f",
        best->fitness );

/*
 * Do all the iterations:
 *
 * Stop when (a) max_iterations reached, or
 *           (b) "pop->iteration_hook" returns FALSE.
 */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, best):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

    allele_id++;
    if (allele_id >= pop->len_chromosomes)
      {
      allele_id = 0;
      chromo_id++;
      if (chromo_id >= pop->num_chromosomes)
        chromo_id = 0;
      }

/*
 * Generate and score a new solution.
 */
    pop->climbing_params->mutate_allele(pop, best, putative, chromo_id, allele_id);
    pop->evaluate(pop, putative);

/*
 * Decide whether this new solution should be selected or discarded based
 * on the relative fitnesses.
 */
  if ( putative->fitness > best->fitness )
    {
    tmp = best;
    best = putative;
    putative = tmp;
    }

/*
 * Use the iteration callback.
 */
    plog( LOG_VERBOSE,
          "After iteration %d, the current solution has fitness score of %f",
          iteration,
          best->fitness );

    }	/* Iteration loop. */

/*
 * Cleanup.
 */
  ga_entity_dereference(pop, putative);

  return iteration;
  }


