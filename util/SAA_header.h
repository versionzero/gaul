/**********************************************************************
  SAA_header.h
 **********************************************************************

  SAA_header.h - My general header to define a few useful things
  Copyright ©1999-2002, Stewart Adcock <stewart@linux-domain.com>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
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

  Updated:	18 Sep 2002 SAA	SUN_FORTE_C is sometimes set to 0 instead of being undefined.
  		09 Aug 2002 SAA	Use unicode encoding of the copyright symbol in comments.  Added constant MAX_FNAME_LEN.
  		07 Aug 2002 SAA	Modification of _Bool stuff for clean compilation using gcc version 3.2
  		01 Jul 2002 SAA	Use the GNU-extensions provided by the Intel C/C++ compiler.
		14 Jun 2002 SAA	Don't redefine TRUE,FALSE if already defined.
		31 May 2002 SAA	Clean compilation with Compaq's ccc compiler.
		14 May 2002 SAA	Adaptations for clean compilation with Sun's Forte Developer 6 C/C++ compilers.
		10 Apr 2002 SAA	Modified copyright notice.  Not all of my projects are GPL compatible, so I'm relaxing the licensing.
		20 Mar 2002 SAA	HAVE_DIEF was incorrectly defined when the dief() macro is unavailable.
		19 Mar 2002 SAA	Moved most parallel specific stuff to mpi_util.h
		15 Mar 2002 SAA	Moved parallel-aware die()/dief() macros to mpi_util.h and allowed over-ride of PARALLEL in config.h by definition of NO_PARALLEL constant.
  		26 Feb 2002 SAA	Added definition of ONE_MINUS_TINY.
		30 Jan 2002 SAA	Parallel versions of the die() and deif() macros do not directly call MPI routines now.
		29 Jan 2002 SAA Changes for removal of splint (http://www.splint.org/) warnings/errors.
		28 Jan 2002 SAA Minor modifications to play nicely with the Intel C/C++ compiler.  Needed a kludge to workaround a problem in the GNU make tools.
		17 Dec 2001 SAA	Boolean stuff is now handled in a much more portable way, and follows C99 where possible.
		30 Nov 2001 SAA	The constant DEBUG will always be defined now.
		29 Nov 2001 SAA	Added checks around definition of BYTEBITS so that it works on Solaris2.7 and others.
		07 Nov 2001 SAA	Added MINMAX macro.
		26/08/01 SAA	boolean is now short instead of int.  Bool and Boolean are now define'd to that instead of int too.  SWAP_BOOLEAN() macro added.
		11/06/01 SAA	Added 'byte' type definition for more readable code.  I Assume that the sizeof of a char is always 1 byte, but reading the ANSI definitions, that seems perfectly reasonable.
		30/04/01 SAA	Added ISTINY() macro.
		11/03/01 SAA	Added maybeinline definition.
		27/02/01 SAA	Added vpointer and constvpointer because I was, on occassion, including glib just for gpointer and gconstpointer!
		09/02/01 SAA	VERSION_STRING is now guaranteed to be defined.
		11/01/01 SAA	System headers are now included after the PARALLEL stuff is sorted out.  This is so that _REENTRANT is correctly defined prior to stdlib.h being included for multi-threaded applications.  If appropriate, _GNU_SOURCE is defined prior to system includes too.  BUILD_DATE constant renamed to BUILD_DATE_STRING for consistency in configure scripts.  If HAVE_CONFIG_H is defined, then include config.h header here.  Added ISSET_BIT() macro.
		04/01/00 SAA	die()/dief() now flush all open output streams.
		11/12/00 SAA	The Kai C++ compiler (KCC) doesn't seem to like the true/false declarations, so they are not made if LINUX_KAI_SOURCE is defined - this is going to cause problems.
		15/11/00 SAA	Replaced PARALLEL compiler constant boolean, integer valued constant.  Tweaks for SGI's MIPSPro CC compiler.
		27/10/00 SAA	'OKAY' redefined as '2' so that gprof works when exit(OKAY) is encountered.
		18/10/00 SAA	If this is parallel code, transparently include the MPI headers.
		17/10/00 SAA	Added specific versions of die() and deif() for parallel code (assumming MPI is used).
		26/09/00 SAA	Added LERP() for linear interpolation.
		08/07/00 SAA	dief() macro added.
		21/06/00 SAA	Added die() and warn() macros from trace_methods.h
		08/06/00 SAA	boolean defined using typedef instead of #define and TRUE/FALSE are now const booleans.  true/false additionally defined.  Compilers typechecking should be more useful now.
		14/04/00 SAA	Added SWAP_XXX() macros.  s_assert() message altered.
		11/04/00 SAA	Added DNEAR() and NDNEAR() macros.
		15/03/00 SAA	Added PI_OVER_TWO.
		17/02/00 SAA	Added GPL text.
		11/01/00 SAA	Added EMAIL constant.
		17/11/99 SAA	SQRT_TWO added for consistency.
		20/10/99 SAA	Added MIN3() and MAX3() macros.
		19/10/99 SAA	FOURTH_POW() and FIFTH_POW() macros added.
		10/10/99 SAA	Replaced "#ifndef __PRETTY_FUNCTION__" with "#ifndef __GNUC__".
		07/09/99 SAA	Added message macros.
		10/05/99 SAA	Added SQRT_PI.
		27/04/99 SAA	ApproxZero is now something sensible (As Danny pointed out, 1.0e-50 is a stupiddly small amount.)  I choose 1.0e-18 since that's slightly more than EPS.
		16/04/99 SAA	Added OKAY, cos I felt like it.
		30/03/99 SAA	If __PRETTY_FUNCTION__ hasn't been defined already (i.e. not a GNU system) set it to "<unavailable>" here.
		21/02/99 SAA	Added RAD2DEG and DEG2RAD constants.  Changed RAD() and DEG() appropriately.  NULL_CHAR constant added.
		05/02/99 SAA	s_where macro moved to methods/trace_methods.h
		01/02/99 SAA	Added 'VOID' definition.
		24/01/99 SAA	Added 'ROOT_TWO' definition.
		20/01/99 SAA	Some of my constants clashed with glib/gdk/gtk etc. so made some modifications.  Added CLAMP, it seems useful.
		12/01/99 SAA	Decided it's probably a good idea to keep a history for this file too!  Added BUILD_DATE definitions.

  To do:	replace die() and dief() with a single die() macro.

 **********************************************************************/

