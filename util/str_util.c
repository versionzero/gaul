/**********************************************************************
  str_util.c
 **********************************************************************

  str_util - Portable string handling, analysis and manipulation library.
  Copyright ©1999-2003, Stewart Adcock <stewart@linux-doamin.com>
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

  Some of these functions came from Babel 1.6, the source code to
  which I believe is in the public domain.  These functions have been
  clearly marked in the code below.  The Babel documentation states:

  Copyright (C) 1992-1996, Pat Walters and Matt Stahl
  babel@mercury.aichem.arizona.edu

  This software is provided on an "as is" basis, and without warranty of any
  kind, including but not limited to any implied warranty of merchantability
  or fitness for a particular purpose.

  In no event shall the authors or the University of Arizona be liable for
  any direct, indirect, incidental, special, or consequential damages arising
  from use or distribution of this software. The University of Arizona also
  shall not be liable for any claim against any user of this program by any
  third party.

 **********************************************************************

  Synopsis:	A collection of miscellaneous string-based routines.
		These are (mostly) coded in ANSI C to enable their use
		on platforms which may not have native equivalents.

  Last Updated:	05 Jan 2003 SAA	Added a cast to avoid a compiler warning.
		08 Jan 2003 SAA	Minor fixes resulting from use of splint ( http://www.splint.org/ ).  Use s_strdup() instead of strdup(), which is a GNU extension.  str_nreadline() optimised slightly, and one parameter changed to const.  New str_rev().
  		29 Oct 2002 SAA	Enhanced str_sncpy() and str_scmp().
		28 Oct 2002 SAA Added str_sncpy().
		15 Oct 2002 SAA Avoid an unsigned comparison warning when using Compaq's ccc.
		13 Aug 2002 SAA	In str_scmp(), trailing NULL char is considered to be the same as a trailing space.
  		25 Mar 2002 SAA Introduced STR_MAX_TOKENS.
  		10 Jan 2002 SAA	Added str_split(), str_freev(), str_join(), and str_joinv() based on code recently removed from methods/compatiability.c
		20/06/01 SAA	Added a few casts for clean compilation on Solaris.
		23/03/01 SAA	Added str_scmp().
		09/01/01 SAA	str_stripnewline() now returns the length of the string returned.
		24/09/00 SAA	Added str_tr().
		18/09/00 SAA	Renamed str_hash() to str_qhash() to avoid name collision with the much much superior function in str_hash.c
		08/07/00 SAA	Removed reliance on my trace library.
		19/05/00 SAA	Added loads of miscellaneous functions for concatenating strings: str_ncatf(), str_cat(), str_cat_va().  Added str_safecopy().  These functions are all for planned improvements to my dstr_util library.
		11/04/00 SAA	Added str_countchar().
		17/08/99 SAA	Fixed bug in str_getline() and str_nreadline() causing hang when EOF reached.
		13/08/99 SAA	Added str_nclone(), str_nreadline() and str_getline().
		14/04/99 SAA	Added str_ncpyt().
		30/03/99 SAA	Added some char to int casts to enable compilatiby cc under IRIX.
		05/03/99 SAA	Added str_cmp_nocase().
		03/03/99 SAA	str_stripspace() added.  Modified str_rev().  Shortened str_clone().
		02/03/99 SAA	str_replaceall() now returns an int instead of void.
		01/03/99 SAA	Added str_cpos() and str_cposr().  Renamed str_translate() to str_ctranslate(), for future naming consistency.  Improved str_toupper() and str_tolower(), in terms of speed.
		20/02/99 SAA	Collected together from various places, with additional inclusion of my memory tracing codes.

  To do:	Add my tokeniser routines.  (Or Dominic Ryan's?)
		Add my string matching routines.

		Macros to replace standard:
		strcat, strdup, strncat, strcmp, strncmp, strcpy, strncpy,
		strlen, strchr, strrchr, strpbrk, strspn, strcspn, strtok,
		strstr, strcasecmp, strncasecmp, index, rindex, strtok_r,
		strndup etc.?

		strspn (or strcspn) returns the length of the initial segment
		of string s1 which consists entirely of characters from
		(not from) string s2.  these are needed...

 **********************************************************************/

#include "gaul/str_util.h"

/**********************************************************************
  int str_cpos()
  synopsis:	Find first occurance of a character in a string.
		Returns index, or -1 if not found.
  parameters:   const	char	*str	The string.
                const	char	c	The character.
  return:	Position of character in string.
  last updated: 20/02/99
 **********************************************************************/

int str_cpos(const char *str, const char c)
  {
  int	pos=0;		/* Postion in string */

  if (!str) die("Null string pointer passed.\n");
  if (c == '\0') die("Null character passed.\n");

  while(str[pos] != '\0')
    {
    if (str[pos] == c) return(pos);
    pos++;
    }

  return(-1);
  }


