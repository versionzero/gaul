/**********************************************************************
  memory_chunks.h
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

 **********************************************************************/

#ifndef INCLUDED_MEMORY_CHUNKS_H
#define INCLUDED_MEMORY_CHUNKS_H

#include <stdlib.h>
#include <string.h>

#include "SAA_header.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else

#ifndef MEMORY_ALIGN_SIZE
#define MEMORY_ALIGN_SIZE       MAX(sizeof(void *), sizeof(long))
#endif

#ifndef MEMORY_PADDING
#define MEMORY_PADDING	FALSE
#endif

#endif

#include "avltree.h"

/*
 * Data types.
 */

typedef struct MemChunk_t MemChunk;

/*
 * Prototypes.
 */

boolean		mem_chunk_has_freeable_atoms(MemChunk *mem_chunk);
MemChunk	*mem_chunk_new_unfreeable(size_t atom_size, unsigned int num_atoms);
MemChunk	*mem_chunk_new(size_t atom_size, unsigned int num_atoms);
void		mem_chunk_destroy(MemChunk *mem_chunk);
vpointer	mem_chunk_alloc(MemChunk *mem_chunk);
void		mem_chunk_free(MemChunk *mem_chunk, vpointer mem);
void		mem_chunk_clean(MemChunk *mem_chunk);
void		mem_chunk_reset(MemChunk *mem_chunk);
boolean		mem_chunk_test(void);
void		mem_chunk_diagnostics(void);

#endif
