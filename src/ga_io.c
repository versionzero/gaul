/**********************************************************************
  ga_io.c
 **********************************************************************

  ga_io - Disk I/O routines.
  Copyright ©2003-2009, Stewart Adcock (http://saa.dyndns.org/)
  All rights reserved.

  The latest version of this program should be available at:
  http://gaul.sourceforge.net/

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

  Synopsis:     Routines for reading/writing GA data from/onto disk.

  To do:	Fix the really inefficient entity format (i.e. remove len).
		Remove excessive use of malloc()/free() in posix versions.
		Support for longer chromosomes in win32 versions.
		It is fair to say that the file formats are embarrasingly poor.
		Alternative optimisation function parameters not saved.

 **********************************************************************/

#include "gaul/ga_core.h"

#define BUFFER_SIZE	1024

/**********************************************************************
  gaul_write_entity_posix()
  synopsis:	Writes an entity to a filepointer using a binary format.
		Note: Currently does not (and probably can not) store
		any of the userdata.
  parameters:
  return:
  last updated: 29 May 2002
 **********************************************************************/

#ifndef USE_WINDOWS_H
static void gaul_write_entity_posix(FILE *fp, population *pop, entity *this_entity)
  {
  gaulbyte		*buffer=NULL;		/* Buffer for genetic data. */
  unsigned int	len, max_len=0;		/* Length of buffer. */

  fwrite(&(this_entity->fitness), sizeof(double), 1, fp);
  len = (int) pop->chromosome_to_bytes(pop, this_entity, &buffer, &max_len);
  fwrite(&len, sizeof(unsigned int), 1, fp);
  fwrite(buffer, sizeof(gaulbyte), len, fp);

  if (max_len!=0)
    s_free(buffer);

  return;
  }
#endif


/**********************************************************************
  gaul_write_entity_win32()
  synopsis:	Writes an entity to a filepointer using a binary format.
		Note: Currently does not (and probably can not) store
		any of the userdata.
  parameters:
  return:
  last updated: 29 May 2002
 **********************************************************************/

