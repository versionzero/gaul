/**********************************************************************
  struggle4.c
 **********************************************************************

  struggle4 - Test/example program for GAUL.
  Copyright ©2001-2003, Stewart Adcock <stewart@linux-domain.com>
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

		This program demonstrates the use of custom GA
		operator functions.  The long-hand method is used
		to set up the population's genomic form, rather than
		and of the high-level utility functions.

		This program aims to generate the final sentence from
		Chapter 3 of Darwin's "The Origin of Species",
		entitled "Struggle for Existence".

		This example is explained in docs/html/tutorial4.html

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
  struggle_adaptation()
  synopsis:	Adapt solution by making a single hill-climbing step
		for a randomly selected allele.
  parameters:
  return:
  updated:	07/07/01
 **********************************************************************/

entity *struggle_adaptation(population *pop, entity *child)
  {
  entity	*adult;		/* Adapted solution. */
  int		allele;		/* Randomly selected allele. */

/*
 * We must generate a new solution by copying the original solution.
 * This function copys all genomic, and if appropriate, phenomic data.
 * It is never safe to adapt the solution in place.
 */
  adult = ga_entity_clone(pop, child);

/* Make point mutation. */
  allele = random_int(strlen(target_text));
  ((char *)adult->chromosome[0])[allele]++;
  struggle_score(pop, adult);

  if (adult->fitness > child->fitness)
    return adult;

/* Searching in that previous direction didn't help. */
  ((char *)adult->chromosome[0])[allele]-=2;
  struggle_score(pop, adult);

  if (adult->fitness > child->fitness)
    return adult;

/* We must already be at a maxima. */
  ((char *)adult->chromosome[0])[allele]++;
  adult->fitness = child->fitness;

  return adult;
  }


/**********************************************************************
  struggle_seed()
  synopsis:	Entity initialisation.
  parameters:
  return:
  updated:	08/07/01
 **********************************************************************/

boolean struggle_seed(population *pop, entity *adam)
  {
  int           chromo;         /* Index of chromosome to seed */
  int           point;          /* Index of 'nucleotide' to seed */

/* Checks (Not really necessary here). */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (chromo=0; chromo<pop->num_chromosomes; chromo++)
    {
    for (point=0; point<pop->len_chromosomes; point++)
      {
      ((char *)adam->chromosome[chromo])[point]
            = random_int('~'-' ')+' ';
      }
    }

  return TRUE;
  }


/**********************************************************************
  struggle_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	08/07/01
 **********************************************************************/

void struggle_mutate(population *pop, entity *father, entity *son)
  {
  int           i;              /* Loop variable over all chromosomes */
  int           chromo;         /* Index of chromosome to mutate */
  int           point;          /* Index of 'nucleotide' to mutate */
  int           dir=random_boolean()?-1:1;      /* The direction of drift. */

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
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(char));
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
  ((char *)son->chromosome[chromo])[point] += dir;

  if (((char *)son->chromosome[chromo])[point]>'~')
    ((char *)son->chromosome[chromo])[point]=' ';
  if (((char *)son->chromosome[chromo])[point]<' ')
    ((char *)son->chromosome[chromo])[point]='~';

  return;
  }


/**********************************************************************
  struggle_crossover()
  synopsis:	A pair of crossover operators which will be called
		alternately through the use of cross definitions.
  parameters:
  return:
  updated:	07/07/01
 **********************************************************************/

void struggle_crossover( population *pop,
                        entity *father, entity *mother,
                        entity *son, entity *daughter );

void the_other_struggle_crossover( population *pop,
                        entity *father, entity *mother,
                        entity *son, entity *daughter )
  {
  ga_crossover_char_singlepoints(pop, father, mother, son, daughter);
  pop->crossover = struggle_crossover;

  return;
  }

