/**********************************************************************
  ga_gradient.c
 **********************************************************************

  ga_gradient - Gradient methods for comparison and local search.
  Copyright ©2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:     Gradient methods for comparison and local search.

		Routines for local search and optimisation using
		non-evolutionary methods.  These methods are all
		first-order, that is, they require first derivatives.

		Note that, these algorithms require that chromosomes
		may be reversibly mapped to arrays of double-precision
		floating-point array chromsomes.  If this is not
		possible then, hmmm, tough luck.

		You might want to think carefully about your convergence
		criteria.

  References:

 **********************************************************************/

#include "ga_gradient.h"

/**********************************************************************
  ga_population_set_gradient_parameters()
  synopsis:     Sets the gradient-search parameters for a population.
  parameters:	population *pop		Population to set parameters of.
		const GAto_double	Map chromosomal data to array of doubles.
		const GAfrom_double	Map array of doubles to chromosomal data.
		const int		Number of dimensions for double array (Needn't match dimensions of chromosome.)
  return:	none
  last updated: 19 Nov 2002
 **********************************************************************/

void ga_population_set_gradient_parameters( population		*pop,
                                        const GAto_double	to_double,
                                        const GAfrom_double	from_double,
                                        const GAgradient	gradient,
					const double		step_size,
					const int		dimensions)
  {

  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !to_double ) die("Null pointer to GAto_double callback passed.");
  if ( !from_double ) die("Null pointer to GAfrom_double callback passed.");

  plog( LOG_VERBOSE, "Population's gradient methods parameters set" );

  pop->gradient_params = s_malloc(sizeof(ga_gradient_t));

  pop->gradient_params->to_double = to_double;
  pop->gradient_params->from_double = from_double;
  pop->gradient_params->gradient = gradient;
  pop->gradient_params->step_size = step_size;
  pop->gradient_params->dimensions = dimensions;

  return;
  }


/**********************************************************************
  ga_steepestascent()
  synopsis:	Performs optimisation on the passed entity by using a
  		steepest ascents method (i.e. steepest descent, except
		maximising the fitness function).
		The passed entity will have its data overwritten.  The
		remainder of the population will be let untouched.
		Note that it is safe to pass a NULL initial structure,
		in which case a random starting structure wil be
		generated, however the final solution will not be
		available to the caller in any obvious way.
  parameters:
  return:
  last updated:	19 Nov 2002
 **********************************************************************/