#ifdef USE_WINDOWS_H
static void gaul_write_entity_win32(HANDLE file,
                                 population *pop, entity *this_entity)
  {
  gaulbyte		buffer[BUFFER_SIZE];	/* Buffer for genetic data. */
  unsigned int	len, max_len=0;		/* Length of buffer. */
  gaulbyte		*bufptr;		/* Pointer into buffer. */
  DWORD		nwrote;			/* Number of bytes written. */

  memcpy(buffer, &(this_entity->fitness), sizeof(double));
  bufptr = buffer+sizeof(double)+sizeof(int);
  len = (int) pop->chromosome_to_bytes(pop, this_entity, &bufptr, &max_len);
  memcpy(buffer+sizeof(double), &len, sizeof(int));

  if ( WriteFile(file, buffer, len+sizeof(double)+sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

  return;
  }
#endif


/**********************************************************************
  gaul_read_entity_posix()
  synopsis:	Reads an entity from a filepointer using a binary
		format.
  parameters:	FILE *fp	File handle from which to read entity.
		population *pop	Population to add new entity to.
  return:	entity*	An entity as read from disk.
  last updated: 30 May 2002
 **********************************************************************/

#ifndef USE_WINDOWS_H
static entity *gaul_read_entity_posix(FILE *fp, population *pop)
  {
  gaulbyte		*buffer=NULL;	/* Buffer for genetic data. */
  unsigned int	len;		/* Length of buffer. */
  entity	*this_entity;	/* New entity read from disk. */

  this_entity = ga_get_free_entity(pop);

  fread(&(this_entity->fitness), sizeof(double), 1, fp);
  fread(&len, sizeof(unsigned int), 1, fp);

  if ( !(buffer = s_malloc(sizeof(gaulbyte)*len)) )
    die("Unable to allocate memory");

  fread(buffer, sizeof(gaulbyte), len, fp);
  pop->chromosome_from_bytes(pop, this_entity, buffer);

  s_free(buffer);

  return this_entity;
  }
#endif


/**********************************************************************
  gaul_read_entity_win32()
  synopsis:	Reads an entity from a filepointer using a binary
		format.
  parameters:	FILE *fp	File handle from which to read entity.
		population *pop	Population to add new entity to.
  return:	entity*	An entity as read from disk.
  last updated: 16 Aug 2003
 **********************************************************************/

#ifdef USE_WINDOWS_H
static entity *gaul_read_entity_win32(HANDLE file, population *pop)
  {
  gaulbyte		buffer[BUFFER_SIZE];	/* Buffer for genetic data. */
  unsigned int	len;		/* Length of buffer. */
  entity	*this_entity;	/* New entity read from disk. */
  DWORD		nread;		/* Number of bytes read. */

  this_entity = ga_get_free_entity(pop);

  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());

  memcpy(&(this_entity->fitness), buffer, sizeof(double));

  if (!ReadFile(file, buffer, sizeof(unsigned int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());

  memcpy(&len, buffer, sizeof(unsigned int));

  if (!ReadFile(file, buffer, len*sizeof(gaulbyte), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());

  pop->chromosome_from_bytes(pop, this_entity, buffer);

  return this_entity;
  }
#endif


enum GA_POPULATION_HOOK_OFFSET
  {
    GA_POPULATION_HOOK_OFFSET_GENERATION_HOOK,
    GA_POPULATION_HOOK_OFFSET_ITERATION_HOOK,
    GA_POPULATION_HOOK_OFFSET_DATA_DESTRUCTOR,
    GA_POPULATION_HOOK_OFFSET_DATA_REF_INCREMENTOR,
    GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_DESTRUCTOR,
    GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_COPY,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_CONSTRUCTOR,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_DESTRUCTOR,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_REPLICATE,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_BYTES,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_FROM_BYTES,
    GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_STRING,
    GA_POPULATION_HOOK_OFFSET_EVALUATE,
    GA_POPULATION_HOOK_OFFSET_SEED,
    GA_POPULATION_HOOK_OFFSET_ADAPT,
    GA_POPULATION_HOOK_OFFSET_SELECT_ONE,
    GA_POPULATION_HOOK_OFFSET_SELECT_TWO,
    GA_POPULATION_HOOK_OFFSET_MUTATE,
    GA_POPULATION_HOOK_OFFSET_CROSSOVER,
    GA_POPULATION_HOOK_OFFSET_REPLACE,
    GA_POPULATION_HOOK_OFFSET_RANK,

    GA_POPULATION_HOOK_COUNT
  };

/**********************************************************************
  ga_population_write()
  synopsis:	Writes entire population and it's genetic data to disk,
		using a binary format.
		Note: Currently does not (and probably can not) store
		any of the userdata.
  parameters:
  return:
  last updated: 29 Jan 2006
 **********************************************************************/

#ifndef USE_WINDOWS_H
GAULFUNC boolean ga_population_write(population *pop, char *fname)
  {
  FILE          *fp;				/* File handle. */
  int		i;				/* Loop variables. */
  char		buffer[BUFFER_SIZE];		/* String buffer. */
  int		id[GA_POPULATION_HOOK_COUNT];	/* Array of hook indices. */
  int		count=0;			/* Number of unrecognised hook functions. */
  char		*format_str="FORMAT: GAUL POPULATION 004";	/* Format tag. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if( !(fp=fopen(fname,"w")) )
    dief("Unable to open population file \"%s\" for output.", fname);

/*
 * Program info.
 */
  fwrite(format_str, sizeof(char), strlen(format_str), fp);
  for (i=0; i<64; i++) buffer[i]='\0';
  snprintf(buffer, 64, "%s %s", GA_VERSION_STRING, GA_BUILD_DATE_STRING);
  fwrite(buffer, sizeof(char), 64, fp);

/*
 * Population info.
 */
  fwrite(&(pop->size), sizeof(int), 1, fp);
  fwrite(&(pop->stable_size), sizeof(int), 1, fp);
  fwrite(&(pop->num_chromosomes), sizeof(int), 1, fp);
  fwrite(&(pop->len_chromosomes), sizeof(int), 1, fp);

/*
 * GA parameters.
 */
  fwrite(&(pop->crossover_ratio), sizeof(double), 1, fp);
  fwrite(&(pop->mutation_ratio), sizeof(double), 1, fp);
  fwrite(&(pop->migration_ratio), sizeof(double), 1, fp);
  fwrite(&(pop->allele_mutation_prob), sizeof(double), 1, fp);
  fwrite(&(pop->allele_min_integer), sizeof(int), 1, fp);
  fwrite(&(pop->allele_max_integer), sizeof(int), 1, fp);
  fwrite(&(pop->allele_min_double), sizeof(double), 1, fp);
  fwrite(&(pop->allele_max_double), sizeof(double), 1, fp);
  fwrite(&(pop->scheme), sizeof(int), 1, fp);
  fwrite(&(pop->elitism), sizeof(int), 1, fp);

  fwrite(&(pop->island), sizeof(int), 1, fp);

/*
 * Callback handling.  Note that user-implemented functions currently
 * can't be handled in these files.
 * id = -1 - Unknown, external function.
 * id = 0  - NULL function.
 * id > 0  - GAUL defined function.
 */
  id[GA_POPULATION_HOOK_OFFSET_GENERATION_HOOK] = ga_funclookup_ptr_to_id((void *)pop->generation_hook);
  id[GA_POPULATION_HOOK_OFFSET_ITERATION_HOOK]  = ga_funclookup_ptr_to_id((void *)pop->iteration_hook);

  /* GAUL doesn't define any functions for any of these four callbacks: */
  id[GA_POPULATION_HOOK_OFFSET_DATA_DESTRUCTOR]             = pop->data_destructor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_DATA_REF_INCREMENTOR]        = pop->data_ref_incrementor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_DESTRUCTOR]  = pop->population_data_destructor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_COPY]        = pop->population_data_copy ? -1 : 0;

  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_CONSTRUCTOR]  = ga_funclookup_ptr_to_id((void *)pop->chromosome_constructor);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_DESTRUCTOR]   = ga_funclookup_ptr_to_id((void *)pop->chromosome_destructor);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_REPLICATE]    = ga_funclookup_ptr_to_id((void *)pop->chromosome_replicate);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_BYTES]     = ga_funclookup_ptr_to_id((void *)pop->chromosome_to_bytes);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_FROM_BYTES]   = ga_funclookup_ptr_to_id((void *)pop->chromosome_from_bytes);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_STRING]    = ga_funclookup_ptr_to_id((void *)pop->chromosome_to_string);

  id[GA_POPULATION_HOOK_OFFSET_EVALUATE]    = ga_funclookup_ptr_to_id((void *)pop->evaluate);
  id[GA_POPULATION_HOOK_OFFSET_SEED]        = ga_funclookup_ptr_to_id((void *)pop->seed);
  id[GA_POPULATION_HOOK_OFFSET_ADAPT]       = ga_funclookup_ptr_to_id((void *)pop->adapt);
  id[GA_POPULATION_HOOK_OFFSET_SELECT_ONE]  = ga_funclookup_ptr_to_id((void *)pop->select_one);
  id[GA_POPULATION_HOOK_OFFSET_SELECT_TWO]  = ga_funclookup_ptr_to_id((void *)pop->select_two);
  id[GA_POPULATION_HOOK_OFFSET_MUTATE]      = ga_funclookup_ptr_to_id((void *)pop->mutate);
  id[GA_POPULATION_HOOK_OFFSET_CROSSOVER]   = ga_funclookup_ptr_to_id((void *)pop->crossover);
  id[GA_POPULATION_HOOK_OFFSET_REPLACE]     = ga_funclookup_ptr_to_id((void *)pop->replace);
  id[GA_POPULATION_HOOK_OFFSET_RANK]        = ga_funclookup_ptr_to_id((void *)pop->rank);

  fwrite(id, sizeof(int), GA_POPULATION_HOOK_COUNT, fp);

