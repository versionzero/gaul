/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Overall debugging level. */
#define DEBUG 0

/* Record date of compilation. */
#define GA_BUILD_DATE_STRING "11/09/11"

/* Core GAUL debugging level. */
#define GA_DEBUG 0

/* Default number of processes. */
#define GA_DEFAULT_NUM_PROCESSES 8

/* Default number of threads. */
#define GA_DEFAULT_NUM_THREADS 4

/* Minor version number for libtool. */
#define GA_LT_MINOR_VERSION 51

/* Major version number. */
#define GA_MAJOR_VERSION 0

/* Minor version number. */
#define GA_MINOR_VERSION 1851

/* Environment variable used to request processes. */
#define GA_NUM_PROCESSES_ENVVAR_STRING "GA_NUM_PROCESSES"

/* Environment variable used to request threads. */
#define GA_NUM_THREADS_ENVVAR_STRING "GA_NUM_THREADS"

/* Patch level. */
#define GA_PATCH_VERSION 0

/* Record compilation platform information. */
#define GA_UNAME_STRING "Darwin homer.local 11.1.0 Darwin Kernel Version 11.1.0: Tue Jul 26 16:07:11 PDT 2011; root:xnu-1699.22.81~1/RELEASE_X86_64 x86_64"

/* Version string. */
#define GA_VERSION_STRING "0.1851-0"

/* Whether to dump certian statistics. */
#define GA_WRITE_STATS 0

/* Whether bcmp() is available */
#define HAVE_BCMP 1

/* Whether bcopy() is available */
#define HAVE_BCOPY 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `floor' function. */
#define HAVE_FLOOR 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the `index' function. */
#define HAVE_INDEX 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
/* #undef HAVE_LIBM */

/* Define to 1 if you have the `mpi' library (-lmpi). */
/* #undef HAVE_LIBMPI */

/* Define to 1 if you have the `mpich' library (-lmpich). */
/* #undef HAVE_LIBMPICH */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the `max' function. */
/* #undef HAVE_MAX */

/* Define to 1 if you have the `memcmp' function. */
#define HAVE_MEMCMP 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Whether memset() is available */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `min' function. */
/* #undef HAVE_MIN */

/* Whether to use MPI for parallel code. */
/* #undef HAVE_MPI */

/* Define to 1 if you have the `pow' function. */
#define HAVE_POW 1

/* Whether libpthread is installed. */
#define HAVE_PTHREADS 1

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `sincos' function. */
/* #undef HAVE_SINCOS */

/* Whether S-Lang is installed. */
/* #undef HAVE_SLANG */

/* Define to 1 if you have the `snooze' function. */
/* #undef HAVE_SNOOZE */

/* Define to 1 if you have the `sqrt' function. */
#define HAVE_SQRT 1

/* Define if you have a working <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strcmp' function. */
#define HAVE_STRCMP 1

/* Define to 1 if you have the `strcpy' function. */
#define HAVE_STRCPY 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Whether strlen() is available */
#define HAVE_STRLEN 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Whether strncmp() is available */
#define HAVE_STRNCMP 1

/* Whether strncpy() is available */
#define HAVE_STRNCPY 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strpbrk' function. */
#define HAVE_STRPBRK 1

/* Define to 1 if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/signal.h> header file. */
#define HAVE_SYS_SIGNAL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Whether usleep() is available */
#define HAVE_USLEEP 1

/* Define to 1 if you have the <values.h> header file. */
/* #undef HAVE_VALUES_H */

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define if compiler has built-in _Bool type. */
#define HAVE__BOOL 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Memory alignment boundry characteristics. */
#define MEMORY_ALIGN_SIZE 8

/* Whether to use debugging memory allocation routines. */
/* #undef MEMORY_ALLOC_DEBUG */

/* Whether to use safe memory allocation routines. */
#define MEMORY_ALLOC_SAFE 1

/* Define to disable memory chunk allocation routines. */
/* #undef MEMORY_CHUNKS_MIMIC */

/* Whether to pad memory. */
/* #undef MEMORY_PADDING */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "gaul@linux-domain.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "gaul-devel"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "gaul-devel 0.1851-0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "gaul-devel"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1851-0"

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if using Sun's Forte C Compiler */
#define SUN_FORTE_C 0

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Whether to use memory chunk handling for chromosomes. */
/* #undef USE_CHROMO_CHUNKS */

/* Whether OpenMP should be used. */
/* #undef USE_OPENMP */

/* Whether to cripple GAUL so it works on windows. */
/* #undef W32_CRIPPLED */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */
