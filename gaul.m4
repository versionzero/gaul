dnl ***************************************************************
dnl gaul/gaul.m4
dnl ***************************************************************
dnl
dnl GAUL - Genetic Algorithm Utility Library.
dnl Copyright ©2001-2003, Stewart Adcock <stewart@linux-domain.com>
dnl All rights reserved.
dnl
dnl The latest version of this program should be available at:
dnl http://www.stewart-adcock.co.uk/
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.  Alternatively, if your project
dnl is incompatible with the GPL, I will probably agree to requests
dnl for permission to use the terms of any other license.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY WHATSOEVER.
dnl
dnl A full copy of the GNU General Public License should be in the file
dnl "COPYING" provided with this distribution; if not, see:
dnl http://www.gnu.org/
dnl
dnl ***************************************************************
dnl
dnl Some useful m4 macros for GAUL configuration.
dnl
dnl ***************************************************************

dnl GAUL_MEMORY_ALIGN_SIZE
dnl Determine memory alignment characteristics.
AC_DEFUN([GAUL_MEMORY_ALIGN_SIZE],
[ AC_MSG_CHECKING(memory alignment)
AC_CACHE_VAL(MEMORY_ALIGN_SIZE,
[AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
int main(void)
  {
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%d\n", (unsigned int) sizeof(long)>sizeof(void *)?sizeof(long):sizeof(void *));
  exit(0);
  }
], MEM_ALIGN_SIZE=`cat conftestval`, MEM_ALIGN_SIZE=4, MEM_ALIGN_SIZE=8)])
AC_MSG_RESULT(${MEM_ALIGN_SIZE})
AC_DEFINE_UNQUOTED(MEMORY_ALIGN_SIZE, ${MEM_ALIGN_SIZE}, [Memory alignment boundry characteristics.])
])

dnl GAUL_GNUMAKE_WARN
dnl Produce warning message if non-GNU make is detected.
AC_DEFUN([GAUL_GNUMAKE_WARN],
[
temp=`make -version -f \dev\null 2>&1 | grep GNU`
if test "x$temp" = "x"; then
  AC_MSG_WARN([You are advised to use GNU Make when compiling GAUL])
fi
])

dnl GAUL_DETECT_BOOL
dnl Determine native boolean stuff.
dnl 1) Check whether stdbool.h defines bool and true/false correctly.
dnl 2) Check whether compiler has built-in _Bool type.
AC_DEFUN([GAUL_DETECT_BOOL],
[
AC_CACHE_CHECK([for working stdbool.h],
	   gaul_header_stdbool_h,
	   [AC_TRY_COMPILE([#include <stdbool.h>],
	[bool blah = false;],
	gaul_header_stdbool_h="yes", gaul_header_stdbool_h="no")])
if test "x$gaul_header_stdbool_h" = "xyes"; then
  AC_DEFINE(HAVE_STDBOOL_H, 1, [Define if you have a working <stdbool.h> header file.])
fi
AC_CACHE_CHECK([for built-in _Bool type],
	      gaul__bool,
	     [AC_TRY_COMPILE(,
	     [_Bool blah = (_Bool)0;],
	     gaul__bool="yes", gaul__bool="no")])
if test "x$gaul__bool" = "xno" && test "x$gaul_header_stdbool_h" = "xyes"; then
  AC_TRY_COMPILE([#include <stdbool.h>],
           [_Bool blah = (_Bool)0;],
           gaul__bool="yes", gaul__bool="no")
fi
if test "x$gaul__bool" = "xyes"; then
  AC_DEFINE(HAVE__BOOL, 1, [Define if compiler has built-in _Bool type.])
fi
])