/**********************************************************************
  int str_cposr()
  synopsis:	Find last occurance of a character in a string.
		Returns index, or -1 if not found.
  parameters:   const	char	*str	The string.
                const	char	c	The character.
  return:	Position of character in string.
  last updated: 20/02/99
 **********************************************************************/

int str_cposr(const char *str, const char c)
  {
  int	pos;		/* Postion in string */

  if (!str) die("NULL string pointer passed.\n");
  if (c == '\0') die("Null character passed.\n");

  pos = (int) strlen(str);

  while(pos>0)
    {
    pos--;
    if (str[pos] == c) return pos;
    }

  return -1;
  }


/**********************************************************************
  void str_toupper()
  synopsis:	Convert all lowercase characters to uppercase characters.
  parameters:   char	*str	The string.
  return:       None.
  last updated: 01/03/99
 **********************************************************************/

void str_toupper(char *str)
  {

  if (!str) die("NULL string pointer passed.\n");

  while( *str != '\0' )
    {
/*    if ( isalpha(*str) && islower(*str) ) *str = toupper(*str);*/
    if ( *str>='a' && *str<='z' ) *str = toupper(*str);
    str++;
    }

  return;
  }


/**********************************************************************
  void str_tolower()
  synopsis:	Convert all uppercase characters to lowercase characters.
  parameters:   char	*str	The string.
  return:       None.
  last updated: 01/03/99
 **********************************************************************/

void str_tolower(char *str)
  {

  if (!str) die("NULL string pointer passed.\n");

  while( *str != '\0' )
    {
/*    if ( isalpha(*str) && isupper(*str) ) *str = tolower(*str);*/
    if ( *str>='A' && *str<='Z' ) *str = tolower(*str);
    str++;
    }

  return;
  }


/**********************************************************************
  int str_stripnewline()
  synopsis:	Remove final '\n' character from a string.
  parameters:   char	*str	The string.
  return:       number of characters in string.
  last updated: 20/02/99
 **********************************************************************/

int str_stripnewline(char *str)
  {
  int	last;	/* Pointer to last character in string (excl. '\0') */

  if (!str) die("NULL string pointer passed.\n");

  last = (int) strlen(str)-1;
  if ( str[last] == '\n' ) str[last] = '\0';

  return last;
  }


/**********************************************************************
  boolean str_isblank()
  synopsis:	Returns TRUE, if string contains only white-space.
		(white-space includes: tab, space, newline)
		NB/ This is different to the POSIX definition of
		white-space.
  parameters:   char	*str	The string.
  return:	boolean
  last updated: 20/02/99
 **********************************************************************/

boolean str_isblank(const char *str)
  {

  if (!str) die("NULL string pointer passed.\n");

  while( *str != '\0' )
    {
    if ( *str != ' ' && *str != '\t' && *str != '\n' ) return(FALSE);
    str++;
    }

  return(TRUE);
  }


/**********************************************************************
  boolean str_eq()
  synopsis:	Returns TRUE, if strings match exactly, upto and
		including final null-char.
  parameters:   char	*str	The string.
  return:	boolean
  last updated: 20/02/99
 **********************************************************************/

boolean str_eq(const char *str1, const char *str2)
  {
  if (!str1 || !str2) die("null string pointer passed");

  while (*str1) if (*str1++ != *str2++) return FALSE;
  return (*str2 == '\0');
  }


/**********************************************************************
  boolean str_eqn()
  synopsis:	Returns TRUE, if strings match exactly, upto a
		given length.
  parameters:   char	*str	The string.
  return:	boolean
  last updated: 20/02/99
 **********************************************************************/

boolean str_eqn(const char *str1, const char *str2, const int n)
  {
  int ptr;

  if (!str1 || !str2) die("null string pointer passed");
  if (n < 1) die("Can't compare less than one character");

  for (ptr = 0; ptr < n; ptr++)
    {
    if (*str1 == '\0') return(*str2 == '\0');
    if (*str1++ != *str2++) return(FALSE);
    }

  return(TRUE);
  }


/**********************************************************************
  boolean str_eq_min()
  synopsis:	Returns TRUE, if strings match exactly, upto the
		length of the shortest.
  parameters:   char	*str	The string.
  return:	boolean
  last updated: 20/02/99
 **********************************************************************/

boolean str_eq_min(const char *str1, const char *str2)
  {
  if (!str1 || !str2) die("null string pointer passed");

  while (*str1 && *str2) if (*str1++ != *str2++) return (FALSE);
  return (TRUE);
  }


