/**********************************************************************
  g_dejong.c
 **********************************************************************

  dejong - Test/example program for GAUL.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
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

		This program runs the De Jong test suite.

 **********************************************************************/

#include "goldberg1.h"

/**********************************************************************
  dejong_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

boolean dejong_score(population *pop, entity *entity)
  {
  unsigned int	allele;
  double	coef;
  int		k;

  entity->fitness = 0.0;
  coef = pow(2.0, (double) pop->len_chromosomes) - 1.0;
  coef = pow(coef, 30.0);

  /* Loop over alleles in chromosome. */
  for (k = 0; k < pop->len_chromosomes; k++)
    {
    /* Loop over bits in current byte. */
    allele = entity->chromosome[0][k];
    if (allele == 1)
      {	/* Bit is set. */
      entity->fitness += pow(2.0, (double) k);
      }
    }

  /* Raise x to the power of 30. */
  entity->fitness = pow(entity->fitness, 30.0);

  /* Normalize fitness. */
  entity->fitness /= coef;
  
  return TRUE;
  }


/**********************************************************************
  dejong_ga_callback()
  synopsis:	Callback.
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

boolean dejong_ga_callback(int generation, population *pop)
  {

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;		/* Runs. */
  int		j;		/* Loop over alleles. */
  population	*pop=NULL;	/* Population of solutions. */

  random_init();

  for (i=0; i<50; i++)
    {
    if (pop) ga_extinction(pop);

    random_seed(i);

    pop = ga_genesis_boolean(
       50,			/* const int              population_size */
       1,			/* const int              num_chromo */
       10,			/* const int              len_chromo */
       NULL, 			/* const char             *fname */
NULL, /*dejong_ga_callback,*/	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       dejong_score,		/* GAevaluate             evaluate */
       ga_seed_boolean_random,	/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_bestof2,	/* GAselect_one           select_one */
       ga_select_two_bestof2,	/* GAselect_two           select_two */
       ga_mutate_boolean_singlepoint,	/* GAmutate               mutate */
       ga_crossover_chromosome_singlepoints,	/* GAcrossover            crossover */
       NULL			/* GAreplace  replace */
            );

    ga_population_set_parameters(
       pop,		/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
       0.3,		/* double  crossover */
       0.005,		/* double  mutation */
       0.0              /* double  migration */
                              );

    ga_evolution(
       pop,		/* population              *pop */
       20		/* const int               max_generations */
              );

    dejong_ga_callback(i, pop);

    printf("The final solution with seed = %d was:", i);
    for (j=0; j<30; j++) printf(" %d", ga_get_entity_from_rank(pop,0)->chromosome[0][j]);
    printf(" score = %f", ga_get_entity_from_rank(pop,0)->fitness);
    printf("\n");
    }

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


