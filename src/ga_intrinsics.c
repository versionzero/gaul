/**********************************************************************
  ga_intrinsics.c
 **********************************************************************

  ga_intrinics - Genetic algorithm routine wrappers.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

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

  Synopsis:     Wrappers around the routines for handling populations
		and performing GA operations using S-Lang scripting.

		Internally, and in the public C interface, pointers
		are used to identify the population and entity
		structures.  However, in the scripting interface these
		pointers are unusable, so identifing integers are
		used instead.

  To do: 	More error checking.
		Add missing wrappers.
		In particular, need facility for mating/mutating entities.
		Needs some tidying.

 **********************************************************************/

#include "ga_intrinsics.h"

#if HAVE_SLANG==1

/**********************************************************************
  ga_population_new_wrapper()
  synopsis:	Allocates and initialises a new population structure,
		and assigns a new population id to it.
  parameters:
  return:	unsigned int	population id for this new structure.
  last updated: 13 Feb 2002
 **********************************************************************/

int ga_population_new_wrapper(	int *stable_size,
				int *num_chromosome,
				int *len_chromosome)
  {
  return ga_get_population_id(ga_population_new(*stable_size, *num_chromosome, *len_chromosome));
  }


/**********************************************************************
  ga_entity_seed_wrapper()
  synopsis:	Fills a population structure with (hopefully) vaguely
		reasonable, starting genes.  Most 'real' work is done
		in a user-specified function.
  parameters:	none
  return:	boolean success.
  last updated: 22/01/01
 **********************************************************************/

int ga_entity_seed_wrapper(int *pop_id, int *id)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  return ga_entity_seed(pop, ga_get_entity_from_id(pop, *id));
  }


/**********************************************************************
  ga_entity_id_from_rank()
  synopsis:	Finds an entity's id from it's rank.
  parameters:	
  return:	
  last updated: 22/01/01
 **********************************************************************/

int ga_entity_id_from_rank(int *pop_id, int *rank)
  {
  return ga_get_entity_id_from_rank(
               ga_get_population_from_id(*pop_id), *rank );
  }


/**********************************************************************
  ga_entity_rank_from_id()
  synopsis:	Finds an entity's id from it's rank.
  parameters:	
  return:	
  last updated: 22/01/01
 **********************************************************************/

int ga_entity_rank_from_id(int *pop_id, int *id)
  {
  return ga_get_entity_rank_from_id(
               ga_get_population_from_id(*pop_id), *id );
  }


/**********************************************************************
  ga_population_seed_wrapper()
  synopsis:	Fills a population structure with vaguely reasonable,
		random, starting genes.  Most 'real' work is done in
		a user-specified function.
  parameters:	none
  return:	boolean success.
  last updated: 22/01/01
 **********************************************************************/

int ga_population_seed_wrapper(int *pop_id)
  {
  return ga_population_seed(ga_get_population_from_id(*pop_id));
  }


/**********************************************************************
  ga_population_save_wrapper()
  synopsis:	Writes entire population and it's genetic data to disk.
		Currently does not store fitness or userdata -- just
		stores the genes.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

int ga_population_save_wrapper(int *pop_id, char *fname)
  {
  return ga_population_save(ga_get_population_from_id(*pop_id), fname);
  }


/**********************************************************************
  ga_population_read_wrapper()
  synopsis:	Reads entire population and it's genetic data back
		from disk.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

int ga_population_read_wrapper(char *fname)
  {
  return ga_get_population_id(ga_population_read(fname));
  }


/**********************************************************************
  ga_entity_compare_genome_wrapper()
  synopsis:	Compares two genotypes.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_entity_compare_genome_wrapper(int *pop_id, int *jack, int *jill)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

/* Are Jack and Jill genetic twins? */
  return ga_compare_genome(pop,
                           ga_get_entity_from_id(pop, *jack),
                           ga_get_entity_from_id(pop, *jill));
  }


/**********************************************************************
  ga_entity_cross_compare_genome_wrapper()
  synopsis:	Compares two genotypes from potentially differing
		populations.  The populations must still have
		consistent chromosome maps.
  parameters:
  return:
  last updated:	07/02/01
 **********************************************************************/

