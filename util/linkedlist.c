/**********************************************************************
  linkedlist.c
 **********************************************************************

  linkedlist - Linked list implementation (singly- and doubly- linked).
  Copyright �2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:	Almost functionally equivalent to the glib list
		functions, based on the 1.2.6 documentation, although
		there are some differences.  This is a 'clean-room'
		implementation though, so I may claim copyright and
		therefore release the code under a license of my
		choice ;)
 
  To do:       Add sorting functions.
               Functions for inserting/appending lists etc. (i.e. slink_append_list() )
               Converting slists to dlists, and visa versa.
               Equivalent of avltree_destroy().
               ?link_unlink_data() etc like delete functions, except without freeing the element(s).

  To compile:	gcc linkedlist.c -DLINKEDLIST_COMPILE_MAIN -g -L . -lmethods

 **********************************************************************/

/*
 * Double-linked list functions are named dlink_*().
 * Single-linked list functions are named slink_*().
 */

#include "linkedlist.h"

#ifdef LINKEDLIST_COMPILE_MAIN
#include <stdio.h>
#include <string.h>
#endif


THREAD_LOCK_DEFINE_STATIC(slist_chunk);
THREAD_LOCK_DEFINE_STATIC(dlist_chunk);
static MemChunk *slist_chunk = NULL;
static MemChunk *dlist_chunk = NULL;


SLList *slink_new(void)
  {
  SLList *element;

  THREAD_LOCK(slist_chunk);
  if (!slist_chunk)
    slist_chunk = mem_chunk_new(sizeof(SLList), 512);

  element = mem_chunk_alloc(slist_chunk);
  THREAD_UNLOCK(slist_chunk);

  element->next = NULL;
  element->data = NULL;

  return element;
  }


void slink_free_all(SLList *list)
  {
  SLList	*element;

  THREAD_LOCK(slist_chunk);
  while (list)
    {
    element = list->next;
    mem_chunk_free(slist_chunk, list);
    list = element;
    }
  THREAD_UNLOCK(slist_chunk);

  return;
  }


void slink_free(SLList *list)
  {
  if (!list) return;

  THREAD_LOCK(slist_chunk);
  mem_chunk_free(slist_chunk, list);
  THREAD_UNLOCK(slist_chunk);

  return;
  }


SLList *slink_append(SLList *list, vpointer data)
  {
  SLList *new_element;
  SLList *last;

  new_element = slink_new();
  new_element->data = data;

  if (!list) return new_element;

  last = slink_last(list);
  last->next = new_element;

  return list;
  }


SLList *slink_prepend(SLList *list, vpointer data)
  {
  SLList *new_element;

  new_element = slink_new();
  new_element->data = data;
  new_element->next = list;

  return new_element;
  }


SLList *slink_insert_next(SLList *list, vpointer data)
  {
  SLList *new_element;
  SLList *next;

  new_element = slink_new();
  new_element->data = data;

  if (!list) return new_element;

  next = list->next;
  list->next = new_element;
  new_element->next = next;

  return list;
  }


SLList *slink_insert_index(SLList *list, vpointer data, int index)
  {
  SLList *prev_list;
  SLList *this_list;
  SLList *new_element;

  new_element = slink_new();
  new_element->data = data;

  if (!list) return new_element;

  prev_list = NULL;
  this_list = list;

  while ((index-- > 0) && this_list)
    {
    prev_list = this_list;
    this_list = this_list->next;
    }

  if (!prev_list)
    {
    new_element->next = list;
    return new_element;
    }

  new_element->next = prev_list->next;
  prev_list->next = new_element;

  return list;
  }


SLList *slink_delete_data(SLList *list, vpointer data)
  {
  SLList *tmp = list;
  SLList *prev = NULL;

  while (tmp)
    {
    if (tmp->data == data)
      {
      if (prev) prev->next = tmp->next;
      if (list == tmp) list = list->next;

      slink_free(tmp);

      return list;
      }

    prev = tmp;
    tmp = tmp->next;
    }

  return list;
  }


