/*
 * Table data structure. (basically a growable table)
 *
 * Thread-safe
 *
 * Updated:	27/02/01 SAA	gpointer replaced with vpointer and G_LOCK etc. replaced with THREAD_LOCK etc.
 *		21/02/01 SAA	Added table_count_items().
 *		22/01/01 SAA	Added table_lookup_index().
 *		18/01/01 SAA	Frist tidy version.
 *
 * To do:	Add table_compress().
 *
 * Compile test program with something like:
 * gcc table.c -DTABLE_COMPILE_MAIN `glib-config --cflags` -I..
 *             -DVERSION_STRING=NULL -lmethods -lglib -L.
 */

#include "table.h"


THREAD_LOCK_DEFINE_STATIC(table_mem_chunk);
static MemChunk *table_mem_chunk = NULL;

static int next_pow2(int num)
  {
  int	n = 1;

  while(n < num) n <<= 1;

  return n;
  }


static boolean table_ensure_size(TableStruct *table, int size)
  {
  int		i;

  if(table->size < size)
    {
    size = next_pow2(size);
    table->data = s_realloc(table->data, sizeof(vpointer)*size);
    table->unused = s_realloc(table->unused, sizeof(unsigned int)*size);
/* FIXME: Need checks here? */
    }

  for (i=table->size; i<size; i++) table->data[i]=NULL;

  return TRUE;
  }


TableStruct *table_new(void)
  {
  TableStruct *table;

  THREAD_LOCK(table_mem_chunk);
  if(!table_mem_chunk)
    table_mem_chunk = mem_chunk_new(sizeof(TableStruct), 64);

  table = mem_chunk_alloc(table_mem_chunk);
  THREAD_UNLOCK(table_mem_chunk);

  table->data = NULL;
  table->unused = NULL;
/*
  table->deallocate = NULL;
*/
  table->size = 0;
  table->numfree = 0;
  table->next = 0;

  return (TableStruct*) table;
  }


void table_destroy(TableStruct *table)
  {
/*
  if(table->deallocate)
    s_free(table->data);
*/
  if (table->data) s_free(table->data);
  if (table->unused) s_free(table->unused);

  THREAD_LOCK(table_mem_chunk);
  mem_chunk_free(table_mem_chunk, table);
  THREAD_UNLOCK(table_mem_chunk);

  return;
  }


boolean table_set_size(TableStruct *table, unsigned int size)
  {

  if(table->size < size)
    {
    return table_ensure_size(table, size);
    }
  else
    {
    printf("FIXME: Maybe need to shrink table if possible.");
    }

  return TRUE;
  }


/* Returns data removed, NULL otherwise. */
vpointer table_remove_index(TableStruct *table, unsigned int index)
  {
  vpointer data;

  if (!table) die("NULL pointer to TableStruct passed.");

  if (index < 0 || index >= table->next) die("Invalid index passed.");

  data = table->data[index];

/* Is this index used?  Do nothing if not. */
  if (data)
    {
    /* Add index to unused list. */
    table->unused[table->numfree]=index;
    table->numfree++;
    table->data[index] = NULL;
    }
  else
    {
    printf("WARNING: Trying to remove unused item.\n");
    }

  return data;
  }


/* Returns index of data removed, table->next otherwise. */
unsigned int table_remove_data(TableStruct *table, vpointer data)
  {
  unsigned int	index=0;

  if (!table) die("NULL pointer to TableStruct passed.");
  if (!data) die("NULL pointer to user data passed.");

  while ( table->data[index] != data && index < table->next) index++;

/* Was that data found in the table?  Do nothing if not. */
  if (index < table->next)
    {
    /* Add index to unused list. */
    table->unused[table->numfree]=index;
    table->numfree++;
    table->data[index] = NULL;
    }
  else
    {
    printf("WARNING: Trying to remove unused item.\n");
    }

  return index;
  }


vpointer table_get_data(TableStruct *table, unsigned int index)
  {
  if (!table) die("NULL pointer to TableStruct passed.");

  if (index < 0 || index >= table->next) dief("Invalid index (%d) passed.", index);

  return table->data[index];
  }


/* Returns the index for given data, or table->next on failure. */
unsigned int table_lookup_index(TableStruct *table, vpointer data)
  {
  unsigned int	index=0;

  if (!table) die("NULL pointer to TableStruct passed.");
  if (!data) die("NULL vpointer data passed.");

  while ( table->data[index] != data && index < table->next) index++;

  return index;
  }


unsigned int table_add(TableStruct *table, vpointer data)
  {
  unsigned int	index;

  if (!table) die("NULL pointer to TableStruct passed.");
  if (!data) die("NULL vpointer data passed.");

  if (table->numfree>0)
    {	/* Re-use some old indices. */
    table->numfree--;
    table->data[table->numfree]=data;
    return table->numfree;
    }

/* Must append to end of array. */
  index = table->next;
  table->next++;
  table_ensure_size(table, table->next);
  table->data[index] = data;

  return index;
  }


unsigned int table_count_items(TableStruct *table)
  {
  if (!table) die("NULL pointer to TableStruct passed.");

  return (table->next-table->numfree);
  }


void table_diagnostics(void)
  {
  printf("=== Table diagnostics ========================================\n");
  printf("Version:           %s\n", VERSION_STRING);
  printf("Build date:        %s\n", BUILD_DATE_STRING);

  printf("--------------------------------------------------------------\n");
  printf("structure          sizeof\n");
#ifdef IRIX_MIPSPRO_SOURCE
  printf("TableStruct        %lu\n", (unsigned long int) sizeof(TableStruct));
#else
  printf("TableStruct        %Zd\n", sizeof(TableStruct));
#endif
  printf("==============================================================\n");

  return;
  }


#ifdef TABLE_COMPILE_MAIN
int main(int argc, char *argv[])
#else
boolean table_test(void)
#endif
  {
/*
  int           i, j;
*/
  TableStruct	*table;

  printf("Testing my table routines.\n");
  printf("FIXME: Actually add some tests!\n");

  table = table_new();
  table_set_size(table, 200);

/*
vpointer table_remove_index(TableStruct *table, unsigned int index);
vpointer table_get_data(TableStruct *table, unsigned int index);
unsigned int table_add(TableStruct *table, vpointer data);
*/

  table_destroy(table);

#ifdef TABLE_COMPILE_MAIN
  exit(2);
#else
  return TRUE;
#endif
  }
