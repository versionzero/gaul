/**********************************************************************
  memory_chunks.c
 **********************************************************************

  memory_chunks - Efficient bulk memory allocation.
  Copyright ©2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

 *
 * Mostly stolen from glib's gmem.c
 *
 * This code is part of memory_util.c - it's own integrated implementation
 * of memory chunks.	It may be used independantly if you feel brave.
 *
 * Last updated:	27/02/01 SAA	gpointer replaced with vpointer.
 *		18/01/01 SAA	Default compiler constant definition moved out to header file.
 *			17/01/01 SAA	Fixed the severe bug causing corruption of data when other data is freed - but I don't know how ;(
			16/01/01 SAA	Some simple padding support added.  Must be configured at compile-time.
 *			04/01/01 SAA	MemChunks with unreleasable atoms available, which doesn't use my AVLTree implementation added.
 *
 * This is thread safe.
 *
 * To do:	Padding for array under/overflow checking.
 *		Observe contents of atoms in the FreeAtom list.
 *
 * Known bugs:	High padding may be offset from the real end of the data - so some
 *		overflows will be missed.
 *
 */

#include "memory_chunks.h"

/* MEMORY_ALIGN_SIZE should have been set in config.h */

#define MEMORY_AREA_SIZE 4L

typedef struct FreeAtom_t
  {
  struct FreeAtom_t *next;
  } FreeAtom;

typedef struct MemArea_t
  {
  struct MemArea_t *next;	/* the next mem area */
  struct MemArea_t *prev;	/* the previous mem area */
#if 0
  unsigned long index;              /* the current index into the "mem" array */
  unsigned long free;               /* the number of free bytes in this mem area */
  unsigned long used;        /* the number of atoms allocated from this area */
  unsigned long mark;               /* is this mem area marked for deletion */
#endif
  size_t	index;		/* the current index into the "mem" array */
  size_t	free;		/* the number of free bytes in this mem area */
  unsigned int	used;		/* the number of atoms allocated from this area */
/* I assume the start of the 'mem' buffer is aligned sensibly. */
/* If not I will restore the four unsigned long's above... */
  unsigned char	mem[MEMORY_AREA_SIZE];  /* the mem array from which atoms get allocated
			      * the actual size of this array is determined by
			      *  the mem chunk "area_size". ANSI says that it
			      *  must be declared to be the maximum size it
			      *  can possibly be (even though the actual size
			      *  may be less).
			      */
  } MemArea;

struct MemChunk_t
  {
  unsigned int	num_mem_areas;		/* the total number of memory areas */
  unsigned int	num_unused_areas;	/* the number of areas that may be deallocated */
  size_t	atom_size;		/* the size of an atom */
  size_t	area_size;	/* the size of a memory area */
  MemArea	*mem_area;	/* the current memory area */
  MemArea	*mem_areas;	/* a list of all the mem areas owned by this chunk */
  MemArea	*free_mem_area;	/* the free area...which is about to be destroyed */
  FreeAtom	*free_atoms;	/* the free atoms list */
  AVLTree	*mem_tree;	/* tree of mem areas sorted by memory address */
  };


/*
 * AVLTree functions:
 */

static AVLKey mem_chunk_key_generate(constvpointer data)
  {
  return (AVLKey) ((MemArea *)data)->mem;
  }


static int mem_chunk_search_func(constvpointer data, vpointer userdata)
  {
  if (userdata < (vpointer) ((MemArea *)data)->mem)
    return -1;
  if (userdata > (vpointer) &(((MemArea *)data)->mem[((MemArea *)data)->index]) )
    return 1;
  return 0;
  }


/*
 * Padding functions:
 */
#if MEMORY_PADDING==TRUE
static unsigned char *pad_values="abcdefghijklmnopqr";

#define BUMP_DOWN(X)	( (void *) (((unsigned char *)(X))-MEMORY_ALIGN_SIZE) )
#define BUMP_UP(X)	( (void *) (((unsigned char *)(X))+MEMORY_ALIGN_SIZE) )

static void set_pad_low(MemChunk *mem_chunk, vpointer mem)
  {
  memcpy(((unsigned char *)mem),pad_values,MEMORY_ALIGN_SIZE);

  return;
  }


static void set_pad_high(MemChunk *mem_chunk, vpointer mem)
  {
  memcpy(((unsigned char *)mem)+mem_chunk->atom_size-MEMORY_ALIGN_SIZE,
            pad_values,MEMORY_ALIGN_SIZE);

  return;
  }


