/**********************************************************************
  ga_utility.h
 **********************************************************************

  ga_utility - High-level genetic algorithm routines.
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

  Synopsis:     High-level GA functions and convenience functions.

 **********************************************************************/

#ifndef GA_UTILITY_H_INCLUDED
#define GA_UTILITY_H_INCLUDED

/*
 * Includes.
 */
#include "gaul.h"

/*
 * Prototypes.
 */
void    ga_diagnostics( void ); 
population *ga_genesis( const int               population_size,
                        const int               num_chromo,
                        const int               len_chromo,
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
population *ga_genesis_int( const int           population_size,
                        const int               num_chromo,
                        const int               len_chromo,
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
population *ga_genesis_boolean( const int               population_size,
                        const int               num_chromo,
                        const int               len_chromo,
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
population *ga_genesis_char( const int               population_size,
                        const int               num_chromo,
                        const int               len_chromo,
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
population *ga_genesis_double( const int               population_size,
                        const int               num_chromo,
                        const int               len_chromo,
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
entity  *ga_allele_search(      population      *pop,
                                const int       chromosomeid,
                                const int       point,
                                const int       min_val, 
                                const int       max_val, 
                                entity          *initial );
void ga_population_dump(population *pop);
void ga_entity_dump(population *pop, entity *john);
boolean ga_fitness_mean_stddev( population *pop,
                             double *average, double *stddev );
boolean ga_fitness_stats( population *pop,
                          double *max, double *min,
                          double *mean, double *median,
                          double *variance, double *stddev,
                          double *kurtosis, double *skew );

#endif	/* GA_UTILITY_H_INCLUDED */

