/*
 * header file for binary tree implementation.
 */

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

