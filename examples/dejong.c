/**********************************************************************
  dejong.c
 **********************************************************************

  dejong - The De Jong test suite's functions.
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

  Synopsis:	Collection of De Jong's 5 functions for use in GAUL's
		test/example programs.

		f1(x1,x2,x3) = x1*x1 + x2*x2 + x3*x3

		where -5.12<=xi<=5.12


		f2(x1,x2) = 100 * (x1*x1 - x2)^2 + (1 - x1)^2

		where -2.048<=xi<=2.048


		f3(x1,x2,x3,x4,x5) = 25 + floor(x1) + floor(x2)
                                   + floor(x3) + floor(x4) + floor(x5)

		where -5.12<=xi<=5.12

		           30
		          ___
		          \   
                f4(xi) =   > { i * xi^4 + Gauss(0,1) }
		          /__
		          i=1

		where -1.28<=xi<=1.28


		(Shekel's foxholes)

		              25                 1
		              ___    ---------------------------
		f5(x1,x2) =   \              2
		              /__           ___ 
		              j=1      j +  \   (x[i] - a[i][j])^6
		                            /__
		                            i=1

		where -65.536<=xi<=65.536

static int a[2][25] = {
  {-32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32        },
  {-32, -32, -32, -32, -32, -16, -16, -16, -16, -16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32  }
    };


  Last Updated:	05/06/01 SAA	Made results negative.
		04/06/01 SAA	Collected together for distribution.

 **********************************************************************/

#include "dejong.h"

/**********************************************************************
  dejong_function_one()
  synopsis:
  parameters:
  return:
  updated:	05/06/01
 **********************************************************************/

double dejong_function_one(double x1, double x2, double x3)
  {
  if (x1 < -5.12 || x1 > 5.12) die("x1 out of range.");
  if (x2 < -5.12 || x2 > 5.12) die("x2 out of range.");
  if (x3 < -5.12 || x3 > 5.12) die("x3 out of range.");

  return -(x1*x1 + x2*x2 + x3*x3);
  }


/**********************************************************************
  dejong_function_two()
  synopsis:
  parameters:
  return:
  updated:	05/06/01
 **********************************************************************/

double dejong_function_two(double x1, double x2)
  {
  int	x3, x4;

  if (x1 < -2.048 || x1 > 2.048) die("x1 out of range.");
  if (x2 < -2.048 || x2 > 2.048) die("x2 out of range.");

  x3 = x1*x1 - x2;
  x4 = 1.0 - x1;

  return -(100.0*x3*x3 + x4+x4);
  }


/**********************************************************************
  dejong_function_three()
  synopsis:
  parameters:
  return:
  updated:	05/06/01
 **********************************************************************/

double dejong_function_three(double x1, double x2, double x3, double x4, double x5)
  {

  if (x1 < -5.12 || x1 > 5.12) die("x1 out of range.");
  if (x2 < -5.12 || x2 > 5.12) die("x2 out of range.");
  if (x3 < -5.12 || x3 > 5.12) die("x3 out of range.");
  if (x4 < -5.12 || x4 > 5.12) die("x4 out of range.");
  if (x5 < -5.12 || x5 > 5.12) die("x5 out of range.");

  return -(25 + floor(x1) + floor(x2) + floor(x3) + floor(x4) + floor(x5));
  }


/**********************************************************************
  dejong_function_four()
  synopsis:
  parameters:
  return:
  updated:	05/06/01
 **********************************************************************/

double dejong_function_four(double x[30])
  {
  int		i;		/* Loop over the parameters. */
  double	score=0.0;	/* The result. */

  for (i=0; i<30; i++)
    {
    if (x[i] < -1.28 || x[i] > 1.28) dief("x[%d] out of range.", i);
    score += i * FOURTH_POW(x[i]) + random_unit_gaussian();
    }

  return -score;
  }


/**********************************************************************
  dejong_function_five()
  synopsis:
  parameters:
  return:
  updated:	05/06/01
 **********************************************************************/

double dejong_function_five(double x1, double x2)
  {
  int		i, j;		/* Loop over the parameters. */
  double	score=0.0;	/* The result. */
  static int	a[2][25] = { {-32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32, -32, -16, 0, 16, 32 }, {-32, -32, -32, -32, -32, -16, -16, -16, -16, -16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32 } };

  if (x1 < -65.536 || x1 > 65.536) die("x1 out of range.");
  if (x2 < -65.536 || x2 > 65.536) die("x2 out of range.");

  for (i=0; i<25; i++)
    {
    score += 1.0/(i + ipow(x1 - a[0][i],6) + ipow(x2 - a[1][i],6);
    }

  return -score;
  }




