/**********************************************************************
  ga_util.c
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

		Also contains a number of helper functions providing
		alternative optimisation schemes for comparison and
		analysis purposes.

		BEWARE: NOT THREAD-SAFE!

		Internally, and in the public C interface, pointers
		are used to identify the population and entity
		structures.  However, in the scripting interface these
		pointers are unusable, so identifing integers are
		used instead.

		Note that the temperatures in the simulated annealling
		and MC functions do not exactly run from the initial
		temperature to the final temperature.  They are offset
		slightly so that sequential calls to these functions
		will have a linear temperature change.

  Vague usage details:	Set-up with ga_genesis().
			Perform calculations with ga_evolution().
			Grab data for post-analysis with ga_transcend().
			Evolution will continue if ga_evolution() is
			called again without calling ga_genesis() again.

  To do:	Reading/writing 'soup'.
		Replace the send_mask int array with a bit vector.
		Use Memory Chunks for efficient memory allocation?
		Instead of using a 'allocated' flag for each entity, keep a list of unused entities.  (Or use mem_chunks).
		Use table structure for maintainance of multiple populations.
		All functions here should be based on entity/population _pointers_ while the functions in ga_intrinsics should be based on _handles_.
		ga_population_new() should return the reference pointer not the handle.
		Consider using the table structure, coupled with memory chunks, for handling the entities within a population.  This should be as efficient as the current method, and will remove the need for a maximum population size.
		More "if (!pop) die("Null pointer to population structure passed.");" checks are needed.
		Use a list of structures containing info about user defined crossover and mutation functions.
		Genome distance measures (tanimoto, euclidean, tverski etc.)
		Rename this as ga_core or ga_base?

		Population/entity iterator functions.

 **********************************************************************/

#include "ga_util.h"

#include "ga_crossover.c"
#include "ga_mutate.c"
#include "ga_replace.c"
#include "ga_seed.c"
#include "ga_select.c"

/*
 * Global variables.
 */
#if 0
THREAD_LOCK_DEFINE_STATIC(ga_active_population);
population	*pop=NULL;		/* The active population. */
unsigned int	pop_id=0;		/* The active population's id. */
#endif

THREAD_LOCK_DEFINE_STATIC(pop_table);
TableStruct	*pop_table=NULL;	/* The population table. */

/**********************************************************************
  Private utility functions.
 **********************************************************************/

/**********************************************************************
  destruct_list()
  synopsis:	Destroys an userdata list and it's contents.  For
		many applications, the destructor callback will just
		be free() or similar.  This callback may safely be
		NULL.
  parameters:
  return:
  last updated:	20/12/00
 **********************************************************************/

static void destruct_list(population *pop, SLList *list)
  {
  SLList        *this;		/* Current list element */
  int		num_destroyed;	/* Count number of things destroyed. */

/* A bit of validation. */
  if ( !pop->data_destructor ) return;
  if ( !list ) die("Null pointer to list passed.");

/* Deallocate individual structures. */
  num_destroyed = 0;
  this=list;

  while(this!=NULL)
    {
    if (this->data)
      {
      pop->data_destructor(this->data);
      num_destroyed++;
      }
    this=slink_next(this);
    }

/* Deallocate list sructure. */
  slink_free_all(list);

#if GA_DEBUG>2
  /*
   * Not typically needed now, because num_destrtoyed may (correctly) differ
   * from the actual number of chromosomes.
   */
  if (num_destroyed != pop->num_chromosomes)
    printf("Uh oh! Dodgy user data here? %d %d\n", num_destroyed, pop->num_chromosomes);
#endif

  return;
  }


/**********************************************************************
  Population handling functions.
 **********************************************************************/

/**********************************************************************
  ga_population_new()
  synopsis:	Allocates and initialises a new population structure,
		and assigns a new population id to it.
  parameters:
  return:	population *	new population structure.
  last updated: 19/01/01
 **********************************************************************/

population *ga_population_new(	const int max_size,
				const int stable_size,
				const int num_chromosome,
				const int len_chromosome)
  {
  int		i;		/* Loop variable. */
  population	*newpop=NULL;	/* New population structure. */
  unsigned int	pop_id;		/* Handle for new population structure. */

  if ( !(newpop = s_malloc(sizeof(population))) )
    die("Unable to allocate memory");

  newpop->size = 0;
  newpop->stable_size = stable_size;
  newpop->max_size = max_size;
  newpop->orig_size = 0;
  newpop->num_chromosomes = num_chromosome;
  newpop->len_chromosomes = len_chromosome;
  newpop->data = NULL;

  newpop->crossover_ratio = 1.0;
  newpop->mutation_ratio = 1.0;
  newpop->migration_ratio = 1.0;

  if ( !(newpop->entity_array = s_malloc(max_size*sizeof(entity))) )
    die("Unable to allocate memory");

  if ( !(newpop->entity_iarray = s_malloc(max_size*sizeof(entity*))) )
    die("Unable to allocate memory");
  
  for (i=0; i<max_size; i++)
    {
    newpop->entity_array[i].allocated=FALSE;
    newpop->entity_array[i].data=NULL;
    newpop->entity_array[i].fitness=-99999.0;	/* Lower bound on fitness */

    newpop->entity_array[i].chromosome=NULL;

/*    newpop->entity_iarray[i] = &(newpop->entity_array[i]);*/
    }

/*
 * Clean the callback functions.
 * Prevents eronerous callbacks - helpful when debugging!
 */
  newpop->generation_hook = NULL;
  newpop->iteration_hook = NULL;

  newpop->data_destructor = NULL;
  newpop->data_ref_incrementor = NULL;

  newpop->evaluate = NULL;
  newpop->seed = NULL;
  newpop->adapt = NULL;
  newpop->select_one = NULL;
  newpop->select_two = NULL;
  newpop->mutate = NULL;
  newpop->crossover = NULL;
  newpop->replace = NULL;

/*
 * Seed the population.
 */

/* FIXME: I should allocate the chromosomes here too. */

/*
 * Add this new population into the population table.
 */
  THREAD_LOCK(pop_table);
  if (!pop_table) pop_table=table_new();

  pop_id = table_add(pop_table, (vpointer) newpop);
  THREAD_UNLOCK(pop_table);

  plog( LOG_DEBUG, "New pop = %p id = %d", newpop, pop_id);

  return newpop;
  }


#if 0
/**********************************************************************
  ga_set_active_population()
  synopsis:	Sets the active population.
  parameters:	unsigned int	id for active population.
  return:	Success/Failure.
  last updated: 19/01/01
 **********************************************************************/

