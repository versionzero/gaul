/**********************************************************************
  ga_intrinsics.h
 **********************************************************************

  ga_intrinsics - Genetic algorithm routine intrinsics.
  Copyright ©2002-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Wrappers around the routines for handling populations
		and performing GA operations.

 **********************************************************************/

#ifndef GA_INTRINSICS_H_INCLUDED
#define GA_INTRINSICS_H_INCLUDED

/*
 * Includes
 */
#include "gaul.h"

#if HAVE_SLANG == 1
#include <slang.h>
#endif

/*
 * Function prototypes.
 */

int	ga_population_new_slang(int *stable_size,
				int *num_chromosome,
				int *len_chromosome);
int	ga_entity_seed_slang(int *pop, int *id);
int	ga_entity_id_from_rank_slang(int *pop, int *rank);
int	ga_entity_rank_from_id_slang(int *pop, int *id);
int	ga_population_seed_slang(int *pop);
int	ga_population_save_slang(int *pop, char *fname);
int	ga_population_read_slang(char *fname);
int	ga_entity_compare_genome_slang(int *pop, int *joe, int *fred);
int	ga_entity_cross_compare_genome_slang(int *jackpopid, int *jack, 
                                           int *jillpopid, int *jill);
int	ga_entity_kill_slang(int *pop, int *id);
int	ga_entity_erase_slang(int *pop, int *id);
int	ga_entity_new_slang(int *pop);
int	ga_entity_clone_slang(int *pop, int *parent);
int	ga_entity_copy_chromosome_slang(int *pop, int *parent, int *child, int *chromosome);
int	ga_entity_copy_all_chromosomes_slang(int *pop, int *parent, int *child);
int	ga_entity_migrate_slang(int *srcpopid, int *destpopid, int *jacques);
int	ga_crossover_chromosome_singlepoints_slang(int *popid,
                     int *father, int *mother, int *son, int *daughter);
int	ga_crossover_chromosome_mixing_slang(int *popid,
                     int *father, int *mother, int *son, int *daughter);
/*
boolean	ga_singlepoint_drift_mutation(entity *father, entity *son);
boolean	ga_singlepoint_randomize_mutation(entity *father, entity *son);
boolean	ga_multipoint_mutation(entity *father, entity *son);
*/
int	ga_entity_score_slang(int *popid, int *joe);
int	ga_evolution_slang(       int     *pop,
                                int     *max_generations );
int	ga_evolution_forked_slang(       int     *pop,
                                int     *max_generations );
int	ga_population_get_size_slang(int *pop);
int	ga_population_get_maxsize_slang(int *pop);
int	ga_population_get_stablesize_slang(int *pop);
int	ga_population_set_stablesize_slang(int *pop, int *size);
int	ga_population_get_chromosomenum_slang(int *pop);
int	ga_population_get_chromosomelen_slang(int *pop);
double	ga_entity_get_fitness_slang(int *pop, int *id);
int	ga_entity_isallocated_slang(int *pop, int *id);
int	ga_extinction_slang(int *pop);
int	ga_genocide_slang(int *pop, int *target_size);
int	ga_allele_search_slang(   int     *popid,
                                int     *chromosomeid,
                                int     *point,
                                int     *min_val,
                                int     *max_val,
                                int     *entity_id );
int	ga_sa_slang(int	*popid,
			int	*entity_id,
			int	*max_iterations );
int	ga_tabu_slang(int	*popid,
			int	*entity_id,
			int	*max_iterations );
int	ga_simplex_slang(int	*popid,
			int	*entity_id,
			int	*max_iterations );
int	ga_nahc_slang(	int	*pop,
			int	*entity_id,
			int	*num_iterations);
int	ga_rmhc_slang(	int	*pop,
			int	*entity_id,
			int	*num_iterations);
int	ga_population_score_and_sort_slang(int *popid);
int	ga_population_sort_slang(int *popid);

boolean	ga_slang_seed(population *pop, entity *adam);
boolean	ga_slang_select_one(population *pop, entity **mother);
boolean	ga_slang_select_two(population *pop, entity **mother, entity **father);
entity	*ga_slang_adapt(population *pop, entity *child);
void	ga_slang_crossover(population *pop, entity *father, entity *mother, entity *daughter, entity *son);
void	ga_slang_mutate(population *pop, entity *father, entity *son);
void	ga_slang_replace(population *pop, entity *child);
boolean	ga_intrinsic_sladd(void);

#endif	/* GA_INTRINSICS_H_INCLUDED */