int ga_entity_cross_compare_genome_wrapper(int *jackpop_id, int *jack,
                                           int *jillpop_id, int *jill)
  {
  population	*jackpop, *jillpop;	/* Population structures. */

  jackpop = ga_get_population_from_id(*jackpop_id);
  jillpop = ga_get_population_from_id(*jillpop_id);

/* Are Jack and Jill genetic twins? */
  return ga_compare_genome(jackpop,
                           ga_get_entity_from_id(jackpop, *jack),
                           ga_get_entity_from_id(jillpop, *jill));
  }


/**********************************************************************
  ga_entity_kill()
  synopsis:	Marks an entity structure as unused (dereferences it).
		Any contents of entities data field are freed.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

int ga_entity_kill(int *pop_id, int *id)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  return ga_entity_dereference_by_rank(
                pop, ga_get_entity_rank_from_id(pop, *id) );
  }


/**********************************************************************
  ga_entity_erase()
  synopsis:	Clears the entity's data.
		Equivalent to an optimised pair of ga_entity_kill()
		and ga_entity_new() calls.
		Chromosomes are guarenteed to be intact, but may be
		overwritten by user.
		Also decrements population size.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_entity_erase(int *pop_id, int *id)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  ga_entity_blank(pop, ga_get_entity_from_id(pop, *id));

  return TRUE;
  }


/**********************************************************************
  ga_entity_new()
  synopsis:	Returns handle of a new, unused, entity structure.
		Also increments population size.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_entity_new(int *pop_id)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  return ga_get_entity_id(pop, ga_get_free_entity(pop));
  }


/**********************************************************************
  ga_entity_clone_wrapper()
  synopsis:	Returns to a new entity structure with the genes and
		structural data copied from the parent.
		Increments population size also.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_entity_clone_wrapper(int *pop_id, int *parent)
  {
  entity	*child;						/* Destination entity. */
  population	*pop = ga_get_population_from_id(*pop_id);	/* Population. */

  child = ga_get_free_entity(pop);
  ga_entity_copy(pop, child, ga_get_entity_from_id(pop, *parent));

  return ga_get_entity_id(pop, child);
  }


/**********************************************************************
  ga_entity_copy_chromosome_wrapper()
  synopsis:	Copy genetic data between entity structures.
  parameters:
  return:
  last updated: 29 Nov 2001
 **********************************************************************/

int ga_entity_copy_chromosome_wrapper(int *pop_id, int *parent, int *child, int *chromosome)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  return ga_entity_copy_chromosome(pop,
                                   ga_get_entity_from_id(pop, *child),
                                   ga_get_entity_from_id(pop, *parent),
                                   *chromosome);
  }


/**********************************************************************
  ga_entity_copy_all_chromosomes_wrapper()
  synopsis:	Copy genetic data between entity structures.
  parameters:
  return:
  last updated: 29 Nov 2001
 **********************************************************************/

int ga_entity_copy_all_chromosomes_wrapper(int *pop_id, int *parent, int *child)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  return ga_entity_copy_all_chromosomes(pop,
                                        ga_get_entity_from_id(pop, *child),
                                        ga_get_entity_from_id(pop, *parent));
  }


/**********************************************************************
  ga_entity_migrate()
  synopsis:	Copy entity from one population into another - does not
		delete the original.  The caller should do that, if
		required.
		FIXME: Need check to confirm that populations are
		compatible.
  parameters:
  return:
  last updated:	14/02/01
 **********************************************************************/

int ga_entity_migrate(int *srcpop_id, int *destpop_id, int *jacques)
  {
  population	*srcpop;	/* Original population. */
  population	*destpop;	/* Destination population. */
  entity	*jack;		/* Migrated entity. */

  srcpop = ga_get_population_from_id(*srcpop_id);
  destpop = ga_get_population_from_id(*destpop_id);

  jack = ga_get_free_entity(destpop);
  ga_entity_copy(srcpop, jack, ga_get_entity_from_id(srcpop, *jacques));

  return ga_get_entity_id(destpop, jack);
  }