boolean ga_set_active_population(unsigned int id)
  {
  THREAD_LOCK(pop_table);
  if (pop_id != id)
    {
    pop = table_get_data(pop_table, id);
    pop_id = id;
    }
  THREAD_UNLOCK(pop_table);

  return TRUE;
  }
#endif


/**********************************************************************
  ga_get_population_from_id()
  synopsis:	Get population pointer from it's internal id.
  parameters:	unsigned int	id for population.
  return:	int
  last updated: 22/01/01
 **********************************************************************/

population *ga_get_population_from_id(unsigned int id)
  {
  return (population *) table_get_data(pop_table, id);
  }


/**********************************************************************
  ga_get_population_id()
  synopsis:	Get population internal id from it's pointer.
  parameters:	unsigned int	id for population.
  return:	int
  last updated: 22/01/01
 **********************************************************************/

unsigned int ga_get_population_id(population *pop)
  {
  return table_lookup_index(pop_table, (vpointer) pop);
  }


#if 0
/**********************************************************************
  ga_entity_seed_random()
  synopsis:	Fills a population structure with vaguely reasonable,
		random, starting genes.  Most 'real' work is done in
		a user-specified function.
  parameters:	population *	The entity's population.
		entity *	The entity.
  return:	boolean success.
  last updated: 19/12/00
 **********************************************************************/

boolean ga_entity_seed_random(population *pop, entity *e)
  {
  int		j;		/* Loop variable. */

  for (j=0; j<pop->num_chromosomes; j++)
    pop->seed_random(j, e->chromosome[j]);

  return TRUE;
  }
#endif


/**********************************************************************
  ga_entity_seed()
  synopsis:	Fills a population structure with genes.  Defined in
		a user-specified function.
  parameters:	population *	The entity's population.
		entity *	The entity.
  return:	boolean success.
  last updated:	28/02/01
 **********************************************************************/

boolean ga_entity_seed(population *pop, entity *adam)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!pop->seed) die("Population seeding function is not defined.");

  pop->seed(pop, adam);

  return TRUE;
  }


/**********************************************************************
  ga_population_seed()
  synopsis:	Fills all entities in a population structure with
		genes from a user-specified function.
  parameters:	none
  return:	boolean success.
  last updated: 28/02/01
 **********************************************************************/

boolean ga_population_seed(population *pop)
  {
  int		i;		/* Loop variables. */
  entity	*adam;		/* New population member. */

  plog(LOG_DEBUG, "Population seeding by user-defined genesis.");

  if (!pop) die("Null pointer to population structure passed.");
  if (!pop->seed) die("Population seeding function is not defined.");

  for (i=0; i<pop->stable_size; i++)
    {
    adam = ga_get_free_entity(pop);
    pop->seed(pop, adam);
    }

  return TRUE;
  }


#if 0
/**********************************************************************
  ga_population_seed_random()
  synopsis:	Fills a population structure with vaguely reasonable,
		random, starting genes.  Most 'real' work is done in
		a user-specified function.
  parameters:	none
  return:	boolean success.
  last updated: 18/12/00
 **********************************************************************/

boolean ga_population_seed_random(population *pop)
  {
  int		i;		/* Loop variables. */
  entity	*adam;		/* New population member. */

  plog(LOG_DEBUG, "Population seeding by random genesis.");

  if (!pop) die("Null pointer to population structure passed.");

  for (i=0; i<pop->stable_size; i++)
    {
    adam = ga_get_free_entity(pop);
    ga_entity_seed_random(pop, adam);
    }

  return TRUE;
  }
#endif


#if 0
/**********************************************************************
  ga_population_seed_pdb()
  synopsis:	Seed population structure with starting genes based
		on structures from a set of PDB files.
		NOTE: Should do this through the function defined
		above instead.
  parameters:
  return:
  last updated: 06/07/00
 **********************************************************************/

boolean ga_population_seed_pdb( population *pop, const char *fname )
  {
  FILE          *fp;		/* File ptr */
  char          *line=NULL;	/* Line buffer */
  int           line_len;	/* Current buffer size */
  molstruct     *mol;		/* Current molstruct structure */
  int		pdb_count=0;	/* Number of pdb files read */
  entity	*entity=NULL;	/* Entity being defined */

  plog(LOG_DEBUG, "Population seeding by reading set of PDB structures.");
  plog(LOG_FIXME, "BROKEN!");

  if (!fname) die("Null pointer to filename passed.");
  if (!pop) die("Null pointer to population structure passed.");

  while (pdb_count < pop->stable_size)
    {
    /* Open file */
    if( !(fp=fopen(fname,"r")) )
      dief("Cannot open file of PDB filenames \"%s\".", fname);

/*
 * Read lines.  Each specifies one filename except those starting with '#'
 * which are comment lines and are ignored
 */
    while (!feof(fp) && pdb_count < pop->stable_size)
      {
      /* read line */
      line=str_getline(fp, &line_len);

      if (line_len<3)
        { /* This is an ugly hack */
        plog(LOG_DEBUG, "A line of garbage has been read.");
        }
      else
        {
        if (line[0] != '#')
          {
          plog(LOG_DEBUG, "Reading PDB file \"%s\"", line);

          if ( !(mol = mol_read_pdb(line)) ) dief("Problem with PDB file \"%s\".", line);

/* Get new entity structure, if needed */
          if ( entity == NULL )
            {
            entity = ga_get_free_entity(pop);
            }

/* Find TM helices. */
          if ( helga_split_mol_into_helices(entity, mol, pop->num_chromosomes, pop->len_chromosomes) )
            {
            pdb_count++;
            entity = NULL;
            }

#if 0
/* Encode TM helices into genes. */
          helga_encode(entity, pop->num_chromosomes);
#endif

/* Deallocate the read structure */
          mol_molstruct_free_all(mol);
          }
        else
          {
          plog(LOG_VERBOSE, "Comment read: \"%s\"", line);
          }
        }

      plog(LOG_DEBUG, "%d PDB structures read.", pdb_count);

      s_free(line);
      }

    plog(LOG_DEBUG, "After pass of input data we have %d structures (Want %d).\n", pdb_count, pop->stable_size);

/* Remember to close the file. */
    fclose(fp);
    }

/* Check */
  if (pdb_count != pop->size)
    {
    dief("Population size is %d, but should be %d.", pop->size, pdb_count);
    }

/* Dereference entity structure -- no, no, no, never needed. */
/*
  if ( entity ) ga_entity_dereference(entity);
*/

  plog(LOG_DEBUG, "Population now contains %d members.", pop->size);

  return TRUE;
  }
#endif


