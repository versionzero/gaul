/**********************************************************************
  ga_simplex.c
 **********************************************************************

  ga_simplex - A simplex search algorithm for comparison and local search.
  Copyright ©2002-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     A simplex search algorithm for comparison and local search.

		Note that, this algorithm requires that chromosomes
		may be reversibly mapped to arrays of double-precision
		floating-point array chromsomes.  If this is not
		possible then, hmmm, tough luck.

		You might want to think carefully about your convergence
		criteria.

  References:	Press, Flannery, Teukolsky, and Vetterling, 
		"Numerical Recipes in C:  The Art of Scientific Computing"
		Cambridge University Press, 2nd edition (1992) pp. 408-412.

		Nelder, J.A., and Mead, R. Computer Journal, 7:308-313 (1965)

		Yarbro, L.A., and Deming, S.N. Analytica Chim. Acta,
		73:391-398 (1974)

  To do:	Make alpha, beta and gamma parameters.

 **********************************************************************/

#include "gaul/ga_simplex.h"

/**********************************************************************
  ga_population_set_simplex_parameters()
  synopsis:     Sets the simplex-search parameters for a population.
  parameters:	population *pop		Population to set parameters of.
		const GAto_double	Map chromosomal data to array of doubles.
		const GAfrom_double	Map array of doubles to chromosomal data.
		const int		Number of dimensions for double array (Needn't match dimensions of chromosome.)
  return:	none
  last updated: 30 Oct 2002
 **********************************************************************/

void ga_population_set_simplex_parameters( population		*pop,
                                        const GAto_double	to_double,
                                        const GAfrom_double	from_double,
					const int		dimensions)
  {

  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !to_double ) die("Null pointer to GAto_double callback passed.");
  if ( !from_double ) die("Null pointer to GAfrom_double callback passed.");

  plog( LOG_VERBOSE, "Population's simplex-search parameters set" );

  if (pop->simplex_params == NULL)
    pop->simplex_params = s_malloc(sizeof(ga_simplex_t));

  pop->simplex_params->to_double = to_double;
  pop->simplex_params->from_double = from_double;
  pop->simplex_params->dimensions = dimensions;

  return;
  }


/**********************************************************************
  ga_simplex()
  synopsis:	Performs optimisation on the passed entity by using a
  		simplistic simplex-search.  The fitness evaluations
		are performed using the standard and evaluation
		callback mechanism.
		The passed entity will have its data overwritten.  The
		remainder of the population will be let untouched.
		Note that it is safe to pass a NULL initial structure,
		in which case a random starting structure will be
		generated, however the final solution will not be
		available to the caller in any obvious way.
  parameters:
  return:
  last updated:	25 Nov 2002
 **********************************************************************/