#if 0
/**********************************************************************
  ga_singlepoint_crossover_chromosome()
  synopsis:	`Mates' two chromosomes by single-point crossover.
  parameters:
  return:
  last updated: 18/10/00
 **********************************************************************/

boolean ga_singlepoint_crossover_chromosome(int *father, int *mother, int *son, int *daughter)
  {
  }


/**********************************************************************
  ga_crossover_chromosome_singlepoints_wrapper()
  synopsis:	`Mates' two genotypes by single-point crossover of
		each chromosome.
  parameters:
  return:
  last updated: 14/02/01
 **********************************************************************/

int ga_crossover_chromosome_singlepoints_wrapper(int *pop_id,
                     int *father, int *mother, int *son, int *daughter)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  ga_crossover_chromosome_singlepoints(pop,
                      ga_get_entity_from_id(pop, *father),
                      ga_get_entity_from_id(pop, *mother),
                      ga_get_entity_from_id(pop, *son),
                      ga_get_entity_from_id(pop, *daughter) );

  return TRUE;
  }


/**********************************************************************
  ga_crossover_chromosome_mixing_wrapper()
  synopsis:	`Mates' two genotypes by crossover (chromosome mixing).
		Keeps all chromosomes intact, and therefore will not
		need to recreate any structural data.
  parameters:
  return:
  last updated:	14/02/01
 **********************************************************************/

