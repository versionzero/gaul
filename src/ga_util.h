/**********************************************************************
  ga_util.h
 **********************************************************************

  ga_util - Genetic algorithm routines.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:     Routines for handling populations and performing GA
		operations.

  Updated:      20/04/01 SAA	Reorganization of the callback functions.  Much more consistent now.
		17/04/01 SAA	Split the optimisation functions into ga_optim.c.  Started coding flexible selection scheme.
		02/02/01 SAA	Removed helga dependancies.  Now use log_util and mpi_util instead of helga_log and helga_mpi.
		30/01/01 SAA	Tidying.
		22/01/01 SAA	Moved a truckload of stuff over to ga_intrisics.h
		18/12/00 SAA	I haven't been keeping this log upto date.  Hmmm.  Today the genome and entity structures are merged.
		11/04/00 SAA	Based on my dt_ga code - but extensive rewriting was required.

 **********************************************************************/

#ifndef GA_UTIL_H_INCLUDED
#define GA_UTIL_H_INCLUDED

/*
 * Includes
 */
#include "SAA_header.h"

#include <unistd.h>

#include "../util/linkedlist.h"		/* For linked lists. */
#include "../util/log_util.h"		/* For logging facilities. */
#include "../util/memory_util.h"	/* Memory handling. */
#include "../util/mpi_util.h"		/* For multiprocessing facilities. */
#include "../util/random_util.h"	/* For PRNGs. */
#include "../util/table.h"		/* Handling unique integer ids. */

/*
 * Debugging
 */
#ifndef GA_DEBUG
#define GA_DEBUG	DEBUG
#endif

/*
 * Forward declarations.
 */
typedef struct entity_t	entity;
typedef struct population_t population;

/*
 * Enumerated types.
 */
typedef enum ga_genesis_type_t
  {
  GA_GENESIS_UNKNOWN = 0,
  GA_GENESIS_RANDOM, GA_GENESIS_PDB, GA_GENESIS_SOUP, GA_GENESIS_USER
  } ga_genesis_type;

typedef enum ga_class_type_t
  {
  GA_CLASS_UNKNOWN = 0,
  GA_CLASS_DARWIN,
  GA_CLASS_LAMARCK, GA_CLASS_LAMARCK_ALL,
  GA_CLASS_BALDWIN, GA_CLASS_BALDWIN_ALL
  } ga_class_type;

typedef enum ga_elitism_type_t
  {
  GA_ELITISM_UNKNOWN = 0,
  GA_ELITISM_PARENTS_SURVIVE, GA_ELITISM_ROUGH, GA_ELITISM_ROUGH_COMP,
  GA_ELITISM_EXACT, GA_ELITISM_EXACT_COMP,
  GA_ELITISM_PARENTS_DIE
  } ga_elitism_type;

/*
 * Include remainder of this libraries headers.
 */
#include "ga_optim.h"
#include "ga_qsort.h"
#include "ga_similarity.h"

/*
 * Compilation constants.
 */
#define GA_BOLTZMANN_FACTOR	1.38066e-23

/* 
 * OLD
 * Callback function typedefs.
 */
#if 0
typedef boolean	(*GAscore_entity)(population *pop, entity *entity);
typedef void	(*GAseed_random)(int chromosome, int *dataptr);
typedef void	(*GAseed_user)(int chromosome, int *dataptr);
typedef void	(*GAgeneration_hook)(const int generation, population *pop);
typedef void	(*GAiteration_hook)(const int iteration, entity *entity);
typedef void	(*GAdata_destructor)(vpointer data);
typedef void	(*GAdata_ref_incrementor)(vpointer data);
typedef boolean	(*GAdrift_nucleotide)(int chromo, int point, int *dataptr);
typedef boolean	(*GArandomize_nucleotide)(int chromo, int point, int *dataptr);
typedef boolean	(*GAmonte_carlo_move)(population *pop, entity *entity);
typedef entity *(*GAadaptation)(population *pop, entity *entity);
typedef boolean	(*GAselect_one)(population *pop, entity **mother);
typedef boolean	(*GAselect_two)(population *pop, entity **mother, entity **father);

/*
 * Add GAselection, GAmutation, GAcrossover, GAadaptation?
 * (And rename the above GAadaptation to GAoptimisation.)
 */
#endif

/* 
 * Callback function typedefs.
 */
/*
 * Analysis and termination.
 */
typedef boolean	(*GAgeneration_hook)(const int generation, population *pop);
typedef boolean	(*GAiteration_hook)(const int iteration, entity *entity);

/*
 * Data cache handling.
 */
typedef void	(*GAdata_destructor)(vpointer data);
typedef void	(*GAdata_ref_incrementor)(vpointer data);

