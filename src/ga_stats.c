/**********************************************************************
  ga_stats.c
 **********************************************************************

  ga_stats - Statistics routines.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Convenience statistics functions.

  To do:	On-line and off-line performance summaries.

 **********************************************************************/

#include "gaul.h"

/**********************************************************************
  ga_fitness_mean()
  synopsis:     Determine mean of the fitness scores.
  parameters:	population *pop		The population to evaluate.
  		double *average		Returns the average fitness.
  return:	TRUE on success.
  last updated: 31 Dec 2002
 **********************************************************************/

boolean ga_fitness_mean( population *pop, double *average )
  {
  int           i;           /* Loop over all entities. */
  double        sum=0.0;     /* Sum of fitnesses. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");
  if (!average) die("Null pointer to double passed.");

  for (i=0; i<pop->size; i++)
    {
    sum += pop->entity_iarray[i]->fitness;
    }

  *average = sum / pop->size;

  return TRUE;
  }


/**********************************************************************
  ga_fitness_mean_stddev()
  synopsis:     Determine mean and standard deviation of the fitness
                scores.
  parameters:	population *pop		The population to evaluate.
  		double *average		Returns the average fitness.
		double *stddev		Returns the standard deviation of the fitnesses.
  return:	TRUE on success.
  last updated: 30/04/01
 **********************************************************************/

boolean ga_fitness_mean_stddev( population *pop,
                             double *average, double *stddev )
  {
  int           i;                      /* Loop over all entities. */
  double        sum=0.0, sumsq=0.0;     /* Sum and sum squared. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");
  if (!stddev || !average) die("Null pointer to double passed.");

  for (i=0; i<pop->size; i++)
    {
    sum += pop->entity_iarray[i]->fitness;
    sumsq += SQU(pop->entity_iarray[i]->fitness);
    }

  *average = sum / pop->size;
  *stddev = (sumsq - sum*sum/pop->size)/pop->size;

  return TRUE;
  }


/**********************************************************************
  ga_fitness_stats()
  synopsis:     Determine some stats about the fitness scores.
  parameters:	population *pop		The population to evaluate.
  		double *mean		Returns the average fitness.
  		double *median		Returns the median fitness.
		double *variance	Returns the variance of the fitnesses.
		double *stddev		Returns the standard deviation of the fitnesses.
		double *kurtosis	Returns the kurtosis of the fitnesses.
		double *skew		Returns the skew of the fitnesses.
  return:	TRUE on success.
  last updated: 24 Dec 2002
 **********************************************************************/

boolean ga_fitness_stats( population *pop,
                          double *max, double *min,
                          double *mean, double *median,
                          double *variance, double *stddev,
                          double *kurtosis, double *skew )
  {
  int           i;                      /* Loop over all entities. */
  double	sum=0.0,sum2=0.0,sum3=0.0,sum4=0.0;	/* Sum and stuff. */
  double	m2=0.0,m3=0.0,m4=0.0;	/* Distribution moments. */
  double	tmp=0.0;		/* Used to save some lookups. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");
  if (!max || !min || !mean || !variance || !stddev || !kurtosis || !skew)
    die("Null pointer to double passed.");

  *min = pop->entity_iarray[0]->fitness;

  for (i=0; i<pop->size; i++)
    {
    tmp = pop->entity_iarray[i]->fitness;
    sum += tmp;
    sum2 += tmp*tmp;
    sum3 += tmp*tmp*tmp;	/* I hope my compiler optimises this... */
    sum4 += tmp*tmp*tmp*tmp;
    }

  *max = tmp;

  *median = *min + (*max - *min)/2;

  *mean = sum/pop->size;
  *stddev = (sum2/pop->size - (*mean)*(*mean));	/* Cleverly avoid a sqrt() calc. */
  *variance = (sum2/pop->size - sum*sum);

/* Sanity check. */
  if ( fabs(sqrt(*variance)-*stddev) > DBL_EPSILON )
    dief("stddev = %f, sqrt(*variance) = %f", *stddev, sqrt(*variance));

  for (i=0; i<pop->size; i++)
    {
    tmp = pop->entity_iarray[i]->fitness - *mean;
    m2 += tmp*tmp;		/* I rely upon the compiler to optimise these three lines! */
    m3 += tmp*tmp*tmp;
    m4 += tmp*tmp*tmp*tmp;
    }

  m2 /= pop->size;
  m3 /= pop->size;
  m4 /= pop->size;

  *skew = m3/pow(m2,3.0/2.0);
  *kurtosis = m4/(m2*m2);

  return TRUE;
  }

