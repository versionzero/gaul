/**********************************************************************
  mpi_util.c
 **********************************************************************

  mpi_util - Generalised message passing.
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

  Synopsis:	Originally Helga's portable routines for message
		passing.  Now generalised.  Currently only fully
                supports the MPI library but written such that MPI
                could be readily replaced by just about any other
                message passing API, hopefully.

		This offers only a tiny subset of the functions from
		the MPI specification.  There is no concept of
		'communicators'.  Message tags are observed, however.

		The wrapped library is set at compile time using the
		pre-processor constant 'PARALLEL' which is defined in
		SAA_header.h (or config.h, if apropriate).  The
		number of threads/processors is determined at run-time.
		Possible values are:

		PARALLEL==0/undefined	Sequential (i.e. just use stubs.)
		PARALLEL==1		Use Pthreads.
		PARALLEL==2		Use MPI (either MPICH or LAM, 1.2.x)
		PARALLEL==3		Use PVM (pvm3)
		PARALLEL==4		Use BSP
		PARALLEL==5		Use OpenMP

		These functions can be tested by compiling with
		something like:
		mpicc -o testmpi mpi_util.c -DMPI_UTIL_TEST

  Updated:	03 Oct 2002 SAA	Tweaks to avoid warnings when using the Compaq C compiler.
		11 Feb 2002 SAA	Sequential version of mpi_setup() now defines namelen.  Arguments for mpi_setup modified to match pthread_create().
		04 Feb 2002 SAA	All global variables are now decleared static.
		31 Jan 2002 SAA	mpi_standard_{send,broadcast,distribute}() renamed to mpi_{send,broadcast,distribute}().  Some debugging output removed.
		30 Jan 2002 SAA	mpi_init() function written as an alternative to mpi_setup(). mpi_dataype is not enum now.
  		23 Jan 2002 SAA	Removed all checkpointing support since that didn't work anyway.  Removed residual traces of population sending code.  mpi_message_tag may now be an arbitrary integer rather than an enumerated type.
		04/02/01 SAA	Code for basic BSPlib support.
		02/02/01 SAA	Removed from helga.  Use plog() instead of helga_log().
		12/01/01 SAA	helga_mpi_test now returns a boolean value (Although, at present this is always TRUE).
		11/01/01 SAA	For convenience in the pthread version, helga_setup_mpi() now takes the server and client functions as arguments.  _Lots_ of functions renamed for more consistent namespace.
		19/12/00 SAA	Added really simple skeleton support for PVM3 parallelization.  Added helga_find_global_max(), helga_standard_distribute(), helga_standard_broadcast().
		21/11/00 SAA	Using PARALLEL compilation constant to adapt these routines for single/parallel environments.  Only MPI based parallisation is currently implemented.  For somne reason helga_mpi_abort() returned a value, which seems a bit dumb - so I changed that.
		27/10/00 SAA	exit() values rationalised.
		22/09/00 SAA	Added helga_get_prev_rank().
		08/08/00 SAA	helga_exit_mpi() changed into a function that returns void.  It is also registered to be called automatically upon program termination.
		28/05/00 SAA	Use helga_log() where appropriate.
		06/05/00 SAA	helga_sync_mpi() and helga_exit_mpi() added.
		05/05/00 SAA	Test functions.
		27/04/00 SAA	First code.

  To do:	OpenMP?
		Sending derived datatypes.
		Complete threaded implementation.
		Complete PVM3 implementation.
		Complete BSP implementation.
		Checkpoint/resume.

 **********************************************************************/

#include "mpi_util.h"

/*
 * Type conversion macro for MPI.
 */
#if PARALLEL == 2
#define MPI_TYPE(type)	((MPI_Datatype)((type)==MPI_TYPE_INT?MPI_INT:((type)==MPI_TYPE_DOUBLE?MPI_DOUBLE:((type)==MPI_TYPE_CHAR?MPI_CHAR:((type)==MPI_TYPE_BYTE?MPI_BYTE:(0))))))
#endif

