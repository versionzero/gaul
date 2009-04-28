/**********************************************************************
  mixed.c
 **********************************************************************

  mixed - Test/example program for GAUL.
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

  Synopsis:	Test/example program for GAUL.

		This example demonstrates the use of mixed chromosome
		types.  It is a modified implementation of the "onemax"
		problem; it has a second chromosome with a harmonic
		function with the maxima at 4.49 if the first
		chromosome has an associated allele value of 1.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/*
 * Hard-coded parameter settings.
 * FIXME: Should make these user options.
 */
#define NBITS		48	/* Number of 'bits' in chromosomes. */

/**********************************************************************
  mixed_score()
  synopsis:	Score solution.  If an allele in the first chromosome
		has the value '1', then score the corresponding allele
		on the second chromosome using a simple harmonic.
  parameters:	population *pop
		entity *entity
  return:	always TRUE
  updated:	20 Aug 2003
 **********************************************************************/

static boolean mixed_score(population *pop, entity *entity)
  {
  int		i;
  double	score = 0.0;

  for(i=0; i<NBITS; i++)
    {
    if(ga_bit_get(((byte *)entity->chromosome[0]), i) == 1)
      {
      score += 100.0-MIN(100.0,SQU(((double *)entity->chromosome[1])[i]-4.49));
      }
    }

  entity->fitness = score;

  return TRUE;
  }


/**********************************************************************
  mixed_chromosome_allocate()
  synopsis:	Allocate the chromosomes for an entity.  Initial
		contents are garbage (there is no need to zero them).
  parameters:
  return:
  last updated: 20 Aug 2003
 **********************************************************************/

static boolean mixed_chromosome_allocate(population *pop, entity *embryo)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!embryo) die("Null pointer to entity structure passed.");

  if (embryo->chromosome!=NULL)
    die("This entity already contains chromosomes.");

  embryo->chromosome = s_malloc(2*sizeof(void *));
  embryo->chromosome[0] = ga_bit_new(pop->len_chromosomes);
  embryo->chromosome[1] = s_malloc(pop->len_chromosomes*sizeof(double));

  return TRUE;
  }


/**********************************************************************
  mixed_chromosome_deallocate()
  synopsis:	Dellocate the chromosomes for an entity.
  parameters:
  return:
  last updated: 20 Aug 2003
 **********************************************************************/

static void mixed_chromosome_deallocate(population *pop, entity *corpse)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!corpse) die("Null pointer to entity structure passed.");

  if (corpse->chromosome==NULL)
    die("This entity already contains no chromosomes.");

  ga_bit_free(corpse->chromosome[0]);
  s_free(corpse->chromosome[1]);
  s_free(corpse->chromosome);
  corpse->chromosome=NULL;

  return;
  }


/**********************************************************************
  mixed_chromosome_replicate()
  synopsis:	Duplicate a chromosome exactly.
  parameters:
  return:
  last updated: 20 Aug 2003
 **********************************************************************/

static void mixed_chromosome_replicate( const population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid )
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!parent || !child) die("Null pointer to entity structure passed.");
  if (!parent->chromosome || !child->chromosome) die("Entity has no chromsomes.");

  if (chromosomeid == 0)
    ga_bit_clone( child->chromosome[0],
                  parent->chromosome[0],
                  pop->len_chromosomes );
  else if (chromosomeid == 1)
    memcpy( child->chromosome[1], parent->chromosome[1],
            pop->len_chromosomes * sizeof(double) );
  else
    die("Only 2 chromosome exist.");

  return;
  }


/**********************************************************************
  mixed_seed()
  synopsis:     Randomly seed chromosomes.
  parameters:   population *pop
                entity *adam
  return:       success
  last updated: 20 Aug 2003
 **********************************************************************/

static boolean mixed_seed(population *pop, entity *adam)
  {
  int           point;          /* Index of 'nucleotide' to seed */

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  for (point=0; point<pop->len_chromosomes; point++)
    {
    ga_bit_randomize((byte *)adam->chromosome[0],point);
    ((double *)adam->chromosome[1])[point] = random_double_range(0.0,10.0);
    }

  return TRUE;
  }


/**********************************************************************
  mixed_crossover()
  synopsis:     `Mates' two genotypes by randomizing the parents
                alleles.
  parameters:
  return:
  last updated: 20 Aug 2003
 **********************************************************************/

