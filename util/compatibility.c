/**********************************************************************
  compatibility.c
 **********************************************************************

  compatibility - Compatibility/Portability stuff.
  Copyright ©2000-2003, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Compatibility/portability functions.

		Note that some of these functions are POSIX
		compliant. Some are ANSI compliant.  Some are just
		plain non-standard.  They do really need to be
		tidied and rationalised.

		Some of these functions come from or are based on code
		from the following GPL'd projects:

		freeciv 1.11.0 - http://www.freeciv.org/
		glib	1.2.8  - http://www.gtk.org/

 ----------------------------------------------------------------------
  freeciv 1.11.0 copyright notice:
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 ----------------------------------------------------------------------
  glib 1.2.8 copyright notice:
 * GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.

 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.

 * Modified by the GLib Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 ----------------------------------------------------------------------

  Updated:	22 Jul 2003 Removed some unused code and applied my prefered indentation style.
		07 Jul 2003 SAA Added dpow().
		10 Jun 2003 SAA	Replaced "#ifndef HAVE_WHATEVER" with "#if HAVE_WHATEVER != 1" which is, apparently, recommended by the autoconf guys.
		25 Feb 2003 SAA	Tweaked strndup() prototype.
		24 Dec 2002 SAA	strlen() should have size_t return type, according to POSIX.
		20 Dec 2002 SAA Modified prototype of strncmp(), strncpy(), strtok() to match iso 9899 specification.  Also added new comments to strcmp() and strncmp().  Fixed strtod() for case that setlocale() is unavailable.  memset() code now matches it's prototype.
		25 Oct 2002 SAA	Added gethostname() stub.
		02 Oct 2002 SAA A #ifndef HAVE_STRCASECMP should have been #ifndef HAVE_STRSEP
		12 Jun 2002 SAA	#ifdef HAVE_STRREV should have been #ifndef HAVE_STRREV.
		09 Apr 2002 SAA	Added memscan(), strpbrk() and strsep().
		14 Mar 2002 SAA	Changes to readline() for clean compilation under AIX.
		13 Mar 2002 SAA	Added itoa().  Use index() for strchr(), when available.
		10 Jan 2002 SAA Removed strsplit(), strjoin(), strjoinv(), strfreev() which I think were amiga functions because they aren't really needed in any of my recent code.  Added strspn() replacement.
  		09 Jan 2002 SAA Reversed brain-dead change from 05 Dec 2001.  Uncommented strtod() stuff.  Some code tidying.
		05 Dec 2001 SAA Only explicitely requested things will be compiled now, i.e. needs HAVE_THING == 0.
		17/12/00 SAA	Added ipow().
		16/11/00 SAA	Use rindex() for strrchr() where available.  Added a few routines from the autobook examples, I don't actually use any of these at the moment, but I thought that they might be useful now that I'm spending some time converting my code to alternative platforms.  Added dief() version because I can't think of anywhere better to put it (I use a macro version of it all the time but it's varargs cause problems for me on some systems, i.e. all non-GNU compilers)
		15/11/00 SAA	Code brought together from various places.

 **********************************************************************/

#include "gaul/compatibility.h"

#if HAVE_IPOW != 1
/*
 * Integer power.
 */
int ipow(int n, int e)
  {
  int	result=1;	/* The answer. */

  while (e>0)
    {
    result*=n;
    e--;
    }

  return result;
  }
#endif	/* HAVE_IPOW */


#if HAVE_DPOW != 1
/*
 * Double to integer power.
 */
double dpow(double n, int e)
  {
  double	result=1.0;	/* The answer. */

  while (e>0)
    {
    result*=n;
    e--;
    }

  return result;
  }
#endif	/* HAVE_DPOW */


#if HAVE_MEMCPY != 1
#if HAVE_BCOPY != 1
/*
 * Copy LEN characters from SRC to DEST
 * 
 * Some systems, such as SunOS do have BCOPY instead.
 * In which case this is defined as a macro in the header.
 */
void memcpy(char *dest, const char *src, size_t len)
{
  char		*dest_p;
  const	char	*src_p;
  int		byte_c;
  
  if (len <= 0)
    return;
  
  src_p = src;
  dest_p = dest;
  
  if (src_p <= dest_p && src_p + (len - 1) >= dest_p)
    {
    /* overlap, must copy right-to-left. */
    src_p += len - 1;
    dest_p += len - 1;
    for (byte_c = 0; byte_c < len; byte_c++)
      *dest_p-- = *src_p--;

    }
  else
    {
    for (byte_c = 0; byte_c < len; byte_c++)
      *dest_p++ = *src_p++;
    }

  return;
  }
#endif /* HAVE_BCOPY */
#endif /* HAVE_MEMCPY */


#if HAVE_STRCHR != 1
#if HAVE_INDEX != 1
/*
 * Find C in STR by searching through the string
 */
char *strchr(const char *str, int c)
  {

  while ( *str != '\0' )
    {
    if (*str == (char)c)
      return (char *)str;
    str++;
    }
  
  if (c == '\0')
    return (char *)str;

  return NULL;
  }
#endif /* HAVE_INDEX */
#endif /* HAVE_STRCHR */


#if HAVE_STRRCHR != 1
#if HAVE_RINDEX != 1
/*
 * Find C in STR by searching backwards through the string
 */