/*
 * Warn user of any unhandled data.
 */
  for (i=0; i < GA_POPULATION_HOOK_COUNT; i++)
    if (id[i] == -1) count++;

  if (count>0)
    plog(LOG_NORMAL, "Unable to handle %d hook function%sspecified in population structure.", count, count==1?" ":"s ");

/*
 * Entity info.
 */
  for (i=0; i<pop->size; i++)
    {
    gaul_write_entity_posix(fp, pop, pop->entity_iarray[i]);
    }

/*
 * Footer info.
 */
  fwrite("END", sizeof(char), 4, fp); 

/*
 * Close file.
 */
  fclose(fp);

  return TRUE;
}
#else

GAULFUNC boolean ga_population_write(population *pop, char *fname)
  {
  HANDLE        file;			/* File handle. */
  int		i;			/* Loop variables. */
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  int		id[GA_POPULATION_HOOK_COUNT];   /* Array of hook indices. */
  int		count=0;		/* Number of unrecognised hook functions. */
  char		*format_str="FORMAT: GAUL POPULATION 004";	/* Format tag. */
  DWORD		nwrote;			/* Number of bytes written. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if ((file = CreateFile(fname, GENERIC_WRITE,
        0, 0, CREATE_NEW, 0, 0)) == INVALID_HANDLE_VALUE)
    dief("Unable to open entity file \"%s\" for output due to error %d.",
         fname, GetLastError());

/*
 * Program info.
 */
  if ( WriteFile(file, format_str, strlen(format_str)*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

  for (i=0; i<64; i++) buffer[i]='\0';
  snprintf(buffer, 64, "%s %s", GA_VERSION_STRING, GA_BUILD_DATE_STRING);
  if ( WriteFile(file, format_str, 64*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

/*
 * Population info.
 */
  if ( WriteFile(file, &(pop->size), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->stable_size), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->num_chromosomes), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->len_chromosomes), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

/*
 * GA parameters.
 */
  if ( WriteFile(file, &(pop->crossover_ratio), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->mutation_ratio), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->migration_ratio), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->allele_mutation_prob), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->allele_min_integer), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->allele_max_integer), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->allele_min_double), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->allele_max_double), sizeof(double), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->scheme), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->elitism), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());
  if ( WriteFile(file, &(pop->island), sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

/*
 * Callback handling.  Note that user-implemented functions currently
 * can't be handled in these files.
 * id = -1 - Unknown, external function.
 * id = 0  - NULL function.
 * id > 0  - GAUL defined function.
 */
  id[GA_POPULATION_HOOK_OFFSET_GENERATION_HOOK] = ga_funclookup_ptr_to_id((void *)pop->generation_hook);
  id[GA_POPULATION_HOOK_OFFSET_ITERATION_HOOK]  = ga_funclookup_ptr_to_id((void *)pop->iteration_hook);

  /* GAUL doesn't define any functions for any of these four callbacks: */
  id[GA_POPULATION_HOOK_OFFSET_DATA_DESTRUCTOR]             = pop->data_destructor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_DATA_REF_INCREMENTOR]        = pop->data_ref_incrementor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_DESTRUCTOR]  = pop->population_data_destructor ? -1 : 0;
  id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_COPY]        = pop->population_data_copy ? -1 : 0;

  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_CONSTRUCTOR]  = ga_funclookup_ptr_to_id((void *)pop->chromosome_constructor);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_DESTRUCTOR]   = ga_funclookup_ptr_to_id((void *)pop->chromosome_destructor);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_REPLICATE]    = ga_funclookup_ptr_to_id((void *)pop->chromosome_replicate);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_BYTES]     = ga_funclookup_ptr_to_id((void *)pop->chromosome_to_bytes);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_FROM_BYTES]   = ga_funclookup_ptr_to_id((void *)pop->chromosome_from_bytes);
  id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_STRING]    = ga_funclookup_ptr_to_id((void *)pop->chromosome_to_string);

  id[GA_POPULATION_HOOK_OFFSET_EVALUATE]    = ga_funclookup_ptr_to_id((void *)pop->evaluate);
  id[GA_POPULATION_HOOK_OFFSET_SEED]        = ga_funclookup_ptr_to_id((void *)pop->seed);
  id[GA_POPULATION_HOOK_OFFSET_ADAPT]       = ga_funclookup_ptr_to_id((void *)pop->adapt);
  id[GA_POPULATION_HOOK_OFFSET_SELECT_ONE]  = ga_funclookup_ptr_to_id((void *)pop->select_one);
  id[GA_POPULATION_HOOK_OFFSET_SELECT_TWO]  = ga_funclookup_ptr_to_id((void *)pop->select_two);
  id[GA_POPULATION_HOOK_OFFSET_MUTATE]      = ga_funclookup_ptr_to_id((void *)pop->mutate);
  id[GA_POPULATION_HOOK_OFFSET_CROSSOVER]   = ga_funclookup_ptr_to_id((void *)pop->crossover);
  id[GA_POPULATION_HOOK_OFFSET_REPLACE]     = ga_funclookup_ptr_to_id((void *)pop->replace);
  id[GA_POPULATION_HOOK_OFFSET_RANK]        = ga_funclookup_ptr_to_id((void *)pop->rank);

  if ( WriteFile(file, id, GA_POPULATION_HOOK_COUNT * sizeof(int), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

/*
 * Warn user of any unhandled data.
 */
  for (i=0; i < GA_POPULATION_HOOK_COUNT; i++)
    if (id[i] == -1) count++;

  if (count>0)
    plog(LOG_NORMAL, "Unable to handle %d hook function%sspecified in population structure.", count, count==1?" ":"s ");

/*
 * Entity info.
 */
  for (i=0; i<pop->size; i++)
    {
    gaul_write_entity_win32(file, pop, pop->entity_iarray[i]);
    }

/*
 * Footer info.
 */
  strncpy(buffer, "END\0", 4); 
  if ( WriteFile(file, format_str, 4*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

/*
 * Close file.
 */
  CloseHandle(file);

  return TRUE;
  }
#endif


/**********************************************************************
  ga_population_read()
  synopsis:	Reads entire population and it's genetic data back
		from disk.   Some things can't be restored.  See
		ga_population_write() for details.
  parameters:	char *fname		Filename to read from.
  return:	population *pop		New population structure.
  last updated: 24 Feb 2005
 **********************************************************************/

#ifndef USE_WINDOWS_H

GAULFUNC population *ga_population_read(char *fname)
  {
  population	*pop=NULL;			/* New population structure. */
  FILE          *fp;				/* File handle. */
  int		i;				/* Loop variables. */
  char		buffer[BUFFER_SIZE];		/* String buffer. */
  int		id[GA_POPULATION_HOOK_COUNT];			/* Array of hook indices. */
  int		count=0;			/* Number of unrecognised hook functions. */
  char		*format_str="FORMAT: GAUL POPULATION 004";	/* Format tag. */
  char		format_str_in[32]="";		/* Input format tag. (Empty initialiser to avoid valgrind warning...) */
  int		size, stable_size, num_chromosomes, len_chromosomes;	/* Input data. */

/* Checks. */
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if( !(fp=fopen(fname,"r")) )
    dief("Unable to open population file \"%s\" for input.", fname);

/*
 * Program info.
 */
  fread(format_str_in, sizeof(char), strlen(format_str), fp);
  if (strcmp(format_str, format_str_in)!=0)
    {
    fclose(fp);
    die("Invalid file format");
    }

  fread(buffer, sizeof(char), 64, fp);	/* Presently ignored. */

/*
 * Population info.
 */
  fread(&size, sizeof(int), 1, fp);
  fread(&stable_size, sizeof(int), 1, fp);
  fread(&num_chromosomes, sizeof(int), 1, fp);
  fread(&len_chromosomes, sizeof(int), 1, fp);

/*
 * Allocate a new population structure.
 */
  pop = ga_population_new(stable_size, num_chromosomes, len_chromosomes);

/*
 * Got a population structure?
 */
  if ( !pop ) die("Unable to allocate population structure.");

/*
 * GA parameters.
 */
  fread(&(pop->crossover_ratio), sizeof(double), 1, fp);
  fread(&(pop->mutation_ratio), sizeof(double), 1, fp);
  fread(&(pop->migration_ratio), sizeof(double), 1, fp);
  fread(&(pop->allele_mutation_prob), sizeof(double), 1, fp);
  fread(&(pop->allele_min_integer), sizeof(int), 1, fp);
  fread(&(pop->allele_max_integer), sizeof(int), 1, fp);
  fread(&(pop->allele_min_double), sizeof(double), 1, fp);
  fread(&(pop->allele_max_double), sizeof(double), 1, fp);
  fread(&(pop->scheme), sizeof(int), 1, fp);
  fread(&(pop->elitism), sizeof(int), 1, fp);
  fread(&(pop->island), sizeof(int), 1, fp);

/*
 * Callback handling.  Note that user-implemented functions currently
 * can't be handled in these files.
 * id = -1 - Unknown, external function.
 * id = 0  - NULL function.
 * id > 0  - GAUL defined function.
 */
  fread(id, sizeof(int), GA_POPULATION_HOOK_COUNT, fp);

  pop->generation_hook        = (GAgeneration_hook)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_GENERATION_HOOK]);
  pop->iteration_hook         = (GAiteration_hook)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_ITERATION_HOOK]);

  pop->data_destructor        = (GAdata_destructor)      ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_DATA_DESTRUCTOR]);
  pop->data_ref_incrementor   = (GAdata_ref_incrementor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_DATA_REF_INCREMENTOR]);

  pop->population_data_destructor = (GAdata_destructor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_DESTRUCTOR]);
  pop->population_data_copy   = (GAdata_copy)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_COPY]);

  pop->chromosome_constructor = (GAchromosome_constructor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_CONSTRUCTOR]);
  pop->chromosome_destructor  = (GAchromosome_destructor)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_DESTRUCTOR]);
  pop->chromosome_replicate   = (GAchromosome_replicate)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_REPLICATE]);
  pop->chromosome_to_bytes    = (GAchromosome_to_bytes)    ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_BYTES]);
  pop->chromosome_from_bytes  = (GAchromosome_from_bytes)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_FROM_BYTES]);
  pop->chromosome_to_string   = (GAchromosome_to_string)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_STRING]);

  pop->evaluate               = (GAevaluate)       ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_EVALUATE]);
  pop->seed                   = (GAseed)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SEED]);
  pop->adapt                  = (GAadapt)          ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_ADAPT]);
  pop->select_one             = (GAselect_one)     ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SELECT_ONE]);
  pop->select_two             = (GAselect_two)     ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SELECT_TWO]);
  pop->mutate                 = (GAmutate)         ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_MUTATE]);
  pop->crossover              = (GAcrossover)      ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CROSSOVER]);
  pop->replace                = (GAreplace)        ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_REPLACE]);
  pop->rank                   = (GArank)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_RANK]);

