/**********************************************************************
  mpi_util_fake.c
 **********************************************************************

  mpi_util_fake - mpi_util compilation kludge.
  Copyright ©2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:     This is a nasty kludge required for 'neat' compilation
		of mixed MPI and non-MPI code via automake and
		friends.

  Updated:      15 Mar 2002 SAA	Nasty hack implemented.

 **********************************************************************/

#include <stdlib.h>

#define NO_PARALLEL

#include "SAA_header.h"

/*
 * Forced over-ride of the PARALLEL constant.
 */
#undef PARALLEL
#define PARALLEL	0

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

#include "mpi_util.c"

