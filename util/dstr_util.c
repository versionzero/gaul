/**********************************************************************
  dstr_util.c
 **********************************************************************

  dstr_util - Dynamic string library.
  Copyright ©1999-2002, Stewart Adcock <stewart@linux-domain.com>

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

  Synopsis:	Dynamic string library.

		These routines still require a lot of work (and testing).

  Updated:      11 Apr 2002 SAA	Added dstr_fromstr() convenience function.
		20 Mar 2002 SAA Replaced use of printf("%Zd", (size_t)) to printf("%lu", (unsigned long)).
		14 Mar 2002 SAA	Changes for clean compilation under AIX.
		13 Mar 2002 SAA	dstr_diagnostics() changed.  Thread locking variable renamed.
		20/06/01 SAA	Added a couple of casts for clean compilation on Solaris.
		27/02/01 SAA	G_LOCK etc. replaced with THREAD_LOCK etc..
		16/01/01 SAA	Defining the constant DSTR_NO_CHUNKS will disable the use of chunks for memory handling.
		03/01/01 SAA	Adapated to use my efficient memory chunk memory allocation routines.
		21/11/00 SAA	IRIX fixes.
		09/07/00 SAA	Removed reliance upon my trace library.
		11/04/00 SAA	Added dstr_countchar().  Added a few assertions.  Filled-in a few tests around memory allocations.
		16/03/00 SAA	Added dstr_scmp_str().
		19/02/00 SAA	Added dstr_readline().
		11/01/00 SAA	Added dstr_setf().
		06/11/99 SAA	Added hashing functions dstr_hash() and dstr_equal() for use with glib's hashing routines.
		25/10/99 SAA	Neatened output from dstr_diagnostics().
		20/10/99 SAA	Some tidying.  Added dstr_match() and str_match_str().
		09/10/99 SAA	dstr_{left/right}_adjust() now return the number of spaces trimmed.  Added dstr_diagnostics();
		14/09/99 SAA	dstr_cmpn(), dstr_cmp_str(), dstr_cmpn_str() added.
		13/09/99 SAA	Minor modifications to make 'ccc' play nicely.
		08/09/99 SAA	dstr_set() and dstr_setn() written.
		30/06/99 SAA	Some more work done on these routines because I'm going to use them in my sequence storage code.
		09/03/99 SAA	based on my str_util.c, with code pulled from various files I've written over time.

  To do:	Remove assertions.
		Wild-card matching.
		Right/left/center justifications.  dstr_fill(*dstr, char, len, mode)
		Documentation.
		Replace assertions with if...die constructs.

		*** Do I need to allocate (max_size+1) everywhere? ***

 **********************************************************************/

/* #define DSTR_NO_CHUNKS 1 To disable usage of memory chunks */

#include "dstr_util.h"

/*
 * Global variables.
 */
THREAD_LOCK_DEFINE_STATIC(dstr_global_lock);
static MemChunk         *dstr_mem_chunk = NULL;
MemChunk	*dstr_get_memchunk() { return dstr_mem_chunk; }


/**********************************************************************
  void dstr_diagnostics()
  synopsis:	Displays diagnostic information about these routines.
  parameters:   none
  return:	none
  last updated: 13 Mar 2002
 **********************************************************************/

void dstr_diagnostics(void)
  {

  printf("=== dynamic string (dstr) routines diagnostic information ====\n");
  printf("Version:                   %s\n", VERSION_STRING);
  printf("Build date:                %s\n", BUILD_DATE_STRING);
  printf("Compilation machine characteristics:\n%s\n", UNAME_STRING);

  printf("--------------------------------------------------------------\n");
  printf("DSTR_DEBUG:                %d\n", DSTR_DEBUG);
#ifdef DSTR_NO_CHUNKS
  printf("DSTR_NO_CHUNKS:            defined\n");
#else
  printf("DSTR_NO_CHUNKS:            undefined\n");
#endif

  printf("--------------------------------------------------------------\n");
  printf("structure          sizeof\n");
  printf("dstring            %lu\n", (unsigned long) sizeof(dstring));

  printf("==============================================================\n");

  return;
  }


/**********************************************************************
  boolean dstr_free(dstring *ds)
  synopsis:	Deallocate a dynamic string.
  parameters:   dstring	*ds	The dynamic string.
  return:	success
  last updated: 09/03/99
 **********************************************************************/