static int check_pad_low(MemChunk *mem_chunk, vpointer mem)
  {
  return memcmp(((unsigned char *)mem),pad_values,MEMORY_ALIGN_SIZE);
  }


static int check_pad_high(MemChunk *mem_chunk, vpointer mem)
  {
  return memcmp(((unsigned char *)mem)+mem_chunk->atom_size-MEMORY_ALIGN_SIZE,
                   pad_values,MEMORY_ALIGN_SIZE);
  }

#endif	/* MEMORY_PADDING==TRUE */


boolean mem_chunk_has_freeable_atoms(MemChunk *mem_chunk)
  {
  return mem_chunk->mem_tree?TRUE:FALSE;
  }


static MemChunk *_mem_chunk_new(size_t atom_size, unsigned int num_atoms)
  {
  MemChunk	*mem_chunk;
/*
  size_t	area_size;
*/

/*
 * Ensure that we don't misalign allocated memory for the user.
 * This also ensures that the minimum atom_size is okay for the
 * FreeAtom list.
 */
  if (atom_size % MEMORY_ALIGN_SIZE)
    {
    atom_size += MEMORY_ALIGN_SIZE - (atom_size % MEMORY_ALIGN_SIZE);
    printf("DEBUG: modified MemChunk atom size.\n");
    }
#if MEMORY_PADDING==TRUE
  atom_size += 2*MEMORY_ALIGN_SIZE;
#endif

/*
  area_size = (area_size + atom_size - 1) / atom_size;
  area_size *= atom_size;
*/

  if ( !(mem_chunk = (MemChunk *) malloc(sizeof(MemChunk))) )
    die("Unable to allocate memory.");

  mem_chunk->num_mem_areas = 0;
  mem_chunk->num_unused_areas = 0;
  mem_chunk->mem_area = NULL;
  mem_chunk->free_mem_area = NULL;
  mem_chunk->free_atoms = NULL;
  mem_chunk->mem_areas = NULL;
  mem_chunk->atom_size = atom_size;
  mem_chunk->area_size = atom_size*num_atoms;
  mem_chunk->mem_tree = NULL;
  
  return mem_chunk;
  }


/*
 * Constricted memory chunks: Atoms may not be individually released.
 */
MemChunk *mem_chunk_new_unfreeable(size_t atom_size, unsigned int num_atoms)
  {
  MemChunk	*mem_chunk;

  if (atom_size<1) die("Passed atom size is < 1 byte.");
  if (num_atoms<1) die("Passed number of atoms is < 1.");

  mem_chunk = _mem_chunk_new(atom_size, num_atoms);

  return mem_chunk;
  }


MemChunk *mem_chunk_new(size_t atom_size, unsigned int num_atoms)
  {
  MemChunk	*mem_chunk;

  if (atom_size<1) die("Passed atom size is < 1 byte.");
  if (num_atoms<1) die("Passed number of atoms is < 1.");

  mem_chunk = _mem_chunk_new(atom_size, num_atoms);
  mem_chunk->mem_tree = avltree_new((AVLKeyFunc) mem_chunk_key_generate);
  
  return mem_chunk;
  }


void mem_chunk_destroy(MemChunk *mem_chunk)
  {
  MemArea *mem_areas;
  MemArea *temp_area;
  
  if (!mem_chunk) die("Null pointer to mem_chunk passed.");

  mem_areas = mem_chunk->mem_areas;
  while (mem_areas)
    {
    temp_area = mem_areas;
    mem_areas = mem_areas->next;
    free(temp_area);
    }
  
  avltree_delete(mem_chunk->mem_tree);
  
  free(mem_chunk);

  return;
  }


