/**********************************************************************
  mpi_util.h
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

  This program is didint_ibuted in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Header file for some abstract message passing functions
		using the MPI API.

  Updated:	19 Mar 2002 SAA	Moved some stuff from SAA_header.h to here.
		15 Mar 2002 SAA	Moved parallel-aware die()/dief() macros to here from SAA_header.h
		30 Jan 2002 SAA	Removed residual HelGA stuff.  mpi_datatype is not enum now.
		23 Jan 2002 SAA Removed all checkpointing support since that didn't work anyway.  Removed residual traces of population sending code.
		02/02/01 SAA	Converted from helga_mpi.h to mpi_util.h
		11/01/01 SAA	SAA_header.h includes the parallel library header files as necessary.
		10/04/00 SAA	First code.

 **********************************************************************/

#ifndef MPI_UTIL_H_INCLUDED
#define MPI_UTIL_H_INCLUDED

#include "SAA_header.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "compatibility.h"
#include "log_util.h"

/* PARALLEL library to use.
 * In most cases, we just wish to check whether PARALLEL > 0,
 * i.e. this is a parallel program.
 *
 * PARALLEL==0/undefined        None.
 * PARALLEL==1                  Pthreads.
 * PARALLEL==2                  MPI 1.2.x
 * PARALLEL==3                  PVM
 * PARALLEL==4                  BSP
 * PARALLEL==5                  OpenMP
 *
 * Transparently include MPI/whatever header in all files if this is parallel
 * code.
 * (This is required for the parallel version of die() and dief() macros -
 *  especially in the case of serial routines used by parallel programs.)
 */
#ifdef NO_PARALLEL
# undef PARALLEL
# define PARALLEL       0
#else
# ifndef PARALLEL
#  define PARALLEL      0
# else
#  if PARALLEL==1
#   include <pthread.h>
#   define _REENTRANT
#  endif
#  if PARALLEL==2
#   include <mpi.h>
#  endif
#  if PARALLEL==3
#   include <pvm3.h>
#  endif
#  if PARALLEL==4
#   include <bsp.h>
#  endif
#  if PARALLEL==5
#   include <omp.h>
#  endif
# endif
#endif

/*
 * Some constants.  Should be defined in config.h or equivalent.
 */
#ifndef MPI_NUM_THREADS_ENVVAR_STRING
#define MPI_NUM_THREADS_ENVVAR_STRING	"NUM_THREADS"
#endif

/*
 * Constants.
 */
#define MPI_TAG_ANY	-1
#define MPI_SOURCE_ANY	-1

/*
 * Message datatypes.
 */
#if PARALLEL == 0 || PARALLEL == 1 || PARALLEL == 2
typedef int mpi_datatype;
#define MPI_TYPE_UNKNOWN	0
#define MPI_TYPE_INT		1
#define MPI_TYPE_DOUBLE		2
#define MPI_TYPE_CHAR		3
#define MPI_TYPE_BYTE		4

#if 0
/* Use the MPI datatype definitions. */
typedef MPI_Datatype mpi_datatype;
#define MPI_TYPE_UNKNOWN	(MPI_Datatype) 0
#define MPI_TYPE_INT		(MPI_Datatype) MPI_INT
#define MPI_TYPE_DOUBLE		(MPI_Datatype) MPI_DOUBLE
#define MPI_TYPE_CHAR		(MPI_Datatype) MPI_CHAR
#define MPI_TYPE_BYTE		(MPI_Datatype) MPI_BYTE
#endif

#else
#if PARALLEL == 3 || PARALLEL == 4
/* For PVM and BSP, use size of the primitives. */
typedef size_t mpi_datatype;
#define MPI_TYPE_UNKNOWN	(size_t) 0
#define MPI_TYPE_INT		SIZEOF_INT
#define MPI_TYPE_DOUBLE		SIZEOF_DOUBLE
#define MPI_TYPE_CHAR		SIZEOF_CHAR
#define MPI_TYPE_BYTE		SIZEOF_BYTE

#endif
#endif

/*
 * Improved (parallel-aware) macros.
 */
#if PARALLEL==2
# undef die
# undef dief
# define die(X)          {					\
			int flubberrank;			\
			mpi_get_rank(&flubberrank);		\
                        printf(							\
		"FATAL ERROR: (process %d) %s\nin %s at \"%s\" line %d\n",	\
				flubberrank,				\
				(X),					\
                               __PRETTY_FUNCTION__,			\
                               __FILE__,				\
                               __LINE__);				\
                        mpi_abort(127);			\
			fflush(NULL);					\
                        }
# define dief(format, args...)	{				\
			int flubberrank;			\
			mpi_get_rank(&flubberrank);		\
			printf("FATAL ERROR: (process %d) ", flubberrank);	\
			printf(format, ##args);			\
			printf("\nin %s at \"%s\" line %d\n",	\
			__PRETTY_FUNCTION__,			\
			__FILE__,				\
			__LINE__);				\
			fflush(NULL);				\
                        mpi_abort(127);				\
			}
#endif

/*
 * Function prototypes
 */

boolean mpi_setup(int *argc, char ***argv, void *(*master_func)(void *), void *(*node_func)(void *));
boolean mpi_init(int *argc, char ***argv);
void mpi_exit(void);
void mpi_abort(int errcode);
boolean mpi_isinit();
boolean mpi_ismaster();
int mpi_get_num_processes();
int mpi_get_rank();
int mpi_get_next_rank();
int mpi_get_prev_rank();
boolean mpi_sync();
int mpi_find_global_max(const double local, double *global);
boolean mpi_synchronous_send(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               int tag);
boolean mpi_send(void *buf, const int count,
                            const mpi_datatype type, const int node,
                            int tag);
boolean mpi_broadcast(void *buf, const int count,
                            const mpi_datatype type,
                            int tag);
boolean mpi_distribute(void *buf, const int count,
                            const mpi_datatype type, const int root,
                            int tag);
boolean mpi_receive(void *buf, const int count,
                               const mpi_datatype type, const int node,
                               int tag);
#ifndef MPI_UTIL_COMPILE_MAIN
/*
boolean mpi_send_test(int node);
boolean mpi_recv_test();
boolean mpi_send_test_all();
boolean mpi_send_test_next();

boolean	mpi_test(void);
*/
#endif

#endif