#ifndef SAA_HEADER_H_INCLUDED
#define SAA_HEADER_H_INCLUDED

/*
 * _GNU_SOURCE includes the _POSIX_SOURCE and _ISOC99_SOURCE
 * C extensions, which are normally fairly useful!
 * This has to be defined prior to the inclusion of any of the 
 * standard C headers.
 */
#if defined(__GNUC__)
# define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
# include "config.h"
/*
 * ICC kludge.  This needs fixing through the GNU make tools.
 */
# ifdef __INTEL_COMPILER
#  define HAVE_MEMCPY 1
#  define HAVE_STRLEN 1
#  define HAVE_STRNCPY 1
#  define HAVE_STRNCMP 1
# endif
#endif

#if PARALLEL!=1
/*
 * If threads are used, these must be properly defined somewhere.
 * Unfortunately empty macros cause splint parse errors.  They
 * also cause lots of warnings when using Sun's and Compaq's
 * compilers.
 */
# if defined(__GNUC__)
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
 * Build date - should be specified at compile time
 * i.e. in the Makefile have something like:
 * -DBUILD_DATE_STRING=`date`
 * or, even better, by the configuration script.
 */
#ifndef BUILD_DATE_STRING
#define BUILD_DATE_STRING	"<Unknown>\0"
#endif

/*
 * Set default version string, so we may rely upon it
 * being defined in our code.
 */
#ifndef VERSION_STRING
#define VERSION_STRING		"<Unknown>\0"
#endif

/*
 * Ensure that the DEBUG constant is set to something.
 */
#ifndef DEBUG
# define DEBUG	0
#endif

/*
 * Define boolean type sensibly.
 */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else

# if !defined(__bool_true_false_are_defined)
#  if !defined(_Bool) && !defined(__GNUC__)
typedef short _Bool;
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
#endif

#define bool _Bool
#define boolean _Bool

#if !defined(TRUE)
#define TRUE  1
#define FALSE 0
#endif

/*
 * Additional types.
 */
#if !defined(SUN_FORTE_C) || SUN_FORTE_C==0
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

#define SIGN(X)		(((X)<0) ? -1 : 0)		/* get sign, -1, or 1 if >= 0 */
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
 * Handy 'portable' version of inline.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define maybeinline     inline
#else
#define maybeinline     /* Oh well. */
#endif

/*
 * Development message macros
 */
#define message(X)	{						\
			printf("MESSAGE: %s\n", (X));			\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			}

#define message_if(X,Y)	if ((X)) {					\
			printf("MESSAGE: %s\n", (X));			\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			}

#define message_if_else(X,Y,Z)	{					\
			printf("MESSAGE: %s\n", (X)?(Y):(Z));		\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			}

#define s_assert(X)     { if(!(X)) { 					\
			printf("Assertion \"%s\" failed:\n", (#X));	\
			printf("(\"%s\", %s, line %d)\n",		\
			__FILE__, __PRETTY_FUNCTION__, __LINE__);	\
			s_breakpoint;					\
                        } }

/*
 * die() macro, inspired by perl!
 */
#define die(X)	{						\
                printf("FATAL ERROR: %s\nin %s at \"%s\" line %d\n",	\
		(X),					\
                __PRETTY_FUNCTION__,			\
                __FILE__,				\
                __LINE__);				\
		fflush(NULL);				\
                s_breakpoint;				\
                }

/* Idea: Decrement warn() in favour of helga_log functionality? */
#define warn(X)         {						\
                        printf("NON-FATAL ERROR: %s\n in %s at \"%s\" line %d\n",	\
				(X),					\
                               __PRETTY_FUNCTION__,			\
                               __FILE__,				\
                               __LINE__);				\
                        }

/* #define helga_log(level, format, args...) do {          \
        if ( (level) <= helga_log_get_level() )         \
          helga_log_output(level, __PRETTY_FUNCTION__,  \
                           __FILE__, __LINE__,          \
                           format, ##args); } while(0)
*/

/*
 * Implement my dief macro where possible.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#  define dief(format, args...)	{				\
			printf("FATAL ERROR: ");		\
			printf(format, ##args);			\
			printf("\nin %s at \"%s\" line %d\n",	\
			__PRETTY_FUNCTION__,			\
			__FILE__,				\
			__LINE__);				\
			fflush(NULL);				\
			s_breakpoint;                           \
			}
# define HAVE_DIEF	1
#else
/*
 * void dief(const char *format, ...);
 * is defined in compatibility.c because vararg macros are not
 * implemented in many inferior compilers ;)
 */
# undef HAVE_DIEF
#endif
		
#endif