#if 0
vpointer old_mem_chunk_alloc(MemChunk *mem_chunk)
  {
  MemArea	*temp_area;
  vpointer	mem;
  FreeAtom	*this=NULL;

  if (!mem_chunk) die("Null pointer to mem_chunk passed.");

/*
printf("Allocating from chunk %p\n", mem_chunk);
printf("mem_chunk->free_atoms = %p\n", mem_chunk->free_atoms);
*/
  
  if (mem_chunk->mem_tree)
    {	/* There may be a free atom available for re-use. */
    while (mem_chunk->free_atoms)
      {
      /* Get the first piece of memory on the "free_atoms" list.
       * We can go ahead and destroy the list node we used to keep
       *  track of it with and to update the "free_atoms" list to
       *  point to its next element.
       */
      mem = mem_chunk->free_atoms;
      mem_chunk->free_atoms = mem_chunk->free_atoms->next;
      
      /* Determine which area this piece of memory is allocated from. */
      temp_area = avltree_ordered_search(mem_chunk->mem_tree,
                          mem_chunk_search_func, mem);
      if (!temp_area) die("Unable to find temp_area");
      
      /* If the area isn't being used, it may be deallocated.
       *
       * We check to see if all of the segments on the free list that
       *  reference this area have been removed. This occurs when
       *  the amount of free memory is less than the allocatable size.
       * If the chunk should be freed, then we place it in the "free_mem_area".
       * This is so we make sure not to free the mem area here and then
       *  allocate it again a few lines down.
       * If we don't allocate a chunk a few lines down then the "free_mem_area"
       *  will be freed.
       * If there is already a "free_mem_area" then we'll just free this mem area.
       */
/*
      printf("temp_area->unused = %s\n", temp_area->unused?TRUE:FALSE);
*/

      if (temp_area->used==0)
        {
        if (temp_area == mem_chunk->mem_area)
          mem_chunk->mem_area = NULL;
	      
        if (mem_chunk->free_mem_area)
          {
          mem_chunk->num_mem_areas--;
		  
          if (temp_area->next)
            temp_area->next->prev = temp_area->prev;
          if (temp_area->prev)
            temp_area->prev->next = temp_area->next;
          if (temp_area == mem_chunk->mem_areas)
            mem_chunk->mem_areas = mem_chunk->mem_areas->next;
		  
          if (!avltree_remove(mem_chunk->mem_tree, temp_area)) die("Unable to find temp_area.");

          /* Loop through free atoms and forget any for this area. */
          this = mem_chunk->free_atoms;
          while (this)
            {
            if ( this->next >= (FreeAtom *) temp_area->mem &&
                 this->next < (FreeAtom *) &(temp_area->mem[temp_area->index]) )
              {
              this->next=this->next->next;
              }
            else
              {
              this=this->next;
              }
            }
          
          free(temp_area);
          }
        else
          {
          mem_chunk->free_mem_area = temp_area;
          }

        mem_chunk->num_unused_areas--;
        }
      else
        {
/*
 * Update the number of allocated atoms count.
 */
        temp_area->used++;
	  
/*
 * The area is used.  return this lump of memory.
 */
/*
printf("formerly freed mem\n");
*/
#if MEMORY_PADDING==TRUE
        set_pad_low(mem_chunk, mem);
        set_pad_high(mem_chunk, mem);
        if (check_pad_low(mem_chunk, mem)!=0) die("LOW MEMORY_PADDING ALREADY CORRUPT!");
        if (check_pad_high(mem_chunk, mem)!=0) die("HIGH MEMORY_PADDING ALREADY CORRUPT!");
        mem = BUMP_UP(mem);
#endif

printf("reused mem %p\n", mem);

        return mem;
        }
      }
    }
  
  /* If there isn't a current mem area or the current mem area is out of space
   *  then allocate a new mem area. We'll first check and see if we can use
   *  the "free_mem_area". Otherwise we'll just malloc the mem area.
   */
  if ((!mem_chunk->mem_area) ||
      ((mem_chunk->mem_area->index + mem_chunk->atom_size) > mem_chunk->area_size))
    {	/* A new MemArea must be allocated. */
/*
printf("Current mem_area out of space.\n");
*/
    if (mem_chunk->free_mem_area)
      {
      mem_chunk->mem_area = mem_chunk->free_mem_area;
      mem_chunk->free_mem_area = NULL;
      }
    else
      {
      mem_chunk->mem_area = (MemArea*) malloc(sizeof(MemArea)-MEMORY_AREA_SIZE+mem_chunk->area_size);

      if (!mem_chunk->mem_area) die("Unable to allocate memory.");
	  
      mem_chunk->num_mem_areas++;
      mem_chunk->mem_area->next = mem_chunk->mem_areas;
      mem_chunk->mem_area->prev = NULL;
	  
      if (mem_chunk->mem_areas)
        mem_chunk->mem_areas->prev = mem_chunk->mem_area;
      mem_chunk->mem_areas = mem_chunk->mem_area;
	  
/*
printf("Inserting new memory area %p size=%Zd (%Zd)\n", mem_chunk->mem_area, mem_chunk->area_size, (sizeof(MemArea)-MEMORY_AREA_SIZE+mem_chunk->area_size));
*/
      avltree_insert(mem_chunk->mem_tree, mem_chunk->mem_area);
      }
      
    mem_chunk->mem_area->index = 0;
    mem_chunk->mem_area->used = 0;
    }
  
  /* Get the memory and modify the state variables appropriately.
   */
  mem = (vpointer) &mem_chunk->mem_area->mem[mem_chunk->mem_area->index];
  mem_chunk->mem_area->index += mem_chunk->atom_size;
  mem_chunk->mem_area->used++;

#if MEMORY_PADDING==TRUE
  set_pad_low(mem_chunk, mem);
  set_pad_high(mem_chunk, mem);
  if (check_pad_low(mem_chunk, mem)!=0) die("LOW MEMORY_PADDING ALREADY CORRUPT!");
  if (check_pad_high(mem_chunk, mem)!=0) die("HIGH MEMORY_PADDING ALREADY CORRUPT!");
  mem = BUMP_UP(mem);
#endif

printf("new mem %p\n", mem);

  return mem;
  }
