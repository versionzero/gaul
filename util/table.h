/**********************************************************************
  table.h
 **********************************************************************

  table - Table Data Structure.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

 **********************************************************************/

#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "SAA_header.h"

#include <limits.h>

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
#define TABLE_ERROR_INDEX	UINT_MAX

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

#endif