char *strrchr(const char *str, int c)
  {
  const char	*pntr = NULL;
  
  while ( *str != '\0' )
    {
    if (*str == (char)c)
      pntr = (char *)str;
    str++;
    }
  
  if (c == '\0') return (char *)str;

  return (char *)pntr;
  }
#endif /* HAVE_RINDEX */
#endif /* HAVE_STRRCHR */


#if HAVE_STRCAT != 1
/*
 * Concatenate STR2 onto the end of STR1
 */
char *strcat(char *str1, const char *str2)
  {
  char	*orig = str1;
  
  while ( *str != '\0' ) str1++;
  
  while (*str2 != '\0') *str1++ = *str2++;

  *str1 = '\0';
  
  return orig;
  }
#endif /* HAVE_STRCAT */


#if HAVE_STRLEN  != 1
/*
 * Return the length in characters of STR
 */
size_t strlen(const char *str)
  {
  int	len=0;
  
  while ( *str != '\0' )
    {
    str++;
    len++;
    }
  
  return len;
  }
#endif /* HAVE_STRLEN */


#if HAVE_STRCMP != 1
/*
 * Compare str1 and str2.  It returns
 * -1, 0 or 1 if str1 is found, to be less than, to be equal to,
 * or be greater than str2, respectively.
 */
int strcmp(const char *str1, const char *str2)
  {

  while (*str1 != '\0' && *str1 == *str2)
    {
    str1++;
    str2++;
    }

  return *str1 - *str2;
  }
#endif /* HAVE_STRCMP */


#if HAVE_STRNCMP != 1
/*
 * Compare at most len characters of str1 and str2.  It returns
 * -1, 0 or 1 if str1 is found, to be less than, to be equal to,
 * or be greater than str2, respectively.
 */
int strncmp(const char *str1, const char *str2, size_t len)
  {
  int	c=0;
  
  while ( c < len )
    {
    if (*str1 != *str2 || *str1 == '\0')
      return *str1 - *str2;
    c++;
    str1++;
    str2++;
    }
  
  return 0;
  }
#endif /* HAVE_STRNCMP */


#if HAVE_STRCPY != 1
/*
 * Copies STR2 to STR1.  Returns STR1.
 */
char *strcpy(char *str1, const char *str2)
  {
  char	*str_p;
  
  for (str_p = str1; *str2 != '\0'; str_p++, str2++) *str_p = *str2;

  *str_p = '\0';
  
  return str1;
  }
#endif /* HAVE_STRCPY */


#if HAVE_STRNCPY != 1
/*
 * Copy STR2 to STR1 until LEN or null character in source.
 */
char	*strncpy(char *str1, const char *str2, size_t len)
  {
  char		*str1_p, null_reached = FALSE;
  int		len_c;
  
  for (len_c = 0, str1_p = str1; len_c < len; len_c++, str1_p++, str2++)
    {
    if (null_reached || *str2 == '\0')
      {
      null_reached = TRUE;
      *str1_p = '\0';
      }
    else
      {
      *str1_p = *str2;
      }
    }
  
  return str1;
  }
#endif /* HAVE_STRNCPY */


#if HAVE_STRTOK != 1
/*
 * Get the next token from STR (pass in NULL on the 2nd, 3rd,
 * etc. calls), tokens are a list of characters deliminated by a
 * character from DELIM.  writes null into STR to end token.
 * This is not thread-safe.
 */
char	*strtok(char *str, const char *delim)
  {
  static char	*last_str = "";
  char		*start, *delim_p;
  
  /* no new strings to search? */
  if (str != NULL)
    {
    last_str = str;
    }
  else
    {
    /* have we reached end of old one? */
    if (*last_str == '\0')
      return NULL;
    }
  
  /* parse through starting token deliminators */
  while ( *last_str != '\0' )
    {
    for (delim_p = delim; *delim_p != '\0'; delim_p++)
      {
      if (*last_str == *delim_p)
	break;
      }
    last_str++;
    }
    
    /* is the character NOT in the delim list? */
    if (*delim_p == '\0')
      break;
    }
  
  /* did we reach the end? */
  if (*last_str == '\0')
    return NULL;
  
  /* now start parsing through the string, could be '\0' already */
  for (start = last_str; *last_str != '\0'; last_str++)
    {
    for (delim_p = delim; *delim_p != '\0'; delim_p++)
      {
      if (*last_str == *delim_p)
        {
	/* punch NULL and point last_str past it */
	*last_str++ = '\0';
	return start;
        }
      }
    }
  
  /* reached the end of the string */
  return start;
  }
#endif /* HAVE_STRTOK */


#if HAVE_STRPBRK != 1
/*
 * Locate the first occurrence in the string s of any of the characters in the string accept.
 */
char *strpbrk(const char *s, const char *accept)
  {
  const char *s1;
  const char *s2;
 
  for (s1 = s; *s1 != '\0'; ++s1)
    {
    for (s2 = accept; *s2 != '\0'; ++s2)
      {
      if (*s1 == *s2) return (char *) s1;
      }
    }
     
  return NULL;
  }
#endif /* HAVE_STRPBRK */


#if HAVE_STRSEP != 1
/*
 * If *str is NULL, return NULL.  Otherwise, this find the first token
 * in the string *str, where tokens are delimited by symbols in the
 * string delim.  This token is terminated with a `\0' character (by
 * overwriting the delimiter) and *str is updated to point past the
 * token.  If no delimiter is found, the token is taken to be the
 * entire string *str, and *str is made NULL.
 *
 * Returns a pointer to the token (i.e returns the original value of *str)
 *
 * The strsep() function was introduced as a replacement for strtok(),
 * which cannot handle empty fields.
 */
