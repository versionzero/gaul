/**********************************************************************
  ga_optim.c
 **********************************************************************

  ga_optim - Gene-based optimisation routines.
  Copyright �2000-2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:     Routines for gene-based optimisation.

  To do:	Rewrite parallel versions, ga_evolution_mp() in particular.
		Temperatures should be double-precision floats?
		Need to fix elitism/crowding stuff.

 **********************************************************************/

#include "ga_core.h"

/**********************************************************************
  ga_evolution()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

boolean ga_evolution(	population		*pop,
			const ga_class_type	class,
			const ga_elitism_type	elitism,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  boolean	finished;		/* Whether crossover/mutation rounds are complete. */
  int		new_pop_size;		/* Population size prior to adaptation. */
  double	elitism_penalty;	/* Penalty for maintaining diversity. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */
#endif

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (class != GA_CLASS_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);
#endif

/*
 * Score and sort the initial population members.
 */
  ga_population_score_and_sort(pop);

  plog( LOG_VERBOSE,
        "Prior to the first generation, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/* Do all the generations: */
  while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
           generation<max_generations )
    {
    generation++;
    pop->orig_size = pop->size;

    plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Zero statistics.
 */
#if GA_WRITE_STATS==TRUE
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;
#endif

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mating cycle ***");

    pop->select_state = 0;

    finished = FALSE;
    while (!finished)
      {
      finished = pop->select_two(pop, &mother, &father);

      if (mother && father)
        {
        plog(LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness,
             ga_get_entity_id(pop, father),
             ga_get_entity_rank(pop, father), father->fitness);

        son = ga_get_free_entity(pop);
        daughter = ga_get_free_entity(pop);
        pop->crossover(pop, mother, father, daughter, son);
        pop->evaluate(pop, daughter);
        pop->evaluate(pop, son);

/*
 * Collate stats.
 */
#if GA_WRITE_STATS==TRUE
        if (son->fitness > father->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (daughter->fitness > father->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (son->fitness > mother->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (daughter->fitness > mother->fitness)
          crossover_good++;
        else
          crossover_poor++;

        if (son->fitness > MAX(mother->fitness,father->fitness))
          crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
        if (daughter->fitness > MAX(mother->fitness,father->fitness))
          crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);
#endif
        }
      else
        {
        plog( LOG_VERBOSE, "Crossover not performed." );
        }
      }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mutation cycle ***");

    pop->select_state = 0;

    finished = FALSE;
    while (!finished)
      {
      finished = pop->select_one(pop, &mother);

      if (mother)
        {
        plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness );

        daughter = ga_get_free_entity(pop);
        pop->mutate(pop, mother, daughter);
        pop->evaluate(pop, daughter);

/*
 * Collate stats.
 */
#if GA_WRITE_STATS==TRUE
        if (daughter->fitness > mother->fitness)
          {
          mutation_good++;
          mutation_gain += daughter->fitness-mother->fitness;
          }
        else
          {
          mutation_poor++;
          }
#endif

        }
      else
        {
        plog( LOG_VERBOSE, "Mutation not performed." );
        }
      }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Lamarckian evolution.
 * Performed, and genes are modified, in the case of Baldwinian evolution.
 *
 * Maybe, could reoptimise all structures at each generation.  This would allow
 * a reduced optimisation protocol and only those structures which are
 * reasonable would survive for further optimisation.
 */
  if (class != GA_CLASS_DARWIN)
    {
    plog(LOG_VERBOSE, "*** Adaptation round ***");

    new_pop_size = pop->size;

    switch (class)
      {
      case (GA_CLASS_BALDWIN):
        /* Baldwinian evolution for children only. */
        for (i=pop->orig_size; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_BALDWIN_ALL):
        /* Baldwinian evolution for entire population. */
        /* I don't recommend this, but it is here for completeness. */
        for (i=0; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_LAMARCK):
        /* Lamarckian evolution for children only. */
        while (new_pop_size>pop->orig_size)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_LAMARCK_ALL):
        /* Lamarckian evolution for entire population. */
        while (new_pop_size>0)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      default:
        dief("Unknown adaptation class %d.\n", class);
      }
    }

/*
 * Need to kill parents?
 */
    if (elitism == GA_ELITISM_PARENTS_DIE)
      {
      while (pop->orig_size>0)
        {
        ga_entity_dereference_by_rank(pop, pop->orig_size);
        pop->orig_size--;
        }
      }

/*
 * Sort all population members by fitness.
 */
    plog(LOG_VERBOSE, "*** Sorting ***");

    quicksort_population(pop);

/*
 * Enforce the type of elitism desired.
 *
 * Rough elitism doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 */
    if (elitism == GA_ELITISM_EXACT || elitism == GA_ELITISM_ROUGH)
      {	/* Fatal version */
      i = 1;

      while (i<pop->size && i<pop->stable_size)
        {
        if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
            (elitism != GA_ELITISM_EXACT ||
             ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
          {
          ga_entity_dereference_by_rank(pop, i);
          }
        else
          {
          i++;
          }
        }
      }
    else if (elitism == GA_ELITISM_EXACT_COMP || elitism == GA_ELITISM_ROUGH_COMP)
      {	/* Increased competition version */
      i = MIN(pop->size, pop->stable_size);
      elitism_penalty = fabs(pop->entity_iarray[0]->fitness*GA_ELITISM_MULTIPLIER)
                        + GA_ELITISM_CONSTANT;

      while (i>0)
        {
        if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
            (elitism != GA_ELITISM_EXACT_COMP ||
             ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
          { 
          pop->entity_iarray[i]->fitness -= elitism_penalty;
          }
        i--;
        }

      plog(LOG_VERBOSE, "*** Sorting again ***");

      quicksort_population(pop);	/* FIXME: We could possibly (certianly) choose
					 a more optimal sort algorithm here. */
      }

/*
 * Least fit population members die to restore the
 * population size to the stable size.
 */
    plog(LOG_VERBOSE, "*** Survival of the fittest ***");

    ga_genocide(pop, pop->stable_size);

/*
 * Use callback.
 */
    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After generation %d, population has fitness scores between %f and %f",
          generation,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", generation,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);
#endif
    }	/* Generation loop. */

  return (generation<max_generations);
  }


/**********************************************************************
  ga_evolution_steady_state()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a steady-state GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

boolean ga_evolution_steady_state(	population		*pop,
					const ga_class_type	class,
					const int		max_iterations )
  {
  int		iteration=0;		/* Current iteration count. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter, *child;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  int		new_pop_size;		/* Population size prior to adaptation. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */
#endif

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (!pop->replace) die("Population's replacement callback is undefined.");
  if (class != GA_CLASS_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);
#endif

/*
 * Score and sort the initial population members.
 */
  ga_population_score_and_sort(pop);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/* Do all the iterations: */
  while ( (pop->generation_hook?pop->generation_hook(iteration, pop):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;
    pop->orig_size = pop->size;

    son = NULL;
    daughter = NULL;
    child = NULL;

    plog(LOG_DEBUG,
              "Population size is %d at start of iteration %d",
              pop->orig_size, iteration );

/*
 * Zero statistics.
 */
#if GA_WRITE_STATS==TRUE
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;
#endif

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mating ***");

    pop->select_state = 0;

    pop->select_two(pop, &mother, &father);

    if (mother && father)
      {
      plog(LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness,
             ga_get_entity_id(pop, father),
             ga_get_entity_rank(pop, father), father->fitness);

      son = ga_get_free_entity(pop);
      daughter = ga_get_free_entity(pop);
      pop->crossover(pop, mother, father, daughter, son);
      pop->evaluate(pop, daughter);
      pop->evaluate(pop, son);

/*
 * Collate stats.
 */
#if GA_WRITE_STATS==TRUE
      if (son->fitness > father->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (daughter->fitness > father->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (son->fitness > mother->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (daughter->fitness > mother->fitness)
        crossover_good++;
      else
        crossover_poor++;

      if (son->fitness > MAX(mother->fitness,father->fitness))
        crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
      if (daughter->fitness > MAX(mother->fitness,father->fitness))
        crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);
#endif
      }
    else
      {
      plog( LOG_VERBOSE, "Crossover not performed." );
      }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mutation ***");

    pop->select_state = 0;

    pop->select_one(pop, &mother);

    if (mother)
      {
      plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness );

      child = ga_get_free_entity(pop);
      pop->mutate(pop, mother, child);
      pop->evaluate(pop, child);

/*
 * Collate stats.
 */
#if GA_WRITE_STATS==TRUE
      if (child->fitness > mother->fitness)
        {
        mutation_good++;
        mutation_gain += child->fitness-mother->fitness;
        }
      else
        {
        mutation_poor++;
        }
#endif

      }
    else
      {
      plog( LOG_VERBOSE, "Mutation not performed." );
      }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Lamarckian evolution.
 * Performed, and genes are modified, in the case of Baldwinian evolution.
 *
 * Maybe, could reoptimise all structures at each generation.  This would allow
 * a reduced optimisation protocol and only those structures which are
 * reasonable would survive for further optimisation.
 */
  if (class != GA_CLASS_DARWIN)
    {
    plog(LOG_VERBOSE, "*** Adaptation ***");

    new_pop_size = pop->size;

    switch (class)
      {
      case (GA_CLASS_BALDWIN):
        /* Baldwinian evolution for children only. */
        for (i=pop->orig_size; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_BALDWIN_ALL):
        /* Baldwinian evolution for entire population. */
        /* I don't recommend this, but it is here for completeness. */
        for (i=0; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_LAMARCK):
        /* Lamarckian evolution for children only. */
        while (new_pop_size>pop->orig_size)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_CLASS_LAMARCK_ALL):
        /* Lamarckian evolution for entire population. */
        while (new_pop_size>0)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      default:
        dief("Unknown adaptation class %d.\n", class);
      }
    }

/*
 * Insert new entities into population.
 */
    if (son) pop->replace(pop, son);
    if (daughter) pop->replace(pop, daughter);
    if (child) pop->replace(pop, child);

/*
 * Use callback.
 */
    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After iteration %d, population has fitness scores between %f and %f",
          iteration,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", iteration,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);
#endif
    }	/* Iteration loop. */

  return (iteration<max_iterations);
  }


/**********************************************************************
  ga_random_mutation_hill_climbing()
  synopsis:	Perform equivalent to zero temperature metropolis
		optimisation.  If initial solution is NULL, then a
		random initial solution is generated.
		Optionally syncronises with the other processors every
		iteration and grabs the overall best solution.
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	18/12/00
 **********************************************************************/

entity *ga_random_mutation_hill_climbing(	population	*pop,
						entity		*initial,
						const int	max_iterations)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new, *temp;	/* The solutions. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

/* Do we need to generate a random solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform RMHC optimisation with random starting solution.");

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform RMHC optimisation with specified starting solution.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "rmhc_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Random Mutation Hill Climbing\n");
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG,
       "Prior to the scoring, the solution has fitness score of %f",
       best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

    plog( LOG_VERBOSE,
          "Prior to the iteration %d, the solution has fitness score of %f",
          iteration, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
         ga_get_entity_id(pop, current),
         ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

    if (best->fitness < current->fitness)
      {
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f\n", iteration, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog( LOG_VERBOSE,
        "After final iteration, the solution has fitness score of %f",
        current->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);

  return best;
  }


/**********************************************************************
  ga_next_ascent_hill_climbing()
  synopsis:	Perform systematic ascent hill climbing optimisation.
		(Needn't nessecarily use next bit each time, but this
		was the simplist to implement)
		If initial solution is NULL, then a randomly generated
		initial solution is generated.
		Optionally syncronises with the other processors every
		iteration and grabs the overall best solution.
		The original entity will not be munged.
		NOTE: Needs to be passed an 'extended' mutation
		function.
		max_iterations refers to the number of _complete_
		cycles.
  parameters:
  return:	Best solution found.
  last updated:	21/12/00
 **********************************************************************/

entity *ga_next_ascent_hill_climbing(	population		*pop,
					entity			*initial,
					const int		max_iterations,
					GAspecificmutate	mutationfunc)
  {
  int		iteration=0;		/* Current iteration number. */
  entity	*current, *best;	/* The solutions. */
  int		chromo=0, point=0;	/* Mutation locus. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
#endif

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !mutationfunc ) die("Mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "NAHC algorithm is not parallelised.");

/* Do we need to generate a random solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform NAHC optimisation with random starting solution.");

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform NAHC optimisation with specified starting solution.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "nahc_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Next Ascent Hill Climbing\n");
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {

    plog( LOG_VERBOSE,
          "Iteration %d chromosome %d, point %d, solution has fitness score of %f",
          iteration, chromo, point,
          current->fitness );

    mutationfunc(chromo, point, current->chromosome[chromo]);

    ga_entity_clear_data(pop, current, chromo);	/* Required to force regeneration of structural data. */
    pop->evaluate(pop, current);

/*
 * Is current better than best?
 */
    if (best->fitness < current->fitness)
      {
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f (%d %d)\n", iteration, best->fitness, chromo, point);
    fclose(STATS_OUT);
#endif

/*
 * Choose next nucleotide to mutate/optimise.
 */
    point++;
    if (point == pop->len_chromosomes)
      {
      point = 0;
      chromo++;
      if (chromo == pop->num_chromosomes)
        {
        chromo=0;
        iteration++;
        }
      }
    }

  plog( LOG_VERBOSE,
        "After final iteration, the solution has fitness score of %f",
        current->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);

  return best;
  }


/**********************************************************************
  ga_metropolis_mutation()
  synopsis:	Perform arbitrary temperature metropolis optimisation.
		If initial solution is NULL, then random solution is
		generated.  Syncs with other processors every iteration
		and grabs the overall best solution if better than
		this processors current solution is worse than the best
		plus the temperature. 
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	19/01/01
 **********************************************************************/

entity *ga_metropolis_mutation(	population		*pop,
				entity			*initial,
				const int		max_iterations,
				const int 		temperature)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new;		/* The solutions. */
  entity	*temp=NULL;			/* Used for swapping current and new. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !pop->mutate ) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "Metropolis algorithm is not parallelised.");

/* Do we need to generate a random solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform metropolis optimisation at %d degrees with random starting solution.", temperature);

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform metropolis optimisation at %d degrees.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "mstats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Metropolis optimisation at %d degrees.\n", temperature);
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

    plog(LOG_VERBOSE,
              "Prior to iteration %d, solution has fitness score of %f",
              iteration, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
         ga_get_entity_id(pop, current),
         ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

/*
 * Should we keep this solution?
 */
    if ( best->fitness < current->fitness ||
         random_boolean_prob(exp((current->fitness-best->fitness)
                                   /(GA_BOLTZMANN_FACTOR*temperature))) )
      {
/*        plog(LOG_DEBUG, "Selecting new solution."); */
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
/*        plog(LOG_DEBUG, "Rejecting new solution."); */
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }  

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f\n", iteration, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog( LOG_VERBOSE,
        "After final iteration, solution has fitness score of %f",
        best->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);
  ga_entity_dereference(pop, temp);

  return best;
  }


