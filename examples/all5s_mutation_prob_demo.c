/**********************************************************************
  all5s_mutation_prob_demo.c
 **********************************************************************

  all5s - Test/example program for GAUL.
  Copyright ©2004-2005, Stewart Adcock <stewart@linux-domain.com>
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

		This searches for a set of values all equalling '5', 
		with allowed bounds of 0 to 10 inclusive.
		It is an example to demonstrate how to impose bounds
		on the chromosome values by using custom seed and
		mutation operator functions.

 **********************************************************************/

#include <gaul.h>

/**********************************************************************
  all5s_score()
  synopsis:	Score solution (Normalised RMSD deviation from values
		of '5' for all parameters).
  parameters:
  return:
  updated:	20 Sep 2004
 **********************************************************************/

static boolean all5s_score(population *pop, entity *entity)
  {
  int		allele;
  int		k;

  entity->fitness = 0.0;

  /* Loop over alleles in chromosome. */
  for (k = 0; k < pop->len_chromosomes; k++)
    {
    allele = ((int *)entity->chromosome[0])[k];
    entity->fitness += (5-allele)*(5-allele);
    }

  entity->fitness = sqrt(entity->fitness);

/* Normalize fitness so larger positive values are better. */
  entity->fitness = 1.0/(1.0+entity->fitness);

  return TRUE;
  }


/**********************************************************************
  all5s_seed()
  synopsis:	Entity initialisation.
  parameters:
  return:
  updated:	20 Sep 2004
 **********************************************************************/

static boolean all5s_seed(population *pop, entity *entity)
  {
  int           point;          /* Index of allele to seed */

  for (point=0; point<pop->len_chromosomes; point++)
    {
    ((int *)entity->chromosome[0])[point] = random_int(11);
    }

  return TRUE;
  }


/**********************************************************************
  all5s_mutate()
  synopsis:	Mutation of a single parameter with wrapping 0<->10.
  parameters:
  return:
  updated:	20 Sep 2004
 **********************************************************************/

static void all5s_mutate(population *pop, entity *father, entity *son)
  {
  int           i;              /* Loop variable over all chromosomes */
  int           point;          /* Index of allele to mutate */
  int           dir=random_boolean()?-1:1;      /* The direction of drift. */

/* Sanity check. */
  if (!father || !son)
    die("Null pointer to entity structure passed");

/* Select mutation locus. */
  point = random_int(pop->len_chromosomes);

/* Copy genome from father. */
  for (i=0; i<pop->len_chromosomes; i++)
    ((int *)son->chromosome[0])[i] = ((int *)father->chromosome[0])[i];

/*
 * Mutate by tweaking a single allele.
 */
  ((int *)son->chromosome[0])[point] += dir;

  if (((int *)son->chromosome[0])[point]>10)
    ((int *)son->chromosome[0])[point]=0;
  else if (((int *)son->chromosome[0])[point]<0)
    ((int *)son->chromosome[0])[point]=10;

  return;
  }


/**********************************************************************
  main()
  synopsis:
  parameters:
  return:
  updated:	20 Sep 2004
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;		/* Population of solutions. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */

  random_seed(20092004);

  pop = ga_genesis_integer(
     200,			/* const int              population_size */
     1,				/* const int              num_chromo */
     100,			/* const int              len_chromo */
     NULL,			/* GAgeneration_hook      generation_hook */
     NULL,			/* GAiteration_hook       iteration_hook */
     NULL,			/* GAdata_destructor      data_destructor */
     NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
     all5s_score,		/* GAevaluate             evaluate */
     all5s_seed,		/* GAseed                 seed */
     NULL,			/* GAadapt                adapt */
     ga_select_one_sus,		/* GAselect_one           select_one */
     ga_select_two_sus,		/* GAselect_two           select_two */
     all5s_mutate,		/* GAmutate               mutate */
     ga_crossover_integer_singlepoints,	/* GAcrossover            crossover */
     NULL,			/* GAreplace		replace */
     NULL			/* vpointer		User data */
          );

  ga_population_set_parameters(
     pop,			/* population      *pop */
     GA_SCHEME_DARWIN,		/* const ga_scheme_type  scheme */
     GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
     0.8,			/* double		 crossover */
     0.05,			/* double		 mutation */
     0.0              		/* double		 migration */
                            );

  ga_evolution(
     pop,			/* population              *pop */
     250			/* const int               max_generations */
            );

/* Display final solution. */
  printf("The final solution was:\n");
  beststring = ga_chromosome_integer_to_string(pop, ga_get_entity_from_rank(pop,0), beststring, &beststrlen);
  printf("%s\n", beststring);
  printf("With score = %f\n", ga_get_entity_from_rank(pop,0)->fitness);

/* Free memory. */
  ga_extinction(pop);
  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


