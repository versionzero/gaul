/**********************************************************************
  ga_select.c
 **********************************************************************

  ga_select - Genetic algorithm selection operators.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:     Routines for performing GA selection operations.

		This selection routines return TRUE if the selection
		procedure has run to completion, otherwise they return
		FALSE.  They may potentially return NULL for the
		selected entities.  This is valid behaviour and doesn't
		necessarily indicate that the selection producedure is
		complete.

		On the first call to these routines in a given
		generation, pop->select_state is gauranteed to be set
		to zero.  These routines are then free to modify this
		value, for example, to store the number of selections
		performed in this generation.

		The ga_select_one_xxx() functions are intended for
		asexual selections.
		The ga_select_two_xxx() functions are intended for
		sexual selections.  Checking whether the mother and
		father are different entities is optional.

		The calling code is welcome to not use any of these
		functions.

		These functions return a pointer to the entity instead
		of an id because, potentially, the entities may come
		from a different population.

		It may be important to use the value held in the
		pop->orig_size field instead of the pop->size field
		because the population size is liable to increase
		between calls to these functions!  (Although, of course,
		you are free to use whichever value you like in
		user-defined functions.)

  To do:	Reimplement stochastic universal selection etc. using this callback mechanism.
		Reimplement probability ranges: mutation_prob = mutation_max - (mutation_max-mutation_min)*i/pop->orig_size;

 **********************************************************************/

#include "ga_core.h"

/**********************************************************************
  ga_select_stats()
  synopsis:     Determine mean and standard deviation (and some other
                potentially useful junk) of the fitness scores.
  parameters:
  return:
  last updated: 30/04/01
 **********************************************************************/

