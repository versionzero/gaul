/**********************************************************************
  random_util.h
 **********************************************************************

  random_util - Random number utility routines.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatiable with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Random number utility routines.

  Updated:	02/02/01 SAA	Converted from helga_random to random_util.
		16/05/00 SAA	First code.

 **********************************************************************/

#ifndef RANDOM_UTIL_H_INCLUDED
#define RANDOM_UTIL_H_INCLUDED

#include "SAA_header.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* #include "log_util.h" */

/*
 * Debugging.
 */
#ifndef RANDOM_DEBUG
#define RANDOM_DEBUG DEBUG
#endif

/*
 * Constants.
 */
#if HAVE_LIMITS_H==1
#include "limits.h"
#define RANDOM_RAND_MAX		UINT_MAX
#else
#define RANDOM_RAND_MAX          0xFFFFFFFF      /* Max. 32-bit unsigned int */
#endif

/*
 * Type definitions.
 * sizeof(random_state) should be 64*sizeof(int) which is
 * hopefully, fairly, optimal.
 */
#define RANDOM_NUM_STATE_VALS	57
typedef struct random_state_t
  {
  unsigned int	v[RANDOM_NUM_STATE_VALS];
  int		j, k, x;
  } random_state;

/*
 * Function prototypes.
 */

unsigned int	random_rand(void);
void	random_seed(const unsigned int seed);
void	random_tseed(void);
void	random_init(void);
boolean	random_isinit(void);
char	*random_get_state_str(void);
unsigned int random_get_state_str_len(void);
void	random_set_state_str(char *state);
boolean	random_boolean(void);
boolean	random_boolean_prob(const double prob);
unsigned int	random_int(const unsigned int max);
int	random_int_range(const int min, const int max);
double	random_double(const double max);
double	random_double_range(const double min, const double max);
double	random_double_1(void);
double	random_unit_uniform(void);
double	random_gaussian(const double mean, const double stddev);
double	random_unit_gaussian(void);
void	random_diagnostics(void);
boolean	random_test(void);


#if HAVE_SLANG==1

/* These functions don't need wrappers:
void	random_init(void)
boolean	random_isinit(void)
unsigned int	random_get_state_str(void);
char	*random_get_state_str_len(void);
void	random_set_state_str(char *state);
double	random_double_1(void);
boolean	random_boolean(void)
double	random_unit_uniform(void)
double	random_unit_gaussian(void)
void	random_diagnostics(void)
boolean	random_test(void)
*/
/* These functions aren't defined as intrinsics anyway:
State fetching/setting stuff.
*/

int	random_rand_wrapper(void);
void	random_seed_wrapper(int *seed);
boolean	random_boolean_prob_wrapper(double *prob);
int	random_int_wrapper(int *max);
int	random_int_range_wrapper(int *min, int *max);
double	random_double_wrapper(double *max);
double	random_double_range_wrapper(double *min, double *max);
double	random_gaussian_wrapper(double *mean, double *stddev);

#endif	/* HAVE_SLANG==1 */

#endif	/* RANDOM_UTIL_H_INCLUDED */

