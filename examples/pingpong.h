/**********************************************************************
  pingpong.c
 **********************************************************************

  pingpong - Test/example program for GAUL.
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

		This program aims to solve a problem proposed in:
		Dennis E. Shasha, "Dr Ecco's Omniheurist Corner: Foxy",
		Dr Dobb's Journal, 323:148-149 (2001).

 **********************************************************************/

#ifndef PINGPONG_H_INCLUDED
#define PINGPONG_H_INCLUDED

/*
 * Includes
 */
#include "gaul.h"

/*
 * Prototypes.
 */
boolean pingpong_score(population *pop, entity *entity);
boolean pingpong_seed(population *pop, entity *adam);
void pingpong_crossover(population *pop, entity *mother, entity *father, entity *daughter, entity *son);
void pingpong_mutate(population *pop, entity *mother, entity *son);

#endif

