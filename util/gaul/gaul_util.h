/**********************************************************************
  gaul_util.h
 **********************************************************************

  gaul_util.h - General header to define a few useful things
  Copyright ©1999-2003, Stewart Adcock <stewart@linux-domain.com>
  All rights reserved.

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This file is free software; you can redistribute it and/or modify
  it either under the terms of version 2 of the GNU General Public
  License as published by the Free Software Foundation, or (at your
  option) any other license approved by the Open Source Initiative.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

  The list of OSI approved licenses is available at
  http://opensource.org/licenses/

 **********************************************************************

  Synopsis:	Define some common macros, constants, etc.

 **********************************************************************/

#ifndef GAUL_UTIL_H_INCLUDED
#define GAUL_UTIL_H_INCLUDED

/*
 * _GNU_SOURCE includes the _POSIX_SOURCE and _ISOC99_SOURCE
 * C extensions, which are normally fairly useful!
 * This has to be defined prior to the inclusion of any of the 
 * standard C headers.
 */
#if defined(__GNUC__)
# define _GNU_SOURCE
#endif

/*
 * Include the platform specific configuration file:
 */
#ifdef WIN32
#include "gaul/gaul_config_win.h"
#else
#include "gaul/gaul_config.h"
#endif

#ifndef USE_PTHREADS
/*
 * If threads are used, these must be properly defined somewhere.
 * Unfortunately empty macros cause splint parse errors.  They
 * also cause lots of warnings when using Sun's and Compaq's
 * compilers.
 * In addition, ISO C99 standard doesn't allow extraneous ';'s
 * so we must define some dummy expressions.
 */
# if defined(__GNUC__) && !defined(_ISOC99_SOURCE)
#  define THREAD_LOCK_DEFINE_STATIC(name)
#  define THREAD_LOCK_DEFINE(name)
#  define THREAD_LOCK_EXTERN(name)
#  define THREAD_LOCK(name)
#  define THREAD_UNLOCK(name)
#  define THREAD_TRYLOCK(name)		0
# else
#  define THREAD_LOCK_DEFINE_STATIC(name)	static int (name) = 0
#  define THREAD_LOCK_DEFINE(name)	int (name) = 0
#  define THREAD_LOCK_EXTERN(name)	extern int (name)
#  define THREAD_LOCK(name)		(name) = 1
#  define THREAD_UNLOCK(name)		(name) = 0
#  define THREAD_TRYLOCK(name)		0
# endif
#else
#  define THREAD_LOCK_DEFINE_STATIC(name)       static pthread_mutex_t (name) = PTHREAD_MUTEX_INITIALIZER
#  define THREAD_LOCK_DEFINE(name)      pthread_mutex_t (name) = PTHREAD_MUTEX_INITIALIZER
#  define THREAD_LOCK_EXTERN(name)      extern pthread_mutex_t (name)
#  define THREAD_LOCK(name)             pthread_mutex_lock((name))
#  define THREAD_UNLOCK(name)           pthread_mutex_unlock((name))
#  define THREAD_TRYLOCK(name)          pthread_mutex_trylock((name))
#  define thread_mutex_new(name)	pthread_mutex_init((name), NULL)
#  define thread_mutex_free(name)	pthread_mutex_destroy((name))
#endif

/*
 * Includes needed for this stuff.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Me.
 */
#define AUTHOR	"Stewart Adcock"
#define EMAIL	"<stewart@linux_domain.com>"

/*
 * Define boolean type sensibly.
 */
#if HAVE_STDBOOL_H != 1
# include <stdbool.h>
#else

# if !defined(__bool_true_false_are_defined)
#  if HAVE__BOOL != 1
typedef short _Bool;
#   define HAVE__BOOL
#  endif

#  if !defined(true)
#   define true  (_Bool) 1
#   define false (_Bool) 0
#  endif

/* According to ISO C99, the boolean stuff must be available
 * in preprocessor directives and __bool_true_false_are_defined
 * should be true.
 */
#  define __bool_true_false_are_defined 1
# endif

# define bool _Bool
#endif

#if HAVE__BOOL != 1
typedef short _Bool;
#endif

#define boolean bool

#if !defined(TRUE)
#define TRUE	(0==0)
#define FALSE	(0!=0)
#endif

/*
 * Additional types.
 */
#if !defined(SUN_FORTE_C) || SUN_FORTE_C==FALSE
typedef void* vpointer;
#else
#define vpointer void*
#endif

typedef const void *constvpointer;
typedef unsigned char byte;

