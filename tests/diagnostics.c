/**********************************************************************
  diagnostics.c
 **********************************************************************

  diagnostics - Generate some diagnostic output for GAUL.
  Copyright ©2003-2009, Stewart Adcock (http://saa.dyndns.org/)
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
#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif
/*
 * Includes
 */
#include "gaul.h"

/*
 * Ensure that any that isn't defined yet gets set to '0'.
 */
#ifdef HAVE_STRINGS_H
#   define IS_HAVE_STRINGS_H     1
#else
#   define IS_HAVE_STRINGS_H     0
#endif
#ifdef HAVE_STRING_H
#   define IS_HAVE_STRING_H     1
#else
#   define IS_HAVE_STRING_H     0
#endif
#ifdef HAVE_STDBOOL_H
#   define IS_HAVE_STDBOOL_H    1
#else
#   define IS_HAVE_STDBOOL_H    0
#endif
#ifdef HAVE_STDINT_H
#   define IS_HAVE_STDINT_H     1
#else
#   define IS_HAVE_STDINT_H     0
#endif
#ifdef HAVE_STDLIB_H
#   define IS_HAVE_STDLIB_H     1
#else
#   define IS_HAVE_STDLIB_H     0
#endif

#ifdef HAVE_BCMP
#   define IS_HAVE_BCMP 1
#else
#   define IS_HAVE_BCMP 0
#endif
#ifdef HAVE_BCOPY
#   define IS_HAVE_BCOPY        1
#else
#   define IS_HAVE_BCOPY        0
#endif
#ifdef HAVE_GETHOSTNAME
#   define IS_HAVE_GETHOSTNAME  1
#else
#   define IS_HAVE_GETHOSTNAME  0
#endif
#ifdef HAVE_INDEX
#   define IS_HAVE_INDEX        1
#else
#   define IS_HAVE_INDEX        0
#endif
#ifdef HAVE_MAX
#   define IS_HAVE_MAX  1
#else
#   define IS_HAVE_MAX  0
#endif
#ifdef HAVE_MEMCPY
#   define IS_HAVE_MEMCPY       1
#else
#   define IS_HAVE_MEMCPY       0
#endif
#ifdef HAVE_MIN
#   define IS_HAVE_MIN  1
#else
#   define IS_HAVE_MIN  0
#endif
#ifdef HAVE_SINCOS
#   define IS_HAVE_SINCOS       1
#else
#   define IS_HAVE_SINCOS       0
#endif
#ifdef HAVE_SNOOZE
#   define IS_HAVE_SNOOZE       1
#else
#   define IS_HAVE_SNOOZE       0
#endif
#ifdef HAVE_STRCASECMP
#   define IS_HAVE_STRCASECMP   1
#else
#   define IS_HAVE_STRCASECMP   0
#endif
#ifdef HAVE_STRCMP
#   define IS_HAVE_STRCMP       1
#else
#   define IS_HAVE_STRCMP       0
#endif
#ifdef HAVE_STRCPY
#   define IS_HAVE_STRCPY       1
#else
#   define IS_HAVE_STRCPY       0
#endif
#ifdef HAVE_STRDUP
#   define IS_HAVE_STRDUP       1
#else
#   define IS_HAVE_STRDUP       0
#endif
#ifdef HAVE_STRLEN
#   define IS_HAVE_STRLEN       1
#else
#   define IS_HAVE_STRLEN       0
#endif
#ifdef HAVE_STRNCASECMP
#   define IS_HAVE_STRNCASECMP  1
#else
#   define IS_HAVE_STRNCASECMP  0
#endif
#ifdef HAVE_STRNCMP
#   define IS_HAVE_STRNCMP      1
#else
#   define IS_HAVE_STRNCMP      0
#endif
#ifdef HAVE_STRNCPY
#   define IS_HAVE_STRNCPY      1
#else
#   define IS_HAVE_STRNCPY      0
#endif
#ifdef HAVE_STRNDUP
#   define IS_HAVE_STRNDUP      1
#else
#   define IS_HAVE_STRNDUP      0
#endif
#ifdef HAVE_STRPBRK
#   define IS_HAVE_STRPBRK      1
#else
#   define IS_HAVE_STRPBRK      0
#endif
#ifdef HAVE_STRSEP
#   define IS_HAVE_STRSEP       1
#else
#   define IS_HAVE_STRSEP       0
#endif
#ifdef HAVE_USLEEP
#   define IS_HAVE_USLEEP       1
#else
#   define IS_HAVE_USLEEP       0
#endif
#ifdef HAVE_WAITPID
#   define IS_HAVE_WAITPID      1
#else
#   define IS_HAVE_WAITPID      0
#endif

#ifdef HAVE__BOOL
#   define IS_HAVE__BOOL        1
#else
#   define IS_HAVE__BOOL        0
#endif

#ifdef HAVE_MPI
#   define IS_HAVE_MPI  1
#else
#   define IS_HAVE_MPI  0
#endif
#ifdef HAVE_PTHREADS
#   define IS_HAVE_PTHREADS 1
#else
#   define IS_HAVE_PTHREADS 0
#endif
#ifdef HAVE_SLANG
#   define IS_HAVE_SLANG    1
#else
#   define IS_HAVE_SLANG    0
#endif

