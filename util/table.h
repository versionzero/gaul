/*
 * Table data structure. (basically a growable table)
 */

#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "SAA_header.h"

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
#define table_error_index(X) (((TableStruct *)(X))->next)

/*
 * Prototypes.
 */
TableStruct	*table_new(void);
void		table_destroy(TableStruct *table);
boolean		table_set_size(TableStruct *table, unsigned int size);
vpointer	table_remove_index(TableStruct *table, unsigned int index);
unsigned int	table_remove_data(TableStruct *table, vpointer data);
vpointer	table_get_data(TableStruct *table, unsigned int index);
unsigned int	table_lookup_index(TableStruct *table, vpointer data);
unsigned int	table_add(TableStruct *table, vpointer data);
unsigned int	table_count_items(TableStruct *table);
void		table_diagnostics(void);

#ifndef TABLE_COMPILE_MAIN
boolean	table_test(void);
#endif

#endif

