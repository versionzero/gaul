/**********************************************************************
  goldberg1.c
 **********************************************************************

  goldberg1 - Test/example program for GAUL.
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

  Synopsis:	Test/example program for GAUL.

		This program aims to solve the first example problem
		proposed in Goldberg's book.

  Last Updated:	31/05/01 SAA	First version.

 **********************************************************************/

#ifndef GOLDBERG1_H_INCLUDED
#define GOLDBERG1_H_INCLUDED

/*
 * Includes
 */
#include "gaul.h"

/*
 * Prototypes.
 */
boolean goldberg1_score(population *pop, entity *entity);

#endif