/**********************************************************************
  ga_population_seed_soup()
  synopsis:	Seed a population structure with starting genes from
		a previously created soup file.
  parameters:
  return:
  last updated: 06/07/00
 **********************************************************************/

boolean ga_population_seed_soup(population *pop, const char *fname)
  {
#if 0
  int		i, j, k;	/* Loop variables */
  entity	*this_entity;	/* Current population member */
  quaternion	q;		/* Quaternion */
  FILE          *fp;		/* File ptr */
  char          *line=NULL;	/* Line buffer */
  int           line_len;	/* Current buffer size */
  molstruct     *mol;		/* Current molstruct structure */
#endif

  plog(LOG_DEBUG, "Population seeding by reading soup file.");
  plog(LOG_FIXME, "Code incomplete.");

  if (!fname) die("Null pointer to filename passed.");
  if (!pop) die("Null pointer to population structure passed.");

#if 0
/*
 * Open CRD format file
 */
  FILE          *fp;		/* File ptr */
  if((fp=fopen(fname,"r"))==NULL)
    dief("Cannot open SOUP file \"%s\".", fname);
#endif


  return TRUE;
  }


/**********************************************************************
  ga_write_soup()
  synopsis:	Writes a soup file based on the current gene pool.
  parameters:
  return:
  last updated: 06/07/00
 **********************************************************************/

boolean ga_write_soup(population *pop)
  {

  plog(LOG_DEBUG, "Writing soup file.");
  plog(LOG_FIXME, "Code incomplete.");

  return TRUE;
  }


/**********************************************************************
  ga_population_save()
  synopsis:	Writes entire population and it's genetic data to disk.
		Note: Currently does not store any of the userdata --
		just stores the genes and fitness.
  parameters:
  return:
  last updated: 12/01/01
 **********************************************************************/

boolean ga_population_save(population *pop, char *fname)
  {
  int		i, j, k;	/* Loop variables. */
  FILE          *fp;		/* File handle. */
  int		*chromosome;	/* Current chromosome. */

  plog(LOG_DEBUG, "Saving population to disk.");
  if (!fname) die("Null pointer to filename passed.");
  if (!pop) die("Null pointer to population structure passed.");

/*
 * Open output file.
 */
  if((fp=fopen(fname,"w"))==NULL)
    dief("Cannot open population file \"%s\" for output.", fname);

/*
 * Program info.
 */
  fprintf(fp, "ga_util.c ga_population_save(\"%s\")\n", fname);
  fprintf(fp, "%s %s\n", VERSION_STRING, BUILD_DATE_STRING);

/*
 * Population info.
 */
  fprintf(fp, "%d %d %d %d %d\n", pop->max_size, pop->size, pop->stable_size,
                  pop->num_chromosomes, pop->len_chromosomes);

/*
 * Entity info.
 */
  for (i=0; i<pop->size; i++)
    {
    fprintf(fp, "%f\n", pop->entity_iarray[i]->fitness);
    for (j=0; j<pop->num_chromosomes; j++)
      {
      chromosome = pop->entity_iarray[i]->chromosome[j];

      for (k=0; k<pop->len_chromosomes; k++)
        fprintf(fp, "%d ", chromosome[k]);

      fprintf(fp, "\n");
      }
    plog(LOG_DEBUG, "Entity %d has fitness %f", i, pop->entity_iarray[i]->fitness);
    }

/*
 * Footer info.
 */
  fprintf(fp, "That's all folks!\n"); 

/*
 * Close file.
 */
  fclose(fp);

  return TRUE;
  }


/**********************************************************************
  ga_population_read()
  synopsis:	Reads entire population and it's genetic data back
		from disk.
  parameters:
  return:
  last updated: 06/07/00
 **********************************************************************/

population *ga_population_read(char *fname)
  {
  int		i, j, k;		/* Loop variables. */
  FILE          *fp;			/* File handle. */
  int		*chromosome;		/* Current chromosome. */
  char		origfname[MAX_LINE_LEN];	/* Original filename. */
  char		version_str[MAX_LINE_LEN], build_str[MAX_LINE_LEN];	/* Version details. */
  char  	test_str[MAX_LINE_LEN];	/* Test string. */
  int		max_size, size, stable_size, num_chromosomes, len_chromosomes;
  population	*pop=NULL;		/* New population structure. */
  entity	*newentity;		/* New entity in new population. */

  plog(LOG_DEBUG, "Reading population from disk.");
  if (!fname) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if((fp=fopen(fname,"r"))==NULL)
    dief("Cannot open population file \"%s\" for input.", fname);

/*
 * Program info.
 * FIXME: Potential buffer overflow.
 */
  fscanf(fp, "ga_util.c ga_population_save(\"%[^\"]\")\n", origfname);
  fscanf(fp, "%s %s\n", version_str, build_str);

  plog(LOG_DEBUG, "Reading \"%s\", \"%s\", \"%s\"",
               origfname, version_str, build_str);

/*
 * Population info.
 */
  fscanf(fp, "%d %d %d %d %d", &max_size, &size, &stable_size,
                  &num_chromosomes, &len_chromosomes);

/*
 * Allocate a new population structure.
 */
  pop = ga_population_new(max_size, stable_size,
                    num_chromosomes, len_chromosomes);

/*
 * Got a population structure?
 */
  if (!pop) die("Unable to allocate population structure.");

/*
 * Entity info.
 */
  for (i=0; i<size; i++)
    {
    newentity = ga_get_free_entity(pop);
    fscanf(fp, "%lf", &(newentity->fitness));

    for (j=0; j<pop->num_chromosomes; j++)
      {
      chromosome = newentity->chromosome[j];

      for (k=0; k<pop->len_chromosomes; k++)
        fscanf(fp, "%d", &(chromosome[k]));
      }
    plog(LOG_DEBUG, "Entity %d has fitness %f", i, newentity->fitness);
    }

/*
 * Footer info.
 */
  fscanf(fp, "%s", test_str); 
/* FIXME: Kludge: */
  if (strncmp(test_str, "That's all folks!", 6)) die("File appears to be corrupt.");

/*
 * Close file.
 */
  fclose(fp);

  plog(LOG_DEBUG, "Have read %d entities.", pop->size);

  return pop;
  }


/**********************************************************************
  ga_population_score_and_sort()
  synopsis:	Score and sort entire population.  This is probably
		a good idea after reading the population from disk!
		Note: remember to define the callback functions first.
  parameters:
  return:
  last updated: 28/02/01
 **********************************************************************/