SLList *slink_delete_all_data(SLList *list, vpointer data)
  {
  SLList *tmp = list;
  SLList *prev = NULL;

  while (tmp)
    {
    if (tmp->data == data)
      {
      if (prev) prev->next = tmp->next;
      if (list == tmp) list = list->next;

      slink_free(tmp);
      }
    else
      {
      prev = tmp;
      tmp = tmp->next;
      }
    }

  return list;
  }


SLList *slink_delete_link(SLList *list, SLList *link)
  {
  SLList *tmp = list;
  SLList *prev = NULL;

  while (tmp)
    {
    if (tmp == link)
      {
      if (prev) prev->next = tmp->next;
      if (list == tmp) list = list->next;

      slink_free(tmp);

      return list;
      }

    prev = tmp;
    tmp = tmp->next;
    }

  return list;
  }


SLList *slink_clone(SLList *list)
  {
  SLList *new_element = NULL;
  SLList *last;

  if (!list) return NULL;

  new_element = slink_new();
  new_element->data = list->data;
  last = new_element;
  list = list->next;
  while (list)
    {
    last->next = slink_new();
    last = last->next;
    last->data = list->data;
    list = list->next;
    }

  return new_element;
  }


SLList *slink_reverse(SLList *list)
  {
  SLList *element;
  SLList *prev = NULL;
  SLList *last = NULL;

  while (list)
    {
    last = list;
    element = list->next;
    list->next = prev;
    prev = list;
    list = element;
    }

  return last;
  }


SLList *slink_nth(SLList *list, unsigned int index)
  {
  while ((index-- > 0) && list) list = list->next;

  return list;
  }


vpointer slink_nth_data(SLList *list, unsigned int index)
  {
  list = slink_nth(list, index);

  return list?list->data:NULL;
  }


SLList *slink_find(SLList *list, vpointer data)
  {
  while (list && list->data != data) list = list->next;

  return list;
  }


SLList *slink_find_custom(SLList *list, vpointer data, LLCompareFunc func)
  {
  if (!func) die("Null pointer to LLCompareFunc passed.");

  while (list && func(list->data, data)==FALSE)
    list = list->next;

  return list;
  }


int slink_index_link(SLList *list, SLList *link)
  {
  int i=0;

  while (list)
    {
    if (list == link) return i;
    i++;
    list = list->next;
    }

  return -1;
  }


int slink_index_data(SLList *list, vpointer data)
  {
  int i=0;

  while (list)
    {
    if (list->data == data) return i;
    i++;
    list = list->next;
    }

  return -1;
  }


SLList *slink_last(SLList *list)
  {
  if (!list) return NULL;

  while (list->next) list = list->next;
  
  return list;
  }


unsigned int slink_size(SLList *list)
  {
  unsigned int size=0;

  while (list)
    {
    size++;
    list = list->next;
    }

  return size;
  }


boolean slink_foreach(SLList *list, LLForeachFunc func, vpointer userdata)
  {

  if (!func) die("Null pointer to LLForeachFunc passed.");

  while (list)
    {
    if ((*func)(list->data, userdata)) return TRUE;
    list = list->next;
    }

  return FALSE;
  }


SLList *slink_insert_sorted(SLList *list, vpointer data, LLCompareFunc func)
  {
  SLList *prev_list;
  SLList *this_list;
  SLList *new_element;

  if (!func) die("Null pointer to LLCompareFunc passed.");

  new_element = slink_new();
  new_element->data = data;

  if (!list) return new_element;

  prev_list = NULL;
  this_list = list;

  while (this_list && func(this_list->data, data)<0)
    {
    prev_list = this_list;
    this_list = this_list->next;
    }

  if (!prev_list)
    {
    new_element->next = list;
    return new_element;
    }

  new_element->next = prev_list->next;
  prev_list->next = new_element;

  return list;
  }


DLList *dlink_new(void)
  {
  DLList *element;

  THREAD_LOCK(dlist_chunk);
  if (!dlist_chunk)
    dlist_chunk = mem_chunk_new(sizeof(DLList), 512);

  element = mem_chunk_alloc(dlist_chunk);
  THREAD_UNLOCK(dlist_chunk);

  element->prev = NULL;
  element->next = NULL;
  element->data = NULL;

  return element;
  }


