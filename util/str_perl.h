/**********************************************************************
  str_perl.h
 **********************************************************************

  str_perl - Perl regular expression stuff.
  Copyright ©2000, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  in incompatiable with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Header file for str_perl.c

  Updated:	18/09/00 SAA	First header file.

 **********************************************************************/

#ifndef STR_PERL_INCLUDED
#define STR_PERL_INCLUDED

#include "SAA_header.h"

#include "str_util.h"

/* prototypes */
int str_perlre(const char *, const char *, ...);

#endif
