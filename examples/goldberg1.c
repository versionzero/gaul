/**********************************************************************
  goldberg1.c
 **********************************************************************

  goldberg1 - Test/example program for GAUL.
  Copyright ©2001-2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test/example program for GAUL.

		This program aims to solve the first example problem
		from Goldberg's book.

		Here a 10-bit chromosome is used to find the maximum
		of function f(x)=x^10, normalized to the range [0,1].

		FIXME: This code is derived from the description
		of the problem in Gallops, I need to acquire the
		goldberg book and check this.

 **********************************************************************/

#include "goldberg1.h"

/**********************************************************************
  goldberg1_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

boolean goldberg1_score(population *pop, entity *entity)
  {
  boolean	allele;
  double	coef;
  int		k;

  entity->fitness = 0.0;
  coef = pow(2.0, (double) pop->len_chromosomes) - 1.0;
  coef = pow(coef, 10.0);

  /* Loop over alleles in chromosome. */
  for (k = 0; k < pop->len_chromosomes; k++)
    {
    /* Loop over bits in current byte. */
    allele = ((boolean *)entity->chromosome[0])[k];
    if (allele == 1)
      {	/* Bit is set. */
      entity->fitness += pow(2.0, (double) k);
      }
    }

  /* Raise x to the power of 10. */
  entity->fitness = pow(entity->fitness, 10.0);

  /* Normalize fitness. */
  entity->fitness /= coef;
  
  return TRUE;
  }


/**********************************************************************
  goldberg1_ga_callback()
  synopsis:	Callback.
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

boolean goldberg1_ga_callback(int generation, population *pop)
  {

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	21 Aug 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;			/* Runs. */
  population	*pop=NULL;		/* Population of solutions. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */

  random_init();

  for (i=0; i<50; i++)
    {
    if (pop) ga_extinction(pop);

    random_seed(i);

    pop = ga_genesis_boolean(
       20,			/* const int              population_size */
       1,			/* const int              num_chromo */
       10,			/* const int              len_chromo */
NULL, /*goldberg1_ga_callback,*/	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       goldberg1_score,		/* GAevaluate             evaluate */
       ga_seed_boolean_random,	/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_bestof2,	/* GAselect_one           select_one */
       ga_select_two_bestof2,	/* GAselect_two           select_two */
       ga_mutate_boolean_singlepoint,	/* GAmutate               mutate */
       ga_crossover_boolean_singlepoints,	/* GAcrossover            crossover */
       NULL,			/* GAreplace		replace */
       NULL			/* vpointer		User data */
            );

    ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
       0.5,			/* double  crossover */
       0.05,			/* double  mutation */
       0.0              	/* double  migration */
                              );

    ga_evolution(
       pop,		/* population              *pop */
       50		/* const int               max_generations */
              );

    goldberg1_ga_callback(i, pop);

    printf("The final solution with seed = %d was:\n", i);
    beststring = ga_chromosome_boolean_to_string(pop, ga_get_entity_from_rank(pop,0), beststring, &beststrlen);
    printf("%s\n", beststring);
    printf("With score = %f\n", ga_get_entity_from_rank(pop,0)->fitness);
    }

  ga_extinction(pop);

  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


