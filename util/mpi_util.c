/**********************************************************************
  mpi_util.c
 **********************************************************************

  mpi_util - Generalised message passing.
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

  Synopsis:	Portable routines for message passing.  Currently only
		fully supports the MPI library but written such that
		MPI could be readily replaced by just about any other
                message passing API, hopefully.

		The number of threads/processors is determined at
		run-time.

		These functions can be tested by compiling with
		something like:
		mpicc -o testmpi mpi_util.c -DMPI_UTIL_TEST

  Updated:	03 Feb 2003 SAA	Start of extensive rewrite.
		03 Oct 2002 SAA	Tweaks to avoid warnings when using the Compaq C compiler.
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

#include "gaul/mpi_util.h"

#ifndef HAVE_MPI
/*
 * Define some stubs to enable MPI code to compile (but probably not run.)
 */
typedef int *MPI_Comm;
typedef int *MPI_Datatype;
typedef struct {
	int             MPI_SOURCE;
        int             MPI_TAG;
        int             MPI_ERROR;
  } MPI_Status;

int MPI_Init(int *a, char ***b) {return 0;}
int MPI_Comm_size(MPI_Comm a, int *b) {return 0;}
int MPI_Comm_rank(MPI_Comm a, int *b) {return 0;}
int MPI_Get_processor_name(char *a, int *b) {return 0;}
int MPI_Abort(MPI_Comm a, int b) {exit(b); return 0;}
int MPI_Barrier(MPI_Comm a) {return 0;}
int MPI_Finalize(void) {return 0;}
int MPI_Recv(void *a, int b, MPI_Datatype c, int d, int e, MPI_Comm f, MPI_Status *g) {return 0;}
int MPI_Send(void *a, int b, MPI_Datatype c, int d, int e, MPI_Comm f) {return 0;}
int MPI_SSend(void *a, int b, MPI_Datatype c, int d, int e, MPI_Comm f) {return 0;}
int MPI_Bcast(void *a, int b, MPI_Datatype c, int d, MPI_Comm e) {return 0;}
#else
# define MPI_TYPE(type)	((MPI_Datatype)((type)==MPI_TYPE_INT?MPI_INT:((type)==MPI_TYPE_DOUBLE?MPI_DOUBLE:((type)==MPI_TYPE_CHAR?MPI_CHAR:((type)==MPI_TYPE_BYTE?MPI_BYTE:(0))))))
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
#ifndef MPI_MAX_PROCESSOR_NAME
#define MPI_MAX_PROCESSOR_NAME	MAX_LINE_LEN
#endif
static char	node_name[MPI_MAX_PROCESSOR_NAME];	/* String containing processor name. */

/**********************************************************************
  mpi_setup()
  synopsis:	Initialise MPI stuff for master/slave paradigm.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_setup( int *argc, char ***argv,
                         void *(*master_func)(void *), void *(*node_func)(void *) )
  {

#ifdef HAVE_MPI
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

#endif

  return TRUE;
  }


/**********************************************************************
  mpi_init()
  synopsis:	Initialise MPI stuff for bulk paradigm.
  parameters:
  return:
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_init( int *argc, char ***argv )
  {

#ifdef HAVE_MPI
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
/*
 * Single-processor, single-thread version.
 */
  plog(LOG_VERBOSE, "Single-processor, single-thread version.");

  size = 1;
  rank = 0;
  if (gethostname(node_name, MAX_LINE_LEN)<0)
    strcpy(node_name, "<unknown>");

#endif

  return TRUE;
  }


/**********************************************************************
  mpi_exit()
  synopsis:	Abort all MPI processes.  This needn't be called by
		the user because setup_mpi() registers this
		function to be called on program exit.
  parameters:
  return:	none.
  last updated:	03 Feb 2003
 **********************************************************************/

void mpi_exit(void)
  {

#ifdef HAVE_MPI
  if ( mpi_isinit() )
    {
    if (rank > 0) _exit(EXIT_SUCCESS);
    }
  else
    {
    plog(LOG_DEBUG,
         "Request for closing MPI services prior to initialisation.");
    }

/* Only the master node will fall through... */

#endif

  return;
  }


/**********************************************************************
  mpi_abort()
  synopsis:	Abort all MPI processes.
  parameters:	int	errcode		The error code.
  return:	none
  last updated:	03 Feb 2003
 **********************************************************************/

void mpi_abort(int errcode)
  {

#ifdef HAVE_MPI
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

#endif

  return;	/* Should never get here. */
  }


/**********************************************************************
  mpi_isinit()
  synopsis:	Has MPI been initialized?  Appears faster than using
		the MPI_Initialized() function.
  parameters:
  return:	TRUE if master process, FALSE otherwise.
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_sync(void)
  {

#ifdef HAVE_MPI
  MPI_Barrier(MPI_COMM_WORLD);
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_find_global_max()
  synopsis:	Find the global maximum across all processors.
  parameters:
  return:	Node that maximum is on.  -1 on failure.
  last updated:	03 Feb 2003
 **********************************************************************/

