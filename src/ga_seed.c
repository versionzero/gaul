/**********************************************************************
  ga_seed.c
 **********************************************************************

  ga_seed - Genetic algorithm genome initialisation operators.
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

  Synopsis:     Routines for performing GA seeding operations.

 **********************************************************************/

#include "ga_util.h"

/**********************************************************************
  ga_seed_boolean_random()
  synopsis:
  parameters:
  return:
  last updated: 15/05/01
 **********************************************************************/

void ga_seed_boolean_random(population *pop, entity *adam)
  {
  int		chromo;		/* Index of chromosome to seed */
  int		point;		/* Index of 'nucleotide' to seed */

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      adam->chromosome[chromo][point] = random_boolean();
      }
    }

  return;
  }


/**********************************************************************
  ga_seed_integer_random()
  synopsis:
  parameters:
  return:
  last updated: 15/05/01
 **********************************************************************/

void ga_seed_integer_random(population *pop, entity *adam)
  {
  int		chromo;		/* Index of chromosome to seed */
  int		point;		/* Index of 'nucleotide' to seed */

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      adam->chromosome[chromo][point] = random_rand();
      }
    }

  return;
  }


/**********************************************************************
  ga_seed_integer_zero()
  synopsis:
  parameters:
  return:
  last updated: 15/05/01
 **********************************************************************/

void ga_seed_integer_zero(population *pop, entity *adam)
  {
  int		chromo;		/* Index of chromosome to seed */
  int		point;		/* Index of 'nucleotide' to seed */

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      adam->chromosome[chromo][point] = 0;
      }
    }

  return;
  }