boolean dstr_free(dstring *ds)
  {

  if (!ds)
    {
    die("Null dstring pointer passed.");
/*
    warn("Null dstring pointer passed.");
    return(FALSE);
*/
    }

  if(ds->string) s_free(ds->string);

#ifndef DSTR_NO_CHUNKS
  THREAD_LOCK(dstr_global_lock);
  mem_chunk_free(dstr_mem_chunk, ds);
  THREAD_UNLOCK(dstr_global_lock);
#else
  s_free(ds);
#endif

  return(TRUE);
  }


/**********************************************************************
  boolean dstr_isvalid(const dstring *ds)
  synopsis:	Checks contents of a dynamic string.
  parameters:   dstring	*ds	The dynamic string.
  return:	TRUE if dstring is valid, otherwise FALSE
  last updated: 09/03/99
 **********************************************************************/

boolean dstr_isvalid(const dstring *ds)
  {

#ifdef DSTR_ISVALID_DIE
  if (!ds) die("Failed (!ds) test.");
  if (!ds->string) die("Failed (!ds->string) test.");
  if (ds->max_size < ds->size) die("Failed (ds->max_size < ds->size) test.");
#else
  if (!ds) return(FALSE);
  if (!ds->string) return(FALSE);
  if (ds->max_size < ds->size) return(FALSE);
#endif

/*  return(strlen(ds->string) == ds->size);*/
  return(TRUE);
  }


/**********************************************************************
  unsigned int dstr_hash()
  synopsis:	Hash function for GHashTable functions in glib.
  parameters:   dstring	*s	The dynamic string.
  return:	int		The hash value of the dstring.
  last updated: 06/11/99
 **********************************************************************/

unsigned int dstr_hash(constvpointer s)
  {
  const char	*str=((dstring *)s)->string;
  const char	*p;
  unsigned int		hash=0, g; 
  
  for(p=str; *p!='\0'; p++)
    {
    hash = ( hash<<4 ) + *p;
    if ( ( g=hash&0xf0000000 ) )
      {
      hash = hash ^ (g>>24);
      hash = hash ^ g; 
      }
    }

  return(hash);
  }


/**********************************************************************
  int dstr_equal()
  synopsis:	Comparison of two dstrings for glib's hashing routines.
  parameters:   dstring	*s1, *s2	The dynamic strings.
  return:	TRUE/FALSE
  last updated: 06/11/99
 **********************************************************************/

int dstr_equal(constvpointer s1, constvpointer s2)
  {
/*
printf("DEBUG: comparing \"%s\" and \"%s\".\n", ((const dstring *)s1)->string, ((const dstring *)s2)->string);
*/
  return(dstr_cmp((const dstring *)s1, (const dstring *)s2)==0);
  }


/**********************************************************************
  int dstr_len(const dstring *ds)
  synopsis:	Returns length of a dynamic string.
  parameters:   dstring	*ds	The dynamic string.
  return:	Length of dstring.
  last updated: 09/03/99
 **********************************************************************/

int dstr_len(const dstring *ds)
  {

  if (!dstr_isvalid(ds)) die("Invalid dstring.\n");

  return(ds->size);
  }


/**********************************************************************
  void dstr_empty(dstring *ds)
  synopsis:	Clears contents of a dynamic string.  Does not
		deallocate the structure so it can be re-used.
  parameters:   dstring	*ds	The dynamic string.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

void dstr_empty(dstring *ds)
  {
  if (!dstr_isvalid(ds)) die("Invalid dstring.\n");

  ds->string[0] = '\0';
  ds->size = 0;
  }


/**********************************************************************
  dstring *dstr_create()
  synopsis:	Allocates an empty dynamic string of a given size.
  parameters:	const int max_size
  return:	dstring	*ds	The dynamic string.
  last updated: 09/03/99
 **********************************************************************/