void dlink_free_all(DLList *list)
  {
  DLList        *element;

  THREAD_LOCK(dlist_chunk);
  while (list->next)
    {
    element = list->next;
    mem_chunk_free(dlist_chunk, list);
    list = element;
    }
  while (list)
    {
    element = list->prev;
    mem_chunk_free(dlist_chunk, list);
    list = element;
    }
  THREAD_UNLOCK(dlist_chunk);

  return;
  }


void dlink_free(DLList *list)
  {
  if (!list) return;
  
  THREAD_LOCK(dlist_chunk);
  mem_chunk_free(dlist_chunk, list);
  THREAD_UNLOCK(dlist_chunk);

  return;
  }


DLList *dlink_append(DLList *list, vpointer data)
  {
  DLList *new_element;
  DLList *last;
  
  new_element = dlink_new();
  new_element->data = data;
  
  if (!list) return new_element;

  last = dlink_last(list);
  last->next = new_element;
  new_element->prev = last;

  return list;
  }


DLList *dlink_prepend(DLList *list, vpointer data)
  {
  DLList *new_element;
  
  new_element = dlink_new();
  new_element->data = data;
  
  if (!list) return new_element;

  if (list->prev)
    {
    list->prev->next = new_element;
    new_element->prev = list->prev;
    }

  list->prev = new_element;
  new_element->next = list;
  
  return new_element;
  }


DLList *dlink_insert_next(DLList *list, vpointer data)
  {
  DLList *new_element;
  DLList *next;

  new_element = dlink_new();
  new_element->data = data;

  if (!list) return new_element;

  next = list->next;
  if (next)
    {
    new_element->next = next;
    next->prev = new_element;
    }
  list->next = new_element;
  new_element->prev = list;

  return list;
  }


DLList *dlink_insert_prev(DLList *list, vpointer data)
  {
  DLList *new_element;
  DLList *prev;

  new_element = dlink_new();
  new_element->data = data;

  if (!list) return new_element;

  prev = list->prev;
  if (prev) 
    { 
    new_element->prev = prev;
    prev->next = new_element;
    }
  list->prev = new_element;
  new_element->next = list;

  return new_element;
  }


DLList *dlink_insert_index(DLList *list, vpointer data, int index)
  {
  DLList *new_element;
  DLList *this_list;
  
  if (index < 0)
    return dlink_append(list, data);
  else if (index == 0)
    return dlink_prepend(list, data);
  
  this_list = dlink_nth(list, index);
  if (!this_list)
    return dlink_append(list, data);
  
  new_element = dlink_new();
  new_element->data = data;
  
  if (this_list->prev)
    {
    this_list->prev->next = new_element;
    new_element->prev = this_list->prev;
    }
  new_element->next = this_list;
  this_list->prev = new_element;
  
  if (this_list == list)
    return new_element;
  else
    return list;
  }


DLList *dlink_delete_all_data(DLList *list, vpointer data)
  {            
  DLList *tmp=list;
 
  while (tmp)
    {
    if (tmp->data == data)
      {
      if (tmp->prev) tmp->prev->next = tmp->next;
      if (tmp->next) tmp->next->prev = tmp->prev;

      if (list == tmp) list = list->next;

      dlink_free(tmp);
      }

    tmp = tmp->next;
    }

  return list;
  }


DLList *dlink_delete_data(DLList *list, vpointer data)
  {
  DLList *tmp=list;
  
  while (tmp)
    {
    if (tmp->data == data)
      {
      if (tmp->prev) tmp->prev->next = tmp->next;
      if (tmp->next) tmp->next->prev = tmp->prev;
	  
      if (list == tmp) list = list->next;
	  
      dlink_free(tmp);
	  
      return list;
      }

    tmp = tmp->next;
    }

  return list;
  }


DLList *dlink_delete_link(DLList *list, DLList *link)
  {
  if (!link) return NULL;

  if (link->prev) link->prev->next = link->next;
  if (link->next) link->next->prev = link->prev;
      
  if (link == list) list = list->next;
      
  link->next = NULL;
  link->prev = NULL;
  
  return list;
  }