char *strsep(char **str, const char *delim)
  {
  char *s = *str, *end;

  if (!s) return NULL;

  end = strpbrk(s, delim);
  if (end) *end++ = '\0';
  *str = end;

  return s;
  }
#endif /* HAVE_STRSEP */


#if HAVE_STRCASECMP != 1
int strcasecmp(const char *str0, const char *str1)
  {

  while( tolower(*str0)==tolower(*str1) )
    {
    if(*str0=='\0')
      return 0;
    str0++;
    str1++;
    }

  return tolower(*str0)-tolower(*str1);
  }
#endif /* HAVE_STRCASECMP */


#if HAVE_STRNCASECMP != 1
/***************************************************************
  Compare strings like strncmp(), but ignoring case.
  ie, only compares first n chars.
***************************************************************/

int strncasecmp(const char *str0, const char *str1, size_t n)
  {
  size_t i;
  
  for(i=0; i<n && tolower(*str0)==tolower(*str1); i++, str0++, str1++)
    if(*str0=='\0')
      return 0;

  return (i==n) ? 0 : (tolower(*str0)-tolower(*str1));
  }
#endif /* HAVE_STRNCASECMP */


/*
 * Sleep for a specified number of microseconds.
 * -- Should replace with the POSIX standard's nanosleep().
 */
#if HAVE_USLEEP != 1
void usleep(unsigned long usec)
  {
#ifdef W32_CRIPPLED
  /* FIXME: Need to add windows code for sleeping here. */
#else
  #ifdef HAVE_SNOOZE		/* i.e. BeOS, AtheOS, Syllable. */
  snooze(usec/1000);		/* BeOS sleep is in milliseconds. */
  #else
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=usec;
  select(0, NULL, NULL, NULL, &tv);
  #endif
#endif

  return;
  }
#endif /* HAVE_USLEEP */


#if HAVE_STRLCPY != 1
/**********************************************************************
 strlcpy() and strlcat() provide (non-standard) functions
 strlcpy() and strlcat(), with semantics following OpenBSD (and
 maybe others).  They are intended as more user-friendly
 versions of stncpy and strncat, in particular easier to
 use safely and correctly, and ensuring nul-terminated results
 while being able to detect truncation.

 n is the full size of the destination buffer, including
 space for trailing nul, and including the pre-existing
 string for mystrlcat().  Thus can eg use sizeof(buffer),
 or exact size malloc-ed.

 Result is always nul-terminated, whether or not truncation occurs,
 and the return value is the strlen the destination would have had
 without truncation.  Ie, a return value >= input n indicates
 truncation occured.

 Will assume that if configure found strlcpy/strlcat they are ok.
 For replacement implementations, will keep it simple rather
 than try for super-efficiency.

 Not sure about the asserts below, but they are easier than
 trying to ensure correct behaviour on strange inputs.
 In particular note that n == 0 is prohibited (eg, since there
 must at least be room for a nul); could consider other options.
***********************************************************************/
size_t strlcpy(char *dest, const char *src, size_t n)
  {
  size_t	len = strlen(src);
  size_t	num_to_copy = (len >= n) ? n-1 : len;

  assert(dest);
  assert(src);
  assert(n>0);

  if (num_to_copy>0)
    memcpy(dest, src, num_to_copy);
  dest[num_to_copy] = '\0';

  return len;
  }
#endif /* HAVE_STRLCPY */


#if HAVE_STRLCAT != 1
size_t strlcat(char *dest, const char *src, size_t n)
  {
  size_t	num_to_copy;
  size_t	len_src;
  size_t	len_dest = strlen(dest);

  assert(dest);
  assert(src);
  assert(n>0);
  assert(len_dest<n);
  /* Otherwise have bad choice of leaving dest not null-terminated
   * within the specified length n (which should be assumable as
   * a post-condition of mystrlcat), or modifying dest before end
   * of existing string (which breaks strcat semantics).
   */
       
  dest += len_dest;
  n -= len_dest;
    
  len_src = strlen(src);
  num_to_copy = (len_src >= n) ? n-1 : len_src;
  if (num_to_copy>0)
    memcpy(dest, src, num_to_copy);
  dest[num_to_copy] = '\0';

  return len_dest + len_src;
  }
#endif /* HAVE_STRLCAT */


#if HAVE_VSNPRINTF != 1
/**********************************************************************
 Convenience function used by check_native_vsnprintf() below.
 (Can test check_native_vsnprintf() by replacing vsnprintf call
 below with vsprintf(), or by changing return value to eg -1.)
***********************************************************************/
static int test_snprintf(char *str, size_t n, const char *format, ...)
  {
  int ret;
  va_list ap;

  va_start(ap, format);
  ret = vsnprintf(str, n, format, ap);
  va_end(ap);
  return ret;
  }

