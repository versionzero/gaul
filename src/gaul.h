/**********************************************************************
  gaul.h
 **********************************************************************

  gaul - Genetic Algorithm Utility Library.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:	Public header file for GAUL.

		This file should be included by any code that will
		be linking to libgaul.

		This file is a bit empty at the moment, but prototypes
		for all of the public functions will be moved into here
		at some point.

 **********************************************************************/

#ifndef GAUL_H_INCLUDED
#define GAUL_H_INCLUDED

/*
 * Includes
 */
#include "SAA_header.h"

/*
 * Forward declarations.
 */
typedef struct entity_t entity;
typedef struct population_t population;

/*
 * Enumerated types, used to define varients of the GA algorithms.
 */
typedef enum ga_genesis_type_t
  {
  GA_GENESIS_UNKNOWN = 0,
  GA_GENESIS_RANDOM, GA_GENESIS_PDB, GA_GENESIS_SOUP, GA_GENESIS_USER
  } ga_genesis_type;

typedef enum ga_class_type_t
  {
  GA_CLASS_UNKNOWN = 0,
  GA_CLASS_DARWIN,
  GA_CLASS_LAMARCK, GA_CLASS_LAMARCK_ALL,
  GA_CLASS_BALDWIN, GA_CLASS_BALDWIN_ALL
  } ga_class_type;

typedef enum ga_elitism_type_t
  {
  GA_ELITISM_UNKNOWN = 0,
  GA_ELITISM_PARENTS_SURVIVE, GA_ELITISM_ROUGH, GA_ELITISM_ROUGH_COMP,
  GA_ELITISM_EXACT, GA_ELITISM_EXACT_COMP,
  GA_ELITISM_PARENTS_DIE
  } ga_elitism_type;

/*
 * Callback function typedefs.
 */
/*
 * Analysis and termination.
 */
typedef boolean (*GAgeneration_hook)(const int generation, population *pop);
typedef boolean (*GAiteration_hook)(const int iteration, entity *entity);

/*
 * Phenome (A general purpose data cache) handling.
 */
typedef void    (*GAdata_destructor)(vpointer data);
typedef void    (*GAdata_ref_incrementor)(vpointer data);

/*
 * Genome handling.
 */
typedef boolean (*GAchromosome_constructor)(population *pop, entity *entity);
typedef void    (*GAchromosome_destructor)(population *pop, entity *entity);
typedef void    (*GAchromosome_replicate)(population *pop, entity *parent, entity *child, const int chromosomeid);
typedef unsigned int    (*GAchromosome_to_bytes)(population *pop, entity *joe, byte **bytes, unsigned int *max_bytes);
typedef void    (*GAchromosome_from_bytes)(population *pop, entity *joe, byte *bytes);
typedef char    *(*GAchromosome_to_string)(population *pop, entity *joe);

/*
 * GA operations.
 *
 * FIXME: Adaptation prototype should match the mutation prototype so that
 * the adaptation local optimisation algorithms may be used as mutation
 * operators.
 */
typedef boolean (*GAevaluate)(population *pop, entity *entity);
typedef boolean	(*GAseed)(population *pop, entity *adam);
typedef entity *(*GAadapt)(population *pop, entity *child);
typedef boolean (*GAselect_one)(population *pop, entity **mother);
typedef boolean (*GAselect_two)(population *pop, entity **mother, entity **father);
typedef void    (*GAmutate)(population *pop, entity *mother, entity *daughter);
typedef void    (*GAcrossover)(population *pop, entity *mother, entity *father, entity *daughter, entity *son);
typedef void    (*GAreplace)(population *pop, entity *child);

/*
 * Include remainder of this library's headers.
 * These should, mostly, contain private definitions etc.
 * But they currently contain almost everything.
 */
#include "ga_core.h"

#if HAVE_SLANG==1
#include "ga_intrinsics.h"
#endif

#endif	/* GAUL_H_INCLUDED */
