/**********************************************************************
  str_hash.c
 **********************************************************************

  str_hash - Portable string hashing.
  Copyright ©1999-2000, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  in incompatiable with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  This code is heavily based on Str-0.9.3 by Ralf S. Engelschall; the
  relevant portion of which carries the following notice:

**  Str - String Library
**  Copyright (c) 1999-2000 Ralf S. Engelschall <rse@engelschall.com>
**
**  This file is part of Str, a string handling and manipulation 
**  library which can be found at http://www.engelschall.com/sw/str/.
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  str_hash.c: hashing functions 

 **********************************************************************

  Synopsis:     A collection of string hashing routines.

		Ultimately the functions in this file will form a
		consistent set of replacements for the dodgy functions
		that currently exist in str_util.c

		It should be noted that these functions can hash
		arbitrary data, not just strings.

		Multi-thread safe.

  Updated:      24/09/00 SAA	Added hash_elf() which is based on the published hashing algotithm for UNIX ELF format object files.  Added hash_generic().  A few tweaks so the compiler can make more optimisations.
		18/09/00 SAA	Tidied for better intergration into my str_util library.  Removed heavy usage of the 'register' keyword because I prefer to let my compiler sort that out (it will nearly always do a better job than I would).  Modified the indentation to my prefered style.  Removed Str Library specific datatypes.

  Todo:		Dynamic, run-time, registration of hashing functions.
		Wrapper for GLIB usage.

 **********************************************************************/

#include "str_hash.h"

/*
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + string[i]''. This is one of the
 * best hashing functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of the number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an own RSE-explanation: if one experimentally tests
 * all multipliers between 1 and 256 (as I did it) one detects that
 * even numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%. 
 *
 * If one compares the Chi/2 values resulting of the various
 * multipliers, the 33 not even has the best value. But the 33 and a
 * few other equally good values like 17, 31, 63, 127 and 129 have
 * nevertheless a great advantage over the remaining values in the large
 * set of possible multipliers: their multiply operation can be replaced
 * by a faster operation based on just one bit-wise shift plus either a
 * single addition or subtraction operation. And because a hash function
 * has to both distribute good and has to be very fast to compute, those
 * few values should be preferred and seems to be also the reason why
 * Daniel J. Bernstein also preferred it.
 */
static unsigned long hash_djbx33_unrolled(unsigned char *key, size_t len)
  {
  unsigned long hash = 5381;

/* Hash unrolled eight times. */
  while ( len >= 8 )
    {
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    hash = ((hash << 5) + hash) + *key++;
    len -= 8;
    }

  switch (len)
    {
    case 7: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 6: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 5: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 4: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 3: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 2: hash = ((hash << 5) + hash) + *key++; /* fallthrough... */
    case 1: hash = ((hash << 5) + hash) + *key++; break;
    default: break;
    }

  return hash;
  }


static unsigned long hash_djbx33(unsigned char *key, size_t len)
  {
  unsigned long hash = 5381;

  if (len == 0)
    while ( *key ) hash = ((hash << 5) + hash) + *key++;
  else
    while ( len-- > 0 ) hash = ((hash << 5) + hash) + *key++;

  return hash;
  }


/*
 * BJDDJ (Bob Jenkins, Dr. Dobbs Journal)
 *
 * This is a very complex but also very good hashing function, as
 * proposed in the March'97 issue of Dr. Dobbs Journal (DDJ) by Bob
 * Jenkins (see http://burtleburtle.net/bob/hash/doobs.html for online
 * version). He showed in his texts, that his hash function has both
 * very good distribution and performance and my own hash function
 * comparison confirmed this, too. The only difference to the original
 * function of B.J. here is that my version doesn't provide the `level'
 * (= previous hash) argument for consistency reasons with the other
 * hash functions (i.e. same function signature). It can be definitely
 * recommended as a very good general purpose hashing function.
 */
