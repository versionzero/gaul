/**********************************************************************
  chromostubs.c
 **********************************************************************

  chromostubs - Stubs for chromosome handling routines.
  Copyright ©2001-2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Stubs for the routines required by GAUL to handle
		user-defined chromosome types.  Note that the function
		names don't matter since they are just callback
		functions.  The prototypes do matter though.

		chromosome_XXX_allocate() - assign memory.
		chromosome_XXX_deallocate() - free memory.
		chromosome_XXX_replicate() - copy genetic information.
		chromosome_XXX_to_bytes() - for serialization.
		   (Leave max_bytes==0, if no need to free (i.e. static))
		chromosome_XXX_from_bytes() - for deserialization.
		chromosome_XXX_to_string() - Human readable NULL-
		   terminated string.

		You are welcome to cut'n'paste code for these functions
		from the GAUL library source, e.g. src/ga_chromo.c and
		friends.

		The examples below define some inefficent
		integer-valued array chromosomes.  pop.num_chromosomes
		and pop.len_chromosomes define the number and length
		of chromosomes in each entity.  In your code, you are
		free to ignore this values if they are inappropriate.

  To do:	Will need chromosome comparison functions.

 **********************************************************************/

#include "chromostubs.h"

/**********************************************************************
  chromosome_XXX_allocate()
  synopsis:	Allocate the chromosomes for an entity.  Initial
		contents are garbage (there is no need to zero them).
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

boolean chromosome_XXX_allocate(population *pop, entity *embryo)
  {
  int		i;	/* Loop variable over all chromosomes. */

  /* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!embryo) die("Null pointer to entity structure passed.");

  if (embryo->chromosome!=NULL)
    die("This entity already contains chromosomes.");

  embryo->chromosome = s_malloc(pop->num_chromosomes*sizeof(int *));

  for (i=0; i<pop->num_chromosomes; i++)
    embryo->chromosome[i] = s_malloc(pop->len_chromosomes*sizeof(int));

  return TRUE;
  }


/**********************************************************************
  chromosome_XXX_deallocate()
  synopsis:	Dellocate the chromosomes for an entity.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

void chromosome_XXX_deallocate(population *pop, entity *corpse)
  {
  int		i;	/* Loop variable over all chromosomes. */

  /* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!corpse) die("Null pointer to entity structure passed.");

  if (corpse->chromosome==NULL)
    die("This entity already contains no chromosomes.");

  /* Deallocate all associated memory. */
  for (i=0; i<pop->num_chromosomes; i++)
    s_free(corpse->chromosome[i]);

  s_free(corpse->chromosome);
  corpse->chromosome=NULL;	/* Must be set to NULL or else
				 * the code may attempt to reuse the
				 * deallocated memory, which would be
				 * bad!
				 */
  return;
  }


/**********************************************************************
  chromosome_XXX_replicate()
  synopsis:	Duplicate a single chromosome exactly.  The destination
		chromosome will already be allocated.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

void chromosome_XXX_replicate( population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid )
  {

  /* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!parent || !child) die("Null pointer to entity structure passed.");
  if (!parent->chromosome || !child->chromosome) die("Entity has no chromsomes.");

  memcpy( child->chromosome[chromosomeid],
          parent->chromosome[chromosomeid],
          pop->len_chromosomes * sizeof(int) );

  return;
  }


/**********************************************************************
  chromosome_XXX_to_bytes()
  synopsis:	Convert to contiguous form.  The array of bytes
		(actually unsigned chars) could be used for saving
		data to disk, or for inter-process communication.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

unsigned int chromosome_XXX_to_bytes(population *pop, entity *joe,
                                     byte **bytes, unsigned int *max_bytes)
  {
  int	num_bytes;	/* Actual size of genes. */
  int	i;		/* Loop variable over all chromosomes. */

  /* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  num_bytes = sizeof(int) * pop->len_chromosomes * pop->num_chromosomes;

  if (num_bytes>*max_bytes)
    {
    *max_bytes = num_bytes;
    *bytes = s_realloc(*bytes, *max_bytes);
    }

  /* Empty chromosomes are easy! */
  if (!joe->chromosome)
    {
    *bytes = (byte *)0;
    return 0;
    }

  for (i=0; i<pop->num_chromosomes; i++)
    memcpy( *bytes+i*pop->len_chromosomes,
            joe->chromosome[i],
            pop->len_chromosomes * sizeof(int) );

  return num_bytes;	/* This is the number of bytes actually used.
			 * Which will be no greater than max_bytes,
			 * unless max_bytes is zero.
			 */
  }


