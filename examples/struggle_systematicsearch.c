/**********************************************************************
  struggle_systematicsearch.c
 **********************************************************************

  struggle - Test/example program for GAUL.
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

  Synopsis:	Test/example program for GAUL.

		This program uses a systematic searching approach
		to solving the problem tackled by all of the struggle*
		examples.

		This program aims to generate the final sentence from
		Chapter 3 of Darwin's "The Origin of Species",
		entitled "Struggle for Existence".

		This example is explained in docs/html/tutorial/simple.html

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/*
 * The solution string.
 */
char *target_text="When we reflect on this struggle, we may console ourselves with the full belief, that the war of nature is not incessant, that no fear is felt, that death is generally prompt, and that the vigorous, the healthy, and the happy survive and multiply.";


/**********************************************************************
  struggle_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	16/06/01
 **********************************************************************/

boolean struggle_score(population *pop, entity *entity)
  {
  int		k;		/* Loop variable over all alleles. */

  entity->fitness = 0.0;

  /* Loop over alleles in chromosome. */
  for (k = 0; k < pop->len_chromosomes; k++)
    {
    if ( ((char *)entity->chromosome[0])[k] == target_text[k])
      entity->fitness+=1.0;
    /*
     * Component to smooth function, which helps a lot in this case:
     * Comment it out if you like.
     */
    entity->fitness += (127.0-abs((int)(((char *)entity->chromosome[0])[k]-target_text[k])))/50.0;
    }

  return TRUE;
  }


/**********************************************************************
  struggle_scan_chromosome()
  synopsis:	Allele combination enumeration function.
  parameters:
  return:
  updated:	12 Nov 2002
 **********************************************************************/

boolean struggle_scan_chromosome(population *pop, entity *entity, int enumeration)
  {
  int		i=0, j=0;	/* Loop variables over all chromosomes, alleles. */

  (((char *)entity->chromosome[0])[0])++;
  
  while (((char *)entity->chromosome[i])[j]>'~')
    {
    ((char *)entity->chromosome[i])[j] = ' ';
    j++;

    if (j == pop->len_chromosomes)
      {
      j = 0;
      i++;

      if (i == pop->num_chromosomes)
        {
        return TRUE;	/* Enumeration is now complete! */
        }
      }

    (((char *)entity->chromosome[i])[j])++;
    }

/* Enumeration is not complete. */
  return FALSE;
  }


/**********************************************************************
  struggle_seed()
  synopsis:	Need to seed with first allele permutation.
  parameters:
  return:
  updated:      12 Nov 2002
 **********************************************************************/

boolean struggle_seed(population *pop, entity *entity)
  {
  int           i=0, j=0;       /* Loop variables over all chromosomes, alleles. */

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  while (i<pop->num_chromosomes)
    {
    while (j<pop->len_chromosomes)
      {
      ((char *)entity->chromosome[i])[j] = ' ';
      j++;
      }
    i++;
    }

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	12 Nov 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;		/* Population of solutions. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */
  entity	*solution;		/* Solution to problem. */
  int		num_iterations;		/* Number of iterations required. */

  random_seed(23091975);

  pop = ga_genesis_char(
       100,			/* const int              population_size */
       1,			/* const int              num_chromo */
       strlen(target_text),	/* const int              len_chromo */
       NULL,		 	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       struggle_score,		/* GAevaluate             evaluate */
       struggle_seed,		/* GAseed                 seed */
       NULL,			/* GAadapt                adapt */
       NULL,			/* GAselect_one           select_one */
       NULL,			/* GAselect_two           select_two */
       NULL,			/* GAmutate               mutate */
       NULL,			/* GAcrossover            crossover */
       NULL,			/* GAreplace		replace */
       NULL			/* vpointer		User data */
            );

  ga_population_set_search_parameters(pop, struggle_scan_chromosome);

  solution = ga_get_free_entity(pop);

  num_iterations = ga_search(
       pop,		/* population      *pop */
       solution		/* entity          *entity */
            );

  printf( "The final solution was:\n");
  beststring = ga_chromosome_char_to_string(pop, solution, beststring, &beststrlen);
  printf("%s\n", beststring);
  printf( "With score = %f\n", ga_entity_get_fitness(solution) );
  printf( "This required %d iterations\n", num_iterations);

  ga_extinction(pop);
  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


