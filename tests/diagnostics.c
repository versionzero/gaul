/**********************************************************************
  diagnostics.c
 **********************************************************************

  diagnostics - Generate some diagnostic output for GAUL.
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

  Synopsis:	Diagnostic checking for GAUL.  This produces some
		standard diagnostic output which is helpful for
		debugging and porting.

 **********************************************************************/

/*
 * Includes
 */
#include "gaul.h"

/**********************************************************************
  main()
  synopsis:	Produce some standard diagnostic output.
  parameters:
  return:
  updated:	25 Jul 2003
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
 * Usual initializations.
 */
  random_init();

/*
 * Output GAUL's diagnostic info.
 */
  printf("This program is linked against GAUL %d.%d-%d\n",
         ga_get_major_version(), ga_get_minor_version(), ga_get_patch_version());
  printf("This program was compiled with the GAUL %d.%d-%d headers.\n",
         GA_MAJOR_VERSION, GA_MINOR_VERSION, GA_PATCH_VERSION);

  ga_diagnostics();

  exit(EXIT_SUCCESS);
  }


