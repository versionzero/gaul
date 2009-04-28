/**********************************************************************
  chromostubs.h
 **********************************************************************

  chromostubs - Stubs for chromosome handling routines.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
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

  Synopsis:     Stubs for the routines required by GAUL to handle
		user-defined chromosome types.  Note that the function
		names don't matter since they are just callback
		functions.  The prototypes do matter though.

 **********************************************************************/

#ifndef CHROMOSTUBS_H_INCLUDED
#define CHROMOSTUBS_H_INCLUDED

/*
 * Includes.
 */
#include "gaul.h"

/*
 * Prototypes.
 */
boolean chromosome_XXX_allocate(population *pop, entity *embryo);
void chromosome_XXX_deallocate(population *pop, entity *corpse);
void chromosome_XXX_replicate( population *pop,
                               entity *parent, entity *child,
                               const int chromosomeid );
unsigned int chromosome_XXX_to_bytes(population *pop, entity *joe,
                                     byte **bytes, unsigned int *max_bytes);
void chromosome_XXX_from_bytes(population *pop, entity *joe, byte *bytes);
char *chromosome_XXX_to_staticstring(population *pop, entity *joe);
boolean chromosome_XXX_seed(population *pop, entity *adam);
void chromosome_XXX_crossover( population *pop,
                               int *father, int *mother,
                               int *son, int *daughter );
void chromosome_XXX_mutate( population *pop,
                            entity *father, entity *son );

#endif /* CHROMOSTUBS_H_INCLUDED */
