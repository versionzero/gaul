/*
 * AVL trees.
 */

#ifndef AVLTREE_H_INCLUDED
#define AVLTREE_H_INCLUDED

#include "SAA_header.h"

#include <stdio.h>
#include <stdlib.h>

#include "memory_util.h"

/*
 * 
 */
#ifdef AVLTREE_KEY_TYPE
typedef AVLTREE_KEY_TYPE AVLKey;
#else
typedef unsigned long AVLKey;
#endif

typedef AVLKey	(*AVLKeyFunc)(constvpointer data);
typedef boolean	(*AVLTraverseFunc)(AVLKey key, vpointer data, vpointer userdata);
typedef boolean	(*AVLMatchFunc)(constvpointer data, vpointer userdata);
typedef int	(*AVLSearchFunc)(constvpointer data, vpointer userdata);
typedef void	(*AVLDestructorFunc)(vpointer data);

typedef struct AVLTree_t
  {
  struct AVLTreeNode_t  *root;  /* opaque from hereonin. */
  AVLKeyFunc            key_generate_func;
  } AVLTree;

/*
 * Prototypes.
 */

AVLTree		*avltree_new(AVLKeyFunc key_generate_func);
void		avltree_delete(AVLTree *tree);
void		avltree_destroy(AVLTree *tree, AVLDestructorFunc free_func);
boolean		avltree_insert(AVLTree *tree, vpointer data);
vpointer	avltree_remove(AVLTree *tree, vpointer data);
vpointer	avltree_remove_key(AVLTree *tree, AVLKey key);
vpointer	avltree_lookup(AVLTree *tree, vpointer data);
vpointer	avltree_lookup_key(AVLTree *tree, AVLKey key);
vpointer	avltree_ordered_search(AVLTree *tree,
                         AVLSearchFunc search_func, vpointer userdata);
vpointer	avltree_search(AVLTree *tree,
                         AVLMatchFunc search_func, vpointer userdata);
void		avltree_traverse(AVLTree *tree,
			 AVLTraverseFunc traverse_func, vpointer userdata);
int		avltree_height(AVLTree *tree);
int		avltree_num_nodes(AVLTree *tree);
void		avltree_diagnostics(void);

#ifndef AVLTREE_COMPILE_MAIN
boolean		avltree_test(void);
#endif

#endif /* AVLTREE_H_INCLUDED */