int ga_simplex(	population		*pop,
		entity			*initial,
		const int		max_iterations )
  {
  int		iteration=0;		/* Current iteration number. */
  int		i, j;			/* Index into putative solution array. */
  entity	**putative;		/* Current working solutions. */
  entity	*new, *new2;		/* New putative solutions. */
  entity	*tmpentity;		/* Used to swap working solutions. */
  double	*tmpdoubleptr;		/* Used to swap working solutions. */
  int		num_points;		/* Number of search points. */
  double	**putative_d, *putative_d_buffer;	/* Storage for double arrays. */
  double	*average;		/* Vector average of solutions. */
  double	*new_d, *new2_d;	/* New putative solutions. */
  int           first=0, last;		/* Indices into solution arrays. */
  boolean       done=FALSE;		/* Whether the shuffle sort is complete. */
  boolean	did_replace;		/* Whether worst solution was replaced. */
  boolean	restart_needed;		/* Whether the search needs restarting. */

/* Make these parameters: */
  double alpha = 1.0;	/* range: 0=no extrap, 1=unit step extrap, higher OK. */
  double beta = 0.5;	/* range: 0=no contraction, 1=full contraction. */
  double gamma = 0.5;	/* range: 0=no contraction, 1=full contraction. */
  double step = 0.5;	/* range: >0, 1=unit step randomisation, higher OK. */

/*
 * Checks.
 */
  if (!pop) die("NULL pointer to population structure passed.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->simplex_params) die("ga_population_set_simplex_params(), or similar, must be used prior to ga_simplex().");
  if (!pop->simplex_params->to_double) die("Population's genome to double callback is undefined.");
  if (!pop->simplex_params->from_double) die("Population's genome from double callback is undefined.");

/* 
 * Prepare working entities and double arrays.
 * The space for the average and new arrays are allocated simultaneously.
 */
  num_points = pop->simplex_params->dimensions+1;
  putative = s_malloc(sizeof(entity *)*num_points);
  putative_d = s_malloc(sizeof(double *)*num_points);
  putative_d_buffer = s_malloc(sizeof(double)*pop->simplex_params->dimensions*num_points*3);

  putative_d[0] = putative_d_buffer;
  average = &(putative_d_buffer[num_points*pop->simplex_params->dimensions]);
  new_d = &(putative_d_buffer[(num_points+1)*pop->simplex_params->dimensions]);
  new2_d = &(putative_d_buffer[(num_points+2)*pop->simplex_params->dimensions]);

  for (i=1; i<num_points; i++)
    {
    putative[i] = ga_get_free_entity(pop);    /* The 'working' solutions. */
    putative_d[i] = &(putative_d_buffer[i*pop->simplex_params->dimensions]);
    }

  new = ga_get_free_entity(pop);
  new2 = ga_get_free_entity(pop);

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform simplex search with random starting solution.");

    putative[0] = ga_get_free_entity(pop);
    ga_entity_seed(pop, putative[0]);
    initial = ga_get_free_entity(pop);
    }
  else
    {   
    plog(LOG_VERBOSE, "Will perform simplex search with specified starting solution.");

    putative[0] = ga_get_free_entity(pop);
    ga_entity_copy(pop, putative[0], initial);
    }

/*
 * Generate sample points.
 * Ensure that these initial solutions are scored.
 *
 * NOTE: Only perturb each solution by one dimension, by a unit
 * amount; it might be better to perturb all dimensions and/or
 * by a randomized amount.
 */
  pop->simplex_params->to_double(pop, putative[0], putative_d[0]);
  pop->evaluate(pop, putative[0]);

  for (i=1; i<num_points; i++)
    {
    for (j=0; j<pop->simplex_params->dimensions; j++)
      putative_d[i][j] = putative_d[0][j] + random_double_range(-step,step);

/*
    if (random_boolean())
      putative_d[i][i-1] -= 1.0;
    else
      putative_d[i][i-1] += 1.0;
*/

    pop->simplex_params->from_double(pop, putative[i], putative_d[i]);
    pop->evaluate(pop, putative[i]);
    }

/*
 * Sort the initial solutions by fitness.
 * We use a bi-directional bubble sort algorithm (which is
 * called shuffle sort, apparently).
 */ 
  last = pop->simplex_params->dimensions-1;
  while (done == FALSE && first < last)
    {
    for (j = last ; j > first ; j--)
      {
      if ( putative[j]->fitness > putative[j-1]->fitness )
        {	/* Swap! */
        tmpentity = putative[j];
        putative[j] = putative[j-1];
        putative[j-1] = tmpentity;
        tmpdoubleptr = putative_d[j];
        putative_d[j] = putative_d[j-1];
        putative_d[j-1] = tmpdoubleptr;
        }
      }
    first++;    /* The first one is definitely correct now. */

    done = TRUE;

    for (j = first ; j < last ; j++)
      {
      if ( putative[j]->fitness < putative[j+1]->fitness )
        {	/* Swap! */
        tmpentity = putative[j];
        putative[j] = putative[j+1];
        putative[j+1] = tmpentity;
        tmpdoubleptr = putative_d[j];
        putative_d[j] = putative_d[j+1];
        putative_d[j+1] = tmpdoubleptr;
        done = FALSE;
        }
      }
    last--;     /* The last one is definitely correct now. */
    }

  plog( LOG_VERBOSE,
        "Prior to the first iteration, the current solution has fitness score of %f",
         putative[0]->fitness );

/*
 * Do all the iterations:
 *
 * Stop when (a) max_iterations reached, or
 *           (b) "pop->iteration_hook" returns FALSE.
 */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, putative[0]):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

