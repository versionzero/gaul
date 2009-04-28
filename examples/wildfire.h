/**********************************************************************
  wildfire.c
 **********************************************************************

  wildfire - Test/example program for GAUL.
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

 **********************************************************************/

#ifndef WILDFIRE_H_INCLUDED
#define WILDFIRE_H_INCLUDED

/*
 * Includes
 */
#include "gaul.h"

/*
 * Constants.
 */
#if 1
#define WILDFIRE_X_DIMENSION		80
#define WILDFIRE_Y_DIMENSION		40
#define WILDFIRE_CISTERNS		200
#define WILDFIRE_CREWS			3
#define WILDFIRE_FLASHPOINTS		5
#define WILDFIRE_WIND_CHANGE_PROB	0.8
#define WILDFIRE_WIND_SPEED		4
#define WILDFIRE_TIME			100
#define WILDFIRE_NUM_SIMULATIONS	25
#define WILDFIRE_PREDICTABLE_WIND	TRUE
#else
#define WILDFIRE_X_DIMENSION		50
#define WILDFIRE_Y_DIMENSION		25
#define WILDFIRE_CISTERNS		125
#define WILDFIRE_CREWS			3
#define WILDFIRE_FLASHPOINTS		4
#define WILDFIRE_WIND_CHANGE_PROB	1.0
#define WILDFIRE_WIND_SPEED		4
#define WILDFIRE_TIME			50
#define WILDFIRE_NUM_SIMULATIONS	10
#define WILDFIRE_PREDICTABLE_WIND	TRUE
#endif

/*
 * Prototypes.
 */
boolean wildfire_score(population *pop, entity *entity);
boolean wildfire_seed(population *pop, entity *adam);
void wildfire_crossover(population *pop, entity *mother, entity *father, entity *daughter, entity *son);
void wildfire_mutate(population *pop, entity *mother, entity *son);

#endif

