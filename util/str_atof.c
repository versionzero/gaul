/**********************************************************************
  str_atof.c
 **********************************************************************

  str_util - String handling functions.
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

  Synopsis:	My fast version of atof().

  Notes:	- Doesn't handle iee-options (NaN...).
		- The precission is a little less for some high exponential numbers (+-1 at 14th place).
		- Doesn't fully handle overflows.
		- Ignorant of locale.
		- Returns 0.0 if overflow or wrong number.

 **********************************************************************/

#include <ctype.h>

static int parse_sign(char **str);
static unsigned int parse_float_number_part(char **str, double *number);
static void parse_decimal_number_part(char **str, double *number);
static int parse_int_number_part(char **str, unsigned int *number);

double str_atof(const char *src)
  {
  int		sign, exp_sign; /* is number negative (+1) or positive (-1) */
  int		length_before_point;
  double	after_point;	/* Number after decimal point and before exp */
  unsigned int	exponent;	/* Exponent value */
  double	exp_log,exp_val;
  char		*ptr;
  double	result;
  int		overflow=0;

  ptr = (char*) src;

/* Skip leading white-space. */
  while (isspace(ptr[0])) ptr++;

  sign = parse_sign(&ptr);
  length_before_point = parse_float_number_part(&ptr, &result);
  if (*ptr == '.')
    {
    ptr++;
    parse_decimal_number_part(&ptr, &after_point);
    result += after_point;
    }
  else if (length_before_point == 0) return 0.0; /* This isn't a number. */

  if (*ptr=='E' || *ptr=='e')
    {
    ptr++;
    exp_sign = parse_sign(&ptr);
    overflow |= parse_int_number_part(&ptr, &exponent);
/*
 * We may have overflowed, but continue parsing string
 * so any more characters of this number are skipped.
 */

    exp_log=10.0;
    exp_val=1.0;
    while(exponent>0)
      {
      if (exponent & 1)
        {
	exp_val *= exp_log;
	exponent--;
        }
      exp_log*=exp_log;
      exponent>>=1;
      }

    if (exp_sign < 0)
      result*=exp_val;
    else
      result/=exp_val;
    }

/*
  if (overflow) return 0.0;

  return (sign>0)?-result:result;
*/
  return overflow?0.0:(sign>0?-result:result);
  }


/*
 * Parse optional + or - character.
 */

static int parse_sign(char **str)
  {
  if (**str == '-')
    {
    (*str)++;
    return 1;
    }

  if (**str == '+') (*str)++;

  return -1;
  }

/*
 * Parses float from string.
 */

static unsigned int parse_float_number_part(char **str, double *number)
  {
  unsigned int length = 0;

  *number = 0;

  while (isdigit(**str))
    {
    (*length)++;
    *number = (*number * 10) + (**str - '0');
    (*str)++;
    }

  return length;
  }

/*
 * Parses decimal int from string.
 */

static void parse_decimal_number_part(char **str, double *number)
  {
  double	exp_log=0.1;

  *number = 0;

  while (isdigit(**str))
    {
    *number += (**str - '0')*exp_log;
    exp_log *= 0.1;
    (*str)++;
    }

  return;
  }

/*
 * Parses exponent int from string.
 */

static int parse_int_number_part(char **str, unsigned int *number)
  {
  *number = 0;

  while (isdigit(**str))
    {
    if (*number >= ((unsigned int) ~0)/10) return 1;	/* Don't overflow */
    *number = (*number * 10) + **str - '0'; 
    (*str)++;
    }

  return 0;
  }

#endif
