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

  Updated:	02/05/01 SAA	Remived some "fixme" comments, which had already been fixed.
		23/04/01 SAA	Split from ga_util.c for ease of maintainance.  These functions now have no return value.

 **********************************************************************/

#include "ga_util.h"

#if 0
/**********************************************************************
  ga_helixswap_mutation()
  synopsis:	Cause a pair of helices to exchange locations.
		FIXME: Should be user specified specialist callback
		function instead.
  parameters:
  return:
  last updated: 09/07/00
 **********************************************************************/

boolean ga_helixswap_mutation(population *pop, entity *father, entity *son)
  {
  int		i;			/* Loop over all chromosomes */
  int		chromo1, chromo2;	/* Indices of chromosomes to swap */

  /* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    }

  /* More checks. */
  if (pop->num_chromosomes < 2)
    {
    plog(LOG_WARNING, "ga_helixswap_mutation() requires more than 1 chromosome.");
    return FALSE;
    }

  chromo1 = random_int(pop->num_chromosomes);
  do
    {
    chromo2 = random_int(pop->num_chromosomes);
    } while (chromo1 == chromo2);

/*
 * Mutate by swaping regions of chromosomes which
 * encode the helix position and orientation.
 */
  for(i=0; i<7; i++)
    {
    son->chromosome[chromo1][i] = father->chromosome[chromo2][i];
    son->chromosome[chromo2][i] = father->chromosome[chromo1][i];
    }

  for (i=0; i<pop->num_chromosomes; i++)
    {
    if (i!=chromo1 && i!=chromo2)
      {
      ga_copy_data(pop, son, father, i);
      }
    else
      {
      ga_copy_data(pop, son, NULL, i);
      }
    }

  return TRUE;
  }


/**********************************************************************
  ga_helixthread_mutation()
  synopsis:	Cause a helix threading mutation event.  i.e. a helix
		twists by one residue in either direction.
		FIXME: Should be user function specified by a callback
		instead.
  parameters:
  return:
  last updated: 27/07/00
 **********************************************************************/

boolean ga_helixthread_mutation(entity *father, entity *son)
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  vector_d3	com, axis, shift;	/* Helical parameters */
  double	angle;		/* Rotation angle around helix axis */
  vector_d3	orig_com;	/* Original center of mass */
  quaternion	orig_q;		/* Original orientation */
  quaternion	q;		/* Helix axis */
  int		*chromoptr;	/* Pointer into current chromosome */
/* DEBUG STUFF */
  int		num_coords, max_coords;
  vector_d3	*coords=NULL;

  /* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    }

  chromo = random_int(pop->num_chromosomes);

/*
 * Mutate.  This has the effect of twisting a helix by one residue.
 */

/*
 * Twist around helix axis:
 * (a) Find Helix axis and COM.
 * (b) Twist around axial vector. ?? degrees.
 * (c) Translate along axial vector. ?? Ang.
 * (d) Write encode and store new helix axis and COM.
 */
/* DEBUG START */
  coords = mol_molstruct_get_coords_by_label(
                        global_mol, "CA", coords,
                        &num_coords, &max_coords);
/*
  helix_kearsleyfit( helga_get_sequence_size(chromo),
                coords,
                &com, &axis, &shift, &angle);
*/
  helix_kearsleyfit( helga_get_sequence_size(chromo),
                     coords, &com, &axis );

/* Update center of mass, axis */
  v3_axis_to_quaternion(&q, angle, axis);
/* DEBUG END */

  chromoptr = son->chromosome[chromo].data[son->chromosome[chromo]);

  *chromoptr++ = helga_coord_to_int(shift.x+helga_int_to_coord(*chromoptr));
  *chromoptr++ = helga_coord_to_int(shift.y+helga_int_to_coord(*chromoptr));
  *chromoptr++ = helga_coord_to_int(shift.z+helga_int_to_coord(*chromoptr));
  orig_q.w = helga_int_to_quaternion( chromoptr[0] );
  orig_q.x = helga_int_to_quaternion( chromoptr[1] );
  orig_q.y = helga_int_to_quaternion( chromoptr[2] );
  orig_q.z = helga_int_to_quaternion( chromoptr[3] );
  quaternion_multiply(&q, &orig_q, &q);
  *chromoptr++ = helga_quaternion_to_int( q.w );
  *chromoptr++ = helga_quaternion_to_int( q.x );
  *chromoptr++ = helga_quaternion_to_int( q.y );
  *chromoptr++ = helga_quaternion_to_int( q.z );

  return TRUE;
  }
#endif


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
/*
  pop->drift_nucleotide(chromo, point, son->chromosome[chromo]);
*/
  son->chromosome[chromo][point] += dir;

  if (son->chromosome[chromo][point]==RAND_MAX) son->chromosome[chromo][point]=0;
  if (son->chromosome[chromo][point]==-1) son->chromosome[chromo][point]=RAND_MAX-1;

  return;
  }


#if 0
/**********************************************************************
  ga_monte_carlo_mutation()
  synopsis:	Cause a mutation event based on a Monte Carlo move.
  parameters:
  return:
  last updated: 23/02/01
 **********************************************************************/

boolean ga_monte_carlo_mutation(population *pop, entity *father, entity *son)
  {
  int		i;		/* Loop variable over chromosomes. */

/*
 * Duplicate the initial data.
 */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    ga_copy_data(pop, son, father, i);
    }

/*
 * Mutate by applying a Monte Carlo move.
 */
  return pop->monte_carlo_move(pop, son);
  }
#endif


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

/*
  pop->randomize_nucleotide(chromo, point, son->chromosome[chromo]);
 */
  son->chromosome[chromo][point] = random_int(RAND_MAX);

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
/*
        pop->drift_nucleotide(chromo, point, son->chromosome[chromo]);
*/

        son->chromosome[chromo][point] += dir;

        if (son->chromosome[chromo][point]==RAND_MAX) son->chromosome[chromo][point]=0;
        if (son->chromosome[chromo][point]==-1) son->chromosome[chromo][point]=RAND_MAX-1;
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

  son->chromosome[chromo][point] = !son->chromosome[chromo][point];

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
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
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
        son->chromosome[chromo][point] = !son->chromosome[chromo][point];
        }
      }
    }

  return;
  }


