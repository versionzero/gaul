/**********************************************************************
  wildfire_forked.c
 **********************************************************************

  wildfire_forked - Test/example program for GAUL.
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

		This is neraly identical to the wildfire example,
		except that it demonstrates ga_evolution_forked()
		instead.

		This program aims to solve a problem proposed in:
		Dennis E. Shasha, "Dr Ecco's Omniheurist Corner:
		Wildfires", Dr Dobb's Journal, 322: (2001).

		This example uses custom crossover and mutation
		operators since a given team member is unable to
		play twice.  The scoring function has high level of
		randomness, so this example shows how we rescore every
		entity at every generation.

  To do:	Different chromosome encoding (i.e. coordinate pairs)

 **********************************************************************/

#include "wildfire.h"

/**********************************************************************
  wildfire_simulation()
  synopsis:	Score map.
  parameters:
  return:
  updated:	11/05/01
 **********************************************************************/

double wildfire_simulation(int map[WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION], boolean show)
  {
  int		time;		/* Simulation time. */
  int		burning[WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION];	/* Map. */
  int		crews[WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION];	/* Map. */
  int		num_burnt=0;	/* Number of burnt squares. */
  int		i, j, k;	/* Loop variables. */
  int		p;		/* Selected map square. */
  int		dir;		/* Wind direction. */

  /* Start with no burnt squares. */
  for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
    burning[p] = 0;

  /* Random initial wind direction. */
  dir = random_int(4);

  /* Perform simulation. */
  for(time=0; time<WILDFIRE_TIME; time++)
    {
    /* Set fire to random squares. */
    for(i=0; i<WILDFIRE_FLASHPOINTS; i++)
      {
      p = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
      if (map[p] == 0 && burning[p] == 0) burning[p]=1;
      }

    /* Apply the wind direction changing. */
    if (WILDFIRE_PREDICTABLE_WIND == TRUE)
      {
      if (dir==3)
        dir=0;
      else
        dir++;
      }
    else if ( random_boolean_prob(WILDFIRE_WIND_CHANGE_PROB) )
      {
      dir = random_int(4);
      }

    /* Fire crews try their best. */
    /* Look for fires that can spread most easily, and extinguish them. */
    /* FIXME: Implement firecrews to take account of predictable winds. */
    /* Currently very simple: Extinguish fires that will spread the most
       during this day. */
    /* Firstly, assess the fires. */
    switch (dir)
      {
      case 0:
        /* North */
        for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
          {
          if (burning[p] > 0 && burning[p] < 2)
            {
            crews[p]=1;
            while( crews[p]<WILDFIRE_WIND_SPEED &&
                   (j=p-crews[p]*WILDFIRE_X_DIMENSION)>0 &&
                   map[j] == 0 &&
                   burning[j] == 0 )
              {
              crews[p]++;
              }
            }
          else
            {
            crews[p]=0;
            }
          }
        break;
      case 1:
        /* East */
        for(p=WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION-1; p>=0; p--)
          {
          if (burning[p] > 0 && burning[p] < 2)
            {
            crews[p]=1;
            while( crews[p]<WILDFIRE_WIND_SPEED &&
                   (j=p+crews[p])%WILDFIRE_X_DIMENSION<(WILDFIRE_X_DIMENSION-1) &&
                   map[j] == 0 &&
                   burning[j] == 0 )
              {
              crews[p]++;
              }
            }
          else
            {
            crews[p]=0;
            }
          }
        break;
      case 2:
        /* South */
        for(p=WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION-1; p>=0; p--)
          {
          if (burning[p] > 0 && burning[p] < 2)
            {
            crews[p]=1;
            while( crews[p]<WILDFIRE_WIND_SPEED &&
                   (j=p+crews[p]*WILDFIRE_X_DIMENSION)<(WILDFIRE_Y_DIMENSION-1) &&
                   map[j] == 0 &&
                   burning[j] == 0 )
              {
              crews[p]++;
              }
            }
          else
            {
            crews[p]=0;
            }
          }
        break;
      case 3:
        /* West */
        for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
          {
          if (burning[p] > 0 && burning[p] < 2)
            {
            crews[p]=1;
            while( crews[p]<WILDFIRE_WIND_SPEED &&
                   (j=p-crews[p])%WILDFIRE_X_DIMENSION>0 &&
                   map[j] == 0 &&
                   burning[j] == 0 )
              {
              crews[p]++;
              }
            }
          else
            {
            crews[p]=0;
            }
          }
        break;
      }

    /* Assign the firecrews. */
    k = WILDFIRE_CREWS;
    j = WILDFIRE_WIND_SPEED;
    while (j>0 && k>0)
      {
      for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION && k>0; p++)
        {
        if (crews[p]==j)
          {
          crews[p]=-1;
          burning[p]=0;
          k--;
          }
        }
      j--;
      }

    /* Age fires. */
    for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
      {
      if (burning[p] > 0 ) burning[p]++;
      }

    for (k=0; k<WILDFIRE_WIND_SPEED; k++)
      {
      /* Spread fires. */
      switch (dir)
        {
        case 0:
          /* North */
          for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
            {
            if (burning[p] > 0 && burning[p] < 3)
              {
              if (p>WILDFIRE_X_DIMENSION)
                if ( map[p-WILDFIRE_X_DIMENSION] == 0 &&
                     burning[p-WILDFIRE_X_DIMENSION] == 0 &&
                     crews[p-WILDFIRE_X_DIMENSION] != -1 )
                  burning[p-WILDFIRE_X_DIMENSION]=1;
              }
            }
          break;
        case 1:
          /* East */
          for(p=WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION-1; p>=0; p--)
            {
            if (burning[p] > 0 && burning[p] < 3)
              {
              if (p%WILDFIRE_X_DIMENSION<(WILDFIRE_X_DIMENSION-1))
                if ( map[p+1] == 0 &&
                     burning[p+1] == 0 &&
                     crews[p+1] != -1 )
                  burning[p+1]=1;
              }
            }
          break;
        case 2:
          /* South */
          for(p=WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION-1; p>=0; p--)
            {
            if (burning[p] > 0 && burning[p] < 3)
              {
              if (p<WILDFIRE_X_DIMENSION*(WILDFIRE_Y_DIMENSION-1))
                if ( map[p+WILDFIRE_X_DIMENSION] == 0 &&
                     burning[p+WILDFIRE_X_DIMENSION] == 0 &&
                     crews[p+WILDFIRE_X_DIMENSION] != -1 )
                  burning[p+WILDFIRE_X_DIMENSION]=1;
              }
            }
          break;
        case 3:
          /* West */
          for(p=0; p<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; p++)
            {
            if (burning[p] > 0 && burning[p] < 3)
              {
              if (p%WILDFIRE_X_DIMENSION>0)
                if ( map[p-1] == 0 &&
                     burning[p-1] == 0 &&
                     crews[p-1] != -1 )
                  burning[p-1]=1;
              }
            }
          break;
        }
      }
    }

  /* Count number of burnt squares. */
  for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
    if (burning[i]!=0) num_burnt++;

  if (show)
    {
    printf("Map after simulation is:\n");
    for (i=0; i<WILDFIRE_Y_DIMENSION; i++)
      {
      for (k=0; k<WILDFIRE_X_DIMENSION; k++)
        {
        printf( "%s ", map[i*WILDFIRE_X_DIMENSION+k]?"o":
                       ((burning[i*WILDFIRE_X_DIMENSION+k]==0)?
                       ((crews[i*WILDFIRE_X_DIMENSION+k]==-1)?"*":"."):"x") );
        }
      printf("\n");
      }
    }
  
  return num_burnt;
  }


