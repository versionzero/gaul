/**********************************************************************
  ga_optim.c
 **********************************************************************

  ga_optim - Optimisation and evolution routines.
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

  Synopsis:     Routines for optimisation and evolution.

                Note that the temperatures in the simulated annealling
                and MC functions do not exactly run from the initial
                temperature to the final temperature.  They are offset
                slightly so that sequential calls to these functions
                will have a linear temperature change.  The SA and MC
		code in this file is deprecated anyway - these routines
		have been replaced with much more flexible alternatives
		and will be removed in the near future.

  To do:	Finish rewriting parallel versions, ga_evolution_mp() in particular.
		Write ga_evolution_pvm().
		Need to fix elitism/crowding stuff.
		Remove much duplicated code.
		OpenMOSIX fix.  See below.

 **********************************************************************/

#include "gaul/ga_optim.h"

/*
 * Here is a kludge.
 *
 * This constant, if defined, causes a 10 microsecond delay to be
 * inserted after each fork() call.  It shouldn't be needed, but
 * apparently on OpenMOSIX lots of processes started at the same
 * time cause all sorts of problems (mostly bus errors).  This
 * delay gives OpenMOSIX a chance to migrate some processes to
 * other nodes before this becomes a problem (hopefully).
 *
 * A long-term fix fix will be to check the return value from the
 * forked processes and repeat them if they died.  This may be
 * added... eventually.
 *
 * I don't think this is needed anymore.
 */
#define NEED_MOSIX_FORK_HACK 1

/**********************************************************************
  gaul_entity_swap_rank()
  synopsis:	Swap the ranks of a pair of entities.
  parameters:	population *pop
		const int rank1
		const int rank2
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_entity_swap_rank(population *pop, const int rank1, const int rank2)
  {
  entity	*tmp;		/* Swapped entity. */

  tmp = pop->entity_iarray[rank1];
  pop->entity_iarray[rank1] = pop->entity_iarray[rank2];
  pop->entity_iarray[rank2] = tmp;

  return;
  }


/**********************************************************************
  gaul_migration()
  synopsis:	Migration cycle.
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_migration(const int num_pops, population **pops)
  {
  int		pop0_osize;		/* Required for correct migration. */
  int		island;			/* Current island number. */
  int		i;			/* Loop over members of population. */

  plog( LOG_VERBOSE, "*** Migration Cycle ***" );

  pop0_osize = pops[0]->size;
  for(island=1; island<num_pops; island++)
    {
    for(i=0; i<pops[island]->size; i++)
      {
      if (random_boolean_prob(pops[island]->migration_ratio))
        {
        ga_entity_clone(pops[island-1], pops[island]->entity_iarray[i]);
/* printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), island, i, pops[island]->entity_iarray[i]->fitness);*/
        }
      }
    }

  for(i=0; i<pop0_osize; i++)
    {
    if (random_boolean_prob(pops[0]->migration_ratio))
      ga_entity_clone(pops[num_pops-1], pops[0]->entity_iarray[i]);
/*  printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[island]->entity_iarray[i]->fitness);*/
    }

/*
 * Sort the individuals in each population.
 * Need this to ensure that new immigrants are ranked correctly.
 * FIXME: It would be more efficient to insert the immigrants correctly.
 */
  for(island=0; island<num_pops; island++)
    {
    sort_population(pops[island]);
    }

  return;
  }


/**********************************************************************
  gaul_crossover()
  synopsis:	Mating cycle. (i.e. Sexual reproduction).
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_crossover(population *pop)
  {
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter;	/* Child entities. */

  plog(LOG_VERBOSE, "*** Mating cycle ***");

  if (pop->crossover_ratio <= 0.0) return;

  pop->select_state = 0;

  /* Select pairs of entities to mate via crossover. */
  while ( !(pop->select_two(pop, &mother, &father)) )
    {

    if (mother && father)
      {
      plog(LOG_VERBOSE, "Crossover between %d (rank %d fitness %f) and %d (rank %d fitness %f)",
           ga_get_entity_id(pop, mother),
           ga_get_entity_rank(pop, mother), mother->fitness,
           ga_get_entity_id(pop, father),
           ga_get_entity_rank(pop, father), father->fitness);

      son = ga_get_free_entity(pop);
      daughter = ga_get_free_entity(pop);
      pop->crossover(pop, mother, father, daughter, son);
      }
    else
      {
      plog( LOG_VERBOSE, "Crossover not performed." );
      }
    }

  return;
  }


/**********************************************************************
  gaul_mutation()
  synopsis:	Mutation cycle.  (i.e. Asexual reproduction)
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_mutation(population *pop)
  {
  entity	*mother;		/* Parent entities. */
  entity	*daughter;		/* Child entities. */

  plog(LOG_VERBOSE, "*** Mutation cycle ***");

  if (pop->mutation_ratio <= 0.0) return;

  pop->select_state = 0;

  /*
   * Select entities to undergo asexual reproduction, in each case the child will
   * have a genetic mutation of some type.
   */
  while ( !(pop->select_one(pop, &mother)) )
    {

    if (mother)
      {
      plog(LOG_VERBOSE, "Mutation of %d (rank %d fitness %f)",
           ga_get_entity_id(pop, mother),
           ga_get_entity_rank(pop, mother), mother->fitness );

      daughter = ga_get_free_entity(pop);
      pop->mutate(pop, mother, daughter);
      }
    else
      {
      plog( LOG_VERBOSE, "Mutation not performed." );
      }
    }

  return;
  }


/**********************************************************************
  gaul_evaluation_slave_mp()
  synopsis:	Fitness evaluations and adaptations are performed here.
  parameters:	population *pop
  return:	none
  last updated:	03 Feb 2003
 **********************************************************************/

