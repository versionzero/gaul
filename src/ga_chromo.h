/**********************************************************************
  ga_chromo.h
 **********************************************************************

  ga_chromo - Genetic algorithm chromosome handling routines.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Synopsis:     Header file for the routines for handling GAUL's
		built-in chromosome types.

 **********************************************************************/

#ifndef GA_CHROMO_H_INCLUDED
#define GA_CHROMO_H_INCLUDED

#include "ga_core.h"

void ga_chromosome_integer_allocate(population *pop, entity *embryo);
void ga_chromosome_integer_deallocate(population *pop, entity *corpse);
void ga_chromosome_integer_replicate( population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid );
unsigned int ga_chromosome_integer_to_bytes(population *pop, entity *joe,
                                     byte **bytes, unsigned int *max_bytes);
void ga_chromosome_integer_from_bytes(population *pop, entity *joe, byte *bytes);
char *ga_chromosome_integer_to_staticstring(
                              population *pop, entity *joe);
void ga_chromosome_boolean_allocate(population *pop, entity *embryo);
void ga_chromosome_boolean_deallocate(population *pop, entity *corpse);
void ga_chromosome_boolean_replicate( population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid );
unsigned int ga_chromosome_boolean_to_bytes(population *pop, entity *joe,
                                    byte **bytes, unsigned int *max_bytes);
void ga_chromosome_boolean_from_bytes(population *pop, entity *joe, byte *bytes);
char *ga_chromosome_boolean_to_staticstring(
                              population *pop, entity *joe);
void ga_chromosome_double_allocate(population *pop, entity *embryo);
void ga_chromosome_double_deallocate(population *pop, entity *corpse);
void ga_chromosome_double_replicate( population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid );
unsigned int ga_chromosome_double_to_bytes(population *pop, entity *joe,
                                    byte **bytes, unsigned int *max_bytes);
void ga_chromosome_double_from_bytes(population *pop, entity *joe, byte *bytes);
char *ga_chromosome_double_to_staticstring(
                              population *pop, entity *joe);
void ga_chromosome_char_allocate(population *pop, entity *embryo);
void ga_chromosome_char_deallocate(population *pop, entity *corpse);
void ga_chromosome_char_replicate( population *pop,
                                      entity *parent, entity *child,
                                      const int chromosomeid );
unsigned int ga_chromosome_char_to_bytes(population *pop, entity *joe,
                                    byte **bytes, unsigned int *max_bytes);
void ga_chromosome_char_from_bytes(population *pop, entity *joe, byte *bytes);
char *ga_chromosome_char_to_staticstring(
                              population *pop, entity *joe);
void ga_chromosome_bitstring_allocate(population *pop, entity *embryo);
void ga_chromosome_bitstring_deallocate(population *pop, entity *corpse);
void ga_chromosome_bitstring_replicate( population *pop, entity *parent, entity *child, const int chromosomeid );
unsigned int ga_chromosome_bitstring_to_bytes(population *pop, entity *joe, byte **bytes, unsigned int *max_bytes);
void ga_chromosome_bitstring_from_bytes(population *pop, entity *joe, byte *bytes);
char *ga_chromosome_bitstring_to_staticstring(population *pop, entity *joe);

#endif /* GA_CHROMO_H_INCLUDED */