/*
 * Warn user of any unhandled data.
 */
  for (i=0; i < GA_POPULATION_HOOK_COUNT; i++)
    if (id[i] == -1) count++;

  if (count>0)
    plog(LOG_NORMAL, "Unable to handle %d hook function%sspecified in population structure.", count, count==1?" ":"s ");

/*
 * Entity info.
 */
  for (i=0; i<size; i++)
    {
    gaul_read_entity_posix(fp, pop);
    }

/*
 * Footer info.
 */
  fread(buffer, sizeof(char), 4, fp); 
  if (strcmp("END", buffer)!=0)
    die("Corrupt population file?");

/*
 * Close file.
 */
  fclose(fp);

  plog(LOG_DEBUG, "Have read %d entities into population.", pop->size);

  return pop;
}

#else

GAULFUNC population *ga_population_read(char *fname)
  {
  population	*pop=NULL;		/* New population structure. */
  HANDLE        file;			/* File handle. */
  int		i;			/* Loop variables. */
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  int		id[GA_POPULATION_HOOK_COUNT];   /* Array of hook indices. */
  int		count=0;		/* Number of unrecognised hook functions. */
  char		*format_str="FORMAT: GAUL POPULATION 004";	/* Format tag. */
  int		size, stable_size, num_chromosomes, len_chromosomes;	/* Input data. */
  DWORD		nread;			/* Number of bytes read. */

/* Checks. */
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if ( (file = CreateFile(fname, GENERIC_READ,
        0, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE )
    dief("Unable to open entity file \"%s\" for input due to error %d.",
         fname, GetLastError());

/*
 * Program info.
 */
  if (!ReadFile(file, buffer, strlen(format_str), &nread, NULL) || nread != strlen(format_str))
    dief("Unable to read data.  Error %d\n", GetLastError());

  if (strncmp(format_str, buffer, strlen(format_str))!=0)
    {
    CloseHandle(file);
    die("Invalid file format");
    }

  /* Presently ignored. */
  if (!ReadFile(file, buffer, 64*sizeof(char), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());

/*
 * Population info.
 */
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&size, buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&stable_size, buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&num_chromosomes, buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&len_chromosomes, buffer, sizeof(int));

/*
 * Allocate a new population structure.
 */
  pop = ga_population_new(stable_size, num_chromosomes, len_chromosomes);

/*
 * Got a population structure?
 */
  if ( !pop ) die("Unable to allocate population structure.");

/*
 * GA parameters.
 */
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->crossover_ratio), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->mutation_ratio), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->migration_ratio), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->allele_mutation_prob), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->allele_min_integer), buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->allele_max_integer), buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->allele_min_double), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->allele_max_double), buffer, sizeof(double));
  if (!ReadFile(file, buffer, sizeof(double), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->scheme), buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->elitism), buffer, sizeof(int));
  if (!ReadFile(file, buffer, sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&(pop->island), buffer, sizeof(int));

/*
 * Callback handling.  Note that user-implemented functions currently
 * can't be handled in these files.
 * id = -1 - Unknown, external function.
 * id = 0  - NULL function.
 * id > 0  - GAUL defined function.
 */
  if (!ReadFile(file, buffer, GA_POPULATION_HOOK_COUNT * sizeof(int), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  memcpy(&id, buffer, GA_POPULATION_HOOK_COUNT * sizeof(int));

  pop->generation_hook        = (GAgeneration_hook)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_GENERATION_HOOK]);
  pop->iteration_hook         = (GAiteration_hook)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_ITERATION_HOOK]);

  pop->data_destructor        = (GAdata_destructor)      ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_DATA_DESTRUCTOR]);
  pop->data_ref_incrementor   = (GAdata_ref_incrementor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_DATA_REF_INCREMENTOR]);

  pop->population_data_destructor = (GAdata_destructor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_DESTRUCTOR]);
  pop->population_data_copy   = (GAdata_copy)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_POPULATION_DATA_COPY]);

  pop->chromosome_constructor = (GAchromosome_constructor) ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_CONSTRUCTOR]);
  pop->chromosome_destructor  = (GAchromosome_destructor)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_DESTRUCTOR]);
  pop->chromosome_replicate   = (GAchromosome_replicate)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_REPLICATE]);
  pop->chromosome_to_bytes    = (GAchromosome_to_bytes)    ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_BYTES]);
  pop->chromosome_from_bytes  = (GAchromosome_from_bytes)  ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_FROM_BYTES]);
  pop->chromosome_to_string   = (GAchromosome_to_string)   ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CHROMOSOME_TO_STRING]);

  pop->evaluate               = (GAevaluate)       ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_EVALUATE]);
  pop->seed                   = (GAseed)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SEED]);
  pop->adapt                  = (GAadapt)          ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_ADAPT]);
  pop->select_one             = (GAselect_one)     ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SELECT_ONE]);
  pop->select_two             = (GAselect_two)     ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_SELECT_TWO]);
  pop->mutate                 = (GAmutate)         ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_MUTATE]);
  pop->crossover              = (GAcrossover)      ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_CROSSOVER]);
  pop->replace                = (GAreplace)        ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_REPLACE]);
  pop->rank                   = (GArank)           ga_funclookup_id_to_ptr(id[GA_POPULATION_HOOK_OFFSET_RANK]);