int ga_crossover_chromosome_mixing_wrapper(int *pop_id,
                     int *father, int *mother, int *son, int *daughter)
  {
  population    *pop;                   /* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  ga_crossover_chromosome_mixing(pop,
                      ga_get_entity_from_id(pop, *father),
                      ga_get_entity_from_id(pop, *mother),
                      ga_get_entity_from_id(pop, *son),
                      ga_get_entity_from_id(pop, *daughter) );

  return TRUE;
  }
#endif


/**********************************************************************
  ga_entity_score()
  synopsis:	Score entity.
  parameters:
  return:
  last updated: 07/02/01
 **********************************************************************/

int ga_entity_score(int *pop_id, int *joe)
  {
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  if (!pop->evaluate) die("Scoring function not defined.");

/*  return pop->evaluate(pop, ga_get_entity_from_id(pop, *joe));*/
  pop->evaluate(pop, ga_get_entity_from_id(pop, *joe));

/*
  plog(LOG_DEBUG, "Return from pop->evaluate().\n");
*/

  return TRUE;
  }


/**********************************************************************
  ga_evolution_wrapper()
  synopsis:	Wrapper around the main genetic algorithm routine.
		It performs a GA-based optimisation on the specified
		population.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_evolution_wrapper(	int	*pop,
				int	*class,
				int	*elitism,
				int	*max_generations )
  {
  ga_evolution( ga_get_population_from_id(*pop),
                *class, *elitism, *max_generations );

  return TRUE;
  }


/**********************************************************************
  ga_population_get_size()
  synopsis:	Access population's size field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_population_get_size(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->size;
  }


/**********************************************************************
  ga_population_get_maxsize()
  synopsis:	Access population's max_size field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_population_get_maxsize(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->max_size;
  }


/**********************************************************************
  ga_population_get_stablesize()
  synopsis:	Access population's stable_size field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_population_get_stablesize(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->stable_size;
  }


/**********************************************************************
  ga_population_set_stablesize()
  synopsis:	Alter population's stable_size field.
		This should be used with care.
  parameters:
  return:
  last updated:	27/03/01
 **********************************************************************/

int ga_population_set_stablesize(int *pop, int *size)
  {
  ((population*) ga_get_population_from_id(*pop))->stable_size = *size;
  return TRUE;
  }


/**********************************************************************
  ga_population_get_crossoverratio()
  synopsis:	Access population's crossover_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

double ga_population_get_crossoverratio(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->crossover_ratio;
  }


/**********************************************************************
  ga_population_set_crossoverratio()
  synopsis:	Alter population's crossover_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

int ga_population_set_crossoverratio(int *pop, double *value)
  {
  ((population*) ga_get_population_from_id(*pop))->crossover_ratio = *value;
  return TRUE;
  }


/**********************************************************************
  ga_population_get_mutationratio()
  synopsis:	Access population's mutation_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

double ga_population_get_mutationratio(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->mutation_ratio;
  }


/**********************************************************************
  ga_population_set_mutationratio()
  synopsis:	Alter population's mutation_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

int ga_population_set_mutationratio(int *pop, double *value)
  {
  ((population*) ga_get_population_from_id(*pop))->mutation_ratio = *value;
  return TRUE;
  }


/**********************************************************************
  ga_population_get_migrationratio()
  synopsis:	Access population's migration_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

int ga_population_get_migrationratio(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->migration_ratio;
  }


/**********************************************************************
  ga_population_set_migrationratio()
  synopsis:	Alter population's migration_ratio field.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

int ga_population_set_migrationratio(int *pop, int *value)
  {
  ((population*) ga_get_population_from_id(*pop))->migration_ratio = *value;
  return TRUE;
  }


/**********************************************************************
  ga_population_get_chromosomenum()
  synopsis:	Access population's num_chromosomes field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_population_get_chromosomenum(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->num_chromosomes;
  }


/**********************************************************************
  ga_population_get_chromosomelen()
  synopsis:	Access population's len_chromosomes field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_population_get_chromosomelen(int *pop)
  {
  return ((population*) ga_get_population_from_id(*pop))->len_chromosomes;
  }


/**********************************************************************
  ga_entity_get_fitness()
  synopsis:	Access entity's fitness field.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

double ga_entity_get_fitness(int *pop, int *id)
  {
  return ga_get_entity_from_id(ga_get_population_from_id(*pop), *id)->fitness;
  }


/**********************************************************************
  ga_entity_isallocated()
  synopsis:	Determine whether the given entity id is used.
  parameters:
  return:
  last updated:	18 Mar 2002
 **********************************************************************/

int ga_entity_isallocated(int *pop, int *id)
  {
  return ga_get_entity_from_id(ga_get_population_from_id(*pop), *id) != NULL;
  }


/**********************************************************************
  ga_extinction_wrapper()
  synopsis:	Purge all memory used by a population.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

int ga_extinction_wrapper(int *pop)
  {
  return ga_extinction( ga_get_population_from_id(*pop) );
  }


/**********************************************************************
  ga_genocide_wrapper()
  synopsis:	Kill population members.
  parameters:
  return:
  last updated:	11/01/01
 **********************************************************************/

int ga_genocide_wrapper(int *pop, int *target_size)
  {
  return ga_genocide( ga_get_population_from_id(*pop), *target_size );
  }


/**********************************************************************
  ga_allele_search_wrapper()
  synopsis:	Wrapper around ga_allele_search() for the scripted API.
  parameters:
  return:	Index of best solution found (A new entity).
  last updated:	18 Mar 2002
 **********************************************************************/

int ga_allele_search_wrapper(	int	*pop_id,
				int	*chromosomeid,
				int 	*point,
				int	*min_val,
				int	*max_val,
				int	*entity_id )
  {
  entity	*initial, *final;	/* Initial and final solutions. */
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  initial = ga_get_entity_from_id(pop, *entity_id);

  final = ga_allele_search( pop,
                            *chromosomeid, *point, *min_val, *max_val,
                            initial );

  return ga_get_entity_id(pop, final);
  }


/**********************************************************************
  ga_metropolis_wrapper()
  synopsis:	Wrapper around ga_metropolis_mutation() for the
		scripted API.
  parameters:
  return:	Index of best solution found (A new entity).
  last updated:	18 Mar 2002
 **********************************************************************/

int ga_metropolis_wrapper(	int	*pop_id,
				int	*entity_id,
				int	*num_iterations,
				int 	*temperature)
  {
  entity	*initial, *final;	/* Initial and final solutions. */
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  initial = ga_get_entity_from_id(pop, *entity_id);

  final = ga_metropolis_mutation( pop, initial,
                         *num_iterations, *temperature );

  return ga_get_entity_id(pop, final);
  }


/**********************************************************************
  ga_sa_wrapper()
  synopsis:	Wrapper around ga_simulated_annealling_mutation()
		for the scripted API.
  parameters:
  return:	Index of best solution found (A new entity).
  last updated:	18 Mar 2002
 **********************************************************************/

int ga_sa_wrapper(	int	*pop_id,
			int	*entity_id,
			int	*num_iterations,
			int 	*initial_temperature,
			int 	*final_temperature )
  {
  entity	*initial, *final;	/* Initial and final solutions. */
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  initial = ga_get_entity_from_id(pop, *entity_id);

  final = ga_simulated_annealling_mutation( pop, initial,
                         *num_iterations, *initial_temperature, *final_temperature );

  return ga_get_entity_id(pop, final);
  }


/**********************************************************************
  ga_nahc_wrapper()
  synopsis:	Wrapper around ga_next_ascent_hill_climbing() for
		scriptted API.
  parameters:
  return:	Index of best solution found (A new entity).
  last updated:	22/01/01
 **********************************************************************/

/*
 * FIXME: This is a temporary function.  The hard-coded values are probably
 * not suitable for your particular task.
 */
static void _temporary_nahc_mutation(int chromosome, int point, int *data)
  {
  int           orig=data[point];       /* The original data */

  do
    {
    data[point] = random_int(RAND_MAX)-(RAND_MAX/2);
    } while (data[point]==orig);

  return;
  }


int ga_nahc_wrapper(	int	*pop_id,
			int	*entity_id,
			int	*num_iterations )
  {
  entity	*initial, *final;	/* Initial and final solutions. */
  population	*pop;			/* Active population structure. */

  plog( LOG_FIXME, "Selection of optimisation function is currently unavailable from the script interface.");

  pop = ga_get_population_from_id(*pop_id);

  initial = ga_get_entity_from_id(pop, *entity_id);

  final = ga_next_ascent_hill_climbing( pop, initial,
                                        *num_iterations,
					_temporary_nahc_mutation );

  return ga_get_entity_id(pop, final);
  }


/**********************************************************************
  ga_rmhc_wrapper()
  synopsis:	Wrapper around ga_next_ascent_hill_climbing() for
		scriptted API.
  parameters:
  return:	Index of best solution found (A new entity).
  last updated:	22/01/01
 **********************************************************************/

int ga_rmhc_wrapper(	int	*pop_id,
			int	*entity_id,
			int	*num_iterations )
  {
  entity	*initial, *final;	/* Initial and final solutions. */
  population	*pop;			/* Active population structure. */

  pop = ga_get_population_from_id(*pop_id);

  initial = ga_get_entity_from_id(pop, *entity_id);

  final = ga_random_mutation_hill_climbing( pop, initial,
                                            *num_iterations );

  return ga_get_entity_id(pop, final);
  }


/**********************************************************************
  ga_population_score_and_sort_wrapper()
  synopsis:	Wrapper around ga_population_score_and_sort() for
		scriptted API.  Recommended for use after reading
		the population from disk.
  parameters:	Population handle.
  return:	Success.
  last updated:	28/02/01
 **********************************************************************/

int ga_population_score_and_sort_wrapper(int *pop_id)
  {
  return ga_population_score_and_sort(ga_get_population_from_id(*pop_id));
  }


/**********************************************************************
  ga_slang_seed()
  synopsis:	Handle SLang based seeding operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

void ga_slang_seed(population *pop, entity *adam)
  {

  plog(LOG_FIXME, "ga_slang_seed() is not implemented.");

  return;
  }


/**********************************************************************
  ga_slang_select_one()
  synopsis:	Handle SLang based single selection operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

boolean ga_slang_select_one(population *pop, entity **mother)
  {

  plog(LOG_FIXME, "ga_slang_select_one() is not implemented.");

  return TRUE;
  }


/**********************************************************************
  ga_slang_select_two()
  synopsis:	Handle SLang based double selection operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

boolean ga_slang_select_two(population *pop, entity **mother, entity **father)
  {

  plog(LOG_FIXME, "ga_slang_select_two() is not implemented.");

  return TRUE;
  }


/**********************************************************************
  ga_slang_adapt()
  synopsis:	Handle SLang based adaption operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

entity *ga_slang_adapt(population *pop, entity *child)
  {
  entity	*adult;		/* Optimised entity. */

/* Checks */
  if (!pop) die("Null pointer to population structure passed.");
  if (!child) die("Null pointer to entity structure passed.");

  adult = ga_get_free_entity(pop);

  if ( SLang_run_hooks( "adapt_hook", 3,
                        ga_get_population_id(pop), 
                        ga_get_entity_id(pop, child), 
                        ga_get_entity_id(pop, adult)) == -1 )
    die("Error calling SLang function \"adapt_hook\".");

  return adult;
  }