/**********************************************************************
 This function checks, at runtime, whether a native vsnprintf() meets
 our requirements; specifically:
 - Should actually obey the parameter n, rather than, eg, just
   calling sprintf and ignoring n.
 - On truncation, should return the number of chars (not counting
   trailing null) which would have been printed, rather than, eg, -1.
 Returns 1 if both ok.
 Also checks whether null-terminates on truncation, but we don't
 base return value on this since it is easy to force this behaviour.
 Reports to stderr if DEBUG set (cannot use freelog since that
 calls my_vsnprintf).
 Could do these checks at configure time, but seems to me easier to
 do at runtime on first call...
***********************************************************************/
static int check_native_vsnprintf(void)
  {
  char buf[20]   = "abcdefghijkl";   /* 12 + null */
  char *test_str = "0123456789";
  const int ntrunc = 5;
  const char one_past = buf[ntrunc];
  int test_len = strlen(test_str);
  int ret;

  ret = test_snprintf(buf, ntrunc, "%s", test_str);
#if DEBUG
  if (buf[ntrunc] != one_past) {
    fprintf(stderr, "debug: test_snprintf wrote past n\n");
  } else if (buf[ntrunc-1] != '\0') {
    fprintf(stderr, "debug: test_snprintf did not null-terminate\n");
  }
  if (ret != test_len) {
    fprintf(stderr, "debug: test_snprintf returned %d,"
	    " not untruncated length %d\n", ret, test_len);
  }
#endif

  return (buf[ntrunc]==one_past && ret==test_len);
  }


/**********************************************************************
 vsnprintf() replacement using a big malloc()ed internal buffer,
 originally by David Pfitzner <dwp@mso.anu.edu.au>

 Parameter n specifies the maximum number of characters to produce.
 This includes the trailing null, so n should be the actual number
 of characters allocated (or sizeof for char array).  If truncation
 occurs, the result will still be null-terminated.  (I'm not sure
 whether all native vsnprintf() functions null-terminate on
 truncation; this does so even if calls native function.)

 Return value: if there is no truncation, returns the number of
 characters printed, not including the trailing null.  If truncation
 does occur, returns the number of characters which would have been
 produced without truncation.
 (Linux man page says returns -1 on truncation, but glibc seems to
 do as above nevertheless; check_native_vsnprintf() above tests this.)

 The method is simply to malloc (first time called) a big internal
 buffer, longer than any result is likely to be (for non-malicious
 usage), then vsprintf to that buffer, and copy the appropriate
 number of characters to the destination.  Thus, this is not 100%
 safe.  But somewhat safe, and at least safer than using raw snprintf!
 :-) (And of course if you have the native version it is safe.)

 Before rushing to provide a 100% safe replacement version, consider
 the following advantages of this method:
 
 - It is very simple, so not likely to have many bugs (other than
 arguably the core design bug regarding absolute safety), nor need
 maintenance.

 - It uses native vsprintf() (which is required), thus exactly
 duplicates the native format-string parsing/conversions.

 - It is *very* portable.  Eg, it does not require mprotect(), nor
 does it do any of its own parsing of the format string, nor use
 any tricks to go through the va_list twice.

***********************************************************************/

/* buffer size: "64k should be big enough for anyone" ;-) */
#define VSNP_BUF_SIZE (64*1024)

int my_vsnprintf(char *str, size_t n, const char *format, va_list ap)
{
  /* This may be overzealous, but I suspect any triggering of these
   * to be bugs.  (Do this even if call native function.)
   */
  assert(str);
  assert(n>0);
  assert(format);

#if HAVE_VSNPRINTF == 1
  {
    static int native_is_ok = -1; /* set to 0 or 1 on first call */

    if (native_is_ok == -1) {
      native_is_ok = check_native_vsnprintf();
    }

    if (native_is_ok) {
      int ret = vsnprintf(str, n, format, ap);
      /* Guarantee null-terminated: (native_is_ok means can use ret like this) */
      if (ret >= n) {
	str[n-1] = '\0';
      }
      return ret;
    }
  }
#endif
  /* Following is used if don't have native, or if fall through
   * from above if native doesn't pass checks.
   */
  {
    static char *buf = NULL;
    int len;

    if (buf==NULL) {
      buf = s_malloc(VSNP_BUF_SIZE);
    }

#if HAVE_VSNPRINTF == 1
    /* This occurs if have native, but didn't pass check:
     * may just be that native doesn't give the right return,
     * in which case may be slightly safer to use it here:
     */
    vsnprintf(buf, VSNP_BUF_SIZE, format, ap);
#else
    vsprintf(buf, format, ap);
#endif

    /* Check strlen of buf straight away: could be more efficient
       not to do this and step through instead (eg if n small and
       len long), but useful anyway to get the return value, and
       importantly want to abort if vsprintf clobbered the heap!
       (Don't just use return from vsprintf since not sure if
       that gives the right thing on all platforms?)
       Will maintain last char of buffer as null, and use SIZE-2
       as longest string which we can detect as untruncated.
       (Don't use freelog() for report since that uses vsnprintf...)
    */
    buf[VSNP_BUF_SIZE-1] = '\0';
    len = strlen(buf);
    if (len >= VSNP_BUF_SIZE-1) {
      fprintf(stderr, "Overflow in vsnprintf replacement!"
	      " (buffer size %d) aborting...\n", VSNP_BUF_SIZE);
      abort();
    }

    if (n > len) {
      memcpy(str, buf, len+1);	/* +1 for terminating null */
      return len;
    } else {
      memcpy(str, buf, n-1);
      str[n-1] = '\0';
      return len;		/* truncated */
    }
  }
}
#endif