#ifdef W32_CRIPPLED
#   define IS_W32_CRIPPLED  1
#else
#   define IS_W32_CRIPPLED  0
#endif
#ifdef USE_CHROMO_CHUNKS
#   define IS_USE_CHROMO_CHUNKS 1
#else
#   define IS_USE_CHROMO_CHUNKS 0
#endif

/**********************************************************************
  main()
  synopsis:	Produce some standard diagnostic output.
  parameters:
  return:
  updated:	23 Sep 2003
 **********************************************************************/

int main(int argc, char **argv)
  {

/*
 * What machine type is this?
 */
  printf("Execution machine characteristics:\n");
#ifndef W32_CRIPPLED
  system("uname -a");
#else
  printf("Native MS Windows support.\n");
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

  printf("Configuration details:\n");

  printf("GA_BUILD_DATE_STRING:           %s\n", GA_BUILD_DATE_STRING);
  printf("GA_DEFAULT_NUM_PROCESSES:       %d\n", GA_DEFAULT_NUM_PROCESSES);
  printf("GA_DEFAULT_NUM_THREADS:         %d\n", GA_DEFAULT_NUM_THREADS);
  printf("GA_NUM_PROCESSES_ENVVAR_STRING: %s\n", GA_NUM_PROCESSES_ENVVAR_STRING);
  printf("GA_NUM_THREADS_ENVVAR_STRING:   %s\n", GA_NUM_THREADS_ENVVAR_STRING);
  printf("HAVE_STDBOOL_H:                 %s\n", IS_HAVE_STDBOOL_H?"true":"false");
  printf("HAVE_STDINT_H:                  %s\n", IS_HAVE_STDINT_H?"true":"false");
  printf("HAVE_STDLIB_H:                  %s\n", IS_HAVE_STDLIB_H?"true":"false");
  printf("HAVE_STRINGS_H:                 %s\n", IS_HAVE_STRINGS_H?"true":"false");
  printf("HAVE_STRING_H:                  %s\n", IS_HAVE_STRING_H?"true":"false");
  printf("HAVE_BCMP:                      %s\n", IS_HAVE_BCMP?"true":"false");
  printf("HAVE_BCOPY:                     %s\n", IS_HAVE_BCOPY?"true":"false");
  printf("HAVE_GETHOSTNAME:               %s\n", IS_HAVE_GETHOSTNAME?"true":"false");
  printf("HAVE_INDEX:                     %s\n", IS_HAVE_INDEX?"true":"false");
  printf("HAVE_MAX:                       %s\n", IS_HAVE_MAX?"true":"false");
  printf("HAVE_MEMCPY:                    %s\n", IS_HAVE_MEMCPY?"true":"false");
  printf("HAVE_MIN:                       %s\n", IS_HAVE_MIN?"true":"false");
  printf("HAVE_SINCOS:                    %s\n", IS_HAVE_SINCOS?"true":"false");
  printf("HAVE_SNOOZE:                    %s\n", IS_HAVE_SNOOZE?"true":"false");
  printf("HAVE_STRCASECMP:                %s\n", IS_HAVE_STRCASECMP?"true":"false");
  printf("HAVE_STRCMP:                    %s\n", IS_HAVE_STRCMP?"true":"false");
  printf("HAVE_STRCPY:                    %s\n", IS_HAVE_STRCPY?"true":"false");
  printf("HAVE_STRDUP:                    %s\n", IS_HAVE_STRDUP?"true":"false");
  printf("HAVE_STRLEN:                    %s\n", IS_HAVE_STRLEN?"true":"false");
  printf("HAVE_STRNCASECMP:               %s\n", IS_HAVE_STRNCASECMP?"true":"false");
  printf("HAVE_STRNCMP:                   %s\n", IS_HAVE_STRNCMP?"true":"false");
  printf("HAVE_STRNCPY:                   %s\n", IS_HAVE_STRNCPY?"true":"false");
  printf("HAVE_STRNDUP:                   %s\n", IS_HAVE_STRNDUP?"true":"false");
  printf("HAVE_STRPBRK:                   %s\n", IS_HAVE_STRPBRK?"true":"false");
  printf("HAVE_STRSEP:                    %s\n", IS_HAVE_STRSEP?"true":"false");
  printf("HAVE_USLEEP:                    %s\n", IS_HAVE_USLEEP?"true":"false");
  printf("HAVE_WAITPID:                   %s\n", IS_HAVE_WAITPID?"true":"false");
  printf("HAVE__BOOL:                     %s\n", IS_HAVE__BOOL?"true":"false");
  printf("HAVE_MPI:                       %s\n", IS_HAVE_MPI?"true":"false");
  printf("HAVE_PTHREADS:                  %s\n", IS_HAVE_PTHREADS?"true":"false");
  printf("HAVE_SLANG:                     %s\n", IS_HAVE_SLANG?"true":"false");
  printf("W32_CRIPPLED:                   %s\n", IS_W32_CRIPPLED?"true":"false");
  printf("USE_CHROMO_CHUNKS:              %s\n", IS_USE_CHROMO_CHUNKS?"true":"false");

  exit(EXIT_SUCCESS);
  }


