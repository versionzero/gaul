/**********************************************************************
  diagnostics.c
 **********************************************************************

  diagnostics - Diagnostic checking for GAUL.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:	Diagnostic checking for GAUL.

		This program run some simple diagnostic checks
		on the GAUL library.  It is currently very basic, but
		I anticipate that it will be extended over time.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/**********************************************************************
  main()
  synopsis:	Erm?
  parameters:
  return:
  updated:	22/07/01
 **********************************************************************/

int main(int argc, char **argv)
  {

/*
 * What machine type is this?
 */
  printf("Execution machine characteristics:\n");
#if W32_CRIPPLED != 1
  system("uname -a");
#else
  printf("Native MS Windows support.");
#endif

/*
 * Usual initilisations.
 */
  random_init();

/*
 * Output GAUL's diagnostic info.
 */
  ga_diagnostics();

/*
 * Run test functions.
 */
  random_test();
  avltree_test();
  table_test();
  linkedlist_test();

  exit(EXIT_SUCCESS);
  }


