/**********************************************************************
  str_util.h
 **********************************************************************

  str_util - Portable string handling, analysis and manipulation library.
  Copyright ©1999-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Header file for my general string utility routines

  Updated:	02 Jan 2003 SAA	str_toupper() defined twice, whilst str_tolower() was nopt defined at all.
  		25 Mar 2002 SAA	Introduced STR_MAX_TOKENS.
  		16 Mar 2002 SAA Check for presence of limits.h on this system.
		10 Jan 2002 SAA	Added str_split(), str_freev(), str_join() and str_joinv() prototypes.
		18/09/00 SAA	Tidied.
		10/05/99 SAA	Added missing prototypes.
		20/02/99 SAA	Collected together from various files.

  To do:	Add M. Ryan's string tokeniser routines.
		Update prototypes.

 **********************************************************************/

#ifndef STR_UTIL_H_INCLUDED
#define STR_UTIL_H_INCLUDED

#include "gaul_util.h"

#include <ctype.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory_util.h"

#include "linkedlist.h"

/* Debugging stuff */
/* STR_UTIL_DEBUG sets debug level for code.
   0 = no debug code compiled.
   1 = some debug code.
   2 = most debug code.
   3+ = probably all debug code.
*/
#ifndef STR_UTIL_DEBUG
#define STR_UTIL_DEBUG	0
#endif

/*
 * Compilation constants.
 */
#ifdef HAVE_LIMITS_H
#define STR_MAX_TOKENS	INT_MAX
#else
#define STR_MAX_TOKENS	0xFFFFFF
#endif

/*
 * Prototypes.
 */
int	str_cpos(const char *str, const char c);
int	str_cposr(const char *str, const char c);
void	str_toupper(char *str);
void	str_tolower(char *str);
boolean	str_isblank(const char *str);
int	str_stripnewline(char *str);
int	str_qhash(const char *str);
void	str_ctranslate(char *str, char find, char replace);
int	str_tr(char *str, int len, char find, char replace);
char	*str_clone(const char *str);
char	*rjust(char *str);
char	*rtrim(char *str);
char	*ltrim(char *str);
char	*fill_space(char *dest, char *in, int gesamtl);
void	str_remove_spaces(char *str);
void	str_remove_blanks(char *str);
char	*str_rev(char *str);
int	substring_count(char *needle, char *haystack);
char	*str_replace(char*, char*, char*);
int	str_replaceall(char*, char*, char*);
char	*str_stripspace(const char *str);
int	str_cmp_nocase(const char *s1, const char *s2);
void	str_ncpyt(char *dest, const char *src, const int len);
char	*str_sncpy(char *dest, char *src, const int len);
char	*str_nclone(const char *str, const int len);
char	*str_getline(FILE *fp, int *len);
int	str_nreadline(FILE *fp, const int len, char *dest);
int	str_countchar(char *str, char c);
int	str_ncatf(char *str, size_t n, const char *format, ...);
char	*str_cat_va(char *str, va_list ap);
char	*str_cat(char *str, ...);
int	str_safecopy(char *src, char *dest, const int len);
int	str_scmp(const char *s1, const char *s2);
char	**str_split(const char *string, const char *delimiter, int max_tokens);
void	str_freev(char **str_array);
char	*str_joinv(const char *separator, char **str_array);
char	*str_join(const char *separator, ...);


#endif