/**********************************************************************
  ga_simulated_annealling_mutation()
  synopsis:	Perform mutation/SA (GA mutation/Simulated Annealling).
		If initial solution is NULL, then random solution is
		generated.  Syncs with other processors every iteration
		and grabs the overall best solution if better than
		this processors current solution is worse than the best
		plus the temperature. 
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	21/02/01
 **********************************************************************/

entity *ga_simulated_annealling_mutation(population	*pop,
					entity		*initial,
					const int	max_iterations,
					const int 	initial_temperature,
					const int 	final_temperature)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new;		/* The solutions. */
  entity	*temp=NULL;			/* Used for swapping current and new. */
  int		temperature;			/* Current temperature. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !pop->mutate ) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "Simulated annealling algorithm is not parallelised.");

/* Do we need to generate a random solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform %d steps of MC/SA optimisation between %d and %d degrees with random starting solution.", max_iterations, initial_temperature, final_temperature);

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform %d steps of MC/SA optimisation between %d and %d degrees.", max_iterations, initial_temperature, final_temperature);

    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "sastats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "MC/SA optimisation at %d to %d degrees.\n",
                     initial_temperature, final_temperature);
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    temperature = initial_temperature + ((double)iteration/max_iterations)*(final_temperature-initial_temperature);
    iteration++;

    plog( LOG_VERBOSE,
          "Prior to iteration %d temperature %d, solution has fitness score of %f",
          iteration, temperature, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
       ga_get_entity_id(pop, current),
       ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

/*
 * Should we keep this solution?
      if ( best->fitness < current->fitness ||
           random_boolean_prob(exp((current->fitness-best->fitness)
                                   /(GA_BOLTZMANN_FACTOR*temperature))) )
 */
    if ( best->fitness < current->fitness+temperature )
      { /* Copy this solution best solution. */
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      { /* Copy best solution over current solution. */
/*        plog(LOG_DEBUG, "Rejecting new solution.");*/
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: (%d degrees) %f\n",
            iteration, temperature, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog(LOG_VERBOSE,
            "After final iteration, the solution has fitness score of %f",
            best->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);
  ga_entity_dereference(pop, temp);

  return best;
  }


/**********************************************************************
  ga_evolution_archipelago()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given populations using a simple
		island model.  Migration occurs around a cyclic
		topology only.  Migration causes a duplication of the
		respective entities.  This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	08/07/01
 **********************************************************************/

boolean ga_evolution_archipelago( const int num_pops,
			population		**pops,
			const ga_class_type	class,
			const ga_elitism_type	elitism,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		island;			/* Current island number. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  boolean	finished;		/* Whether crossover/mutation rounds are complete. */
  int		new_pop_size;		/* Population size prior to adaptation. */
  double	elitism_penalty;	/* Penalty for maintaining diversity. */
  population	*pop=NULL;		/* Current population. */
  boolean	complete=FALSE;		/* Whether evolution is terminated. */
  int		pop0_osize;		/* Required for correct migration. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */
#endif

/* Checks. */
  if (!pops)
    die("NULL pointer to array of population structures passed.");
  if (num_pops<2)
    die("Need at least two populations for the island model.");

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (!pop->evaluate)
      die("Population's evaluation callback is undefined.");
    if (!pop->select_one)
      die("Population's asexual selection callback is undefined.");
    if (!pop->select_two)
      die("Population's sexual selection callback is undefined.");
    if (!pop->mutate)
      die("Population's mutation callback is undefined.");
    if (!pop->crossover)
      die("Population's crossover callback is undefined.");
    if (class != GA_CLASS_DARWIN && !pop->adapt)
      die("Population's adaption callback is undefined.");
    if (pop->size < 1)
      die("Population is empty (ga_genesis() or equivalent should be called).");

/* Set island property. */
    pop->island = island;
    }

  plog(LOG_VERBOSE, "The evolution has begun on %d islands!", num_pops);

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);
#endif

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];
/*
 * Score and sort the initial population members.
 */
    ga_population_score_and_sort(pop);
    plog( LOG_VERBOSE,
          "Prior to the first generation, population on island %d has fitness scores between %f and %f",
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );
    }

