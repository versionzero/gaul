/**********************************************************************
  mpi_util.h
 **********************************************************************

  mpi_util - Generalised message passing and checkpointing functions.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Updated:	02/02/01 SAA	Converted from helga_mpi.h to mpi_util.h
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

#include "log_util.h"

/*
 * Some constants.  Should be defined in config.h or equivalent.
 */
#ifndef MPI_NUM_THREADS_ENVVAR_STRING
#define MPI_NUM_THREADS_ENVVAR_STRING	"NUM_THREADS"
#endif
#ifndef MPI_CHECKPOINT_ONSYNC
#define MPI_CHECKPOINT_ONSYNC	TRUE
#endif

/*
 * Message identification tags.
 * FIXME: Needs sorting out...
 */
enum mpi_message_tag {
  HELGA_TAG_NONE=0, HELGA_TAG_TEST,
  HELGA_TAG_IPARAM, HELGA_TAG_DPARAM, HELGA_TAG_SPARAM,
  HELGA_TAG_SEQUENCE,
  HELGA_TAG_MIGRATIONINFO, HELGA_TAG_MIGRATIONDATA,
  HELGA_TAG_BESTSYNC
  };

/*
 * Message datatypes.
 */
#if PARALLEL == 0 || PARALLEL == 1
enum mpi_datatype {
  MPI_TYPE_UNKNOWN=0,
  MPI_TYPE_INT, MPI_TYPE_DOUBLE, MPI_TYPE_CHAR
  };
#else
#if PARALLEL == 2
/* Use the MPI datatype definitions. */
enum mpi_datatype {
  MPI_TYPE_UNKNOWN=0,
  MPI_TYPE_INT=MPI_INT,
  MPI_TYPE_DOUBLE=MPI_DOUBLE,
  MPI_TYPE_CHAR=MPI_CHAR
  };
#else
#if PARALLEL == 3 || PARALLEL == 4
/* For PVM and BSP, use size of the primitives. */
enum mpi_datatype {
  MPI_TYPE_UNKNOWN=0,
  MPI_TYPE_INT=SIZEOF_INT,
  MPI_TYPE_DOUBLE=SIZEOF_DOUBLE,
  MPI_TYPE_CHAR=SIZEOF_CHAR
  };
#endif
#endif
#endif

/*
 * Function prototypes
 */

boolean mpi_setup(int *argc, char ***argv, void *(*master_func)(void *), void *(*node_func)(void *));
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
boolean mpi_send_test(int node);
boolean mpi_recv_test();
boolean mpi_send_test_all();
boolean mpi_send_test_next();
boolean mpi_send_population(int node);
boolean mpi_send_population_all();
boolean mpi_send_population_next();
boolean mpi_synchronous_send(const void *buf, const int count,
                               const enum mpi_datatype type, const int node,
                               const enum mpi_message_tag tag);
boolean mpi_standard_send(const void *buf, const int count,
                            const enum mpi_datatype type, const int node,
                            const enum mpi_message_tag tag);
boolean mpi_standard_broadcast(const void *buf, const int count,
                            const enum mpi_datatype type,
                            const enum mpi_message_tag tag);
boolean mpi_standard_distribute(const void *buf, const int count,
                            const enum mpi_datatype type, const int root,
                            const enum mpi_message_tag tag);
boolean mpi_receive(const void *buf, const int count,
                               const enum mpi_datatype type, const int node,
                               const enum mpi_message_tag tag);
#ifndef HELGA_MPI_COMPILE_MAIN
boolean	mpi_test(void);
#endif

#endif