#if HAVE_SNPRINTF != 1
int snprintf(char *str, size_t n, const char *format, ...)
  {
  int ret;
  va_list ap;

  assert(format);
  
  va_start(ap, format);
  ret = my_vsnprintf(str, n, format, ap);
  va_end(ap);
  return ret;
  }
#endif /* HAVE_SNPRINTF */


#if HAVE_VSNPRINTF != 1
int vsnprintf(char *str, size_t n, const char *format, va_list ap)
  {
  return my_vsnprintf(str, n, format, ap);
  }
#endif /* HAVE_VSNPRINTF */


#if HAVE_MEMSCAN != 1
/*
 * Find a character in an area of memory.
 * Returns the address of the first occurrence of c, or 1 byte past the area if c is not found.
 */
void *memscan(void *addr, int c, size_t size)
  {
  unsigned char *p = (unsigned char*)addr;

  while(size)
    {
    if(*p == c) return (void*)p;
    p++;
    size--;
    }

  return (void*)p;
  }
#endif /* HAVE_MEMSCAN */


#if HAVE_MEMSET != 1
/*
 * Set LEN characters in STR to character C
 */
#if USE_OPTIMISED_MEMSET != 1
/* Original version.  Must use this on Solaris, by the looks of things. */
void *memset(void *str, int c, size_t len)
  {
  char	*orig = str;
  
  for (; len > 0; len--, str++) *(char *)str = (char)c;
  
  return orig;
  }

#else

/* Optimised version */
void *memset(void *dst0, int c0, size_t bytes)
  {
    size_t t;
    unsigned int c;
    unsigned char *dst;
    const int word_size = sizeof(unsigned int);
    const int word_mask = (sizeof(unsigned int) - 1);

    dst = dst0;

    /* if not enough words for a reasonable speedup, just fill bytes */
    if (bytes < 3 * word_size) {
        while (bytes != 0) {
            *dst++ = c0;
            --bytes;
        }
        return dst0;
    }

    /* fill the whole stamping word */
    if ((c = (unsigned char)c0) != 0) { 
        c = c | (c << 8);
#if (SIZEOF_INT > 2)
        c = c | (c << 16);
#endif
#if (SIZEOF_INT > 4)
        c = c | (c << 32);
#endif
    }

    /* align destination by filling in bytes */
    if ((t = (long)dst & word_mask) != 0) {
        t = word_size - t;
        bytes -= t;
        do {
            *dst++ = c0;
        } while (--t != 0);
    }

    /* now fill with words. length was >= 2*words so we know t >= 1 here */
    t = bytes / word_size;
    do {
        *(unsigned int *)dst = c;
        dst += word_size;
    } while (--t != 0);

    /* finish with trailing bytes, if there are bytes left */
    t = bytes & word_mask;
    if (t != 0) {
        do {
            *dst++ = c0;
        } while (--t != 0);
    }

  return dst0;
  }
#endif
#endif /* HAVE_MEMSET */

#if HAVE_MEMMOVE != 1
#if HAVE_BCOPY != 1
/*
 * Some systems, such as SunOS do have BCOPY instead.
 * In which case this is defined as a macro in the header.
 */
void *memmove(void *dst, const void *src, size_t bytes)
  {
  unsigned char *dst_p;
  const unsigned char *src_p;

  if (src == NULL || dst == NULL) return NULL;

  if (dst > src)
    {	/* To avoid dodgy overwrites must go high to low */
    src_p = src + bytes;
    dst_p = dst + bytes;
    while (bytes-- > 0) *--dst_p = *--src_p;
    }
  else if (dst < src)
    {	/* To avoid dodgy overwrites must go low to high */
    src_p = src;
    dst_p = dst;
    while (bytes-- > 0) *dst_p++ = *src_p++;
    }

  return dst;
  }
#endif /* HAVE_BCOPY */
#endif /* HAVE_MEMMOVE */


#if HAVE_MEMREV != 1
void *memrev(void *src, size_t bytes)
  {
  unsigned char *p1;
  unsigned char *p2;
  unsigned char c;

  p1 = (unsigned char *)src;
  p2 = (unsigned char *)src + bytes;
  while (p1 < p2) c = *p1, *p1++ = *p2, *p2-- = c;

  return src;
  }
#endif /* HAVE_MEMREV */


#if HAVE_MEMCHR != 1
void *memchr(const void *src, int c, size_t bytes)
{
    const unsigned char *cp;

    if (bytes != 0) {
        cp = src;
        do {
            if (*cp++ == (unsigned char)c)
                return ((void *)(cp - 1));
        } while (--bytes != 0);
    }

    return NULL;
}
#endif /* HAVE_MEMCHR */


#if HAVE_MEMMEM != 1
void *memmem(const void *haystack, size_t haystack_len,
           const void *needle,   size_t needle_len)
{
    register const char *begin;
    register const char *last_possible;
    
    if (needle_len == 0) 
        /* The first occurrence of the empty string is deemed to occur at
           the end of the string. */
        return (void *)&((const char *)haystack)[haystack_len - 1];
    last_possible = (const char *)haystack + haystack_len - needle_len;
    for (begin = (const char *)haystack; begin <= last_possible; begin++)
        if (*begin == *((const char *)needle) &&
            memcmp(&begin[1], (const char *)needle + 1, needle_len - 1) == 0)
        return (void *)begin;

    return NULL;
}
#endif /* HAVE_MEMMEM */


#if HAVE_MEMCMP != 1
#if HAVE_BCMP != 1
/*
 * Some systems, such as SunOS do have BCMP instead.
 * In which case this is defined as a macro in the header.
 */