/**********************************************************************
  void str_fill_blanks()
  synopsis:	Fills a string with a given character.
  parameters:   char	*str	The string.
  return:	none
  last updated: 20/02/99
 **********************************************************************/

void str_fill_blanks(char *str, const char c, const int length)
  {
  if (str) die("null string pointer passed");
  if (length < 0) die("That's a dumb length");

  memset(str, c, length);
  str[length] = '\0';
  }

/*
 * Handy macro for case that char is a space.
 */
#define str_fill_spaces(X, Y)	str_fill_blanks(X, ' ', Y)


/**********************************************************************
  void str_remove_spaces()
  synopsis:	Removes all 'space' characters from the string, thus
		shortening it.
  parameters:   char	*str	The string.
  return:	none
  last updated: 20/02/99
 **********************************************************************/

void str_remove_spaces(char *str)
  {
  char *p;

  if (!str) die("Null pointer to string passed");

  for (p = str; *p; p++) if (*p != ' ') *str++ = *p;
  *str = '\0';
  }


/**********************************************************************
  void str_remove_spaces()
  synopsis:	Removes all POSIX white-space characters from the
                string, thus shortening it.
  parameters:   char	*str	The string.
  return:	none
  last updated: 20/02/99
 **********************************************************************/

void str_remove_whitespace(char *str)
  {
  char *p;

  if (!str) die("Null pointer to string passed.");

  for (p = str; *p; p++) if (!isspace((int)*p)) *str++ = *p;
  *str = '\0';
  }


/**********************************************************************
  char *str_clone(char *str)
  synopsis:	Copy contents of string into new string.
  parameters:   const	char	*str		The string.
  return:	New string.
  last updated: 03/03/99
 **********************************************************************/

char *str_clone(const char *str)
  {
  char *new;
  int len;

  if (!str) die("Null string pointer passed.");

  len = 1 + (int)strlen(str);
  if ( !(new = s_malloc(len * sizeof(char))) ) die("Unable to allocate memory.");

  return(memcpy(new, str, len));
  }


/**********************************************************************
  char *str_subclone(char *str)
  synopsis:	Create a new string containing a substring of the
		given string.
  parameters:   const	char	*str		The string.
  return:	New string.
  last updated: 03/03/99
 **********************************************************************/

char *str_subclone(const char *str, const int first, const int last)
  {
  char *new;
  int len = last - first + 1;

  if (str) die("null string pointer passed");
  if (first < 0 || last < 0 || first > last || last > (int) strlen(str))
    die("That's a dumb parameter");

  new = s_malloc((len + 1) * sizeof (char));

  if (new)
    {
    memcpy(new, str + first, len);
    new[len] = '\0';
    }
  return( new );
  }


/**********************************************************************
  int str_qhash()
  synopsis:	Return an integer hash code for the string.
		Hash codes are in range 0 to INT_MAX.
		This is probably far from optimal for a given
		purpose - It is intended for rapid prototyping only.
  parameters:   char	*str	The string.
  return:       None.
  last updated: 20/02/99
 **********************************************************************/

int str_qhash(const char *str)
  {
  int h = 0;

  if (!str) die("NULL pointer passed\n");
  if (!*str) die("Empty string passed\n");

  while (*str)
    {
    h = (h << 4) + *str++;
    }

  return( h%RAND_MAX );
/*  return( h%MAX_INT );*/
  }


/**********************************************************************
  void str_ctranslate()
  synopsis:	Convert all occurances of a character to another
		character.
  parameters:   char	*str	The string.
		char	find	The character to translate.
		char	replace	The character to replace with.
  return:       None.
  last updated: 20/02/99
 **********************************************************************/

void str_ctranslate(char *str, char find, char replace)
  {
  if (!str) die("Null string pointer passed.\n");

  while (*str)
    {
    if (*str == find) *str = replace;
    str++;
    }

  return;
  }


/**********************************************************************
  int str_tr()
  synopsis:	Convert all occurances of a character to another
		character.
  parameters:   char	*str	The string.
		unsigned long	len	Length of string.
		char	find	The character to translate.
		char	replace	The character to replace with.
  return:       Number of translations performed.
  last updated: 24/09/00
 **********************************************************************/

int str_tr(char *str, int len, char find, char replace)
  {
  int	count=0;

  if (!str) die("Null string pointer passed.\n");

  while (len > 0)
    {
    if (*str == find)
      {
      *str = replace;
      count++;
      }
    len--;
    str++;
    }

  return count;
  }

/*
 * *** THE FOLLOWING FUNCTIONS COME FROM BABEL1.6 ***
 * (Which I assume is in the public domain...)
 */