#endif


vpointer mem_chunk_alloc(MemChunk *mem_chunk)
  {
  MemArea *temp_area;
  vpointer mem;

  if (!mem_chunk) die("Null pointer to mem_chunk passed.");

  while (mem_chunk->free_atoms)
    {
      /* Get the first piece of memory on the "free_atoms" list.
       * We can go ahead and destroy the list node we used to keep
       *  track of it with and to update the "free_atoms" list to
       *  point to its next element.
       */
      mem = mem_chunk->free_atoms;
      mem_chunk->free_atoms = mem_chunk->free_atoms->next;

      /* Determine which area this piece of memory is allocated from */
      temp_area = avltree_ordered_search(mem_chunk->mem_tree,
                          mem_chunk_search_func, mem);

      /* If the area is unused, then it may be destroyed.
       * We check to see if all of the segments on the free list that
       *  reference this area have been removed. This occurs when
       *  the ammount of free memory is less than the allocatable size.
       * If the chunk should be freed, then we place it in the "free_mem_area".
       * This is so we make sure not to free the mem area here and then
       *  allocate it again a few lines down.
       * If we don't allocate a chunk a few lines down then the "free_mem_area"
       *  will be freed.
       * If there is already a "free_mem_area" then we'll just free this mem area.
       */
      if (temp_area->used==0)
        {
          /* Update the "free" memory available in that area */
          temp_area->free += mem_chunk->atom_size;

          if (temp_area->free == mem_chunk->area_size)
            {
              if (temp_area == mem_chunk->mem_area)
                mem_chunk->mem_area = NULL;

              if (mem_chunk->free_mem_area)
                {
mem_chunk->num_mem_areas--;

                  if (temp_area->next)
                    temp_area->next->prev = temp_area->prev;
                  if (temp_area->prev)
                    temp_area->prev->next = temp_area->next;
                  if (temp_area == mem_chunk->mem_areas)
                    mem_chunk->mem_areas = mem_chunk->mem_areas->next;

                  if (mem_chunk->mem_tree)
                    {
if (!avltree_remove(mem_chunk->mem_tree, temp_area)) die("Unable to find temp_area.");
                    }

                  free (temp_area);
                }
              else
                mem_chunk->free_mem_area = temp_area;

              mem_chunk->num_unused_areas--;
            }
        }
      else
        {
          /* Update the number of allocated atoms count.
           */
          temp_area->used++;

          /* The area is still in use...return the memory
           */
#if MEMORY_PADDING==TRUE
  set_pad_low(mem_chunk, mem);
  set_pad_high(mem_chunk, mem);
/*
  if (check_pad_low(mem_chunk, mem)!=0) die("LOW MEMORY_PADDING ALREADY CORRUPT!");
  if (check_pad_high(mem_chunk, mem)!=0) die("HIGH MEMORY_PADDING ALREADY CORRUPT!");
*/
  mem = BUMP_UP(mem);
#endif

        return mem;
        }
    }

  /* If there isn't a current mem area or the current mem area is out of space
   *  then allocate a new mem area. We'll first check and see if we can use
   *  the "free_mem_area". Otherwise we'll just malloc the mem area.
   */
  if ((!mem_chunk->mem_area) ||
      ((mem_chunk->mem_area->index + mem_chunk->atom_size) > mem_chunk->area_size))
    {
      if (mem_chunk->free_mem_area)
        {
          mem_chunk->mem_area = mem_chunk->free_mem_area;
          mem_chunk->free_mem_area = NULL;
        }
      else
        {
          mem_chunk->mem_area = (MemArea*) malloc(sizeof(MemArea) -
                                                       MEMORY_AREA_SIZE +
                                                       mem_chunk->area_size);

          mem_chunk->num_mem_areas++;
          mem_chunk->mem_area->next = mem_chunk->mem_areas;
          mem_chunk->mem_area->prev = NULL;

          if (mem_chunk->mem_areas)
            mem_chunk->mem_areas->prev = mem_chunk->mem_area;
          mem_chunk->mem_areas = mem_chunk->mem_area;

          if (mem_chunk->mem_tree)
            avltree_insert(mem_chunk->mem_tree, mem_chunk->mem_area);
        }

      mem_chunk->mem_area->index = 0;
      mem_chunk->mem_area->free = mem_chunk->area_size;
      mem_chunk->mem_area->used = 0;
    }

/*
 * Get the memory and modify the state variables appropriately.
 */
  mem = (vpointer) &mem_chunk->mem_area->mem[mem_chunk->mem_area->index];
  mem_chunk->mem_area->index += mem_chunk->atom_size;
  mem_chunk->mem_area->free -= mem_chunk->atom_size;
  mem_chunk->mem_area->used++;

#if MEMORY_PADDING==TRUE
  set_pad_low(mem_chunk, mem);
  set_pad_high(mem_chunk, mem);
/*
  if (check_pad_low(mem_chunk, mem)!=0) die("LOW MEMORY_PADDING ALREADY CORRUPT!");
  if (check_pad_high(mem_chunk, mem)!=0) die("HIGH MEMORY_PADDING ALREADY CORRUPT!");
*/
  mem = BUMP_UP(mem);
#endif

  return mem;
  }


