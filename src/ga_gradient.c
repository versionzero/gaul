/**********************************************************************
  ga_gradient.c
 **********************************************************************

  ga_gradient - Gradient methods for comparison and local search.
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

#include "gaul/ga_gradient.h"

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
					const int		dimensions,
					const double		step_size)
  {

  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !to_double ) die("Null pointer to GAto_double callback passed.");
  if ( !from_double ) die("Null pointer to GAfrom_double callback passed.");

  plog( LOG_VERBOSE, "Population's gradient methods parameters set" );

  if (pop->gradient_params == NULL)
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
  last updated:	28 May 2003
 **********************************************************************/

int ga_steepestascent(	population	*pop,
			entity		*current,
			const int	max_iterations )
  {
  int		iteration=0;		/* Current iteration number. */
  int		i;			/* Index into arrays. */
  double	*current_d;		/* Current solution array. */
  double	*current_g;		/* Current solution gradient array. */
  entity	*new;			/* New putative solution. */
  double	*new_d;			/* New putative solution array. */
  entity	*tmpentity;		/* Used to swap working solutions. */
  double	*tmpdoubleptr;		/* Used to swap working solutions. */
  double	*buffer;		/* Storage for double arrays. */
  double	step_size;		/* Current step size. */
  double	grms;			/* Current RMS gradient. */
  boolean	force_terminate=FALSE;	/* Force optimisation to terminate. */

/*
 * Checks.
 */
  if (!pop) die("NULL pointer to population structure passed.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->gradient_params) die("ga_population_set_gradient_params(), or similar, must be used prior to ga_gradient().");
  if (!pop->gradient_params->to_double) die("Population's genome to double callback is undefined.");
  if (!pop->gradient_params->from_double) die("Population's genome from double callback is undefined.");
  if (!pop->gradient_params->gradient) die("Population's first derivatives callback is undefined.");

/* 
 * Prepare working entity and double arrays.
 */
  buffer = s_malloc(sizeof(double)*pop->gradient_params->dimensions*3);

  current_d = buffer;
  current_g = &(buffer[pop->gradient_params->dimensions]);
  new_d = &(buffer[pop->gradient_params->dimensions*2]);

  new = ga_get_free_entity(pop);

/* Do we need to generate a random starting solution? */
  if (current==NULL)
    {
    plog(LOG_VERBOSE, "Will perform gradient search with random starting solution.");

    current = ga_get_free_entity(pop);
    ga_entity_seed(pop, current);
    }
  else
    {   
    plog(LOG_VERBOSE, "Will perform gradient search with specified starting solution.");
    }

/*
 * Get initial fitness and derivatives.
 */
  pop->evaluate(pop, current);
  pop->gradient_params->to_double(pop, current, current_d);

  grms = pop->gradient_params->gradient(pop, current, current_d, current_g);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, the current solution has fitness score of %f and a RMS gradient of %f",
         current->fitness, grms );

/*
 * Adjust step size based on gradient.
 * This scales the step size according to the initial gradient so that the
 * calculation doesn't blow-up completely.
 */
/*  step_size=(pop->gradient_params->dimensions*pop->gradient_params->step_size)/grms;*/
  step_size=pop->gradient_params->step_size;

/*
 * Do all the iterations:
 *
 * Stop when (a) max_iterations reached, or
 *           (b) "pop->iteration_hook" returns FALSE.
 * The iteration hook could evaluate the RMS gradient, or the maximum component
 * of the gradient, or any other termination criteria that may be desirable.
 */
  while ( force_terminate==FALSE &&
          (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
          iteration<max_iterations )
    {
    iteration++;

    for( i=0; i<pop->gradient_params->dimensions; i++ )
      new_d[i]=current_d[i]+step_size*current_g[i];

    pop->gradient_params->from_double(pop, new, new_d);
    pop->evaluate(pop, new);

    printf("DEBUG: current_d = %f %f %f %f\n", current_d[0], current_d[1], current_d[2], current_d[3]);
    printf("DEBUG: current_g = %f %f %f %f grms = %f\n", current_g[0], current_g[1], current_g[2], current_g[3], grms);
    printf("DEBUG: new_d = %f %f %f %f fitness = %f\n", new_d[0], new_d[1], new_d[2], new_d[3], new->fitness);

    if ( current->fitness > new->fitness )
      {	/* New solution is worse. */

      do
        {
        step_size *= 0.5;	/* FIXME: Should be a parameter. */
        printf("DEBUG: step_size = %e\n", step_size);

        for( i=0; i<pop->gradient_params->dimensions; i++ )
          new_d[i]=current_d[i]+step_size*current_g[i];

        pop->gradient_params->from_double(pop, new, new_d);
        pop->evaluate(pop, new);

        printf("DEBUG: new_d = %f %f %f %f fitness = %f\n", new_d[0], new_d[1], new_d[2], new_d[3], new->fitness);
        } while( current->fitness > new->fitness && step_size > ApproxZero);

      if (step_size <= ApproxZero && grms <= ApproxZero) force_terminate=TRUE;
      }
    else 
      {	/* New solution is an improvement. */
      step_size *= 1.2;	/* FIXME: Should be a parameter. */
      printf("DEBUG: step_size = %e\n", step_size);
      }

/* Store improved solution. */
    tmpentity = current;
    current = new;
    new = tmpentity;

    tmpdoubleptr = current_d;
    current_d = new_d;
    new_d = tmpdoubleptr;

    grms = pop->gradient_params->gradient(pop, current, current_d, current_g);

/*
 * Use the iteration callback.
 */
    plog( LOG_VERBOSE,
          "After iteration %d, the current solution has fitness score of %f and RMS gradient of %f (step_size = %f)",
          iteration, current->fitness, grms, step_size );

    }	/* Iteration loop. */

/*
 * Cleanup.
 */
  ga_entity_dereference(pop, new);

  s_free(buffer);

  return iteration;
  }


#if 0
This is the code that I usually use to perform SD minimization:

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

#endif