/*
 * Compute the vector average of all solutions except the least fit.
 * Exploration will proceed along the vector from the least fit point
 * to that vector average.
 */
    for (j = 0; j < pop->simplex_params->dimensions; j++)
      {
      average[j] = 0.0;
      }

/*    for (i = 1; i < num_points; i++)*/
    for (i = 0; i < num_points-1; i++)
      {
      for (j = 0; j < pop->simplex_params->dimensions; j++)
        {
        average[j] += putative_d[i][j];
        }
      }

/*
 * Check for convergence and restart if needed.
 * Reduce step, alpha, beta and gamma each time this happens.
 */
    restart_needed = TRUE;
printf("DEBUG: average = ");
    for (j = 0; j < pop->simplex_params->dimensions; j++)
      {
      /* Finish calculating average here to avoid an extra loop. */
      average[j] /= pop->simplex_params->dimensions;
      if ( average[j]-TINY > putative_d[pop->simplex_params->dimensions][j] ||
           average[j]+TINY < putative_d[pop->simplex_params->dimensions][j] )
        restart_needed = FALSE;
      
      printf("%f ", average[j]/pop->simplex_params->dimensions);
      }
printf("\n");

  if (restart_needed != FALSE)
    {
printf("DEBUG: restarting search.\n");
    step /= 2.0;
    alpha /= 2.0;
    beta /= 2.0;
    gamma /= 2.0;

    for (i=1; i<num_points; i++)
      {
      for (j=0; j<pop->simplex_params->dimensions; j++)
        putative_d[i][j] = putative_d[0][j] + random_double_range(-step,step);

      pop->simplex_params->from_double(pop, putative[i], putative_d[i]);
      pop->evaluate(pop, putative[i]);
      }
    }

/*
 * Simplex reflection - Extrapolate by a factor alpha away from worst point.
 */
   for (j = 0; j < pop->simplex_params->dimensions; j++)
     {
     new_d[j] = (1.0 + alpha) * average[j] - alpha * putative_d[num_points-1][j];
     }

