/**********************************************************************
  ga_mutate.c
 **********************************************************************

  ga_mutate - Genetic algorithm mutation operators.
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

  Synopsis:     Routines for performing GA mutation operations.

		These functions should duplicate user data were
		appropriate.

 **********************************************************************/

#include "ga_core.h"

/**********************************************************************
  ga_singlepoint_drift_mutation()
  synopsis:	Cause a single mutation event in which a single
		nucleotide is cycled.
  parameters:
  return:
  last updated: 01/09/00
 **********************************************************************/

void ga_singlepoint_drift_mutation(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of 'nucleotide' to mutate */
  int		dir=random_boolean()?-1:1;	/* The direction of drift. */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  chromo = random_int(pop->num_chromosomes);
  point = random_int(pop->len_chromosomes);

/*
 * Copy unchanged data.
 */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    if (i!=chromo)
      {
      ga_copy_data(pop, son, father, i);
      }
    else
      {
      ga_copy_data(pop, son, NULL, i);
      }
    }

/*
 * Mutate by tweaking a single nucleotide.
 */
  ((int *)son->chromosome[chromo])[point] += dir;

  if (((int *)son->chromosome[chromo])[point]==RAND_MAX) ((int *)son->chromosome[chromo])[point]=0;
  if (((int *)son->chromosome[chromo])[point]==-1) ((int *)son->chromosome[chromo])[point]=RAND_MAX-1;

  return;
  }


/**********************************************************************
  ga_singlepoint_randomize_mutation()
  synopsis:	Cause a single mutation event in which a single
		nucleotide is randomized.
  parameters:
  return:
  last updated: 01/09/00
 **********************************************************************/

void ga_singlepoint_randomize_mutation(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of 'nucleotide' to mutate */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  chromo = random_int(pop->num_chromosomes);
  point = random_int(pop->len_chromosomes);

/* Copy unchanging data. */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    if (i!=chromo)
      {
      ga_copy_data(pop, son, father, i);
      }
    else
      {
      ga_copy_data(pop, son, NULL, i);
      }
    }

  ((int *)son->chromosome[chromo])[point] = random_int(RAND_MAX);

  return;
  }


/**********************************************************************
  ga_multipoint_mutation()
  synopsis:	Cause a number of mutation events.  This is equivalent
		to the more common 'bit-drift' mutation.
  parameters:
  return:
  last updated: 21/07/00
 **********************************************************************/

void ga_multipoint_mutation(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of 'nucleotide' to mutate */
  int		dir=random_boolean()?-1:1;	/* The direction of drift. */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Copy chromosomes of parent to offspring. */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    }

/*
 * Mutate by tweaking nucleotides.
 */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      if (random_boolean_prob(GA_MULTI_BIT_CHANCE))
        {
        ((int *)son->chromosome[chromo])[point] += dir;

        if (((int *)son->chromosome[chromo])[point]==RAND_MAX)
          ((int *)son->chromosome[chromo])[point]=0;
        if (((int *)son->chromosome[chromo])[point]==-1)
          ((int *)son->chromosome[chromo])[point]=RAND_MAX-1;
        }
      }
    }

  return;
  }


/**********************************************************************
  ga_mutate_boolean_singlepoint()
  synopsis:	Cause a single mutation event in which a single
		nucleotide is inverted.
  parameters:
  return:
  last updated: 31/05/01
 **********************************************************************/

void ga_mutate_boolean_singlepoint(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of 'nucleotide' to mutate */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  chromo = random_int(pop->num_chromosomes);
  point = random_int(pop->len_chromosomes);

/* Copy unchanging data. */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(boolean));
    if (i!=chromo)
      {
      ga_copy_data(pop, son, father, i);
      }
    else
      {
      ga_copy_data(pop, son, NULL, i);
      }
    }

  ((boolean *)son->chromosome[chromo])[point] = !((boolean *)son->chromosome[chromo])[point];

  return;
  }


/**********************************************************************
  ga_mutate_boolean_multipoint()
  synopsis:	Cause a number of mutation events.
  parameters:
  return:
  last updated: 31/05/01
 **********************************************************************/

void ga_mutate_boolean_multipoint(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of 'nucleotide' to mutate */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Copy chromosomes of parent to offspring. */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(boolean));
    }

/*
 * Mutate by flipping random bits.
 */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      if (random_boolean_prob(GA_MULTI_BIT_CHANCE))
        {
        ((boolean *)son->chromosome[chromo])[point] = !((boolean *)son->chromosome[chromo])[point];
        }
      }
    }

  return;
  }