/*-----------------------------------------------------------------------------
FUNCTION : rjust
PURPOSE  : right justify a string
------------------------------------------------------------------------------*/
char *rjust(char *str)
{
  int n = (int) strlen(str);
  char *dup_str;
  
  dup_str = (char *)s_malloc(sizeof(char) * (1 + (int)strlen(str)));
  strcpy(dup_str,str);
  rtrim(dup_str);
  sprintf(str,"%*.*s",n,n,dup_str);
  s_free(dup_str);
  return(str);
}

/*-------------------------------------------------------------------------------
FUNCTION : rtrim
PURPOSE  : trim trailing spaces from a string
--------------------------------------------------------------------------------*/
char *rtrim(char *str)
{
  int n = (int)strlen(str) - 1;
  
  while (n >= 0)
  {
    if (*(str+n) != ' ')
    {
      *(str+n+1) = '\0';
      break;
    }
    else n--;
  }
  if ((n < 0) && (str[0] == ' ')) str[0] = '\0';
  
  return str;
}

/*---------------------------------------------------------
FUNCTION : ltrim
PURPOSE  : trim leading spaces from a string
-----------------------------------------------------------*/
char *ltrim(char *str)
{
  str_rev(str);
  rtrim(str);
  str_rev(str);
  
  return str;
}


/*---------------------------------------------------------
FUNCTION : fill_space 
PURPOSE  : pad and right justify a string
AUTHOR   : Andreas Bohne (andreas@physik.uni-hildesheim.de)
DATE     : 2-14-96
----------------------------------------------------------*/
char *fill_space(char* dest,char* in,int gesamtl)
{
        char*   tmp     = (char*)(s_calloc(255,sizeof(char)));
        char*   lauf    = tmp;
        int     len     = (int)strlen(in);
        int     ende    = gesamtl-len;
        int     i;
        for(i=0;i<ende;i++)
                *(lauf++)=' ';        /* or use '.' or '-' */
        dest = strcpy(dest,tmp);
        s_free(tmp);
        return dest;
}
  

#if 0
/*---------------------------------------------------------
FUNCTION : strrev
PURPOSE  : reverse a string
AUTHOR   : Bob Stout
FOUND AT : ftp://www.cdrom.com/pub/algorithms/c/strrev.c
----------------------------------------------------------*/
/*char *strrev(char *str)*/

char *str_rev(char *str)
  {
  char *p1, *p2;

  if (!str || !*str) return(str);
  
  for (p1 = str, p2 = str + (int)strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
    }
  
  return(str);
  }
#endif


/*---------------------------------------------------------
FUNCTION : substr count
PURPOSE  : count how many times the string needle occurs 
           in the string haystack
DATE     : 5-21-96
----------------------------------------------------------*/

int substring_count(char *needle, char *haystack)
{
  char *pos = NULL, *newpos = NULL;
  int match_count = 0;
  int len = (int)strlen(needle);
  
  pos = strstr(haystack,needle);
  if (pos)
  {
    match_count++;
    while (pos)
    {
      newpos = strstr(pos+(len*sizeof(char)),needle);
      if (newpos)
      {
	  match_count++;
	}
      pos = newpos;
    }
  }

  return(match_count);
}


/*--------------------------------------------------------------------
FUCTION  :     REPLACE.C
AUTHOR   :     Gilles Kohl
PURPOSE  :    Replace one string by another in a given buffer.
              This code is public domain. Use freely.
--------------------------------------------------------------------*/

/*char *replace_string(char *Str, char *OldStr, char *NewStr)*/
char *str_replace(char *Str, char *OldStr, char *NewStr)
  {
  int OldLen, NewLen;
  char *p, *q;

  if(NULL == (p = strstr(Str, OldStr)))
    return p;

  OldLen = (int)strlen(OldStr);
  NewLen = (int)strlen(NewStr);
  memmove(q = p+NewLen, p+OldLen, (int)strlen(p+OldLen)+1);
  memcpy(p, NewStr, NewLen);
  return q;
  }


/**********************************************************************
  int str_replaceall(char *str, char *find, char *replace)
  synopsis:	Find and replace all occurances of a sub-string.
  parameters:   char	*str		The string buffer.
                char	*find		String to locate.
                char	*replace	Replacement string.
  return:	Number of replacements made.
  last updated: 02/03/99
 **********************************************************************/

int str_replaceall(char *str, char *find, char *replace)
  {
  int	count=0;		/* Number of replacements */
  char	*start=NULL;		/* Start position of search */

  if (!str || !find || !replace)
  
  start = str;
  while(NULL != (start = str_replace(start,find,replace))) count++;

  return(count);
  }


/**********************************************************************
  char *str_stripspace(char *str)
  synopsis:	Copy string, but ignoring any leading white-space.
  parameters:   const	char	*str		The string.
  return:	New string.
  last updated: 03/03/99
 **********************************************************************/

char *str_stripspace(const char *str)
  {
  char	*nstr;	/* Returned string */
  int	len;	/* Length of new string */

  if (!str) die("Null string pointer passed.");
  if (str[0] == '\0') die("Empty string pointer passed.");

  while (*str && isspace((int)*str)) str++;

  len = 1+(int)strlen(str);

  if ( !(nstr=s_malloc(len*sizeof(char))) ) die("Unable to allocate memory.");

  return(memcpy(nstr, str, len));
  }


/**********************************************************************
  int str_cmp_nocase(const char *s1, const char *s2)
  synopsis:	Compare strings but ignore case.  Similar to strcmp().
		Note, does same job as BSD's strcasecmp(), and Amiga's
		stricmp(),  for which there doesn't seem to be an
		ANSI equivalent.
  parameters:   const   char    *s1, *s2            The strings.
  return:	int	return <0,0,>0 depending whether s1 is lexicographically less than, equal to or greater than s2.
  last updated: 03/03/99
 **********************************************************************/

int str_cmp_nocase(const char *s1, const char *s2)
  {
  char	c1, c2;		/* Temp chars */

  if (!s1 || !s2) die("Null string pointer passed.");
  
  if (s1 == s2) return(0);

  do
    {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    }
  while (c1 == c2 && c1 != '\0');

  return((int) c1-c2);
  }


/**********************************************************************
  void str_ncpyt(char *dest, const char *src, const int len)
  synopsis:	Copies string as strncpy(), but also ensures the string
		is null-terminated.
  parameters:   char    	*dest	The destination string.
		const char	*src	The source string.
		const int	len	Number of bytes to copy.
  return:	none
  last updated: 13/04/99
 **********************************************************************/

void str_ncpyt(char *dest, const char *src, const int len)
  {

  if (!dest || !src) die("null string pointer passed.\n");
  if (len < 1) die("Stupid length.\n");
  
  if (dest == src) die("Destination and source are same.\n");

  strncpy(dest, src, (size_t)len);
  dest[len-1]='\0';

  return;
  }


/**********************************************************************
  void str_sncpy(char *dest, const char *src, const int len)
  synopsis:	Copies string as strncpy(), but also ensures the string
		is null-terminated and terminates at the first
		white-space character.
  parameters:   char    	*dest	The destination string.
		const char	*src	The source string.
		const int	len	Number of bytes to copy.
  return:	Pointer to final char in src string looked at.
  last updated: 29 Oct 2002
 **********************************************************************/

char *str_sncpy(char *dest, char *src, const int len)
  {
  char	*end=&(dest[len-1]);

  if (!dest || !src) die("null string pointer passed.\n");
  if (len < 1) die("Stupid length.\n");
  
  if (dest == src) die("Destination and source are same.\n");

  while (*src != ' ' && *src != '\t' && *src != '\n' && *src != '\0' && dest < end)
    {
    *dest = *src;
    src++;
    dest++;
    }

  *dest='\0';

  return src;
  }

#if 0
/*
 * These functions come from Eric Harlow's GTK book.
 */

/*
 * FindDigit
 *
 * Look for a digit in the string buffer
 *
 * szBuffer - string we are looking in for a numeric
 *            value
 */
char *FindDigit (char *szBuffer)
{
    /* If it's NULL, there's nothing here */
    if (szBuffer == NULL) {
        return (NULL);
    }

    /* While we have string and no number... */
    while (*szBuffer && !isdigit (*szBuffer)) {

        szBuffer++;
    }

    /* Return the buffer */
    return (szBuffer);
}

/*
 * ExtractDigit
 *
 * Extract the numeric data from the string and return the
 * remainder of the string
 *
 * sTmp - string to extract numeric value.
 * pnValue - value to set
 *
 * returns remainder of the string
 */
char *ExtractDigit (char *sTmp, int *pnValue)
{
    int nIndex = 0;
    char buffer[88];

    /* If it's NULL, there's no value. */
    if (sTmp == NULL) {
        return (NULL);
    }

    /* While we are finding digits */
    while (*sTmp && isdigit (*sTmp)) {

        /* Add to the string */
        buffer[nIndex] = *sTmp;
        nIndex++;
        sTmp++;
    }

    /* Mark the end of the string */
    buffer[nIndex] = (char) 0;

    /* Convert string to number */
    *pnValue = atoi (buffer);

    /* Return the remainder of the string (if any) */
    if (*sTmp) return (sTmp);

    /* Nope.. all done */
    return (NULL);

}
#endif