/**********************************************************************
  ga_slang_crossover()
  synopsis:	Handle SLang based crossover operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

void ga_slang_crossover(population *pop, entity *father, entity *mother, entity *daughter, entity *son)
  {

/* Checks */
  if (!pop) die("Null pointer to population structure passed.");
  if (!father || !mother || !son || !daughter)
    die("Null pointer to entity structure passed.");

  if ( SLang_run_hooks( "crossover_hook", 5,
                        ga_get_population_id(pop), 
                        ga_get_entity_id(pop, father), 
                        ga_get_entity_id(pop, mother), 
                        ga_get_entity_id(pop, son), 
                        ga_get_entity_id(pop, daughter)) == -1 )
    die("Error calling SLang function \"crossover_hook\".");

  return;
  }


/**********************************************************************
  ga_slang_mutate()
  synopsis:	Handle SLang based mutation operator.
  parameters:
  return:
  last updated:	24/04/01
 **********************************************************************/

void ga_slang_mutate(population *pop, entity *father, entity *son)
  {

/* Checks */
  if (!pop) die("Null pointer to population structure passed.");
  if (!father || !son) die("Null pointer to entity structure passed.");

#if 0
/*
 * Duplicate the initial data.
 */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(int));
    ga_copy_data(pop, son, father, i);
    }