void mem_chunk_free(MemChunk *mem_chunk, vpointer mem)
  {
  MemArea *temp_area;
  FreeAtom *free_atom;

  if (!mem_chunk) die("Null pointer to mem_chunk passed.");
  if (!mem_chunk->mem_tree) die("MemChunk passed has no freeable atoms.");
  if (!mem) die("NULL pointer passed.");

#if MEMORY_PADDING==TRUE
  mem = BUMP_DOWN(mem);
  if (check_pad_low(mem_chunk, mem)!=0)
    dief("LOW MEMORY_PADDING CORRUPT! (%*s)", MEMORY_ALIGN_SIZE, (unsigned char *)mem);
  if (check_pad_high(mem_chunk, mem)!=0)
    dief("HIGH MEMORY_PADDING CORRUPT!(%*s)", MEMORY_ALIGN_SIZE, (unsigned char *)mem);
#endif

/*
 * Place the memory on the "free_atoms" list.
 */
  free_atom = (FreeAtom*) mem;
  free_atom->next = mem_chunk->free_atoms;
  mem_chunk->free_atoms = free_atom;

  if (!(temp_area = avltree_ordered_search(mem_chunk->mem_tree, mem_chunk_search_func, mem)) )
    die("Unable to find temp_area.");

  temp_area->used--;

  if (temp_area->used == 0)
    {
    mem_chunk->num_unused_areas++;
    }

  return;
  }


