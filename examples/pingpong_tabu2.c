/**********************************************************************
  pingpong_tabu2.c
 **********************************************************************

  pingpong_tabu2 - Test/example program for GAUL.
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

  Synopsis:	An improved test/example program for GAUL.

		This program aims to solve a problem proposed in:
		Dennis E. Shasha, "Dr Ecco's Omniheurist Corner: Foxy",
		Dr Dobb's Journal, 323:148-149 (2001).

		This example uses the tabu-search algorithm instead of
		a GA.

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

  return TRUE;
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
  pingpong_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	09 Oct 2002
 **********************************************************************/

void pingpong_mutate(population *pop, entity *mother, entity *son)
  {

  /* Checks. */
  if (!mother || !son) die("Null pointer to entity structure passed");

  if (random_boolean_prob(0.5))
    pingpong_mutate_swap(pop, mother, son);
  else
    pingpong_mutate_shift(pop, mother, son);

  return;
  }


/**********************************************************************
  pingpong_iteration_callback()
  synopsis:	Analysis callback.
  parameters:
  return:
  updated:	09 Oct 2002
 **********************************************************************/

boolean pingpong_iteration_callback(int iteration, entity *this)
  {
  int		i;		/* Team member. */
  int		score[25];	/* Scores. */
  int		loss=0;		/* Number of matches lost. */
  double	lossscore=0;	/* Average score in lost matches. */

  for (i=0; i<25; i++)
    {
    score[i] = (((int *)this->chromosome[0])[i] - i)*4 + 2;
    if (score[i] > 0)
      {
      loss++;
      lossscore += score[i];
      }
    }
    lossscore /= loss;

    printf( "%d: %f %d %d %d %d %d %d %d %d %d\n",
            iteration,
            this->fitness,
            ((int *)this->chromosome[0])[0],
            ((int *)this->chromosome[0])[1],
            ((int *)this->chromosome[0])[2],
            ((int *)this->chromosome[0])[3],
            ((int *)this->chromosome[0])[4],
            ((int *)this->chromosome[0])[5],
            ((int *)this->chromosome[0])[6],
            ((int *)this->chromosome[0])[7],
            ((int *)this->chromosome[0])[8] );

  printf( "     %d %d %d %d %d %d %d %d %d   Ave. loss = %f    wins = %d\n",
          score[0], score[1], score[2],
          score[3], score[4], score[5],
          score[6], score[7], score[8],
          lossscore, 25-loss );

  return TRUE;	/* If this was to return FALSE, then the search would terminate. */
  }


/**********************************************************************
  main()
  synopsis:	main function.
  parameters:
  return:
  updated:	18 Feb 2005
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;			/* Runs. */
  population	*pop=NULL;		/* Population of solutions. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */
  entity	*solution;		/* Fittest entity in population. */

  for (i=0; i<50; i++)
    {

    random_seed(230975*i);

    /* Note that most of the population data is not required for a tabu-search. */
    pop = ga_genesis_integer(
       50,			/* const int              population_size */
       1,			/* const int              num_chromo */
       25,			/* const int              len_chromo */
       NULL,			/* GAgeneration_hook      generation_hook */
#if 0
       pingpong_iteration_callback,	/* GAiteration_hook       iteration_hook */
#endif
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       pingpong_score,		/* GAevaluate             evaluate */
       pingpong_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       NULL,			/* GAselect_one           select_one */
       NULL,			/* GAselect_two           select_two */
       pingpong_mutate,		/* GAmutate               mutate */
       NULL,			/* GAcrossover            crossover */
       NULL,			/* GAreplace              replace */
       NULL			/* vpointer		User data */
            );

    /* Evaluate the initial 50 population members. */
    ga_population_seed(pop);
    ga_population_score_and_sort(pop);

    /* ga_population_set_tabu_parameters() is required instead of
     * the usual ga_population_set_parameters().
     */
    ga_population_set_tabu_parameters(
       pop,			/* population		*pop */
       ga_tabu_check_integer,	/* GAtabu_accept	tabu acceptance criterion */
       50,			/* const int		tabu list length */
       20			/* const int		tabu neighbourhood search count */
                              );

    /* ga_tabu() is called instead of ga_evolution().  We use the best of the
     * initial 50 population members.  This entity is overwritten with an
     * improved version.
     */
    solution = ga_get_entity_from_rank(pop,0);
    ga_tabu(
       pop,			/* population		*pop */
       solution,		/* entity		*initial */
       60			/* const int		max_iterations */
           );

    printf("The best solution found on attempt %d with fitness %f was:\n", i, solution->fitness);
    beststring = ga_chromosome_integer_to_string(pop, solution, beststring, &beststrlen);
    printf("%s\n", beststring);
    pingpong_iteration_callback(i, solution);	/* Just used here to write the resulting solution. */

    ga_extinction(pop);
    }

  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