/**********************************************************************
  wildfire_score()
  synopsis:	Score solution.  (i.e. See how this arrangement of
		cisterns does.)
  parameters:	population *pop
		entity *entity
  return:	Always TRUE
  updated:	23 May 2002
 **********************************************************************/

boolean wildfire_score(population *pop, entity *entity)
  {
  int		i;		/* Map square. */
  int		s;		/* Loop over simulations. */
  int		map[WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION];	/* Map. */
  int		count=0;	/* Number of cisterns. */
  double	fitness;	/* Solution's "fitness". */

  /* Decode chromsome, and count number of cisterns. */
  for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
    {
    map[i] = ((int *)entity->chromosome[0])[i];
    if (map[i]) count++;
    }

  /* Penalise an incorrect number of cisterns. */
  fitness = -0.01*SQU(WILDFIRE_CISTERNS-count);

  for(s=0; s<WILDFIRE_NUM_SIMULATIONS; s++)
    {
    fitness -= wildfire_simulation(map, FALSE);
    }

  /* See how the arrangement of cisterns does. */
  fitness /= WILDFIRE_NUM_SIMULATIONS;

  ga_entity_set_fitness(entity, fitness);

  fprintf(stderr, "#");
  
  return TRUE;
  }


/**********************************************************************
  wildfire_seed()
  synopsis:	Seed initial solutions by random placement
		of cisterns 50% of the time and regular placement
		for the remainder.
  parameters:
  return:
  updated:	16 Aug 2002
 **********************************************************************/