#endif

  if ( SLang_run_hooks( "mutate_hook", 3,
                        ga_get_population_id(pop), 
                        ga_get_entity_id(pop, father), 
                        ga_get_entity_id(pop, son)) == -1 )
    die("Error calling SLang function \"mutate_hook\".");

  return;
  }


/**********************************************************************
  ga_slang_replace()
  synopsis:	Handle SLang based replacement operator.
  parameters:
  return:
  last updated:	03/06/01
 **********************************************************************/

void ga_slang_replace(population *pop, entity *child)
  {

/* Checks */
  if (!pop) die("Null pointer to population structure passed.");
  if (!child) die("Null pointer to entity structure passed.");

  if ( SLang_run_hooks( "replace_hook", 3,
                        ga_get_population_id(pop), 
                        ga_get_entity_id(pop, child)) == -1 )
    die("Error calling SLang function \"replace_hook\".");

  return;
  }
#endif


/**********************************************************************
  ga_intrinsic_sladd()
  synopsis:	Register the SLang intrinsics.
  parameters:	none
  return:	success/failure.
  last updated:	17/04/01
 **********************************************************************/

#if HAVE_SLANG==0
boolean ga_intrinsic_sladd(void)
  {
  plog(LOG_WARNING, "No SLang support compiled into GAUL.");
  return TRUE;
  }
#else