static void gaul_evaluation_slave_mp(population *pop)
  {
#if HAVE_MPI == 1
  int		i;			/* Loop variable over entity ranks. */
  MPI_Status	status;			/* MPI status structure. */
  int		int_single;		/* Receive buffer. */
  byte		*buffer;		/* Receive buffer. */
  boolean	finished=FALSE;		/* Whether this slave is done. */
  entity	*entity, *adult;	/* Received entity, adapted entity. */
  int		len=0;			/* Length of buffer to receive. */

/*
 * Allocate receive buffer.
 * FIXME: This length data shouldn't be needed!
 */
  MPI_Recv(&len, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  buffer = s_malloc(len*sizeof(byte));

/*
 * Instruction packet.
 */
  do
    {
    MPI_Recv(&int_single, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch (int_single)
      {
      case 0:
        /* Evaluation required. */
        entity = ga_get_free_entity(pop);
        MPI_Recv(&buffer, len, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pop->chromosome_from_bytes(pop, entity, buffer);
        pop->evaluate(pop, entity);
        MPI_Send(&(entity->fitness), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        break;
      case 1:
        /* Baldwinian adaptation required. */
        entity = ga_get_free_entity(pop);
        MPI_Recv(&buffer, len, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pop->chromosome_from_bytes(pop, entity, buffer);
        adult = pop->adapt(pop, entity);
        MPI_Send(&(adult->fitness), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        break;
      case 2:
        /* Lamarkian adaptation required. */
        entity = ga_get_free_entity(pop);
        MPI_Recv(&buffer, len, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pop->chromosome_from_bytes(pop, entity, buffer);
        adult = pop->adapt(pop, entity);
        MPI_Send(&(adult->fitness), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        pop->chromosome_to_bytes(pop, adult, &buffer, &len);
        MPI_Send(&buffer, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        break;
      case 3:
        /* No more jobs. */
        finished=TRUE;
        break;
      default:
        die("Unknown instruction packet recieved");
      }

    } while (finished==FALSE);

/*
 * Synchronise population on this process with that on the master process.
 */
  ga_genocide(pop,0);
  ga_population_append_receive(pop, 0);

  s_free(buffer);

#else
  die("Parallel routine called in code compiled without MPI support.");
#endif

  return;
  }


/**********************************************************************
  gaul_ensure_evaluations()
  synopsis:	Fitness evaluations.
		Evaluate all previously unevaluated entities.
		No adaptation.
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_ensure_evaluations(population *pop)
  {
  int		i;			/* Loop variable over entity ranks. */

  for (i=0; i<pop->size; i++)
    {
    if (pop->entity_iarray[i]->fitness == GA_MIN_FITNESS)
      pop->evaluate(pop, pop->entity_iarray[i]);
    }

  return;
  }


/**********************************************************************
  gaul_ensure_evaluations_mp()
  synopsis:	Fitness evaluations.
		Evaluate all previously unevaluated entities.
		No adaptation.
  parameters:	population *pop
  return:	none
  last updated:	03 Feb 2003
 **********************************************************************/

static void gaul_ensure_evaluations_mp(population *pop)
  {
#if HAVE_MPI == 1
  int		i;			/* Loop variable over entity ranks. */

  plog(LOG_FIXME, "Need to parallelise this!");

  for (i=0; i<pop->size; i++)
    {
    if (pop->entity_iarray[i]->fitness == GA_MIN_FITNESS)
      pop->evaluate(pop, pop->entity_iarray[i]);
    }

#else
  die("Parallel routine called in code compiled without MPI support.");
#endif

  return;
  }


/**********************************************************************
  gaul_ensure_evaluations_forked()
  synopsis:	Fitness evaluations.
		Evaluate all previously unevaluated entities.
		No adaptation.
  parameters:	population *pop
  return:	none
  last updated:	30 Jun 2002
 **********************************************************************/

#if W32_CRIPPLED != 1
static void gaul_ensure_evaluations_forked(population *pop, const int num_processes,
			int *eid, pid_t *pid, const int *evalpipe)
  {
  int		fork_num;		/* Index of current forked process. */
  int		num_forks;		/* Number of forked processes. */
  int		eval_num;		/* Index of current entity. */
  pid_t		fpid;			/* PID of completed child process. */

/*
 * A forked process is started for each fitness evaluation upto
 * a maximum of max_processes at which point we wait for
 * results before forking more.
 *
 * Skip evaluations for entities that have been previously evaluated.
 */
  fork_num = 0;
  eval_num = 0;

  /* Fork initial processes. */
  /* Skip to the next entity which needs evaluating. */
  while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;

  while (fork_num < num_processes && eval_num < pop->size)
    {
    eid[fork_num] = eval_num;
    pid[fork_num] = fork();

    if (pid[fork_num] < 0)
      {       /* Error in fork. */
      dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
      }
    else if (pid[fork_num] == 0)
      {       /* This is the child process. */
      pop->evaluate(pop, pop->entity_iarray[eval_num]);

      write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

      fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
      _exit(1);
      }

    fork_num++;
    eval_num++;

    /* Skip to the next entity which needs evaluating. */
    while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;
#ifdef NEED_MOSIX_FORK_HACK
    usleep(10);
#endif
    }
  num_forks = fork_num;

  /* Wait for a forked process to finish and, if needed, fork another. */
  while (num_forks > 0)
    {
    fpid = wait(NULL);

    if (fpid == -1) die("Error in wait().");

    /* Find which entity this forked process was evaluating. */
    fork_num = 0;
    while (fpid != pid[fork_num]) fork_num++;

    if (eid[fork_num] == -1) die("Internal error.  eid is -1");

    read(evalpipe[2*fork_num], &(pop->entity_iarray[eid[fork_num]]->fitness), sizeof(double));

    if (eval_num < pop->size)
      {       /* New fork. */
      eid[fork_num] = eval_num;
      pid[fork_num] = fork();

      if (pid[fork_num] < 0)
        {       /* Error in fork. */
        dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
        }
      else if (pid[fork_num] == 0)
        {       /* This is the child process. */
        pop->evaluate(pop, pop->entity_iarray[eval_num]);

        write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

        fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
        _exit(1);
        }

      eval_num++;

      /* Skip to the next entity which needs evaluating. */
      while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;
      }
    else
      {
      pid[fork_num] = -1;
      eid[fork_num] = -1;
      num_forks--;
      }
    }

  return;
  }
#endif


/**********************************************************************
  gaul_ensure_evaluations_threaded()
  synopsis:	Fitness evaluations.
		Evaluate all previously unevaluated entities.
		No adaptation.
  parameters:	population *pop
  return:	none
  last updated:	18 Sep 2002
 **********************************************************************/

#if HAVE_PTHREAD == 1
static void gaul_ensure_evaluations_threaded(population *pop, const int num_threads,
			int *eid, pthread_t *tid)
  {
  int		thread_num;		/* Index of current thread. */
  int		num_threads;		/* Number of threads. */
  int		eval_num;		/* Index of current entity. */
  pthread_t	fpid;			/* TID of completed child process. */

/*
 * A thread is created for each fitness evaluation upto
 * a maximum of max_threads at which point we wait for
 * results before continuing.
 *
 * Skip evaluations for entities that have been previously evaluated.
 */
  thread_num = 0;
  eval_num = 0;

  /* Fork initial processes. */
  /* Skip to the next entity which needs evaluating. */
  while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;

  while (thread_num < num_processes && eval_num < pop->size)
    {
    eid[thread_num] = eval_num;
    pid[thread_num] = XXXX();

    if (pid[thread_num] < 0)
      {       /* Error in fork. */
      dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
      }
    else if (pid[thread_num] == 0)
      {       /* This is the child process. */
      pop->evaluate(pop, pop->entity_iarray[eval_num]);

      write(evalpipe[2*thread_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

      fsync(evalpipe[2*thread_num+1]);	/* Ensure data is written to pipe. */
      _exit(1);
      }

    thread_num++;
    eval_num++;

    /* Skip to the next entity which needs evaluating. */
    while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;
    }
  num_threads = thread_num;

  /* Wait for a thread to finish and, if needed, create another. */
  while (num_threads > 0)
    {
    fpid = wait(NULL);

    if (fpid == -1) die("Error in wait().");

    /* Find which entity this thread was evaluating. */
    thread_num = 0;
    while (fpid != pid[thread_num]) thread_num++;

    if (eid[thread_num] == -1) die("Internal error.  eid is -1");

    read(evalpipe[2*thread_num], &(pop->entity_iarray[eid[thread_num]]->fitness), sizeof(double));

    if (eval_num < pop->size)
      {       /* New thread. */
      eid[thread_num] = eval_num;
      pid[thread_num] = fork();

      if (pid[thread_num] < 0)
        {       /* Error in thread. */
        dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
        }
      else if (pid[thread_num] == 0)
        {       /* This is the child process. */
        pop->evaluate(pop, pop->entity_iarray[eval_num]);

        write(evalpipe[2*thread_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

        fsync(evalpipe[2*thread_num+1]);	/* Ensure data is written to pipe. */
        _exit(1);
        }

      eval_num++;

      /* Skip to the next entity which needs evaluating. */
      while (eval_num < pop->size && pop->entity_iarray[eval_num]->fitness!=GA_MIN_FITNESS) eval_num++;
      }
    else
      {
      pid[thread_num] = -1;
      eid[thread_num] = -1;
      num_threads--;
      }
    }

  return;
  }
#endif /* HAVE_PTHREAD */


/**********************************************************************
  gaul_adapt_and_evaluate()
  synopsis:	Fitness evaluations.
		Evaluate the new entities produced in the current
		generation, whilst performing any necessary adaptation.
		Simple sequential version.
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

static void gaul_adapt_and_evaluate(population *pop)
  {
  int		i;			/* Loop variable over entity ranks. */
  entity	*adult=NULL;		/* Adapted entity. */
  int		adultrank;		/* Rank of adapted entity. */

  if (pop->scheme == GA_SCHEME_DARWIN)
    {	/* This is pure Darwinian evolution.  Simply assess fitness of all children.  */

    plog(LOG_VERBOSE, "*** Fitness Evaluations ***");

    for (i=pop->orig_size; i<pop->size; i++)
      {
      pop->evaluate(pop, pop->entity_iarray[i]);
      }

    return;
    }
  else
    {	/* Some kind of adaptation is required.  First reevaluate parents, as needed, then children. */

    plog(LOG_VERBOSE, "*** Adaptation and Fitness Evaluations ***");

    if ( (pop->scheme & GA_SCHEME_BALDWIN_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }

    if ( (pop->scheme & GA_SCHEME_BALDWIN_CHILDREN)!=0 )
      { 
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_CHILDREN)!=0 )
      {
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }
    }

  return;
  }


/**********************************************************************
  gaul_adapt_and_evaluate_mp()
  synopsis:	Fitness evaluations.
		Evaluate the new entities produced in the current
		generation, whilst performing any necessary adaptation.
		MPI version.
  parameters:	population *pop
  return:	none
  last updated:	03 Feb 2003
 **********************************************************************/

static void gaul_adapt_and_evaluate_mp(population *pop)
  {
#if HAVE_MPI == 1
  int		i;			/* Loop variable over entity ranks. */
  entity	*adult=NULL;		/* Adapted entity. */
  int		adultrank;		/* Rank of adapted entity. */

  plog(LOG_FIXME, "Need to parallelise this!");

  if (pop->scheme == GA_SCHEME_DARWIN)
    {	/* This is pure Darwinian evolution.  Simply assess fitness of all children.  */

    plog(LOG_VERBOSE, "*** Fitness Evaluations ***");

    for (i=pop->orig_size; i<pop->size; i++)
      {
      pop->evaluate(pop, pop->entity_iarray[i]);
      }

    return;
    }
  else
    {	/* Some kind of adaptation is required.  First reevaluate parents, as needed, then children. */

    plog(LOG_VERBOSE, "*** Adaptation and Fitness Evaluations ***");

    if ( (pop->scheme & GA_SCHEME_BALDWIN_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }

    if ( (pop->scheme & GA_SCHEME_BALDWIN_CHILDREN)!=0 )
      { 
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_CHILDREN)!=0 )
      {
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }
    }

#else
  die("Parallel routine called in code compiled without MPI support.");
#endif

  return;
  }


/**********************************************************************
  gaul_adapt_and_evaluate_forked()
  synopsis:	Fitness evaluations.
		Evaluate the new entities produced in the current
		generation, whilst performing any necessary adaptation.
		Forked processing version.
  parameters:	population *pop
  return:	none
  last updated:	11 Jun 2002
 **********************************************************************/

#if W32_CRIPPLED != 1
static void gaul_adapt_and_evaluate_forked(population *pop,
	       		const int num_processes,
			int *eid, pid_t *pid, const int *evalpipe)
  {
  int		i;			/* Loop variable over entity ranks. */
  entity	*adult=NULL;		/* Adapted entity. */
  int		adultrank;		/* Rank of adapted entity. */
  int		fork_num;		/* Index of current forked process. */
  int		num_forks;		/* Number of forked processes. */
  int		eval_num;		/* Index of current entity. */
  pid_t		fpid;			/* PID of completed child process. */

  if (pop->scheme == GA_SCHEME_DARWIN)
    {	/* This is pure Darwinian evolution.  Simply assess fitness of all children.  */

    plog(LOG_VERBOSE, "*** Fitness Evaluations ***");

/* 
 * A forked process is started for each fitness evaluation upto
 * a maximum of max_processes at which point we wait for 
 * results before forking more.
 *
 * FIXME: This lump of code is almost identical to that in
 * gaul_ensure_evaluations_forked() and shouldn't really be duplicated.
 */
    fork_num = 0;
    eval_num = pop->orig_size;

    /* Fork initial processes. */
    while (fork_num < num_processes && eval_num < pop->size)
      {
      eid[fork_num] = eval_num;
      pid[fork_num] = fork();

      if (pid[fork_num] < 0)
        {	/* Error in fork. */
        dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
        }
      else if (pid[fork_num] == 0)
        {	/* This is the child process. */
        pop->evaluate(pop, pop->entity_iarray[eval_num]);

        write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

        fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
        _exit(1);
        }
      fork_num++;
      eval_num++;
#ifdef NEED_MOSIX_FORK_HACK
      usleep(10);
#endif
      }
    num_forks = fork_num;

    /* Wait for a forked process to finish and, if needed, fork another. */
    while (num_forks > 0)
      {
      fpid = wait(NULL);

      if (fpid == -1) die("Error in wait().");

      /* Find which entity this forked process was evaluating. */
      fork_num = 0;
      while (fpid != pid[fork_num]) fork_num++;

      if (eid[fork_num] == -1) die("Internal error.  eid is -1");

      read(evalpipe[2*fork_num], &(pop->entity_iarray[eid[fork_num]]->fitness), sizeof(double));

      if (eval_num < pop->size)
        {	/* New fork. */
        eid[fork_num] = eval_num;
        pid[fork_num] = fork();

        if (pid[fork_num] < 0)
          {       /* Error in fork. */
          dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
          }
        else if (pid[fork_num] == 0)
          {       /* This is the child process. */
          pop->evaluate(pop, pop->entity_iarray[eval_num]);

          write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

          fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
          _exit(1);
          }

        eval_num++;
        }
      else
        {
        pid[fork_num] = -1;
        eid[fork_num] = -1;
        num_forks--;
        }
      }

    return;
    }
  else
    {	/* Some kind of adaptation is required.  First reevaluate parents, as needed, then children. */

    plog(LOG_VERBOSE, "*** Adaptation and Fitness Evaluations ***");

    if ( (pop->scheme & GA_SCHEME_BALDWIN_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_PARENTS)!=0 )
      {
      for (i=0; i<pop->orig_size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }

    if ( (pop->scheme & GA_SCHEME_BALDWIN_CHILDREN)!=0 )
      { 
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        pop->entity_iarray[i]->fitness=adult->fitness;
        ga_entity_dereference(pop, adult);
        }
      }
    else if ( (pop->scheme & GA_SCHEME_LAMARCK_CHILDREN)!=0 )
      {
      for (i=pop->orig_size; i<pop->size; i++)
        {
        adult = pop->adapt(pop, pop->entity_iarray[i]);
        adultrank = ga_get_entity_rank(pop, adult);
        gaul_entity_swap_rank(pop, i, adultrank);
        ga_entity_dereference_by_rank(pop, adultrank);
        }
      }
    }

  return;
  }
#endif


/**********************************************************************
  gaul_survival()
  synopsis:	Survival of the fittest.
		Enforce elitism, apply crowding operator, reduce
		population back to its stable size and rerank entities,
		as required.

		*** FIXME: crowding analysis incomplete. ***

  parameters:	population *pop
  return:	none
  last updated:	18 Mar 2003
 **********************************************************************/

static void gaul_survival(population *pop)
  {
  int		i;			/* Loop variable over entity ranks. */

  plog(LOG_VERBOSE, "*** Survival of the fittest ***");

/*
 * Need to kill parents, or rescore parents?
 */
  if (pop->elitism == GA_ELITISM_PARENTS_DIE || pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES)
    {
    while (pop->orig_size>(pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES))
      {
      pop->orig_size--;
      ga_entity_dereference_by_rank(pop, pop->orig_size);
      }
    }
  else if (pop->elitism == GA_ELITISM_RESCORE_PARENTS)
    {
    plog(LOG_VERBOSE, "*** Fitness Re-evaluations ***");

    for (i=pop->orig_size; i<pop->size; i++)
      {
      pop->evaluate(pop, pop->entity_iarray[i]);
      }
    }

/*
 * Sort all population members by fitness.
 */
  sort_population(pop);

/*
 * Enforce the type of crowding desired.
 *
 * Rough crowding doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 */
#if 0
    if (pop->elitism == GA_ELITISM_EXACT || pop->elitism == GA_ELITISM_ROUGH)
      { /* Fatal version */
      i = 1;

      while (i<pop->size && i<pop->stable_size)
        {
        if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
            (pop->elitism != GA_ELITISM_EXACT ||
             ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
          {
          ga_entity_dereference_by_rank(pop, i);
          }
        else
          {
          i++;
          }
        }
      }
    else if (pop->elitism == GA_ELITISM_EXACT_COMP || pop->elitism == GA_ELITISM_ROUGH_COMP)
      { /* Increased competition version */
      i = MIN(pop->size, pop->stable_size);
      elitism_penalty = fabs(pop->entity_iarray[0]->fitness*GA_ELITISM_MULTIPLIER)
                        + GA_ELITISM_CONSTANT;

      while (i>0)
        {
        if (pop->entity_iarray[i]->fitness==pop->entity_iarray[i-1]->fitness &&
            (pop->elitism != GA_ELITISM_EXACT_COMP ||
             ga_compare_genome(pop, pop->entity_iarray[i], pop->entity_iarray[i-1])) )
          {
          pop->entity_iarray[i]->fitness -= elitism_penalty;
          }
        i--;
        }

      plog(LOG_VERBOSE, "*** Sorting again ***");

      sort_population(pop);     /* FIXME: We could possibly (certianly) choose
                                         a more optimal sort algorithm here. */
      }
#endif

/*
 * Least fit population members die to restore the
 * population size to its stable size.
 */
  ga_genocide(pop, pop->stable_size);

  return;
  }


/**********************************************************************
  gaul_survival_mp()
  synopsis:	Survival of the fittest.
		Enforce elitism, apply crowding operator, reduce
		population back to its stable size and rerank entities,
		as required.

		*** FIXME: crowding analysis incomplete. ***

  parameters:	population *pop
  return:	none
  last updated:	18 Mar 2003
 **********************************************************************/

static void gaul_survival_mp(population *pop)
  {
#if HAVE_MPI == 1
  int		i;			/* Loop variable over entity ranks. */

  plog(LOG_FIXME, "Need to parallelise this!");

  plog(LOG_VERBOSE, "*** Survival of the fittest ***");

/*
 * Need to kill parents, or rescore parents?
 */
  if (pop->elitism == GA_ELITISM_PARENTS_DIE || pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES)
    {
    while (pop->orig_size>(pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES))
      {
      pop->orig_size--;
      ga_entity_dereference_by_rank(pop, pop->orig_size);
      }
    }
  else if (pop->elitism == GA_ELITISM_RESCORE_PARENTS)
    {
    plog(LOG_VERBOSE, "*** Fitness Re-evaluations ***");

    for (i=pop->orig_size; i<pop->size; i++)
      {
      pop->evaluate(pop, pop->entity_iarray[i]);
      }
    }

/*
 * Sort all population members by fitness.
 */
  sort_population(pop);

/*
 * Enforce the type of crowding desired.
 *
 * Rough crowding doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 *
 * FIXME: Crowding code missing!!!
 */

/*
 * Least fit population members die to restore the
 * population size to its stable size.
 */
  ga_genocide(pop, pop->stable_size);

#else
  die("Parallel routine called in code compiled without MPI support.");
#endif

  return;
  }


/**********************************************************************
  gaul_survival_forked()
  synopsis:	Survival of the fittest.
		Enforce elitism, apply crowding operator, reduce
		population back to its stable size and rerank entities,
		as required.

		*** FIXME: crowding analysis incomplete. ***

  parameters:	population *pop
  return:	none
  last updated:	18 Mar 2003
 **********************************************************************/

#if W32_CRIPPLED != 1
static void gaul_survival_forked(population *pop,
			const int num_processes,
			int *eid, pid_t *pid, const int *evalpipe)
  {
  int		fork_num;		/* Index of current forked process. */
  int		num_forks;		/* Number of forked processes. */
  int		eval_num;		/* Index of current entity. */
  pid_t		fpid;			/* PID of completed child process. */

  plog(LOG_VERBOSE, "*** Survival of the fittest ***");

/*
 * Need to kill parents, or rescore parents?
 */
  if (pop->elitism == GA_ELITISM_PARENTS_DIE || pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES)
    {
    while (pop->orig_size>(pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES))
      {
      pop->orig_size--;
      ga_entity_dereference_by_rank(pop, pop->orig_size);
      }
    }
  else if (pop->elitism == GA_ELITISM_RESCORE_PARENTS)
    {
    plog(LOG_VERBOSE, "*** Fitness Re-evaluations ***");

/*
 * A forked process is started for each fitness evaluation upto
 * a maximum of max_processes at which point we wait for
 * results before forking more.
 */
  fork_num = 0;
  eval_num = 0;

  /* Fork initial processes. */

  while (fork_num < num_processes && eval_num < pop->orig_size)
    {
    eid[fork_num] = eval_num;
    pid[fork_num] = fork();

    if (pid[fork_num] < 0)
      {       /* Error in fork. */
      dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
      }
    else if (pid[fork_num] == 0)
      {       /* This is the child process. */
      pop->evaluate(pop, pop->entity_iarray[eval_num]);

      write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

      fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
      _exit(1);
      }

    fork_num++;
    eval_num++;

#ifdef NEED_MOSIX_FORK_HACK
    usleep(10);
#endif
    }
  num_forks = fork_num;

  /* Wait for a forked process to finish and, if needed, fork another. */
  while (num_forks > 0)
    {
    fpid = wait(NULL);

    if (fpid == -1) die("Error in wait().");

    /* Find which entity this forked process was evaluating. */
    fork_num = 0;
    while (fpid != pid[fork_num]) fork_num++;

    if (eid[fork_num] == -1) die("Internal error.  eid is -1");

    read(evalpipe[2*fork_num], &(pop->entity_iarray[eid[fork_num]]->fitness), sizeof(double));

    if (eval_num < pop->size)
      {       /* New fork. */
      eid[fork_num] = eval_num;
      pid[fork_num] = fork();

      if (pid[fork_num] < 0)
        {       /* Error in fork. */
        dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
        }
      else if (pid[fork_num] == 0)
        {       /* This is the child process. */
        pop->evaluate(pop, pop->entity_iarray[eval_num]);

        write(evalpipe[2*fork_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

        fsync(evalpipe[2*fork_num+1]);	/* Ensure data is written to pipe. */
        _exit(1);
        }

      eval_num++;
      }
    else
      {
      pid[fork_num] = -1;
      eid[fork_num] = -1;
      num_forks--;
      }
    }
    }

/*
 * Sort all population members by fitness.
 */
  sort_population(pop);

/*
 * Enforce the type of crowding desired.
 *
 * Rough crowding doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 *
 * FIXME: Crowding code missing!!!
 */

/*
 * Least fit population members die to restore the
 * population size to its stable size.
 */
  ga_genocide(pop, pop->stable_size);

  return;
  }
#endif


/**********************************************************************
  gaul_survival_threaded()
  synopsis:	Survival of the fittest.
		Enforce elitism, apply crowding operator, reduce
		population back to its stable size and rerank entities,
		as required.

		*** FIXME: crowding analysis incomplete. ***

  parameters:	population *pop
  return:	none
  last updated:	18 Mar 2003
 **********************************************************************/

#if HAVE_PTHREAD == 1
static void gaul_survival_threaded(population *pop,
			const int num_threads,
			int *eid, pthread_t *tid)
  {
  int		thread_num;		/* Index of current thread. */
  int		num_threads;		/* Number of threads. */
  int		eval_num;		/* Index of current entity. */
  pthread_t	fpid;			/* TID of completed child process. */

  plog(LOG_VERBOSE, "*** Survival of the fittest ***");

/*
 * Need to kill parents, or rescore parents?
 */
  if (pop->elitism == GA_ELITISM_PARENTS_DIE || pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES)
    {
    while (pop->orig_size>(pop->elitism == GA_ELITISM_ONE_PARENT_SURVIVES))
      {
      pop->orig_size--;
      ga_entity_dereference_by_rank(pop, pop->orig_size);
      }
    }
  else if (pop->elitism == GA_ELITISM_RESCORE_PARENTS)
    {
    plog(LOG_VERBOSE, "*** Fitness Re-evaluations ***");

/*
 * A thread is created for each fitness evaluation upto
 * a maximum of max_threads at which point we wait for
 * results before continuing.
 */
  thread_num = 0;
  eval_num = 0;

  /* Fork initial processes. */
  while (thread_num < num_processes && eval_num < pop->orig_size)
    {
    eid[thread_num] = eval_num;
    pid[thread_num] = XXXX();

    if (pid[thread_num] < 0)
      {       /* Error in fork. */
      dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
      }
    else if (pid[thread_num] == 0)
      {       /* This is the child process. */
      pop->evaluate(pop, pop->entity_iarray[eval_num]);

      write(evalpipe[2*thread_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

      fsync(evalpipe[2*thread_num+1]);	/* Ensure data is written to pipe. */
      _exit(1);
      }

    thread_num++;
    eval_num++;
    }
  num_threads = thread_num;

  /* Wait for a thread to finish and, if needed, create another. */
  while (num_threads > 0)
    {
    fpid = wait(NULL);

    if (fpid == -1) die("Error in wait().");

    /* Find which entity this thread was evaluating. */
    thread_num = 0;
    while (fpid != pid[thread_num]) thread_num++;

    if (eid[thread_num] == -1) die("Internal error.  eid is -1");

    read(evalpipe[2*thread_num], &(pop->entity_iarray[eid[thread_num]]->fitness), sizeof(double));

    if (eval_num < pop->orig_size)
      {       /* New thread. */
      eid[thread_num] = eval_num;
      pid[thread_num] = fork();

      if (pid[thread_num] < 0)
        {       /* Error in thread. */
        dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
        }
      else if (pid[thread_num] == 0)
        {       /* This is the child process. */
        pop->evaluate(pop, pop->entity_iarray[eval_num]);

        write(evalpipe[2*thread_num+1], &(pop->entity_iarray[eval_num]->fitness), sizeof(double));

        fsync(evalpipe[2*thread_num+1]);	/* Ensure data is written to pipe. */
        _exit(1);
        }

      eval_num++;
      }
    else
      {
      pid[thread_num] = -1;
      eid[thread_num] = -1;
      num_threads--;
      }
    }
    }

/*
 * Sort all population members by fitness.
 */
  sort_population(pop);

/*
 * Enforce the type of crowding desired.
 *
 * Rough crowding doesn't actual check whether two chromosomes are
 * identical - just assumes they are if they have identical
 * fitness.  Exact elitism does make the full check.
 *
 * FIXME: Crowding code missing!!!
 */

/*
 * Least fit population members die to restore the
 * population size to its stable size.
 */
  ga_genocide(pop, pop->stable_size);

  return;
  }
#endif /* HAVE_PTHREAD */


/**********************************************************************
  ga_evolution()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	11 Jun 2002
 **********************************************************************/

int ga_evolution(	population		*pop,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

  pop->generation = 0;

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations(pop);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first generation, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/*
 * Do all the generations:
 *
 * Stop when (a) max_generations reached, or
 *           (b) "pop->generation_hook" returns FALSE.
 */
  while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
           generation<max_generations )
    {
    generation++;
    pop->generation = generation;
    pop->orig_size = pop->size;

    plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Crossover step.
 */
    gaul_crossover(pop);

/*
 * Mutation step.
 */
    gaul_mutation(pop);

/*
 * Apply environmental adaptations, score entities, sort entities, etc.
 */
    gaul_adapt_and_evaluate(pop);

/*
 * Survival of the fittest.
 */
    gaul_survival(pop);

/*
 * Use callback.
 */
    plog(LOG_VERBOSE,
          "After generation %d, population has fitness scores between %f and %f",
          generation,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Generation loop. */

  return generation;
  }


/**********************************************************************
  ga_evolution_forked()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.  This is a
		generation-based GA.  ga_genesis(), or equivalent,
		must be called prior to this function.

		This function is like ga_evolution(), except that all
		fitness evaluations will be performed in forked
		processes which is ideal for use on multiprocessor
		machines or Beowulf-style clusters with process
		migration e.g. Mosix ( http://www.mosix.org/ ) or
		openMosix ( http://openmosix.sourceforge.net/ )

		Thanks go to Syrrx, Inc. who, in essence, funded
		development of this function.

  parameters:
  return:	Number of generations performed.
  last updated:	11 Jun 2002
 **********************************************************************/

#if W32_CRIPPLED != 1
int ga_evolution_forked(	population		*pop,
				const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		i;			/* Loop over members of population or pipes. */
  int		*evalpipe;		/* Pipes for returning fitnesses. */
  pid_t		*pid;			/* Child PIDs. */
  int		*eid;			/* Entity which forked process is evaluating. */
  int		max_processes=0;	/* Maximum number of processes to fork at one time. */
  char		*max_proc_str;		/* Value of enviroment variable. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called first).");
  
/*
 * Look at environment to find number of processes to fork.
 */
  max_proc_str = getenv(GA_NUM_PROCESSES_ENVVAR_STRING);
  if (max_proc_str) max_processes = atoi(max_proc_str);
  if (max_processes == 0) max_processes = GA_DEFAULT_NUM_PROCESSES;

  plog(LOG_VERBOSE, "The evolution has begun!  Upto %d processes will be fork'ed", max_processes);

  pop->generation = 0;

/*
 * Allocate memory required for handling the forked processes.
 * Open pipes for reporting fitnesses.
 * Clear pid and eid arrays.
 */
  pid = s_malloc(max_processes*sizeof(pid_t));
  eid = s_malloc(max_processes*sizeof(int));
  evalpipe = s_malloc(2*max_processes*sizeof(int));
  for (i=0; i<max_processes; i++)
    {
    if (pipe(&evalpipe[2*i])==-1) die("Unable to open pipe");
    pid[i] = -1;
    eid[i] = -1;
    }

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations_forked(pop, max_processes, eid, pid, evalpipe);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first generation, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/*
 * Do all the generations:
 *
 * Stop when (a) max_generations reached, or
 *           (b) "pop->generation_hook" returns FALSE.
 */
  while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
           generation<max_generations )
    {
    generation++;
    pop->generation = generation;
    pop->orig_size = pop->size;

    plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Crossover step.
 */
    gaul_crossover(pop);

/*
 * Mutation step.
 */
    gaul_mutation(pop);

/*
 * Score all child entities from this generation.
 */
    gaul_adapt_and_evaluate_forked(pop, max_processes, eid, pid, evalpipe);

/*
 * Apply survival pressure.
 */
    gaul_survival_forked(pop, max_processes, eid, pid, evalpipe);

    plog(LOG_VERBOSE,
          "After generation %d, population has fitness scores between %f and %f",
          generation,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Main generation loop. */

/*
 * Close the pipes and free memory.
 */
  for (i=0; i<max_processes; i++)
    {
    close(evalpipe[2*i]);
    close(evalpipe[2*i+1]);
    }

  s_free(pid);
  s_free(eid);
  s_free(evalpipe);

  return generation;
  }
#else
int ga_evolution_forked(	population		*pop,
				const int		max_generations )
  {
  die("Sorry, the ga_evolution_forked() function isn't available for Windows.");
  return 0;
  }
#endif


/**********************************************************************
  ga_evolution_threaded()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.  This is a
		generation-based GA.  ga_genesis(), or equivalent,
		must be called prior to this function.

		This function is like ga_evolution(), except that all
		fitness evaluations will be performed in threads
		and is therefore ideal for use on SMP multiprocessor
		machines or multipipelined processors (e.g. the new
		Intel Xeons).

  parameters:
  return:	Number of generations performed.
  last updated:	18 Sep 2002
 **********************************************************************/

#if HAVE_PTHREAD == 1
int ga_evolution_threaded(	population		*pop,
				const int		max_generations )
  {
  int			generation=0;		/* Current generation number. */
  int			i;			/* Loop over members of population. */
  pthread_t		*tid;			/* Child thread IDs. */
  pthread_mutex_t	*mid;			/* Child mutex IDs. */
  pthread_cond_t	*cid;			/* Child condition variable IDs. */
  int			*eid;			/* Entity which forked process is evaluating. */
  int			max_threads=0;		/* Maximum number of evaluation threads to use at one time. */
  char			*max_thread_str;	/* Value of enviroment variable. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called first).");
  
/*
 * Look at environment to find number of threads to create.
 */
  max_thread_str = getenv(GA_NUM_THREADS_ENVVAR_STRING);
  if (max_thread_str) max_threads = atoi(max_thread_str);
  if (max_threads == 0) max_threads = GA_DEFAULT_NUM_THREADS;

  plog(LOG_VERBOSE, "The evolution has begun!  Upto %d threads will be created", max_threads);

  pop->generation = 0;

/*
 * Start with all threads locked.
 */
THREAD_LOCK(global_thread_lock);

/*
 * Allocate memory required for handling the threads.
 * Initially use eid array to pass thread enumerations.
 */
  tid = s_malloc(max_threads*sizeof(pthread_t));
  mid = s_malloc(max_threads*sizeof(pthread_mutex_t));
  cid = s_malloc(max_threads*sizeof(pthread_cond_t));
  eid = s_malloc(max_threads*sizeof(int));
  for (i=0; i<max_threads; i++)
    {
    eid[i] = i;
    if ( !pthread_mutex_init(&(mid[i]), NULL) )
      die("Unable to initialize mutex variable.");
    if ( !pthread_cond_init(&(cid[i]), NULL) )
      die("Unable to initialize condition variable.");
    if ( !pthread_create(&(tid[i]), NULL, (void *(*)(void *)) worker_thread, (void *) &(eid[i])) )
      die("Unable to create thread.");
    }

/*
 * Clear eid array.
 */
  for (i=0; i<max_threads; i++)
    {
    pthread_mutex_lock(&(mid[i]));
    pthread_cond_wait(&(cid[i]), &(mid[i]));
    pthread_mutex_unlock(&(mid[i]));
    eid[i] = -1;
    }

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations_threaded(pop, max_threads, eid, tid);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first generation, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/*
 * Do all the generations:
 *
 * Stop when (a) max_generations reached, or
 *           (b) "pop->generation_hook" returns FALSE.
 */
  while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
           generation<max_generations )
    {
    generation++;
    pop->generation = generation;
    pop->orig_size = pop->size;

    plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Crossover step.
 */
    gaul_crossover(pop);

/*
 * Mutation step.
 */
    gaul_mutation(pop);

/*
 * Score all child entities from this generation.
 */
    gaul_adapt_and_evaluate_threaded(pop, max_threads, eid, tid);

/*
 * Apply survival pressure.
 */
    gaul_survival_threaded(pop, max_threads, eid, tid);

    plog(LOG_VERBOSE,
          "After generation %d, population has fitness scores between %f and %f",
          generation,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Main generation loop. */

/*
 * Clean-up threads.
 */
  for (i=0; i<max_threads; i++)
    {
    pthread_cancel(&(tid[i]));
    pthread_join(&(tid[i]));
    pthread_mutex_destroy(&(mid[i]));
    pthread_cond_destroy(&(cid[i]));
    }
  
/*
 * Free memory.
 */
  s_free(tid);
  s_free(mid);
  s_free(cid);
  s_free(eid);

  return generation;
  }
#else
int ga_evolution_threaded(	population		*pop,
				const int		max_generations )
  {

  die("Support for ga_evolution_threaded() not compiled.");

  return 0;
  }
#endif /* HAVE_PTHREAD */


/**********************************************************************
  ga_evolution_with_stats()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
		This is almost identical to ga_evolution() except is
		modified to facilitate the collection of certain
		statistics.

		*** Should be deprecated. ***
  parameters:
  return:
  last updated:	11 Jun 2002
 **********************************************************************/

#ifndef COMPILE_DEPRECATED_FUNCTIONS

int ga_evolution_with_stats(	population		*pop,
					const ga_elitism_type	elitism,
					const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  boolean	finished;		/* Whether crossover/mutation rounds are complete. */
  int		new_pop_size;		/* Population size prior to adaptation. */
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_WARNING, "This should be a deprecated function!");

  plog(LOG_VERBOSE, "The evolution has begun!");

  pop->generation = 0;

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations(pop);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first generation, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/* Do all the generations: */
  while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
           generation<max_generations )
    {
    generation++;
    pop->generation = generation;
    pop->orig_size = pop->size;

    plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Zero statistics.
 */
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mating cycle ***");

    pop->select_state = 0;

    finished = FALSE;
    while (!finished)
      {
      finished = pop->select_two(pop, &mother, &father);

      if (mother && father)
        {
        plog(LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness,
             ga_get_entity_id(pop, father),
             ga_get_entity_rank(pop, father), father->fitness);

        son = ga_get_free_entity(pop);
        daughter = ga_get_free_entity(pop);
        pop->crossover(pop, mother, father, daughter, son);
        pop->evaluate(pop, daughter);
        pop->evaluate(pop, son);

/*
 * Collate stats.
 */
        if (son->fitness > father->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (daughter->fitness > father->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (son->fitness > mother->fitness)
          crossover_good++;
        else
          crossover_poor++;
        if (daughter->fitness > mother->fitness)
          crossover_good++;
        else
          crossover_poor++;

        if (son->fitness > MAX(mother->fitness,father->fitness))
          crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
        if (daughter->fitness > MAX(mother->fitness,father->fitness))
          crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);
        }
      else
        {
        plog( LOG_VERBOSE, "Crossover not performed." );
        }
      }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mutation cycle ***");

    pop->select_state = 0;

    finished = FALSE;
    while (!finished)
      {
      finished = pop->select_one(pop, &mother);

      if (mother)
        {
        plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness );

        daughter = ga_get_free_entity(pop);
        pop->mutate(pop, mother, daughter);
        pop->evaluate(pop, daughter);

/*
 * Collate stats.
 */
        if (daughter->fitness > mother->fitness)
          {
          mutation_good++;
          mutation_gain += daughter->fitness-mother->fitness;
          }
        else
          {
          mutation_poor++;
          }

        }
      else
        {
        plog( LOG_VERBOSE, "Mutation not performed." );
        }
      }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Baldwinian evolution.
 * Performed, and genes are modified, in the case of Lamarckian evolution.
 *
 * Maybe, could reoptimise all solutions at each generation.  This would allow
 * a reduced optimisation protocol and only those solutions which are
 * reasonable would survive for further optimisation.
 */
  if (pop->scheme != GA_SCHEME_DARWIN)
    {
    plog(LOG_VERBOSE, "*** Adaptation round ***");

    new_pop_size = pop->size;

    switch (pop->scheme)
      {
      case (GA_SCHEME_BALDWIN_CHILDREN):
        /* Baldwinian evolution for children only. */
        for (i=pop->orig_size; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_BALDWIN_ALL):
        /* Baldwinian evolution for entire population. */
        /* I don't recommend this, but it is here for completeness. */
        for (i=0; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_CHILDREN):
        /* Lamarckian evolution for children only. */
        while (new_pop_size>pop->orig_size)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_ALL):
        /* Lamarckian evolution for entire population. */
        while (new_pop_size>0)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      default:
        dief("Unknown evolutionary scheme %d.\n", pop->scheme);
      }
    }

/*
 * Least fit population members die to restore the
 * population size to the stable size.
 */
    gaul_survival(pop);

    plog(LOG_VERBOSE,
          "After generation %d, population has fitness scores between %f and %f",
          generation,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", generation,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);
    }	/* Generation loop. */

  return generation;
  }
#endif


/**********************************************************************
  ga_evolution_steady_state()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a steady-state GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	11 Jun 2002
 **********************************************************************/

int ga_evolution_steady_state(	population		*pop,
					const int		max_iterations )
  {
  int		iteration=0;		/* Current iteration count. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter, *child;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  int		new_pop_size;		/* Population size prior to adaptation. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (!pop->replace) die("Population's replacement callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

  pop->generation = 0;

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations(pop);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/* Do all the iterations: */
  while ( (pop->generation_hook?pop->generation_hook(iteration, pop):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;
    pop->orig_size = pop->size;

    son = NULL;
    daughter = NULL;
    child = NULL;

    plog(LOG_DEBUG,
              "Population size is %d at start of iteration %d",
              pop->orig_size, iteration );

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mating ***");

    pop->select_state = 0;

    pop->select_two(pop, &mother, &father);

    if (mother && father)
      {
      plog(LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness,
             ga_get_entity_id(pop, father),
             ga_get_entity_rank(pop, father), father->fitness);

      son = ga_get_free_entity(pop);
      daughter = ga_get_free_entity(pop);
      pop->crossover(pop, mother, father, daughter, son);
      pop->evaluate(pop, daughter);
      pop->evaluate(pop, son);
      }
    else
      {
      plog( LOG_VERBOSE, "Crossover not performed." );
      }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mutation ***");

    pop->select_state = 0;

    pop->select_one(pop, &mother);

    if (mother)
      {
      plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness );

      child = ga_get_free_entity(pop);
      pop->mutate(pop, mother, child);
      pop->evaluate(pop, child);

      }
    else
      {
      plog( LOG_VERBOSE, "Mutation not performed." );
      }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Baldwinian evolution.
 * Performed, and genes are modified, in the case of Lamarckian evolution.
 *
 * Maybe, could reoptimise all solutions at each generation.  This would allow
 * a reduced optimisation protocol and only those solutions which are
 * reasonable would survive for further optimisation.
 *
 * FIXME: This is wrong for GA_SCHEME_BALDWIN, GA_SCHEME_LAMARCK and may be
 * optimised for GA_SCHEME_BALDWIN_ALL, GA_SCHEME_LAMARCK_ALL. 
 */
  if (pop->scheme != GA_SCHEME_DARWIN)
    {
    plog(LOG_VERBOSE, "*** Adaptation ***");

    new_pop_size = pop->size;

    switch (pop->scheme)
      {
      case (GA_SCHEME_BALDWIN_CHILDREN):
        /* Baldwinian evolution for children only. */
        for (i=pop->orig_size; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_BALDWIN_ALL):
        /* Baldwinian evolution for entire population. */
        /* I don't recommend this, but it is here for completeness. */
        for (i=0; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_CHILDREN):
        /* Lamarckian evolution for children only. */
        while (new_pop_size>pop->orig_size)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_ALL):
        /* Lamarckian evolution for entire population. */
        while (new_pop_size>0)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      default:
        dief("Unknown evolutionary scheme %d.\n", pop->scheme);
      }
    }

/*
 * Insert new entities into population.
 */
    if (son) pop->replace(pop, son);
    if (daughter) pop->replace(pop, daughter);
    if (child) pop->replace(pop, child);

/*
 * Use callback.
 */
    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After iteration %d, population has fitness scores between %f and %f",
          iteration,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Iteration loop. */

  return (iteration<max_iterations);
  }


/**********************************************************************
  ga_evolution_steady_state_with_stats()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a steady-state GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	11 Jun 2002
 **********************************************************************/

#ifndef COMPILE_DEPRECATED_FUNCTIONS

int ga_evolution_steady_state_with_stats(	population	*pop,
						const int	max_iterations )
  {
  int		iteration=0;		/* Current iteration count. */
  int		i;			/* Loop over members of population. */
  entity	*mother, *father;	/* Parent entities. */
  entity	*son, *daughter, *child;	/* Child entities. */
  entity	*adult;			/* Temporary copy for gene optimisation. */
  int		new_pop_size;		/* Population size prior to adaptation. */
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
  int		crossover_good, crossover_poor;	/* Fornication statistics. */
  int		mutation_good, mutation_poor;	/*  - " -  */
  double	crossover_gain, mutation_gain;	/*  - " -  */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (!pop->replace) die("Population's replacement callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

  pop->generation = 0;

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
  sprintf(stats_fname, "ga_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "gen crossover mutation\n");
  fclose(STATS_OUT);

/*
 * Score and sort the initial population members.
 */
  gaul_ensure_evaluations(pop);
  sort_population(pop);

  plog( LOG_VERBOSE,
        "Prior to the first iteration, population has fitness scores between %f and %f",
        pop->entity_iarray[0]->fitness,
        pop->entity_iarray[pop->size-1]->fitness );

/* Do all the iterations: */
  while ( (pop->generation_hook?pop->generation_hook(iteration, pop):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;
    pop->orig_size = pop->size;

    son = NULL;
    daughter = NULL;
    child = NULL;

    plog(LOG_DEBUG,
              "Population size is %d at start of iteration %d",
              pop->orig_size, iteration );

/*
 * Zero statistics.
 */
    crossover_good=0;
    crossover_poor=0;
    mutation_good=0;
    mutation_poor=0;

    crossover_gain=0.0;
    mutation_gain=0.0;

/*
 * Mating cycle.
 *
 * Select pairs of entities to mate via crossover. (Sexual reproduction).
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mating ***");

    pop->select_state = 0;

    pop->select_two(pop, &mother, &father);

    if (mother && father)
      {
      plog(LOG_VERBOSE, "Crossover between %d (%d = %f) and %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness,
             ga_get_entity_id(pop, father),
             ga_get_entity_rank(pop, father), father->fitness);

      son = ga_get_free_entity(pop);
      daughter = ga_get_free_entity(pop);
      pop->crossover(pop, mother, father, daughter, son);
      pop->evaluate(pop, daughter);
      pop->evaluate(pop, son);

/*
 * Collate stats.
 */
      if (son->fitness > father->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (daughter->fitness > father->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (son->fitness > mother->fitness)
        crossover_good++;
      else
        crossover_poor++;
      if (daughter->fitness > mother->fitness)
        crossover_good++;
      else
        crossover_poor++;

      if (son->fitness > MAX(mother->fitness,father->fitness))
        crossover_gain += son->fitness-MAX(mother->fitness,father->fitness);
      if (daughter->fitness > MAX(mother->fitness,father->fitness))
        crossover_gain += daughter->fitness-MAX(mother->fitness,father->fitness);

      }
    else
      {
      plog( LOG_VERBOSE, "Crossover not performed." );
      }

/*
 * Mutation cycle.
 *
 * Select entities to undergo asexual reproduction, in which case the child will
 * have a genetic mutation of some type.
 *
 * Score the new entities as we go.
 */
    plog(LOG_VERBOSE, "*** Mutation ***");

    pop->select_state = 0;

    pop->select_one(pop, &mother);

    if (mother)
      {
      plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
             ga_get_entity_id(pop, mother),
             ga_get_entity_rank(pop, mother), mother->fitness );

      child = ga_get_free_entity(pop);
      pop->mutate(pop, mother, child);
      pop->evaluate(pop, child);

/*
 * Collate stats.
 */
      if (child->fitness > mother->fitness)
        {
        mutation_good++;
        mutation_gain += child->fitness-mother->fitness;
        }
      else
        {
        mutation_poor++;
        }

      }
    else
      {
      plog( LOG_VERBOSE, "Mutation not performed." );
      }

/*
 * Environmental adaptation.
 *
 * Skipped in the case of Darwinian evolution.
 * Performed in the case of Baldwinian evolution.
 * Performed, and genes are modified, in the case of Lamarckian evolution.
 *
 * Maybe, could reoptimise all solutions at each generation.  This would allow
 * a reduced optimisation protocol and only those solutions which are
 * reasonable would survive for further optimisation.
 *
 * FIXME: This is wrong for GA_SCHEME_BALDWIN, GA_SCHEME_LAMARCK and may be
 * optimised for GA_SCHEME_BALDWIN_ALL, GA_SCHEME_LAMARCK_ALL. 
 */
  if (pop->scheme != GA_SCHEME_DARWIN)
    {
    plog(LOG_VERBOSE, "*** Adaptation ***");

    new_pop_size = pop->size;

    switch (pop->scheme)
      {
      case (GA_SCHEME_BALDWIN_CHILDREN):
        /* Baldwinian evolution for children only. */
        for (i=pop->orig_size; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_BALDWIN_ALL):
        /* Baldwinian evolution for entire population. */
        /* I don't recommend this, but it is here for completeness. */
        for (i=0; i<new_pop_size; i++)
          {
          adult = pop->adapt(pop, pop->entity_iarray[i]);
          pop->entity_iarray[i]->fitness=adult->fitness;
/* check. */ s_assert(ga_get_entity_rank(pop, adult) == new_pop_size);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_CHILDREN):
        /* Lamarckian evolution for children only. */
        while (new_pop_size>pop->orig_size)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      case (GA_SCHEME_LAMARCK_ALL):
        /* Lamarckian evolution for entire population. */
        while (new_pop_size>0)
          {
          new_pop_size--;
          adult = pop->adapt(pop, pop->entity_iarray[new_pop_size]);
          ga_entity_dereference_by_rank(pop, new_pop_size);
          }
        break;
      default:
        dief("Unknown evolutionary scheme %d.\n", pop->scheme);
      }
    }

/*
 * Insert new entities into population.
 */
    if (son) pop->replace(pop, son);
    if (daughter) pop->replace(pop, daughter);
    if (child) pop->replace(pop, child);

/*
 * Use callback.
 */
    plog(LOG_VERBOSE, "*** Analysis ***");

    plog(LOG_VERBOSE,
          "After iteration %d, population has fitness scores between %f and %f",
          iteration,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Write statistics.
 */
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %d-%d %f %d-%d %f\n", iteration,
            crossover_good, crossover_poor, crossover_gain,
            mutation_good, mutation_poor, mutation_gain);
    fclose(STATS_OUT);

    }	/* Iteration loop. */

  return (iteration<max_iterations);
  }
#endif


/**********************************************************************
  ga_random_mutation_hill_climbing()
  synopsis:	Perform equivalent to zero temperature metropolis
		optimisation.  If initial solution is NULL, then a
		random initial solution is generated.
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	18/12/00
 **********************************************************************/

entity *ga_random_mutation_hill_climbing(	population	*pop,
						entity		*initial,
						const int	max_iterations)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new, *temp;	/* The solutions. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform RMHC optimisation with random starting solution.");

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform RMHC optimisation with specified starting solution.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "rmhc_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Random Mutation Hill Climbing\n");
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG,
       "Prior to the scoring, the solution has fitness score of %f",
       best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

    plog( LOG_VERBOSE,
          "Prior to the iteration %d, the solution has fitness score of %f",
          iteration, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
         ga_get_entity_id(pop, current),
         ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

    if (best->fitness < current->fitness)
      {
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f\n", iteration, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog( LOG_VERBOSE,
        "After final iteration, the solution has fitness score of %f",
        current->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);

  return best;
  }


/**********************************************************************
  ga_next_ascent_hill_climbing()
  synopsis:	Perform systematic ascent hill climbing optimisation.
		(Needn't nessecarily use next allele each time, but
	       	this was the simplist to implement.)
		If initial solution is NULL, then a randomly generated
		initial solution is generated.
		The original entity will not be munged.
		NOTE: Needs to be passed an 'extended' mutation
		function.
		max_iterations refers to the number of _complete_
		cycles.
  parameters:
  return:	Best solution found.
  last updated:	21/12/00
 **********************************************************************/

#ifndef COMPILE_DEPRECATED_FUNCTIONS

entity *old_ga_next_ascent_hill_climbing(	population		*pop,
					entity			*initial,
					const int		max_iterations,
					GAspecificmutate	mutationfunc)
  {
  int		iteration=0;		/* Current iteration number. */
  entity	*current, *best;	/* The solutions. */
  int		chromo=0, point=0;	/* Mutation locus. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;		/* Filehandle for stats log. */
  char		stats_fname[80];	/* Filename for stats log. */
#endif

  plog(LOG_WARNING, "This is a deprecated function!");

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !mutationfunc ) die("Mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "NAHC algorithm is not parallelised.");

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform NAHC optimisation with random starting solution.");

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform NAHC optimisation with specified starting solution.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "nahc_stats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Next Ascent Hill Climbing\n");
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {

    plog( LOG_VERBOSE,
          "Iteration %d chromosome %d, point %d, solution has fitness score of %f",
          iteration, chromo, point,
          current->fitness );

    mutationfunc(chromo, point, current->chromosome[chromo]);

    ga_entity_clear_data(pop, current, chromo);	/* Required to force regeneration of structural data. */
    pop->evaluate(pop, current);

/*
 * Is current better than best?
 */
    if (best->fitness < current->fitness)
      {
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f (%d %d)\n", iteration, best->fitness, chromo, point);
    fclose(STATS_OUT);
#endif

/*
 * Choose next nucleotide to mutate/optimise.
 */
    point++;
    if (point == pop->len_chromosomes)
      {
      point = 0;
      chromo++;
      if (chromo == pop->num_chromosomes)
        {
        chromo=0;
        iteration++;
        }
      }
    }

  plog( LOG_VERBOSE,
        "After final iteration, the solution has fitness score of %f",
        current->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);

  return best;
  }
#endif


/**********************************************************************
  ga_metropolis_mutation()
  synopsis:	Perform arbitrary temperature metropolis optimisation.
		If initial solution is NULL, then random solution is
		generated.  Syncs with other processors every iteration
		and grabs the overall best solution if better than
		this processors current solution is worse than the best
		plus the temperature. 
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	19/01/01
 **********************************************************************/

#ifndef COMPILE_DEPRECATED_FUNCTIONS

entity *ga_metropolis_mutation(	population		*pop,
				entity			*initial,
				const int		max_iterations,
				const int 		temperature)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new;		/* The solutions. */
  entity	*temp=NULL;			/* Used for swapping current and new. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

  plog(LOG_NORMAL, "This function is deprecated!");

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !pop->mutate ) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "Metropolis algorithm is not parallelised.");

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform metropolis optimisation at %d degrees with random starting solution.", temperature);

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform metropolis optimisation at %d degrees.");
    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "mstats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "Metropolis optimisation at %d degrees.\n", temperature);
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution found over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    iteration++;

    plog(LOG_VERBOSE,
              "Prior to iteration %d, solution has fitness score of %f",
              iteration, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
         ga_get_entity_id(pop, current),
         ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

/*
 * Should we keep this solution?
 */
    if ( best->fitness < current->fitness ||
         random_boolean_prob(exp((current->fitness-best->fitness)
                                   /(GA_BOLTZMANN_FACTOR*temperature))) )
      {
/*        plog(LOG_DEBUG, "Selecting new solution."); */
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      {
/*        plog(LOG_DEBUG, "Rejecting new solution."); */
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }  

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: %f\n", iteration, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog( LOG_VERBOSE,
        "After final iteration, solution has fitness score of %f",
        best->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);
  ga_entity_dereference(pop, temp);

  return best;
  }
#endif


/**********************************************************************
  ga_simulated_annealling_mutation()
  synopsis:	Perform mutation/SA (GA mutation/Simulated Annealling).
		If initial solution is NULL, then random solution is
		generated.  Syncs with other processors every iteration
		and grabs the overall best solution if better than
		this processors current solution is worse than the best
		plus the temperature. 
		The original entity will not be munged.
  parameters:
  return:	Best solution found.
  last updated:	21/02/01
 **********************************************************************/

#ifndef COMPILE_DEPRECATED_FUNCTIONS

entity *ga_simulated_annealling_mutation(population	*pop,
					entity		*initial,
					const int	max_iterations,
					const int 	initial_temperature,
					const int 	final_temperature)
  {
  int		iteration=0;			/* Current iteration number. */
  entity	*current, *best, *new;		/* The solutions. */
  entity	*temp=NULL;			/* Used for swapping current and new. */
  int		temperature;			/* Current temperature. */
#if GA_WRITE_STATS==TRUE
  FILE		*STATS_OUT;			/* Filehandle for stats log. */
  char		stats_fname[80];		/* Filename for stats log. */
#endif

  plog(LOG_NORMAL, "This function is deprecated!");

/* Checks. */
  if ( !pop ) die("NULL pointer to population structure passed.");
  if ( !pop->evaluate ) die("Population's evaluation callback is undefined.");
  if ( !pop->mutate ) die("Population's mutation callback is undefined.");

  current = ga_get_free_entity(pop);	/* The 'working' solution. */
  best = ga_get_free_entity(pop);	/* The best solution so far. */

  plog(LOG_FIXME, "Simulated annealling algorithm is not parallelised.");

/* Do we need to generate a random starting solution? */
  if (!initial)
    {
    plog(LOG_VERBOSE, "Will perform %d steps of MC/SA optimisation between %d and %d degrees with random starting solution.", max_iterations, initial_temperature, final_temperature);

    ga_entity_seed(pop, best);
    }
  else
    {
    plog(LOG_VERBOSE, "Will perform %d steps of MC/SA optimisation between %d and %d degrees.", max_iterations, initial_temperature, final_temperature);

    ga_entity_copy(pop, best, initial);
    }

/*
 * Create name for statistics log file.
 * Write a simple header to that file.
 */
#if GA_WRITE_STATS==TRUE
  sprintf(stats_fname, "sastats_%d.dat", (int) getpid());
  STATS_OUT = fopen(stats_fname, "a");
  fprintf(STATS_OUT, "MC/SA optimisation at %d to %d degrees.\n",
                     initial_temperature, final_temperature);
  fclose(STATS_OUT);
#endif

/*
 * Score the initial solution.
 */
  if (best->fitness==GA_MIN_FITNESS) pop->evaluate(pop, best);
  plog(LOG_DEBUG, "Prior to the scoring, the solution has fitness score of %f", best->fitness );

/*
 * Copy best solution over current solution.
 */
  ga_entity_copy(pop, current, best);
  new = ga_get_free_entity(pop);

/* Do all the iterations: */
  while ( (pop->iteration_hook?pop->iteration_hook(iteration, current):TRUE) &&
           iteration<max_iterations )
    {
    temperature = initial_temperature + ((double)iteration/max_iterations)*(final_temperature-initial_temperature);
    iteration++;

    plog( LOG_VERBOSE,
          "Prior to iteration %d temperature %d, solution has fitness score of %f",
          iteration, temperature, current->fitness );

/*
 * Perform random mutation.
 */
    plog(LOG_VERBOSE, "Mutation of %d (%d = %f)",
       ga_get_entity_id(pop, current),
       ga_get_entity_rank(pop, current), current->fitness );

    pop->mutate(pop, current, new);

    temp = current;
    current = new;
    new = temp;

    pop->evaluate(pop, current);

/*
 * Should we keep this solution?
      if ( best->fitness < current->fitness ||
           random_boolean_prob(exp((current->fitness-best->fitness)
                                   /(GA_BOLTZMANN_FACTOR*temperature))) )
 */
    if ( best->fitness < current->fitness+temperature )
      { /* Copy this solution best solution. */
/*        plog(LOG_DEBUG, "Selecting new solution.");*/
      ga_entity_blank(pop, best);
      ga_entity_copy(pop, best, current);
      }
    else
      { /* Copy best solution over current solution. */
/*        plog(LOG_DEBUG, "Rejecting new solution.");*/
      ga_entity_blank(pop, current);
      ga_entity_copy(pop, current, best);
      }

    ga_entity_blank(pop, new);

/*
 * Write statistics.
 */
#if GA_WRITE_STATS==TRUE
    STATS_OUT = fopen(stats_fname, "a");
    fprintf(STATS_OUT, "%d: (%d degrees) %f\n",
            iteration, temperature, best->fitness);
    fclose(STATS_OUT);
#endif
    }

  plog(LOG_VERBOSE,
            "After final iteration, the solution has fitness score of %f",
            best->fitness );

/*
 * Current no longer needed.  It is upto the caller to dereference the
 * optimum solution found.
 */
  ga_entity_dereference(pop, current);
  ga_entity_dereference(pop, temp);

  return best;
  }
#endif


/**********************************************************************
  ga_evolution_archipelago()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given populations using a simple
		island model.  Migration occurs around a cyclic
		topology only.  Migration causes a duplication of the
		respective entities.  This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:	const int	num_pops
		population	**pops
		const int	max_generations
  return:	number of generation performed
  last updated:	11 Jun 2002
 **********************************************************************/

int ga_evolution_archipelago( const int num_pops,
			population		**pops,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */
  int		island;			/* Current island number. */
  population	*pop=NULL;		/* Current population. */
  boolean	complete=FALSE;		/* Whether evolution is terminated. */

/* Checks. */
  if (!pops)
    die("NULL pointer to array of population structures passed.");
  if (num_pops<2)
    die("Need at least two populations for the island model.");

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (!pop->evaluate) die("Population's evaluation callback is undefined.");
    if (!pop->select_one) die("Population's asexual selection callback is undefined.");
    if (!pop->select_two) die("Population's sexual selection callback is undefined.");
    if (!pop->mutate) die("Population's mutation callback is undefined.");
    if (!pop->crossover) die("Population's crossover callback is undefined.");
    if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
    if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

/* Set island property. */
    pop->island = island;
    }

  plog(LOG_VERBOSE, "The evolution has begun on %d islands!", num_pops);

  pop->generation = 0;

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

/*
 * Score and sort the initial population members.
 */
    gaul_ensure_evaluations(pop);
    sort_population(pop);
  
    plog( LOG_VERBOSE,
          "Prior to the first generation, population on island %d has fitness scores between %f and %f",
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );
    }

/* Do all the generations: */
  while ( generation<max_generations && complete==FALSE)
    {
    generation++;
    pop->generation = generation;

/*
 * Migration step.
 */
    gaul_migration(num_pops, pops);

    for(island=0; island<num_pops; island++)
      {
      pop = pops[island];

      plog( LOG_VERBOSE, "*** Evolution on island %d ***", island );

      if (pop->generation_hook?pop->generation_hook(generation, pop):TRUE)
        {
        pop->orig_size = pop->size;

        plog( LOG_DEBUG,
              "Population %d size is %d at start of generation %d",
              island, pop->orig_size, generation );

/*
 * Crossover step.
 */
        gaul_crossover(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Mutation step.
 */
        gaul_mutation(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Apply environmental adaptations, score entities, sort entities, etc.
 */
        gaul_adapt_and_evaluate(pop);

/*
 * Survival of the fittest.
 */
        gaul_survival(pop);

        }
      else
        {
        complete = TRUE;
        }
      }

    plog(LOG_VERBOSE,
          "After generation %d, population %d has fitness scores between %f and %f",
          generation,
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Generation loop. */

  return generation;
  }


/**********************************************************************
  ga_evolution_archipelago_forked()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given populations using a simple
		island model.  Migration occurs around a cyclic
		topology only.  Migration causes a duplication of the
		respective entities.  This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
		This is a multiprocess version, using a forked process
		for each island.
  parameters:	const int	num_pops
		population	**pops
		const int	max_generations
  return:	number of generation performed
  last updated:	11 Jun 2002
 **********************************************************************/

#if W32_CRIPPLED != 1
int ga_evolution_archipelago_forked( const int num_pops,
			population		**pops,
			const int		max_generations )
  {
  int		generation=0;		/* Current generation number. */

  plog(LOG_FIXME, "Code incomplete.");

#if 0
  int		island;			/* Current island number. */
  population	*pop=NULL;		/* Current population. */
  boolean	complete=FALSE;		/* Whether evolution is terminated. */
  int		i;			/* Loop over members of population. */
  int		*evalpipe;		/* Pipes for returning fitnesses. */
  pid_t		*pid;			/* Child PIDs. */
  int		*eid;			/* Entity which forked process is evaluating. */
  int		fork_num;		/* Index of current forked process. */
  int		num_forks;		/* Number of forked processes. */
  int		eval_num;		/* Index of current entity. */
  pid_t		fpid;			/* PID of completed child process. */

/* Checks. */
  if (!pops)
    die("NULL pointer to array of population structures passed.");
  if (num_pops<2)
    die("Need at least two populations for the island model.");

  pop->generation = 0;

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (!pop->evaluate) die("Population's evaluation callback is undefined.");
    if (!pop->select_one) die("Population's asexual selection callback is undefined.");
    if (!pop->select_two) die("Population's sexual selection callback is undefined.");
    if (!pop->mutate) die("Population's mutation callback is undefined.");
    if (!pop->crossover) die("Population's crossover callback is undefined.");
    if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
    if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

/* Set island property. */
    pop->island = island;
    }

  plog(LOG_VERBOSE, "The evolution has begun on %d islands!", num_pops);
  
/*
 * Allocate memory.
 * Open pipes for reporting fitnesses.
 * Clear pid and eid arrays.
 */
  pid = s_malloc(max_processes*sizeof(pid_t));
  eid = s_malloc(max_processes*sizeof(int));
  evalpipe = s_malloc(2*max_processes*sizeof(int));
  for (i=0; i<max_processes; i++)
    {
    if (pipe(&evalpipe[2*i])==-1) die("Unable to open pipe");
    pid[i] = -1;
    eid[i] = -1;
    }

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (pid[fork_num] < 0)
      {       /* Error in fork. */
      dief("Error %d in fork. (%s)", errno, errno==EAGAIN?"EAGAIN":errno==ENOMEM?"ENOMEM":"unknown");
      }
    else if (pid[fork_num] == 0)
      {       /* This is the child process. */

/*
 * Score and sort the initial population members.
 */
    gaul_ensure_evaluations(pop);
    sort_population(pop);

    plog( LOG_VERBOSE,
          "Prior to the first generation, population on island %d has fitness scores between %f and %f",
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );
    }

/* Do all the generations: */
    while ( generation<max_generations && complete==FALSE)
      {
      generation++;
      pop->generation = generation;

/*
 * Migration and synchronisation step.
 */
      FIXME.

      plog( LOG_VERBOSE, "*** Evolution on island %d ***", island );

      if ( pop->generation_hook?pop->generation_hook(generation, pop):TRUE &&
           complete == FALSE )
        {
        pop->orig_size = pop->size;

        plog( LOG_DEBUG,
              "Population %d size is %d at start of generation %d",
              island, pop->orig_size, generation );

/*
 * Crossover step.
 */
        gaul_crossover(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Mutation step.
 */
        gaul_mutation(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Apply environmental adaptations, score entities, sort entities, etc.
 */
        gaul_adapt_and_evaluate(pop);

/*
 * Survival of the fittest.
 */
        gaul_survival(pop);

        }
      else
        {
        complete = TRUE;
        }

      plog(LOG_VERBOSE,
          "After generation %d, population %d has fitness scores between %f and %f",
          generation,
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

      }	/* Generation loop. */

    _exit((int) complete);
    }

/*
 * Parent waits for children.
 * Synchronise children at the beginning of each generation, and
 * tell them to die if necessary.
 */
  FIXME.
  
/*
 * Collate final entities.
 */
  FIXME.

/*
 * Close the pipes and free memory.
 */
  for (i=0; i<max_processes; i++)
    {
    close(evalpipe[2*i]);
    close(evalpipe[2*i+1]);
    }

  s_free(pid);
  s_free(eid);
  s_free(evalpipe);
#endif

  return generation;
  }
#else
int ga_evolution_archipelago_forked( const int num_pops,
			population		**pops,
			const int		max_generations )
  {
  die("Sorry, the ga_evolution_archipelago_forked() function isn't available for Windows.");
  return 0;
  }
#endif


/**********************************************************************
  ga_evolution_archipelago_mp()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given populations using a simple
		island model.  Migration occurs around a cyclic
		topology only.  Migration causes a duplication of the
		respective entities.  This is a generation-based GA.
		This is a multi-processor version with uses one
	       	processor for one or more islands.  Note that the
		populations must be pre-distributed.  The number of
		populations on each processor and the properties (e.g.
		size) of those populations need not be equal - but be
		careful of load-balancing issues in this case.  Safe
		to call (but slightly inefficient) in single processor
		case.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:	const int	num_pops
		population	**pops
		const int	max_generations
  return:	number of generation performed
  last updated:	11 Jun 2002
 **********************************************************************/

int ga_evolution_archipelago_mp( const int num_pops,
			population		**pops,
			const int		max_generations )
  {
#if HAVE_MPI == 1
  int		generation=0;		/* Current generation number. */
  int		island;			/* Current island number. */
  int		i;			/* Loop over members of population. */
  population	*pop=NULL;		/* Current population. */
  boolean	complete=FALSE;		/* Whether evolution is terminated. */
  int		pop0_osize;		/* Required for correct migration. */
  boolean	*send_mask;		/* Whether particular entities need to be sent. */
  int		send_count;		/* Number of entities to send. */
  int		max_size=0;		/* Largest maximum size of populations. */

/* Checks. */
  if (!pops)
    die("NULL pointer to array of population structures passed.");

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

    if (!pop->evaluate) die("Population's evaluation callback is undefined.");
    if (!pop->select_one) die("Population's asexual selection callback is undefined.");
    if (!pop->select_two) die("Population's sexual selection callback is undefined.");
    if (!pop->mutate) die("Population's mutation callback is undefined.");
    if (!pop->crossover) die("Population's crossover callback is undefined.");
    if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
    if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

/* Set island property. */
    pop->island = island;
    }

  plog(LOG_VERBOSE, "The evolution has begun on %d islands on node %d!", num_pops, mpi_get_rank());

  for (island=0; island<num_pops; island++)
    {
    pop = pops[island];

/*
 * Score and sort the initial population members.
 */
    ga_population_score_and_sort(pop);
    plog( LOG_VERBOSE,
          "Prior to the first generation, population on island %d (process %d) has fitness scores between %f and %f",
          island, mpi_get_rank(),
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    max_size = max(max_size, pop->max_size);
    }

  /* Allocate send_mask array. */
  send_mask = s_malloc(max_size*sizeof(boolean));

/* Do all the generations: */
  while ( generation<max_generations && complete==FALSE)
    {
    generation++;

/*
 * Migration Cycle.
 * 1) Migration that doesn't require inter-process communication.
 * 2) Migration that requires migration from 'even' processes.
 * 3) Migration that requires migration from 'odd' processes.
 * (Special case due to odd number of nodes is okay)
 */
    plog( LOG_VERBOSE, "*** Migration Cycle ***" );
    pop0_osize = pops[0]->size;
    for(island=1; island<num_pops; island++)
      {
      for(i=0; i<pops[island]->size; i++)
        {
        if (random_boolean_prob(pops[island]->migration_ratio))
	  {
          ga_entity_clone(pops[island-1], pops[island]->entity_iarray[i]);
/*	  printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), island, i, pops[island]->entity_iarray[i]->fitness);*/
	  }
        }
      }

    if (mpi_get_num_processes()<2)
      {	/* No parallel stuff initialized, or only 1 processor. */
      if (num_pops>1)
        { /* There is more than one island. */
        for(i=0; i<pop0_osize; i++)
          {
          if (random_boolean_prob(pops[0]->migration_ratio))
            {
            ga_entity_clone(pops[num_pops-1], pops[0]->entity_iarray[i]);
/*	    printf("%d, %d: Cloned %d %f\n", mpi_get_rank(), 0, i, pops[0]->entity_iarray[i]->fitness);*/
            }
	  }
	}
      }
    else
      {
      if (ISEVEN(mpi_get_rank()))
	{ /* Send then Recieve. */
	send_count = 0;
        for(i=0; i<pop0_osize; i++)
          {
          send_mask[i] = random_boolean_prob(pops[0]->migration_ratio);
	  send_count += send_mask[i];
/*	  if (send_mask[i]) printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[num_pops-1]->entity_iarray[i]->fitness);*/
          }

        ga_population_send_by_mask(pops[0], mpi_get_prev_rank(), send_count, send_mask);

        ga_population_append_receive(pops[num_pops-1], mpi_get_next_rank());
	}
      else
	{ /* Recieve then Send. */
        ga_population_append_receive(pops[num_pops-1], mpi_get_next_rank());

	send_count = 0;
	for(i=0; i<pop0_osize; i++)
          {
          send_mask[i] = random_boolean_prob(pops[0]->migration_ratio);
	  send_count += send_mask[i];
/*	  if (send_mask[i]) printf("%d, 0: Cloned %d %f\n", mpi_get_rank(), i, pops[num_pops-1]->entity_iarray[i]->fitness);*/
          }

        ga_population_send_by_mask(pops[0], mpi_get_prev_rank(), send_count, send_mask);
	}
      }

    for(island=0; island<num_pops; island++)
      {
      pop = pops[island];

      plog( LOG_VERBOSE, "*** Evolution on island %d ***", island );

/*
 * Sort the individuals in each population.
 * Need this to ensure that new immigrants are ranked correctly.
 * ga_population_score_and_sort(pop) is needed if scores may change during migration.
 */
      sort_population(pop);

      if (pop->generation_hook?pop->generation_hook(generation, pop):TRUE)
        {
        pop->orig_size = pop->size;

        plog( LOG_DEBUG,
              "Population %d size is %d at start of generation %d",
              island, pop->orig_size, generation );

/*
 * Crossover step.
 */
        gaul_crossover(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Mutation step.
 */
        gaul_mutation(pop);	/* FIXME: Need to pass island for messages. */

/*
 * Apply environmental adaptations, score entities, sort entities, etc.
 */
        gaul_adapt_and_evaluate(pop);

/*
 * Survival of the fittest.
 */
        gaul_survival(pop);

        }
      else
        {
        complete = TRUE;
        }
      }

    plog(LOG_VERBOSE,
          "After generation %d, population %d has fitness scores between %f and %f",
          generation,
          island,
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

    }	/* Generation loop. */

  /* Free the send_mask array. */
  s_free(send_mask);

  return generation;
#else
  plog(LOG_WARNING, "Attempt to use parallel function without compiled support.");

  return 0;
#endif
  }


/**********************************************************************
  ga_evolution_mp()
  synopsis:	Main genetic algorithm routine.  Performs GA-based
		optimisation on the given population.
		This is a generation-based GA.
		ga_genesis(), or equivalent, must be called prior to
		this function.
  parameters:
  return:
  last updated:	03 Feb 2003
 **********************************************************************/

int ga_evolution_mp(	population		*pop,
			const int		max_generations )
  {
#if HAVE_MPI
  int		generation=0;		/* Current generation number. */

/* Checks. */
  if (!pop) die("NULL pointer to population structure passed.");
  if (!pop->evaluate) die("Population's evaluation callback is undefined.");
  if (!pop->select_one) die("Population's asexual selection callback is undefined.");
  if (!pop->select_two) die("Population's sexual selection callback is undefined.");
  if (!pop->mutate) die("Population's mutation callback is undefined.");
  if (!pop->crossover) die("Population's crossover callback is undefined.");
  if (pop->scheme != GA_SCHEME_DARWIN && !pop->adapt) die("Population's adaption callback is undefined.");
  if (pop->size < 1) die("Population is empty (ga_genesis() or equivalent should be called).");

  plog(LOG_VERBOSE, "The evolution has begun!");

  pop->generation = 0;

/*
 * Rank zero process is master.  This handles evolution.  Other processes are slaves
 * which simply evaluate entities.
 */
  if (mpi_ismaster())
    {

/*
 * Score and sort the initial population members.
 */
    gaul_ensure_evaluations_mp(pop);
    sort_population(pop);

    plog( LOG_VERBOSE,
          "Prior to the first generation, population has fitness scores between %f and %f",
          pop->entity_iarray[0]->fitness,
          pop->entity_iarray[pop->size-1]->fitness );

/*
 * Do all the generations:
 *
 * Stop when (a) max_generations reached, or
 *           (b) "pop->generation_hook" returns FALSE.
 */
    while ( (pop->generation_hook?pop->generation_hook(generation, pop):TRUE) &&
             generation<max_generations )
      {
      generation++;
      pop->generation = generation;
      pop->orig_size = pop->size;

      plog(LOG_DEBUG,
              "Population size is %d at start of generation %d",
              pop->orig_size, generation );

/*
 * Crossover step.
 */
      gaul_crossover(pop);

/*
 * Mutation step.
 */
      gaul_mutation(pop);

/*
 * Apply environmental adaptations, score entities, sort entities, etc.
 */
      gaul_adapt_and_evaluate_mp(pop);

/*
 * Survival of the fittest.
 */
      gaul_survival_mp(pop);

/*
 * Use callback.
 */
      plog(LOG_VERBOSE,
           "After generation %d, population has fitness scores between %f and %f",
           generation,
           pop->entity_iarray[0]->fitness,
           pop->entity_iarray[pop->size-1]->fitness );

      }	/* Generation loop. */

/*
 * Synchronise the population structures held across the processors.
 */
    /*gaul_broadcast_population_mp(pop);*/
    ga_population_send_every(pop, -1);
    }
  else
    {
    gaul_evaluation_slave_mp(pop);
    }

  return generation;
#else
  plog(LOG_WARNING, "Attempt to use parallel function without compiled support.");

  return 0;
#endif
  }