/*
 * Evaluate the function at this reflected point.  
 */
    pop->simplex_params->from_double(pop, new, new_d);
    pop->evaluate(pop, new);

    if (new->fitness > putative[0]->fitness)
      {
/*
 * The new solution is fitter than the previously fittest solution, so attempt an 
 * additional extrapolation by a factor alpha.
 */
printf("DEBUG: new (%f) is fitter than p0 ( %f )\n", new->fitness, putative[0]->fitness);

      for (j = 0; j < pop->simplex_params->dimensions; j++)
        new2_d[j] = (1.0 + alpha) * new_d[j] - alpha * putative_d[num_points-1][j];

      pop->simplex_params->from_double(pop, new2, new2_d);
      pop->evaluate(pop, new2);

      if (new2->fitness > putative[0]->fitness)
        {
/*
 * This additional extrapolation succeeded, so replace the least fit solution
 * by inserting new solution in correct position.
 */
printf("DEBUG: new2 (%f) is fitter than p0 ( %f )\n", new2->fitness, putative[0]->fitness);

        tmpentity = putative[pop->simplex_params->dimensions];
        tmpdoubleptr = putative_d[pop->simplex_params->dimensions];

        for (j = pop->simplex_params->dimensions; j > 0; j--)
          {
          putative[j]=putative[j-1];
          putative_d[j]=putative_d[j-1];
          }

        putative[0] = new2;
        putative_d[0] = new2_d;

        new2 = tmpentity;
        new2_d = tmpdoubleptr;
        }
      else
        {
/*
 * This additional extrapolation failed, so use the original
 * reflected solution.
 */
        tmpentity = putative[pop->simplex_params->dimensions];
        tmpdoubleptr = putative_d[pop->simplex_params->dimensions];

        for (j = pop->simplex_params->dimensions; j > 0; j--)
          {
          putative[j]=putative[j-1];
          putative_d[j]=putative_d[j-1];
          }

        putative[0] = new;
        putative_d[0] = new_d;

        new = tmpentity;
        new_d = tmpdoubleptr;
        }
      }
    else if (new->fitness < putative[pop->simplex_params->dimensions-1]->fitness)
      {
/*
 * The reflected point is worse than the second-least fit.  
 */
printf("DEBUG: new (%f) is less fit than p(n-1) ( %f )\n", new->fitness, putative[pop->simplex_params->dimensions-1]->fitness);

      did_replace = FALSE;

      if (new->fitness > putative[pop->simplex_params->dimensions]->fitness)
        {
/*
 * It is better than the least fit, so use it to replace the
 * least fit.
 */
printf("DEBUG: but fitter than p(n) ( %f )\n", putative[pop->simplex_params->dimensions]->fitness);
        did_replace = TRUE;

        tmpentity = putative[pop->simplex_params->dimensions];
        tmpdoubleptr = putative_d[pop->simplex_params->dimensions];

        putative[pop->simplex_params->dimensions] = new;
        putative_d[pop->simplex_params->dimensions] = new_d;

        new = tmpentity;
        new_d = tmpdoubleptr;
        }
/*
 * Perform a contraction of the simplex along one dimension, away from worst point.
 */
      for (j = 0; j < num_points; j++)
        new_d[j] = (1.0 - beta) * average[j] + beta * putative_d[num_points-1][j];

      pop->simplex_params->from_double(pop, new, new_d);
      pop->evaluate(pop, new);

      if (new->fitness > putative[pop->simplex_params->dimensions]->fitness)
        {
/*
 * The contraction gave an improvement, so accept it by
 * inserting the new solution at the correct position.
 */
        did_replace = TRUE;

printf("DEBUG: contracted new (%f) is fitter than p(n) ( %f )\n", new->fitness, putative[pop->simplex_params->dimensions]->fitness);
        i = 0;
        while (putative[i]->fitness > new->fitness) i++;

        tmpentity = putative[pop->simplex_params->dimensions];
        tmpdoubleptr = putative_d[pop->simplex_params->dimensions];

        for (j = pop->simplex_params->dimensions; j > i; j--)
          {
          putative[j]=putative[j-1];
          putative_d[j]=putative_d[j-1];
          }

        putative[i] = new;
        putative_d[i] = new_d;

        new = tmpentity;
        new_d = tmpdoubleptr;
        }

      if (did_replace == FALSE)
        {
/*
 * The new solution is worse than the previous worse.  So, contract
 * toward the most fit point.
 */
printf("DEBUG: new (%f) is worse than all.\n", new->fitness);

        for (i = 1; i < num_points; i++)
          {
          for (j = 0; j < pop->simplex_params->dimensions; j++)
            putative_d[i][j] = putative_d[0][j] + gamma * (putative_d[i][j] - putative_d[0][j]);

          pop->simplex_params->from_double(pop, putative[i], putative_d[i]);
          pop->evaluate(pop, putative[i]);
          }
        }
      }
    else
      {
/*
 * The reflection gave a solution which was better than the worst two
 * solutions, but worse than the best solution.
 * Replace the old worst solution by inserting the new solution at the
 * correct position.
 */
printf("DEBUG: new (%f) is fitter than worst 2\n", new->fitness);
      for (j=0; j < pop->simplex_params->dimensions; j++)
        printf("%d fitness = %f\n", j, putative[j]->fitness);

      i = 0;
      while (putative[i]->fitness > new->fitness) i++;

printf("DEBUG: new inserted at position %d\n", i);

      tmpentity = putative[pop->simplex_params->dimensions];
      tmpdoubleptr = putative_d[pop->simplex_params->dimensions];

      for (j = pop->simplex_params->dimensions; j > i; j--)
        {
        putative[j]=putative[j-1];
        putative_d[j]=putative_d[j-1];
        }

      putative[i] = new;
      putative_d[i] = new_d;

      new = tmpentity;
      new_d = tmpdoubleptr;
      }

/*
 * Use the iteration callback.
 */
    plog( LOG_VERBOSE,
          "After iteration %d, the current solution has fitness score of %f",
          iteration,
          putative[0]->fitness );

    }	/* Iteration loop. */

/*
 * Store best solution.
 */
  ga_entity_copy(pop, initial, putative[0]);

/*
 * Cleanup.
 */
  ga_entity_dereference(pop, new);
  ga_entity_dereference(pop, new2);

  for (i=0; i<num_points; i++)
    {
    ga_entity_dereference(pop, putative[i]);
    }

  s_free(putative);
  s_free(putative_d);
  s_free(putative_d_buffer);

  return iteration;
  }


