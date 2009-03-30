/**********************************************************************
  ga_bitstring.h
 **********************************************************************

  ga_bitstring - GAUL's low-level bitstring routines.
  Copyright ©2001-2009, Stewart Adcock (http://saa.dyndns.org/)
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

  Synopsis:     Low-level bitstring handling functions.

 **********************************************************************/

#ifndef GA_BITSTRING_H_INCLUDED
#define GA_BITSTRING_H_INCLUDED

/*
 * Includes.
 */
#include "gaul.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

/*
 * Prototypes.
 */
GAULFUNC gaulbyte *ga_bit_new( int length );
GAULFUNC void ga_bit_free( gaulbyte *bstr );
GAULFUNC void ga_bit_set( gaulbyte *bstr, int n );
GAULFUNC void ga_bit_clear( gaulbyte *bstr, int n );
GAULFUNC void ga_bit_invert( gaulbyte *bstr, int n );
GAULFUNC boolean ga_bit_get( gaulbyte *bstr, int n );
GAULFUNC void ga_bit_randomize( gaulbyte *bstr, int n );
GAULFUNC void ga_bit_copy( gaulbyte *dest, gaulbyte *src, int ndest, int nsrc, int length );
GAULFUNC size_t ga_bit_sizeof( int length );
GAULFUNC gaulbyte *ga_bit_clone( gaulbyte *dest, gaulbyte *src, int length );

/* Integer conversion. */
GAULFUNC unsigned int ga_bit_decode_binary_uint( gaulbyte *bstr, int n, int length );
GAULFUNC void ga_bit_encode_binary_uint( gaulbyte *bstr, int n, int length, unsigned int value );
GAULFUNC int ga_bit_decode_binary_int( gaulbyte *bstr, int n, int length );
GAULFUNC void ga_bit_encode_binary_int( gaulbyte *bstr, int n, int length, int value );
GAULFUNC int ga_bit_decode_gray_int( gaulbyte *bstr, int n, int length );
GAULFUNC void ga_bit_encode_gray_int( gaulbyte *bstr, int n, int length, int value );
GAULFUNC unsigned int ga_bit_decode_gray_uint( gaulbyte *bstr, int n, int length );
GAULFUNC void ga_bit_encode_gray_uint( gaulbyte *bstr, int n, int length, unsigned int value );

/* Real conversion. */
GAULFUNC double ga_bit_decode_binary_real( gaulbyte *bstr, int n, int mantissa, int exponent );
GAULFUNC void ga_bit_encode_binary_real( gaulbyte *bstr, int n, int mantissa, int exponent, double value );
GAULFUNC double ga_bit_decode_gray_real( gaulbyte *bstr, int n, int mantissa, int exponent );
GAULFUNC void ga_bit_encode_gray_real( gaulbyte *bstr, int n, int mantissa, int exponent, double value );

/* Test. */
GAULFUNC boolean ga_bit_test( void );

#endif	/* GA_BITSTRING_H_INCLUDED */

