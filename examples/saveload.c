/**********************************************************************
  saveload.c
 **********************************************************************

  saveload - Test/example program for GAUL.
  Copyright Â©2002, Stewart Adcock <stewart@linux-domain.com>
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

		This program shows how to write/read populations
		to/from disk using GAUL.

		This example is explained in docs/html/tutorial15.html

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
    entity->fitness += (127.0-abs((int)((char *)entity->chromosome[0])[k]-target_text[k]))/50.0;
    }

  return TRUE;
  }


/**********************************************************************
  write_usage()
  synopsis:	Write usage details.
  parameters:
  return:
  updated:	28 May 2002
 **********************************************************************/

void write_usage(void)
  {
  
  printf("\n"
         "saveload ©2002, \"Stewart Adcock\" <stewart@linux-domain.com>\n"
         "\n");

  printf("Usage: saveload [-n INTEGER] [-i FILENAME] -o FILENAME\n"
         "-o FILENAME   Write a population file to FILENAME.\n"
         "-i FILENAME   Read population from FILENAME, otherwise create a new population.\n"
         "-n INTEGER    Number of generations to perform.  [default=10]\n"
         "\n");

  return;
  }


/**********************************************************************
  main()
  synopsis:	Another GAUL example.
  parameters:
  return:
  updated:	19 Aug 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;		/* Population of solutions. */
  char		*filename_in=NULL;	/* Input filename. */
  char		*filename_out=NULL;	/* Output filename. */
  int		i;			/* Loop variable over command-line arguments. */
  int		generations=10;		/* Number of generations to perform. */
  char		*beststring=NULL;	/* Human readable form of best solution. */
  size_t	beststrlen=0;		/* Length of beststring. */

  random_seed(42);

/*
 * Parse command-line.  Expect '-i FILENAME' for a population to read,
 * otherwise a new population will be created.
 * '-o FILENAME' is absolutely required to specify a file to write to.
 * If we don't get these, then we will write the options.
 */
  if (argc<2)
    {
    write_usage();
    exit(0);
    }
  for (i=1; i<argc; i++)
    {
    if (strcmp(argv[i], "-i")==0)
      { /* Read pop. */
      i++;
      if (i==argc) 
        {
        printf("Input filename not specified.\n");
        write_usage();
        exit(0);
        }
      filename_in = argv[i];
      printf("Input filename set to \"%s\"\n", filename_in);
      }
    else if (strcmp(argv[i], "-o")==0)
      {	/* Out pop. */
      i++;
      if (i==argc)
        {
        printf("Output filename not specified.\n");
        write_usage();
        exit(0);
        }
      filename_out = argv[i];
      printf("Output filename set to \"%s\"\n", filename_out);
      }
    else if (strcmp(argv[i], "-n")==0)
      { /* Number of generations requested. */
      i++;
      if (i==argc)
        {
        printf("Number of generations not specified.\n");
        write_usage();
        exit(0);
        }
      generations = atoi(argv[i]);
      printf("Number of generations set to %d.\n", generations);
      }
    else
      {	/* Error parsing args. */
      printf("Unable to parse command-line argument \"%s\"\n", argv[i]);
      write_usage();
      exit(0);
      }
    }

/*
 * Check that we had the required inputs.
 */
  if (filename_out == NULL)
    {
    printf("No output filename was specified.\n");
    write_usage();
    exit(0);
    }

/*
 * Read or create population.
 */
  if (filename_in == NULL)
    {
    pop = ga_genesis_char(
       100,			/* const int              population_size */
       1,			/* const int              num_chromo */
       strlen(target_text),	/* const int              len_chromo */
       NULL,		 	/* GAgeneration_hook      generation_hook */
       NULL,			/* GAiteration_hook       iteration_hook */
       NULL,			/* GAdata_destructor      data_destructor */
       NULL,			/* GAdata_ref_incrementor data_ref_incrementor */
       struggle_score,			/* GAevaluate             evaluate */
       ga_seed_printable_random,	/* GAseed                 seed */
       NULL,				/* GAadapt                adapt */
       ga_select_one_roulette,		/* GAselect_one           select_one */
       ga_select_two_roulette,		/* GAselect_two           select_two */
       ga_mutate_printable_singlepoint_drift,	/* GAmutate               mutate */
       ga_crossover_char_allele_mixing,	/* GAcrossover            crossover */
       NULL,			/* GAreplace replace */
       NULL			/* vpointer	User data */
            );

    ga_population_set_parameters(
       pop,				/* population      *pop */
       GA_SCHEME_DARWIN,		/* const ga_scheme_type    scheme */
       GA_ELITISM_PARENTS_SURVIVE,	/* const ga_elitism_type   elitism */
       1.0,				/* double  crossover */
       0.1,				/* double  mutation */
       0.0				/* double  migration */
                              );
    }
  else
    {
    pop = ga_population_read(filename_in);
    pop->evaluate = struggle_score;	/* Custom functions can't be saved and
                                         * therefore "pop->evaluate" must be
					 * defined manually.  Likewise, if a
					 * custom crossover routine was used, for
					 * example, then that would also need
					 * to be manually defined here.
					 */
    }

  ga_evolution(
       pop,				/* population              *pop */
       generations			/* const int               max_generations */
              );

  printf("The final solution with seed = %d was:\n", i);
  beststring = ga_chromosome_char_to_string(pop, ga_get_entity_from_rank(pop,0), beststring, &beststrlen);
  printf("%s\n", beststring);
  printf("With score = %f\n", ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)) );

  ga_population_write(pop, filename_out);

  printf("Population has been saved as \"%s\"\n", filename_out);

  ga_extinction(pop);

  s_free(beststring);

  exit(EXIT_SUCCESS);
  }


