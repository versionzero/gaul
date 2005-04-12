/**********************************************************************
  test_de.c
 **********************************************************************

  test_de - Test program for GAUL.
  Copyright ©2002-2005, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test program for GAUL's differential evolution algorithm.

		This program aims to solve a function of the form
		(0.75-A)+(0.95-B)^2+(0.23-C)^3+(0.71-D)^4 = 0

 **********************************************************************/

#include "gaul.h"

struct strategies_t
  {
  char			*label; 
  ga_de_strategy_type	strategy;
  ga_de_crossover_type	crossover;
  int			num_perturbed;
  double		crossover_factor;
  double		weighting_factor;
  double		weighting_factor2;
  };

static struct strategies_t strategy[]={
        { "DE/best/1/exp (DE0)",     GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 0.5, 0.5 },
        { "DE/best/1/exp (DE0)",     GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 2.0, 0.0 },
        { "DE/best/2/exp",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 0.5, 0.5 },
        { "DE/best/2/exp",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 2.0, 0.0 },
        { "'DE/best/3/exp'",         GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 3, 0.8, 0.5, 0.5 },
        { "'DE/best/3/exp'",         GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_EXPONENTIAL, 3, 0.8, 2.0, 0.0 },
        { "DE/rand/1/exp (DE1)",     GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 0.5, 0.5 },
        { "DE/rand/1/exp (DE1)",     GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 2.0, 0.0 },
        { "DE/rand/2/exp",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 0.5, 0.5 },
        { "DE/rand/2/exp",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 2.0, 0.0 },
        { "'DE/rand/3/exp'",         GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 3, 0.8, 0.5, 0.5 },
        { "'DE/rand/3/exp'",         GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_EXPONENTIAL, 3, 0.8, 2.0, 0.0 },
        { "DE/rand-to-best/1/exp",   GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 0.5, 0.5 },
        { "DE/rand-to-best/1/exp",   GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_EXPONENTIAL, 1, 0.8, 2.0, 0.0 },
        { "'DE/rand-to-best/2/exp'", GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 0.5, 0.5 },
        { "'DE/rand-to-best/2/exp'", GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_EXPONENTIAL, 2, 0.8, 2.0, 0.0 },
        { "DE/best/1/bin",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 0.5, 0.5 },
        { "DE/best/1/bin",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 2.0, 0.0 },
        { "DE/best/2/bin",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 0.5, 0.5 },
        { "DE/best/2/bin",           GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 2.0, 0.0 },
        { "'DE/best/3/bin'",         GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    3, 0.8, 0.5, 0.5 },
        { "'DE/best/3/bin'",         GA_DE_STRATEGY_BEST,       GA_DE_CROSSOVER_BINOMIAL,    3, 0.8, 2.0, 0.0 },
        { "DE/rand/1/bin",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 0.5, 0.5 },
        { "DE/rand/1/bin",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 2.0, 0.0 },
        { "DE/rand/2/bin",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 0.5, 0.5 },
        { "DE/rand/2/bin",           GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 2.0, 0.0 },
        { "'DE/rand/3/bin'",         GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    3, 0.8, 0.5, 0.5 },
        { "'DE/rand/3/bin'",         GA_DE_STRATEGY_RAND,       GA_DE_CROSSOVER_BINOMIAL,    3, 0.8, 2.0, 0.0 },
        { "DE/rand-to-best/1/bin",   GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 0.5, 0.5 },
        { "DE/rand-to-best/1/bin",   GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_BINOMIAL,    1, 0.8, 2.0, 0.0 },
        { "'DE/rand-to-best/2/bin'", GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 0.5, 0.5 },
        { "'DE/rand-to-best/2/bin'", GA_DE_STRATEGY_RANDTOBEST, GA_DE_CROSSOVER_BINOMIAL,    2, 0.8, 2.0, 0.0 },
        { NULL, 0, 0, 0, 0.0, 0.0 } };


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
  test_generation_callback()
  synopsis:	Generation callback
  parameters:
  return:
  updated:	21 Mar 2005
 **********************************************************************/

boolean test_generation_callback(int generation, population *pop)
  {

/*
 * This is a easy method for implementing randomly selected
 * scaling factor (F in original paper) for each generation, as
 * suggested in:
 *
 * Karaboga D., Okdem, S. "A simple and global optimization algorithm
 * for engineering problems: differential evolution algorithm",
 * Elec. Engin. 12:53-60 (2004).
 *
 * Uncomment, if desired.
 */
/*
  pop->de_params->weighting_factor = random_double_range(-2.0, 2.0);
*/

/*
 * Write rank 1 solution every tenth generation.  Note, that this is
 * not neccesarily the best solution because DE doesn't require the
 * population to be sorted, as genetic algorithms usually do.
 */
  if ( generation%10 == 0)
    printf( "%d: A = %f B = %f C = %f D = %f (fitness = %f)\n",
            generation,
            ((double *)pop->entity_iarray[0]->chromosome[0])[0],
            ((double *)pop->entity_iarray[0]->chromosome[0])[1],
            ((double *)pop->entity_iarray[0]->chromosome[0])[2],
            ((double *)pop->entity_iarray[0]->chromosome[0])[3],
            pop->entity_iarray[0]->fitness );

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
  updated:	21 Mar 2005
 **********************************************************************/

int main(int argc, char **argv)
  {
  population		*pop;		/* Population of solutions. */
  int			i=0;		/* Loop variable over strategies. */

  random_seed(23091975);

  log_init(LOG_NORMAL, NULL, NULL, FALSE);

  while ( strategy[i].label != NULL )
    {
    if ( strategy[i].weighting_factor != strategy[i].weighting_factor2 )
      {
      printf( "Strategy %s ; C = %f ; F = rand( %f, %f )\n",
            strategy[i].label,
            strategy[i].crossover_factor,
            strategy[i].weighting_factor, strategy[i].weighting_factor2 );
      }
    else
      {
      printf( "Strategy %s ; C = %f ; F = %f\n",
              strategy[i].label,
              strategy[i].crossover_factor,
              strategy[i].weighting_factor );
      }

    pop = ga_genesis_double(
         40,			/* const int              population_size */
         1,			/* const int              num_chromo */
         4,			/* const int              len_chromo */
         test_generation_callback,/* GAgeneration_hook      generation_hook */
         NULL,			/* GAiteration_hook       iteration_hook */
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
         NULL			/* vpointer               User data */
            );

    ga_population_set_differentialevolution_parameters(
        pop, strategy[i].strategy, strategy[i].crossover,
        strategy[i].num_perturbed, strategy[i].weighting_factor, strategy[i].weighting_factor2,
        strategy[i].crossover_factor
        );

    ga_differentialevolution(
         pop,				/* population	*pop */
         50				/* const int	max_generations */
              );

    printf( "Final: A = %f B = %f C = %f D = %f (fitness = %f)\n",
            ((double *)pop->entity_iarray[0]->chromosome[0])[0],
            ((double *)pop->entity_iarray[0]->chromosome[0])[1],
            ((double *)pop->entity_iarray[0]->chromosome[0])[2],
            ((double *)pop->entity_iarray[0]->chromosome[0])[3],
            pop->entity_iarray[0]->fitness );

    ga_extinction(pop);

    i++;
    }

  exit(EXIT_SUCCESS);
  }