dstring *dstr_create(const int max_size)
  {
  dstring	*ds=NULL;

  if (max_size < 0) die("Stupid dstring maximum length requested.\n");

#ifndef DSTR_NO_CHUNKS
  THREAD_LOCK(dstr_global_lock);
  if (!dstr_mem_chunk)
    dstr_mem_chunk = mem_chunk_new(sizeof(dstring), 1024);

  ds = mem_chunk_alloc(dstr_mem_chunk);
  THREAD_UNLOCK(dstr_global_lock);
#else
  if ( !(ds = s_malloc(sizeof(dstring))) ) die("Unable to allocate memory.");
#endif

  ds->max_size = (max_size<DSTR_MAXSIZE_INITIAL)?DSTR_MAXSIZE_INITIAL:max_size;
  if ( !(ds->string = s_malloc((ds->max_size+1) * sizeof(char))) ) die("Unable to allocate memory.");

  ds->string[0] = '\0';
  ds->size = 0;

  return(ds);
  }


/**********************************************************************
  dstring *dstr_allocate()
  synopsis:	Allocates an empty dynamic string of the default
		initial size.
  parameters:   dstring	*ds	The dynamic string.
  return:	success
  last updated: 30/06/99
 **********************************************************************/

dstring *dstr_allocate(void)
  {
  dstring	*ds;

#ifndef DSTR_NO_CHUNKS
  THREAD_LOCK(dstr_global_lock);
  if (!dstr_mem_chunk)
    dstr_mem_chunk = mem_chunk_new(sizeof(dstring), 1024);

  ds = mem_chunk_alloc(dstr_mem_chunk);
  THREAD_UNLOCK(dstr_global_lock);
#else
  if ( !(ds = s_malloc(sizeof(dstring))) ) die("Unable to allocate memory.");
#endif

  ds->max_size = DSTR_MAXSIZE_INITIAL;

  if ( !(ds->string = s_malloc((ds->max_size+1) * sizeof(char))) )
    die("Unable to allocate memory.\n");

  ds->string[0] = '\0';
  ds->size = 0;

#if DSTR_DEBUG>2
  printf("DEBUG: max_size = %d\n", ds->max_size);
#endif

  return(ds);
  }


/**********************************************************************
  dstring *dstr_convertstr()
  synopsis:	Creates a dstring from a standard null-terminated
		char array.
  parameters:	char *str
  return:	dstring	*ds	The dynamic string.
  last updated: 30/06/99
 **********************************************************************/

dstring *dstr_convertstr(char *str)
  {
  dstring	*ds;

#ifndef DSTR_NO_CHUNKS
  THREAD_LOCK(dstr_global_lock);
  if (!dstr_mem_chunk)
    dstr_mem_chunk = mem_chunk_new(sizeof(dstring), 1024);

  ds = mem_chunk_alloc(dstr_mem_chunk);
  THREAD_UNLOCK(dstr_global_lock);
#else
  if ( !(ds = s_malloc(sizeof(dstring))) ) die("Unable to allocate memory.");
#endif

  ds->string = str;

  if (!ds || !ds->string) die("Unable to allocate memory.\n");

  ds->string[0] = '\0';
  ds->size = 0;

  return( ds );
  }


/**********************************************************************
  dstring *dstr_fromstr()
  synopsis:	Creates a dstring from a standard null-terminated
		char array.
  parameters:	char *str
  return:	dstring	*ds	The dynamic string.
  last updated: 11 Apr 2002
 **********************************************************************/

dstring *dstr_fromstr(char *str)
  {
  dstring	*ds;

#ifndef DSTR_NO_CHUNKS
  THREAD_LOCK(dstr_global_lock);
  if (!dstr_mem_chunk)
    dstr_mem_chunk = mem_chunk_new(sizeof(dstring), 1024);

  ds = mem_chunk_alloc(dstr_mem_chunk);
  THREAD_UNLOCK(dstr_global_lock);
#else
  if ( !(ds = s_malloc(sizeof(dstring))) ) die("Unable to allocate memory.");
#endif

  ds->string = s_strdup(str);
  ds->size = strlen(str);

  return( ds );
  }


/**********************************************************************
  static void dstr_realloc()
  synopsis:	Increase the max_size of the dstring to at least the
		given value.  Do nothing if string already has
		sufficient dimensions.
  parameters:   dstring	*ds		The dynamic string.
		const int max_size	The new max_size.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

static void dstr_realloc(dstring *ds, const int max_size)
  {
  if (!ds) die("Null pointer to dstring passed.");

  if (ds->max_size >= max_size) return;

/*
 * Determine new max_size, which will be at least 
 * double the current max_size
 */
  ds->max_size *= 2;
  if (ds->max_size < max_size) ds->max_size = max_size;

  if ( !(ds->string = s_realloc(ds->string, (ds->max_size+1)*sizeof(char))) )
    die("Unable to reallocate memory.");
  }


