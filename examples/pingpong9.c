/**********************************************************************
  pingpong.c
 **********************************************************************

  pingpong - Test/example program for GAUL.
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

		This program aims to solve a problem proposed in:
		Dennis E. Shasha, "Dr Ecco's Omniheurist Corner: Foxy",
		Dr Dobb's Journal, 323:148-149 (2001).

  Last Updated:	23/04/01 SAA	First version (actually second due to incorrect use of 'rm').

 **********************************************************************/

#include "pingpong.h"

/**********************************************************************
  pingpong_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

boolean pingpong_score(population *pop, entity *entity)
  {
  int		i;	/* Team member. */
  double	score=0.0, lossscore=0.0, badscore=0.0;
  int		badloss=0, loss=0;

  entity->fitness = 0;

  for (i=0; i<9; i++)
    {
    score = (entity->chromosome[0][i] - i)*4 + 2;
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
  entity->fitness -= SQU(6 - lossscore) * 2.0;

/* Team should win a majority of games. */
  if (loss>4) entity->fitness -= loss * 3.0;
  
  return TRUE;
  }


/**********************************************************************
  pingpong_seed()
  synopsis:	Seed initial solutions.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

boolean pingpong_seed(int chromosome, int *data)
  {
  int		i, j;	/* Team members. */

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
      son->chromosome[0][i] = father->chromosome[0][i];
      daughter->chromosome[0][i] = mother->chromosome[0][i];
      }
   else
      {
      son->chromosome[0][i] = father->chromosome[0][i];
      daughter->chromosome[0][i] = mother->chromosome[0][i];
      }
    }

  for (i=1; i<9; i++)
    {
    for (j=0; j<i; j++)
      {
      if (son->chromosome[0][j] == son->chromosome[0][i])
        {
        if (son->chromosome[0][i]==8)
          son->chromosome[0][i]=0;
        else
          son->chromosome[0][i]++;
        j=0;
        }
      }
    for (j=0; j<i; j++)
      {
      if (daughter->chromosome[0][j] == daughter->chromosome[0][i])
        {
        if (daughter->chromosome[0][i]==8)
          daughter->chromosome[0][i]=0;
        else
          daughter->chromosome[0][i]++;
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

  tmp = son->chromosome[0][i];
  son->chromosome[0][i] = son->chromosome[0][j];
  son->chromosome[0][j] = tmp;

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
    tmp = son->chromosome[0][j];
    for (k=j; k<i; k++)
      {
      son->chromosome[0][k] = son->chromosome[0][k+1];
      }
    son->chromosome[0][i] = tmp;
    }
  else
    {
    tmp = son->chromosome[0][j];
    for (k=j; k>i; k--)
      {
      son->chromosome[0][k] = son->chromosome[0][k-1];
      }
    son->chromosome[0][i] = tmp;
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
  synopsis:	Mutation.
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

boolean pingpong_ga_callback(int generation, population *pop)
  {
  int		i;		/* Team member. */
  int		score[9];	/* Scores. */
  int		loss=0;
  double	lossscore=0;

  for (i=0; i<9; i++)
    {
    score[i] = (pop->entity_iarray[0]->chromosome[0][i] - i)*4 + 2;
    if (score[i] > 0)
      {
      loss++;
      lossscore += score[i];
      }
    }
    lossscore /= loss;

    printf( "%d: %f %d %d %d %d %d %d %d %d %d\n",
            generation,
            pop->entity_iarray[0]->fitness,
            pop->entity_iarray[0]->chromosome[0][0],
            pop->entity_iarray[0]->chromosome[0][1],
            pop->entity_iarray[0]->chromosome[0][2],
            pop->entity_iarray[0]->chromosome[0][3],
            pop->entity_iarray[0]->chromosome[0][4],
            pop->entity_iarray[0]->chromosome[0][5],
            pop->entity_iarray[0]->chromosome[0][6],
            pop->entity_iarray[0]->chromosome[0][7],
            pop->entity_iarray[0]->chromosome[0][8] );

  printf( "     %d %d %d %d %d %d %d %d %d   Ave. loss = %f    wins = %d\n",
          score[0], score[1], score[2],
          score[3], score[4], score[5],
          score[6], score[7], score[8],
          lossscore, 9-loss );

#if 0
  for (i=0; i<9; i++)
    {
    score[i] = (pop->entity_iarray[0]->chromosome[0][i] - i)*4 + 2;

    printf( "%d: %f %d %d %d %d %d %d %d %d %d\n",
            generation,
            pop->entity_iarray[i]->fitness,
            pop->entity_iarray[i]->chromosome[0][0],
            pop->entity_iarray[i]->chromosome[0][1],
            pop->entity_iarray[i]->chromosome[0][2],
            pop->entity_iarray[i]->chromosome[0][3],
            pop->entity_iarray[i]->chromosome[0][4],
            pop->entity_iarray[i]->chromosome[0][5],
            pop->entity_iarray[i]->chromosome[0][6],
            pop->entity_iarray[i]->chromosome[0][7],
            pop->entity_iarray[i]->chromosome[0][8] );
    }

  printf( "     %d %d %d %d %d %d %d %d %d\n",
          score[0], score[1], score[2],
          score[3], score[4], score[5],
          score[6], score[7], score[8] );
#endif

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	23/04/01
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop;		/* Population of solutions. */

  random_init();

  pop = ga_genesis_int(
       2000,			/* const int              population_size */
       1,			/* const int              num_chromo */
       9,			/* const int              len_chromo */
       NULL, 			/* const char             *fname */
       pingpong_ga_callback,	/* GAgeneration_hook      generation_hook */
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
       0.0      	        /* double  migration */
                              );

  ga_evolution(
       pop,			/* population              *pop */
       20			/* const int               max_generations */
              );

  exit(EXIT_SUCCESS);
  }