/*
 * Global variables.
 * Having this stuff stored removes the need for quite a number
 * of MPI calls.
 *
 * Size is used to inticate that MPI has been initialized.  Appears to be
 * much more rapid than using MPI_Initialized().  (Also isn't restricted
 * to MPI only).
 */
static int	rank=-1;				/* Current process's rank. */
static int	size=0;					/* Total number of processes. */
static int	namelen;				/* Length of processor name. */
#if PARALLEL == 2
static char	node_name[MPI_MAX_PROCESSOR_NAME];	/* String containing processor name. */
#else
static char	node_name[MAX_LINE_LEN];		/* String containing processor name. */
#endif

/**********************************************************************
  mpi_setup()
  synopsis:	Initialise MPI stuff for master/slave paradigm.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	11 Feb 2002
 **********************************************************************/

boolean mpi_setup( int *argc, char ***argv,
                         void *(*master_func)(void *), void *(*node_func)(void *) )
  {
#if PARALLEL == 0
/*
 * Single-processor, single-thread version.
 */
  plog(LOG_VERBOSE, "Single-processor, single-thread version.");

  size = 1;
  rank = 0;
  if (gethostname(node_name, MAX_LINE_LEN)<0)
    {
    strcpy(node_name, "<unknown>");
    }
  else
    {
    namelen = strlen(node_name);
    }

  master_func(NULL);

#else
#if PARALLEL == 1
/*
 * Single-processor, multi-threaded version. (Using pthreads)
 */
  char		*env_num_threads=NULL;
  int		i;	/* Loop over threads. */
  pthread_t	thread;	/* Pthread identifier. */

  /* Find the number of threads to spawn. */
  env_num_threads=getenv(MPI_NUM_THREADS_ENVVAR_STRING);

  plog(LOG_VERBOSE, "Single-processor, multi-threaded version. (Using pthreads)");

  if (env_num_threads)
    {
    size = atoi(env_num_threads);
    }
  else
    {
    size=1;
    plog(LOG_NORMAL, "Preparing 1 thread.  You may specify the number of threads required by defining the environment variable \"%s\".", MPI_NUM_THREADS_ENVVAR_STRING);
    }

  if (gethostname(node_name, MAX_LINE_LEN)<0)
    strcpy(node_name, "<unknown>");

  for (i=0; i<size; i++)
    {
    rank = i;
    if (pthread_create(&thread, NULL, node_func(), NULL))
      plog(LOG_FATAL, "Unable to start thread %d.", rank);
    }

  rank = 0;
  master_func(NULL);

  die("FIXME: PARALLEL == 1 case (pthreads) not fully implemented.");
#else
#if PARALLEL == 2
/*
 * Multi-processor version. (Using MPI)
 */
  plog(LOG_VERBOSE, "Multi-processor version. (Using MPI)");

/*
  plog(LOG_DEBUG, "MPI_Init() returned %d", MPI_Init(argc, argv));
*/
  MPI_Init(argc, argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(node_name, &namelen);

  plog(LOG_VERBOSE,
            "Process %d of %d initialized on %s",
            rank, size, node_name);

/*
 * Now wait for the other processors to be syncronised.
 */
  plog(LOG_DEBUG, "Process %d sync-ing", rank);
  mpi_sync();
  plog(LOG_DEBUG, "Process %d sync-ed", rank);

/* Register exit function. */
  atexit(&mpi_exit);

  if (rank==0)
    master_func(NULL);
  else
    node_func(NULL);

  plog(LOG_DEBUG, "Process %d has fallen through...", rank);

#else
#if PARALLEL == 3
/*
 * Multi-processor version. (Using PVM3)
 */
  plog(LOG_VERBOSE, "Multi-processor version. (Using PVM3)");

  die("FIXME: PARALLEL == 3 case (PVM) not implemented.");
#else
#if PARALLEL == 4
/*
 * Multi-processor version. (Using BSPlib)
 */
  char          *env_num_threads=NULL;
  unsigned int	real_size;

  /* Find the number of processes to spawn. */
  env_num_threads=getenv(MPI_NUM_PROCESSES_ENVVAR_STRING);

  if (env_num_threads)
    {
    size = atoi(env_num_threads);
    }
  else
    {
    size=1;
    plog(LOG_NORMAL, "Starting 1 processes.  You may specify the number of processes required by defining the environment variable \"%s\".", MPI_NUM_PROCESSES_ENVVAR_STRING);
    }

  if (gethostname(node_name, MAX_LINE_LEN)<0)
    strcpy(node_name, "<unknown>");

  plog(LOG_VERBOSE, "Multi-processor version. (Using BSP)");

  bsp_begin(size);

  realsize = bsp_nprocs();

  if (real_size != size)
    {
    plog(LOG_NORMAL, "%d processes requested, but only %s started.",
         size, real_size);
    size = real_size;
    }

  rank = bsp_pid();

  plog(LOG_VERBOSE,
            "Process %d of %d initialized on %s",
            rank, size, node_name);

/*
 * Now wait for the other processors to be syncronised.
 */
  plog(LOG_DEBUG, "Process %d sync-ing", rank);
  mpi_sync();
  plog(LOG_DEBUG, "Process %d sync-ed", rank);

/* Register exit function. */
  atexit(&mpi_exit);

  if (rank==0)
    master_func(NULL);
  else
    node_func(NULL);

  plog(LOG_DEBUG, "Process %d has fallen through...", rank);

#endif
#endif
#endif
#endif
#endif /* PARALLEL */

  return TRUE;
  }


/**********************************************************************
  mpi_init()
  synopsis:	Initialise MPI stuff for bulk paradigm.
  parameters:
  return:
  last updated:	30 Jan 2002
 **********************************************************************/

boolean mpi_init( int *argc, char ***argv )
  {
#if PARALLEL == 0
/*
 * Single-processor, single-thread version.
 */
  plog(LOG_VERBOSE, "Single-processor, single-thread version.");

  size = 1;
  rank = 0;
  if (gethostname(node_name, MAX_LINE_LEN)<0)
    strcpy(node_name, "<unknown>");

#else
#if PARALLEL == 1
  die("FIXME: PARALLEL == 1 case (pthreads) not implemented.");
#else
#if PARALLEL == 2
/*
 * Multi-processor version. (Using MPI)
 */
/*
  plog(LOG_VERBOSE, "Multi-processor version. (Using MPI)");
*/

/*
  plog(LOG_DEBUG, "MPI_Init() returned %d", MPI_Init(argc, argv));
*/
  MPI_Init(argc, argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(node_name, &namelen);

  plog(LOG_VERBOSE,
            "Process %d of %d initialized on %s",
            rank, size, node_name);

/*
 * Now wait for the other processors to be syncronised.
 */
  plog(LOG_DEBUG, "Process %d sync-ing", rank);
  mpi_sync();
  plog(LOG_DEBUG, "Process %d sync-ed", rank);

#else
#if PARALLEL == 3
/*
 * Multi-processor version. (Using PVM3)
 */
  plog(LOG_VERBOSE, "Multi-processor version. (Using PVM3)");

  die("FIXME: PARALLEL == 3 case (PVM) not implemented.");
#else
#if PARALLEL == 4
/*
 * Multi-processor version. (Using BSPlib)
 */
  char          *env_num_threads=NULL;
  unsigned int	real_size;

  /* Find the number of processes to spawn. */
  env_num_threads=getenv(MPI_NUM_PROCESSES_ENVVAR_STRING);

  if (env_num_threads)
    {
    size = atoi(env_num_threads);
    }
  else
    {
    size=1;
    plog(LOG_NORMAL, "Starting 1 processes.  You may specify the number of processes required by defining the environment variable \"%s\".", MPI_NUM_PROCESSES_ENVVAR_STRING);
    }

  if (gethostname(node_name, MAX_LINE_LEN)<0)
    strcpy(node_name, "<unknown>");

  plog(LOG_VERBOSE, "Multi-processor version. (Using BSP)");

  bsp_begin(size);

  realsize = bsp_nprocs();

  if (real_size != size)
    {
    plog(LOG_NORMAL, "%d processes requested, but only %s started.",
         size, real_size);
    size = real_size;
    }

  rank = bsp_pid();

  plog(LOG_VERBOSE,
            "Process %d of %d initialized on %s",
            rank, size, node_name);

/*
 * Now wait for the other processors to be syncronised.
 */
  plog(LOG_DEBUG, "Process %d sync-ing", rank);
  mpi_sync();
  plog(LOG_DEBUG, "Process %d sync-ed", rank);

#endif
#endif
#endif
#endif
#endif /* PARALLEL */

  return TRUE;
  }


/**********************************************************************
  mpi_exit()
  synopsis:	Abort all MPI processes.  This needn't be called by
		the user because setup_mpi() registers this
		function to be called on program exit.
  parameters:
  return:	none.
  last updated:	06/05/00
 **********************************************************************/

void mpi_exit(void)
  {

#if PARALLEL == 0
  /* Do nothing. */

#else
#if PARALLEL == 1
  if ( mpi_isinit() )
    {
    if (rank > 0) _exit();
    }
  else
    {
    plog(LOG_DEBUG,
         "Request for closing MPI services prior to initialisation.");
    }

/* Only the master node will fall through... */

#else
#if PARALLEL == 2
  if ( mpi_isinit() )
    {
    plog(LOG_DEBUG,
         "Process %d (%s) is closing MPI services.", rank, node_name);

    MPI_Finalize();
    }
  else
    {
    plog(LOG_DEBUG,
         "Request for closing MPI services prior to initialisation.");
    }
#else
#if PARALLEL == 3
/*
 * Multi-processor version. (Using PVM3)
 */
  die("FIXME: PARALLEL == 3 case (PVM) not implemented.");
#else
#if PARALLEL == 4
/*
 * Multi-processor version. (Using BSPlib)
 */
  bsp_end();
#endif
#endif
#endif
#endif
#endif

  return;
  }


/**********************************************************************
  mpi_abort()
  synopsis:	Abort all MPI processes.
  parameters:	int	errcode		The error code.
  return:	none
  last updated:	21/11/00
 **********************************************************************/

void mpi_abort(int errcode)
  {
#if PARALLEL == 0
  plog(LOG_DEBUG, "Forced abort due to error %d.", errcode);

  abort();
#else
#if PARALLEL == 1
  die("FIXME: PARALLEL == 1 case not implemented.");
#else
#if PARALLEL == 2
  if ( mpi_isinit() )
    {
    plog(LOG_DEBUG, "Process %d (%s) is forcing abort due to error %d.", rank, node_name, errcode);

    MPI_Abort(MPI_COMM_WORLD, errcode);
    }
  else
    {
    plog(LOG_DEBUG, "Aborting forced prior to MPI initialization.  errcode = %d",
         errcode);

    abort();
    }
#else
#if PARALLEL == 3
  die("FIXME: PARALLEL == 3 case not implemented.");
#else
#if PARALLEL == 4
  if ( mpi_isinit() )
    {
    plog(LOG_DEBUG, "Process %d (%s) is forcing abort due to error %d.",
         rank, node_name, errcode);

    bsp_end();
    }
  else
    {
    plog(LOG_DEBUG, "Aborting forced prior to MPI initialization.  errcode = %d",
         errcode);
    }

  abort();
#endif
#endif
#endif
#endif
#endif /* PARALLEL */

  return;	/* Should never get here. */
  }


/**********************************************************************
  mpi_isinit()
  synopsis:	Has MPI been initialized?  Appears faster than using
		the MPI_Initialized() function.
  parameters:
  return:	TRUE if master process, FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

boolean mpi_isinit(void)
  {
  return (size!=0);
  }


/**********************************************************************
  mpi_ismaster()
  synopsis:	Is this the master process?
  parameters:
  return:	TRUE if master process, FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

boolean mpi_ismaster(void)
  {
  return (rank==0);
  }


/**********************************************************************
  mpi_get_num_processes()
  synopsis:	Return the total number of MPI processes.
  parameters:
  return:	int	number of processes.
  last updated:	05/05/00
 **********************************************************************/

int mpi_get_num_processes(void)
  {
  return (size);
  }


/**********************************************************************
  mpi_get_rank()
  synopsis:	Return the rank of this process.
  parameters:
  return:	int	rank
  last updated:	05/05/00
**********************************************************************/

int mpi_get_rank(void)
  {
  return (rank);
  }


/**********************************************************************
  mpi_get_next_rank()
  synopsis:	Return the rank of the next node in a circular
		topology.
  parameters:
  return:	int	rank
  last updated:	05/05/00
 **********************************************************************/

int mpi_get_next_rank(void)
  {
  int	next=rank+1;		/* The rank of the next process */

  if (next==size) next=0;	/* Last process sends to first process */

  return (next);
  }


/**********************************************************************
  mpi_get_prev_rank()
  synopsis:	Return the rank of the previous node in a circular
		topology.
  parameters:
  return:	int	rank
  last updated:	22/09/00
 **********************************************************************/

int mpi_get_prev_rank(void)
  {
  int	prev=rank;		/* The rank of the previous process */

  if (prev==0) prev=size;	/* First process sends to last process */

  return (prev-1);
  }


/**********************************************************************
  mpi_sync()
  synopsis:	Syncronize all MPI processes.  Does nothing on scalar
		versions.
  parameters:
  return:
  last updated:	06/05/00
 **********************************************************************/

boolean mpi_sync(void)
  {

#if PARALLEL == 0
  /* Do nothing. */
#else
#if PARALLEL == 1
  die("pthread syncing code not implemented.");
#else
#if PARALLEL == 2
  MPI_Barrier(MPI_COMM_WORLD);
#else
#if PARALLEL == 3
  die("PVM syncing code not implemented.");
#else
#if PARALLEL == 4
  bsp_sync();
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_find_global_max()
  synopsis:	Find the global maximum across all processors.
  parameters:
  return:	Node that maximum is on.  -1 on failure.
  last updated:	06/05/00
 **********************************************************************/

int mpi_find_global_max(const double local, double *global)
  {
  int	maxnode;	/* Node from which maximum value belongs. */
#if PARALLEL == 2
  struct inout { double val; int node; } in, ex;	/* Transfer structs. */
#endif

#if PARALLEL == 0
  *global = local;
  maxnode = rank;
#else
#if PARALLEL == 1
  die("Pthread maxloc reduction not implemented");
#else
#if PARALLEL == 2
  in.val = local;
  in.node = -1;

  MPI_Allreduce( &in, &ex, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD );

  *global = ex.val;
  maxnode = ex.node;

#else
#if PARALLEL == 3
  die("PVM maxloc reduction not implemented");

#else
#if PARALLEL == 4
  die("BSP maxloc reduction not implemented");
#endif
#endif
#endif
#endif
#endif

  plog(LOG_DEBUG, "Local %d/%f Global %d/%f.", local, rank, *global, maxnode);

  return maxnode;
  }


#ifdef MPI_UTIL_COMPILE_MAIN
/**********************************************************************
  mpi_send_test()
  parameters:	The node to send test message to.  node = -1 for
		broadcast to all nodes.
  parameters:	The node to send test message to.
  return:	TRUE successful.  FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

#if PARALLEL == 2
boolean mpi_send_test(int node)
  {
  char	msg[255];	/* Message */
  int	i;		/* Loop over all processes */

  plog(LOG_NORMAL, "Test string from node %d to node %d.", rank, node);

  if (node == -1)
    {	/* Broadcast */
    for (i=0; i<size; i++)
      {
      if (i!=rank)
        {
        plog(LOG_DEBUG, "Sending test string to process %d.", i);
        MPI_Send( msg, 255, MPI_CHAR, i, HELGA_TAG_TEST, MPI_COMM_WORLD );
        }
      }
    }
  else
    {	/* Send to single node */
    MPI_Send( msg, 255, MPI_CHAR, node, HELGA_TAG_TEST, MPI_COMM_WORLD );
    }

  return TRUE;
  }


/**********************************************************************
  mpi_recv_test()
  parameters:	The node to send test message to.  node = -1 for
		broadcast to all nodes.
  parameters:	none
  return:	TRUE successful.  FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

boolean mpi_recv_test(void)
  {
  char		msg[255];	/* Message */
  MPI_Status	status;		/* MPI status struct */

  plog(LOG_DEBUG, "Node %d waiting for test message.", rank);

  MPI_Recv( msg, 255, MPI_CHAR, MPI_ANY_SOURCE, HELGA_TAG_TEST, MPI_COMM_WORLD, &status );

  plog(LOG_DEBUG, "Node %d received test message: \"%s\".", rank, msg);

  return TRUE;
  }


/**********************************************************************
  mpi_send_test_all()
  synopsis:	Send the test message to all nodes.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

boolean mpi_send_test_all(void)
  {
  return( mpi_send_test(-1) );
  }


/**********************************************************************
  mpi_send_test_next()
  synopsis:	Send the test message to the next node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	05/05/00
 **********************************************************************/

boolean mpi_send_test_next(void)
  {
  int	next=rank+1;		/* The rank of the next process */

  if (next==size) next=0;	/* Last process sends to first process */
  return( mpi_send_test(next) );
  }
#endif /* PARALLEL==2 */
#endif


/**********************************************************************
  mpi_synchronous_send()
  synopsis:	Send data to another node using sychronous (blocking)
  		communication.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	21/11/00
 **********************************************************************/

boolean mpi_synchronous_send(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               const int tag)
  {
  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to send an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Not implemented.");
#else
#if PARALLEL == 2
  MPI_Ssend( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD );
#else
#if PARALLEL == 3
  die("PVM send not implemented.");
#else
#if PARALLEL == 4
  bsp_put( node, buf, NULL, 0, count*type );
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }


#if 0
/**********************************************************************
  mpi_nonblocking_send()
  synopsis:	Send data to another node using buffered (non-blocking)
  		communication.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	23 Jan 2002
 **********************************************************************/

boolean mpi_nonblocking_send(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               const int tag)
  {
  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to send an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Not implemented.");
#else
#if PARALLEL == 2
  MPI_IBsend( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD );
#else
#if PARALLEL == 3
  die("PVM send not implemented.");
#else
#if PARALLEL == 4
  bsp_put( node, buf, NULL, 0, count*type );
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }
#endif


/**********************************************************************
  mpi_send()
  synopsis:	Send data to another node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	31 Jan 2002
 **********************************************************************/

boolean mpi_send(void *buf, const int count,
                            const mpi_datatype type, const int node,
                            const int tag)
  {
  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to send an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Pthreads version not implemented.");
#else
#if PARALLEL == 2
  MPI_Send( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD );
#else
#if PARALLEL == 3
  die("FIXME: PVM version not implemented.");
#else
#if PARALLEL == 4
  bsp_put( node, buf, NULL, 0, count*type );
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_broadcast()
  synopsis:	Send data to all nodes.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	19/12/00
 **********************************************************************/

boolean mpi_broadcast(void *buf, const int count,
                            const mpi_datatype type,
                            const int tag)
  {
#if PARALLEL == 2 || PARALLEL == 4
  int	i;
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to send an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Pthreads version not implemented.");
#else
#if PARALLEL == 2
  for (i=0; i<size; i++)
    {
    if (i!=rank)
      {
      MPI_Send( buf, count, MPI_TYPE(type), i, tag, MPI_COMM_WORLD );
      }
    }
#else
#if PARALLEL == 3
  die("FIXME: PVM version not implemented.");
#else
#if PARALLEL == 4
  for (i=0; i<size; i++)
    {
    if (i!=rank)
      {
      bsp_put( i, buf, NULL, 0, count*type );
      }
    }
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_distribute()
  synopsis:	Distribute data from one node to all other nodes.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	19/12/00
 **********************************************************************/

boolean mpi_distribute(void *buf, const int count,
                            const mpi_datatype type, const int root,
                            const int tag)
  {
  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to send an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Pthreads version not implemented.");
#else
#if PARALLEL == 2
  /* FIXME: Tag is ignored. */
  MPI_Bcast( buf, count, MPI_TYPE(type), root, MPI_COMM_WORLD );
#else
#if PARALLEL == 3
  die("FIXME: PVM version not implemented.");
#else
#if PARALLEL == 4
  die("FIXME: BSP version not implemented.");
#endif
#endif
#endif
#endif
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_receive()
  synopsis:	Receive specific data from a a specific node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	21/11/00
 **********************************************************************/

boolean mpi_receive(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               const int tag)
  {
#if PARALLEL == 2
  MPI_Status	status;		/* MPI status struct */
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to receive an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Not implemented.");
#else
#if PARALLEL == 2
  MPI_Recv( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD, &status );

  /* FIXME: Should check the status structure here! */
#else
#if PARALLEL == 3
  die("FIXME: PVM version not implemented.");
#else
#if PARALLEL == 4
  bsp_get( node, buf, 0, NULL, count*type );
#endif
#endif
#endif
#endif
#endif /* PARALLEL */

  return TRUE;
  }


/**********************************************************************
  mpi_receive_any()
  synopsis:	Receive any data (assuming correct type) from another
		any node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	23 Jan 2002
 **********************************************************************/

boolean mpi_receive_any(void *buf, const int count,
                    const mpi_datatype type,
		    int *node, int *tag)
  {
#if PARALLEL == 2
  MPI_Status	status;		/* MPI status struct */
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed");
  if (!node) die("Null pointer to node variable passed");
  if (!tag) die("Null pointer to tag variable passed");

#if PARALLEL == 0
  die("Uh oh.  Single processor, single threaded code trying to receive an interprocess message.");
#else
#if PARALLEL == 1
  die("FIXME: Not implemented.");
#else
#if PARALLEL == 2
  MPI_Recv( buf, count, MPI_TYPE(type), MPI_SOURCE_ANY, MPI_TAG_ANY, MPI_COMM_WORLD, &status );
  *node = status.MPI_SOURCE;
  *tag = status.MPI_TAG;

#else
#if PARALLEL == 3
  die("FIXME: PVM version not implemented.");
#else
#if PARALLEL == 4
  bsp_get( node, buf, 0, NULL, count*type );
#endif
#endif
#endif
#endif
#endif /* PARALLEL */

  return TRUE;
  }


/**********************************************************************
  mpi_test()
  synopsis:	Test the MPI interface routines.
		This is the main() function if 'MPI_UTIL_COMPILE_MAIN'
		is defined.
  parameters:
  return:	A GSList, or NULL on failure.
  last updated:	11/04/00
 **********************************************************************/

#ifdef MPI_UTIL_COMPILE_MAIN
int main(int argc, char **argv)
  {
  mpi_setup(&argc, &argv);

/*#else*/

/*boolean mpi_test(void)*/
/*  {*/
/*#endif*/

#if PARALLEL == 2
  if ( mpi_ismaster() )
    {
    mpi_send_test_all();
    }
  else
    {
    mpi_recv_test();
    }

  mpi_send_test_next();
  mpi_recv_test();
#endif /* PARALLEL == 2 */

  plog(LOG_DEBUG, "The end of process %d.", rank);

/*#ifdef MPI_UTIL_COMPILE_MAIN*/
  exit(OKAY);
  }
/*#else*/
/*  return TRUE;*/
/*  }*/
#endif
