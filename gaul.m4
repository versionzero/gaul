# m4 macros for Helga configuration.

dnl HELGA_MEMORY_ALIGN
AC_DEFUN(HELGA_MEMORY_ALIGN_SIZE
[ AC_MSG_CHECKING(memory alignment)
AC_CACHE_VAL(HELGA_MEMORY_ALIGN_SIZE_VAL,
[AC_TRY_RUN([#include <stdio.h>
main()
{
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%d\n", sizeof(long)>sizeof(void *)?sizeof(long),sizeof(void *));
  exit(0);
}], HELGA_MEMORY_ALIGN_SIZE_VAL=`cat conftestval`, HELGA_MEMORY_ALIGN_SIZE_VAL=0, HELGA_MEMORY_ALIGN_SIZE_VAL=8])
AC_MSG_RESULT($HELGA_MEMORY_ALIGN_SIZE_VAL)
AC_DEFINE_UNQUOTED(MEMORY_ALIGN_SIZE, $HELGA_MEMORY_ALIGN_SIZE_VAL)
undefine([HELGA_MEMORY_ALIGN_SIZE_VAL])
])