boolean ga_population_score_and_sort(population *pop)
  {
  int		i;		/* Loop variable over all entities. */
  double	origfitness;	/* Stored fitness value. */

/*
 * Score and sort the read population members.
 *
 * Note that this will (potentially) use a huge amount of memory more
 * than the original population data if the userdata hasn't been maintained.
 * Each chromosome is decoded separately, whereas originally many
 * degenerate chromosomes would share their userdata elements.
 */
  for (i=0; i<pop->size; i++)
    {
    origfitness = pop->entity_iarray[i]->fitness;
    pop->evaluate(pop, pop->entity_iarray[i]);

#if GA_DEBUG>2
    if (origfitness != pop->entity_iarray[i]->fitness)
      plog(LOG_NORMAL,
           "Recalculated fitness %f doesn't match stored fitness %f for entity %d.",
           pop->entity_iarray[i]->fitness, origfitness, i);
#endif
    }

  quicksort_population(pop);

  return TRUE;
  }


/**********************************************************************
  ga_population_convergence_genotypes()
  synopsis:	Determine ratio of converged genotypes in population.
  parameters:
  return:
  last updated: 31/05/01
 **********************************************************************/

double ga_population_convergence_genotypes( population *pop )
  {
  int		i, j;		/* Loop over pairs of entities. */
  int		total=0, converged=0;	/* Number of comparisons, matches. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");

  for (i=1; i<pop->size; i++)
    {
    for (j=0; j<i; j++)
      {
      if (ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[j]))
        converged++;
      total++;
      }
    }

  return (double) converged/total;
  }


/**********************************************************************
  ga_population_convergence_chromosomes()
  synopsis:	Determine ratio of converged chromosomes in population.
  parameters:
  return:
  last updated: 31/05/01
 **********************************************************************/

double ga_population_convergence_chromosomes( population *pop )
  {
  int		i, j;		/* Loop over pairs of entities. */
  int		k;		/* Loop over chromosomes. */
  int		total=0, converged=0;	/* Number of comparisons, matches. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");

  for (i=1; i<pop->size; i++)
    {
    for (j=0; j<i; j++)
      {
      for (k=0; k<pop->num_chromosomes; k++)
        {
/* FIXME: Not totally effiecient: */
        if (ga_count_match_alleles( pop->len_chromosomes,
                                    pop->entity_iarray[i]->chromosome[k],
                                    pop->entity_iarray[j]->chromosome[k] ) == pop->len_chromosomes)
          converged++;
        total++;
        }
      }
    }

  return (double) converged/total;
  }


/**********************************************************************
  ga_population_convergence_alleles()
  synopsis:	Determine ratio of converged alleles in population.
  parameters:
  return:
  last updated: 31/05/01
 **********************************************************************/

double ga_population_convergence_alleles( population *pop )
  {
  int		i, j;		/* Loop over pairs of entities. */
  int		k;		/* Loop over chromosomes. */
  int		total=0, converged=0;	/* Number of comparisons, matches. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");

  for (i=1; i<pop->size; i++)
    {
    for (j=0; j<i; j++)
      {
      for (k=0; k<pop->num_chromosomes; k++)
        {
        converged+=ga_count_match_alleles( pop->len_chromosomes,
                                           pop->entity_iarray[i]->chromosome[k],
                                           pop->entity_iarray[j]->chromosome[k] );
        total+=pop->len_chromosomes;
        }
      }
    }

  return (double) converged/total;
  }


/**********************************************************************
  ga_population_stats()
  synopsis:	Determine mean and standard deviation of the fitness
		scores.
  parameters:
  return:
  last updated: 30/04/01
 **********************************************************************/

boolean ga_population_stats( population *pop,
                             double *average, double *stddev )
  {
  int		i;			/* Loop over all entities. */
  double	sum=0.0, sumsq=0.0;	/* Sum and sum squared. */

  if (!pop) die("Null pointer to population structure passed.");
  if (pop->size < 1) die("Pointer to empty population structure passed.");

  for (i=0; i<pop->size; i++)
    {
    sum += pop->entity_iarray[i]->fitness;
    sumsq += SQU(pop->entity_iarray[i]->fitness);
    }

  *average = sum / pop->size;
  *stddev = (sumsq - sum*sum/pop->size)/pop->size;

  return TRUE;
  }


/**********************************************************************
  ga_get_entity_rank()
  synopsis:	Gets an entity's rank (subscript into entity_iarray of
		the population).  This is not necessarily the fitness
		rank unless the population has been sorted.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

int ga_get_entity_rank(population *pop, entity *e)
  {
  int	rank=0;		/* The rank. */

  while (rank < pop->size)
    {
    if (pop->entity_iarray[rank] == e) return rank;
    rank++;
    }

  return -1;
  }


/**********************************************************************
  ga_get_entity_rank_from_id()
  synopsis:	Gets an entity's rank (subscript into entity_iarray of
		the population).  This is not necessarily the fitness
		rank unless the population has been sorted.
  parameters:
  return:
  last updated: 16/03/01
 **********************************************************************/

int ga_get_entity_rank_from_id(population *pop, int id)
  {
  int	rank=0;		/* The rank. */

  while (rank < pop->size)
    {
    if (pop->entity_iarray[rank] == &(pop->entity_array[id])) return rank;
    rank++;
    }

  return -1;
  }


/**********************************************************************
  ga_get_entity_id_from_rank()
  synopsis:	Gets an entity's id from its rank.
  parameters:
  return:
  last updated: 16/03/01
 **********************************************************************/

int ga_get_entity_id_from_rank(population *pop, int rank)
  {
  int	id=0;		/* The entity's index. */

  while (id < pop->max_size)
    {
    if (&(pop->entity_array[id]) == pop->entity_iarray[rank]) return id;
    id++;
    }

  return -1;
  }


/**********************************************************************
  ga_get_entity_id()
  synopsis:	Gets an entity's internal index (subscript into the
		entity_array buffer).
		FIXME: Add some more error checking.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

int ga_get_entity_id(population *pop, entity *e)
  {
  int	id=0;	/* The index. */

  if (!e) die("Null pointer to entity structure passed.");

  while (id < pop->max_size)
    {
    if (&(pop->entity_array[id]) == e) return id;
    id++;
    }

  return -1;

/*  return (&(pop->entity_array[0]) - &(e[0]))/sizeof(entity);*/
/*  return (pop->entity_array - e)/sizeof(entity);*/
  }