/**********************************************************************
  char *str_nclone(const char *str, const int len)
  synopsis:	Copy contents of string of given length into a newly
		allocated string.
  parameters:   const char	*str		The string.
		const int	len		Length.
  return:	New string.
  last updated: 13/08/99
 **********************************************************************/

char *str_nclone(const char *str, const int len)
  {
  char *new;

  if (!str) die("null string pointer passed");

  if ( !(new = s_malloc(len * sizeof(char))) ) die("unable to allocate memory");

  return(memcpy(new, str, len));
  }


/**********************************************************************
  char *str_getline(FILE *fp, int *len)
  synopsis:	Reads upto newline/eof from specified stream, also
		ensures the string is null-terminated.  Tries to avoid
		unnecessary memory allocation.
  parameters:	FILE	*fp	The input stream.
		int	*len	Length of string.
  return:	char	*dest	The destination string.
  last updated: 17/08/99
 **********************************************************************/

char *str_getline(FILE *fp, int *len)
  {
  static char	*temp=NULL;	/* Intermediate string - grows as required */
  char		*dest;		/* Destination string */
  static int	temp_len=0;	/* Size of temp */
  int		c;		/* Current character */

  if (!len) die("Null integer pointer passed.\n");
  if (!fp) die("Null file handle passed.\n");

  *len = 0;

  while((c=fgetc(fp))!=EOF && ((char)c!='\n'))
    {
    if (*len==temp_len)
      {	/* Need more memory */
      temp_len+=256;	/* Magic number */
      temp=s_realloc(temp, temp_len*sizeof(char));
      }
    temp[(*len)++]=(char)c;
    }
  temp[(*len)++]='\0';

  dest = str_nclone(temp, *len);

  return(dest);
  }


/**********************************************************************
  int str_nreadline(FILE *fp, int len, char *dest)
  synopsis:	Reads upto newline/eof from specified stream, to a
		maximum of len characters, also
		ensures that the string is always null-terminated.
  parameters:   char    	*dest	The destination string.
		FILE		*fp	The input stream.
  return:	int	actual number of characters read. -1 on failure.
  last updated: 08 Jan 2003
 **********************************************************************/

int str_nreadline(FILE *fp, const int len, char *dest)
  {
  int		count=0, max_count;	/* Number of chars read */
  int		c;			/* Current character */

  if (!fp) die("Null file handle passed.\n");
  if (len < 1) die("Stupid length.\n");
  if (!dest) die("Null string pointer passed.\n");

  max_count = len-1;

/*  while((!feof(fp)) && (c=fgetc(fp)) && (c!='\n') && count<len)*/

  while(count<len && (c=fgetc(fp))!=EOF && ((char)c!='\n'))
    dest[count++]=(char)c;

  dest[count]='\0';

  return count-1;
  }


/**********************************************************************
  void str_left_adjust()
  synopsis:	Remove all leading white-space from the string.
  parameters:
  return:	none
  last updated: 13/08/99
 **********************************************************************/

void str_left_adjust(char *str)
  {
  char *nb;

  if (!str) die("null string pointer passed");

  nb = str;
  while (*nb!='\0' && isspace((int)*nb)) nb++;
  memmove(str, nb, strlen(str) - (str-nb) + 1);
  }


/**********************************************************************
  void str_right_adjust()
  synopsis:	Remove all trailing white-space from the string.
  parameters:
  return:	none
  last updated: 13/08/99
 **********************************************************************/

void str_right_adjust(char *str)
  {
  int len;

  if (!str) die("null string pointer passed");

  len = (int) strlen(str);
  while (len--)
    {
    if (!isspace((int)str[len]))
      {
      str[len+1] = '\0';
      return;
      }
    }
  }


/**********************************************************************
  str_countchar()
  synopsis:	Count all occurances of a given character in a string.
  parameters:	char	*str	The string (null-terminated)
		char	c	The character
  return:	int	count	Number of occurances.
  last updated: 11/04/00
 **********************************************************************/

int str_countchar(char *str, char c)
  {
  int count = 0;

  while( *str != '\0' )
    {
    if (*str == c) count++;
    str++;
    }

  return (count);
  }


/**********************************************************************
  str_ncatf()
  synopsis:	Basically does snprintf() to the end of an existing
		string.  n is the total length available for str,
		which includes the current null-terminated contents.
		the string is uneffected if there is no free room
		available.
		returns the final length that str would have been
		without truncation.  i.e. if >= n, truncation occured.
  parameters:	char	*str	The string (null-terminated)
		char	c	The character
  return:	int	count	Number of occurances.
  last updated: 19/05/00
 **********************************************************************/

