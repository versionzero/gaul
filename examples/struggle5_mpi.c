/**********************************************************************
  struggle5.c
 **********************************************************************

  struggle5 - Test/example program for GAUL.
  Copyright ©2001-2005, Stewart Adcock <stewart@linux-domain.com>
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

		This example shows the use of multiple populations
		with GAUL's so called "archipelago" scheme.  This is
		the basic island model of evolution.  It uses the
		MPI-parallelised version of ga_evolution().

		This program aims to generate the final sentence from
		Chapter 3 of Darwin's "The Origin of Species",
		entitled "Struggle for Existence".

		This example is explained in
		http://gaul.sourceforge.net/tutorial/island.html
                and
		http://gaul.sourceforge.net/tutorial/multiprocessor.html

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

#if HAVE_MPI != 1

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
  {
  printf("GAUL was not compiled with MPI support.\n");

  exit(EXIT_FAILURE);
  }

#else

/* Specify the number of populations (islands) to use. */
#define GA_STRUGGLE_NUM_POPS	5

/*
 * The solution string.
 */
static char *target_text="When we reflect on this struggle, we may console ourselves with the full belief, that the war of nature is not incessant, that no fear is felt, that death is generally prompt, and that the vigorous, the healthy, and the happy survive and multiply.";


/**********************************************************************
  struggle_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	16/06/01
 **********************************************************************/

static boolean struggle_score(population *pop, entity *entity)
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
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	19 Feb 2005
 **********************************************************************/

int main(int argc, char **argv)
  {
  int		i;				/* Loop over populations. */
  population	*pop[GA_STRUGGLE_NUM_POPS];	/* Array of populations. */
  population	*slavepop;			/* Population for slave calculations. */
  char		*beststring=NULL;		/* Human readable form of best solution. */
  size_t	beststrlen=0;			/* Length of beststring. */
  int		rank;				/* MPI rank. */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("Process %d initialised (rank %d)\n", getpid(), rank);

  random_seed(42);

  if (rank != 0)
    { /* This is a slave process. */

/*
 * A population is created so that the callbacks are defined.  Evolution doesn't
 * occur with this population, so population_size can be zero.  In such a case,
 * no entities are ever seeded, so there is no significant overhead.
 * Strictly, several of these callbacks are not needed on the slave processes, but
 * their definition doesn't have any adverse effects.
 */
    slavepop = ga_genesis_char(
            0,					/* const int              population_size */
            1,					/* const int              num_chromo */
            (int) strlen(target_text),		/* const int              len_chromo */
            NULL,		 		/* GAgeneration_hook      generation_hook */
            NULL,				/* GAiteration_hook       iteration_hook */
            NULL,				/* GAdata_destructor      data_destructor */
            NULL,				/* GAdata_ref_incrementor data_ref_incrementor */
            struggle_score,			/* GAevaluate             evaluate */
            ga_seed_printable_random,		/* GAseed                 seed */
            NULL,				/* GAadapt                adapt */
            ga_select_one_sus,			/* GAselect_one           select_one */
            ga_select_two_sus,			/* GAselect_two           select_two */
            ga_mutate_printable_singlepoint_drift,	/* GAmutate               mutate */
            ga_crossover_char_allele_mixing,	/* GAcrossover            crossover */
            NULL,				/* GAreplace		replace */
            NULL				/* vpointer		User data */
            );

    printf("DEBUG: Attaching process %d\n", rank);
    ga_attach_mpi_slave( slavepop );		/* The slaves halt here until ga_detach_mpi_slaves(), below, is called. */
    }
  else
    {
/*
 * This is the master process.  Other than calling ga_evolution_mpi() instead
 * of ga_evolution(), there are no differences between this code and the usual
 * GAUL invocation.
 */

    for (i=0; i<GA_STRUGGLE_NUM_POPS; i++)
      {
      pop[i] = ga_genesis_char(
            80,			/* const int              population_size */
            1,			/* const int              num_chromo */
            (int) strlen(target_text),	/* const int              len_chromo */
            NULL,		 	/* GAgeneration_hook      generation_hook */
            NULL,			/* GAiteration_hook       iteration_hook */
            NULL,			/* GAdata_destructor      data_destructor */
            NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
            struggle_score,		/* GAevaluate             evaluate */
            ga_seed_printable_random,	/* GAseed                 seed */
            NULL,			/* GAadapt                adapt */
            ga_select_one_sus,	/* GAselect_one           select_one */
            ga_select_two_sus,	/* GAselect_two           select_two */
            ga_mutate_printable_singlepoint_drift,	/* GAmutate       mutate */
            ga_crossover_char_allele_mixing,	/* GAcrossover            crossover */
            NULL,			/* GAreplace		replace */
            NULL			/* vpointer		User data */
            );

      ga_population_set_parameters( pop[i], GA_SCHEME_DARWIN, GA_ELITISM_PARENTS_DIE, 0.75, 0.25, 0.001 );
      }

    ga_evolution_archipelago_mpi( GA_STRUGGLE_NUM_POPS, pop, 250 );

    for (i=0; i<GA_STRUGGLE_NUM_POPS; i++)
      {
      printf( "The best solution on island %d with score %f was:\n",
              i, ga_get_entity_from_rank(pop[i],0)->fitness );
      beststring = ga_chromosome_char_to_string(pop[i], ga_get_entity_from_rank(pop[i],0), beststring, &beststrlen);
      printf("%s\n", beststring);

      ga_extinction(pop[i]);
      }

    s_free(beststring);

    ga_detach_mpi_slaves();	/* Allow all slave processes to continue. */
    }

  MPI_Finalize();

  exit(EXIT_SUCCESS);
  }

#endif /* #if HAVE_MPI != 1 */

