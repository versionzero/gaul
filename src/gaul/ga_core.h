/**********************************************************************
  ga_core.h
 **********************************************************************

  ga_core - Genetic algorithm routines.
  Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Routines for handling populations and performing GA
		operations.

 **********************************************************************/

#ifndef GA_CORE_H_INCLUDED
#define GA_CORE_H_INCLUDED

/*
 * Includes.
 */
#include "gaul.h"

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
# if GA_DEBUG > 1
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
#include "ga_deterministiccrowding.h"
#include "ga_gradient.h"
#include "ga_optim.h"
#include "ga_qsort.h"
#include "ga_randomsearch.h"
#include "ga_sa.h"
#include "ga_similarity.h"
#include "ga_systematicsearch.h"
#include "ga_simplex.h"
#include "ga_tabu.h"

/*
 * Compilation constants.
 */
#define GA_BOLTZMANN_FACTOR	(1.38066e-23)
#define GA_TINY_DOUBLE		(1.0e-9)

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
 * Deterministic crowding parameter structure.
 */
typedef struct
  {
  GAcompare	compare;	/* Compare two entities (either genomic or phenomic space). */
  } ga_dc_t;

/*
 * Gradient methods parameter structure.
 */
typedef struct
  {
  int		dimensions;	/* Size of double array. */
  double	step_size;	/* Step size, or initial step size. */
  GAto_double	to_double;	/* Convert chromosome to double array. */
  GAfrom_double	from_double;	/* Convert chromosome from double array. */
  GAgradient	gradient;	/* Return gradients array. */
  } ga_gradient_t;

/*
 * Systematic search parameter structure.
 */
typedef struct
  {
  GAscan_chromosome	scan_chromosome;	/* Allele searching function. */
  int			chromosome_state;	/* Permutation counter. */
  int			allele_state;		/* Permutation counter. */
  } ga_search_t;

/*
 * Probabilistic sampling parameter structure.
 */
typedef struct
  {
  int			**num_states;		/* Number of states for each allele. */
  } ga_sampling_t;

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
  int		generation;		/* For ga_population_get_generation(). */

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
  ga_tabu_t		*tabu_params;		/* Parameters for tabu-search. */
  ga_sa_t		*sa_params;		/* Parameters for simulated annealling. */
  ga_climbing_t		*climbing_params;	/* Parameters for hill climbing. */
  ga_simplex_t		*simplex_params;	/* Parameters for simplex search. */
  ga_dc_t		*dc_params;		/* Parameters for deterministic crowding. */
  ga_gradient_t		*gradient_params;	/* Parameters for gradient methods. */
  ga_search_t		*search_params;		/* Parameters for systematic search. */
  ga_sampling_t		*sampling_params;	/* Parameters for probabilistic sampling. */

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
 * FIXME: There should be no purpose for these 3 constants.  I'd say
 * they result from inflexibilities in the GAUL library.  Consider
 * them as marked for removal!
 */
#define GA_MULTI_BIT_CHANCE	(0.02)
#define GA_ELITISM_MULTIPLIER	(0.05)
#define GA_ELITISM_CONSTANT	(2.0)

/* Define lower bound on fitness. */
#define GA_MIN_FITNESS		DBL_MIN

/* Final includes. */
#include "gaul/ga_utility.h"	/* Hmm. */

#endif	/* GA_CORE_H_INCLUDED */

