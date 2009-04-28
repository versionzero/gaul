/**********************************************************************
  onemax.c
 **********************************************************************

  onemax - Test/example program for GAUL.
  Copyright ©2003, Stewart Adcock <stewart@linux-domain.com>
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

		This is an implementation of the common "onemax" GA
		test.  This version uses a GAUL bitstring chromosome
		with a standard generational GA.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/*
 * Hard-coded parameter settings.
 * FIXME: Should make these user options.
 */
#define NBITS		64	/* Number of 'bits' in chromosomes. */

/**********************************************************************
  onemax_score()
  synopsis:	Score solution.  Simply count the '1's in the
		chromosome.
  parameters:	population *pop
		entity *entity
  return:	always TRUE
  updated:	25 Jul 2003
 **********************************************************************/

boolean onemax_score(population *pop, entity *entity)
  {
  double	score = 0.0;
  int		i;

  for(i=0; i<NBITS; i++)
    if(ga_bit_get((byte *)entity->chromosome[0], i) == 1) score += 1.0;

  entity->fitness = score;

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	The common "onemax" GA test.
  parameters:
  return:
  updated:	25 Jul 2003
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;	/* Population of solutions. */
  int		seed=12345678;	/* Random number seed. */
  int		i;		/* Loop over alleles. */

  printf("This is an implementation of the trivial \"onemax\" GA test using GAUL.\n");
  printf("The chromosome contains %d bits.\n", NBITS);
  printf("\n");
  printf("Random number seed is %d\n", seed);
  printf("\n");

  random_seed(seed);

  pop = ga_genesis_bitstring(
     100,			/* const int              population_size */
     1,				/* const int              num_chromo */
     NBITS,			/* const int              len_chromo */
     NULL,			/* GAgeneration_hook      generation_hook */
     NULL,			/* GAiteration_hook       iteration_hook */
     NULL,			/* GAdata_destructor      data_destructor */
     NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
     onemax_score,		/* GAevaluate             evaluate */
     ga_seed_bitstring_random,	/* GAseed                 seed */
     NULL,			/* GAadapt                adapt */
     ga_select_one_bestof2,	/* GAselect_one           select_one */
     ga_select_two_bestof2,	/* GAselect_two           select_two */
     ga_mutate_bitstring_singlepoint,	/* GAmutate               mutate */
     ga_crossover_bitstring_doublepoints,	/* GAcrossover            crossover */
     NULL,			/* GAreplace              replace */
     NULL			/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_DIE,	/* const ga_elitism_type   elitism */
       0.9,			/* double  crossover */
       0.1,			/* double  mutation */
       0.0              	/* double  migration */
                              );

  ga_evolution(
       pop,		/* population              *pop */
       100		/* const int               max_generations */
              );

  printf("The final solution with seed = %d had score %d\n",
         seed, (int) ga_get_entity_from_rank(pop,0)->fitness);
  for (i=0; i<NBITS; i++)
    printf("%d", ga_bit_get((byte *)ga_get_entity_from_rank(pop,0)->chromosome[0],i)?1:0);
  printf("\n");

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


