/**********************************************************************
  ga_crossover.c
 **********************************************************************

  ga_crossover - Genetic algorithm crossover operators.
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

  Synopsis:     Routines for performing GA crossover operations.

		These functions should duplicate user data where
		appropriate.

  Updated:	23/04/01 SAA	Split from ga_util.c for ease of maintainance.  These functions now have no return value.

 **********************************************************************/

#include "ga_util.h"

/**********************************************************************
  ga_singlepoint_crossover_chromosome()
  synopsis:	`Mates' two chromosomes by single-point crossover.
  parameters:
  return:
  last updated: 18/10/00
 **********************************************************************/

void ga_singlepoint_crossover_chromosome(population *pop, int *father, int *mother, int *son, int *daughter)
  {
  int	location;	/* Point of crossover */

  /* Checks */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to chromosome structure passed.");

#if 0
  if (dint_size(father) != dint_size(mother))
    die("Chromosomes have different lengths.");
#endif

  /* Choose crossover point and perform operation */
  location=random_int(pop->len_chromosomes);

  if (random_boolean())
    {
    memcpy(son, father, location*sizeof(int));
    memcpy(daughter, mother, location*sizeof(int));

    memcpy(&(son[location]), &(mother[location]), (pop->len_chromosomes-location)*sizeof(int));
    memcpy(&(daughter[location]), &(father[location]), (pop->len_chromosomes-location)*sizeof(int));
    }
  else
    {
    memcpy(son, mother, location);
    memcpy(daughter, father, location);

    memcpy(&(son[location]), &(father[location]), (pop->len_chromosomes-location)*sizeof(int));
    memcpy(&(daughter[location]), &(mother[location]), (pop->len_chromosomes-location)*sizeof(int));
    }

  return;
  }


/**********************************************************************
  ga_crossover_chromosome_singlepoints()
  synopsis:	`Mates' two genotypes by single-point crossover of
		each chromosome.
  parameters:
  return:
  last updated: 12/05/00
 **********************************************************************/

void ga_crossover_chromosome_singlepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter)
  {
  int		i;	/* Loop variable over all chromosomes */

  /* Checks */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    ga_singlepoint_crossover_chromosome( pop,
                        father->chromosome[i],
			mother->chromosome[i],
			son->chromosome[i],
			daughter->chromosome[i]);
    }

  return;
  }


/**********************************************************************
  ga_crossover_chromosome_mixing()
  synopsis:	`Mates' two genotypes by single-point crossover.
		Keeps all chromosomes intact, and therefore do not
		need to recreate structural data.
  parameters:
  return:
  last updated: 27/04/00
 **********************************************************************/

void ga_crossover_chromosome_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter)
  {
  int		i;		/* Loop variable over all chromosomes */

  /* Checks */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    if (random_boolean())
      {
      memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
      memcpy(daughter->chromosome[i], mother->chromosome[i], pop->len_chromosomes*sizeof(int));
      ga_copy_data(pop, son, father, i);
      ga_copy_data(pop, daughter, mother, i);
      }
    else
      {
      memcpy(daughter->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
      memcpy(son->chromosome[i], mother->chromosome[i], pop->len_chromosomes*sizeof(int));
      ga_copy_data(pop, daughter, father, i);
      ga_copy_data(pop, son, mother, i);
      }
    }

/*
  debug_chromo(&(father->chromosome[0]));
  debug_chromo(&(son->chromosome[0]));
*/

  return;
  }


/**********************************************************************
  ga_crossover_allele_mixing()
  synopsis:	`Mates' two genotypes by randomizing the parents
		alleles.
		Keeps no chromosomes intact, and therefore will
		need to recreate all structural data.
  parameters:
  return:
  last updated: 30/04/00
 **********************************************************************/

void ga_crossover_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter )
  {
  int		i, j;		/* Loop over all chromosomes, alleles. */

  /* Checks. */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to entity structure passed.");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    for (j=0; j<pop->len_chromosomes; j++)
      {
      if (random_boolean())
        {
        son->chromosome[i][j] = father->chromosome[i][j];
        daughter->chromosome[i][j] = mother->chromosome[i][j];
        }
      else
        {
        daughter->chromosome[i][j] = father->chromosome[i][j];
        son->chromosome[i][j] = mother->chromosome[i][j];
        }
      }
    }

  return;
  }


