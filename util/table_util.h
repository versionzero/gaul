/**********************************************************************
  table_util.h
 **********************************************************************

  table_util - Data table routines.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Table data structure. (basically a growable table)

  Last Updated:	12 Jun 2002 SAA	Simple fix for clean compilation on QNX V6.
		20 mar 2002 SAA When limits.h is not available, compilation failed since UINT_MAX was not defined.
		16 Mar 2002 SAA	Check for presence of limits.h on this system.

 **********************************************************************/

#ifndef TABLE_UTIL_H_INCLUDED
#define TABLE_UTIL_H_INCLUDED

#include "gaul_util.h"

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include "memory_util.h"

typedef struct TableStruct_t
  {
  vpointer              *data;
  unsigned int          *unused;
/*
  (TableFreeFunc)       *deallocate;
*/
  unsigned int          size;
  unsigned int          numfree;
  unsigned int          next;
  } TableStruct;

/*
 * Convenience macro.
 */
/*#define table_error_index(X)	(((TableStruct *)(X))->next)*/

#ifdef UINT_MAX
/* UINT_MAX comes from limits.h */
# define TABLE_ERROR_INDEX	(unsigned int) UINT_MAX
#else
# define TABLE_ERROR_INDEX	(unsigned int) 0xFFFFFFFF      /* Maximum 32-bit unsigned int */
#endif

/*
 * Prototypes.
 */
TableStruct	*table_new(void);
void		table_destroy(TableStruct *table);
boolean		table_set_size(TableStruct *table, unsigned int size);
vpointer	table_remove_index(TableStruct *table, unsigned int index);
unsigned int	table_remove_data(TableStruct *table, vpointer data);
unsigned int	table_remove_data_all(TableStruct *table, vpointer data);
vpointer	table_get_data(TableStruct *table, unsigned int index);
vpointer	*table_get_data_all(TableStruct *table);
unsigned int	*table_get_index_all(TableStruct *table);
unsigned int	table_lookup_index(TableStruct *table, vpointer data);
unsigned int	table_add(TableStruct *table, vpointer data);
unsigned int	table_count_items(TableStruct *table);
void		table_diagnostics(void);

#ifndef TABLE_COMPILE_MAIN
boolean	table_test(void);
#endif

#endif /* TABLE_UTIL_H_INCLUDED */

