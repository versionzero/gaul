/**********************************************************************
  nnevolve.c
 **********************************************************************

  nnevolve - GAUL example: Evolve a fixed topology neural network.
  Copyright ©2002, The Regents of the University of California.
  Primary author: "Stewart Adcock" <stewart@linux-domain.com>

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

  Synopsis:	Example program for GAUL.

		This program aims to evolve a fixed topology neural
		network.  Although the topology of the network is
		fixed, certian parameters are evolved along with the
		weights.  I haven't seen this in the literature, but
		I haven't looked particularly hard either.

		The genome consists of a single chromosome which is
		simply the neural network structure, as defined in
		nn_util.h

 **********************************************************************/

#include "nnevolve.h"

/**********************************************************************
  nnevolve_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

boolean nnevolve_score(population *pop, entity *entity)
  {
  int		i;	/* Team member. */
  double	score=0.0, lossscore=0.0, badscore=0.0;
  int		badloss=0, loss=0;

  entity->fitness = 0;

  for (i=0; i<25; i++)
    {
    score = (((int *)entity->chromosome[0])[i] - i)*4 + 2;
    if (score > 0)
      {
      loss++;
      lossscore += score;
      if (score > 10)
        {
        badloss++;
        badscore += score;
        }
      }
    }
  lossscore /= loss;

/* In no case should a player lose by > 10 points. */
/*  entity->fitness -= badloss * 3.0;*/
  entity->fitness -= badscore * 2.0;

/* Average loss should be as close to 6 as possible. */
  entity->fitness -= SQU(6 - lossscore) * 3.0;

/* Team should win majority of the games. */
  if (loss>12) entity->fitness -= loss;
  
  return TRUE;
  }


/**********************************************************************
  nnevolve_seed()
  synopsis:	Seed initial solutions.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

void nnevolve_seed(population *pop, entity *adam)
  {
  int		i, j;	/* Team members. */
  int		*data;	/* Chromosome. */

  data = (int *)adam->chromosome[0];

  for (i=0; i<25; i++) data[i] = -1;

  for (i=0; i<25; i++)
    {
    j = random_int(25);
    while (data[j] > -1)
      {
      if (j == 24)
        j = 0;
      else
        j++;
      }

    data[j] = i;
    }

  return;
  }


/**********************************************************************
  nnevolve_crossover()
  synopsis:	Crossover.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

void nnevolve_crossover(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
  {
  int		i, j;	/* Team members. */

  for (i=0; i<25; i++)
    {
    if (random_boolean())
      {
      ((int *)son->chromosome[0])[i] = ((int *)father->chromosome[0])[i];
      ((int *)daughter->chromosome[0])[i] = ((int *)mother->chromosome[0])[i];
      }
   else
      {
      ((int *)son->chromosome[0])[i] = ((int *)father->chromosome[0])[i];
      ((int *)daughter->chromosome[0])[i] = ((int *)mother->chromosome[0])[i];
      }
    }

  for (i=1; i<25; i++)
    {
    for (j=0; j<i; j++)
      {
      if (((int *)son->chromosome[0])[j] == ((int *)son->chromosome[0])[i])
        {
        if (((int *)son->chromosome[0])[i]==24)
          ((int *)son->chromosome[0])[i]=0;
        else
          ((int *)son->chromosome[0])[i]++;
        j=0;
        }
      }
    for (j=0; j<i; j++)
      {
      if (((int *)daughter->chromosome[0])[j] == ((int *)daughter->chromosome[0])[i])
        {
        if (((int *)daughter->chromosome[0])[i]==24)
          ((int *)daughter->chromosome[0])[i]=0;
        else
          ((int *)daughter->chromosome[0])[i]++;
        j=0;
        }
      }
    }

  return;
  }


/**********************************************************************
  nnevolve_mutate_swap()
  synopsis:	Mutation by swapping two team members.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

void nnevolve_mutate_swap(population *pop, entity *mother, entity *son)
  {
  int		i, j;	/* Team members. */
  int		tmp;	/* For swapping i and j. */

/* Copy chromosomes of parent to offspring. */
  memcpy( son->chromosome[0],
          mother->chromosome[0],
          pop->len_chromosomes*sizeof(int) );

  i = random_int(25);
  j = random_int(25);

  if (i==j)
    {
    if (j==24)
      j=0;
    else
      j++;
    }

  tmp = ((int *)son->chromosome[0])[i];
  ((int *)son->chromosome[0])[i] = ((int *)son->chromosome[0])[j];
  ((int *)son->chromosome[0])[j] = tmp;

  return;
  }