/**********************************************************************
  ga_get_entity_from_id()
  synopsis:	Gets a pointer to an entity from it's internal index
		(subscript into the entity_array buffer).
		FIXME: Add some error checking.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

entity *ga_get_entity_from_id(population *pop, const unsigned int id)
  {
  return &(pop->entity_array[id]);
  }


/**********************************************************************
  ga_get_entity_from_rank()
  synopsis:	Gets a pointer to an entity from it's internal rank.
		(subscript into the entity_iarray buffer).
		Note that this only relates to fitness ranking if
		the population has been properly sorted.
		FIXME: Add some error checking.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

entity *ga_get_entity_from_rank(population *pop, const unsigned int rank)
  {
  return pop->entity_iarray[rank];
  }


/**********************************************************************
  ga_entity_setup()
  synopsis:	Allocates/prepares an entity structure for use.
		Chromosomes are allocated, but will contain garbage.
  parameters:
  return:
  last updated: 19/12/00
 **********************************************************************/

boolean ga_entity_setup(population *pop, entity *joe)
  {
  int	i;	/* Loop over chromosomes. */

  if (!joe) die("Null pointer to entity structure passed.");

  if (joe->chromosome==NULL)
    {
/*
 * This would be more efficient, in terms of memory usage, if
 * I used the individual length for each chromosome.
 */
    joe->chromosome = s_malloc(pop->num_chromosomes*sizeof(int *));
    joe->chromosome[0] = s_calloc(pop->num_chromosomes,
                 pop->len_chromosomes*sizeof(int));

    for (i=1; i<pop->num_chromosomes; i++)
      {
      joe->chromosome[i] = &(joe->chromosome[i-1][pop->len_chromosomes]);
      }
    }

/* Physical characteristics currently undefined. */
  joe->data=NULL;

  joe->fitness=GA_UTIL_MIN_FITNESS;	/* Lower bound on fitness */

/* Mark it as used. */
  joe->allocated=TRUE;

  return TRUE;
  }


/**********************************************************************
  ga_entity_dereference_by_rank()
  synopsis:	Marks an entity structure as unused.
		Deallocation is expensive.  It is better to re-use this
		memory.  So, that is what we do.
		Any contents of entities data field are freed.
		If rank is known, this is much quicker than the plain
		ga_entity_dereference() function.
		Note, no error checking in the interests of speed.
  parameters:
  return:
  last updated:	16/03/01
 **********************************************************************/

boolean ga_entity_dereference_by_rank(population *pop, int rank)
  {
  int		i;	/* Loop variable over the indexed array. */
  entity	*dying=pop->entity_iarray[rank];	/* Dead entity. */

/* Bye bye entity. */
  dying->allocated=FALSE;

/* Clear user data. */
  if (dying->data)
    {
    destruct_list(pop, dying->data);
    dying->data=NULL;
    }

/* Population size is one less now! */
  pop->size--;

/* Update entity_iarray[], so there are no gaps! */
  for (i=rank; i<pop->size; i++)
    pop->entity_iarray[i] = pop->entity_iarray[i+1];

/*
  printf("ENTITY %d DEREFERENCED.\n", ga_get_entity_id(dying));
  printf("New pop size = %d\n", pop->size);
*/

  return TRUE;
  }


/**********************************************************************
  ga_entity_dereference()
  synopsis:	Marks an entity structure as unused.
		Deallocation is expensive.  It is better to re-use this
		memory.  So, that is what we do.
		Any contents of entities data field are freed.
		If rank is known, this above
		ga_entity_dereference_by_rank() function is much
		quicker.
  parameters:
  return:
  last updated:	16/03/01
 **********************************************************************/

boolean ga_entity_dereference(population *pop, entity *dying)
  {
  return ga_entity_dereference_by_rank(pop, ga_get_entity_rank(pop, dying));
  }


/**********************************************************************
  ga_entity_clear_data()
  synopsis:	Clears some of the entity's data.  Safe if data doesn't
		exist anyway.
  parameters:
  return:
  last updated: 20/12/00
 **********************************************************************/

void ga_entity_clear_data(population *p, entity *entity, const int chromosome)
  {
  SLList	*tmplist;

  if (entity->data)
    {
    tmplist = slink_nth(entity->data, chromosome);
    if (tmplist->data)
      {
      p->data_destructor(tmplist->data);
      tmplist->data=NULL;
      }
    }

  return;
  }


/**********************************************************************
  ga_entity_blank()
  synopsis:	Clears the entity's data.
		Equivalent to an optimised ga_entity_dereference()
		followed by ga_get_free_entity().  It is much more
		preferable to use this fuction!
		Chromosomes are guarenteed to be intact, but may be
		overwritten by user.
  parameters:
  return:
  last updated: 18/12/00
 **********************************************************************/

void ga_entity_blank(population *p, entity *entity)
  {
  if (entity->data)
    {
    destruct_list(p, entity->data);
    entity->data=NULL;
    }

  entity->fitness=GA_UTIL_MIN_FITNESS;

/*
  printf("ENTITY %d CLEARED.\n", ga_get_entity_id(entity));
*/

  return;
  }


/**********************************************************************
  ga_get_free_entity()
  synopsis:	Returns pointer to an unused entity structure from the
		population's entity pool.
		Increments population size also.
  parameters:
  return:
  last updated: 17/05/00
 **********************************************************************/

entity *ga_get_free_entity(population *pop)
  {
  static int index=0;

/*
  plog(LOG_DEBUG, "Locating free entity structure.");
*/

/*
 * Do we have any free structures?
 */
  if (pop->max_size == (pop->size+1))
    {
    plog(LOG_FIXME, "Should reallocate data structures here.\n");
    die("Whoops, no free structures.");
    }

/* Find unused entity structure */
  while (pop->entity_array[index].allocated==TRUE)
    {
    if (index == 0) index=pop->max_size;
    index--;
    }

/* Prepare it */
  ga_entity_setup(pop, &(pop->entity_array[index]));

/* Store in lowest free slot in entity_iarray */
  pop->entity_iarray[pop->size] = &(pop->entity_array[index]);

/* Population is bigger now! */
  pop->size++;

/*
  printf("ENTITY %d ALLOCATED.\n", index);
*/

  return &(pop->entity_array[index]);
  }


/**********************************************************************
  ga_copy_data()
  synopsis:	Copy one chromosome's portion of the data field of an
		entity structure.  'Copies' NULL data safely.
		The destination chromosomes must be filled in order.
		If these entities are in differing populations, no
		problems will occur provided that the
		data_ref_incrementor callbacks are identical.
  parameters:
  return:
  last updated: 18/12/00
 **********************************************************************/

boolean ga_copy_data(population *pop, entity *dest, entity *src, const int chromosome)
  {
  vpointer	tmpdata;	/* Temporary pointer. */

  if (!src || (tmpdata = slink_nth_data(src->data, chromosome)) == NULL)
    {
    dest->data = slink_append(dest->data, NULL);
    }
  else
    {
    dest->data = slink_append(dest->data, tmpdata);
    pop->data_ref_incrementor(tmpdata);
    }

  return TRUE;
  }