int str_ncatf(char *str, const size_t n, const char *format, ...)
  {
  size_t	len;		/* Length of original string */
  int		app_len;	/* Length of appended string */
  va_list	ap;		/* Variable argument list */

  if (!str) die("Null pointer to string passed.");
  if (!format) die("Null pointer to format string passed.");
  if (n<=0) die("Stupid length passed.");

  len = strlen(str);
  if (n<=len)
    die("Dodgy string passed (not null-terminated) or length passed too short.");

  va_start(ap, format);
  app_len = vsnprintf(str+len, n-len, format, ap);
  va_end(ap);

  return app_len + len;
  }


/**********************************************************************
  str_cat_va()
  synopsis:	Concatenate one or more strings.
  parameters:	char	*str	The string (null-terminated).
		va_list ap	Variable argument list.
  return:	char	*dest	Concatenated string, or NULL on failure.
  last updated: 19/05/00
 **********************************************************************/

char *str_cat_va(char *str, va_list ap)
  {
  va_list	orig_ap;	/* Store the passed va list */
  size_t	len;		/* Required buffer length */
  char		*dest;		/* The destination string */
  char		*dp, *sp;	/* Pointer into dest string, source string */

/* At least one string? */
  if (!str) return NULL;

/*
 * Determine required size for destination buffer.
 */
  orig_ap = ap;
  len = strlen(str);

  while ( (sp = va_arg(ap, char *))!=NULL ) len += strlen(sp);

  ap = orig_ap;

/* Allocate destination buffer (+1 for terminating NULL char). */
  if ( !(dest = s_malloc(len+1)) ) return NULL;

/* Concatenate the strings. */
  dp = dest;
  while ( (*dp++ = *str++)!='\0' );

  while ( (sp = va_arg(ap, char *))!=NULL )
    {
    while ( (*dp++ = *sp++)!='\0' );
    }

/* Tack on a terminating null char. */
  *dp = '\0';

  return dest;
  }


/**********************************************************************
  str_cat()
  synopsis:	Concatenate one or more strings.  Uses the str_cat_va()
		function.
  parameters:	char	*str	The string (null-terminated)
  return:	char	*dest	Concatenated string, or NULL on failure.
  last updated: 19/05/00
 **********************************************************************/

char *str_cat(char *str, ...)
  {
  char		*dest;
  va_list	ap;

  if (!str) return NULL;

  va_start(ap, str);
  dest = str_cat_va(str, ap);
  va_end(ap);

  return dest;
  }


#if 0
/**********************************************************************
  str_nprintf()
  synopsis:	Portable version of the GNU snprintf() function.
  parameters:	char	*str	The string (null-terminated)
		char	c	The character
  return:	int	count	Number of occurances.
  last updated: 19/05/00
 **********************************************************************/

int str_nprintf(char *str, size_t n, const char *format, ...)
  {
  int		destlen;
  va_list	ap;

  assert(format);

  va_start(ap, format);
  destlen = vsnprintf(str, n, format, ap);
  va_end(ap);

  return destlen;
  }

#endif


/**********************************************************************
  str_safecopy()
  synopsis:	Copy strings where the source and destination buffers
		may overlap.  Also ensures that the destination string
		is always NULL-terminated.
  parameters:	char	*src	The original string.
		char	*dest	The destination string.
		const int len	Number of characters to copy.
  return:	int	ret	Number of characters copied.
  last updated: 19/05/00
 **********************************************************************/

int str_safecopy(char *src, char *dest, const int len)
  {
  char		*s;	/* Pointer into src strings */
  char		*d;	/* Pointer into dest strings */
  int		n;

  if (!src || !dest) die("Pointer to NULL char passed.");
  if (len < 1) die("Stupid length passed.");

  s = src;
  d = dest;
  n = len;

  if (s < d)
    {	/* End of source may overlap start of dest. */
    s += n-1;
    d += n;
    *d-- = '\0';
    while (n-- > 0) *d-- = *s--;
    return len;
    }
  else if (s > d)
    {	/* Start of source may overlap end of dest. */
    while (n-- > 0) *d++ = *s++;
    *s = '\0';
    return len;
    }
/* else source == dest anyway */

  return 0;
  }


/**********************************************************************
  str_scmp()
  synopsis:	Compare strings upto first white-space.
  parameters:	char	*src	The original string.
  return:	int	ret	Number of characters copied.
  last updated: 29 Oct 2002
 **********************************************************************/

int str_scmp(const char *s1, const char *s2)
  {

  while (*s1 != ' ' && *s1 != '\0' && *s1 == *s2)
    {
    s1++;
    s2++;
    }

  if ( (*s1 == ' ' || *s1 == '\t' || *s1 == '\n' || *s1 == '\0') && 
       (*s2 == ' ' || *s2 == '\t' || *s2 == '\n' || *s2 == '\0') ) return 0;

  return (int) (*s1 - *s2);
  }