/**********************************************************************
  chromosome_XXX_from_bytes()
  synopsis:	Convert from contiguous form.  The reverse of
		chromosome_XXX_to_bytes().
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

void chromosome_XXX_from_bytes(population *pop, entity *joe, byte *bytes)
  {
  int	i;		/* Loop variable over all chromosomes. */

  /* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  if (!joe->chromosome) die("Entity has no chromsomes.");

  for (i=0; i<pop->num_chromosomes; i++)
    memcpy( joe->chromosome[i],
            *bytes+i*pop->len_chromosomes,
            pop->len_chromosomes * sizeof(int) );

  return;
  }


/**********************************************************************
  chromosome_XXX_to_string()
  synopsis:	Convert to human readable form.
  parameters:
  return:
  last updated: 21 Aug 2002
 **********************************************************************/

char *chromosome_XXX_to_string(
                              const population *pop, const entity *joe,
                              char *text, size_t *textlen)
  {

 nt           i, j;           /* Loop over chromosome, alleles. */
  int           k=0;            /* Pointer into 'text'. */
  int           l;              /* Number of appended digits. */

  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

/* Ensure that a reasonable amount of memory is allocated. */
  if (!text || *textlen < 8 * pop->len_chromosomes * pop->num_chromosomes)
    {
    *textlen = 8 * pop->len_chromosomes * pop->num_chromosomes;
    text = s_realloc(text, sizeof(char) * *textlen);
    }

/* Handle empty chromosomes. */
  if (!joe->chromosome)
    {
    text[1] = '\0';
    return text;
    }

  for(i=0; i<pop->num_chromosomes; i++)
    {
    for(j=0; j<pop->len_chromosomes; j++)
      {
      if (*textlen-k<8)
        {
        *textlen *= 2;   /* FIXME: This isn't intelligent. */
        text = s_realloc(text, sizeof(char) * *textlen);
        }

      l = snprintf(&(text[k]), *textlen-k, "%d ",
                       ((int *)joe->chromosome[i])[j]);

      if (l == -1)
        {       /* Truncation occured. */
        *textlen *= 2;  /* FIXME: This isn't intelligent. */
        text = s_realloc(text, sizeof(char) * *textlen);
        l = snprintf(&(text[k]), *textlen-k, "%d ",
                       ((int *)joe->chromosome[i])[j]);

        if (l == -1) die("Internal error, string truncated again.");
        }

      k += l;
      }
    }

/* Replace last space character with NULL character. */
  text[k-1]='\0';

  return text;
 }


/**********************************************************************
  chromosome_XXX_seed()
  synopsis:	Seed operator.  Initialise the contents of the
		chromosomes randomly.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

boolean chromosome_XXX_seed(population *pop, entity *adam)
  {
  int           chromo;         /* Index of chromosome to seed */
  int           point;          /* Index of 'nucleotide' to seed */

/* Sanity checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    for (point=0; point<pop->len_chromosomes; point++)
      ((int *)adam->chromosome[chromo])[point] = random_rand();

  return TRUE;
  }


/**********************************************************************
  chromosome_XXX_crossover()
  synopsis:     Crossover operator.  In this case, two chromosomes are
		combined by single-point crossover.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

void chromosome_XXX_crossover( population *pop,
                               int *father, int *mother,
                               int *son, int *daughter )
  {
  int   location;       /* Point of crossover */

  /* Sanity checks. */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to chromosome structure passed.");

  /* Choose crossover point and perform operation. */
  location=random_int(pop->len_chromosomes);

  memcpy(son, mother, location*sizeof(int));
  memcpy(daughter, father, location*sizeof(int));

  memcpy( &(son[location]), &(father[location]),
          (pop->len_chromosomes-location)*sizeof(int) );
  memcpy( &(daughter[location]), &(mother[location]),
          (pop->len_chromosomes-location)*sizeof(int) );

  return;
  }


/**********************************************************************
  chromosome_XXX_mutate()
  synopsis:     Mutation operator.  In this case a single nucleotide
		is randomized.
  parameters:
  return:
  last updated: 01/07/01
 **********************************************************************/

void chromosome_XXX_mutate( population *pop,
                                              entity *father, entity *son )
  {
  int           i;              /* Loop variable over all chromosomes */
  int           chromo;         /* Index of chromosome to mutate */
  int           point;          /* Index of 'nucleotide' to mutate */

/* Sanity checks. */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  chromo = random_int(pop->num_chromosomes);
  point = random_int(pop->len_chromosomes);

/* Copy unchanging data. */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy( son->chromosome[i], father->chromosome[i],
            pop->len_chromosomes*sizeof(int) );
    if (i!=chromo)
      ga_copy_data(pop, son, father, i);
    else
      ga_copy_data(pop, son, NULL, i);
    }

/* Cause the mutation. */
  ((int *)son->chromosome[chromo])[point] = random_int(RAND_MAX);

  return;
  }