static void mixed_crossover( population *pop,
                      entity *father, entity *mother,
                      entity *son, entity *daughter )
  {
  int           j;           /* Loop over all chromosomes, alleles. */

  /* Checks. */
  if (!father || !mother || !son || !daughter)
    die("Null pointer to entity structure passed.");

  for (j=0; j<pop->len_chromosomes; j++)
    {
    if (random_boolean())
      {
      if (ga_bit_get(father->chromosome[0],j))
        ga_bit_set(son->chromosome[0],j);
      else
        ga_bit_clear(son->chromosome[0],j);

      if (ga_bit_get(mother->chromosome[0],j))
        ga_bit_set(daughter->chromosome[0],j);
      else
        ga_bit_clear(daughter->chromosome[0],j);

      ((double *)son->chromosome[1])[j] = ((double *)father->chromosome[1])[j];
      ((double *)daughter->chromosome[1])[j] = ((double *)mother->chromosome[1])[j];
      }
    else
      {
      if (ga_bit_get(father->chromosome[0],j))
        ga_bit_set(daughter->chromosome[0],j);
      else
        ga_bit_clear(daughter->chromosome[0],j);

      if (ga_bit_get(mother->chromosome[0],j))
        ga_bit_set(son->chromosome[0],j);
      else
        ga_bit_clear(son->chromosome[0],j);

      ((double *)daughter->chromosome[1])[j] = ((double *)father->chromosome[1])[j];
      ((double *)son->chromosome[1])[j] = ((double *)mother->chromosome[1])[j];
      }
    }

  return;
  }


/**********************************************************************
  mixed_mutation()
  synopsis:     Mutation.
  parameters:
  return:
  last updated: 20 Aug 2003
 **********************************************************************/

static void mixed_mutation( population *pop,
                     entity *father, entity *son )
  {
  int           point;          /* Index of allele to mutate */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  point = random_int(pop->len_chromosomes);

/* Copy unchanging data. */
  ga_bit_clone(son->chromosome[0], father->chromosome[0], pop->len_chromosomes);
  memcpy(son->chromosome[1], father->chromosome[1], pop->len_chromosomes*sizeof(double));

/* A singlepoint mutation. */
  if (random_boolean())
    {
    ga_bit_invert(son->chromosome[0],point);
    }
  else
    {
    ((double *)son->chromosome[1])[point] = random_double_range(0.0,10.0);
    }

  return;
  }


/**********************************************************************
  mixed_genesis()
  synopsis:     Create a new population and perform the basic setup
		(i.e. initial seeding) required for further
		optimisation and manipulation.
  parameters:
  return:       population, or NULL on failure.
  last updated: 20 Aug 2003
 **********************************************************************/

static population *mixed_genesis( void )
  {
  population    *pop;   /* The new population structure. */

/*
 * Allocate and initialise a new population.
 */
  if ( !(pop = ga_population_new( 100, 2, NBITS )) )
    return NULL;

/*
 * Define some callback functions.
 */
  pop->chromosome_constructor = mixed_chromosome_allocate;
  pop->chromosome_destructor = mixed_chromosome_deallocate;
  pop->chromosome_replicate = mixed_chromosome_replicate;
  pop->chromosome_to_bytes = NULL;
  pop->chromosome_from_bytes = NULL;
  pop->chromosome_to_string = NULL;

/*
 * Define GA operators.
 */
  pop->evaluate = mixed_score;
  pop->seed = mixed_seed;
  pop->select_one = ga_select_one_roulette;
  pop->select_two = ga_select_two_roulette;
  pop->mutate = mixed_mutation;
  pop->crossover = mixed_crossover;

  ga_population_seed(pop);

  return pop;
  }


/**********************************************************************
  main()
  synopsis:	Demonstrate mixed chromosome types.
  parameters:
  return:
  updated:	20 Aug 2003
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;	/* Population of solutions. */
  int		seed=20030820;	/* Random number seed. */
  int		i;		/* Loop over alleles. */

  printf("This is a GAUL example program to demonstrate mixing of chromosome types.\n");
  printf("One chromosome contains %d bits and another contains %d doubles.\n", NBITS, NBITS);
  printf("\n");
  printf("Random number seed is %d\n", seed);
  printf("\n");

  random_seed(seed);

  pop = mixed_genesis();

  ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_DIE,	/* const ga_elitism_type   elitism */
       0.8,			/* double  crossover */
       0.2,			/* double  mutation */
       0.0              	/* double  migration */
                              );

  ga_evolution(
       pop,		/* population              *pop */
       1000		/* const int               max_generations */
              );

  printf("The final solution with seed = %d had score %8.0f\n",
         seed, ga_get_entity_from_rank(pop,0)->fitness);
  for (i=0; i<NBITS; i++)
    printf("%d", ga_bit_get((byte *)ga_get_entity_from_rank(pop,0)->chromosome[0],i)?1:0);
  printf("\n");
  for (i=0; i<NBITS; i++)
    printf("%f ", ((double *)ga_get_entity_from_rank(pop,0)->chromosome[1])[i]);
  printf("\n");

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


