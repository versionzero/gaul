/**********************************************************************
  btree.c
 **********************************************************************

  btree - Binary Tree implementation.
  Copyright ©2001-2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:	Binary Tree implementation.
 
		Note: This is not a full implementation: just enough
		to serve as an example in GAUL.

  To do:	Add sorting functions.
 		Sorted insertion etc.
 		Converting to/from slists, dlists and avltrees.
 		Equivalent of avltree_destroy().

 **********************************************************************/

#include "btree.h"

THREAD_LOCK_DEFINE_STATIC(btree_chunk_lock);
static MemChunk *btree_chunk = NULL;

BTree *btree_new(void)
  {
  BTree *node;

  THREAD_LOCK(btree_chunk_lock);
  if (!btree_chunk)
    btree_chunk = mem_chunk_new(sizeof(BTree), 512);

  node = mem_chunk_alloc(btree_chunk);
  THREAD_UNLOCK(btree_chunk_lock);

  node->parent = NULL;
  node->left = NULL;
  node->right = NULL;
  node->data = NULL;

  return node;
  }


void btree_free(BTree *tree)
  {
  if (tree)
    {
    THREAD_LOCK(btree_chunk_lock);
    mem_chunk_free(btree_chunk, tree);
    THREAD_UNLOCK(btree_chunk_lock);
    }

  return;
  }


BTree *btree_root(BTree *tree)
  {
  if (tree)
    {
    while (tree->parent) tree = tree->parent;
    }

  return tree;
  }


BTree *btree_leftmost(BTree *tree)
  {
  if (tree)
    {
    while (tree->left) tree = tree->left;
    }

  return tree;
  }


BTree *btree_rightmost(BTree *tree)
  {
  if (tree)
    {
    while (tree->right) tree = tree->right;
    }

  return tree;
  }


void q_btree_delete(BTree *tree)
  {
  if (tree)
    {
    q_btree_delete(tree->left);
    q_btree_delete(tree->right);
    btree_free(tree);
    }

  return;
  }


void q_btree_destroy(BTree *tree, BTreeDestructorFunc free_func)
  {
  if (tree)
    {
    q_btree_delete(tree->left);
    q_btree_delete(tree->right);
    free_func(tree->data);
    btree_free(tree);
    }

  return;
  }


void btree_delete(BTree *tree)
  {
  if (tree)
    {
    q_btree_delete(tree->left);
    q_btree_delete(tree->right);
    btree_free(tree);

    if (tree->parent)
      {
      if (tree->parent->left == tree)
        tree->parent->left = NULL;
      else
        tree->parent->right = NULL;
      }
    }

  return;
  }


void btree_destroy(BTree *tree, BTreeDestructorFunc free_func)
  {
  if (tree)
    {
    q_btree_delete(tree->left);
    q_btree_delete(tree->right);
    free_func(tree->data);
    btree_free(tree);

    if (tree->parent)
      {
      if (tree->parent->left == tree)
        tree->parent->left = NULL;
      else
        tree->parent->right = NULL;
      }
    }

  return;
  }


void btree_delete_all(BTree *tree)
  {
  q_btree_delete(btree_root(tree));

  return;
  }


void btree_destroy_all(BTree *tree, BTreeDestructorFunc free_func)
  {
  q_btree_delete(btree_root(tree));

  return;
  }


BTree *btree_insert_left(BTree *tree, vpointer data)
  {
  BTree *new_node;
  BTree *left;

  new_node = btree_new();
  new_node->data = data;

  if (!tree) return new_node;

  left = tree->left;
  if (left)
    {
    new_node->left = left;
    left->parent = new_node;
    }
  tree->left = new_node;
  new_node->parent = tree;

  return tree;
  }


BTree *btree_insert_right(BTree *tree, vpointer data)
  {
  BTree *new_node;
  BTree *right;

  new_node = btree_new();
  new_node->data = data;

  if (!tree) return new_node;

  right = tree->right;
  if (right)
    {
    new_node->right = right;
    right->parent = new_node;
    }
  tree->right = new_node;
  new_node->parent = tree;

  return tree;
  }


BTree *btree_insert_parent(BTree *tree, vpointer data)
  {
  BTree *new_node;
  BTree *parent;

  new_node = btree_new();
  new_node->data = data;

  if (!tree) return new_node;

  parent = tree->parent;
  if (parent)
    {
    new_node->parent = parent;
    if (parent->left == tree)
      {
      parent->left=new_node;
      new_node->left = tree;
      }
    else
      {
      parent->right=new_node;
      new_node->right = tree;
      }
    }
  else
    {
    new_node->left = tree;
    }
  tree->parent = new_node;

  return tree;
  }


unsigned int btree_size(BTree *tree)
  {
  unsigned int size=0;
  
  while (tree)
    {
    size++;
    size += btree_size(tree->right);
    tree = tree->left;
    }
  
  return size;
  }


boolean btree_foreach(BTree *tree, BTreeForeachFunc func, vpointer userdata)
  {

  if (!func) die("Null pointer to LLForeachFunc passed.");

  while (tree)
    {
    if ((*func)(tree->data, userdata)) return TRUE;
    btree_foreach(tree->left, func, userdata);
    tree = tree->right;
    }

  return FALSE;
  }


boolean btree_foreach_reverse(BTree *tree,
                       BTreeForeachFunc func, vpointer userdata)
  {

  if (!func) die("Null pointer to BTreeForeachFunc passed.");

  while (tree)
    {
    if ((*func)(tree->data, userdata)) return TRUE;
    btree_foreach(tree->right, func, userdata);
    tree = tree->left;
    }

  return FALSE;
  }


/*
 * Testing:
 */

void btree_diagnostics(void)
  {
  printf("=== Binary Tree Diagnostics ==================================\n");
  printf("Version:           %s\n", VERSION_STRING);
  printf("Build date:        %s\n", BUILD_DATE_STRING);

  printf("--------------------------------------------------------------\n");
  printf("structure          sizeof\n");
#ifdef IRIX_MIPSPRO_SOURCE
  printf("BTree             %lu\n", (unsigned long int) sizeof(BTree));
#else
  printf("BTree             %Zd\n", sizeof(BTree));
#endif
  printf("==============================================================\n");

  return;
  }