static unsigned long hash_bjddj(unsigned char *k, size_t length)
  {
  unsigned long a,b,c,len;

    /* some abbreviations */
#define ub4 unsigned long
#define mix(a,b,c) { \
        a -= b; a -= c; a ^= (c>>13); \
        b -= c; b -= a; b ^= (a<< 8); \
        c -= a; c -= b; c ^= (b>>13); \
        a -= b; a -= c; a ^= (c>>12); \
        b -= c; b -= a; b ^= (a<<16); \
        c -= a; c -= b; c ^= (b>> 5); \
        a -= b; a -= c; a ^= (c>> 3); \
        b -= c; b -= a; b ^= (a<<10); \
        c -= a; c -= b; c ^= (b>>15); \
    }

    /* setup the internal state */
    len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = 0;

    /* handle most of the key */
    while (len >= 12) {
        a += (k[0] +((ub4)k[1]<<8) +((ub4)k[ 2]<<16) +((ub4)k[ 3]<<24));
        b += (k[4] +((ub4)k[5]<<8) +((ub4)k[ 6]<<16) +((ub4)k[ 7]<<24));
        c += (k[8] +((ub4)k[9]<<8) +((ub4)k[10]<<16) +((ub4)k[11]<<24));
        mix(a,b,c);
        k += 12; len -= 12;
    }

    /* handle the last 11 bytes */
    c += length;
    switch(len) { 
        /* all the case statements fall through */
        case 11: c += ((ub4)k[10]<<24);
        case 10: c += ((ub4)k[ 9]<<16);
        case 9 : c += ((ub4)k[ 8]<< 8);
        /* the first byte of c is reserved for the length */
        case 8 : b += ((ub4)k[ 7]<<24);
        case 7 : b += ((ub4)k[ 6]<<16);
        case 6 : b += ((ub4)k[ 5]<< 8);
        case 5 : b += k[4];
        case 4 : a += ((ub4)k[ 3]<<24);
        case 3 : a += ((ub4)k[ 2]<<16);
        case 2 : a += ((ub4)k[ 1]<< 8);
        case 1 : a += k[0];
        /* case 0: nothing left to add */
    }
    mix(a,b,c);

/* delete abbreviations */
#undef ub4
#undef mix

  return c;
  }


/*
 * MACRC32 (Mark Adler, Cyclic Redundancy Check 32-Bit)
 *
 * This hash function is based on the good old CRC-32 (Cyclic Redundancy
 * Check with 32 Bit) algorithm as invented by Mark Adler. It is one
 * of the hash functions with medium performance but with very good
 * distribution. So it can be considered as a rock solid general purpose
 * hash function. It should be used if good distribution is more
 * important than high performance.
 */
static unsigned long hash_macrc32(unsigned char *key, size_t len)
  {
  unsigned long	hash = 0xffffffff;

  /* the CRC-32 table */
  static unsigned long tab[256] = {
        0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
        0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
        0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
        0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
        0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
        0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
        0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
        0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
        0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
        0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
        0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
        0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
        0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
        0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
        0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
        0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
        0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
        0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
        0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
        0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
        0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
        0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
        0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
        0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
        0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
        0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
        0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
        0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
        0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
        0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
        0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
        0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
        0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
        0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
        0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
        0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
        0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
        0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
        0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
        0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
        0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
        0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
        0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
        0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
        0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
        0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
        0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
        0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
        0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
        0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
        0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
        0x2d02ef8dL };
    
  if (len==0)
    {
    while (*key) hash = tab[(hash ^ *key++) & 0xff] ^ (hash >> 8);
    }
  else
    {
    while (len-- > 0) hash = tab[(hash ^ *key++) & 0xff] ^ (hash >> 8);
    }

  hash ^= 0xffffffff;

  return hash;
  }


