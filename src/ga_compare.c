/**********************************************************************
  ga_compare.c
 **********************************************************************

  ga_compare - genomic comparison routines.
  Copyright ©2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Routines for comparing genomes.

  To do:	Equivalent functions for the other chromosome types.

 **********************************************************************/

#include "gaul/ga_core.h"

/**********************************************************************
  ga_compare_char()
  synopsis:	Compares two char-array genomes and returns hamming
		distance.
  parameters:	population *pop	Population of entities (you may use
			differing populations if they are "compatible")
		entity *alpha	Test entity.
		entity *beta	Test entity.
  return:	Returns Hamming distance between two entities' genomes.
  last updated:	21 May 2003
 **********************************************************************/

double ga_compare_char_hamming(population *pop, entity *alpha, entity *beta)
  {
  int		i,j;		/* Loop variable over all chromosomes, alleles. */
  int		dist=0;		/* Genomic distance. */
  char		*a, *b;		/* Pointers to chromosomes. */

  /* Checks */
  if (!alpha || !beta) die("Null pointer to entity structure passed");

  for (i=0; i<pop->num_chromosomes; i++)
    {
    a = (char *)(alpha->chromosome[i]);
    b = (char *)(beta->chromosome[i]);

    for (j=0; j<pop->len_chromosomes; j++)
      {
      dist += abs((int)a[j]-b[j]);
      }
    }

  return (double) dist;
  }