/*
 * Warn user of any unhandled data.
 */
  for (i=0; i < GA_POPULATION_HOOK_COUNT; i++)
    if (id[i] == -1) count++;

  if (count>0)
    plog(LOG_NORMAL, "Unable to handle %d hook function%sspecified in population structure.", count, count==1?" ":"s ");

/*
 * Entity info.
 */
  for (i=0; i<size; i++)
    {
    gaul_read_entity_win32(file, pop);
    }

/*
 * Footer info.
 */
  if (!ReadFile(file, buffer, 4*sizeof(char), &nread, NULL) || nread < 1)
    dief("Unable to read data.  Error %d\n", GetLastError());
  if (strcmp("END", buffer)!=0) die("Corrupt population file?");

/*
 * Close file.
 */
  CloseHandle(file);

  plog(LOG_DEBUG, "Have read %d entities into population.", pop->size);

  return pop;
  }
#endif


/**********************************************************************
  ga_entity_write()
  synopsis:	Write an entity to disk.
		Note: Currently does not (and probably can not) store
		any of the userdata.
  parameters:	population *pop
		entity *this_entity
		char *fname
  return:	TRUE
  last updated: 07 Nov 2002
 **********************************************************************/

#ifndef USE_WINDOWS_H
GAULFUNC boolean ga_entity_write(population *pop, entity *this_entity, char *fname)
  {
  int		i;			/* Loop variable. */
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  char		*format_str="FORMAT: GAUL ENTITY 001";	/* Format tag. */
  FILE		*fp;			/* Filehandle. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !this_entity ) die("Null pointer to entity structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if( !(fp=fopen(fname,"w")) )
    dief("Unable to open entity file \"%s\" for output.", fname);

/*
 * Write stuff.
 */
  fwrite(format_str, sizeof(char), strlen(format_str), fp);
  for (i=0; i<64; i++) buffer[i]='\0';
  snprintf(buffer, 64, "%s %s", GA_VERSION_STRING, GA_BUILD_DATE_STRING);
  fwrite(buffer, sizeof(char), 64, fp);

  gaul_write_entity_posix(fp, pop, this_entity);

  fwrite("END", sizeof(char), 4, fp); 

  fclose(fp);

  return TRUE;
  }
