/**********************************************************************
  ga_search.h
 **********************************************************************

  ga_search - Non-evolutionary local search routines.
  Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:     Routines for local search and optimisation using
                non-evolutionary methods.

 **********************************************************************/

#ifndef GA_SEARCH_H_INCLUDED
#define GA_SEARCH_H_INCLUDED

/*
 * Includes
 */
#include "gaul.h"

/* debugging flags */
#define OPTIMIZERS_DEBUG	2

#define MaxQNIter     300

#define INITIAL_SD_STEP	0.1

/* relevant for linesearch code */
#define ALPHA   1.0e-4
#define BETA    0.9
#define LMMAX   16

/*
 * Prototypes.
 */

#endif /* GA_SEARCH_H_INCLUDED */


