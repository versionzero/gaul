/**********************************************************************
  SAA_header.h
 **********************************************************************

  SAA_header.h - My general header to define a few useful things
  Copyright ©1999-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

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

  Updated:	30 Nov 2001 SAA	The constant DEBUG will always be defined now.
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
		11/12/00 SAA	The Kai C++ compiler (KCC) doesn't sem to like the true/false declarations, so they are not made if LINUX_KAI_SOURCE is defined - this is going to cause problems.
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
#ifdef __GNUC__
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* PARALLEL library to use.
 * In most cases, we just wish to check whether PARALLEL > 0,
 * i.e. this is a parallel program.
 *
 * PARALLEL==0/undefined	None.
 * PARALLEL==1			Pthreads.
 * PARALLEL==2			MPI 1.2.x
 * PARALLEL==3			PVM
 * PARALLEL==4			BSP
 * PARALLEL==5			OpenMP
 *
 * Transparently include MPI/whatever header in all files if this is parallel
 * code.
 * (This is required for the parallel version of die() and dief() macros -
 *  especially in the case of serial routines used by parallel programs.)
 */
#ifndef PARALLEL
#define PARALLEL	0
#endif
#if PARALLEL==1
#include <pthread.h>
#define _REENTRANT
#endif
#if PARALLEL==2
#include <mpi.h>
#endif
#if PARALLEL==3
#include <pvm3.h>
#endif
#if PARALLEL==4
#include <bsp.h>
#endif
#if PARALLEL==5
#include <omp.h>
#endif

#if PARALLEL!=1
/* If threads are used, these must be properly defined somewhere. */
#define THREAD_LOCK_DEFINE_STATIC(name)
#define THREAD_LOCK_DEFINE(name)
#define THREAD_LOCK_EXTERN(name)
#define THREAD_LOCK(name)
#define THREAD_UNLOCK(name)
#define THREAD_TRYLOCK(name)		0
#endif

/*
 * Includes needed for this stuff.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* Me */
#define AUTHOR	"Stewart Adcock"
#define EMAIL	"<stewart@bellatrix.pcl.ox.ac.uk>"

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
 * Types
 * Boolean and bool should both be deprecated.
 */
#if _USE_C99_SOURCE
typedef _Bool boolean;
#else
typedef short boolean;
#endif

#define Boolean         boolean
#define bool		boolean
/* am I getting lazier? */

#ifndef FALSE
#if 0
static const boolean FALSE = (0!=0);	/* 0 */
static const boolean TRUE  = (0==0);	/* 1 */
#endif
#define FALSE	(0)
#define TRUE	(1)
#endif
#ifndef LINUX_KAI_SOURCE
# ifndef true
static const boolean false = (0!=0);	/* 0 */
static const boolean true  = (0==0);	/* 1 */
# endif
#endif

typedef void* vpointer;
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
#define MAX_LINE_LENGTH 255
#define MAX_LINE_LEN	255
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
#ifndef __GNUC__
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
# define s_breakpoint	abort()
#endif

/*
 * Handy 'portable' version of inline.
 */
#ifdef __GNUC__
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
#if PARALLEL==2
#define die(X)          {						\
			int flubberrank;			\
			MPI_Comm_rank(MPI_COMM_WORLD, &flubberrank);		\
                        printf(							\
		"FATAL ERROR: (process %d) %s\nin %s at \"%s\" line %d\n",	\
				flubberrank,				\
				(X),					\
                               __PRETTY_FUNCTION__,			\
                               __FILE__,				\
                               __LINE__);				\
                        MPI_Abort(MPI_COMM_WORLD, 2);			\
			fflush(NULL);					\
                        }
#else
#define die(X)	{						\
                printf("FATAL ERROR: %s\nin %s at \"%s\" line %d\n",	\
		(X),					\
                __PRETTY_FUNCTION__,			\
                __FILE__,				\
                __LINE__);				\
		fflush(NULL);				\
                s_breakpoint;				\
                }
#endif

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

#ifndef __GNUC__
/*
 * void dief(const char *format, ...);
 * is defined in compatibility.c because vararg macros are not
 * implemented in many inferior compilers ;)
 */
#define HAVE_DIEF	0
#else
#define HAVE_DIEF	1
#if PARALLEL==2
#define dief(format, args...)	{				\
			int flubberrank;			\
			MPI_Comm_rank(MPI_COMM_WORLD, &flubberrank);		\
			printf("FATAL ERROR: (process %d) ", flubberrank);	\
			printf(format, ##args);			\
			printf("\nin %s at \"%s\" line %d\n",	\
			__PRETTY_FUNCTION__,			\
			__FILE__,				\
			__LINE__);				\
			fflush(NULL);				\
                        MPI_Abort(MPI_COMM_WORLD, 2);		\
			}
#else
#define dief(format, args...)	{				\
			printf("FATAL ERROR: ");		\
			printf(format, ##args);			\
			printf("\nin %s at \"%s\" line %d\n",	\
			__PRETTY_FUNCTION__,			\
			__FILE__,				\
			__LINE__);				\
			fflush(NULL);				\
			s_breakpoint;                           \
			}
#endif
#endif
		
#endif