#else

GAULFUNC boolean ga_entity_write(population *pop, entity *this_entity, char *fname)
  {
  int		i;			/* Loop variable. */
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  char		*format_str="FORMAT: GAUL ENTITY 001";	/* Format tag. */
  HANDLE	file;			/* Filehandle. */
  DWORD		nwrote;			/* Number of bytes written. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !this_entity ) die("Null pointer to entity structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open output file.
 */
  if ((file = CreateFile(fname, GENERIC_WRITE,
        0, 0, CREATE_NEW, 0, 0)) == INVALID_HANDLE_VALUE)
    dief("Unable to open entity file \"%s\" for output due to error %d.",
         fname, GetLastError());

/*
 * Write stuff.
 */
  if ( WriteFile(file, format_str, strlen(format_str)*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

  for (i=0; i<64; i++) buffer[i]='\0';
  snprintf(buffer, 64, "%s %s", GA_VERSION_STRING, GA_BUILD_DATE_STRING);
  if ( WriteFile(file, format_str, 64*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

  gaul_write_entity_win32(file, pop, this_entity);

  strncpy(buffer, "END\0", 4); 
  if ( WriteFile(file, format_str, 4*sizeof(char), &nwrote, NULL)==0 )
    dief("Error writing %d\n", GetLastError());

  CloseHandle(file);

  return TRUE;
  }
#endif


/**********************************************************************
  ga_entity_read()
  synopsis:	Read an entity from disk.
		Note: Currently does not (and probably can not) store
		any of the userdata.
  parameters:	population *pop
		entity *this_entity
		char *fname
  return:	TRUE
  last updated: 30 May 2002
 **********************************************************************/

#ifndef USE_WINDOWS_H
/*
 * UNIX/POSIX version:
 */

GAULFUNC entity *ga_entity_read(population *pop, char *fname)
  {
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  char		*format_str="FORMAT: GAUL ENTITY 001";	/* Format tag. */
  char		format_str_in[32];	/* Input format tag. */
  FILE		*fp;			/* Filehandle. */
  entity	*this_entity;		/* Input entity. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open input file.
 */
  if( !(fp=fopen(fname,"r")) )
    dief("Unable to open entity file \"%s\" for input.", fname);

/*
 * Read stuff.
 */
  fread(format_str_in, sizeof(char), strlen(format_str), fp);
  if (strcmp(format_str, format_str_in)!=0) die("Incorrect format for entity file.");
  fread(buffer, sizeof(char), 64, fp);	/* Ignored. */

  this_entity = gaul_read_entity_posix(fp, pop);

  fread(buffer, sizeof(char), 4, fp); 
  if (strcmp("END", buffer)!=0) die("Corrupt population file?");

  fclose(fp);

  return this_entity;
  }

#else
/*
 * MS Windows version:
 */

GAULFUNC entity *ga_entity_read(population *pop, char *fname)
  {
  char		buffer[BUFFER_SIZE];	/* String buffer. */
  char		*format_str="FORMAT: GAUL ENTITY 001";	/* Format tag. */
  char		format_str_in[32];	/* Input format tag. */
  HANDLE	file;			/* Filehandle. */
  entity	*this_entity;		/* Input entity. */
  DWORD		nread;			/* Number of bytes read. */

/* Checks. */
  if ( !pop ) die("Null pointer to population structure passed.");
  if ( !fname ) die("Null pointer to filename passed.");

/*
 * Open input file.
 */
  if ( (file = CreateFile(fname, GENERIC_READ,
        0, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE )
    dief("Unable to open entity file \"%s\" for input due to error %d.",
         fname, GetLastError());

/*
 * Read stuff.
 */
  if (!ReadFile(file, format_str_in, strlen(format_str)*sizeof(char), &nread, NULL) && nread < 1)
    dief("Unable to open entity file \"%s\" for input due to error %d.",
         fname, GetLastError());

  if (strncmp(format_str, format_str_in, strlen(format_str))!=0)
    die("Incorrect format for entity file.");

  ReadFile(file, buffer, 64*sizeof(char), &nread, NULL);	/* Ignored. */

  this_entity = gaul_read_entity_win32(file, pop);

  if (!ReadFile(file, buffer, 4, &nread, NULL) && nread < 1)
    dief("Unable to open entity file \"%s\" for input due to error %d.",
         fname, GetLastError());

  if (strcmp("END", buffer)!=0) die("Corrupt population file?");

  CloseHandle(file);

  return this_entity;
  }
#endif

