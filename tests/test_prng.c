/**********************************************************************
  test_prng.c
 **********************************************************************

  test_prng - Test GAUL pseudo-random number generator.
  Copyright ©2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test GAUL pseudo-random number generator.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/**********************************************************************
  main()
  synopsis:	Test GAUL's PRNG code.
  parameters:
  return:
  updated:	25 Jul 2003
 **********************************************************************/

int main(int argc, char **argv)
  {
  boolean	success;

/*
 * Usual initializations.
 */
  random_init();

/*
 * Run randomness test function.
 */
  success = random_test();

  if (success==FALSE)
    {
    printf("*** Routines DO NOT produce sufficiently random numbers.\n");
    }
  else
    {
    printf("Routines produce sufficiently random numbers.\n");
    }

  exit(EXIT_SUCCESS);
  }


