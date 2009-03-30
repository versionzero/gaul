/**********************************************************************
  test_moga.c
 **********************************************************************

  test_moga - Test program for GAUL.
  Copyright ©2002-2006, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test program for GAUL's multiobjective genetic
		algorithm.

		This program aims to solve a function of the form
		(0.75-A)+(0.95-B)^2+(0.23-C)^3+(0.71-D)^4 = 0

		The values of the individual terms, A, B, C and D, are
		the multiobjective terms.

 **********************************************************************/

#include "gaul.h"


/**********************************************************************
  test_score()
  synopsis:	Fitness function.
  parameters:
  return:
  updated:	05 Aug 2005
 **********************************************************************/

static boolean test_score(population *pop, entity *this_entity)
  {
  double		A, B, C, D;	/* Parameters. */

  A = ((double *)this_entity->chromosome[0])[0];
  B = ((double *)this_entity->chromosome[0])[1];
  C = ((double *)this_entity->chromosome[0])[2];
  D = ((double *)this_entity->chromosome[0])[3];

  this_entity->fitvector[0] = fabs(0.75-A);
  this_entity->fitvector[1] = fabs(0.95-B);
  this_entity->fitvector[2] = fabs(0.23-C);
  this_entity->fitvector[3] = fabs(0.71-D);

  this_entity->fitness = -(fabs(0.75-A)+SQU(0.95-B)+fabs(CUBE(0.23-C))+FOURTH_POW(0.71-D));

  return TRUE;
  }


/**********************************************************************
  test_generation_callback()
  synopsis:	Generation callback
  parameters:
  return:
  last updated: 05 Aug 2005
 **********************************************************************/

static boolean test_generation_callback(int generation, population *pop)
  {
  entity	*this_entity = ga_get_entity_from_rank(pop, 0);	/* The best entity. */

  printf( "%d: A = %f B = %f C = %f D = %f (fitness = %f) pop_size %d\n",
            generation,
            ((double *)this_entity->chromosome[0])[0],
            ((double *)this_entity->chromosome[0])[1],
            ((double *)this_entity->chromosome[0])[2],
            ((double *)this_entity->chromosome[0])[3],
            ga_entity_get_fitness(this_entity),
            pop->size );

  return TRUE;
  }


/**********************************************************************
  test_seed()
  synopsis:	Seed genetic data.
  parameters:	population *pop
		entity *adam
  return:	success
  last updated: 25 Nov 2002
 **********************************************************************/

static boolean test_seed(population *pop, entity *adam)
  {

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  ((double *)adam->chromosome[0])[0] = random_double(2.0);
  ((double *)adam->chromosome[0])[1] = random_double(2.0);
  ((double *)adam->chromosome[0])[2] = random_double(2.0);
  ((double *)adam->chromosome[0])[3] = random_double(2.0);

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Main function.
  parameters:
  return:
  updated:	05 Aug 2005
 **********************************************************************/

int main(int argc, char **argv)
  {
  population		*pop;			/* Population of solutions. */

  random_seed(23091975);

/* "Best Set" Multiobjective GA. */
  printf("Using the Best Set Multiobjective GA varient.\n");

  pop = ga_genesis_double(
       100,			/* const int              population_size */
       1,			/* const int              num_chromo */
       4,			/* const int              len_chromo */
       test_generation_callback,/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       test_score,		/* GAevaluate             evaluate */
       test_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_bestof2,	/* GAselect_one           select_one */
       ga_select_two_bestof2,	/* GAselect_two           select_two */
       ga_mutate_double_singlepoint_drift,	/* GAmutate               mutate */
       ga_crossover_double_doublepoints,	/* GAcrossover            crossover */
       NULL,			/* GAreplace              replace */
       NULL			/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,				/* population      *pop */
       GA_SCHEME_DARWIN,		/* const ga_scheme_type     scheme */
       GA_ELITISM_BEST_SET_SURVIVE,	/* const ga_elitism_type   elitism */
       0.8,				/* double  crossover */
       0.2,				/* double  mutation */
       0.0      		        /* double  migration */
                              );

  ga_population_set_fitness_dimensions(pop, 4);

  ga_evolution(
       pop,				/* population	*pop */
       200				/* const int	max_generations */
              );

  ga_extinction(pop);

/* "Pareto Set" Multiobjective GA. */
  printf("Using the Pareto Set Multiobjective GA varient.\n");

  pop = ga_genesis_double(
       100,			/* const int              population_size */
       1,			/* const int              num_chromo */
       4,			/* const int              len_chromo */
       test_generation_callback,/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       test_score,		/* GAevaluate             evaluate */
       test_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_bestof2,	/* GAselect_one           select_one */
       ga_select_two_bestof2,	/* GAselect_two           select_two */
       ga_mutate_double_singlepoint_drift,	/* GAmutate               mutate */
       ga_crossover_double_doublepoints,	/* GAcrossover            crossover */
       NULL,			/* GAreplace              replace */
       NULL			/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,				/* population      *pop */
       GA_SCHEME_DARWIN,		/* const ga_scheme_type     scheme */
       GA_ELITISM_PARETO_SET_SURVIVE,	/* const ga_elitism_type   elitism */
       0.8,				/* double  crossover */
       0.2,				/* double  mutation */
       0.0      		        /* double  migration */
                              );

  ga_population_set_fitness_dimensions(pop, 4);

  ga_evolution(
       pop,				/* population	*pop */
       200				/* const int	max_generations */
              );

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }

