/**********************************************************************
  compatibility.h
 **********************************************************************

  compatibility - Compatibility/Portability stuff.
  Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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
int ipow(int n, int e);
#endif

#if HAVE_DPOW != 1
double dpow(double n, int e);
#endif

#if HAVE_STRCHR != 1
# if HAVE_INDEX == 1
#  define strchr index
# else
char *strchr(const char *str, int c);
# endif
#endif

#if HAVE_STRRCHR != 1
/* Some systems have rindex() instead */
# if HAVE_RINDEX == 1
#  define strrchr rindex
# else
char *strrchr(const char *str, int c);
# endif
#endif

#if HAVE_STRCAT  != 1
char *strcat(char *str1, const char *str2);
#endif

#if HAVE_STRLEN != 1
size_t strlen(const char *str);
#endif

#if HAVE_STRCMP != 1
int strcmp(const char *str1, const char *str2);
#endif

#if HAVE_STRNCMP != 1
int strncmp(const char *str1, const char *str2, size_t len);
#endif

#if HAVE_STRCPY != 1
char *strcpy(char *str1, const char *str2);
#endif

#if HAVE_STRNCPY != 1
char *strncpy(char *str1, const char *str2, size_t len);
#endif

#if HAVE_STRTOK != 1
char *strtok(char *str, const char *delim);
#endif

#if HAVE_STRPBRK != 1
char *strpbrk(const char *s, const char *accept);
#endif

#if HAVE_STRSEP != 1
char *strsep(char **str, const char *delim);
#endif

#if HAVE_STRCASECMP != 1
int strcasecmp(const char *str0, const char *str1);
#endif

#if HAVE_STRNCASECMP != 1
int strncasecmp(const char *str0, const char *str1, size_t n);
#endif

#if HAVE_USLEEP != 1
/* FIXME: Need to add Win32 version of this code. */
void usleep(unsigned long usec);
#endif

#if HAVE_STRLCPY != 1
size_t strlcpy(char *dest, const char *src, size_t n);
#endif

#if HAVE_STRLCAT != 1
size_t strlcat(char *dest, const char *src, size_t n);
#endif

#if HAVE_SNPRINTF != 1
int snprintf(char *str, size_t n, const char *format, ...);
#endif

#if HAVE_VSNPRINTF != 1
int vsnprintf(char *str, size_t n, const char *format, va_list ap);
#endif

#if HAVE_MEMCPY != 1
/* Some systems, such as SunOS do have BCOPY instead. */
# if HAVE_BCOPY == 1
#  define memcpy(A, B, C) bcopy((B), (A), (C))
# else
void memcpy(char *dest, const char *src, size_t len);
# endif
#endif

#if HAVE_MEMMOVE != 1
/* Some systems, such as SunOS do have BCOPY instead. */
# if HAVE_BCOPY == 1
#  define memmove(A, B, C) bcopy((B), (A), (C))
# else
void *memmove(void *dst, const void *src, size_t bytes);
# endif
#endif

#if HAVE_MEMSCAN != 1
void *memscan(void *addr, int c, size_t size);
#endif

#if HAVE_MEMSET != 1
void *memset(void *dst0, int c0, size_t bytes);
#endif

#if HAVE_MEMREV != 1
void *memrev(void *src, size_t bytes);
#endif

#if HAVE_MEMCHR != 1
void *memchr(const void *src, int c, size_t bytes);
#endif

#if HAVE_MEMMEM != 1
void *memmem(const void *haystack, size_t haystack_len,
           const void *needle,   size_t needle_len);
#endif

#if HAVE_MEMCMP != 1
/* Some systems, such as SunOS do have BCMP instead. */
# if HAVE_BCMP == 1
#  define memcmp(A, B, C) bcmp((B), (A), (C))
# else
int memcmp(const void *src1, const void *src2, size_t n);
# endif
#endif

#if HAVE_STRDUP != 1
char *strdup(const char *str);
#endif

#if HAVE_MEMDUP != 1
void *memdup(const void *mem, int byte_size);
#endif

#if HAVE_STRNDUP != 1
char *strndup(const char *str, size_t n);
#endif

#if HAVE_STRNFILL != 1
char *strnfill(int length, char fill_char);
#endif

#if HAVE_STRCATV != 1
char *strcatv(const char *string1, ...);
#endif

#if HAVE_STRREV != 1
void strrev(char *string);
#endif

#if HAVE_DIEF != 1
/*
 * HAVE_DIEF is set in "SAA_header.h", not "config.h"
 */
void dief(const char *format, ...);
#endif

#if HAVE_STRSPN != 1
size_t strspn(const char *string, const char *accept);
#endif

#if HAVE_STRCSPN != 1
size_t strcspn(const char *string, const char *reject);
#endif

#if HAVE_WAITPID != 1 && !defined( W32_CRIPPLED )
/* FIXME: Need to add Win32 version of this code. */
pid_t waitpid(pid_t pid, int *pstatus, int options);
#endif

#if HAVE_MIN != 1
int min(int a, int b);
#endif

#if HAVE_MAX != 1
int max(int a, int b);
#endif

#if HAVE_SINCOS != 1
void sincos( double radians, double *s, double *c );
#endif

#if HAVE_GETHOSTNAME != 1
int gethostname(char *name, size_t len);
#endif

#endif /* COMPATIBILITY_H_INCLUDED */


