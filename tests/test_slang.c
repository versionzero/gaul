/**********************************************************************
  test_slang.c
 **********************************************************************

  test_slang - Test S-Lang scripting in GAUL.
  Copyright ©2003, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:	Embed the S-Lang scripting engine and allow scripts
		using the GAUL intrinsic functions to be run.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

#if HAVE_SLANG == 1

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
  test_slang_quit()
  synopsis:	Gracefully die.
  parameters:	none
  return:	none
  last updated:	18 Aug 2003
 **********************************************************************/

void test_slang_quit(void)
  {

  printf("The S-Lang script contained a \"test_slang_quit()\" call.\n");

  exit(EXIT_SUCCESS);
  }


/**********************************************************************
  test_slang_genesis()
  synopsis:	Allocates and initialises a new population structure,
		and assigns a new population id to it.
  parameters:
  return:	unsigned int	population id for this new structure.
  last updated: 18 Aug 2003
 **********************************************************************/

int test_slang_genesis(	int *stable_size )
  {
  population	*pop;

  pop = ga_genesis_char(*stable_size, 1, strlen(target_text),
            NULL, NULL, NULL, NULL, struggle_score,
            ga_seed_printable_random, NULL,
            ga_select_one_sus, ga_select_two_sus,
            ga_mutate_printable_singlepoint_drift,
            ga_crossover_char_allele_mixing, NULL, NULL);

  return ga_get_population_id(pop);
  }


/**********************************************************************
  test_slang_set_evaluation_function()
  synopsis:	Assigns the evaluation callback for a population.
  parameters:	unsigned int	population id
  return:
  last updated: 18 Aug 2003
 **********************************************************************/

int test_slang_set_evaluation_function(	int *pop_id )
  {
  population	*pop;

  pop = ga_get_population_from_id(*pop_id);
  pop->evaluate = struggle_score;

  return TRUE;
  }



/**********************************************************************
  test_slang_call()
  synopsis:	Call a user-defined or intrinisic S-Lang function.
  parameters:	char *func_name	The function's name.
  return:	Success/Failure.
  last updated:	18 Aug 2003
 **********************************************************************/

boolean test_slang_call(char *func_name)
  {

/*
 * int	type=SLang_is_defined(func_name);
 * if (type < 1) return FALSE;
 * SLang_is_defined() returns:
 *   0    no defined object
 *   1    intrinsic function
 *   2    user-defined slang function
 *  -1    intrinsic variable
 *  -2    user-defined global variable
 */

  if (SLang_execute_function(func_name)>0) return TRUE;

  return FALSE;
  }


/**********************************************************************
  test_slang_init()
  synopsis:	Initialise S-Lang scripting engine.
  parameters:	none
  return:	Success/Failure.
  last updated:	18 Aug 2003
 **********************************************************************/

