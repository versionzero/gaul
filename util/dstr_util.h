/********************************************************************
  dstr_util.h
 ********************************************************************

  Header file for my dynamic string library.

  Updated:      09/03/99 SAA	First version

 ********************************************************************/

#ifndef DYNAMIC_STRING_H_INCLUDED
#define DYNAMIC_STRING_H_INCLUDED

#include "SAA_header.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*#include <glib.h>*/

#include "memory_util.h"
#include "str_util.h"
#include "str_match.h"

/* Debugging */
#ifndef DSTR_DEBUG
#define DSTR_DEBUG	2
#endif
#ifndef DSTR_ISVALID_DIE
#define DSTR_ISVALID_DIE
#endif

/* The dynamic string structure */
typedef struct dstring_t
  {
  int	size;
  int	max_size;
  char	*string;
  } dstring;

/* Constants */
#if 0
#define DSTR_MAXSIZE_INITIAL	32
#define DSTR_MAXSIZE_INCREMENT	32
#endif
#define DSTR_MAXSIZE_INITIAL	8
#define DSTR_MAXSIZE_INCREMENT	16

/* Macros */

/*
 * Function prototypes
 */

void	dstr_diagnostics(void);
boolean	dstr_free(dstring *ds);
unsigned int	dstr_hash(constvpointer s);
int	dstr_equal(constvpointer s1, constvpointer s2);
boolean	dstr_isvalid(const dstring *ds);
int	dstr_len(const dstring *ds);
void	dstr_empty(dstring *ds);
dstring	*dstr_create(const int max_size);
dstring	*dstr_allocate(void);
dstring	*dstr_convertstr(char *str);
/*static void dstr_realloc(dstring *ds, const int max_size)*/
void	dstr_reallocate(dstring *ds, const int max_size);
int	dstr_cmp(const dstring *dstr1, const dstring *dstr2);
dstring	*dstr_clone(dstring *src);
void	dstr_set(dstring *ds, const char *str);
void	dstr_setn(dstring *ds, const int len, const char *str);
void	dstr_subset(dstring *ds, const char *str, const int first, const int last);
int	dstr_cmpn(const dstring *dstr1, const dstring *dstr2, const int len);
int	dstr_cmp_str(const dstring *dstr, const char *str);
int	dstr_scmp_str(const dstring *dstr, const char *str);
int	dstr_cmpn_str(const dstring *dstr, const char *str, const int len);
void	dstr_append_str(dstring *ds, const char *str);
void	dstr_append_char(dstring *ds, const char c);
void	dstr_append_int(dstring *ds, int i);
void	dstr_append(dstring *ds1, const dstring *ds2);
void	dstr_remove_blanks(dstring *ds);
int	dstr_left_adjust(dstring *ds);
int	dstr_right_adjust(dstring *ds);
void	dstr_to_upper(dstring *ds);
void	dstr_to_lower(dstring *ds);
dstring	*dstr_readline(dstring *ds, FILE *fp);

#endif