/**********************************************************************
  void dstr_reallocate()
  synopsis:	Increase the max_size of the dstring to at least the
		given value.
  parameters:   dstring	*ds	The dynamic string.
		const int max_size	The new max_size.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

void dstr_reallocate(dstring *ds, const int max_size)
  {
  if (max_size <= 0) die("Stupid max_size requested.");
  if (!dstr_isvalid(ds)) die("Invalid dstring passed.");

  dstr_realloc(ds, max_size);
  }


/**********************************************************************
  int dstr_cmp()
  synopsis:	Wrapper around strcmp() for dstrings.
  parameters:   dstring	*dstr1, *dstr2	The dynamic strings.
  return:	int
  last updated: 09/03/99
 **********************************************************************/

int dstr_cmp(const dstring *dstr1, const dstring *dstr2)
  {

  if (!dstr_isvalid(dstr1) || !dstr_isvalid(dstr2))
    die("Invalid dstring passed.");

  return(strcmp(dstr1->string, dstr2->string));
  }


/**********************************************************************
  int dstr_cmpn()
  synopsis:	Wrapper around strncmp() for dstrings.
  parameters:   dstring	*dstr1, *dstr2	The dynamic strings.
		const int len	The number of chars to compare.
  return:	int
  last updated: 14/09/99
 **********************************************************************/

int dstr_cmpn(const dstring *dstr1, const dstring *dstr2, const int len)
  {

  if (!dstr_isvalid(dstr1) || !dstr_isvalid(dstr2))
    die("Invalid dstring passed.");
  if (len < 1) die("Stupid length passed.");

  return(strncmp(dstr1->string, dstr2->string, len));
  }


/**********************************************************************
  int dstr_cmp_str()
  synopsis:	Wrapper around strcmp() for dstrings vs. char[].
  parameters:   dstring	*dstr	The dynamic string.
		char	*str	The char[].
  return:	int
  last updated: 14/09/99
 **********************************************************************/

int dstr_cmp_str(const dstring *dstr, const char *str)
  {

  if (!dstr_isvalid(dstr)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char[] passed.");

  return(strcmp(dstr->string, str));
  }


/**********************************************************************
  int dstr_scmp_str()
  synopsis:	Wrapper around strcmp() for dstrings vs. char[].
		Ignores trailing white space.
  parameters:   dstring	*dstr	The dynamic string.
		char	*str	The char[].
  return:	int
  last updated: 13/03/00
 **********************************************************************/

int dstr_scmp_str(const dstring *dstr, const char *str)
  {
  char	c1, c2;         /* Temp chars */
  char	*s1;		/* Temp pointers to chars */

  if (!dstr_isvalid(dstr)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char[] passed.");

#if DSTR_DEBUG>2
  printf("Comparing dynamic string \"%s\" with character array \"%s\"\n", dstr->string, str);
#endif

/* Reduce lookup overhead */
  s1 = dstr->string;

  if (s1 == str) return 0;

/*  s2 = str; */

  c1 = *s1++;
  c2 = *str++;

/* Compare upto first non-match, or null char */
  while (c1 != '\0' && c2 != '\0')
    {
/*
printf("1. c1 = %d c2 = %d\n", (int)c1, (int)c2);
*/
    if (c1 != c2) return((int) c1-c2);
    c1 = *s1++;
    c2 = *str++;
    }

/* Check remainder of strings */
  if (c1 == '\0')
    {
    while (c2 != '\0')
      {
/*
printf("2. c1 = %d c2 = %d\n", (int)c1, (int)c2);
*/
      if (!isspace((int)c2)) return(-1);
      c2 = *str++;
      }
    }
  else
    {
    while (c1 != '\0')
      {
/*
printf("3. c1 = %d c2 = %d\n", (int)c1, (int)c2);
*/
      if (!isspace((int)c1)) return(1);
      c1 = *s1++;
      }
    }

  return 0;
  }


/**********************************************************************
  int dstr_cmpn_str()
  synopsis:	Wrapper around strncmp() for dstrings vs. char[].
  parameters:   dstring	*dstr	The dynamic string.
		char	*str	The char[].
		const int len	The number of chars to compare.
  return:	int
  last updated: 14/09/99
 **********************************************************************/

int dstr_cmpn_str(const dstring *dstr, const char *str, const int len)
  {

  if (!dstr_isvalid(dstr)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char[] passed.");

#if DSTR_DEBUG>2
  printf("DEBUG: comparing %d chars of \"%s\" and \"%s\"\n", len, dstr->string, str);
#endif

  return(strncmp(dstr->string, str, len));
  }


#if 0
/*------------------------------------------------------------*/
dstring *
dstr_subcreate (const char *str, int first, int last)
     /*
       Create a dstring containing the given substring.
     */
{
  dstring *new = s_malloc (sizeof (dstring));

  /* pre */
  s_assert (str);
  s_assert (first >= 0);
  s_assert (last < strlen (str));
  s_assert (first <= last);

  new->size = last - first + 1;
  dstr_allocate (new, new->size);
  strncpy (new->string, str + first, new->size);
  new->string[new->size] = '\0';

  s_assert (dstr_isvalid (new));

  return new;
}
#endif

/**********************************************************************
  dstring *dstr_clone()
  synopsis:	Create a new copy of the given dstring.
  parameters:   dstring	*src		The dynamic string.
		const int max_size	The new max_size.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

dstring *dstr_clone(dstring *src)
  {
  dstring *dest=NULL;

  if (!dstr_isvalid(src)) die("Invalid dstring passed.");

  dest = dstr_create(src->size);
  dest->size = src->size;
  strncpy(dest->string, src->string, src->size);
  dest->string[dest->size]='\0';

  return(dest);
  }


#if 0
/*------------------------------------------------------------*/
dstring *
dstr_subclone (dstring *ds, int first, int last)
     /*
       Create a separate copy of a substring of the given dstring.
     */
{
  dstring *new = s_malloc (sizeof (dstring));

  /* pre */
  s_assert (ds);
  s_assert (dstr_isvalid (ds));
  s_assert (first >= 0);
  s_assert (last < ds->size);
  s_assert (first <= last);

  new->size = last - first + 1;
  dstr_allocate (new, new->size);
  strncpy (new->string, ds->string + first, new->size);
  new->string[new->size] = '\0';

  s_assert (dstr_isvalid (new));

  return new;
}
#endif


/**********************************************************************
  void dstr_set()
  synopsis:	Copy the given string into the dstring.
  parameters:   dstring	*ds	The dynamic string.
		const char *str	The string.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

void dstr_set(dstring *ds, const char *str)
  {
  if (!dstr_isvalid(ds)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char passed.");

  ds->size = strlen(str);
  dstr_realloc(ds, ds->size);
  strcpy(ds->string, str);
  }


/**********************************************************************
  void dstr_setn()
  synopsis:	Copies number of characters from the given string into
		the dstring.  Ensures null-termination.
  parameters:   dstring	*ds	The dynamic string.
		const int len	Length of string.
		const char *str	The string.
  return:	none
  last updated: 09/03/99
 **********************************************************************/

void dstr_setn(dstring *ds, const int len, const char *str)
  {

#if DSTR_DEBUG>2
  printf("DEBUG: len = %d, str = \"%s\", ds->size = %d, ds->max_size = %d.\n", len, str, ds->size, ds->max_size);
#endif

  if (!dstr_isvalid(ds)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char passed.");
  if (len < 1) die("Stupid length passed.");

  ds->size = len;
  dstr_realloc(ds, len);
  strncpy(ds->string, str, len);
  ds->string[len] = '\0';

#if DSTR_DEBUG>2
  printf("DEBUG: dstr set \"%s\"\n", ds->string);
#endif

  return;
  }


/**********************************************************************
  void dstr_setf()
  synopsis:	Copy the given string into the dstring.
  parameters:   dstring	*ds	The dynamic string.
		const char *str	The string.
  return:	none
  last updated: 11/01/00
 **********************************************************************/

void dstr_setf(dstring *ds, const char *format, ...)
  {
  va_list	args;		/* Variable argument list */
  int		size;		/* Returned from vsnprintf() */

  if (!dstr_isvalid(ds)) die("Invalid dstring passed.");
  if (!format) die("Null pointer to format string passed.");

/*
 * We must be careful because the return value of snprintf()
 * and vsnprintf() changed between glibc 2.0.6. and glibc 2.1
 */
  while (TRUE)
    {
    /* Try to print in the current space */
    va_start(args, format);
    size = vsnprintf(ds->string, ds->max_size, format, args);
    va_end(args);

    /* Did that work? */
    if (size > -1 && size < ds->max_size)
      {
      ds->size = size;
      return;
      }

    /* No, so try again with more space. */
    if (size > -1)
      ds->max_size = size+1;	/* We know how much is needed */
    else
      ds->max_size *= 2;	/* We don't, so guess: twice the old size */

/* We can avoid copying the memory contents
    if ( !(ds->string = s_realloc(ds->string, (ds->max_size+1)*sizeof(char))) )
      die("Unable to reallocate memory.");
*/
    s_free(ds->string);
    if ( !(ds->string = s_malloc((ds->max_size+1)*sizeof(char))) )
      die("Unable to allocate memory.");
    }

/* Should never get here. */
  }


/**********************************************************************
  void dstr_subset()
  synopsis:	Copy a portion of the given string into the dstring.
  parameters:   dstring	*ds	The dynamic string.
		const char *str	The string.
		const int first
		const int last
  return:	none
  last updated: 09/03/99
 **********************************************************************/

void dstr_subset(dstring *ds, const char *str, const int first, const int last)
  {
  if (!dstr_isvalid(ds)) die("Invalid dstring passed.");
  if (!str) die("Null pointer to char passed.");
  s_assert (first >= 0);
  s_assert (last < strlen (str));
  s_assert (first <= last);

  ds->size = last - first + 1;
  dstr_realloc(ds, ds->size);
  strncpy(ds->string, str + first, ds->size);
  ds->string[ds->size] = '\0';
  }


/**********************************************************************
  void dstr_append_str()
  synopsis:	Copy a string onto the end of a dstring.
  parameters:   dstring	*ds	The dynamic string.
		const char *str	The string.
  return:	none
  last updated: 14/09/99
 **********************************************************************/

void dstr_append_str(dstring *ds, const char *str)
  {
  int size;

  s_assert(dstr_isvalid(ds));
  s_assert(str);
  s_assert(ds->string != str);

  size = strlen(str);
  dstr_realloc(ds, ds->size + size);
  strcpy(ds->string + ds->size, str);
  ds->size += size;

  s_assert(dstr_isvalid(ds));
  }


/**********************************************************************
  void dstr_append_str()
  synopsis:	Concatenate the given substring to the dstring.
  parameters:   dstring	*ds	The dynamic string.
		const char *str	The string.
		const int first	Beginning of substring.
		const int last	End of substring.
  return:	none
  last updated: 14/09/99
 **********************************************************************/

void dstr_subcat(dstring *ds, const char *str, int first, int last)
  {
  int	size;	/* Original length of dstring */

  /* Initial Checks */
  s_assert(ds);
  s_assert(dstr_isvalid(ds));
  s_assert(str);
  s_assert(first >= 0);
  s_assert(last < strlen(str));
  s_assert(first <= last);

  size = ds->size;
  ds->size += last - first + 1;
  dstr_realloc(ds, ds->size);
  strncpy(ds->string + size, str + first, last - first + 1);
  ds->string[ds->size] = '\0';

  s_assert(dstr_isvalid(ds));
  }


/*------------------------------------------------------------*/
void dstr_append_char(dstring *ds, const char c)
     /*
       Add the character to the dstring.
     */
  {
  s_assert(dstr_isvalid(ds));
  s_assert(c);

  dstr_realloc(ds, ds->size + 1);
  ds->string[ds->size] = c;
  ds->size++;
  ds->string[ds->size] = '\0';

  s_assert(dstr_isvalid(ds));
  }


/*------------------------------------------------------------*/
void dstr_append_int(dstring *ds, int i)
     /*
       Add the decimal string representation of the integer to the dstring.
     */
  {
  char str[40];

  s_assert(dstr_isvalid(ds));

  sprintf(str, "%i", i);
  dstr_append_str(ds, str);
  }


/*------------------------------------------------------------*/
void dstr_append(dstring *ds1, const dstring *ds2)
     /*
       Copy the contents of second dstring to the end of the first dstring.
     */
  {
  s_assert(dstr_isvalid(ds1));
  s_assert(dstr_isvalid(ds2));

  dstr_realloc(ds1, ds1->size + ds2->size);
  strcpy(ds1->string + ds1->size, ds2->string);
  ds1->size += ds2->size;

  s_assert(dstr_isvalid(ds1));
  }


/*------------------------------------------------------------*/
void
dstr_replace (dstring *ds, const int pos, const int len, const char *str)
     /*
       Replace the contents of the given substring in the dstring with
       the given string.
     */
{
  int len2, size, swap;

  /* pre */
  s_assert (ds);
  s_assert (dstr_isvalid (ds));
  s_assert (pos >= 0);
  s_assert (len >= 0);
  s_assert (pos + len <= ds->size);

  len2 = (str == NULL) ? 0 : strlen (str);
  size = ds->size + len2 - len;
  dstr_realloc (ds, size);
  swap = ds->size - (pos + len) + 1;
  if (swap > 0) memmove (ds->string + pos + len2, ds->string + pos + len, swap);
  if (len2 > 0) memcpy (ds->string + pos, str, len2);
  ds->string[size] = '\0';
  ds->size = size;

  s_assert (dstr_isvalid (ds));
}


/*------------------------------------------------------------*/
void
dstr_fill_blanks (dstring *ds, const int size)
     /*
       Fill the dstring with the given number of blanks.
     */
{
  /* pre */
  s_assert (ds);
  s_assert (dstr_isvalid (ds));
  s_assert (size >= 0);

  dstr_realloc (ds, size);
  memset (ds->string, ' ', size);
  ds->string[size] = '\0';
  ds->size = size;

  s_assert (dstr_isvalid (ds));
}


/*------------------------------------------------------------*/
/*
 * Remove all blanks from the dstring.
 */
void dstr_remove_blanks(dstring *ds)
  {
  char *pos, *last;

  /* pre */
  s_assert(ds);
  s_assert(dstr_isvalid(ds));

/*
printf("Initial: \"%s\"\n", ds->string);
*/

  pos = ds->string;
  last = pos;
  while (*pos)
    {
    if (*pos != ' ') *last++ = *pos;
    pos++;
    }
  *last = '\0';
  ds->size = last - ds->string;

  s_assert(dstr_isvalid(ds));

/*
printf("Final: \"%s\"\n", ds->string);
*/

  return;
  }


/*------------------------------------------------------------*/
void
dstr_pad_blanks (dstring *ds, int size)
     /*
       Pad the dstring to the right with blanks up to the given total
       size of the string.
     */
{
  /* pre */
  s_assert (ds);
  s_assert (dstr_isvalid (ds));
  s_assert (size >= 0);

  if (ds->size >= size) return;
  dstr_realloc (ds, size);
  memset (ds->string + ds->size, ' ', size - ds->size);
  ds->string[size] = '\0';
  ds->size = size;

  s_assert (dstr_isvalid (ds));
}



/*------------------------------------------------------------*/
int dstr_left_adjust(dstring *ds)
     /*
       Left-adjust the dstring, i.e. remove leading blanks.
	Returns the number of spaces removed.
     */
  {
  char	*c;		/* Pointer to start of new string */
  int	count=0;	/* Number of spaces removed */

  s_assert(dstr_isvalid(ds));

  c = ds->string;
  while(*c && (*c == ' '))
    {
    c++;
    count++;
    }
  ds->size -= count;
  memmove(ds->string, c, ds->size + 1);

  s_assert(dstr_isvalid(ds));

  return(count);
  }


/*------------------------------------------------------------*/
int dstr_right_adjust(dstring *ds)
     /*
       Right-adjust the dstring, i.e. remove trailing blanks.
	Returns the number of spaces removed.
     */
  {
  char	*pos;
  int	count=0;	/* Number of spaces removed */

  s_assert(dstr_isvalid(ds));

  pos = ds->string + ds->size - 1;
  while (pos >= ds->string)
    {
    if (isspace((int)*pos))
      {
      pos--;
      count++;
      }
    else
      {
      pos[1] = '\0';
      ds->size = pos - ds->string + 1;

      s_assert(dstr_isvalid(ds));
      return(count);
      }
    }
  ds->string[0] = '\0';
  ds->size = 0;

  s_assert(dstr_isvalid(ds));
  return(count);
  }


/**********************************************************************
  void dstr_to_upper()
  synopsis:	Convert each character in the dstring to upper case.
  parameters:   dstring	*ds	The dynamic string.
  return:	none
  last updated: 20/10/99
 **********************************************************************/

void dstr_to_upper(dstring *ds)
  {
  char *c;

  s_assert(dstr_isvalid(ds));

  for (c = ds->string; *c; c++) *c = toupper(*c);
  }


/**********************************************************************
  void dstr_to_lower()
  synopsis:	Convert each character in the dstring to lower case.
  parameters:   dstring	*ds	The dynamic string.
  return:	none
  last updated: 20/10/99
 **********************************************************************/

void dstr_to_lower(dstring *ds)
  {
  char *c;

  s_assert(dstr_isvalid(ds));

  for (c = ds->string; *c; c++) *c = tolower(*c);
  }


/**********************************************************************
  boolean dstr_match()
  synopsis:	Match dstring with given pattern dstring.
  parameters:   dstring	*ds	The dynamic string.
		dstring *pat	The pattern.
  return:	TRUE/FALSE
  last updated: 20/10/99
 **********************************************************************/

boolean dstr_match(dstring *ds, dstring *pat)
  {
  s_assert(dstr_isvalid(ds));

  return( recursive_str_match(ds->string, &(ds->string[ds->size]), pat->string, &(pat->string[pat->size])) );
  }


/**********************************************************************
  boolean dstr_match_str()
  synopsis:	Match dstring with given pattern (null terminated char
		array)
  parameters:   dstring	*ds	The dynamic string.
		char *pat	The pattern.
  return:	TRUE/FALSE
  last updated: 20/10/99
 **********************************************************************/

boolean dstr_match_str(dstring *ds, char *pat)
  {
  char	*pat_end;

  s_assert(dstr_isvalid(ds));

  /* Find extent of pattern */
  pat_end = pat;
  while (*pat_end != '\0') pat_end++;

  return( recursive_str_match(ds->string, &(ds->string[ds->size]), pat, pat_end) );
  }


/**********************************************************************
  dstring *dstr_readline(dstring *ds, FILE *fp)
  synopsis:     Reads upto newline/eof from specified stream, into
		a dstring, also
                ensures the string is null-terminated.
		If null pointer to dstr passed, creates new dtring.
  parameters:   FILE    *fp     The input stream.
		dstring	*ds	The destination string.
  return:       dstring	*ds	The destination string.
  last updated: 19/02/00
 **********************************************************************/

dstring *dstr_readline(dstring *ds, FILE *fp)
  {
  char          c;              /* Current character */

  if (!fp) die("null file handle passed.\n");
  if (!ds) ds=dstr_allocate();
  if (!dstr_isvalid(ds)) die("Invalid dstring.\n");

  ds->size=0;

  while((!feof(fp)) && (c=fgetc(fp)) && (c!='\n'))
    {
    dstr_realloc(ds, ds->size + 1);
    ds->string[ds->size] = c;
    ds->size++;
    }

  ds->string[ds->size] = '\0';

  return(ds);
  }


/**********************************************************************
  dstr_countchar()
  synopsis:	Count all occurances of a given character in a given
		dstring.
  parameters:	dstring	*ds	The dynamic string
		char	c	The character
  return:       int	count	Number of occurances.
  last updated:	11/04/00
 **********************************************************************/

int dstr_countchar(dstring *ds, char c)
  {
  s_assert(dstr_isvalid(ds));

  return ( str_countchar(ds->string, c) );
  }


/**********************************************************************
  dstr_truncate()
  synopsis:	Truncate the dstring at the given size.  If the
		dstring is already shorter then do nothing.
  parameters:	dstring	*ds	The dynamic string
		int	len	The string length
  return:	none
  last updated:	09/03/99
 **********************************************************************/

void dstr_truncate(dstring *ds, int size)
  {
  s_assert(dstr_isvalid(ds));
  s_assert(size >= 0);

  if (size >= ds->size) return;

  ds->string[size] = '\0';
  ds->size = size;

  s_assert(dstr_isvalid(ds));
  }