int memcmp(const void *src1, const void *src2, size_t n)
  {
  const unsigned char *cp1=src1;
  const unsigned char *cp2=src2;
   
  while (n-- > 0) if (*cp1++ != *cp2++) return (*--cp1 - *--cp2);

  return 0;
  }
#endif /* HAVE_BCMP */
#endif /* HAVE_MEMCMP */


#if HAVE_STRDUP != 1
char *strdup(const char *str)
  {
  char *new_str;

  if (!str) return NULL;

  new_str = s_malloc(sizeof(char)*(strlen(str)+1));
  strcpy(new_str, str);

  return new_str;
  }
#endif /* HAVE_STRDUP */


#if HAVE_MEMDUP != 1
void *memdup(const void *mem, int byte_size)
  {
  void *dest;

  if (!mem) return NULL;

  dest = s_malloc(byte_size);

/* Don't need memmove; I be concerned if these blocks of memory ever overlap! */
  memcpy(dest, mem, byte_size);

  return dest;
  }
#endif /* HAVE_MEMDUP */


#if HAVE_STRNDUP != 1
char *strndup(const char *str, size_t n)
  {
  char *new_str=NULL;

  if (str)
    {
    new_str = s_malloc(sizeof(char)*(n+1));
    strncpy(new_str, str, n);
    new_str[n] = '\0';
    }

  return new_str;
  }
#endif /* HAVE_STRNDUP */


#if HAVE_STRNFILL != 1
char *strnfill(int length, char fill_char)
  {
  char *str, *s, *end;

  str = s_malloc(sizeof(char)*(length+1));

  s = str;
  end = str + length;
  while(s < end) *(s++) = fill_char;
  *s = 0;

  return str;
  }
#endif /* HAVE_STRNFILL */


#if 0
char* strdup_vprintf(const char *format,
		  va_list      args1)
  {
  char *buffer;
  va_list args2;

  VA_COPY(args2, args1);

  buffer = s_malloc(sizeof(char)*(printf_string_upper_bound(format, args1)));

  vsprintf(buffer, format, args2);
  va_end(args2);

  return buffer;
  }

char*
strdup_printf(const char *format,
		 ...)
  {
  char *buffer;
  va_list args;

  va_start(args, format);
  buffer = strdup_vprintf(format, args);
  va_end(args);

  return buffer;
  }
#endif


#if HAVE_STRCATV != 1
char *strcatv(const char *string1, ...)
  {
  int	  l;
  va_list args;
  char	  *s;
  char	  *concat;

  if (!string1) return NULL;

  l = 1 + strlen(string1);
  va_start(args, string1);
  s = va_arg(args, char*);
  while(s)
    {
    l += strlen(s);
    s = va_arg(args, char*);
    }
  va_end(args);

  concat = s_malloc(sizeof(char)*l);
  concat[0] = 0;

  strcat(concat, string1);
  va_start(args, string1);
  s = va_arg(args, char*);
  while(s)
    {
    strcat(concat, s);
    s = va_arg(args, char*);
    }
  va_end(args);

  return concat;
  }
#endif /* HAVE_STRCATV */


#if HAVE_STRTOD != 1
double strtod(const char *nptr, char **endptr)
  {
  char *fail_pos_1;
  char *fail_pos_2;
  double val_1;
  double val_2 = 0;

  if (!nptr) return 0;

  fail_pos_1 = NULL;
  fail_pos_2 = NULL;

  val_1 = strtod(nptr, &fail_pos_1);

  if (fail_pos_1 && fail_pos_1[0] != 0)
    {
#if HAVE_SETLOCALE != 1
    val_2 = strtod(nptr, &fail_pos_2);
#else
    char *old_locale;

    old_locale = strdup(setlocale(LC_NUMERIC, NULL));
    setlocale(LC_NUMERIC, "C");
    val_2 = strtod(nptr, &fail_pos_2);
    setlocale(LC_NUMERIC, old_locale);
    s_free(old_locale);
#endif
    }

  if (!fail_pos_1 || fail_pos_1[0] == 0 || fail_pos_1 >= fail_pos_2)
    {
    if (endptr) *endptr = fail_pos_1;
    return val_1;
    }
  else
    {
    if (endptr) *endptr = fail_pos_2;
    return val_2;
    }

/* Should never get here! */
  }
#endif /* HAVE_STRTOD */


