/********************************************************************
  str_match.h
 **********************************************************************

  str_match - String matching with wildcards
  Copyright ©1999-2003, Stewart Adcock <stewart@linux-domain.com>
  All rights reserved.

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

  Header file for my string matching routines

  Updated:	20/10/99 SAA	First version.

 **********************************************************************/

#ifndef STR_MATCH_INCLUDED
#define STR_MATCH_INCLUDED

#include "gaul_util.h"

#include "str_util.h"

/* Prototypes */
boolean	str_match(char *, char *);
boolean	recursive_str_match(char *, char *, char *, char *);
boolean	str_match_charmm(char *, char *);
boolean	recursive_str_match_charmm(char *, char *, char *, char *);

#endif

