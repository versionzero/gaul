/**********************************************************************
  str_hash.h
 **********************************************************************

  str_hash - Portable string hashing.
  Copyright ©2000, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Header file for string hashing routines.

  Updated:      18/09/00 SAA	New file.

 **********************************************************************/

#ifndef STR_HASH_INCLUDED
#define STR_HASH_INCLUDED

#include <limits.h>

#include "SAA_header.h"
#include "str_util.h"

typedef enum str_hash_type_e
  {
  STR_HASH_NULL = 0,
  STR_HASH_X33,		/* Daniel J. Bernstein's "Times 33" */
  STR_HASH_BJ,		/* Bob Jenkins, from Dr. Dobbs Journal */ 
  STR_HASH_CRC32,	/* Mark Adler's "Cyclic Redudancy Check 32" */
  STR_HASH_ELF,		/* ELF format hash */
  STR_HASH_GENERIC	/* Peter Weinberger's generic hash */	
  } str_hash_type;

/* Prototypes */
unsigned long	str_hash(const char *, size_t, str_hash_type);

#endif /* STR_HASH_INCLUDED */