/*
 * GA operations.
 *
 * FIXME: Adaptation prototype should match the mutation prototype so that
 * the adaptation local optimisation algorithms may be used as mutation
 * operators.
 */
typedef boolean	(*GAevaluate)(population *pop, entity *entity);
/*typedef void	(*GAseed)(int chromosome, int *data);*/
typedef void	(*GAseed)(population *pop, entity *adam);
typedef entity *(*GAadapt)(population *pop, entity *child);
typedef boolean	(*GAselect_one)(population *pop, entity **mother);
typedef boolean	(*GAselect_two)(population *pop, entity **mother, entity **father);
typedef void	(*GAmutate)(population *pop, entity *mother, entity *daughter);
typedef void	(*GAcrossover)(population *pop, entity *mother, entity *father, entity *daughter, entity *son);
typedef void	(*GAreplace)(population *pop, entity *child);

/*
 * Entity Structure.
 *
 * FIXME: Make opaque i.e. move definition into ga_util.c
 * Should encourage the use of accessor functions rather than directly tweaking
 * the values in this structure manually.
 *
 * FIXME: chomosome field should be a moe generic type, and cast when required.
 */
struct entity_t
  {
  double	fitness;	/* Fitness score. */
  int		**chromosome;	/* Array of integer array chromosomes (the genotye). */
  vpointer	data;		/* User data containing physical properties. (the phenotype) */
  boolean	allocated;	/* Whether this structure in the buffer is used. */
  };

/*
 * Population Structure.
 *
 * FIXME: Make opaque. (I have already written the accessor functions.)
 * IMPORTANT NOTE: If you really must iterate over all entities in
 * a population in external code, loop over entity_iarray... NOT entity_array.
 */
struct population_t
  {
  int		max_size;		/* Maximum population size. */
  int		stable_size;		/* Requested population size. */
  int		size;			/* Actual population size. */
  int		orig_size;		/* Number of parents (entities at start of generation). */

/*  int		max_entity;*/		/* Number of allocated entities. */
  entity	*entity_array;		/* The population as an array
                                           (malloc'ed in one go! */
  entity	**entity_iarray;	/* The population as an indexed array. */
  int		num_chromosomes;	/* Number of chromosomes in genotype - should be an array of lengths. */
  int		len_chromosomes;	/* Maximum length of each chromosome. */
  vpointer	data;			/* User data. */

  int		select_state;		/* Available to selection algorithm. */
  double	crossover_ratio;	/* Chance for crossover. */
  double	mutation_ratio;		/* Chance for mutation. */
  double	migration_ratio;	/* Chance for migration. */

/*
 * Scoring function and the other callbacks are defined here.
 * OLD:
  GAscore_entity		score_entity;
  GAseed_random			seed_random;
  GAseed_user			seed_user;
  GAgeneration_hook		generation_hook;
  GAiteration_hook		iteration_hook;
  GAdata_destructor		data_destructor;
  GAdata_ref_incrementor	data_ref_incrementor;
  GAdrift_nucleotide		drift_nucleotide;
  GArandomize_nucleotide	randomize_nucleotide;
  GAmonte_carlo_move		monte_carlo_move;
  GAadaptation			adaptation;
 */
  GAgeneration_hook		generation_hook;
  GAiteration_hook		iteration_hook;

  GAdata_destructor		data_destructor;
  GAdata_ref_incrementor	data_ref_incrementor;

  GAevaluate			evaluate;
  GAseed			seed;
  GAadapt			adapt;
  GAselect_one			select_one;
  GAselect_two			select_two;
  GAmutate			mutate;
  GAcrossover			crossover;
  GAreplace			replace;
  };

/*
 * Constant definitions.
 */
#define GA_MULTI_BIT_CHANCE	0.02
#define GA_ELITISM_MULTIPLIER	0.05
#define GA_ELITISM_CONSTANT	2.0

#define GA_UTIL_MIN_FITNESS	-999999999.0

/*
 * Prototypes
 */

population *ga_population_new(	const int max_size,
				const int stable_size,
				const int num_chromosome,
				const int len_chromosome);
population *ga_get_population_from_id(unsigned int id);
unsigned int ga_get_population_id(population *pop);
boolean	ga_entity_seed(population *pop, entity *e);
boolean ga_population_seed(population *pop);
boolean ga_population_seed_soup(population *pop, const char *fname);
boolean ga_write_soup(population *pop);
boolean ga_population_save(population *pop, char *fname);
population *ga_population_read(char *fname);
boolean	ga_population_score_and_sort(population *pop);
double	ga_population_convergence_genotypes( population *pop );
double	ga_population_convergence_chromsomes( population *pop );
double	ga_population_convergence_alleles( population *pop );
boolean	ga_population_stats( population *pop,
                             double *average, double *stddev );
boolean ga_compare_genome(population *pop, entity *alpha, entity *beta);
int ga_get_entity_rank(population *pop, entity *e);
int ga_get_entity_id(population *pop, entity *e);
entity *ga_get_entity_from_id(population *pop, const unsigned int id);
entity *ga_get_entity_from_rank(population *pop, const unsigned int rank);
int	ga_get_entity_rank_from_id(population *pop, int id);
int	ga_get_entity_id_from_rank(population *pop, int rank);
boolean ga_entity_setup(population *pop, entity *e);
boolean	ga_entity_dereference_by_rank(population *pop, int rank);
boolean ga_entity_dereference(population *p, entity *dying);
void ga_entity_clear_data(population *p, entity *entity, const int chromosome);
void ga_entity_blank(population *p, entity *entity);
entity *ga_get_free_entity(population *pop);
boolean ga_copy_data(population *pop, entity *dest, entity *src, const int chromosome);
boolean ga_copy_entity_all_chromosomes(population *pop, entity *dest, entity *src);
boolean ga_copy_entity_chromosome(population *pop, entity *dest, entity *src, int chromo);
boolean ga_copy_entity(population *pop, entity *dest, entity *src);
entity *ga_multiproc_compare_entities( population *pop, entity *localnew, entity *local );
boolean ga_sendrecv_entities( population *pop, int *send_mask, int send_count );
entity *ga_optimise_entity(population *pop, entity *unopt);
population *ga_genesis( const int               population_size,
                        const int               num_chromo,
                        const int               len_chromo,
                        const char              *fname,
                        GAgeneration_hook       generation_hook,
                        GAiteration_hook        iteration_hook,
                        GAdata_destructor       data_destructor,
                        GAdata_ref_incrementor  data_ref_incrementor,
                        GAevaluate              evaluate,
                        GAseed                  seed,
                        GAadapt                 adapt,
                        GAselect_one            select_one,
                        GAselect_two            select_two,
                        GAmutate                mutate,
                        GAcrossover             crossover,
                        GAreplace               replace );
void	ga_population_set_parameters(      population      *pop,
                                        double  crossover,
                                        double  mutation,
                                        double  migration);
population *ga_transcend(unsigned int id);
unsigned int ga_resurect(population *pop);
boolean ga_extinction(population *extinct);
boolean ga_genocide(population *pop, int target_size);
void	ga_diagnostics( void );
entity	*ga_allele_search(      population      *pop,
                                const int       chromosomeid,
                                const int       point,
                                const int       min_val,
                                const int       max_val,
                                entity          *initial );



boolean ga_select_one_random(population *pop, entity **mother);
boolean ga_select_two_random(population *pop, entity **mother, entity **father);
boolean ga_select_one_every(population *pop, entity **mother);
boolean ga_select_two_every(population *pop, entity **mother, entity **father);
boolean	ga_select_one_randomrank(population *pop, entity **mother);
boolean ga_select_two_randomrank(population *pop, entity **mother, entity **father);
boolean ga_select_one_bestof2(population *pop, entity **mother);
boolean ga_select_two_bestof2(population *pop, entity **mother, entity **father);
boolean	ga_select_one_roulette( population *pop,
                                entity **mother );
boolean	ga_select_two_roulette( population *pop,
                                entity **mother, entity **father );
boolean	ga_select_one_roulette_rebased( population *pop,
                                entity **mother );
boolean	ga_select_two_roulette_rebased( population *pop,
                                entity **mother, entity **father );

void	ga_singlepoint_crossover_chromosome(population *pop, int *father, int *mother, int *son, int *daughter);
void	ga_crossover_chromosome_singlepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_chromosome_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );
void	ga_crossover_chromosome_doublepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);

void	ga_singlepoint_drift_mutation(population *pop, entity *father, entity *son);
void	ga_singlepoint_randomize_mutation(population *pop, entity *father, entity *son);
void	ga_multipoint_mutation(population *pop, entity *father, entity *son);
void	ga_mutate_boolean_singlepoint(population *pop, entity *father, entity *son);
void	ga_mutate_boolean_multipoint(population *pop, entity *father, entity *son);

void	ga_seed_boolean_random(population *pop, entity *adam);
void	ga_seed_integer_random(population *pop, entity *adam);
void	ga_seed_integer_zero(population *pop, entity *adam);

void	ga_replace_by_fitness(population *pop, entity *child);

#endif	/* GA_UTIL_H */