DLList *dlink_clone(DLList *list)
  {
  DLList *new_element = NULL;
  DLList *last;

  if (!list) return NULL;

  new_element = dlink_new();
  new_element->data = list->data;
  last = new_element;
  list = list->next;
  while (list)
    {
    last->next = dlink_new();
    last->next->prev = last;
    last = last->next;
    last->data = list->data;
    list = list->next;
    }

  return new_element;
  }


DLList *dlink_reverse(DLList *list)
  {
  DLList *last=NULL;
  
  while (list)
    {
    last = list;
    list = last->next;
    last->next = last->prev;
    last->prev = list;
    }
  
  return last;
  }


DLList *dlink_nth(DLList *list, unsigned int index)
  {
  while ((index-- > 0) && list) list = list->next;
  
  return list;
  }


DLList *dlink_pth(DLList *list, unsigned int index)
  {
  while ((index-- > 0) && list) list = list->prev;
  
  return list;
  }


vpointer dlink_nth_data(DLList *list, unsigned int index)
  {
  list = dlink_nth(list, index);
  
  return list?list->data:NULL;
  }


vpointer dlink_pth_data(DLList *list, unsigned int index)
  {
  list = dlink_pth(list, index);
  
  return list?list->data:NULL;
  }


DLList *dlink_find(DLList *list, vpointer data)
  {
  while (list && list->data != data) list = list->next;
  
  return list;
  }


DLList *dlink_find_custom(DLList *list, vpointer data, LLCompareFunc func)
  {
  if (!func) die("Null pointer to LLCompareFunc passed.");

  while (list && func(list->data, data)==FALSE)
    list = list->next;

  return list;
  }


int dlink_index_link(DLList *list, DLList *link)
  {
  int i=0;

  while (list)
    {
    if (list == link) return i;
    i++;
    list = list->next;
    }

  return -1;
  }


int dlink_index_data(DLList *list, vpointer data)
  {
  int i=0;

  while (list)
    {
    if (list->data == data) return i;
    i++;
    list = list->next;
    }

  return -1;
  }


DLList *dlink_last(DLList *list)
  {
  if (!list) return NULL;

  while (list->next) list = list->next;
  
  return list;
  }


DLList *dlink_first(DLList *list)
  {
  if (!list) return NULL;

  while (list->prev) list = list->prev;
  
  return list;
  }


unsigned int dlink_size(DLList *list)
  {
  unsigned int size=0;
  
  while (list)
    {
    size++;
    list = list->next;
    }
  
  return size;
  }


boolean dlink_foreach(DLList *list, LLForeachFunc func, vpointer userdata)
  {

  if (!func) die("Null pointer to LLForeachFunc passed.");

  while (list)
    {
    if ((*func)(list->data, userdata)) return TRUE;
    list = list->next;
    }

  return FALSE;
  }


boolean dlink_foreach_reverse(DLList *list,
                       LLForeachFunc func, vpointer userdata)
  {

  if (!func) die("Null pointer to LLForeachFunc passed.");

  while (list)
    {
    if ((*func)(list->data, userdata)) return TRUE;
    list = list->prev;
    }

  return FALSE;
  }


DLList *dlink_insert_sorted(DLList *list, vpointer data, LLCompareFunc func)
  {
  DLList *this_list;
  DLList *prev_list;
  DLList *new_element;

  if (!func) die("Null pointer to LLCompareFunc passed.");

  new_element = dlink_new();
  new_element->data = data;

  if (!list) return new_element;

  this_list = list;
  prev_list = NULL;

  while (this_list && func(this_list->data, data)<0)
    {
    prev_list = this_list;
    this_list = this_list->next;
    }

  new_element->next = this_list;
  new_element->prev = prev_list;

  if (this_list)
    {
    this_list->prev = new_element;
    if (!prev_list) return new_element;
    }

  prev_list->next = new_element;

  return list;
  }


/*
 * Testing:
 */

void linkedlist_diagnostics(void)
  {
  printf("=== Linked list diagnostics ==================================\n");
  printf("Version:           %s\n", VERSION_STRING);
  printf("Build date:        %s\n", BUILD_DATE_STRING);

  printf("--------------------------------------------------------------\n");
  printf("structure          sizeof\n");
#ifdef IRIX_MIPSPRO_SOURCE
  printf("SLList             %lu\n", (unsigned long int) sizeof(SLList));
  printf("DLList             %lu\n", (unsigned long int) sizeof(DLList));
#else
  printf("SLList             %Zd\n", sizeof(SLList));
  printf("DLList             %Zd\n", sizeof(DLList));
#endif
  printf("==============================================================\n");

  return;
  }