#ifdef BITSPERBYTE
# define BYTEBITS	BITSPERBYTE
#else
# ifdef CHARBITS
#  define BYTEBITS	CHARBITS
# else
/* Guess! */
#  define BYTEBITS	8
# endif
#endif

/*
 * Useful constants
 */
#ifndef PI
#define PI              3.1415926536
#endif
#define	EPSILONZERO	(8.854187817e-12)
#define	TWO_PI		6.2831853072
#define SQRT_PI		1.7724538509
#define SQRT_TWO	1.4142135624
#define EXP             2.7182818285
#define ROOT_TWO	1.4142135624
#define	INV_SQRT_3	(1.0/sqrt(3.0))
#define PI_OVER_TWO	(3.1415926536/2.0)
#define EOF             (-1)
#ifndef NULL
#define NULL		((void*)0)
#endif
#define VOID		((void)0)
#define NULL_CHAR	('\0')
#define ERROR           (-5)
#define OKAY		2
#define TINY            (1.0e-8)
#define ONE_MINUS_TINY  (1.0 - TINY)
#define MAX_LINE_LENGTH 1024
#define MAX_LINE_LEN	1024
#define MAX_FNAME_LEN	256
#define LARGE_AMOUNT	(1.0e38)
#define ApproxZero      (1e-18)
#define IsApproxZero(x) (fabs(x)<=ApproxZero)
#define RAD2DEG		57.2957795128		/* 180.0/PI */
#define DEG2RAD		0.01745329252		/* PI/180.0 */

/*
 * Useful macros
 */
#define SQU(x)          ((x)*(x))		/* Square */
#define CUBE(x)         ((x)*(x)*(x))		/* Cube */
#define FOURTH_POW(x)	((x)*(x)*(x)*(x))	/* Guess ;) */
#define FIFTH_POW(x)	((x)*(x)*(x)*(x)*(x))	/*  - " - */
#define DEG(x)		((x)*RAD2DEG)		/* Radians -> Degrees */
#define RAD(x)		((x)*DEG2RAD)		/* Degress -> Radians */
#define DNEAR(x,y)	(!((x)>y+TINY||(x)<y-TINY))	/* Are two doubles/floats very nearly equal? */
#define NDNEAR(x,y)	(((x)>y+TINY||(x)<y-TINY))	/* Are two doubles/floats not nearly equal? */
#define ISTINY(x)	(((x)<TINY)&&((x)>-TINY))	/* Near zero? */

#define ISODD(x)	(((x)&1))		/* True if x is odd */
#define ISEVEN(x)	(!((x)&1))		/* True if x is even */

#ifndef MIN
#define MIN(x,y)	(((x)<(y))?(x):(y))	/* Return smallest */
#define MAX(x,y)	(((x)>(y))?(x):(y))	/* Return largest */
#endif
#define MIN3(x,y,z)	(((x)<(y))?(((x)<(z))?(x):(z)):(((z)<(y))?(z):(y)))	/* Return smallest */
#define MAX3(x,y,z)	(((x)>(y))?(((x)>(z))?(x):(z)):(((z)>(y))?(z):(y)))	/* Return largest */
#define MINMAX(a,z,x,y) {if((x)<(y)){a=x;z=y;}else{z=x;a=y;}}

/* Swap primitives */
#define SWAP_INT(x,y)		{int t; t = x; x = y; y = t; }
#define SWAP_CHAR(x,y)		{char t; t = x; x = y; y = t; }
#define SWAP_FLOAT(x,y)		{float t; t = x; x = y; y = t; }
#define SWAP_DOUBLE(x,y)	{double t; t = x; x = y; y = t; }
#define SWAP_BOOLEAN(x,y)	{boolean t; t = x; x = y; y = t; }

/* Working with bitvectors. c is the vector, n is the bit index */
#define	GET_BIT(c,n)		(*((c)+((n)>>3)) >> (7-((n)&7))&1)
#define	SET_BIT(c,n)		(*((c)+((n)>>3)) |=   1<<(7-((n)&7)))
#define	CLEAR_BIT(c,n)		(*((c)+((n)>>3)) &= ~(1<<(7-((n)&7))))
#define	ISSET_BIT(c,n)		(*((c)+((n)>>3)) &    1<<(7-((n)&7)))

/* These macros clash with glib: */
#ifndef ABS
# define ABS(x)		(((x)>0)?(x):-(x))	/* Return absolute value */
#endif
#ifndef CLAMP
# define CLAMP(x,l,h)	(((x)>(h))?(h):(((x)>(l))?(x):(l)))	/* Return clamped value */
#endif

/* Rounding.
 * FLOOR() rounds to nearest integer towards 0.
 * CEILING() rounds to nearest integer away from 0.
 * ROUND() rounds to the nearest integer.
 */