boolean test_slang_init(void)
  {
  static int	true_var=TRUE, false_var=FALSE;	/* Intrinsic variables. */

  printf("Initialising S-Lang.\n");

/*
 * Enable S-Lang interpreter with all intrinsic functions.
 * A lower overhead version would probably need:
 * if ( -1 == SLang_init_slang()  ||
 *      -1 == SLang_init_slmath() ||
 *      -1 == SLang_init_stdio()     ) return FALSE;
 */
  if (-1 == SLang_init_all()) return FALSE;

/*
 * Genetic Algorithm interface stuff: The GAUL intrinsics.
 * These intrinsic functions are defined in ga_intrinsics.c (Although they are
 * mostly just simple wrapper functions.)
 */
  if ( ga_intrinsic_sladd()==FALSE ) return FALSE;

/*
 * Random number stuff.
  if (   SLadd_intrinsic_function("random_init",
             (FVOID_STAR) random_init, SLANG_VOID_TYPE, 0)
      || SLadd_intrinsic_function("random_isinit",
             (FVOID_STAR) random_isinit, SLANG_INT_TYPE, 0)
 */
  if (   SLadd_intrinsic_function("random_get_state",
             (FVOID_STAR) random_get_state_str, SLANG_STRING_TYPE, 0)
      || SLadd_intrinsic_function("random_get_state_len",
             (FVOID_STAR) random_get_state_str_len, SLANG_INT_TYPE, 0)
      || SLadd_intrinsic_function("random_set_state",
             (FVOID_STAR) random_set_state_str, SLANG_VOID_TYPE, 1,
             SLANG_STRING_TYPE)
      || SLadd_intrinsic_function("random_boolean",
             (FVOID_STAR) random_boolean, SLANG_INT_TYPE, 0)
      || SLadd_intrinsic_function("random_unit_uniform",
             (FVOID_STAR) random_unit_uniform, SLANG_DOUBLE_TYPE, 0)
      || SLadd_intrinsic_function("random_unit_gaussian",
             (FVOID_STAR) random_unit_gaussian, SLANG_DOUBLE_TYPE, 0)
      || SLadd_intrinsic_function("random_unit_gaussian",
             (FVOID_STAR) random_unit_gaussian, SLANG_DOUBLE_TYPE, 0)
      || SLadd_intrinsic_function("random_rand",
             (FVOID_STAR) random_rand_wrapper, SLANG_INT_TYPE, 0)
      || SLadd_intrinsic_function("random_tseed",
             (FVOID_STAR) random_tseed, SLANG_VOID_TYPE, 0)
      || SLadd_intrinsic_function("random_seed",
             (FVOID_STAR) random_seed_wrapper, SLANG_VOID_TYPE, 1,
             SLANG_INT_TYPE)
      || SLadd_intrinsic_function("random_boolean_prob",
             (FVOID_STAR) random_boolean_prob_wrapper, SLANG_INT_TYPE, 1,
             SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("random_int",
             (FVOID_STAR) random_int_wrapper, SLANG_INT_TYPE, 1,
             SLANG_INT_TYPE)
      || SLadd_intrinsic_function("random_int_range",
             (FVOID_STAR) random_int_range_wrapper, SLANG_INT_TYPE, 2,
             SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("random_double_range",
             (FVOID_STAR) random_double_range_wrapper, SLANG_DOUBLE_TYPE, 2,
             SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("random_double",
             (FVOID_STAR) random_double_wrapper, SLANG_DOUBLE_TYPE, 1,
             SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("random_double_1",
             (FVOID_STAR) random_double_1, SLANG_DOUBLE_TYPE, 0)
      || SLadd_intrinsic_function("random_gaussian",
             (FVOID_STAR) random_gaussian_wrapper, SLANG_DOUBLE_TYPE, 2,
             SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE)
     ) return FALSE;

/* (Almost) Graceful quit. */
  if (   SLadd_intrinsic_function("test_slang_quit",
            (FVOID_STAR) test_slang_quit, SLANG_VOID_TYPE, 0)
     ) return FALSE;

/* The crucial genesis function and a function to define the scoring function. */
  if (   SLadd_intrinsic_function("test_slang_genesis",
            (FVOID_STAR) test_slang_genesis, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("test_slang_set_evaluation_function",
            (FVOID_STAR) test_slang_set_evaluation_function, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
     ) return FALSE;

/*
 * Define some read-only intrinsic variables.
 */
  if (   SLadd_intrinsic_variable("TRUE", &true_var, SLANG_INT_TYPE, TRUE)
      || SLadd_intrinsic_variable("FALSE", &false_var, SLANG_INT_TYPE, TRUE)
     ) return FALSE;

  return TRUE;
  }


/**********************************************************************
  test_slang_interpret_file()
  synopsis:
  parameters:
  return:
  last updated:	18 Aug 2003
 **********************************************************************/

boolean test_slang_interpret_file(char *script_fname)
  {

  if (!script_fname) die("NULL pointer to filename passed.");

  printf("Interpreting S-Lang script \"%s\".\n", script_fname);

/*
 * Load S-Lang script.
 */
  if (-1 == SLang_load_file(script_fname))
    {
    dief("Error %d interpreting the S-Lang script \"%s\".",
         SLang_Error, script_fname);
    SLang_restart(1);	/* reset interpreter. */
    SLang_Error = 0;
    return FALSE;
    }

  return TRUE;
  }


/**********************************************************************
  test_slang_interpret_stdin()
  synopsis:
  parameters:
  return:
  last updated:	18 Aug 2003
 **********************************************************************/

boolean test_slang_interpret_stdin(void)
  {
  printf("Interpreting stdin as S-Lang.\n");

/*
 * Interprete S-Lang script from stdin.
 */
  if (-1 == SLang_load_file(NULL))
    {
    dief("Error %d interpreting the SLang script from stdin.", SLang_Error);
    SLang_restart(1);
    SLang_Error = 0;
    return FALSE;
    }

#if 0
/*
 * This is how the S-Lang documentation implies the above should be done.
 * -- but it doesn't work...
 */
  char	buffer[256];	/* Stdin buffer. */
  int	linecount=0;	/* Number of lines parsed. */

  while (fgets(buffer, 256, stdin))
    {
    printf("STDIN line %d\n", linecount);
    if (-1 == SLang_load_string(buffer))
      {
      printf("SLang_Error = %d\n", SLang_Error);
      SLang_restart (1);
      }
    SLang_Error = 0;
    linecount++;
    }
#endif

  return TRUE;
  }


/**********************************************************************
  main()
  Synopsis:	Test the S-Lang interface of GAUL.
  Parameters:
  Return:	none
  Last Updated:	18 Aug 2003
 **********************************************************************/

int main( int argc, char **argv )
  {
  int		i;			/* Loop variable over arguments. */
  char		*script_fname=NULL;	/* Name of script file. */

/* Initialise interpreter. */
  if (!test_slang_init())
    die("Unable to initialise S-Lang interpreter.");

/* Initialise PRNG. */
  random_init();

/*
 * Parse command-line arguments.
 */
  for (i=1; i<argc; i++)
    {
    if (strcmp(argv[i], "--script") == 0)
      {
      /* Read S-Lang script from file */
      i++;
      script_fname = argv[i];

      test_slang_interpret_file(script_fname);
      }
    else if (strcmp(argv[i], "--scripts") == 0)
      {
      /* Read multiple S-Lang scripts from files */
      i++;
      while (i<argc)
        {
        script_fname = argv[i];
        printf("Running script \"%s\"\n", script_fname);
        test_slang_interpret_file(script_fname);
        }
      }
    else if (strcmp(argv[i], "--slang") == 0)
      {
      /* Read S-Lang script from stdin */
      i++;
      script_fname = argv[i];

      test_slang_interpret_stdin();
      }
    else
      {
      dief("Unknown option: \"%s\"", argv[i]);
      }
    }

  exit(EXIT_SUCCESS);
  }

#else
int main( int argc, char **argv )
  {

  printf("Support for S-Lang was not compiled.\n");

  exit(EXIT_SUCCESS);
  }
#endif	/* HAVE_SLANG == 1 */
