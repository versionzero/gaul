/**********************************************************************
  royalroad_bitstring.c
 **********************************************************************

  royalroad_bitstring - Test/example program for GAUL.
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

		The scoring function was adapted from GAlib 2.45,
		which states:

		This is the objective function for computing Holland's
		1993 ICGA version of the Royal Road problem.  It has
		been corrected per GAList volume 7 number 23, 8/26/93.
		No bonus points are awarded for a given level until it
		has been achieved (this fixes Holland's coding error in
		GAList).  Holland posed this problem as a challenge to
		test the performance of genetic algorithms.  He
		indicated that, with the parameter settings of:

		schemata size = 8
		bits between schemata = 7
		m* = 4
		U* = 1.0
		u = 0.3
		v = 0.02

		Holland could attain royal_road_level 3 most of the time
		within 10,000 function evaluations.  He challenged other
		GA users to match or beat that performance.  He
		indicated that he used a population size of 512 to
		obtain his solutions, and did NOT use a "simple genetic
		algorithm."

		The genome for this problem is a single-dimension
		bitstring with length defined by the block size and gap
		size as:

		    length = (blocksize+gapsize) * (2^K)

		where K = 1,2,3, or 4.  Holland used K = 4.

		This implementation uses a GAUL bitstring chromosome
		with a standard generational GA.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/*
 * Hard-coded parameter settings.
 * FIXME: Should make these user options.
 */
#define NBLOCKS		16	/* this number is 2^K */

#define BLOCKSIZE	8	/* block size - length of target schemata */
#define GAPSIZE		7	/* gap size - number of bits between target schemata */
#define MSTAR		4	/* Holland's m* - up to this many bits in low level block rewarded */
#define USTAR		1.0	/* Holland's U* - first block earns this */
#define RR_U		0.3	/* Holland's u - increment for lowest level match */
#define RR_V		0.02	/* Holland's v - reward/penalty per bit */

#define NBITS	(BLOCKSIZE+GAPSIZE)*NBLOCKS	/* Number of 'bits' in chromosomes. */

/* Global variable. */
int highestlevel=0;

/**********************************************************************
  royalroad_score()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	29 Jun 2003
 **********************************************************************/

boolean royalroad_score(population *pop, entity *entity)
  {
  double	score = 0.0;
  int		count, i, j, index, n;
  int		blockarray[NBLOCKS];
  int		proceed, level;

/* do the lowest level blocks first. */

  n = 0;
  for(i=0; i<NBLOCKS; i++) {
    count = 0;
    for(j=i*(BLOCKSIZE + GAPSIZE); j<i*(BLOCKSIZE+GAPSIZE)+BLOCKSIZE; j++)
      if(ga_bit_get((byte *)entity->chromosome[0], j) == 1) count++;  /* count the bits in the block. */
    if(count > MSTAR && count < BLOCKSIZE)
      score -= (count-MSTAR)*RR_V;
    else if(count <= MSTAR)
      score += count * RR_V;
    if(count == BLOCKSIZE) {
      blockarray[i] = 1;
      n++;
    }
    else{
      blockarray[i] = 0;
    }
  }

/* bonus for filled low-level blocks. */

  if(n > 0) score += USTAR + (n-1)*RR_U;

/* now do the higher-level blocks. */

  n = NBLOCKS;		/* n is now number of filled low level blocks. */
  proceed = 1;		/* should we look at the next higher level? */
  level = 0;
  while ((n > 1) && proceed) {
    proceed = 0;
    count = 0;
      /* there are n valid blocks in the blockarray each time */
      /* round, so n=2 is the last.                           */
    for(i=0,index=0; i<(n/2)*2; i+=2,index++) {
      if(blockarray[i] == 1 && blockarray[i+1] == 1) {
	count++;
	proceed = 1;
	blockarray[index] = 1;
      }
      else{
	blockarray[index] = 0;
      }
    }
    if (count > 0){
      score += USTAR + (count-1)*RR_U;
      level++;
    }
    n /= 2;
  }

  if (highestlevel < level) highestlevel = level;

  entity->fitness = score;

  return TRUE;
  }


/**********************************************************************
  royalroad_ga_callback()
  synopsis:	Analysis callback.
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

boolean royalroad_ga_callback(int generation, population *pop)
  {

  printf( "generation = %d best score = %f highestlevel = %d\n",
          generation,
          ga_get_entity_from_rank(pop,0)->fitness,
          highestlevel );

  return TRUE;
  }


/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	31/05/01
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;	/* Population of solutions. */
  int		seed=12345678;	/* Random number seed. */
  int		i;		/* Loop over alleles. */

  printf("Running Holland's Royal Road test problem with a genome that\n");
  printf("is %d bits long with %d blocks.  The parameters are:\n", NBITS, NBLOCKS);
  printf("  block size: %d\n", BLOCKSIZE);
  printf("  gap size  : %d\n", GAPSIZE);
  printf("  m*        : %d\n", MSTAR);
  printf("  u*        : %f\n", USTAR);
  printf("  u         : %f\n", RR_U);
  printf("  v         : %f\n", RR_V);
  printf("\n");
  printf("Random number seed is %d\n", seed);
  printf("\n");

  random_seed(seed);

  pop = ga_genesis_bitstring(
     512,			/* const int              population_size */
     1,				/* const int              num_chromo */
     NBITS,			/* const int              len_chromo */
     royalroad_ga_callback,	/* GAgeneration_hook      generation_hook */
     NULL,			/* GAiteration_hook       iteration_hook */
     NULL,			/* GAdata_destructor      data_destructor */
     NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
     royalroad_score,		/* GAevaluate             evaluate */
     ga_seed_bitstring_random,	/* GAseed                 seed */
     NULL,			/* GAadapt                adapt */
     ga_select_one_bestof2,	/* GAselect_one           select_one */
     ga_select_two_bestof2,	/* GAselect_two           select_two */
     ga_mutate_bitstring_singlepoint,	/* GAmutate               mutate */
     ga_crossover_bitstring_doublepoints,	/* GAcrossover            crossover */
     NULL,			/* GAreplace              replace */
     NULL			/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,			/* population      *pop */
       GA_SCHEME_DARWIN,	/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_DIE,	/* const ga_elitism_type   elitism */
       0.9,			/* double  crossover */
       0.1,			/* double  mutation */
       0.0              	/* double  migration */
                              );

  ga_evolution(
       pop,		/* population              *pop */
       100		/* const int               max_generations */
              );

  printf("The final solution with seed = %d was: \n", seed);
  for (i=0; i<NBITS; i++)
    printf("%d", ga_bit_get((byte *)ga_get_entity_from_rank(pop,0)->chromosome[0],i)?1:0);
  printf("\nscore = %f highestlevel = %d\n",
         ga_get_entity_from_rank(pop,0)->fitness,
         highestlevel);

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


