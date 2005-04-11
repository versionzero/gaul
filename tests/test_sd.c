/**********************************************************************
  test_sd.c
 **********************************************************************

  test_sd - Test/example program for GAUL.
  Copyright ©2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test/example program for GAUL demonstrating use
		of the steepest ascent algorithm.

		This program aims to solve a function of the form
		(0.75-A)+(0.95-B)^2+(0.23-C)^3+(0.71-D)^4 = 0

  Last Updated:	25 Nov 2002 SAA	Based on examples/test_simplex.c

 **********************************************************************/

#include "gaul.h"

/**********************************************************************
  test_to_double()
  synopsis:     Convert to double array.
  parameters:
  return:
  last updated: 25 Nov 2002
 **********************************************************************/

boolean test_to_double(population *pop, entity *entity, double *array)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  array[0] = ((double *)entity->chromosome[0])[0];
  array[1] = ((double *)entity->chromosome[0])[1];
  array[2] = ((double *)entity->chromosome[0])[2];
  array[3] = ((double *)entity->chromosome[0])[3];

  return TRUE;
  }


/**********************************************************************
  test_from_double()
  synopsis:     Convert from double array.
  parameters:
  return:
  last updated: 25 Nov 2002
 **********************************************************************/

boolean test_from_double(population *pop, entity *entity, double *array)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  if (!entity->chromosome) die("Entity has no chromsomes.");

  ((double *)entity->chromosome[0])[0] = array[0];
  ((double *)entity->chromosome[0])[1] = array[1];
  ((double *)entity->chromosome[0])[2] = array[2];
  ((double *)entity->chromosome[0])[3] = array[3];

  return TRUE;
  }


/**********************************************************************
  test_score()
  synopsis:	Fitness function.
  parameters:
  return:
  updated:	25 Nov 2002
 **********************************************************************/

boolean test_score(population *pop, entity *entity)
  {
  double		A, B, C, D;	/* Parameters. */

  A = ((double *)entity->chromosome[0])[0];
  B = ((double *)entity->chromosome[0])[1];
  C = ((double *)entity->chromosome[0])[2];
  D = ((double *)entity->chromosome[0])[3];

  entity->fitness = -(fabs(0.75-A)+SQU(0.95-B)+fabs(CUBE(0.23-C))+FOURTH_POW(0.71-D));

  return TRUE;
  }


/**********************************************************************
  test_analytical_gradient()
  synopsis:     Calculate gradients analytically.
  parameters:
  return:
  last updated: 25 Nov 2002
 **********************************************************************/

double test_analytical_gradient(population *pop, entity *entity, double *params, double *grad)
  {
  double		grms=0.0;	/* RMS gradient. */
  double		A, B, C, D;	/* The parameters. */

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  A = params[0];
  B = params[1];
  C = params[2];
  D = params[3];

  grad[0] = A > 0.75+TINY ? -1.0 : ( A < 0.75-TINY ? 1.0 : 0.0 );
  grad[1] = (0.95 - B);
  grad[2] = C > 0.23 ? -SQU(0.23 - C) : SQU(0.23 - C);
  grad[3] = CUBE(0.71 - D);

  grms = sqrt(grad[0]*grad[0]+grad[1]*grad[1]+grad[2]*grad[2]+grad[3]*grad[3]);

  return grms;
  }


/**********************************************************************
  test_iteration_callback()
  synopsis:	Generation callback
  parameters:
  return:
  updated:	25 Nov 2002
 **********************************************************************/

boolean test_iteration_callback(int iteration, entity *solution)
  {

  printf( "%d: A = %f B = %f C = %f D = %f (fitness = %f)\n",
            iteration,
            ((double *)solution->chromosome[0])[0],
            ((double *)solution->chromosome[0])[1],
            ((double *)solution->chromosome[0])[2],
            ((double *)solution->chromosome[0])[3],
            solution->fitness );

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

boolean test_seed(population *pop, entity *adam)
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
  updated:	25 Nov 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population		*pop;			/* Population of solutions. */
  entity		*solution;		/* Optimised solution. */

  random_seed(23091975);

  pop = ga_genesis_double(
       50,			/* const int              population_size */
       1,			/* const int              num_chromo */
       4,			/* const int              len_chromo */
       NULL,			/* GAgeneration_hook      generation_hook */
       test_iteration_callback,	/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       test_score,		/* GAevaluate             evaluate */
       test_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       NULL,			/* GAselect_one           select_one */
       NULL,			/* GAselect_two           select_two */
       NULL,			/* GAmutate               mutate */
       NULL,			/* GAcrossover            crossover */
       NULL,			/* GAreplace              replace */
       NULL			/* vpointer	User data */
            );

  ga_population_set_gradient_parameters(
       pop,				/* population		*pop */
       test_to_double,		/* const GAto_double	to_double */
       test_from_double,		/* const GAfrom_double	from_double */
       test_analytical_gradient,	/* const GAgradient	gradient */
       4,				/* const int		num_dimensions */
       0.1				/* const double		step_size */
                       );

  /* Evaluate and sort the initial population members (i.e. select best of 50 random solutions. */
  ga_population_score_and_sort(pop);

  /* Use the best population member. */
  solution = ga_get_entity_from_rank(pop, 0);

  ga_steepestascent(
       pop,				/* population		*pop */
       solution,			/* entity		*solution */
       1000				/* const int		max_iterations */
            );

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }

