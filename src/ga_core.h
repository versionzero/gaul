/**********************************************************************
  ga_core.h
 **********************************************************************

  ga_core - Genetic algorithm routines.
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

  Synopsis:     Routines for handling populations and performing GA
		operations.

 **********************************************************************/

#ifndef GA_CORE_H_INCLUDED
#define GA_CORE_H_INCLUDED

/*
 * Includes
 */
#include "SAA_header.h"

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include <unistd.h>

/*
 * Debugging
 */
#ifndef GA_DEBUG
# ifdef DEBUG
#  define GA_DEBUG	DEBUG
# else
#  define GA_DEBUG	0
# endif
#endif

/*
 * Specification of number of processes used in
 * multiprocess functions.
 */
#ifndef GA_NUM_PROCESSES_ENVVAR_STRING
#define GA_NUM_PROCESSES_ENVVAR_STRING  "GAUL_NUM_PROCESSES"
#endif
  
#ifndef GA_DEFAULT_NUM_PROCESSES
#define GA_DEFAULT_NUM_PROCESSES        8
#endif

/*
 * Specification of number of threads used in
 * multithreaded functions.
 */
#ifndef GA_NUM_THREADS_ENVVAR_STRING
#define GA_NUM_THREADS_ENVVAR_STRING	"GAUL_NUM_THREADS"
#endif
  
#ifndef GA_DEFAULT_NUM_THREADS
#define GA_DEFAULT_NUM_THREADS		4
#endif

/*
 * Whether simple statistics should be dumped to disk.
 */
#ifndef GA_WRITE_STATS
# if DEBUG > 1
#  define GA_WRITE_STATS	TRUE
# else
#  define GA_WRITE_STATS	FALSE
# endif
#endif

/*
 * Include remainder of this library's headers.
 */
#include "gaul.h"
#include "ga_bitstring.h"
#include "ga_chromo.h"
#include "ga_climbing.h"
#include "ga_optim.h"
#include "ga_qsort.h"
#include "ga_randomsearch.h"
#include "ga_sa.h"
#include "ga_similarity.h"
#include "ga_tabu.h"
#include "ga_simplex.h"

/*
 * Compilation constants.
 */
#define GA_BOLTZMANN_FACTOR	1.38066e-23
#define GA_TINY_DOUBLE		1.0e-9

/*
 * MPI message tags.
 */
#define GA_TAG_NULL		0

#define GA_TAG_NUMENTITIES	101
#define GA_TAG_ENTITYLEN	102
#define GA_TAG_ENTITYFITNESS	103
#define GA_TAG_ENTITYCHROMOSOME	104

#define GA_TAG_POPSTABLESIZE	201
#define GA_TAG_POPCROSSOVER	202
#define GA_TAG_POPMUTATION	203
#define GA_TAG_POPMIGRATION	204

/*
 * Entity Structure.
 *
 * FIXME: Make opaque i.e. move definition into ga_core.c
 * Should encourage the use of accessor functions rather than directly tweaking
 * the values in this structure manually.
 */
struct entity_t
  {
  double	fitness;	/* Fitness score. */
  vpointer	*chromosome;	/* The chromosomes (the genotype). */
  vpointer	data;		/* User data containing physical properties. (the phenotype) */
  };

/*
 * Tabu-search parameter structure.
 */
typedef struct
  {
  int		list_length;	/* Length of the tabu-list. */
  int		search_count;	/* Number of local searches initiated at each iteration. */
  GAtabu_accept	tabu_accept;	/* Acceptance function. */
  } ga_tabu_t;

/*
 * Simulated Annealling search parameter structure.
 */
typedef struct
  {
  double	initial_temp;	/* Initial temperature. */
  double	final_temp;	/* Final temperature. */
  double	temp_step;	/* Increment of temperature updates. */
  int		temp_freq;	/* Frequency for temperature updates.
				 * (Or, -1 for smooth transition between Ti and Tf) */
  double	temperature;	/* Current temperature. */
  GAsa_accept	sa_accept;	/* Acceptance criterion function. */
  } ga_sa_t;

/*
 * Hill climbing parameter structure.
 */
typedef struct
  {
  GAmutate_allele	mutate_allele;	/* Allele mutation function. */
  } ga_climbing_t;

/*
 * Simplex parameter structure.
 */
typedef struct
  {
  int		dimensions;	/* Size of double array. */
  GAto_double	to_double;	/* Convert chromosome to double array. */
  GAfrom_double	from_double;	/* Convert chromosome from double array. */
  } ga_simplex_t;

/*
 * Population Structure.
 *
 * FIXME: Make opaque. (I have already written the accessor functions.)
 * IMPORTANT NOTE: If you really must iterate over all entities in
 * a population in external code, loop over entity_iarray... NOT entity_array.
 */
struct population_t
  {
  int		max_size;		/* Current maximum population size. */
  int		stable_size;		/* Requested population size. */
  int		size;			/* Actual population size. */
  int		orig_size;		/* Number of parents (entities at start of generation). */
  int		island;			/* Population's island. */
  int		free_index;		/* Next potentially free entity index. */

  MemChunk	*entity_chunk;		/* Buffer for entity structures. */
  entity	**entity_array;		/* The population in id order. */
  entity	**entity_iarray;	/* The population sorted by fitness. */

  int		num_chromosomes;	/* Number of chromosomes in genotype.  FIXME: should be an array of lengths. */
  int		len_chromosomes;	/* Maximum length of each chromosome. */
  vpointer	data;			/* User data. */

  int		select_state;		/* Available to selection algorithm. */

/*
 * Evolutionary parameters.
 */
  double		crossover_ratio;	/* Chance for crossover. */
  double		mutation_ratio;		/* Chance for mutation. */
  double		migration_ratio;	/* Chance for migration. */
  ga_scheme_type	scheme;			/* Evolutionary scheme. */
  ga_elitism_type	elitism;		/* Elitism mode. */

/*
 * Non-evolutionary parameters.
 */
  ga_tabu_t	*tabu_params;			/* Parameters for tabu-search. */
  ga_sa_t	*sa_params;			/* Parameters for simulated annealling. */
  ga_climbing_t	*climbing_params;		/* Parameters for hill climbing. */
  ga_simplex_t	*simplex_params;		/* Parameters for simplex search. */

/*
 * Scoring function and the other callbacks are defined here.
 */
  GAgeneration_hook		generation_hook;
  GAiteration_hook		iteration_hook;

  GAdata_destructor		data_destructor;
  GAdata_ref_incrementor	data_ref_incrementor;

  GAchromosome_constructor	chromosome_constructor;
  GAchromosome_destructor	chromosome_destructor;
  GAchromosome_replicate	chromosome_replicate;
  GAchromosome_to_bytes		chromosome_to_bytes;
  GAchromosome_from_bytes	chromosome_from_bytes;
  GAchromosome_to_string	chromosome_to_string;

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

/* Define lower bound on fitness. */
#define GA_MIN_FITNESS		DBL_MIN

/*
 * Prototypes
 */

population *ga_population_new(	const int stable_size,
				const int num_chromosome,
				const int len_chromosome);
population *ga_population_clone_empty( population *pop );
population *ga_population_clone( population *pop );
int	ga_get_num_populations(void);
population *ga_get_population_from_id(unsigned int id);
unsigned int ga_get_population_id(population *pop);
unsigned int *ga_get_all_population_ids(void);
population **ga_get_all_populations(void);
boolean	ga_entity_seed(population *pop, entity *e);
boolean ga_population_seed(population *pop);
boolean ga_population_seed_soup(population *pop, const char *fname);
boolean ga_write_soup(population *pop);
boolean ga_population_write(population *pop, char *fname);
population *ga_population_read(char *fname);
boolean ga_entity_write(population *pop, entity *entity, char *fname);
entity *ga_entity_read(population *pop, char *fname);
double	ga_entity_evaluate(population *pop, entity *entity);
boolean	ga_population_score_and_sort(population *pop);
boolean	ga_population_sort(population *pop);
double	ga_population_convergence_genotypes( population *pop );
double	ga_population_convergence_chromsomes( population *pop );
double	ga_population_convergence_alleles( population *pop );
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
/*
vpointer	ga_entity_get_data(population *pop, entity *e, const int chromosome);
void	ga_entity_set_data(population *pop, entity *e, vpointer data, const int chromosome);
*/
boolean ga_entity_copy_all_chromosomes(population *pop, entity *dest, entity *src);
boolean ga_entity_copy_chromosome(population *pop, entity *dest, entity *src, int chromo);
boolean ga_entity_copy(population *pop, entity *dest, entity *src);
entity	*ga_entity_clone(population *pop, entity *parent);

void ga_population_send_by_mask( population *pop, int dest_node, int num_to_send, boolean *send_mask );
void ga_population_send_every( population *pop, int dest_node );
void ga_population_append_receive( population *pop, int src_node );
population *ga_population_new_receive( int src_node );
population *ga_population_receive( int src_node );
void ga_population_send( population *pop, int dest_node );
void ga_population_send_all( population *pop, int dest_node );

entity	*ga_optimise_entity(population *pop, entity *unopt);
void	ga_population_set_parameters(  population            *pop,
                                       const ga_scheme_type  scheme,
                                       const ga_elitism_type elitism,
                                       const double          crossover,
                                       const double          mutation,
                                       const double          migration);
void	ga_population_set_scheme(      population            *pop,
                                       const ga_scheme_type  scheme);
void	ga_population_set_elitism(     population            *pop,
                                       const ga_elitism_type elitism);
void	ga_population_set_crossover(   population            *pop,
                                       const double          crossover);
void	ga_population_set_mutation(    population            *pop,
                                       const double          mutation);
void	ga_population_set_migration(   population            *pop,
                                       const double          migration);
population *ga_transcend(unsigned int id);
unsigned int ga_resurect(population *pop);
boolean ga_extinction(population *extinct);
boolean ga_genocide(population *pop, int target_size);


boolean ga_select_one_random(population *pop, entity **mother);
boolean ga_select_two_random(population *pop, entity **mother, entity **father);
boolean ga_select_one_every(population *pop, entity **mother);
boolean ga_select_two_every(population *pop, entity **mother, entity **father);
boolean	ga_select_one_randomrank(population *pop, entity **mother);
boolean ga_select_two_randomrank(population *pop, entity **mother, entity **father);
boolean ga_select_one_bestof2(population *pop, entity **mother);
boolean ga_select_two_bestof2(population *pop, entity **mother, entity **father);
boolean	ga_select_one_roulette( population *pop, entity **mother );
boolean	ga_select_two_roulette( population *pop, entity **mother, entity **father );
boolean	ga_select_one_roulette_rebased( population *pop, entity **mother );
boolean	ga_select_two_roulette_rebased( population *pop, entity **mother, entity **father );
boolean	ga_select_one_sus( population *pop, entity **mother );
boolean	ga_select_two_sus( population *pop, entity **mother, entity **father );


void	ga_crossover_integer_singlepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_integer_doublepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_integer_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_integer_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );
void	ga_crossover_boolean_singlepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_boolean_doublepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_boolean_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_boolean_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );
void	ga_crossover_char_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_char_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );
void	ga_crossover_double_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_double_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );
void	ga_crossover_char_singlepoints( population *pop,
                                     entity *father, entity *mother,
                                     entity *son, entity *daughter );