static boolean ga_select_stats( population *pop,
                             double *average, double *stddev, double *sum )
  {
  int           i;                      /* Loop over all entities. */
  double        fsum=0.0, fsumsq=0.0;   /* Sum and sum squared. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");

  for (i=0; i<pop->orig_size; i++)
    {
    fsum += pop->entity_iarray[i]->fitness;
    fsumsq += SQU(pop->entity_iarray[i]->fitness);
    }

  *sum = fsum;
  *average = fsum / pop->orig_size;
  *stddev = (fsumsq - fsum*fsum/pop->orig_size)/pop->orig_size;

  return TRUE;
  }


/**********************************************************************
  ga_select_one_random()
  synopsis:	Select a single random entity.  Selection stops when
		(population size)*(mutation ratio)=(number selected)
  parameters:
  return:	
  last updated: 30/04/01
 **********************************************************************/

boolean ga_select_one_random(population *pop, entity **mother)
  {

  if (!pop) die("Null pointer to population structure passed.");

  if (pop->orig_size < 1)
    {
    *mother = NULL;
    return TRUE;
    }

  *mother = pop->entity_iarray[random_int(pop->orig_size)];

  pop->select_state++;

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_two_random()
  synopsis:	Select a pair of random entities.  Selection stops when
		(population size)*(crossover ratio)=(number selected)
  parameters:
  return:	
  last updated: 30/04/01
 **********************************************************************/

boolean ga_select_two_random(population *pop, entity **mother, entity **father)
  {

  if (!pop) die("Null pointer to population structure passed.");

  if (pop->orig_size < 2)
    {
    *mother = NULL;
    *father = NULL;
    return TRUE;
    }

  *mother = pop->entity_iarray[random_int(pop->orig_size)];
  do 
    {
    *father = pop->entity_iarray[random_int(pop->orig_size)];
    } while (*mother == *father);

  pop->select_state++;

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


/**********************************************************************
  ga_select_one_every()
  synopsis:	Select every entity.
  parameters:
  return:	
  last updated: 23/04/01
 **********************************************************************/

boolean ga_select_one_every(population *pop, entity **mother)
  {

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if ( pop->orig_size <= pop->select_state )
    {
    return TRUE;
    }

  *mother = pop->entity_iarray[pop->select_state];

  pop->select_state++;

  return FALSE;
  }


/**********************************************************************
  ga_select_two_every()
  synopsis:	Select every possible pair of parents.
  parameters:
  return:	
  last updated: 23/04/01
 **********************************************************************/

boolean ga_select_two_every(population *pop, entity **mother, entity **father)
  {

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;
  *father = NULL;

  if ( SQU(pop->orig_size) <= pop->select_state )
    {
    return TRUE;
    }

  *mother = pop->entity_iarray[pop->select_state%pop->orig_size];
  *father = pop->entity_iarray[pop->select_state/pop->orig_size];

  pop->select_state++;

  return FALSE;
  }


/**********************************************************************
  ga_select_one_randomrank()
  synopsis:	Select a single entity by my rank-based method.
  parameters:
  return:	
  last updated: 23/04/01
 **********************************************************************/

boolean ga_select_one_randomrank(population *pop, entity **mother)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *mother = NULL;

  if ( pop->orig_size < pop->select_state )
    {
    return TRUE;
    }

  if ( random_boolean_prob(pop->mutation_ratio) )
    {
    *mother = pop->entity_iarray[random_int(pop->select_state)];
    }

  return FALSE;
  }


/**********************************************************************
  ga_select_two_randomrank()
  synopsis:	Select a pair of entities by my rank-based method.
		Basically, I loop through all entities, and each is
		paired with a random, fitter, partner.
  parameters:
  return:	
  last updated: 23/04/01
 **********************************************************************/

boolean ga_select_two_randomrank(population *pop, entity **mother, entity **father)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *mother = NULL;
  *father = NULL;

  if ( pop->orig_size < pop->select_state )
    {
    return TRUE;
    }

  if ( random_boolean_prob(pop->crossover_ratio) )
    {
    *mother = pop->entity_iarray[random_int(pop->select_state)];
    *father = pop->entity_iarray[pop->select_state];
    }

  return FALSE;
  }


/**********************************************************************
  ga_select_one_bestof2()
  synopsis:	Kind of tournament selection.  Choose two random
		entities, return the best as the selection.  Selection
		stops when
		(population size)*(mutation ratio)=(number selected)
  parameters:
  return:	
  last updated: 30/04/01
 **********************************************************************/

boolean ga_select_one_bestof2(population *pop, entity **mother)
  {
  entity	*mother2;	/* Random competitor. */

  if (!pop) die("Null pointer to population structure passed.");

  if (pop->orig_size < 1)
    {
    *mother = NULL;
    return TRUE;
    }

  *mother = pop->entity_iarray[random_int(pop->orig_size)];
  mother2 = pop->entity_iarray[random_int(pop->orig_size)];

  if (mother2->fitness > (*mother)->fitness)
    *mother = mother2;

  pop->select_state++;

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_two_bestof2()
  synopsis:	Kind of tournament selection.  For each parent, choose
		two random entities, return the best as the selection.
		The two parents will be different.  Selection
		stops when
		(population size)*(crossover ratio)=(number selected)
  parameters:
  return:	
  last updated: 30/04/01
 **********************************************************************/

boolean ga_select_two_bestof2(population *pop, entity **mother, entity **father)
  {
  entity	*challenger;	/* Random competitor. */

  if (!pop) die("Null pointer to population structure passed.");

  if (pop->orig_size < 2)
    {
    *mother = NULL;
    *father = NULL;
    return TRUE;
    }

  *mother = pop->entity_iarray[random_int(pop->orig_size)];
  challenger = pop->entity_iarray[random_int(pop->orig_size)];

  if (challenger->fitness > (*mother)->fitness)
    *mother = challenger;

  do 
    {
    *father = pop->entity_iarray[random_int(pop->orig_size)];
    challenger = pop->entity_iarray[random_int(pop->orig_size)];

    if (challenger->fitness > (*father)->fitness)
      *father = challenger;
    } while (*mother == *father);

  pop->select_state++;

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


/**********************************************************************
  ga_select_one_roulette()
  synopsis:	Fitness-proportionate roulette wheel selection.
		If pop->mutation_ratio is 1.0, the wheel will be spun
		pop->orig_size times, which matches Holland's original
		implementation.
		This version is for fitness values where 0.0 is bad and
		large positive values are good.  Negative values will
		severely mess-up the algorithm.
  parameters:
  return:	
  last updated: 11 Apr 2002
 **********************************************************************/

boolean ga_select_one_roulette(population *pop, entity **mother)
  {
  static double	mean, stddev, sum;	/* Fitness statistics. */
  static double	total_expval;		/* Total of expectancy values. */
  static int	marker;			/* The roulette wheel marker. */
  double	selectval;		/* Select when this reaches zero. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    ga_select_stats(pop, &mean, &stddev, &sum);
    total_expval=sum/mean;
    marker = random_int(pop->orig_size);
    }

  selectval = random_double(total_expval)*mean;

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= pop->entity_iarray[marker]->fitness;

    } while (selectval>0.0);

  pop->select_state++;

  *mother = pop->entity_iarray[marker];

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_one_roulette_rebased()
  synopsis:	Fitness-proportionate roulette wheel selection.
		If pop->mutation_ratio is 1.0, the wheel will be spun
		pop->orig_size times, which matches Holland's original
		implementation.
		This version can cope with a mixture of positive and
		negative fitness scores.  The single least fit entity
		will never be selected, but this is not considered a
		problem.
  parameters:
  return:	
  last updated: 11 Apr 2002
 **********************************************************************/

boolean ga_select_one_roulette_rebased(population *pop, entity **mother)
  {
  static double	mean, stddev, sum;	/* Fitness statistics. */
  static double	total_expval;		/* Total of expectancy values. */
  static double	minval;			/* Worst fitness value. */
  static int	marker;			/* The roulette wheel marker. */
  double	selectval;		/* Select when this reaches zero. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    ga_select_stats(pop, &mean, &stddev, &sum);
    marker = random_int(pop->orig_size);
    minval = pop->entity_iarray[pop->orig_size-1]->fitness;
    mean -= minval;
    if (ISTINY(mean)) die("Degenerate population?");
    total_expval = (sum-minval*pop->orig_size)/mean;
    }

  selectval = random_double(total_expval);

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= (pop->entity_iarray[marker]->fitness-minval)/mean;

    } while (selectval>0.0);

  pop->select_state++;

  *mother = pop->entity_iarray[marker];

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_two_roulette()
  synopsis:	Fitness-proportionate roulette wheel selection.
		If pop->mutation_ratio is 1.0, the wheel will be spun
		pop->orig_size times, which matches Holland's original
		implementation.
		This version is for fitness values where 0.0 is bad and
		large positive values are good.  Negative values will
		severely mess-up the algorithm.
                Mother and father may be the same.
  parameters:
  return:	
  last updated: 11 Apr 2002
 **********************************************************************/

boolean ga_select_two_roulette( population *pop,
                                entity **mother, entity **father )
  {
  static double	mean, stddev, sum;	/* Fitness statistics. */
  static double	total_expval;		/* Total of expectancy values. */
  static int	marker;			/* The roulette wheel marker. */
  double	selectval;		/* Select when this reaches zero. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;
  *father = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    ga_select_stats(pop, &mean, &stddev, &sum);
    total_expval=sum/mean;
    marker = random_int(pop->orig_size);
/*
printf("Mean fitness = %f stddev = %f sum = %f expval = %f\n", mean, stddev, sum, total_expval);
*/
    }

  pop->select_state++;

  selectval = random_double(total_expval)*mean;

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= pop->entity_iarray[marker]->fitness;

    } while (selectval>0.0);

  *mother = pop->entity_iarray[marker];

  selectval = random_double(total_expval)*mean;

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= pop->entity_iarray[marker]->fitness;

    } while (selectval>0.0);

  *father = pop->entity_iarray[marker];

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


/**********************************************************************
  ga_select_two_roulette_rebased()
  synopsis:	Fitness-proportionate roulette wheel selection.
		If pop->mutation_ratio is 1.0, the wheel will be spun
		pop->orig_size times, which matches Holland's original
		implementation.
		This version can cope with a mixture of positive and
		negative fitness scores.  The single least fit entity
		will never be selected, but this is not considered a
		problem.
                Mother and father may be the same.
  parameters:
  return:	
  last updated: 11 Apr 2002
 **********************************************************************/

boolean ga_select_two_roulette_rebased( population *pop,
                                        entity **mother, entity **father )
  {
  static double	mean, stddev, sum;	/* Fitness statistics. */
  static double	total_expval;		/* Total of expectancy values. */
  static double	minval;			/* Worst fitness value. */
  static int	marker;			/* The roulette wheel marker. */
  double	selectval;		/* Select when this reaches zero. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    ga_select_stats(pop, &mean, &stddev, &sum);
    marker = random_int(pop->orig_size);
    minval = pop->entity_iarray[pop->orig_size-1]->fitness;
    mean -= minval;
    if (ISTINY(mean)) die("Degenerate population?");
    total_expval = (sum-minval*pop->orig_size)/mean;
    }

  pop->select_state++;

  selectval = random_double(total_expval);

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= (pop->entity_iarray[marker]->fitness-minval)/mean;

    } while (selectval>0.0);

  *mother = pop->entity_iarray[marker];

  selectval = random_double(total_expval);

  do
    {
    marker++;

    if (marker >= pop->orig_size) marker=0;

    selectval -= (pop->entity_iarray[marker]->fitness-minval)/mean;

    } while (selectval>0.0);

  *father = pop->entity_iarray[marker];

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


