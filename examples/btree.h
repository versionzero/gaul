/**********************************************************************
  btree.h
 **********************************************************************

  btree - Banary Tree Implementation.
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

  Synopsis:	Header file for binary tree implementation.

 **********************************************************************/

#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include "SAA_header.h"

#include "memory_util.h"

/*
 * Type definitions.
 */

typedef int	(*BTreeCompareFunc)(constvpointer data1, constvpointer data2);
typedef boolean	(*BTreeForeachFunc)(vpointer data, vpointer userdata);
typedef void	(*BTreeDestructorFunc)(vpointer data);

typedef struct BTree_t
  {
  struct BTree_t *parent;	/* Parent node. */
  struct BTree_t *left;		/* Left branch. */
  struct BTree_t *right;	/* Right branch. */
  vpointer data;		/* Data stored in this node. */
  } BTree;


/*
 * Function prototypes.
 */

BTree *btree_new(void);
void btree_free(BTree *tree);
BTree *btree_root(BTree *tree);
BTree *btree_leftmost(BTree *tree);
BTree *btree_rightmost(BTree *tree);
void btree_delete(BTree *tree);
void btree_destroy(BTree *tree, BTreeDestructorFunc free_func);
void btree_delete_all(BTree *tree);
void btree_destroy_all(BTree *tree, BTreeDestructorFunc free_func);
BTree *btree_insert_left(BTree *tree, vpointer data);
BTree *btree_insert_right(BTree *tree, vpointer data);
BTree *btree_insert_parent(BTree *tree, vpointer data);
unsigned int btree_size(BTree *tree);
boolean btree_foreach(BTree *tree,
                        BTreeForeachFunc func, vpointer userdata);
boolean btree_foreach_reverse(BTree *tree,
                        BTreeForeachFunc func, vpointer userdata);
void btree_diagnostics(void);

#endif /* BTREE_H_INCLUDED */

