/* acconfig.h

 GAUL's default settings.

 */

@TOP@

/* Other stuff */

#undef MEMORY_ALLOC_DEBUG
#undef MEMORY_ALLOC_SAFE
#undef MEMORY_ALIGN_SIZE
#undef MEMORY_PADDING

#undef DEBUG
#undef GA_DEBUG
#undef GA_WRITE_STATS

#undef VERSION
#undef VERSION_STRING
#undef BUILD_DATE_STRING
#undef MAJOR_VERSION
#undef MINOR_VERSION
#undef PATCH_VERSION
#undef UNAME_STRING

#undef AVLTREE_KEY_TYPE
#undef PARALLEL
#undef V3_QUICK
#undef HAVE_SLANG
#undef MPI_NUM_THREADS_ENVVAR_STRING

#undef HAVE_FLOAT_H
#undef HAVE_LIMITS_H
#undef HAVE_POLL
#undef HAVE_PWD_H
#undef HAVE_SYS_PARAM_H
#undef HAVE_SYS_POLL_H
#undef HAVE_SYS_SELECT_H
#undef HAVE_SYS_TIME_H
#undef HAVE_SYS_TIMES_H
#undef HAVE_STRERROR
#undef HAVE_STRSIGNAL
#undef HAVE_UNISTD_H
#undef HAVE_VALUES_H

/* Kludge required for linking to LAM-MPI */
#undef HAVE_ITOA

#undef NO_SYS_ERRLIST

#undef SIZEOF_DOUBLE
#undef SIZEOF_CHAR
#undef SIZEOF_SHORT
#undef SIZEOF_LONG
#undef SIZEOF_INT
#undef SIZEOF_VOID_P

/* #undef PACKAGE */

@BOTTOM@