boolean wildfire_seed(population *pop, entity *adam)
  {
  int		i, j, k;	/* Map square. */

  if (random_boolean())
    {
    for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
      {
      ((int *)adam->chromosome[0])[i] = random_boolean_prob((double)WILDFIRE_CISTERNS/
                 ((double)WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION));
      }
    }
  else
    {
    for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
      {
      ((int *)adam->chromosome[0])[i] = 0;
      }

    /* Deliberately places slightly fewer cisterns than allowed. */
    i = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
    j = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
    k = WILDFIRE_CISTERNS;
    while (k>0)
      {
      i = (i+j)%(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
      if (((int *)adam->chromosome[0])[i] == 1)
        {
        j = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
        }
      else
        {
        ((int *)adam->chromosome[0])[i] = 1;
        }
      k--;
      }
    }

  return TRUE;
  }


#if 0
/**********************************************************************
  wildfire_mutate_move()
  synopsis:	Mutation by moving a cistern.
  parameters:
  return:
  updated:	12/05/01
 **********************************************************************/

void wildfire_mutate_move(population *pop, entity *mother, entity *son)
  {
  int		i;	/* Map square. */

  /* Copy chromosome. */
  for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
    ((int *)son->chromosome[0])[i] = ((int *)mother->chromosome[0])[i];

  /* Mutation. */
  i = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
  ((int *)son->chromosome[0])[i] = !((int *)son->chromosome[0])[i];

  return;
  }
#endif


/**********************************************************************
  wildfire_mutate_flip()
  synopsis:	Mutation by flipping a cistern.
  parameters:
  return:
  updated:	11/05/01
 **********************************************************************/

void wildfire_mutate_flip(population *pop, entity *mother, entity *son)
  {
  int		i;	/* Map square. */

  /* Copy chromosome. */
  for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
    ((int *)son->chromosome[0])[i] = ((int *)mother->chromosome[0])[i];

  /* Mutation. */
  i = random_int(WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION);
  ((int *)son->chromosome[0])[i] = !((int *)son->chromosome[0])[i];

  return;
  }


#if 0
/**********************************************************************
  wildfire_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	11/05/01
 **********************************************************************/

void wildfire_mutate(population *pop, entity *mother, entity *son)
  {

  /* Checks. */
  if (!mother || !son) die("Null pointer to entity structure passed");

  if (random_boolean_prob(0.2))
    wildfire_mutate_swap(pop, mother, son);
  else
    wildfire_mutate_shift(pop, mother, son);

  return;
  }
#endif


/**********************************************************************
  wildfire_crossover()
  synopsis:	Crossover map.
  parameters:
  return:
  updated:	12/05/01
 **********************************************************************/

void wildfire_crossover( population *pop,
                         entity *mother, entity *father,
                         entity *daughter, entity *son )
  {
  int	i, j;		/* Loop over map squares. */
  int	split;		/* Crossover locus. */

  /* Checks. */
  if (!mother || !father) die("Null pointer to entity structure passed.");
  if (!daughter || !son) die("Null pointer to entity structure passed.");

  if (random_boolean())
    {	/* Vertical split. */
    split = random_int(WILDFIRE_X_DIMENSION);
    for (i=0; i<=split; i++)
      {
      for (j=0; j<WILDFIRE_Y_DIMENSION; j++)
        {
        ((int *)son->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)mother->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        ((int *)daughter->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)father->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        }
      }
    for (; i<WILDFIRE_X_DIMENSION; i++)
      {
      for (j=0; j<WILDFIRE_Y_DIMENSION; j++)
        {
        ((int *)daughter->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)mother->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        ((int *)son->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)father->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        }
      }
    }
  else
    {	/* Horizontal split. */
    split = random_int(WILDFIRE_Y_DIMENSION);
    for (j=0; j<=split; j++)
      {
      for (i=0; i<WILDFIRE_X_DIMENSION; i++)
        {
        ((int *)son->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)mother->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        ((int *)daughter->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)father->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        }
      }
    for (; j<WILDFIRE_Y_DIMENSION; j++)
      {
      for (i=0; i<WILDFIRE_X_DIMENSION; i++)
        {
        ((int *)daughter->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)mother->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        ((int *)son->chromosome[0])[j*WILDFIRE_X_DIMENSION+i] =
           ((int *)father->chromosome[0])[j*WILDFIRE_X_DIMENSION+i];
        }
      }
    }

  return;
  }


/**********************************************************************
  wildfire_ga_callback()
  synopsis:	Rescore all entities.  This is needed since the
		scores depend upon the wind directions and random
		lightning strikes.
  parameters:
  return:
  updated:	23 May 2002
 **********************************************************************/

boolean wildfire_ga_callback(int generation, population *pop)
  {
  double	average, stddev;	/* Statistics. */

  fprintf(stderr, "%f\n", ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)));

  if (generation > 0)
    {
    ga_population_score_and_sort(pop);
    ga_fitness_mean_stddev(pop, &average, &stddev);
    printf( "%d: Best %d Average %f Stddev %f\n",
            generation, (int) ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)),
            average, stddev );
    }
  else
    {
    printf( "Best random solution has score %d\n",
            (int) ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)) );
    }

  return TRUE;	/* Always TRUE, so search doesn't finish. */
  }