#if HAVE_STRSIGNAL != 1
char *strsignal(int signum)
  {
  switch(signum)
    {
#ifdef SIGHUP
    case SIGHUP: return "Hangup";
#endif
#ifdef SIint
    case SIint: return "Interrupt";
#endif
#ifdef SIGQUIT
    case SIGQUIT: return "Quit";
#endif
#ifdef SIGILL
    case SIGILL: return "Illegal instruction";
#endif
#ifdef SIGTRAP
    case SIGTRAP: return "Trace/breakpoint trap";
#endif
#ifdef SIGABRT
    case SIGABRT: return "IOT trap/Abort";
#endif
#ifdef SIGBUS
    case SIGBUS: return "Bus error";
#endif
#ifdef SIGFPE
    case SIGFPE: return "Floating point exception";
#endif
#ifdef SIGKILL
    case SIGKILL: return "Killed";
#endif
#ifdef SIGUSR1
    case SIGUSR1: return "User defined signal 1";
#endif
#ifdef SIGSEGV
    case SIGSEGV: return "Segmentation fault";
#endif
#ifdef SIGUSR2
    case SIGUSR2: return "User defined signal 2";
#endif
#ifdef SIGPIPE
    case SIGPIPE: return "Broken pipe";
#endif
#ifdef SIGALRM
    case SIGALRM: return "Alarm clock";
#endif
#ifdef SIGTERM
    case SIGTERM: return "Terminated";
#endif
#ifdef SIGSTKFLT
    case SIGSTKFLT: return "Stack fault";
#endif
#ifdef SIGCHLD
    case SIGCHLD: return "Child exited";
#endif
#ifdef SIGCONT
    case SIGCONT: return "Continued";
#endif
#ifdef SIGSTOP
    case SIGSTOP: return "Stopped(signal)";
#endif
#ifdef SIGTSTP
    case SIGTSTP: return "Stopped";
#endif
#ifdef SIGTTIN
    case SIGTTIN: return "Stopped(tty input)";
#endif
#ifdef SIGTTOU
    case SIGTTOU: return "Stopped(tty output)";
#endif
#ifdef SIGURG
    case SIGURG: return "Urgent condition";
#endif
#ifdef SIGXCPU
    case SIGXCPU: return "CPU time limit exceeded";
#endif
#ifdef SIGXFSZ
    case SIGXFSZ: return "File size limit exceeded";
#endif
#ifdef SIGVTALRM
    case SIGVTALRM: return "Virtual time alarm";
#endif
#ifdef SIGPROF
    case SIGPROF: return "Profile signal";
#endif
#ifdef SIGWINCH
    case SIGWINCH: return "Window size changed";
#endif
#ifdef SIGIO
    case SIGIO: return "Possible I/O";
#endif
#ifdef SIGPWR
    case SIGPWR: return "Power failure";
#endif
#ifdef SIGUNUSED
    case SIGUNUSED: return "Unused signal";
#endif
    }

  return "Unknown signal";
  }
#endif


#if 0
int printf_string_upper_bound(const char* format,
			     va_list      args)
  {
  int len = 1;

  while(*format)
    {
      gboolean long_int = FALSE;
      gboolean extra_long = FALSE;
      char c;

      c = *format++;

      if (c == '%')
	{
	  gboolean done = FALSE;

	  while(*format && !done)
	    {
	      switch(*format++)
		{
		  char *string_arg;

		case '*':
		  len += va_arg(args, int);
		  break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		  /* add specified format length, since it might exceed the
		   * size we assume it to have.
		   */
		  format -= 1;
		  len += strtol(format,(char**) &format, 10);
		  break;
		case 'h':
		  /* ignore short int flag, since all args have at least the
		   * same size as an int
		   */
		  break;
		case 'l':
		  if (long_int)
		    extra_long = TRUE; /* linux specific */
		  else
		    long_int = TRUE;
		  break;
		case 'q':
		case 'L':
		  long_int = TRUE;
		  extra_long = TRUE;
		  break;
		case 's':
		  string_arg = va_arg(args, char *);
		  if (string_arg)
		    len += strlen(string_arg);
		  else
		    {
		      /* add enough padding to hold "(null)" identifier */
		      len += 16;
		    }
		  done = TRUE;
		  break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
#ifdef	G_HAVE_int64
		  if (extra_long)
		   (void) va_arg(args, int64);
		  else
#endif	/* HAVE_int64 */
		    {
		      if (long_int)
			(void) va_arg(args, long);
		      else
			(void) va_arg(args, int);
		    }
		  len += extra_long ? 64 : 32;
		  done = TRUE;
		  break;
		case 'D':
		case 'O':
		case 'U':
		 (void) va_arg(args, long);
		  len += 32;
		  done = TRUE;
		  break;
		case 'e':
		case 'E':
		case 'f':
		case 'g':
#ifdef HAVE_LONG_DOUBLE
		  if (extra_long)
		   (void) va_arg(args, long double);
		  else
#endif	/* HAVE_LONG_DOUBLE */
		   (void) va_arg(args, double);
		  len += extra_long ? 128 : 64;
		  done = TRUE;
		  break;
		case 'c':
		 (void) va_arg(args, int);
		  len += 1;
		  done = TRUE;
		  break;
		case 'p':
		case 'n':
		 (void) va_arg(args, void*);
		  len += 32;
		  done = TRUE;
		  break;
		case '%':
		  len += 1;
		  done = TRUE;
		  break;
		default:
		  /* ignore unknow/invalid flags */
		  break;
		}
	    }
	}
      else
	len += 1;
    }

  return len;
  }
#endif


#if HAVE_STRREV != 1
void strrev(char *string)
  {
  if (!string) return;

  if (*string)
    {
    char *h, *t;

    h = string;
    t = string + strlen(string) - 1;

    while(h < t)
      {
      char c;

      c = *h;
      *h = *t;
      h++;
      *t = c;
      t--;
      }
    }

  return;
  }
#endif /* HAVE_STRREV */


#if HAVE_STRERROR != 1
char *strerror(int errnum)
  {
  static char buf[64];
  snprintf(buf, sizeof(buf), "error %d (compiled without strerror)", errnum);
  return buf;
  }
#endif /* HAVE_STRERROR */


#if HAVE_DIEF != 1
/*
 * Needed as a function because many compilers don't use vararg macros.
 * HAVE_DIEF is set in "SAA_header.h", not "config.h".
 */