boolean ga_intrinsic_sladd(void)
  {
  return (
         SLadd_intrinsic_function("ga_population_new",
            (FVOID_STAR) ga_population_new_wrapper, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_seed",
            (FVOID_STAR) ga_entity_seed_wrapper, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_id_from_rank",
            (FVOID_STAR) ga_entity_id_from_rank, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_rank_from_id",
            (FVOID_STAR) ga_entity_rank_from_id, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_seed",
            (FVOID_STAR) ga_population_seed_wrapper, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_save",
            (FVOID_STAR) ga_population_save_wrapper, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_STRING_TYPE)
      || SLadd_intrinsic_function("ga_population_read",
            (FVOID_STAR) ga_population_read_wrapper, SLANG_INT_TYPE, 1,
            SLANG_STRING_TYPE)
      || SLadd_intrinsic_function("ga_entity_compare_genome",
            (FVOID_STAR) ga_entity_compare_genome_wrapper, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_kill",
            (FVOID_STAR) ga_entity_kill, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_erase",
            (FVOID_STAR) ga_entity_erase, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_new",
            (FVOID_STAR) ga_entity_new, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_clone",
            (FVOID_STAR) ga_entity_clone_wrapper, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_copy_chromosome",
            (FVOID_STAR) ga_entity_copy_chromosome_wrapper, SLANG_INT_TYPE, 4,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_copy_all_chromosomes",
            (FVOID_STAR) ga_entity_copy_all_chromosomes_wrapper, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_evolution",
            (FVOID_STAR) ga_evolution_wrapper, SLANG_INT_TYPE, 4,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_get_size",
            (FVOID_STAR) ga_population_get_size, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_get_maxsize",
            (FVOID_STAR) ga_population_get_maxsize, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_get_stablesize",
            (FVOID_STAR) ga_population_get_stablesize, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_set_stablesize",
            (FVOID_STAR) ga_population_set_stablesize, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_get_crossoverratio",
            (FVOID_STAR) ga_population_get_crossoverratio, SLANG_DOUBLE_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_set_crossoverratio",
            (FVOID_STAR) ga_population_set_crossoverratio, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("ga_population_get_mutationratio",
            (FVOID_STAR) ga_population_get_mutationratio, SLANG_DOUBLE_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_set_mutationratio",
            (FVOID_STAR) ga_population_set_mutationratio, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("ga_population_get_migrationratio",
            (FVOID_STAR) ga_population_get_migrationratio, SLANG_DOUBLE_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_set_migrationratio",
            (FVOID_STAR) ga_population_set_migrationratio, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_DOUBLE_TYPE)
      || SLadd_intrinsic_function("ga_population_get_chromosomenum",
            (FVOID_STAR) ga_population_get_chromosomenum, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_get_chromosomelen",
            (FVOID_STAR) ga_population_get_chromosomelen, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_get_fitness",
            (FVOID_STAR) ga_entity_get_fitness, SLANG_DOUBLE_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_isallocated",
            (FVOID_STAR) ga_entity_isallocated, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_extinction",
            (FVOID_STAR) ga_extinction_wrapper, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_genocide",
            (FVOID_STAR) ga_genocide_wrapper, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_allele_search",
            (FVOID_STAR) ga_allele_search_wrapper, SLANG_INT_TYPE, 6,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_metropolis",
            (FVOID_STAR) ga_metropolis_wrapper, SLANG_INT_TYPE, 4,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_sa",
            (FVOID_STAR) ga_sa_wrapper, SLANG_INT_TYPE, 5,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_nahc",
            (FVOID_STAR) ga_nahc_wrapper, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_rmhc",
            (FVOID_STAR) ga_rmhc_wrapper, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_sort",
            (FVOID_STAR) ga_population_sort, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_population_score_and_sort",
            (FVOID_STAR) ga_population_score_and_sort_wrapper, SLANG_INT_TYPE, 1,
            SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_score",
            (FVOID_STAR) ga_entity_score, SLANG_INT_TYPE, 2,
            SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_entity_migrate",
            (FVOID_STAR) ga_entity_migrate, SLANG_INT_TYPE, 3,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
/*
      || SLadd_intrinsic_function("ga_crossover_chromosome_singlepoints",
            (FVOID_STAR) ga_crossover_chromosome_singlepoints_wrapper, SLANG_INT_TYPE, 5,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
      || SLadd_intrinsic_function("ga_crossover_chromosome_mixing",
            (FVOID_STAR) ga_crossover_chromosome_mixing_wrapper, SLANG_INT_TYPE, 5,
            SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE)
*/
     );
  }

#endif