int mpi_find_global_max(const double local, double *global)
  {
  int	maxnode;	/* Node from which maximum value belongs. */
#ifdef HAVE_MPI
  struct inout { double val; int node; } in, ex;	/* Transfer structs. */
#else
  *global = local;
  maxnode = rank;
#endif

#ifdef HAVE_MPI
  in.val = local;
  in.node = -1;

  MPI_Allreduce( &in, &ex, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD );

  *global = ex.val;
  maxnode = ex.node;

#endif

  plog(LOG_DEBUG, "Local %d/%f Global %d/%f.", local, rank, *global, maxnode);

  return maxnode;
  }


/**********************************************************************
  mpi_synchronous_send()
  synopsis:	Send data to another node using sychronous (blocking)
  		communication.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_synchronous_send(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               const int tag)
  {

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#ifdef HAVE_MPI
  MPI_Ssend( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD );
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_send()
  synopsis:	Send data to another node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_send(void *buf, const int count,
                            const mpi_datatype type, const int node,
                            const int tag)
  {

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#ifdef HAVE_MPI
  MPI_Send( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD );
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_broadcast()
  synopsis:	Send data to all nodes.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_broadcast(void *buf, const int count,
                            const mpi_datatype type,
                            const int tag)
  {
#ifdef HAVE_MPI
  int	i;
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");

#ifdef HAVE_MPI
  for (i=0; i<size; i++)
    {
    if (i!=rank)
      {
      MPI_Send( buf, count, MPI_TYPE(type), i, tag, MPI_COMM_WORLD );
      }
    }
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_distribute()
  synopsis:	Distribute data from one node to all other nodes.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_distribute(void *buf, const int count,
                            const mpi_datatype type, const int root,
                            const int tag)
  {
  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");

#ifdef HAVE_MPI
  /* FIXME: Tag is ignored. */
  MPI_Bcast( buf, count, MPI_TYPE(type), root, MPI_COMM_WORLD );
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_receive()
  synopsis:	Receive specific data from a a specific node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_receive(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               const int tag)
  {
#ifdef HAVE_MPI
  MPI_Status	status;		/* MPI status struct */
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed.");
  if (node==rank) die("Why should I send a message to myself?");

#ifdef HAVE_MPI
  MPI_Recv( buf, count, MPI_TYPE(type), node, tag, MPI_COMM_WORLD, &status );

  /* FIXME: Should check the status structure here! */
#endif

  return TRUE;
  }


/**********************************************************************
  mpi_receive_any()
  synopsis:	Receive any data (assuming correct type) from another
		any node.
  parameters:
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_receive_any(void *buf, const int count,
                    const mpi_datatype type,
		    int *node, int *tag)
  {
#ifdef HAVE_MPI
  MPI_Status	status;		/* MPI status struct */
#endif

  /* Checks */
  if (!buf) die("Null pointer to (void *) buffer passed");
  if (!node) die("Null pointer to node variable passed");
  if (!tag) die("Null pointer to tag variable passed");

#ifdef HAVE_MPI
  MPI_Recv( buf, count, MPI_TYPE(type), MPI_SOURCE_ANY, MPI_TAG_ANY, MPI_COMM_WORLD, &status );
  *node = status.MPI_SOURCE;
  *tag = status.MPI_TAG;
#endif

  return TRUE;
  }


#ifdef MPI_UTIL_COMPILE_MAIN
#ifdef HAVE_MPI
/**********************************************************************
  mpi_send_test()
  parameters:	The node to send test message to.  node = -1 for
		broadcast to all nodes.
  parameters:	The node to send test message to.
  return:	TRUE successful.  FALSE otherwise.
  last updated:	03 Feb 2003
 **********************************************************************/

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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
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
  last updated:	03 Feb 2003
 **********************************************************************/

boolean mpi_send_test_next(void)
  {
  int	next=rank+1;		/* The rank of the next process */

  if (next==size) next=0;	/* Last process sends to first process */
  return( mpi_send_test(next) );
  }

#endif


/**********************************************************************
  mpi_test()
  synopsis:	Test the MPI interface routines.
		This is the main() function if 'MPI_UTIL_COMPILE_MAIN'
		is defined.
  parameters:
  return:	A GSList, or NULL on failure.
  last updated:	03 Feb 2003
 **********************************************************************/

int main(int argc, char **argv)
  {
  mpi_setup(&argc, &argv);

#ifdef HAVE_MPI
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
#endif

  plog(LOG_DEBUG, "The end of process %d.", rank);

  exit(OKAY);
  }
#endif