/* Do all the generations: */
  while ( generation<max_generations && complete==FALSE)
    {
    generation++;

/*
 * Zero statistics.
 */
#if GA_WRITE_STATS==TRUE
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;
#endif

/*
 * Migration Cycle.
 */
    plog( LOG_VERBOSE, "*** Migration Cycle ***" );
    pop0_osize = pops[0]->size;
    for(island=1; island<num_pops; island++)
      {
      for(i=0; i<pops[island]->size; i++)
        {
        if (random_boolean_prob(pops[island]->migration_ratio))
	  {
          ga_entity_clone(pops[island-1], pops[island]->entity_iarray[i]);
/*	  printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), island, i, pops[island]->entity_iarray[i]->fitness);*/
	  }
        }
      }
    for(i=0; i<pop0_osize; i++)
      {
      if (random_boolean_prob(pops[0]->migration_ratio))
        ga_entity_clone(pops[num_pops-1], pops[0]->entity_iarray[i]);
/*        printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[island]->entity_iarray[i]->fitness);*/
      }

    for(island=0; island<num_pops; island++)
      {
      pop = pops[island];

      plog( LOG_VERBOSE, "*** Evolution on island %d ***", island );

/*
 * Sort the individuals in each population.
 * Need this to ensure that new immigrants are ranked correctly.
 */
      quicksort_population(pop);

      if (pop->generation_hook?pop->generation_hook(generation, pop):TRUE)
        {
        pop->orig_size = pop->size;

        plog( LOG_DEBUG,
              "Population %d size is %d at start of generation %d",
              island, pop->orig_size, generation );

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
        plog(LOG_VERBOSE, "*** Mating cycle ***");

        pop->select_state = 0;

        finished = FALSE;
        while (!finished)
          {
          finished = pop->select_two(pop, &mother, &father);

          if (mother && father)
            {
            plog( LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f) on island %d",
                  ga_get_entity_id(pop, mother),
                  ga_get_entity_rank(pop, mother), mother->fitness,
                  ga_get_entity_id(pop, father),
                  ga_get_entity_rank(pop, father), father->fitness,
                  island );

            son = ga_get_free_entity(pop);
            daughter = ga_get_free_entity(pop);
            pop->crossover(pop, mother, father, daughter, son);
            pop->evaluate(pop, daughter);
            pop->evaluate(pop, son);

/* Collate stats. */
#if GA_WRITE_STATS==TRUE
            if (son->fitness > father->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (daughter->fitness > father->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (son->fitness > mother->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (daughter->fitness > mother->fitness)
              crossover_good++;
            else
              crossover_poor++;

            if (son->fitness > MAX(mother->fitness,father->fitness))
              crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
            if (daughter->fitness > MAX(mother->fitness,father->fitness))
              crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);
#endif
            }
          else
            {
            plog( LOG_VERBOSE, "Crossover not performed." );
            }
          }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
        plog(LOG_VERBOSE, "*** Mutation cycle ***");

        pop->select_state = 0;

        finished = FALSE;
        while (!finished)
          {
          finished = pop->select_one(pop, &mother);

          if (mother)
            {
            plog( LOG_VERBOSE, "Mutation of %d (%d = %f)",
                  ga_get_entity_id(pop, mother),
                  ga_get_entity_rank(pop, mother), mother->fitness );

            daughter = ga_get_free_entity(pop);
            pop->mutate(pop, mother, daughter);
            pop->evaluate(pop, daughter);

/* Collate stats. */
#if GA_WRITE_STATS==TRUE
            if (daughter->fitness > mother->fitness)
              {
              mutation_good++;
              mutation_gain += daughter->fitness-mother->fitness;
              }
            else
              {
              mutation_poor++;
              }
#endif

            }
          else
            {
            plog( LOG_VERBOSE, "Mutation not performed." );
            }
          }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Lamarckian evolution.
 * Performed, and genes are modified, in the case of Baldwinian evolution.
 *
 * Maybe, could reoptimise all structures at each generation.  This would allow
 * a reduced optimisation protocol and only those structures which are
 * reasonable would survive for further optimisation.
 */
      if (class != GA_CLASS_DARWIN)
        {
        plog(LOG_VERBOSE, "*** Adaptation round ***");

        new_pop_size = pop->size;

        switch (class)
          {
          case (GA_CLASS_BALDWIN):
            /* Baldwinian evolution for children only. */
            for (i=pop->orig_size; i<new_pop_size; i++)
              {
              adult = pop->adapt(pop, pop->entity_iarray[i]);
              pop->entity_iarray[i]->fitness=adult->fitness;
    /* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_BALDWIN_ALL):
            /* Baldwinian evolution for entire population. */
            /* I don't recommend this, but it is here for completeness. */
            for (i=0; i<new_pop_size; i++)
              {
              adult = pop->adapt(pop, pop->entity_iarray[i]);
              pop->entity_iarray[i]->fitness=adult->fitness;
    /* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_LAMARCK):
            /* Lamarckian evolution for children only. */
            while (new_pop_size>pop->orig_size)
              {
              new_pop_size--;
              adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_LAMARCK_ALL):
            /* Lamarckian evolution for entire population. */
            while (new_pop_size>0)
              {
              new_pop_size--;
              adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          default:
            dief("Unknown adaptation class %d.\n", class);
          }
        }

/*
 * Need to kill parents?
 */
        if (elitism == GA_ELITISM_PARENTS_DIE)
          {
          while (pop->orig_size>0)
            {
            ga_entity_dereference_by_rank(pop, pop->orig_size);
            pop->orig_size--;
            }
          }

/*
 * Sort all population members by fitness.
 */
        plog(LOG_VERBOSE, "*** Sorting ***");

        quicksort_population(pop);

/*
 * Enforce the type of elitism desired.
 *
 * Rough elitism doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 */
        if (elitism == GA_ELITISM_EXACT || elitism == GA_ELITISM_ROUGH)
          {	/* Fatal version */
          i = 1;

          while (i<pop->size && i<pop->stable_size)
            {
            if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
                (elitism != GA_ELITISM_EXACT ||
                 ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
              {
              ga_entity_dereference_by_rank(pop, i);
              }
            else
              {
              i++;
              }
            }
          }
        else if (elitism == GA_ELITISM_EXACT_COMP || elitism == GA_ELITISM_ROUGH_COMP)
          {	/* Increased competition version */
          i = MIN(pop->size, pop->stable_size);
          elitism_penalty = fabs(pop->entity_iarray[0]->fitness*GA_ELITISM_MULTIPLIER)
                            + GA_ELITISM_CONSTANT;

          while (i>0)
            {
            if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
                (elitism != GA_ELITISM_EXACT_COMP ||
                 ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
              { 
              pop->entity_iarray[i]->fitness -= elitism_penalty;
              }
            i--;
            }

          plog(LOG_VERBOSE, "*** Sorting again ***");

          quicksort_population(pop);
          }

/*
 * Least fit population members die to restore the
 * population size to the stable size.
 */
        plog(LOG_VERBOSE, "*** Survival of the fittest ***");

        ga_genocide(pop, pop->stable_size);
        }
      else
        {
        complete = TRUE;
        }
      }

    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After generation %d, population %d has fitness scores between %f and %f",
          generation,
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", generation,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);
#endif
    }	/* Generation loop. */

  return (generation<max_generations);
  }


/**********************************************************************
  ga_evolution_archipelago_mp()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given populations using a simple
		island model.  Migration occurs around a cyclic
		topology only.  Migration causes a duplication of the
		respective entities.  This is a generation-based GA.
		This is a multi-processor version with uses one
	       	processor for one or more islands.  Note that the
		populations must be pre-distributed.  The number of
		populations on each processor and the properties (e.g.
		size) of those populations need not be equal - but be
		careful of load-balancing issues in this case.  Safe
		to call (but slightly inefficient) in single processor
		case.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	24 Jan 2002
 **********************************************************************/

boolean ga_evolution_archipelago_mp( const int num_pops,
			population		**pops,
			const ga_class_type	class,
			const ga_elitism_type	elitism,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		island;			/* Current island number. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  boolean	finished;		/* Whether crossover/mutation rounds are complete. */
  int		new_pop_size;		/* Population size prior to adaptation. */
  double	elitism_penalty;	/* Penalty for maintaining diversity. */
  population	*pop=NULL;		/* Current population. */
  boolean	complete=FALSE;		/* Whether evolution is terminated. */
  int		pop0_osize;		/* Required for correct migration. */
  boolean	*send_mask;		/* Whether particular entities need to be sent. */
  int		send_count;		/* Number of entities to send. */
  int		max_size=0;		/* Largest maximum size of populations. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */
#endif

/* Checks. */
  if (!pops)
    die("NULL pointer to array of population structures passed.");

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (!pop->evaluate)
      die("Population's evaluation callback is undefined.");
    if (!pop->select_one)
      die("Population's asexual selection callback is undefined.");
    if (!pop->select_two)
      die("Population's sexual selection callback is undefined.");
    if (!pop->mutate)
      die("Population's mutation callback is undefined.");
    if (!pop->crossover)
      die("Population's crossover callback is undefined.");
    if (class != GA_CLASS_DARWIN && !pop->adapt)
      die("Population's adaption callback is undefined.");
    if (pop->size < 1)
      die("Population is empty (ga_genesis() or equivalent should be called).");

/* Set island property. */
    pop->island = island;
    }

  plog(LOG_VERBOSE, "The evolution has begun on %d islands on node %d!", num_pops, mpi_get_rank());

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);
#endif

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

/*
 * Score and sort the initial population members.
 */
    ga_population_score_and_sort(pop);
    plog( LOG_VERBOSE,
          "Prior to the first generation, population on island %d (process %d) has fitness scores between %f and %f",
          island, mpi_get_rank(),
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    max_size = max(max_size, pop->max_size);
    }

  /* Allocate send_mask array. */
  send_mask = s_malloc(max_size*sizeof(boolean));

/* Do all the generations: */
  while ( generation<max_generations && complete==FALSE)
    {
    generation++;

/*
 * Zero statistics.
 */
#if GA_WRITE_STATS==TRUE
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;
#endif

/*
 * Migration Cycle.
 * 1) Migration that doesn't require inter-process communication.
 * 2) Migration that requires migration from 'even' processes.
 * 3) Migration that requires migration from 'odd' processes.
 * (Special case due to odd number of nodes is okay)
 */
    plog( LOG_VERBOSE, "*** Migration Cycle ***" );
    pop0_osize = pops[0]->size;
    for(island=1; island<num_pops; island++)
      {
      for(i=0; i<pops[island]->size; i++)
        {
        if (random_boolean_prob(pops[island]->migration_ratio))
	  {
          ga_entity_clone(pops[island-1], pops[island]->entity_iarray[i]);
/*	  printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), island, i, pops[island]->entity_iarray[i]->fitness);*/
	  }
        }
      }

    if (mpi_get_num_processes()<2)
      {	/* No parallel stuff initialized, or only 1 processor. */
      if (num_pops>1)
        { /* There is more than one island. */
        for(i=0; i<pop0_osize; i++)
          {
          if (random_boolean_prob(pops[0]->migration_ratio))
            {
            ga_entity_clone(pops[num_pops-1], pops[0]->entity_iarray[i]);
/*	    printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), 0, i, pops[0]->entity_iarray[i]->fitness);*/
            }
	  }
	}
      }
    else
      {
      if (ISEVEN(mpi_get_rank()))
	{ /* Send then Recieve. */
	send_count = 0;
        for(i=0; i<pop0_osize; i++)
          {
          send_mask[i] = random_boolean_prob(pops[0]->migration_ratio);
	  send_count += send_mask[i];
/*	  if (send_mask[i]) printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[num_pops-1]->entity_iarray[i]->fitness);*/
          }

        ga_population_send_by_mask(pops[0], mpi_get_prev_rank(), send_count, send_mask);

        ga_population_append_receive(pops[num_pops-1], mpi_get_next_rank());
	}
      else
	{ /* Recieve then Send. */
        ga_population_append_receive(pops[num_pops-1], mpi_get_next_rank());

	send_count = 0;
	for(i=0; i<pop0_osize; i++)
          {
          send_mask[i] = random_boolean_prob(pops[0]->migration_ratio);
	  send_count += send_mask[i];
/*	  if (send_mask[i]) printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[num_pops-1]->entity_iarray[i]->fitness);*/
          }

        ga_population_send_by_mask(pops[0], mpi_get_prev_rank(), send_count, send_mask);
	}
      }

    for(island=0; island<num_pops; island++)
      {
      pop = pops[island];

      plog( LOG_VERBOSE, "*** Evolution on island %d ***", island );

/*
 * Sort the individuals in each population.
 * Need this to ensure that new immigrants are ranked correctly.
 * ga_population_score_and_sort(pop) is needed if scores may change during migration.
 */
      quicksort_population(pop);

      if (pop->generation_hook?pop->generation_hook(generation, pop):TRUE)
        {
        pop->orig_size = pop->size;

        plog( LOG_DEBUG,
              "Population %d size is %d at start of generation %d",
              island, pop->orig_size, generation );

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
        plog(LOG_VERBOSE, "*** Mating cycle ***");

        pop->select_state = 0;

        finished = FALSE;
        while (!finished)
          {
          finished = pop->select_two(pop, &mother, &father);

          if (mother && father)
            {
            plog( LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f) on island %d",
                  ga_get_entity_id(pop, mother),
                  ga_get_entity_rank(pop, mother), mother->fitness,
                  ga_get_entity_id(pop, father),
                  ga_get_entity_rank(pop, father), father->fitness,
                  island );

            son = ga_get_free_entity(pop);
            daughter = ga_get_free_entity(pop);
            pop->crossover(pop, mother, father, daughter, son);
            pop->evaluate(pop, daughter);
            pop->evaluate(pop, son);

/* Collate stats. */
#if GA_WRITE_STATS==TRUE
            if (son->fitness > father->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (daughter->fitness > father->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (son->fitness > mother->fitness)
              crossover_good++;
            else
              crossover_poor++;
            if (daughter->fitness > mother->fitness)
              crossover_good++;
            else
              crossover_poor++;

            if (son->fitness > MAX(mother->fitness,father->fitness))
              crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
            if (daughter->fitness > MAX(mother->fitness,father->fitness))
              crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);
#endif
            }
          else
            {
            plog( LOG_VERBOSE, "Crossover not performed." );
            }
          }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
        plog(LOG_VERBOSE, "*** Mutation cycle ***");

        pop->select_state = 0;

        finished = FALSE;
        while (!finished)
          {
          finished = pop->select_one(pop, &mother);

          if (mother)
            {
            plog( LOG_VERBOSE, "Mutation of %d (%d = %f)",
                  ga_get_entity_id(pop, mother),
                  ga_get_entity_rank(pop, mother), mother->fitness );

            daughter = ga_get_free_entity(pop);
            pop->mutate(pop, mother, daughter);
            pop->evaluate(pop, daughter);

/* Collate stats. */
#if GA_WRITE_STATS==TRUE
            if (daughter->fitness > mother->fitness)
              {
              mutation_good++;
              mutation_gain += daughter->fitness-mother->fitness;
              }
            else
              {
              mutation_poor++;
              }
#endif

            }
          else
            {
            plog( LOG_VERBOSE, "Mutation not performed." );
            }
          }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Lamarckian evolution.
 * Performed, and genes are modified, in the case of Baldwinian evolution.
 *
 * Maybe, could reoptimise all structures at each generation.  This would allow
 * a reduced optimisation protocol and only those structures which are
 * reasonable would survive for further optimisation.
 */
      if (class != GA_CLASS_DARWIN)
        {
        plog(LOG_VERBOSE, "*** Adaptation round ***");

        new_pop_size = pop->size;

        switch (class)
          {
          case (GA_CLASS_BALDWIN):
            /* Baldwinian evolution for children only. */
            for (i=pop->orig_size; i<new_pop_size; i++)
              {
              adult = pop->adapt(pop, pop->entity_iarray[i]);
              pop->entity_iarray[i]->fitness=adult->fitness;
    /* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_BALDWIN_ALL):
            /* Baldwinian evolution for entire population. */
            /* I don't recommend this, but it is here for completeness. */
            for (i=0; i<new_pop_size; i++)
              {
              adult = pop->adapt(pop, pop->entity_iarray[i]);
              pop->entity_iarray[i]->fitness=adult->fitness;
    /* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_LAMARCK):
            /* Lamarckian evolution for children only. */
            while (new_pop_size>pop->orig_size)
              {
              new_pop_size--;
              adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          case (GA_CLASS_LAMARCK_ALL):
            /* Lamarckian evolution for entire population. */
            while (new_pop_size>0)
              {
              new_pop_size--;
              adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
              ga_entity_dereference_by_rank(pop, new_pop_size);
              }
            break;
          default:
            dief("Unknown adaptation class %d.\n", class);
          }
        }

/*
 * Need to kill parents?
 */
        if (elitism == GA_ELITISM_PARENTS_DIE)
          {
          while (pop->orig_size>0)
            {
            ga_entity_dereference_by_rank(pop, pop->orig_size);
            pop->orig_size--;
            }
          }

/*
 * Sort all population members by fitness.
 */
        plog(LOG_VERBOSE, "*** Sorting ***");

        quicksort_population(pop);

/*
 * Enforce the type of elitism desired.
 *
 * Rough elitism doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 */
        if (elitism == GA_ELITISM_EXACT || elitism == GA_ELITISM_ROUGH)
          {	/* Fatal version */
          i = 1;

          while (i<pop->size && i<pop->stable_size)
            {
            if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
                (elitism != GA_ELITISM_EXACT ||
                 ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
              {
              ga_entity_dereference_by_rank(pop, i);
              }
            else
              {
              i++;
              }
            }
          }
        else if (elitism == GA_ELITISM_EXACT_COMP || elitism == GA_ELITISM_ROUGH_COMP)
          {	/* Increased competition version */
          i = MIN(pop->size, pop->stable_size);
          elitism_penalty = fabs(pop->entity_iarray[0]->fitness*GA_ELITISM_MULTIPLIER)
                            + GA_ELITISM_CONSTANT;

          while (i>0)
            {
            if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
                (elitism != GA_ELITISM_EXACT_COMP ||
                 ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
              { 
              pop->entity_iarray[i]->fitness -= elitism_penalty;
              }
            i--;
            }

          plog(LOG_VERBOSE, "*** Sorting again ***");

          quicksort_population(pop);
          }

/*
 * Least fit population members die to restore the
 * population size to the stable size.
 */
        plog(LOG_VERBOSE, "*** Survival of the fittest ***");

        ga_genocide(pop, pop->stable_size);
        }
      else
        {
        complete = TRUE;
        }
      }

    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After generation %d, population %d has fitness scores between %f and %f",
          generation,
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", generation,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);
#endif
    }	/* Generation loop. */

  /* Free the send_mask array. */
  s_free(send_mask);

  return (generation<max_generations);
  }

