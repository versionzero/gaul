/**********************************************************************
  compatibility.h
 **********************************************************************

  compatibility - Compatibility/Portability stuff.
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

  Synopsis:	Compatibility/Portability stuff.

  Updated:    	19/03/01 SAA	Removed GNU specific stuff.
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

#if HAVE_IPOW == 0
int ipow(int n, int e);
#endif

#if HAVE_MEMCPY == 0
void memcpy(char *dest, const char *src, size_t len);
#endif

#if HAVE_STRCHR == 0
char *strchr(const char *str, int c);
#endif

#if HAVE_STRRCHR == 0
/* Some systems have rindex() instead */
#if HAVE_RINDEX == 1
#define strrchr rindex
#else
char *strrchr(const char *str, int c);
#endif
#endif

#if HAVE_STRCAT == 0
char *strcat(char *str1, const char *str2);
#endif

#if HAVE_STRLEN == 0
int strlen(const char *str);
#endif

#if HAVE_STRCMP == 0
int strcmp(const char *str1, const char *str2);
#endif

#if HAVE_STRNCMP == 0
int strncmp(const char *str1, const char *str2, const int len);
#endif

#if HAVE_STRCPY == 0
char *strcpy(char *str1, const char *str2);
#endif

#if HAVE_STRNCPY == 0
char *strncpy(char *str1, const char *str2, const int len);
#endif

#if HAVE_STRTOK == 0
char *strtok(char *str, char *delim);
#endif

#if HAVE_STRCASECMP==0
int strcasecmp(const char *str0, const char *str1);
#endif

#if HAVE_STRNCASECMP==0
int strncasecmp(const char *str0, const char *str1, size_t n);
#endif

#if HAVE_USLEEP==0
void usleep(unsigned long usec);
#endif

#if HAVE_STRLCPY==0
size_t strlcpy(char *dest, const char *src, size_t n);
#endif

#if HAVE_STRLCAT==0
size_t strlcat(char *dest, const char *src, size_t n);
#endif

#if HAVE_SNPRINTF==0
int snprintf(char *str, size_t n, const char *format, ...);
#endif

#if HAVE_VSNPRINTF==0
int vsnprintf(char *str, size_t n, const char *format, va_list ap);
#endif

#if HAVE_MEMSET==0
void *memset(void *dst0, int c0, size_t bytes);
#endif

#if HAVE_MEMMOVE==0
/* Some systems, such as SunOS do have BCOPY instead. */
#if HAVE_BCOPY==1
#define memmove(A, B, C) bcopy((A), (B), (C))
#else
void *memmove(void *dst, const void *src, size_t bytes);
#endif
#endif

#if HAVE_MEMREV==0
void *memrev(void *src, size_t bytes);
#endif

#if HAVE_MEMCHR==0
void *memchr(const void *src, int c, size_t bytes);
#endif

#if HAVE_MEMMEM==0
void *memmem(const void *haystack, size_t haystack_len,
           const void *needle,   size_t needle_len);
#endif

#if HAVE_MEMCMP==0
int memcmp(const void *src1, const void *src2, size_t n);
#endif

#if HAVE_STRDUP==0
char *strdup(const char *str);
#endif

#if HAVE_MEMDUP==0
void *memdup(const void *mem, int byte_size);
#endif

#if HAVE_STRNDUP==0
char *strndup(const char *str, int n);
#endif

#if HAVE_STRNFILL==0
char *strnfill(int length, char fill_char);
#endif

#if HAVE_STRCATV==0
char *strcatv(const char *string1, ...);
#endif

/*#if HAVE_STRTOD==0*/
#if 0
double strtod(const char *nptr, char **endptr);
#endif

#if HAVE_STRSIGNAL==0
char *strsignal(int signum);
#endif

#if HAVE_STRREV==0
void strrev(char *string);
#endif

#if HAVE_STRERROR==0
char *strerror(int errnum);
#endif

#if HAVE_DIEF==0
/* HAVE_DIEF is set in "SAA_header.h", not "config.h" */
void dief(const char *format, ...);
#endif

#if HAVE_BASENAME==0
char *basename (char *path);
#endif

#if HAVE_READLINE == 0
char *readline (char *prompt);
#endif

#if HAVE_STRCSPN == 0
size_t strcspn(const char *string, const char *reject);
#endif

#if HAVE_WAITPID == 0
pid_t waitpid(pid_t pid, int *pstatus, int options);
#endif

#if HAVE_MIN == 0
int min(int a, int b);
#endif

#if HAVE_MAX == 0
int max(int a, int b);
#endif

/*if HAVE_STRRUP == 0*/
/*if HAVE_STRICMP == 0*/
/*if HAVE_STRNICMP == 0*/

#if HAVE_SINCOS == 0
maybeinline void sincos( double radians, double *s, double *c );
#endif

#endif
