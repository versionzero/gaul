/**********************************************************************
  ga_qsort.h
 **********************************************************************

  ga_qsort - GA population sorting routines.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:     Sort the population by fitness.

  Last Updated: 08/05/00 SAA	Some stuff moved out of qsort.c prior to rename to helga_qsort.c

 **********************************************************************/

#ifndef GA_QSORT_H_INCLUDED
#define GA_QSORT_H_INCLUDED

#include "SAA_header.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ga_util.h"

/* Activate the timing code with this constant */
/*#define GA_QSORT_TIME*/

#ifndef GA_QSORT_DEBUG
#define GA_QSORT_DEBUG	1
#endif

/*
 * Prototypes.
 */
void	quicksort_population(population *pop);
boolean	ga_qsort_test(void);

#if HELGA_USE_SLANG==1
void	ga_population_sort(int *id);
#endif

#endif	/* GA_QSORT_H_INCLUDED */