/**********************************************************************
  unsigned long hash_elf()
  synopsis:	The published hashing algorithm as used in the UNIX
		ELF object file format.  Slightly modifed to allow
		a maximum length specification.
  parameters:	unsigned char	*key	The string to hash.
		size_t		n	Length of string, or 0.
  return:	unsigned long	hash	Hash value.
  last updated: 24/09/00
 **********************************************************************/

static unsigned long hash_elf(unsigned char *key, size_t len)
  {
  unsigned long	hash=0, g;

  if (len==0)
    {
    while (*key)
      {
      hash = (hash << 4) + *key++;
      if (g = hash & 0xF0000000) hash ^= g >> 24;
      hash &= ~g;
      }
    }
  else
    {
    while (len-- > 0)
      {
      hash = (hash << 4) + *key++;
      if (g = hash & 0xF0000000) hash ^= g >> 24;
      hash &= ~g;
      }
    }

  return hash;
  }


/**********************************************************************
  unsigned long hash_generic()
  synopsis:	An adaptation of Peter Weinberger's (PJW) generic
		hashing algorithm.  This was based on a GPL'd function
		found on www.sourceforge.com/snippets/.  I've adjusted
		it to use longs instead of ints.  Replaced dodgy 'for'
		loop with a much more sensible 'while' loop.  Added
		optional specification of maximum length of string or
		data to hash.
  parameters:	unsigned char	*key	The string to hash.
		size_t		n	Length of string, or 0.
  return:	unsigned long	hash	Hash value.
  last updated: 24/09/00
 **********************************************************************/

#define BITS_IN_LONG	( sizeof(long) * CHAR_BIT )
#define HIGH_BITS	( ~((unsigned long)(~0) >> ( (long)(BITS_IN_LONG/8) )) )

static unsigned long hash_generic(unsigned char *key, size_t len)
  {
  unsigned long	hash=0, i;

  if (len==0)
    {
    while (*key)
      {
      hash = (hash << ( (long)(BITS_IN_LONG/8) )) + *key++;
      if ((i = hash & HIGH_BITS) != 0 )
        hash = (hash ^ (i >> ( (long)((BITS_IN_LONG*3)/4) ))) & ~HIGH_BITS;
      }
    }
  else
    {
    while (len-- > 0)
      {
      hash = (hash << ( (long)(BITS_IN_LONG/8) )) + *key++;
      if ((i = hash & HIGH_BITS) != 0 )
        hash = (hash ^ (i >> ( (long)((BITS_IN_LONG*3)/4) ))) & ~HIGH_BITS;
      }
    }

  return hash;
  }

#undef BITS_IN_LONG
#undef HIGH_BITS


/**********************************************************************
  unsigned long str_hash()
  synopsis:	General purpose hashing function wrapper.  Length of
		string/data to hash may be specified, otherwise it
		is hashed upto the first null character.
  parameters:	const char	*str    The string to hash.
		size_t		n	Length of string, or 0.
		str_hash_type	mode	Hashing algorithm to employ.
  return:	unsigned long	hash	Hash value, or 0 upon failure.
  last updated: 18/09/00
 **********************************************************************/

unsigned long str_hash(const char *str, size_t n, str_hash_type mode)
  {
  unsigned long hash;

  if (!str) return 0;

  switch (mode)
    {
    case STR_HASH_X33:
/*
      if (n==0) n = strlen(str);
      hash = hash_djbx33_unrolled((unsigned char *)str, n);
*/
      hash = hash_djbx33((unsigned char *)str, n);
      break;
    case STR_HASH_BJ:
      if (n==0) n = strlen(str);
      hash = hash_bjddj((unsigned char *)str, n);
      break;
    case STR_HASH_CRC32:
      hash = hash_macrc32((unsigned char *)str, n);
      break;
    case STR_HASH_ELF:
      hash = hash_elf((unsigned char *)str, n);
      break;
    case STR_HASH_GENERIC:
      hash = hash_generic((unsigned char *)str, n);
      break;
    default:
      dief("Unknown hashing algorithm %d.", mode);
      break;
    }

  return hash;
  }

