/**********************************************************************
  compatibility.h
 **********************************************************************

  compatibility - Compatibility/Portability stuff.
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

  Synopsis:	Compatibility/Portability stuff.

  Updated:	10 Apr 2002 SAA	Use bcopy() for memcpy(), when available.  Fixed memmove() bug.  Added memscan(), strpbrk() and strsep().
		13 Mar 2002 SAA	Use index() for strchr(), when available.
		10 Jan 2002 SAA	Removed stuff relating to strsplit(), strfreev(), strjoin(), strjoinv().  Added stuff for strspn().
	    	09 Jan 2002 SAA Reversed brain-dead change from 05 Dec 2001.  Uncommented strtod() stuff.
		05 Dec 2001 SAA Only explicitely requested things will be compiled now, i.e. needs HAVE_THING == 0.
		19/03/01 SAA	Removed GNU specific stuff.
		16/11/00 SAA	I liked the helga_compat.c version, so used it to replace methods/compatibility.c
		15/11/00 SAA	new version for helga_compat.c

 **********************************************************************/

#ifndef COMPATIBILITY_H_INCLUDED
#define COMPATIBILITY_H_INCLUDED

#include "SAA_header.h"

#include "config.h"				/* for HAVE_WHATEVER */

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
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include "SAA_header.h"

#include "memory_util.h"

/*
 * Debugging
 */
#ifndef COMPAT_DEBUG
#define COMPAT_DEBUG   2
#endif

/*
 * Prototypes
 */

#ifndef HAVE_IPOW
int ipow(int n, int e);
#endif

#ifndef HAVE_STRCHR
# ifdef HAVE_INDEX
#  define strchr index
# else
char *strchr(const char *str, int c);
# endif
#endif

#ifndef HAVE_STRRCHR
/* Some systems have rindex() instead */
# ifdef HAVE_RINDEX
#  define strrchr rindex
# else
char *strrchr(const char *str, int c);
# endif
#endif

#ifndef HAVE_STRCAT 
char *strcat(char *str1, const char *str2);
#endif

#ifndef HAVE_STRLEN
int strlen(const char *str);
#endif

#ifndef HAVE_STRCMP
int strcmp(const char *str1, const char *str2);
#endif

#ifndef HAVE_STRNCMP
int strncmp(const char *str1, const char *str2, const int len);
#endif

#ifndef HAVE_STRCPY
char *strcpy(char *str1, const char *str2);
#endif

#ifndef HAVE_STRNCPY
char *strncpy(char *str1, const char *str2, const int len);
#endif

#ifndef HAVE_STRTOK
char *strtok(char *str, char *delim);
#endif

#ifndef HAVE_STRPBRK
char *strpbrk(const char *s, const char *accept);
#endif

#ifndef HAVE_STRSEP
char *strsep(char **str, const char *delim);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *str0, const char *str1);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *str0, const char *str1, size_t n);
#endif

#ifndef HAVE_USLEEP
void usleep(unsigned long usec);
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t n);
#endif

#ifndef HAVE_STRLCAT
size_t strlcat(char *dest, const char *src, size_t n);
#endif

#ifndef HAVE_SNPRINTF
int snprintf(char *str, size_t n, const char *format, ...);
#endif

#ifndef HAVE_VSNPRINTF
int vsnprintf(char *str, size_t n, const char *format, va_list ap);
#endif

#ifndef HAVE_MEMCPY
/* Some systems, such as SunOS do have BCOPY instead. */
# ifdef HAVE_BCOPY
#  define memcpy(A, B, C) bcopy((B), (A), (C))
# else
void memcpy(char *dest, const char *src, size_t len);
# endif
#endif

#ifndef HAVE_MEMMOVE
/* Some systems, such as SunOS do have BCOPY instead. */
# ifdef HAVE_BCOPY
#  define memmove(A, B, C) bcopy((B), (A), (C))
# else
void *memmove(void *dst, const void *src, size_t bytes);
# endif
#endif

#ifndef HAVE_MEMSCAN
void *memscan(void *addr, int c, size_t size);
#endif

#ifndef HAVE_MEMSET
void *memset(void *dst0, int c0, size_t bytes);
#endif

#ifndef HAVE_MEMREV
void *memrev(void *src, size_t bytes);
#endif

#ifndef HAVE_MEMCHR
void *memchr(const void *src, int c, size_t bytes);
#endif

#ifndef HAVE_MEMMEM
void *memmem(const void *haystack, size_t haystack_len,
           const void *needle,   size_t needle_len);
#endif

#ifndef HAVE_MEMCMP
/* Some systems, such as SunOS do have BCMP instead. */
# ifdef HAVE_BCMP
#  define memcmp(A, B, C) bcmp((B), (A), (C))
# else
int memcmp(const void *src1, const void *src2, size_t n);
# endif
#endif

#ifndef HAVE_STRDUP
char *strdup(const char *str);
#endif

#ifndef HAVE_MEMDUP
void *memdup(const void *mem, int byte_size);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *str, int n);
#endif

#ifndef HAVE_STRNFILL
char *strnfill(int length, char fill_char);
#endif

#ifndef HAVE_STRCATV
char *strcatv(const char *string1, ...);
#endif

#ifndef HAVE_STRTOD
double strtod(const char *nptr, char **endptr);
#endif

#ifndef HAVE_STRSIGNAL
char *strsignal(int signum);
#endif

#ifndef HAVE_STRREV
void strrev(char *string);
#endif

#ifndef HAVE_STRERROR
char *strerror(int errnum);
#endif

#ifndef HAVE_DIEF
/*
 * HAVE_DIEF is set in "SAA_header.h", not "config.h"
 */
void dief(const char *format, ...);
#endif

#ifndef HAVE_BASENAME
char *basename(char *path);
#endif

#ifndef HAVE_READLINE
char *readline(char *prompt);
#endif

#ifndef HAVE_STRSPN
size_t strspn(const char *string, const char *accept);
#endif

#ifndef HAVE_STRCSPN
size_t strcspn(const char *string, const char *reject);
#endif

#ifndef HAVE_WAITPID
pid_t waitpid(pid_t pid, int *pstatus, int options);
#endif

#ifndef HAVE_MIN
int min(int a, int b);
#endif

#ifndef HAVE_MAX
int max(int a, int b);
#endif

#ifndef HAVE_STRUPR
char *strupr( char *s );
#endif

#ifndef HAVE_STRICMP
int stricmp( char *s1, char *s2 );
#endif

#ifndef HAVE_STRNICMP
int strnicmp( char *s1, char *s2, int n );
#endif

#ifndef HAVE_SINCOS
void sincos( double radians, double *s, double *c );
#endif

#ifndef HAVE_ITOA
void itoa(const int n, char *s);
#endif

#endif
