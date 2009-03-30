/**********************************************************************
  compatibility.h
 **********************************************************************

  compatibility - Compatibility/Portability stuff.
  Copyright ©2000-2009, Stewart Adcock (http://saa.dyndns.org/)
  All rights reserved.

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

  Synopsis:	Compatibility/Portability stuff.

 **********************************************************************/

#ifndef COMPATIBILITY_H_INCLUDED
#define COMPATIBILITY_H_INCLUDED

#include "gaul/gaul_util.h"

/* Includes */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>		/* usleep, fcntl */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include "gaul/memory_util.h"

/*
 * Debugging
 */
#ifndef COMPAT_DEBUG
#define COMPAT_DEBUG   2
#endif

/*
 * Prototypes
 */

#if HAVE_IPOW != 1
GAULFUNC int ipow(int n, int e);
#endif

#if HAVE_DPOW != 1
GAULFUNC double dpow(double n, int e);
#endif

#if !defined(HAVE_STRCHR)
# if defined(HAVE_INDEX)
#  define strchr index
# else
GAULFUNC char *strchr(const char *str, int c);
# endif
#endif

#if !defined(HAVE_STRLEN)
GAULFUNC size_t strlen(const char *str);
#endif

#if !defined(HAVE_STRCMP)
GAULFUNC int strcmp(const char *str1, const char *str2);
#endif

#if !defined(HAVE_STRNCMP)
GAULFUNC int strncmp(const char *str1, const char *str2, size_t len);
#endif

#if !defined(HAVE_STRCPY)
GAULFUNC char *strcpy(char *str1, const char *str2);
#endif

#if !defined(HAVE_STRNCPY)
GAULFUNC char *strncpy(char *str1, const char *str2, size_t len);
#endif

#if !defined(HAVE_STRPBRK)
GAULFUNC char *strpbrk(const char *s, const char *accept);
#endif

#if !defined(HAVE_STRSEP)
GAULFUNC char *strsep(char **str, const char *delim);
#endif

#if !defined(HAVE_STRCASECMP)
GAULFUNC int strcasecmp(const char *str0, const char *str1);
#endif

#if !defined(HAVE_STRNCASECMP)
GAULFUNC int strncasecmp(const char *str0, const char *str1, size_t n);
#endif

#if !defined(HAVE_USLEEP)
GAULFUNC void usleep(unsigned long usec);
#endif

#if !defined(HAVE_MEMCPY)
/* Some systems, such as SunOS do have BCOPY instead. */
# if defined(HAVE_BCOPY)
#  define memcpy(A, B, C) bcopy((B), (A), (C))
# else
GAULFUNC void memcpy(char *dest, const char *src, size_t len);
# endif
#endif

#if !defined(HAVE_MEMCMP)
/* Some systems, such as SunOS do have BCMP instead. */
# if defined(HAVE_BCMP)
#  define memcmp(A, B, C) bcmp((B), (A), (C))
# else
GAULFUNC int memcmp(const void *src1, const void *src2, size_t n);
# endif
#endif

#if !defined(HAVE_STRDUP)
GAULFUNC char *strdup(const char *str);
#endif

#if !defined(HAVE_STRNDUP)
GAULFUNC char *strndup(const char *str, size_t n);
#endif

#if HAVE_DIEF != 1
/*
 * HAVE_DIEF is set in "SAA_header.h", not "config.h"
 */
GAULFUNC void dief(const char *format, ...);
#endif

#if !defined(HAVE_WAITPID) && !defined( W32_CRIPPLED )
/* FIXME: Need to add a Windows compatible version of this function. */
GAULFUNC pid_t waitpid(pid_t pid, int *pstatus, int options);
#endif

#if !defined(HAVE_MIN)
GAULFUNC int min(int a, int b);
#endif

#if !defined(HAVE_MAX)
GAULFUNC int max(int a, int b);
#endif

#if !defined(HAVE_SINCOS)
GAULFUNC void sincos( double radians, double *s, double *c );
#endif

#if !defined(HAVE_GETHOSTNAME)
GAULFUNC int gethostname(char *name, size_t len);
#endif

#endif /* COMPATIBILITY_H_INCLUDED */


