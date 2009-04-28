/**********************************************************************
  pingpong.c
 **********************************************************************

  pingpong - Test/example program for GAUL.
  Copyright ©2001-2005, Stewart Adcock <stewart@linux-domain.com>
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

		This program aims to solve a problem proposed in:
		Dennis E. Shasha, "Dr Ecco's Omniheurist Corner: Foxy",
		Dr Dobb's Journal, 323:148-149 (2001).

		Note that this probably isn't the method of choice
		for solving this problem - it is just used as an
		illustration.

		This example uses custom crossover and mutation
		operators since a given team member is unable to
		play twice.

 **********************************************************************/

#include "pingpong.h"

/**********************************************************************
  pingpong_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	03/07/01
 **********************************************************************/

boolean pingpong_score(population *pop, entity *entity)
  {
  int		i;	/* Team member. */
  double	score=0.0, lossscore=0.0, badscore=0.0;
  int		badloss=0, loss=0;

  entity->fitness = 0;

  for (i=0; i<9; i++)
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
  pingpong_seed()
  synopsis:	Seed initial solutions.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

boolean pingpong_seed(population *pop, entity *adam)
  {
  int		i, j;	/* Team members. */
  int		*data;	/* Chromosome. */

  data = (int *)adam->chromosome[0];

  for (i=0; i<9; i++) data[i] = -1;

  for (i=0; i<9; i++)
    {
    j = random_int(9);
    while (data[j] > -1)
      {
      if (j == 8)
        j = 0;
      else
        j++;
      }

    data[j] = i;
    }

  return TRUE;
  }


/**********************************************************************
  pingpong_crossover()
  synopsis:	Crossover.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

void pingpong_crossover(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
  {
  int		i, j;	/* Team members. */

  for (i=0; i<9; i++)
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

  for (i=1; i<9; i++)
    {
    for (j=0; j<i; j++)
      {
      if (((int *)son->chromosome[0])[j] == ((int *)son->chromosome[0])[i])
        {
        if (((int *)son->chromosome[0])[i]==8)
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
        if (((int *)daughter->chromosome[0])[i]==8)
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
  pingpong_mutate_swap()
  synopsis:	Mutation by swapping two team members.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

void pingpong_mutate_swap(population *pop, entity *mother, entity *son)
  {
  int		i, j;	/* Team members. */
  int		tmp;	/* For swapping i and j. */

/* Copy chromosomes of parent to offspring. */
  memcpy( son->chromosome[0],
          mother->chromosome[0],
          pop->len_chromosomes*sizeof(int) );

  i = random_int(9);
  j = random_int(9);

  if (i==j)
    {
    if (j==8)
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
  pingpong_mutate_shift()
  synopsis:	Mutation by shifting a team member.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

void pingpong_mutate_shift(population *pop, entity *mother, entity *son)
  {
  int		i, j, k;	/* Team members. */
  int		tmp;		/* For swapping i and j. */

/* Copy chromosomes of parent to offspring. */
  memcpy( son->chromosome[0],
          mother->chromosome[0],
          pop->len_chromosomes*sizeof(int) );

  i = random_int(9);

  do
    {
    j = random_int(9);
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
  pingpong_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

void pingpong_mutate(population *pop, entity *mother, entity *son)
  {

  /* Checks. */
  if (!mother || !son) die("Null pointer to entity structure passed");

  if (random_boolean_prob(0.2))
    pingpong_mutate_swap(pop, mother, son);
  else
    pingpong_mutate_shift(pop, mother, son);

  return;
  }


/**********************************************************************
  pingpong_ga_callback()
  synopsis:	Analysis callback.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

boolean pingpong_ga_callback(int generation, population *pop)
  {
  int		i;		/* Team member. */
  int		score[9];	/* Scores. */
  int		loss=0;		/* Number of matches lost. */
  double	lossscore=0;	/* Average score in lost matches. */
  entity	*best;		/* Top ranked solution. */

  best = ga_get_entity_from_rank(pop, 0);

  for (i=0; i<9; i++)
    {
    score[i] = (((int *)best->chromosome[0])[i] - i)*4 + 2;
    if (score[i] > 0)
      {
      loss++;
      lossscore += score[i];
      }
    }
    lossscore /= loss;

    printf( "%d: %f %d %d %d %d %d %d %d %d %d\n",
            generation,
            best->fitness,
            ((int *)best->chromosome[0])[0],
            ((int *)best->chromosome[0])[1],
            ((int *)best->chromosome[0])[2],
            ((int *)best->chromosome[0])[3],
            ((int *)best->chromosome[0])[4],
            ((int *)best->chromosome[0])[5],
            ((int *)best->chromosome[0])[6],
            ((int *)best->chromosome[0])[7],
            ((int *)best->chromosome[0])[8] );

  printf( "     %d %d %d %d %d %d %d %d %d   Ave. loss = %f    wins = %d\n",
          score[0], score[1], score[2],
          score[3], score[4], score[5],
          score[6], score[7], score[8],
          lossscore, 9-loss );

  return TRUE;	/* If this was to return FALSE, then the GA would terminate. */
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	19 Aug 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;			/* Runs. */
  population	*pop=NULL;		/* Population of solutions. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */

  for (i=0; i<50; i++)
    {
    if (pop) ga_extinction(pop);

    random_seed(424242*i);

    pop = ga_genesis_integer(
       50,			/* const int              population_size */
       1,			/* const int              num_chromo */
       9,			/* const int              len_chromo */
NULL, /*pingpong_ga_callback,*/	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       pingpong_score,		/* GAevaluate             evaluate */
       pingpong_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_randomrank,	/* GAselect_one           select_one */
       ga_select_two_randomrank,	/* GAselect_two           select_two */
       pingpong_mutate,		/* GAmutate               mutate */
       pingpong_crossover,	/* GAcrossover            crossover */
       NULL,			/* GAreplace              replace */
       NULL			/* vpointer		User data */
            );

    ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
       0.5,			/* double  crossover */
       0.5,			/* double  mutation */
       0.0              	/* double  migration */
                              );

    ga_evolution(
       pop,			/* population              *pop */
       200			/* const int               max_generations */
              );

    pingpong_ga_callback(i, pop);
    }

  printf("The final solution found was:\n");
  beststring = ga_chromosome_integer_to_string(pop, ga_get_entity_from_rank(pop,0), beststring, &beststrlen);
  printf("%s\n", beststring);

  ga_extinction(pop);

  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


