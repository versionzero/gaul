/**********************************************************************
  ga_similarity.c
 **********************************************************************

  ga_util - Genetic algorithm genome/chromosome comparison routines.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:     Routines for comparing genomes/chromosomes.

		Definitions:

		I define a pair of alleles to be matching if they are
		identical and not matching if the are different - even
		if the differing values are equivalent (i.e. if the
		modulus is significant rather than the actual value.)
		There is no concept of distance between alleles, they
		either match or they don't.

		Distance is obviously, somehow, related to negative
		similarity.

		'thing' is a chromosome or entity.
		n = total number of alleles in thing.
		M = total number of matching alleles in two things.

		Hamming coefficient = M/n
		Euclidean coefficient = sqrt(M/n)

  Warning:	THESE ARE CURRENTLY ONLY VALID FOR INTEGER CHROMOSOMES.

  References:	A general reference that I've found useful in the past
		but maybe is tricky to obtain, but is:
		Bradshaw J., "Introduction to Tversky similarity
		measure", MUG '97 - 11th annual Daylight user group
		meeting.

 **********************************************************************/

#include "ga_core.h"

/**********************************************************************
  ga_compare_genome()
  synopsis:	Compares two genotypes.  Simple memcmp() comparison of
		the chromosomes.
  parameters:	population *pop	Population of entities (you may use
			differing populations if they are "compatiable")
		entity *alpha	Test entity.
		entity *beta	Test entity.
  return:	Returns TRUE if all chromosomes are identical.
  last updated:	19/12/00
 **********************************************************************/

boolean ga_compare_genome(population *pop, entity *alpha, entity *beta)
  {
  int		i;		/* Loop variable over all chromosomes. */

  /* Checks */
  if (!alpha || !beta) die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
/*
 * FIXME: Here is a kludge.
 * This version is more efficient, but invalid when chromosomes have
 * differing lengths.  When differing length chromosomes are properly handled
 * this code should be changed.
*/
    if ( memcmp( alpha->chromosome[i], beta->chromosome[i], pop->len_chromosomes*sizeof(int) ) !=0 )

/* FIXME: This would require a dependance upon helga, need to modify the
 * population structure to hold this data.
    if ( memcmp( alpha->chromosome[i], beta->chromosome[i], helga_get_sequence_size(i)*sizeof(int) ) !=0 )
 */
      return FALSE;
    }

  return TRUE;
  }


/**********************************************************************
  ga_count_matching_alleles()
  synopsis:	Compares two chromosomes and counts matching alleles.
  parameters:	const int length	Chromosome length.
		const int *alpha	Alpha chromosome.
		const int *beta		Beta chromosome.
  return:	Returns number of matching alleles.
  last updated:	13/02/01
 **********************************************************************/

int ga_count_match_alleles(const int length, const int *alpha, const int *beta)
  {
  int		i;		/* Loop variable over all alleles. */
  int		count=0;	/* Number of matching alleles. */

  /* Checks. */
  if (!alpha || !beta) die("Null pointer to chromosome passed");

  for (i=0; i<length; i++)
    if (alpha[i] == beta[i]) count++;

  return count;
  }


/**********************************************************************
  ga_genome_hamming_similarity()
  synopsis:	Measures the Hamming coefficient of the genomes of
		a pair of entities.
  parameters:	population *pop	Population of entities (you may use
			differing populations if they are "compatiable")
		entity *alpha	Test entity.
		entity *beta	Test entity.
  return:	double	similarity
  last updated:	13/02/01
 **********************************************************************/

double ga_genome_hamming_similarity(population *pop, entity *alpha, entity *beta)
  {
  int		i;			/* Loop over chromosomes. */
  int		match=0;		/* Count of matching alleles. */
  int		length, total_length=0;	/* Number of alleles. */

  /* Checks */
  if (!alpha || !beta) die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
/*
 * FIXME: Here is a kludge.
 * This version is more efficient, but invalid when chromosomes have
 * differing lengths.  When differing length chromosomes are properly handled
 * this code should be changed.
*/
    length = pop->len_chromosomes;

/* FIXME: This would require a dependance upon helga, need to modify the
 * population structure to hold this data.
    length = helga_get_sequence_size(i);
 */

    total_length += length;
    match += ga_count_match_alleles(length, alpha->chromosome[i], beta->chromosome[i]);
    }

  return (double) match/total_length;
  }


/**********************************************************************
  ga_genome_euclidian_similarity()
  synopsis:	Measures the Euclidean coefficient of the genomes of
		a pair of entities.
  parameters:	population *pop	Population of entities (you may use
			differing populations if they are "compatiable")
		entity *alpha	Test entity.
		entity *beta	Test entity.
  return:	double	similarity
  last updated:	13/02/01
 **********************************************************************/

double ga_genome_euclidian_similarity(population *pop, entity *alpha, entity *beta)
  {
  return sqrt(ga_genome_hamming_similarity(pop, alpha, beta));
  }