#define FLOOR(X)	((X)>0 ? (int)(X) : -(int)(-(X)))
#define CEILING(X)	((X)==(int)(X) ? (X) : (X)>0 ? 1+(int)(X) : -(1+(int)(-(X))))
#define ROUND(X)        ((X)>0?(int)(0.5+(X)):-(int)(0.5-(X)))

#define SIGN(X)		(((X)<0) ? -1 : 1)		/* get sign, -1, or 1 if >= 0 */
#define SIGNZ(X)	(((X)<0) ? -1 : (X)>0 ? 1 : 0)	/* get sign, -1, 0, or 1 */

/*
 * Linear interpolation from l (at x=0) to h (at x=1).
 * This is equivalent to (x*h)+((1-x)*l).
 */
#define LERP(x,l,h)	((l)+(((h)-(l))*(x)))

/* Is this a GNU system? */
#if !defined(__GNUC__) && !defined(__INTEL_COMPILER)
/* No. */
#define __PRETTY_FUNCTION__ "<unavailable>"
#endif

/*
 * Wrappers around nasty, unportable, harsh breakpoints.
 */
#ifdef __GNUC__
# ifdef __i386__
#  define s_breakpoint	__asm__ __volatile__ ("int $03")
# else
#  ifdef __alpha__
#   define s_breakpoint	__asm__ __volatile__ ("bpt")
#  else
#   define s_breakpoint	abort()
#  endif
# endif
#else
# ifdef __INTEL_COMPILER
#  ifdef __i386__
#   define s_breakpoint	asm volatile ("int $03")
#  else
#   define s_breakpoint	abort()
#  endif
# else
#  define s_breakpoint	abort()
# endif
#endif

/*
 * Wrappers for multi-statement macros.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define MWRAP_BEGIN
#define MWRAP_END
#else
#define MWRAP_BEGIN	do
#define MWRAP_END	while(0==1);
#endif

/*
 * Development message macros
 */
#define message(X)	MWRAP_BEGIN {					\
			printf("MESSAGE: %s\n", (X));			\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			} MWRAP_END

#define message_if(X,Y)	if ((X)) {					\
			printf("MESSAGE: %s\n", (X));			\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			}

#define message_if_else(X,Y,Z)	MWRAP_BEGIN {				\
			printf("MESSAGE: %s\n", (X)?(Y):(Z));		\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			} MWRAP_END

#define s_assert(X)     MWRAP_BEGIN { if(!(X)) { 			\
			printf("Assertion \"%s\" failed:\n", (#X));	\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			s_breakpoint;					\
                        } } MWRAP_END

/*
 * die() macro, inspired by perl!
 */
#define die(X)	MWRAP_BEGIN {						\
                printf("FATAL ERROR: %s\nin %s at \"%s\" line %d\n",	\
		(X),					\
                __PRETTY_FUNCTION__,			\
                __FILE__,				\
                __LINE__);				\
		fflush(NULL);				\
                s_breakpoint;				\
		} MWRAP_END

/* Idea: Decrement warn() in favour of helga_log functionality? */
#define warn(X)         MWRAP_BEGIN {					\
                        printf("NON-FATAL ERROR: %s\n in %s at \"%s\" line %d\n",	\
				(X),					\
                               __PRETTY_FUNCTION__,			\
                               __FILE__,				\
                               __LINE__);				\
                        } MWRAP_END

/* #define helga_log(level, format, ...) MWRAP_BEGIN {          \
        if ( (level) <= helga_log_get_level() )         \
          helga_log_output(level, __PRETTY_FUNCTION__,  \
                           __FILE__, __LINE__,          \
                           format , ##__VA_ARGS__); } MWRAP_END
*/

/*
 * Implement my dief macro where possible.
 */
#if ( defined(__GNUC__) || defined(__INTEL_COMPILER) ) && !defined(__APPLE_CPP__) && !defined(__APPLE_CC__)
#  define dief(format, ...)	MWRAP_BEGIN {			\
			printf("FATAL ERROR: ");		\
			printf(format , ##__VA_ARGS__);		\
			printf("\nin %s at \"%s\" line %d\n",	\
			__PRETTY_FUNCTION__,			\
			__FILE__,				\
			__LINE__);				\
			fflush(NULL);				\
			s_breakpoint;                           \
			} MWRAP_END
# define HAVE_DIEF	1
#else
/*
 * void dief(const char *format, ...);
 * is defined in compatibility.c because vararg macros are not
 * implemented in many inferior compilers ;)
 */
# define HAVE_DIEF	0
#endif
		
#endif

