/************************************************************************
  str_match.c
 **********************************************************************

  str_match - String matching with wildcards.
  Copyright ©1999-2000, Stewart Adcock <stewart@linux-domain.com>
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

  Routines for string matching with simple wildcards.

  Wildcard rules:
		?	match any single character
		*	match any number of characters
		[<chars>]	match from set of characters

  Charmm wildcard rules:
	*	matches any string of characters (including none),
	%	matches any single character,
	#	matches any string of digits (including none),
	+	matches any single digit.

  Last Updated:	20/06/01 SAA	Added a few casts for clean compilation on Solaris.
		10/03/00 SAA	Added version which uses CHARMM's atom selection wildcards.
		20/10/99 SAA	Tidied from some of my old Amiga code!!!

  To do:	Make "[blah]" stuff more sophisticated.
		Incorporate into str_util.c

 ***********************************************************************/

#include "gaul/str_match.h"

boolean str_match(char *str, char *pat)
  {
  char		*str_end, *pat_end;	/* Pointers to the ends of string and pattern */

  if (!str || !pat)
    {
    printf("str_match(): WARNING: Null pointer to string passed\n");
    return (FALSE);
    }

/* Find extent of string and pattern */
  str_end = str;
  while (*str_end != '\0') str_end++;

  pat_end = pat;
  while (*pat_end != '\0') pat_end++;

  return( recursive_str_match(str, --str_end, pat, --pat_end) );
  }


boolean recursive_str_match(char *str_start, char *str_end, char *pat_start, char *pat_end)
  {
  boolean	match;

/* Match from start of pattern */
  while (*pat_start != '*')
    {
    if (pat_start > pat_end) return(str_start > str_end);

    if (str_start > str_end) return(FALSE);
    
    if (*pat_start == '[')
      {
      match = FALSE;
      pat_start++;
      while (pat_start <= pat_end && *pat_start != ']')
        {
        if (*pat_start == *str_start) match = TRUE;
        pat_start++;
        }
      if (!match) return(FALSE);
      }
    else if (*pat_start != '?' && *pat_start != *str_start)
      {
      return(FALSE);
      }

    str_start++;
    pat_start++;
    }

/* Match from end of pattern */
  while(*pat_end != '*')
    {
    if (pat_start > pat_end) return(str_start > str_end);

    if (str_start > str_end) return(FALSE);
    
    if (*pat_end == ']')
      {
      match = FALSE;
      pat_end--;
      while (pat_end >= pat_start && *pat_end != '[')
        {
        if (*pat_end == *str_end) match = TRUE;
        pat_end--;
        }
      if (!match) return(FALSE);
      }
    else if (*pat_end != '?' && *pat_end != *str_end)
      {
      return(FALSE);
      }

    str_end--;
    pat_end--;
    }

/* Is the pattern a single '*'? */
  if (pat_start == pat_end) return(TRUE);
  
/*
 * The pattern must have '*' at beginning and at end.  So skip
 * through string until we get a complete match with the stuff inbetween.
 */
  pat_start++;

  while (str_start <= str_end)
    {
    if ( recursive_str_match(str_start, str_end, pat_start, pat_end) ) return(TRUE);

    str_start++;
    }

  return(FALSE);
  }


boolean str_match_charmm(char *str, char *pat)
  {
  char		*str_end, *pat_end;	/* Pointers to the ends of string and pattern */

  if (!str || !pat)
    {
    printf("str_match_charmm(): WARNING: Null pointer to string passed\n");
    return (FALSE);
    }

/* Find extent of string and pattern */
  str_end = str;
  while (*str_end != '\0') str_end++;

  pat_end = pat;
  while (*pat_end != '\0') pat_end++;

  return( recursive_str_match_charmm(str, --str_end, pat, --pat_end) );
  }


boolean recursive_str_match_charmm(char *str_start, char *str_end, char *pat_start, char *pat_end)
  {
/*  boolean	match;*/

/* Match from start of pattern */
  while (*pat_start != '*' || *pat_start != '#')
    {
    if (pat_start > pat_end) return(str_start > str_end);

    if (str_start > str_end) return FALSE;
    
    if (*pat_start != '%' && *pat_start != *str_start)
      {
      if (*pat_start == '+' && !isdigit((int)*str_start)) return FALSE;
      }

    str_start++;
    pat_start++;
    }

/* Match from end of pattern */
  while(*pat_end != '*' || *pat_end != '#')
    {
    if (pat_start > pat_end) return(str_start > str_end);

    if (str_start > str_end) return FALSE;
    
    if (*pat_end != '%' && *pat_end != *str_end)
      {
      if (*pat_end == '+' && !isdigit((int)*str_end)) return FALSE;
      }

    str_end--;
    pat_end--;
    }

/* Is the pattern a single '*' or '#'? */
  if (pat_start == pat_end)
    {
    if (*pat_start == '*') return(TRUE);

/* Pattern is a single '#', and the string contains only digits? */
    while (str_start <= str_end)
      {
      if (!isdigit((int)*str_start)) return(FALSE);
      str_start++;
      }
    return(TRUE);
    }
  
/*
 * The pattern must have '*' or '#' at beginning and at end.  So skip
 * through string until we get a complete match with the stuff inbetween.
 */
  pat_start++;

  while (str_start <= str_end)
    {
    if ( recursive_str_match_charmm(str_start, str_end, pat_start, pat_end) ) return(TRUE);

    str_start++;
    }

  return(FALSE);
  }