void dief(const char *format, ...)
  {
  va_list       ap;                        /* variable args structure */

  printf("FATAL ERROR: ");
  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);
  printf("\n");

  abort();
  }
#endif /* HAVE_DIEF */


#if HAVE_STRSPN != 1
/*
 * The strspn() function calculates the length of the initial segment of s which
 * consists entirely of characters in accept.
 */
size_t strspn(const char *string, const char *accept)
  {
  size_t count = 0;

  while (strchr(accept, *string)!=0)
    {
    count++;
    string++;
    }

  return count;
  }
#endif /* HAVE_STRSPN */


#if HAVE_STRCSPN != 1
/*
 * The  strcspn()  function  calculates  the  length  of the initial segment of s which
 * consists entirely of characters not in reject.
 */
size_t strcspn(const char *string, const char *reject)
  {
  size_t count = 0;

  while (strchr(reject, *string) == 0)
    {
    count++;
    string++;
    }

  return count;
  }
#endif /* HAVE_STRCSPN */


#if HAVE_WAITPID != 1 && !defined( W32_CRIPPLED )
pid_t waitpid(pid_t pid, int *pstatus, int options)
  {
  pid_t result;

  do
    {
    result = wait(pstatus);
    } while (result >= 0 && result != pid);

  return result;
  }
#endif /* HAVE_WAITPID */


#if HAVE_MIN != 1
int min( int a, int b )
  {
  return a <= b ? a : b;
  }
#endif


#if HAVE_MAX != 1
int max( int a, int b )
  {
  return a >= b ? a : b;
  }
#endif


/*
 * The next 3 functions are commented out because they are
 * identical to other functions, with the exception of their
 * names.  FIXME: Should create sensible macros and/or check
 * for native versions to these to use instead of the above
 * replacement functions.
 * (I believe that these are ANSI-defined functions that were
 * replaced in the POSIX specifications)
 */
#if HAVE_STRUPR != 1
char *strupr( char *s )
  {
  char    *p = s;

  while( *s )
    {
    *s = toupper( *s );
    s++;
    }

  return p;
  }
#endif /* HAVE_STRUPR */


#if HAVE_STRICMP != 1
int stricmp( char *s1, char *s2 )
{
        while( *s1 && *s2 ) {
                if( toupper( *s1 ) < toupper( *s2 ) )
                        return -1;
                else if( toupper( *s1 ) > toupper( *s2 ) )
                        return 1;
                s1++; s2++;
        }
        if( *s1 < *s2 )
                return -1;
        else if( *s1 > *s2 )
                return 1;
        else
                return 0;
}
#endif /* HAVE_STRICMP */


#if HAVE_STRNICMP != 1
int strnicmp( char *s1, char *s2, int n )
{
        int     i;

        for( i=0; i<n; i++ ) {
                if( toupper( *s1 ) < toupper( *s2 ) )
                        return -1;
                else if( toupper( *s1 ) > toupper( *s2 ) )
                        return 1;
                s1++; s2++;
        }

        return 0;
}
#endif /* HAVE_STRNICMP */


#if HAVE_SINCOS != 1
/*
 * This is an undocumented GNU extension, which is actually fairly useful.
 */
void sincos( double radians, double *s, double *c )
  {

#if __i368__
  __asm__ ("fsincos" : "=t" (*c), "=u" (*s) : "0" (radians));
#else
  *s = sin(radians);
  *c = cos(radians);
#endif

/*printf("DEBUG: sincos(%f) = %f %f\n", radians, *s, *c);*/

  return;
  }
#endif /* HAVE_SINCOS */


#if HAVE_ITOA != 1
/*
 * Convert an integer to a string.
 */
void itoa(const int n, char *s)
  {
  int   number=n;
  char	c, *end=s;

  if (n < 0) {number = -number;}

  do
    {
    *end++ = (number % 10) + '0';
    } while ((number /= 10) > 0);

  if (n < 0) {*end++ = '-';}

  *end-- = '\0';

  while ( *s < *end )
    {
    c = *s;
    *s++ = *end;
    *end++ = c;
    }

  return;
  }
#endif /* HAVE_ITOA */

#if HAVE_GETHOSTNAME != 1
/*
 * gethostname() - get host name

   This function is used to access the host name of the current processor.  It
   returns a NULL-terminated hostname by filling the array name with a length of
   len bytes.  In case the NULL-terminated hostname does not fit, no error is
   returned, but the hostname is truncated.  In the specification, it is
   unspecified whether the truncated hostname will be NULL-terminated, but here
   it is.

   What should happen is that upon success, zero is returned.  On error, -1 
   is returned, and errno is set to EINVAL.  But here the hostname is set to
   <unknown>.

  From the manpage:
       EINVAL len is negative or, for sethostname, len is larger than the
              maximum allowed size, or, for gethostname on Linux/i386, len
              is smaller than the actual size.  (In this last case glibc 2.1
              uses ENAMETOOLONG.)

   SUSv2 guarantees that host names are limited to 255 bytes.  POSIX
   1003.1-2001 guarantees that host names (not including the terminating
   NUL) are limited to HOST_NAME_MAX bytes.

 */

int gethostname(char *name, size_t len)
  {

  snprintf(name, len, "<unknown>");

  return TRUE;
  }
#endif /* HAVE_GETHOSTNAME */

