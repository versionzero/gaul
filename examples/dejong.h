/**********************************************************************
  dejong.h
 **********************************************************************

  dejong - The De Jong test suite's functions.
  Copyright ©2001-2003, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>
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

  Synopsis:	Collection of De Jong's 5 functions for use in GAUL's
		test/example programs.

  Last Updated:	04/06/01 SAA	Collected together for distribution.

 **********************************************************************/

#ifndef DEJONG_H_INCLUDED
#define DEJONG_H_INCLUDED

/*
 * Includes.
 */
#include "gaul.h"

/*
 * Prototypes.
 */
double	dejong_function_one(double x1, double x2, double x3);
double	dejong_function_two(double x1, double x2);
double	dejong_function_three(double x1, double x2, double x3, double x4, double x5);
double	dejong_function_four(double x[30]);
double	dejong_function_five(double x1, double x2);

#endif