/**********************************************************************
  str_split()
  synopsis:	Tokenise strings.
  parameters:
  return:
  last updated: 10 Jan 2002
 **********************************************************************/

char **str_split(const char *string,
                 const char *delimiter,
	         int         max_tokens)
  {
  SLList *string_list = NULL, *slist;
  char **str_array, *s;
  int i, n = 1;

  if (!string) return NULL;
  if (!delimiter) return NULL;

  if (max_tokens < 1)
    max_tokens = STR_MAX_TOKENS;

  s = strstr(string, delimiter);
  if (s)
    {
    int delimiter_len = (int) strlen(delimiter);

    do
      {
      int len;
      char *new_string;

      len = s - string;
      new_string = s_malloc(sizeof(char)*(len+1));
      strncpy(new_string, string, (size_t) len);
      new_string[len] = '\0';
      string_list = slink_prepend(string_list, new_string);
      n++;
      string = s + delimiter_len;
      s = strstr(string, delimiter);
      } while(--max_tokens && s);
    }
  if (*string != '\0')
    {
    n++;
    string_list = slink_prepend(string_list, (void *)s_strdup(string));
    }

  str_array = s_malloc(sizeof(char*)*n);

  i = n - 1;

  str_array[i--] = NULL;
  slist = string_list;
  while(slist!=NULL)
    {
    str_array[i--] = slink_data(slist);
    slist = slink_next(slist);
    }

  slink_free(string_list);

  return str_array;
  }


/**********************************************************************
  str_freev()
  synopsis:	Free tokenised strings.
  parameters:
  return:
  last updated: 10 Jan 2002
 **********************************************************************/

void str_freev(char **str_array)
  {
  int i;

  if (!str_array) return;

  for(i = 0; str_array[i] != NULL; i++) s_free(str_array[i]);

  s_free(str_array);

  return;
  }


/**********************************************************************
  str_joinv()
  synopsis:	Rejoin tokenised strings.
  parameters:
  return:
  last updated: 10 Jan 2002
 **********************************************************************/

char *str_joinv(const char *separator, char **str_array)
  {
  char *string;

  if (!str_array) return NULL;

  if (separator == NULL)
    separator = "";

  if (*str_array)
    {
      int i, len;
      int separator_len;

      separator_len = (int) strlen(separator);
      len = 1 + (int) strlen(str_array[0]);
      for(i = 1; str_array[i] != NULL; i++)
	len += separator_len + (int) strlen(str_array[i]);

      string = s_malloc(sizeof(char)*len);
      *string = '\0';
      strcat(string, *str_array);
      for(i = 1; str_array[i] != NULL; i++)
	{
	  strcat(string, separator);
	  strcat(string, str_array[i]);
	}
      }
  else
    string = s_strdup("");

  return string;
  }


/**********************************************************************
  str_joinv()
  synopsis:	Rejoin tokenised strings.
  parameters:
  return:
  last updated: 10 Jan 2002
 **********************************************************************/

char *str_join(const char *separator, ...)
  {
  char *string, *s;
  va_list args;
  int len;
  int separator_len;

  if (separator == NULL)
    separator = "";

  separator_len = (int) strlen(separator);

  va_start(args, separator);

  s = va_arg(args, char*);

  if (s)
    {
      len = (int) strlen(s);
      
      s = va_arg(args, char*);
      while(s)
	{
	  len += separator_len + (int) strlen(s);
	  s = va_arg(args, char*);
	}
      va_end(args);
      
      string = s_malloc(sizeof(char)*(len+1));
      *string = '\0';
      
      va_start(args, separator);
      
      s = va_arg(args, char*);
      strcat(string, s);
      
      s = va_arg(args, char*);
      while(s)
	{
	  strcat(string, separator);
	  strcat(string, s);
	  s = va_arg(args, char*);
	}
    }
  else
    string = s_strdup("");
  
  va_end(args);

  return string;
  }


/**********************************************************************
  str_rev()
  synopsis:	Reverse the contents of a string.
  parameters:	char *str	String to reverse.
  return:	The same pointer as parameter *str.
  last updated: 08 Jan 2003
 **********************************************************************/

char *str_rev(char *str)
  {
  char *p1, *p2, tmp;

  if (str==NULL || *str=='\0') return str;
  
  p1 = str;
  p2 = str + (int)strlen(str) - 1;
 
  while (p1 < p2)
    {
    tmp = *p2;
    *p2 = *p1;
    *p1 = tmp;
    p1++;
    p2--;
    }
  
  return str;
  }