/**********************************************************************
  ga_copy_entity_all_chromosomes()
  synopsis:	Copy genetic data between entity structures.
		If these entities are in differing populations, no
		problems will occur provided that the chromosome
		properties are identical.
  parameters:
  return:
  last updated: 20/12/00
 **********************************************************************/

boolean ga_copy_entity_all_chromosomes(population *pop, entity *dest, entity *src)
  {
  int		i;		/* Loop variable over all chromosomes. */

  /* Checks */
  if (!dest || !src) die("Null pointer to entity structure passed");

/*
 * Ensure destination structure is not likely be already in use.
 */
  if (dest->data) die("Why does this entity already contain data?");

/*
 * Copy genetic data.
 */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(dest->chromosome[i], src->chromosome[i],
           pop->len_chromosomes*sizeof(int));

    ga_copy_data(pop, dest, src, i);
    }

  return TRUE;
  }


/**********************************************************************
  ga_copy_entity_chromosome()
  synopsis:	Copy chromosome and structural data between entity
		structures.
  parameters:
  return:
  last updated: 22/01/01
 **********************************************************************/

boolean ga_copy_entity_chromosome(population *pop, entity *dest, entity *src, int chromo)
  {

/* Checks. */
  if (!dest || !src) die("Null pointer to entity structure passed");
  if (chromo<0 || chromo>=pop->num_chromosomes) die("Invalid chromosome number.");

/*
 * Ensure destination structure is not likely be already in use.
 */
  if (dest->data) die("Why does this entity already contain data?");

/*
 * Copy genetic and associated structural data.
 */
  memcpy(dest->chromosome[chromo], src->chromosome[chromo],
           pop->len_chromosomes*sizeof(int));
  ga_copy_data(pop, dest, src, chromo);

  return TRUE;
  }


/**********************************************************************
  ga_copy_entity()
  synopsis:	Er..., copy entire entity structure.
  parameters:
  return:
  last updated:	22/01/01
 **********************************************************************/

boolean ga_copy_entity(population *pop, entity *dest, entity *src)
  {

  ga_copy_entity_all_chromosomes(pop, dest, src);

  dest->fitness = src->fitness;

  return TRUE;
  }


/**********************************************************************
  Network communication (population migration) functions.
 **********************************************************************/

#if 0
/**********************************************************************
  ga_marktosend_entity()
  synopsis:	Mark an entity to be sent to another subpopulation
		(i.e. jump to another processor).
  parameters:
  return:
  last updated: 22/09/00
 **********************************************************************/

void ga_marktosend_entity(int *send_mask)
  {
  }
#endif


#if 0
/**********************************************************************
  ga_multiproc_compare_entities()
  synopsis:	Synchronise processors and if appropriate transfer
		better solution to this processor.
		local will contain the optimum solution from local
		and localnew on all processors.
  parameters:
  return:
  last updated:	18/12/00
 **********************************************************************/

entity *ga_multiproc_compare_entities( population *pop, entity *localnew, entity *local )
  {
  double	global_max;		/* Maximum value across all nodes. */
  int		maxnode;		/* Node with maximum value. */
  int		*buffer=NULL;		/* Send/recieve buffer. */
  int		*buffer_ptr=NULL;	/* Current position in end/recieve buffer. */
  int		buffer_size;		/* Size of buffer. */
  int		j;			/* Loop over chromosomes. */
  entity	*tmpentity;		/* Entity ptr for swapping. */

  maxnode = mpi_find_global_max(MAX(localnew->fitness, local->fitness), &global_max);

  buffer_size = pop->num_chromosomes*pop->len_chromosomes;
  buffer_ptr = buffer = s_malloc(buffer_size*sizeof(int));

  if (maxnode == mpi_get_rank())
    {
    if (localnew->fitness > local->fitness)
      {
      tmpentity = local;
      local = localnew;
      localnew = tmpentity;
      }

    for (j=0; j<pop->num_chromosomes; j++)
      {
      memcpy(buffer_ptr, local->chromosome[j], pop->len_chromosomes*sizeof(int));
      buffer_ptr += pop->len_chromosomes;
      }

    mpi_standard_distribute( buffer, buffer_size, MPI_TYPE_INT, maxnode, HELGA_TAG_BESTSYNC );
    }
  else
    {
    mpi_standard_distribute( buffer, buffer_size, MPI_TYPE_INT, maxnode, HELGA_TAG_BESTSYNC );

    for (j=0; j<pop->num_chromosomes; j++)
      {
      memcpy(local->chromosome[j], buffer_ptr, pop->len_chromosomes*sizeof(int));
      buffer_ptr += pop->len_chromosomes;
      }

    pop->evaluate(pop, local);
    if (local->fitness != global_max)
      dief("Best scores don't match %f %f.", local->fitness, global_max);
    }

  s_free(buffer);

  return local;
  }


/**********************************************************************
  ga_sendrecv_entities()
  synopsis:	Make entities change subpopulations based on the
		previously set mask. (i.e. entities jump to
		another processor).
		Currently only sends the genetic data and rebuilds the
		structure.
		FIXME: Send structural data too.
		(This functionality should be provided by a user
		specified callback.)
  parameters:
  return:
  last updated: 22/09/00
 **********************************************************************/

