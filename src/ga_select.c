/**********************************************************************
  ga_select.c
 **********************************************************************

  ga_select - Genetic algorithm selection operators.
  Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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

#include "gaul/ga_core.h"

/**********************************************************************
  gaul_select_stats()
  synopsis:     Determine mean and standard deviation (and some other
                potentially useful junk) of the fitness scores.
  parameters:	population *pop
  return:	TRUE
  last updated: 30/04/01
 **********************************************************************/

static boolean gaul_select_stats( population *pop,
                             double *average, double *stddev, double *sum )
  {
  int           i;                      /* Loop over all entities. */
  double        fsum=0.0, fsumsq=0.0;   /* Sum and sum squared. */

#if 0
/*
 * Checks not needed for this static function unless used by
 * external code... which it isn't.
 */
  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");
#endif

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
  gaul_select_sum_fitness()
  synopsis:	Determine sum of entity fitnesses.
  parameters:	population *pop
  return:	double sum
  last updated: 11 Jun 2002
 **********************************************************************/

static double gaul_select_sum_fitness( population *pop )
  {
  int           i;		/* Loop over all entities. */
  double        sum=0.0;	/* Sum and sum squared. */

  for (i=0; i<pop->orig_size; i++)
    {
    sum += pop->entity_iarray[i]->fitness;
    }

  return sum;
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
    gaul_select_stats(pop, &mean, &stddev, &sum);
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
    gaul_select_stats(pop, &mean, &stddev, &sum);
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
    gaul_select_stats(pop, &mean, &stddev, &sum);
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
    gaul_select_stats(pop, &mean, &stddev, &sum);
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


/**********************************************************************
  ga_select_one_sus()
  synopsis:	Stochastic Universal Sampling selection.
  		pop->mutation_ratio multiplied by pop->orig_size gives
		the number of selections which will be performed.
		This version is for fitness values where 0.0 is bad and
		large positive values are good.  Negative values will
		severely mess-up the algorithm.
  parameters:
  return:	
  last updated: 11 Jun 2002
 **********************************************************************/

boolean ga_select_one_sus(population *pop, entity **mother)
  {
  static double	offset;			/* Current pointer offset. */
  static double	step;			/* Distance between each pointer. */
  static int	current;		/* Currently selected individual. */
  static int	num_to_select;		/* Number of individuals to select. */
  double	sum;			/* Fitness total. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    num_to_select = (pop->orig_size*pop->mutation_ratio);
    sum = gaul_select_sum_fitness(pop);
    step = sum/(pop->orig_size*pop->mutation_ratio);
    offset = random_double(step);
    current=0;
    }
  else if (pop->select_state>num_to_select)
    {
    return TRUE;
    }
  else
    {
    offset += step;
    }

  while (offset > pop->entity_iarray[current]->fitness)
    {
    offset -= pop->entity_iarray[current]->fitness;
    current++;
    if (current>=pop->orig_size) current-=pop->orig_size;
    }

  *mother = pop->entity_iarray[current];

  pop->select_state++;

  return FALSE;
  }


/**********************************************************************
  ga_select_two_sus()
  synopsis:	Stochastic Universal Sampling selection.
  		pop->mutation_ratio multiplied by pop->orig_size gives
		the number of selections which will be performed.
		This version is for fitness values where 0.0 is bad and
		large positive values are good.  Negative values will
		severely mess-up the algorithm.
  parameters:
  return:	
  last updated: 11 Jun 2002
 **********************************************************************/

boolean ga_select_two_sus(population *pop, entity **mother, entity **father)
  {
  static double	offset1, offset2;	/* Current pointer offsets. */
  static double	step;			/* Distance between each pointer. */
  static int	current1, current2;	/* Currently selected individuals. */
  static int	*permutation=NULL;	/* Randomly ordered indices. */
  static int	num_to_select;		/* Number of individuals to select. */
  double	sum;			/* Fitness total. */
  int		*ordered;		/* Ordered indices. */
  int		i;			/* Loop variable over indices. */

  if (!pop) die("Null pointer to population structure passed.");

  *mother = NULL;

  if (pop->orig_size < 1)
    {
    return TRUE;
    }

  if (pop->select_state == 0)
    { /* First call of this generation. */
    num_to_select = (pop->orig_size*pop->crossover_ratio);
    sum = gaul_select_sum_fitness(pop);
    step = sum/num_to_select;
    offset1 = offset2 = random_double(step);
    current1=0;
    current2=0;

    if (permutation!=NULL) dief("Internal error.  Permutation buffer not NULL.");
    permutation = s_malloc(sizeof(int)*pop->orig_size);
    ordered = s_malloc(sizeof(int)*pop->orig_size);
    for (i=0; i<pop->orig_size;i++)
      ordered[i]=i;
    random_int_permutation(pop->orig_size, ordered, permutation);
    s_free(ordered);
    }
  else if (pop->select_state>num_to_select)
    {
    s_free(permutation);
    permutation=NULL;
    return TRUE;
    }
  else
    {
    offset1 += step;
    offset2 += step;
    }

  while (offset1 > pop->entity_iarray[current1]->fitness)
    {
    offset1 -= pop->entity_iarray[current1]->fitness;
    current1++;
    if (current1>=pop->orig_size) current1-=pop->orig_size;
    }

  while (offset2 > pop->entity_iarray[permutation[current2]]->fitness)
    {
    offset2 -= pop->entity_iarray[permutation[current2]]->fitness;
    current2++;
    if (current2>=pop->orig_size) current2-=pop->orig_size;
    }

  *mother = pop->entity_iarray[current1];
  *father = pop->entity_iarray[permutation[current2]];

  pop->select_state++;

  return FALSE;
  }


/**********************************************************************
  ga_select_one_aggressive()
  synopsis:	Select an entity using a very aggressive procedure.
  parameters:
  return:	
  last updated: 18 Apr 2003
 **********************************************************************/

boolean ga_select_one_aggressive(population *pop, entity **mother)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *mother = pop->entity_iarray[random_int(1+pop->select_state%(pop->orig_size-1))];

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_two_aggressive()
  synopsis:	Select a pair of entities, both by a very aggressive
		procedure.  The entities may be the same.
  parameters:
  return:	
  last updated: 18 Apr 2003
 **********************************************************************/

boolean ga_select_two_aggressive(population *pop, entity **mother, entity **father)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *father = pop->entity_iarray[random_int(1+pop->select_state%(pop->orig_size-1))];
  *mother = pop->entity_iarray[random_int(1+pop->select_state%(pop->orig_size-1))];

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


/**********************************************************************
  ga_select_one_best()
  synopsis:	Select the best entity only.
  parameters:
  return:	
  last updated: 18 Apr 2003
 **********************************************************************/

boolean ga_select_one_best(population *pop, entity **mother)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *mother = pop->entity_iarray[0];

  return pop->select_state>(pop->orig_size*pop->mutation_ratio);
  }


/**********************************************************************
  ga_select_two_best()
  synopsis:	Select a pair of entities, one of which is random, the
                other is the best entity.
  parameters:
  return:	
  last updated: 18 Apr 2003
 **********************************************************************/

boolean ga_select_two_best(population *pop, entity **mother, entity **father)
  {

  if (!pop) die("Null pointer to population structure passed.");

  pop->select_state++;

  *mother = pop->entity_iarray[random_int(pop->orig_size)];
  *father = pop->entity_iarray[0];

  return pop->select_state>(pop->orig_size*pop->crossover_ratio);
  }