void struggle_crossover( population *pop,
                        entity *father, entity *mother,
                        entity *son, entity *daughter )
  {
  ga_crossover_char_doublepoints(pop, father, mother, son, daughter);
  pop->crossover = the_other_struggle_crossover;

  return;
  }


/**********************************************************************
  struggle_generation_hook()
  synopsis:	This function is called by the main GA routine at the
		beginning of every generation.
  parameters:
  return:
  updated:	07/07/01
 **********************************************************************/

boolean struggle_generation_hook(int generation, population *pop)
  {

/*
 * Stop if we have the exact solution.
 */
  if (!strncmp(target_text,
               (char *)ga_get_entity_from_rank(pop,0)->chromosome[0],
               strlen(target_text)))
    {
    printf("Exact solution has been found!\n");
    return FALSE;
    }


  return TRUE;	/* TRUE indicates that evolution should continue. */
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	21 Aug 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;		/* Population structure. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */

  random_seed(23091975);

/*
 * Allocate a new popuation structure.
 * stable num. individuals = 100.
 * num. chromosomes = 1.
 * length of chromosomes = strlen(target_text);
 */
  pop = ga_population_new( 100, 1, (int) strlen(target_text) );

  if ( !pop ) die("Unable to allocate population.");

/*
 * Define chromosome handling functions.
 * Normally these functions would only be set manually when
 * creating a custom chromosome type.
 */
  pop->chromosome_constructor = ga_chromosome_char_allocate;
  pop->chromosome_destructor = ga_chromosome_char_deallocate;
  pop->chromosome_replicate = ga_chromosome_char_replicate;
  pop->chromosome_to_bytes = ga_chromosome_char_to_bytes;
  pop->chromosome_from_bytes = ga_chromosome_char_from_bytes;
  pop->chromosome_to_string = ga_chromosome_char_to_string;

/* Define all the needed callback functions. */
  pop->generation_hook = NULL;
  pop->iteration_hook = NULL;
  pop->data_destructor = NULL;
  pop->data_ref_incrementor = NULL;

/* Fitness evaluation. */
  pop->evaluate = struggle_score;

/* Individual initialisation. */
  pop->seed = struggle_seed;

/* Environmental adaptation operator. */
  pop->adapt = struggle_adaptation;

/* Mutation selection operator. */
  pop->select_one = ga_select_one_sus;

/* Crossover selection operator. */
  pop->select_two = ga_select_two_sus;

/* Mutation operator. */
  pop->mutate = struggle_mutate;

/* Crossover operator. */
  pop->crossover = struggle_crossover;

/* Replacement operator. (Only used in steady-state evolution) */
  pop->replace = NULL;

/*
 * Seed the initial population. (Could do this manually - it
 * just calls pop->seed() 100 times in this case.)
 */
  ga_population_seed(pop);

/*
 * Set the GA parameters.
 * Lamarkian evolution.
 * Parents may survive into next generation.
 * Crossover ratio = 0.9.
 * Mutation ratio = 0.1.
 * Migration ration = 0.0.
 *
 * Note that custom operator functions may choose to ignore these values.
 *
 * There are functions for setting these individually too.
 */
  ga_population_set_parameters( pop, GA_SCHEME_LAMARCK_CHILDREN, GA_ELITISM_PARENTS_DIE, 0.9, 0.1, 0.0 );

  ga_evolution( pop, 500 );

  printf( "The final solution with score %f was:\n",
          ga_get_entity_from_rank(pop,0)->fitness );
  beststring = ga_chromosome_char_to_string(pop, ga_get_entity_from_rank(pop,0), beststring, &beststrlen);
  printf("%s\n", beststring);

/*
 * This deallocates the population, all individuals, and
 * any genomic and phenomic data that may be associated.
 * (Not quite true - it actually dereferences any phenomic
 * data, which will then be deallocated if appropriate.)
 */
  ga_extinction(pop);

  s_free(beststring);
  
  exit(EXIT_SUCCESS);
  }