boolean ga_sendrecv_entities( population *pop, int *send_mask, int send_count )
  {
  int		i, j;			/* Loop over all chromosomes in all entities. */
  int		next, prev;		/* Processor to send/recieve entities with. */
  int		*buffer=NULL;		/* Send/recieve buffer. */
  int		*buffer_ptr=NULL;	/* Current position in end/recieve buffer. */
  int		recv_count;		/* Number of entities to recieve. */
  int		recv_size, send_size=0;	/* Size of buffer. */
  int		index=0;		/* Index of entity to send. */
  entity	*immigrant;		/* New entity. */

/* Checks */
  if (!pop) die("Null pointer to population structure passed.");
  if (!send_mask) die("Null pointer to int array.");

  next = mpi_get_next_rank();
  prev = mpi_get_prev_rank();

/* Pack chromosomes into buffer. */
  if (send_count > 0)
    {
    send_size = send_count*pop->num_chromosomes*pop->len_chromosomes;
    if ( !(buffer=s_malloc(send_size*sizeof(int))) )
      die("Unable to allocate memory.");

    buffer_ptr = buffer;

    for (i=0; i<send_count; i++)
      {
      while ( *send_mask == 0 )
        {	/* Skipping structure. */
        send_mask++;
        index++;
        }

      for (j=0; j<pop->num_chromosomes; j++)
        {
        memcpy(buffer_ptr,
               pop->entity_iarray[index]->chromosome[j],
               pop->len_chromosomes*sizeof(int));
        buffer_ptr += pop->len_chromosomes;
        }

      send_mask++;	/* Ready for next loop */
      index++;
      }
    }

/* Send data to next node. */
  plog(LOG_DEBUG, "Sending %d to node %d.", send_count, next);
  mpi_standard_send( &send_count, 1, MPI_TYPE_INT, next, HELGA_TAG_MIGRATIONINFO );

  if (send_count > 0)
    {
    plog(LOG_DEBUG, "Sending %d ints to node %d.", send_size, next);
    mpi_standard_send( buffer, send_size, MPI_TYPE_INT, next, HELGA_TAG_MIGRATIONDATA );
    }

/*
  helga_start_timer();
*/

/* Recieve data from previous node. */
  plog(LOG_DEBUG, "Recieving messages from node %d.", prev);

  mpi_receive( &recv_count, 1, MPI_TYPE_INT, prev, HELGA_TAG_MIGRATIONINFO );

  plog(LOG_DEBUG, "Will be recieving %d entities = %d ints (%Zd bytes).",
            recv_count,
            recv_count*pop->num_chromosomes*pop->len_chromosomes,
            recv_count*pop->num_chromosomes*pop->len_chromosomes*sizeof(int));

  if (recv_count > 0)
    {
    recv_size = recv_count*pop->num_chromosomes*pop->len_chromosomes;
    if ( !(buffer=s_realloc(buffer, recv_size*sizeof(int))) )
      die("Unable to reallocate memory.");

    buffer_ptr = buffer;

    mpi_receive( buffer, recv_size, MPI_TYPE_INT, prev, HELGA_TAG_MIGRATIONDATA );

    for (i=0; i<recv_count; i++)
      {
      immigrant = ga_get_free_entity(pop);
      for (j=0; j<pop->num_chromosomes; j++)
        {
        memcpy(buffer_ptr,
               immigrant->chromosome[j],
               pop->len_chromosomes*sizeof(int));
        buffer_ptr += pop->len_chromosomes;
        }
      pop->evaluate(pop, immigrant);

/*
      plog(LOG_VERBOSE, "Immigrant has fitness %f", immigrant->fitness);
*/
      }
    }

/* How much time did we waste? */
/*
  helga_check_timer();
*/

  if (buffer) s_free(buffer);

  return TRUE;
  }
#endif


#if 0
/**********************************************************************
  ga_recv_entities()
  synopsis:	Handle immigration.
  parameters:
  return:
  last updated: 08/08/00
 **********************************************************************/

boolean ga_recv_entities(entity *migrant)
  {
  int		i;		/* Loop over all chromosomes */

  /* Checks */
  if (!migrant) die("Null pointer to entity structure passed");

  /* Ensure destination structure is ready */
  ga_entity_setup(migrant);

  for (i=0; i<pop->num_chromosomes; i++)
    {
    }

  return TRUE;
  }
#endif


/**********************************************************************
  Environmental operator function.
 **********************************************************************/

/**********************************************************************
  ga_optimise_entity()
  synopsis:	Optimise the entity's structure through systematic
		searching in the gene space.
		Should be default choice for "adaptation" function.
  parameters:
  return:
  last updated: 01/08/00
 **********************************************************************/

entity *ga_optimise_entity(population *pop, entity *unopt)
  {
  entity	*optimised;

  /* Checks */
  if (!unopt) die("Null pointer to entity structure passed.");

  plog(LOG_FIXME,
            "Code incomplete, using 20 iterations of the RMHC algorithm for now.");

/* FIXME: hard-coded values. */
  optimised = ga_random_mutation_hill_climbing( pop, unopt, 20 );

  plog(LOG_DEBUG, "Entity optimised from %f to %f.",
            unopt->fitness, optimised->fitness);

  return optimised;
  }


/**********************************************************************
  GA functions.
 **********************************************************************/

/**********************************************************************
  ga_genesis()
  synopsis:	High-level function to create a new population and
		perform the basic setup (i.e. initial seeding) required
		for further optimisation and manipulation.
  parameters:
  return:	population, or NULL on failure.
  last updated:	23/04/01
 **********************************************************************/

population *ga_genesis(	const int		population_size,
			const int		num_chromo,
			const int		len_chromo,
			const char		*fname,
			GAgeneration_hook	generation_hook,
			GAiteration_hook	iteration_hook,
			GAdata_destructor	data_destructor,
			GAdata_ref_incrementor	data_ref_incrementor,
			GAevaluate		evaluate,
			GAseed			seed,
			GAadapt			adapt,
			GAselect_one		select_one,
			GAselect_two		select_two,
			GAmutate		mutate,
			GAcrossover		crossover,
			GAreplace		replace )
  {
  population	*pop;	/* The new population structure. */

  plog(LOG_VERBOSE, "Genesis is beginning!");
  plog(LOG_FIXME, "There are hard coded values in ga_genesis().");

/*
 * Allocate and initialise a new population.
 * This call also sets this as the active population.
 *
 * FIXME:
 * The hard-coded value below "4(N+2)" should be determined based on the
 * actual mutation and crossover rates to be used.
 */
  if ( !(pop = ga_population_new( 4*(population_size+2),
                              population_size,
                              num_chromo,
                              len_chromo )) ) return NULL;

/*
 * Define some callback functions.
 */
  pop->generation_hook = generation_hook;
  pop->iteration_hook = iteration_hook;

  pop->data_destructor = data_destructor;
  pop->data_ref_incrementor = data_ref_incrementor;

  pop->evaluate = evaluate;
  pop->seed = seed;
  pop->adapt = adapt;
  pop->select_one = select_one;
  pop->select_two = select_two;
  pop->mutate = mutate;
  pop->crossover = crossover;
  pop->replace = replace;

/*
 * Seed the population.
 */
  if (!seed)
    {
    plog(LOG_VERBOSE, "Entity seed function not defined.  Genesis can not occur.  Continuing anyway.");
    }
  else
    {
    ga_population_seed(pop);
    plog(LOG_VERBOSE, "Genesis has occured!");
    }

  return pop;
  }


/**********************************************************************
  ga_population_set_parameters()
  synopsis:	Sets the GA parameters for a population.
  parameters:
  return:
  last updated:	23/04/01
 **********************************************************************/