#if 0
/* This doesn't free the free_area if there is one */
void old_mem_chunk_clean(MemChunk *mem_chunk)
  {
  MemArea	*mem_area;
  FreeAtom	*prev_free_atom;
  FreeAtom	*temp_free_atom;
  vpointer	mem;
  
  if (!mem_chunk) die("Null pointer to mem_chunk passed.");
  if (!mem_chunk->mem_tree) die("MemChunk passed has no freeable atoms.");
  
  prev_free_atom = NULL;
  temp_free_atom = mem_chunk->free_atoms;
      
  while (temp_free_atom)
    {
    mem = (vpointer) temp_free_atom;
	  
    if (!(mem_area = avltree_ordered_search(mem_chunk->mem_tree, mem_chunk_search_func, mem)) ) die("mem_area not found.");
	  
/*
 * If this mem area is unused so we may deallocate the entire
 *  area and list node and decrement the free mem.
 */
    if (mem_area->used==0)
      {
      if (prev_free_atom)
        prev_free_atom->next = temp_free_atom->next;
      else
        mem_chunk->free_atoms = temp_free_atom->next;
      temp_free_atom = temp_free_atom->next;
	      
      mem_chunk->num_mem_areas--;
      mem_chunk->num_unused_areas--;
		  
      if (mem_area->next)
        mem_area->next->prev = mem_area->prev;
      if (mem_area->prev)
        mem_area->prev->next = mem_area->next;
      if (mem_area == mem_chunk->mem_areas)
        mem_chunk->mem_areas = mem_chunk->mem_areas->next;
      if (mem_area == mem_chunk->mem_area)
        mem_chunk->mem_area = NULL;
		  
      if (!avltree_remove_key(mem_chunk->mem_tree, (AVLKey) mem_area))
        die("mem_area not found.");

      free(mem_area);
      }
    else
      {
      prev_free_atom = temp_free_atom;
      temp_free_atom = temp_free_atom->next;
      }
    }

  return;
  }
#endif


/* This doesn't free the free_area if there is one */
void mem_chunk_clean(MemChunk *mem_chunk)
  {
  MemArea *mem_area;
  FreeAtom *prev_free_atom;
  FreeAtom *temp_free_atom;
  vpointer mem;

  if (!mem_chunk) die("Null pointer to mem_chunk passed.");
  if (!mem_chunk->mem_tree) die("MemChunk passed has no freeable atoms.");

  if (mem_chunk->mem_tree)
    {
    prev_free_atom = NULL;
    temp_free_atom = mem_chunk->free_atoms;

    while (temp_free_atom)
      {
      mem = (vpointer) temp_free_atom;

      if (!(mem_area = avltree_ordered_search(mem_chunk->mem_tree, mem_chunk_search_func, mem)) ) die("mem_area not found.");

/*
 * If this mem area is unused then delete the
 *  area and list node and decrement the free mem.
 */
      if (mem_area->used==0)
        {
        if (prev_free_atom)
          prev_free_atom->next = temp_free_atom->next;
        else
          mem_chunk->free_atoms = temp_free_atom->next;
        temp_free_atom = temp_free_atom->next;

        mem_area->free += mem_chunk->atom_size;
        if (mem_area->free == mem_chunk->area_size)
          {
          mem_chunk->num_mem_areas--;
          mem_chunk->num_unused_areas--;

          if (mem_area->next)
          mem_area->next->prev = mem_area->prev;
          if (mem_area->prev)
            mem_area->prev->next = mem_area->next;
          if (mem_area == mem_chunk->mem_areas)
            mem_chunk->mem_areas = mem_chunk->mem_areas->next;
          if (mem_area == mem_chunk->mem_area)
            mem_chunk->mem_area = NULL;
 
          if (mem_chunk->mem_tree)
            {
            if (!avltree_remove_key(mem_chunk->mem_tree, (AVLKey) mem_area))
              die("mem_area not found.");
            }

          free(mem_area);
          }
        }
      else
        {
        prev_free_atom = temp_free_atom;
        temp_free_atom = temp_free_atom->next;
        }
      }
    }

  return;
  }


void mem_chunk_reset(MemChunk *mem_chunk)
  {
  MemArea *mem_areas;
  MemArea *temp_area;
  
  if (!mem_chunk) die("Null pointer to mem_chunk passed.");
  
  mem_areas = mem_chunk->mem_areas;
  mem_chunk->num_mem_areas = 0;
  mem_chunk->mem_areas = NULL;
  mem_chunk->mem_area = NULL;
  
  while (mem_areas)
    {
    temp_area = mem_areas;
    mem_areas = mem_areas->next;
    free(temp_area);
    }
  
  mem_chunk->free_atoms = NULL;
  
  if (mem_chunk->mem_tree)
    {
    avltree_delete(mem_chunk->mem_tree);
    mem_chunk->mem_tree = avltree_new((AVLKeyFunc)mem_chunk_key_generate);
    }

  return;
  }


#if MEMORY_PADDING==TRUE
static int memarea_check_bounds(MemChunk *mem_chunk, MemArea *mem_area)
  {
  int		count = 0;
  unsigned char	*mem;
  int		index = 0;

  while (index < mem_area->index)
    {
    mem = (unsigned char*) &mem_area->mem[index];
    if (check_pad_low(mem_chunk, mem)!=0) count++;
    if (check_pad_high(mem_chunk, mem)!=0) count++;
    index += mem_chunk->atom_size;
    }

  return count;
  }