/**********************************************************************
  nnevolve_mutate_shift()
  synopsis:	Mutation by shifting a team member.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

void nnevolve_mutate_shift(population *pop, entity *mother, entity *son)
  {
  int		i, j, k;	/* Team members. */
  int		tmp;		/* For swapping i and j. */

/* Copy chromosomes of parent to offspring. */
  memcpy( son->chromosome[0],
          mother->chromosome[0],
          pop->len_chromosomes*sizeof(int) );

  i = random_int(25);

  do
    {
    j = random_int(25);
    } while(i==j);

  if (i>j)
    {
    tmp = ((int *)son->chromosome[0])[j];
    for (k=j; k<i; k++)
      {
      ((int *)son->chromosome[0])[k] = ((int *)son->chromosome[0])[k+1];
      }
    ((int *)son->chromosome[0])[i] = tmp;
    }
  else
    {
    tmp = ((int *)son->chromosome[0])[j];
    for (k=j; k>i; k--)
      {
      ((int *)son->chromosome[0])[k] = ((int *)son->chromosome[0])[k-1];
      }
    ((int *)son->chromosome[0])[i] = tmp;
    }

  return;
  }


/**********************************************************************
  nnevolve_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

void nnevolve_mutate(population *pop, entity *mother, entity *son)
  {

  /* Checks. */
  if (!mother || !son) die("Null pointer to entity structure passed");

  if (random_boolean_prob(0.2))
    nnevolve_mutate_swap(pop, mother, son);
  else
    nnevolve_mutate_shift(pop, mother, son);

  return;
  }


/**********************************************************************
  nnevolve_ga_callback()
  synopsis:	Analysis callback.
  parameters:
  return:
  updated:	25 Jan 2002
 **********************************************************************/

boolean nnevolve_generation_hook(int generation, population *pop)
  {
  entity	*best;		/* Top ranked solution. */

  best = ga_get_entity_from_rank(pop, 0);

  printf( "%d: fitness = %f momentum = %f gain = %f rate = %f bias = %f\n",
            generation,
            best->fitness,
            ((network_t *)best->chromosome[0])->momentum,
            ((network_t *)best->chromosome[0])->gain,
            ((network_t *)best->chromosome[0])->rate,
            ((network_t *)best->chromosome[0])->bias );

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Evolve a fixed topology neural network.
  parameters:
  return:	2, on success.
  updated:	25 Jan 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;		/* Runs. */
  population	*pop=NULL;	/* Population of solutions. */

/*
 * Initialize random number generator.
 */
  random_init();
  random_seed(42);

/*
 * Allocate a new popuation structure.
 * max. individuals        = 200
 * stable num. individuals = 40
 * num. chromosomes        = 1
 * length of chromosomes   = 0 (This is ignored by the constructor)
 */
  pop = ga_population_new( 200, 40, 1, 0 );
  if ( !pop ) die("Unable to allocate population.");

/*
 * Define chromosome handling functions for the custom
 * NN chromosome type.
 */
  pop->chromosome_constructor = nnevolve_constructor;
  pop->chromosome_destructor = nnevolve_destructor;
  pop->chromosome_replicate = nnevolve_replicate;
  pop->chromosome_to_bytes = nnevolve_to_bytes;
  pop->chromosome_from_bytes = nnevolve_from_bytes;
  pop->chromosome_to_string = nnevolve_to_string;

/*
 * Define all the needed callback functions.
 */
  pop->generation_hook = nnevolve_generation_hook;
  pop->iteration_hook = NULL;
  pop->data_destructor = NULL;
  pop->data_ref_incrementor = NULL;

  pop->evaluate = nnevolve_evaluate;
  pop->seed = nnevolve_seed;
  pop->adapt = nnevolve_adapt;
  pop->select_one = ga_select_one_roulette;
  pop->select_two = ga_select_two_roulette;
  pop->mutate = nnevolve_mutate;
  pop->crossover = nnevolve_crossover;
  pop->replace = NULL;

/*
 * Seed the initial population.
 */
  ga_population_seed(pop);

/*
 * Set the GA parameters:
 * Crossover ratio  = 0.7
 * Mutation ratio   = 0.1
 * Migration ration = 0.0
 */
  ga_population_set_parameters( pop, 0.7, 0.1, 0.0 );

/*
 * Perform Lamarckian evolution for 200 generations.
 */
  ga_evolution( pop, GA_CLASS_LAMARCK_ALL, GA_ELITISM_PARENTS_SURVIVE, 200 );

  printf("The fitness of the final solution found was: %f\n",
		  ga_get_entity_from_rank(pop,0)->fitness);

  ga_extinction(pop);

  exit(2);
  }