void	ga_crossover_char_doublepoints( population *pop,
                                     entity *father, entity *mother,
                                     entity *son, entity *daughter );
void	ga_crossover_bitstring_singlepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_bitstring_doublepoints(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_bitstring_mixing(population *pop, entity *father, entity *mother, entity *son, entity *daughter);
void	ga_crossover_bitstring_allele_mixing( population *pop,
                                 entity *father, entity *mother,
                                 entity *son, entity *daughter );


void	ga_mutate_integer_singlepoint_drift(population *pop, entity *father, entity *son);
void	ga_mutate_integer_singlepoint_randomize(population *pop, entity *father, entity *son);
void	ga_mutate_integer_multipoint(population *pop, entity *father, entity *son);
void	ga_mutate_integer_allpoint(population *pop, entity *father, entity *son);
void	ga_mutate_boolean_singlepoint(population *pop, entity *father, entity *son);
void	ga_mutate_boolean_multipoint(population *pop, entity *father, entity *son);
void	ga_mutate_char_singlepoint_drift(population *pop, entity *father, entity *son);
void	ga_mutate_char_singlepoint_randomize(population *pop, entity *father, entity *son);
void	ga_mutate_char_allpoint(population *pop, entity *father, entity *son);
void	ga_mutate_char_multipoint(population *pop, entity *father, entity *son);
void	ga_mutate_printable_singlepoint_drift(population *pop, entity *father, entity *son);
void	ga_mutate_printable_singlepoint_randomize(population *pop, entity *father, entity *son);
void	ga_mutate_printable_multipoint(population *pop, entity *father, entity *son);
void	ga_mutate_bitstring_singlepoint(population *pop, entity *father, entity *son);
void	ga_mutate_double_singlepoint_randomize( population *pop,
                                              entity *father, entity *son );
void	ga_mutate_double_singlepoint_drift( population *pop,
                                          entity *father, entity *son );


boolean	ga_seed_boolean_random(population *pop, entity *adam);
boolean	ga_seed_integer_random(population *pop, entity *adam);
boolean	ga_seed_integer_zero(population *pop, entity *adam);
boolean	ga_seed_double_random(population *pop, entity *adam);
boolean	ga_seed_double_zero(population *pop, entity *adam);
boolean	ga_seed_char_random(population *pop, entity *adam);
boolean	ga_seed_printable_random(population *pop, entity *adam);
boolean	ga_seed_bitstring_random(population *pop, entity *adam);


void	ga_replace_by_fitness(population *pop, entity *child);

double	ga_entity_get_fitness(entity *e);
boolean	ga_entity_set_fitness(entity *e, double fitness);
int	ga_population_get_stablesize(population *pop);
int	ga_population_get_size(population *pop);
int	ga_population_get_maxsize(population *pop);
boolean	ga_population_set_stablesize(population *pop, int stable_size);


#include "ga_utility.h"	/* Hmm. */

#endif	/* GA_CORE_H_INCLUDED */