void mem_chunk_check_all_bounds(MemChunk *mem_chunk)
  {
  MemArea	*mem_area;
  int		badcount=0;
  
  if (!mem_chunk) die("Null pointer to mem_chunk passed.");
  
  mem_area = mem_chunk->mem_areas;
  
  while (mem_area)
    {
    if (mem_chunk->mem_area->used>0)
      {
      badcount += memarea_check_bounds(mem_chunk, mem_area);
      mem_area = mem_area->next;
      }
    }

  printf("%d pads corrupt or free.\n", badcount);
  
  return;
  }


boolean mem_chunk_check_bounds(MemChunk *mem_chunk, vpointer mem)
  {
  mem = BUMP_DOWN(mem);
  if (check_pad_low(mem_chunk, mem)!=0)
    dief("Low padding corrupt! (%*s)", MEMORY_ALIGN_SIZE, (unsigned char *)mem);
  if (check_pad_high(mem_chunk, mem)!=0)
    dief("High padding corrupt!(%*s)", MEMORY_ALIGN_SIZE, (unsigned char *)mem);

  return TRUE;
  }
#else
boolean mem_chunk_check_bounds(MemChunk *mem_chunk, vpointer mem)
  {
  return TRUE;
  }
#endif


boolean mem_chunk_test(void)
  {
  unsigned char	*tmem[10000];
  MemChunk	*tmem_chunk=NULL;
  size_t	atomsize=40;
  int		i, j;

  printf("checking mem chunks...\n");

  tmem_chunk = mem_chunk_new(atomsize, 100);

  printf("alloc*1000...\n");
  for (i = 0; i < 1000; i++)
    {
    tmem[i] = mem_chunk_alloc(tmem_chunk);

    *tmem[i] = i%254;
    }

  for (i = 0; i < 1000; i++)
    {
    mem_chunk_check_bounds(tmem_chunk, tmem[i]);
    }

  printf("free*500...\n");
  for (i = 0; i < 500; i++)
    {
    mem_chunk_free(tmem_chunk, tmem[i]);
    }

  for (i = 500; i < 1000; i++)
    {
    mem_chunk_check_bounds(tmem_chunk, tmem[i]);
    }

  printf("alloc*500...\n");
  for (i = 0; i < 500; i++)
    {
    tmem[i] = mem_chunk_alloc(tmem_chunk);

    *tmem[i] = i%254;
    }

  for (i = 0; i < 1000; i++)
    {
    mem_chunk_check_bounds(tmem_chunk, tmem[i]);
    }

  printf("free*1000...\n");

  for (i = 0; i < 1000; i++)
    {
    if (*tmem[i] != i%254) die("Uh oh.");

    for (j = i; j<1000; j++)
      mem_chunk_check_bounds(tmem_chunk, tmem[j]);

    mem_chunk_free(tmem_chunk, tmem[i]);
    }

  printf("ok.\n");

  return TRUE;
  }


void mem_chunk_diagnostics(void)
  {
  printf("=== mem_chunk diagnostics ====================================\n");
  printf("Version:           %s\n", VERSION_STRING);
  printf("Build date:        %s\n", BUILD_DATE_STRING);
  printf("MEMORY_PADDING:    %s\n", MEMORY_PADDING?"TRUE":"FALSE");
  printf("MEMORY_ALIGN_SIZE  %d\n", MEMORY_ALIGN_SIZE);
  printf("MEMORY_AREA_SIZE   %ld\n", MEMORY_AREA_SIZE);

  printf("--------------------------------------------------------------\n");
  printf("structure          sizeof\n");
#ifdef IRIX_MIPSPRO_SOURCE
  printf("FreeAtom           %lu\n", (unsigned long int) sizeof(FreeAtom));
  printf("MemArea            %lu\n", (unsigned long int) sizeof(MemArea));
  printf("MemChunk           %lu\n", (unsigned long int) sizeof(MemChunk));
#else
  printf("FreeAtom           %Zd\n", sizeof(FreeAtom));
  printf("MemArea            %Zd\n", sizeof(MemArea));
  printf("MemChunk           %Zd\n", sizeof(MemChunk));
#endif
  printf("==============================================================\n");

  return;
  }