void ga_population_set_parameters(	population	*pop,
					double	crossover,
					double	mutation,
					double	migration)
  {

  if ( !pop ) die("Null pointer to population structure passed.");

  plog( LOG_VERBOSE,
        "The population's GA parameters have been set. cro. %f mut. %f mig. %f",
        crossover, mutation, migration );

  pop->crossover_ratio = crossover;
  pop->mutation_ratio = mutation;
  pop->migration_ratio = migration;

  return;
  }


/**********************************************************************
  ga_transcend()
  synopsis:	Return a population structure to user for analysis or
		whatever.  But remove it from the population table.
		(Like ga_extinction, except doesn't purge memory.)
  parameters:
  return:
  last updated:	19/01/01
 **********************************************************************/

population *ga_transcend(unsigned int id)
  {
  plog(LOG_VERBOSE, "This population has achieved transcendance!");

  return (population *) table_remove_index(pop_table, id);
  }


/**********************************************************************
  ga_resurect()
  synopsis:	Restores a population structure into the population
		table from an external source.
  parameters:
  return:
  last updated:	19/01/01
 **********************************************************************/

unsigned int ga_resurect(population *pop)
  {
  plog(LOG_VERBOSE, "The population has been restored!");

  return table_add(pop_table, pop);
  }


/**********************************************************************
  ga_extinction()
  synopsis:	Purge all memory used by a population.
  parameters:
  return:
  last updated:	12/07/00
 **********************************************************************/

boolean ga_extinction(population *extinct)
  {
  int		i;		/* Loop over population members (existing or not). */
  int		id;		/* Internal index for this extinct population. */

  plog(LOG_VERBOSE, "This population is becoming extinct!");

  if (!extinct) die("Null pointer to population structure passed.");

/*
 * Remove this population from the population table.
 */
  THREAD_LOCK(pop_table);
  id = table_remove_data(pop_table, extinct);
  THREAD_UNLOCK(pop_table);

/*
 * Error check.
 */
  if (id == table_error_index(pop_table))
    die("Unable to find population structure in table.");

/*
 * Any user-data?
 */
  if (extinct->data)
    plog(LOG_WARNING, "User data field is not empty. (Potential memory leak)");

/*
 * Dereference/free everyting.
 */
  if (!ga_genocide(extinct, 0))
    {
    plog(LOG_NORMAL, "This population is already extinct!");
    }
  else
    {
    for (i=0; i<extinct->max_size; i++)
      {
      if (extinct->entity_array[i].chromosome)
        {
        s_free(extinct->entity_array[i].chromosome[0]);
        s_free(extinct->entity_array[i].chromosome);
        }
      }

    s_free(extinct->entity_array);
    s_free(extinct->entity_iarray);
    s_free(extinct);
    }

  return TRUE;
  }


/**********************************************************************
  ga_genocide()
  synopsis:	Kill population members.
  parameters:
  return:
  last updated:	11/01/01
 **********************************************************************/

boolean ga_genocide(population *pop, int target_size)
  {
  if (!pop) return FALSE;

  plog(LOG_VERBOSE,
            "The population is being culled from %d to %d individuals!",
            pop->size, target_size);

/*
 * Dereference the structures relating to the least
 * fit population members.  (I assume the population has been
 * sorted by fitness.)
 */
  while (pop->size>target_size)
    {
    ga_entity_dereference_by_rank(pop, pop->size-1);
    }

  return TRUE;
  }


/**********************************************************************
  ga_diagnostics()
  synopsis:	Diagnostics.
  parameters:
  return:	none
  last updated:	02/02/01
 **********************************************************************/

void ga_diagnostics(void)
  {

  printf("=== GA utility library =======================================\n");
  printf("Build date:                %s\n", BUILD_DATE_STRING);
  printf("GA_DEBUG:                  %d\n", GA_DEBUG);
  printf("GA_BOLTZMANN_FACTOR:       %f\n", GA_BOLTZMANN_FACTOR);
  printf("GA_UTIL_MIN_FITNESS:       %f\n", GA_UTIL_MIN_FITNESS);
  printf("GA_MULTI_BIT_CHANCE:       %f\n", GA_MULTI_BIT_CHANCE);
  printf("GA_ELITISM_MULTIPLIER:     %f\n", GA_ELITISM_MULTIPLIER);
  printf("GA_ELITISM_CONSTANT:       %f\n", GA_ELITISM_CONSTANT);
  printf("--------------------------------------------------------------\n");
  printf("structure                  sizeof\n");
  printf("population                 %Zd\n", sizeof(population));
  printf("entity                     %Zd\n", sizeof(entity));
  printf("--------------------------------------------------------------\n");
  if (pop_table)
    {
    printf("Population table:          defined\n");
    printf("Size:                      %ud\n", table_count_items(pop_table));
    }
  else
    {
    printf("Population table:          undefined\n");
    }
  printf("--------------------------------------------------------------\n");

  return;
  }


/**********************************************************************
  ga_allele_search()
  synopsis:	Perform complete systematic search on a given allele
		in a given entity.  If initial solution is NULL, then
		a random solution is generated (but use of that feature
		is unlikely to be useful!).
		The original entity will not be munged.
                NOTE: max_val is the maximum value _+_ 1!
  parameters:
  return:	Best solution found.
  last updated:	24/03/01
 **********************************************************************/

entity *ga_allele_search(	population	*pop,
				const int	chromosomeid,
				const int	point,
				const int 	min_val,
				const int 	max_val,
				entity		*initial )
  {
  int		val;			/* Current value for point. */
  entity	*current, *best;	/* The solutions. */

/* Checks. */
/* FIXME: More checks needed. */
  if ( !pop ) die("Null pointer to population structure passed.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "Systematic allele search algorithm is not parallelised.");

/* Do we need to generate a random solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform systematic allele search with random starting solution.");

    pop->seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform systematic allele search.");

    ga_copy_entity(pop, best, initial);
    }

/*
 * Copy best solution over current solution.
 */
  ga_copy_entity(pop, current, best);
  best->fitness=GA_UTIL_MIN_FITNESS;

/*
 * Loop over the range of legal values.
 */
  for (val = min_val; val < max_val; val++)
    {
    current->chromosome[chromosomeid][point] = val;
    ga_entity_clear_data(pop, current, chromosomeid);

    pop->evaluate(pop, current);

/*
 * Should we keep this solution?
 */
    if ( best->fitness < current->fitness )
      { /* Copy this solution best solution. */
      ga_entity_blank(pop, best);
      ga_copy_entity(pop, best, current);
      }
    else
      { /* Copy best solution over current solution. */
      ga_entity_blank(pop, current);
      ga_copy_entity(pop, current, best);
      }

    }

  plog(LOG_VERBOSE,
            "After complete search the best solution has fitness score of %f",
            best->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);

  return best;
  }