/**********************************************************************
  main()
  synopsis:	A GAUL example.
  parameters:
  return:
  updated:	03 Oct 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;		/* Runs. */
  int		j;		/* Loop variable. */
  population	*pop=NULL;	/* Population of solutions. */
  int		map[WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION];	/* Map. */
  int		count=0;	/* Number of cisterns. */

  random_seed(23091975);

  pop = ga_genesis_int(
       100,			/* const int              population_size */
       1,			/* const int              num_chromo */
       WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION,/* const int      len_chromo */
       wildfire_ga_callback,	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       wildfire_score,		/* GAevaluate             evaluate */
       wildfire_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       ga_select_one_roulette_rebased,	/* GAselect_one           select_one */
       ga_select_two_roulette_rebased,	/* GAselect_two           select_two */
       wildfire_mutate_flip,	/* GAmutate               mutate */
       wildfire_crossover,	/* GAcrossover   crossover */
       NULL,			/* GAreplace     replace */
       NULL			/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,		/* const ga_scheme_type    scheme */
       GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
       0.8,			/* double  crossover */
       0.2,			/* double  mutation */
       0.0              	/* double  migration */
                              );

  ga_evolution_forked(
       pop,		/* population              *pop */
       250		/* const int               max_generations */
              );

  printf( "Best solution, with score %d, was:\n",
          (int) ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)) );
  /* Decode chromsome, and count number of cisterns. */
  for(i=0; i<WILDFIRE_X_DIMENSION*WILDFIRE_Y_DIMENSION; i++)
    {
    map[i] = ((int *)ga_get_entity_from_rank(pop,0)->chromosome[0])[i];
    if (map[i]) count++;
    }
  printf("%d cisterns\n", count);

  for (i=0; i<WILDFIRE_Y_DIMENSION; i++)
    {
    for (j=0; j<WILDFIRE_X_DIMENSION; j++)
      {
      printf("%s ", ((int *)ga_get_entity_from_rank(pop,0)->chromosome[0])[i*WILDFIRE_X_DIMENSION+j]?"X":"-");
      }
    printf("\n");
    }

  wildfire_simulation(map, TRUE);
  printf("\n");
  wildfire_simulation(map, TRUE);
  printf("\n");
  wildfire_simulation(map, TRUE);
  printf("\n");
  wildfire_simulation(map, TRUE);
  printf("\n");

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