int ga_steepestascent(	population		*pop,
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

/*
 * Checks.
 */
  if (!pop) die("NULL pointer to population structure passed.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->gradient_params) die("ga_population_set_gradient_params(), or similar, must be used prior to ga_gradient().");

/* 
 * Prepare working entities and double arrays.
 * The space for the average and new arrays are allocated simultaneously.
 */
  num_points = pop->gradient_params->dimensions+1;
  putative = s_malloc(sizeof(entity *)*num_points);
  putative_d = s_malloc(sizeof(double *)*num_points);
  putative_d_buffer = s_malloc(sizeof(double)*pop->gradient_params->dimensions*num_points+3);

  putative_d[0] = putative_d_buffer;
  average = &(putative_d_buffer[num_points*pop->gradient_params->dimensions]);
  new_d = &(putative_d_buffer[(num_points+1)*pop->gradient_params->dimensions]);
  new2_d = &(putative_d_buffer[(num_points+2)*pop->gradient_params->dimensions]);

  for (i=1; i<num_points; i++)
    {
    putative[i] = ga_get_free_entity(pop);    /* The 'working' solutions. */
    putative_d[i] = &(putative_d_buffer[i*pop->gradient_params->dimensions]);
    }

  new = ga_get_free_entity(pop);
  new2 = ga_get_free_entity(pop);

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform gradient search with random starting solution.");

    putative[0] = ga_get_free_entity(pop);
    ga_entity_seed(pop, putative[0]);
    initial = ga_get_free_entity(pop);
    }
  else
    {   
    plog(LOG_VERBOSE, "Will perform gradient search with specified starting solution.");
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
  pop->gradient_params->to_double(pop, putative[0], putative_d[0]);
  pop->evaluate(pop, putative[0]);

  for (i=1; i<num_points; i++)
    {
    for (j=0; j<pop->gradient_params->dimensions; j++)
      putative_d[i][j] = putative_d[0][j];

    if (random_boolean())
      putative_d[i][i] -= 1.0;
    else
      putative_d[i][i] += 1.0;

    pop->gradient_params->from_double(pop, putative[i], putative_d[i]);
    pop->evaluate(pop, putative[i]);
    }

/*
 * Sort the initial solutions by fitness.
 * We use a bi-directional bubble sort algorithm (which is
 * called shuffle sort, apparently).
 */ 
  last = pop->gradient_params->dimensions-1;
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
    for (j = 0; j < pop->gradient_params->dimensions; j++)
      {
      average[j] = 0.0;
      }

    for (i = 1; i < num_points; i++)
      {
      for (j = 0; j < pop->gradient_params->dimensions; j++)
        {
        average[j] += putative_d[i][j];
        }
      }

/*
 * Simplex reflection - Extrapolate by a factor alpha away from worst point.
 */
   for (j = 0; j < pop->gradient_params->dimensions; j++)
     {
     /* Finish calculating average here to avoid an extra loop. */
     average[j] /= pop->gradient_params->dimensions;
     new_d[j] = (1.0 + alpha) * average[j] - alpha * putative_d[num_points-1][j];
     }

/*
 * Evaluate the function at this reflected point.  
 */
    pop->gradient_params->from_double(pop, new, new_d);
    pop->evaluate(pop, new);

    if (new->fitness > putative[0]->fitness)
      {
/*
 * The new solution is fitter than the previously fittest solution, so attempt an 
 * additional extrapolation by a factor alpha.
 */
      for (j = 0; j < pop->gradient_params->dimensions; j++)
        new2_d[j] = (1.0 + alpha) * new_d[j] - alpha * putative_d[num_points-1][j];

      pop->gradient_params->from_double(pop, new2, new2_d);
      pop->evaluate(pop, new2);

      if (new2->fitness > putative[0]->fitness)
        {
/*
 * This additional extrapolation succeeded, so replace the least fit solution
 * by inserting new solution in correct position.
 */
        tmpentity = putative[pop->gradient_params->dimensions];
        tmpdoubleptr = putative_d[pop->gradient_params->dimensions];

        for (j = pop->gradient_params->dimensions; j > 0; j--)
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
        tmpentity = putative[pop->gradient_params->dimensions];
        tmpdoubleptr = putative_d[pop->gradient_params->dimensions];

        for (j = pop->gradient_params->dimensions; j > 0; j--)
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
    else if (new->fitness < putative[pop->gradient_params->dimensions-1]->fitness)
      {
/*
 * The reflected point is worse than the second-least fit.  
 * If it is better than the least fit, then use it to replace the
 * least fit.
 */
      did_replace = FALSE;

      if (new->fitness > putative[pop->gradient_params->dimensions]->fitness)
        {
        did_replace = TRUE;

        tmpentity = putative[pop->gradient_params->dimensions];
        tmpdoubleptr = putative_d[pop->gradient_params->dimensions];

        putative[pop->gradient_params->dimensions-1] = new;
        putative_d[pop->gradient_params->dimensions-1] = new_d;

        new = tmpentity;
        new_d = tmpdoubleptr;
        }
/*
 * Perform a contraction of the gradient along one dimension, away from worst point.
 */
      for (j = 0; j < num_points; j++)
        new_d[j] = (1.0 - beta) * average[j] + beta * putative_d[num_points-1][j];

      pop->gradient_params->from_double(pop, new, new_d);
      pop->evaluate(pop, new);

      if (new->fitness > putative[pop->gradient_params->dimensions]->fitness)
        {
/*
 * The contraction gave an improvement, so accept it by
 * inserting the new solution at the correct position.
 */
        i = 0;
        while (putative[i]->fitness > new->fitness) i++;

        tmpentity = putative[pop->gradient_params->dimensions];
        tmpdoubleptr = putative_d[pop->gradient_params->dimensions];

        for (j = pop->gradient_params->dimensions; j > i; j--)
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
        for (i = 1; i < num_points; i++)
          {
          for (j = 0; j < pop->gradient_params->dimensions; j++)
            putative_d[i][j] = putative_d[0][j] + gamma * (putative_d[i][j] - putative_d[0][j]);

          pop->gradient_params->from_double(pop, putative[i], putative_d[i]);
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
      i = 0;
      while (putative[i]->fitness > new->fitness) i++;

      tmpentity = putative[pop->gradient_params->dimensions];
      tmpdoubleptr = putative_d[pop->gradient_params->dimensions];

      for (j = pop->gradient_params->dimensions; j > i; j--)
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


/* Steepest Descent */
boolean SteepestDescentMinimize(	double (*ObjectiveFunc)(),
					double (*DerivativeFunc)(),
					int n,
					double *p,
					double *px,
					double *gmin,
					int max_iter,
					vpointer userdata)
  {
  double grad,step;
  double val;
  int iter;
  int i;
  double *x,*nx;

  x = s_malloc(sizeof(double)*n);
  nx = s_malloc(sizeof(double)*n);

  step = INITIAL_SD_STEP;
  *gmin = (*DerivativeFunc)(n,p,px,userdata);

  for( iter=0; iter<max_iter; iter++ )
    {
#if OPTIMIZERS_DEBUG>0
    printf("DEBUG: %d: %f [ ",iter,*gmin);
    for(i=0; i<n; i++) printf("%f ", p[i]);
    printf("]\n");
#endif

    for( i=0; i<n; i++ ) x[i]=p[i]-step*px[i];

    val = (*DerivativeFunc)(n,x,nx,userdata);
    if ( val >= *gmin )
      {
      grad = 0.0;
      for( i=0; i<n; i++ ) grad += px[i]*px[i];
      if ( grad < ApproxZero )
        {
#if OPTIMIZERS_DEBUG>0
        printf("DEBUG: SteepestDescentMinimize() Finshed due to small gradient (sum grad squared = %f)\n", grad);
#endif
        s_free(x);
        s_free(nx);
        return TRUE;
        }

      do
        {
        step *= 0.5;
        if ( step < ApproxZero )
          {
#if OPTIMIZERS_DEBUG>0
          printf("DEBUG: SteepestDescentMinimize() Finshed due to small step size ( step %f)\n", step);
#endif
          s_free(x);
          s_free(nx);
          return FALSE;
          }

/*
        for( i=0; i<n; i++ ) x[i]=p[i]-step*px[i];
*/
        for( i=0; i<n; i++ ) x[i]=p[i]+step*px[i];
        val = (*ObjectiveFunc)(n,x,userdata);
        } while( val > *gmin );

      for( i=0; i<n; i++ ) p[i] = x[i];
      *gmin = (*DerivativeFunc)(n,p,px,userdata);
      }
    else 
      {
      for( i=0; i<n; i++ )
        {
        px[i] = nx[i];
        p[i] = x[i];
        }
      step *= 1.2;
      *gmin = val;
      }
    }

#if OPTIMIZERS_DEBUG>0
  printf("DEBUG: SteepestDescentMinimize() Finshed because max. iterations performed.\n");
#endif
  s_free(x);
  s_free(nx);

  return FALSE;
  }


