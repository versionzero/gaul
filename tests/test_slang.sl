%######################################################################
% gaul/tests/test_slang.sl
%######################################################################
%
% GAUL - Genetic Algorithm Utility library.
% Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
%
% The latest version of this program should be available at:
% http://gaul.sourceforge.net/
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.  Alternatively, if your project
% is incompatible with the GPL, I will probably agree to requests
% for permission to use the terms of any other license.
%
% This program is distributed in the hope that it will be useful, but
% WITHOUT ANY WARRANTY WHATSOEVER.
%
% A full copy of the GNU General Public License should be in the file
% "COPYING" provided with this distribution; if not, see:
% http://www.gnu.org/
%
%######################################################################

% Enable debugging facilities.
_debug_info = 1;
_traceback = 1;

define test_evolution()
  {
  variable	population;	% Population handle.
  variable	best;		% Entity handle of most fit solution.
  variable	fitness;	% Fitness of best solution.

% Prepare new population.
% This function must be equivalent to the ga_genesis_XXX() C API
% functions.  To use S-Lang operators, the callbacks should be
% defined as "ga_slang_mutate()", "ga_slang_crossover()", etc. which
% will call the S-Lang defined functions "mutate_hook()",
% "crossover_hook()", etc.
  message( ">> Creating population." );
  population = test_slang_genesis(100);

% Evolutionary parameters are:
% Type of evolution.
% Type of elitism.
% Crossover rate
% Mutation rate
% Migration rate
  message( ">> Defining evolutionary parameters." );
  ga_population_set_parameters(population, GA_SCHEME_DARWIN, GA_ELITISM_PARENTS_DIE, 0.8, 0.05, 0.25 );

  message( ">> Evolution." );
  ga_evolution( population, 2000);

%
% Display the best fitness.
%
  best = ga_entity_id_from_rank(population, 0);
  fitness = ga_entity_get_fitness(population, best);
  message(">> best fitness = "+string(fitness));

%
% Write the population in a native, non-portable binary format.
%
  message( ">> Writing population." );
  ga_population_write( population, "test.pop" );

%
% Kill of least fit individuals.
% The second argument specifies the number of individuals to _survive_.
%
  message( ">> Genocide." );
  ga_genocide( population, 20 );

%
% Destroy the entire population.
%
  message( ">> Extinction." );
  ga_extinction( population );

%
% Read the binary format population file.
%
  message( ">> Reading population." );
  population = ga_population_read( "test.pop" );
  test_slang_set_evaluation_function(population);
  ga_population_score_and_sort(population);

  best = ga_entity_id_from_rank(population, 0);
  fitness = ga_entity_get_fitness(population, best);
  message(">> best fitness = "+string(fitness));

  ga_extinction( population );
  }


%
% This is an example S-Lang function called to perform a
% fitness evaluation.
%
define scoring_callback()
  {
  message(">> S-Lang scoring function successfully called." );

  return 0.0;	% Must return a double precision result.
  }


% MAIN.
message( ">> This is an S-Lang GAUL test script." );
message( ">> See source for documentation." );

test_evolution();

message( ">> The S-Lang GAUL test script is complete." );

test_slang_quit();

% The end.