static boolean test_list_print(vpointer a, vpointer b)
  {
  int val = *((int*)a);
  printf("%d ", val);
  return FALSE;
  }


static int test_list_compare_one(constvpointer a, constvpointer b)
  {
  int one = *((const int*)a);
  int two = *((const int*)b);
  return one-two;
  }


static int test_list_compare_two(constvpointer a, constvpointer b)
  {
  int one = *((const int*)a);
  int two = *((const int*)b);
  return two-one;
  }


/*
 * This function is 'borrowed' from glib.
 * This shows that the glib-emulation works.
 */
#ifdef LINKEDLIST_COMPILE_MAIN
int main(int argc, char **argv)
#else
boolean linkedlist_test(void)
#endif
  {
  DLList *list, *t;
  SLList *slist, *st;
  int sorteddata[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  int data[10] = { 8, 9, 7, 0, 3, 2, 5, 1, 4, 6 };
  int i;

  printf("Checking doubly linked lists...\n");

  list = NULL;
  for (i = 0; i < 10; i++)
    list = g_list_append(list, &sorteddata[i]);
  list = g_list_reverse(list);

  for (i = 0; i < 10; i++)
    {
      t = g_list_nth (list, i);
      if (*((int*) t->data) != (9 - i))
	printf("Regular insert failed\n");
    }

  for (i = 0; i < 10; i++)
    if(g_list_position(list, g_list_nth (list, i)) != i)
      printf("g_list_position does not seem to be the inverse of g_list_nth\n");

  g_list_free(list);
  list = NULL;

  for (i = 0; i < 10; i++)
    list = g_list_insert_sorted(list, &data[i], test_list_compare_one);

  g_list_foreach(list, test_list_print, NULL);
  printf("\n");

  for (i = 0; i < 10; i++)
    {
      t = g_list_nth(list, i);
      if (*((int*) t->data) != i)
         printf("Sorted insert failed\n");
    }

  g_list_free(list);
  list = NULL;

  for (i = 0; i < 10; i++)
    list = g_list_insert_sorted(list, &data[i], test_list_compare_two);

  g_list_foreach(list, test_list_print, NULL);
  printf("\n");

  for (i = 0; i < 10; i++)
    {
      t = g_list_nth(list, i);
      if (*((int*) t->data) != (9 - i))
         printf("Sorted insert failed\n");
    }

  g_list_free(list);

  printf ("ok\n");

  printf ("Checking singly linked lists...\n");

  slist = NULL;
  for (i = 0; i < 10; i++)
    slist = g_slist_append(slist, &sorteddata[i]);
  slist = g_slist_reverse(slist);

  for (i = 0; i < 10; i++)
    {
      st = g_slist_nth(slist, i);
      if (*((int*) st->data) != (9 - i))
	printf ("failed\n");
    }

  g_slist_free(slist);
  slist = NULL;

  for (i = 0; i < 10; i++)
    slist = g_slist_insert_sorted(slist, &data[i], test_list_compare_one);

  g_slist_foreach(slist, test_list_print, NULL);
  printf("\n");

  for (i = 0; i < 10; i++)
    {
      st = g_slist_nth (slist, i);
      if (*((int*) st->data) != i)
         printf ("Sorted insert failed\n");
    }

  g_slist_free(slist);
  slist = NULL;

  for (i = 0; i < 10; i++)
    slist = g_slist_insert_sorted(slist, &data[i], test_list_compare_two);

  g_slist_foreach(slist, test_list_print, NULL);
  printf("\n");

  for (i = 0; i < 10; i++)
    {
      st = g_slist_nth(slist, i);
      if (*((int*) st->data) != (9 - i))
         printf("Sorted insert failed\n");
    }

  g_slist_free(slist);

  printf("ok\n");

#ifdef LINKEDLIST_COMPILE_MAIN
  exit(2);
#else
  return TRUE;
#endif
  }

