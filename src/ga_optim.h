/**********************************************************************
  ga_optim.h
 **********************************************************************

  ga_optim - Gene-based optimisation routines.
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

 **********************************************************************/

#ifndef GA_OPTIM_H_INCLUDED
#define GA_OPTIM_H_INCLUDED

/*
 * Includes
 */
#include "SAA_header.h"

#include "ga_core.h"

/*
 * Callback function prototype.
 */
typedef void    (*GAspecificmutate)(int chromo, int point, int *data);

/*
 * Prototypes
 */
boolean ga_evolution(	population		*pop,
			const ga_class_type	class,
			const ga_elitism_type	elitism,
			const int		max_generations );
boolean ga_evolution_steady_state(	population		*pop,
			const ga_class_type	class,
			const int		max_iterations );
entity	*ga_random_mutation_hill_climbing(	population	*pop,
			entity			*initial,
			const int		num_iterations);
entity	*ga_next_ascent_hill_climbing(  population              *pop,
                                        entity                  *initial,
                                        const int               max_iterations,
                                        GAspecificmutate        mutationfunc);
entity	*ga_metropolis_mutation(	population		*pop,
			entity			*initial,
			const int		num_iterations,
			const int 		temperature);
entity	*ga_simulated_annealling_mutation(population      *pop,
                                entity          *initial,
                                const int       num_iterations,
                                const int       initial_temperature,
                                const int       final_temperature);

#endif	/* GA_OPTIM_H */
