/**********************************************************************
  log_util.c
 **********************************************************************

  log_util - general logging services.
  Copyright ©2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatiable with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Helga's routines for logging and debug messages.

		These functions can be tested by compiling with
		something like:
		gcc -o testlog log_util.c -DLOG_UTIL_TEST

		These functions are thread-safe.

  Updated:	27/02/01 SAA	gpointer replaced with vpointer and G_LOCK etc. replaced with THREAD_LOCK.
		07/02/01 SAA	Added warning-level messages.  LOG_WARNING is intended for non-fatal errors and so on.
		02/02.01 SAA	Converted from helga_log.c to log_util.c
		03/01/01 SAA	non-GNU reduced version of helga_log() now will call the callback function if defined.  Made thread-safe.
		15/11/00 SAA	Minor tweaks for IRIX systems.  Slight improvements to the namespace.
		11/10/00 SAA	Optional, time/date output facility added.
		21/06/00 SAA	If helga_get_rank() returns -1, don't write rank to log.
		28/05/00 SAA	'enum helga_log_type' used instead of 'int' for log level specification.  helga_log() is now a macro which means that (a) line number and so on can be passed to the real output function, and (b) don't need function call overhead just to decide not to do logging.  Added helga_log_get_level().
		23/05/00 SAA	Now use stdout by default instead of stderr.
		16/05/00 SAA	Added helga_log_set_file().
		08/05/00 SAA	Ability to define a callback function at run-time added.
		07/05/00 SAA	First code.

  To do:	Seperate levels for callback, file, stderr outputs.
		Validate the logging level when set.
		Seperate levels/files/whatever possible for seperate files.
		Option for outputing the time.

 **********************************************************************/

#include "log_util.h"

/*
 * Global variables.
 *
 * NB/ log_filename and log_callback are meaningless on non-GNU C systems.
 */
THREAD_LOCK_DEFINE_STATIC(log_global);
THREAD_LOCK_DEFINE_STATIC(log_callback);

char		*log_filename=NULL;		/* Log filename */
log_func	log_callback=NULL;		/* Callback function for log */
enum log_level_type	log_level=LOG_NONE;	/* Logging level */
boolean		log_date=TRUE;			/* Whether to display date in logs */


/**********************************************************************
  log_init()
  synopsis:	Initialise logging facilities.  If func and fname are
		both NULL, use stdout instead.
  parameters:	int	level	Logging level.
		char	*fname	Filename (I don't think this is safe
				for multi-process apps). Or NULL.
		log_func	func	Callback function. Or NULL.
  return:	none
  last updated:	08/05/00
 **********************************************************************/

void log_init(	enum log_level_type	level,
			char			*fname,
			log_func		func,
			boolean			date)
  {

  THREAD_LOCK(log_global);
  log_level = level;
  log_filename = fname;
  log_date = date;
  THREAD_UNLOCK(log_global);

  THREAD_LOCK(log_callback);
  log_callback = func;
  THREAD_UNLOCK(log_callback);

#if PARALLEL==2
  plog(LOG_VERBOSE, "Log started. (parallel with MPI)");
#else
  plog(LOG_VERBOSE, "Log started.");
#endif
  plog(LOG_DEBUG, "Debug output activated.");

  return;
  }


/**********************************************************************
  log_set_level()
  synopsis:	Adjust logging level.
  parameters:	int	level	New logging level.
  return:	none
  last updated:	07/05/00
 **********************************************************************/

void log_set_level(const enum log_level_type level)
  {
  THREAD_LOCK(log_global);
  log_level = level;
  plog(LOG_VERBOSE, "Log level adjusted to %d.", level);
  THREAD_UNLOCK(log_global);

  return;
  }


/**********************************************************************
  log_get_level()
  synopsis:	Return logging level.
  parameters:	none
  return:	log_level
  last updated:	28/05/00
 **********************************************************************/

#ifdef __GNUC__
inline enum log_level_type log_get_level(void)
#else
enum log_level_type log_get_level(void)
#endif
  {
  return log_level;
  }


/**********************************************************************
  log_set_file()
  synopsis:	Adjust log file level.
  parameters:	int	level	New logging level.
  return:	none
  last updated:	16/05/00
 **********************************************************************/

void log_set_file(const char *fname)
  {
  THREAD_LOCK(log_global);
  log_filename = str_clone(fname);
  plog(LOG_VERBOSE, "Log file adjusted to \"%s\".", fname);
  THREAD_UNLOCK(log_global);

  return;
  }


/**********************************************************************
  log_output()
  synopsis:	If log level is appropriate, append message to log
		file.  log_init() should really be called prior
		to the first use of the function, although nothing will
		break if you don't.
  parameters:	int	level	Logging level.
		char		format	Format string.
		...		Variable args.
  return:       none
  last updated: 21/06/00
 **********************************************************************/

void log_output(	const enum	log_level_type level,
			const char	*func_name,
			const char	*file_name,
			const int	line_num,
			const char	*format, ...)
  {
  va_list	ap;				/* variable args structure */
  char		message[LOG_MAX_LEN];	/* The text to write */
  FILE		*fh;				/* File handle */
/* FIXME: Needs to be more general */
  const char	log_text[7][9] = {"?????: ", "FATAL: ", "WARNING: ",
                                  "",        "",
                                  "FIXME: ", "DEBUG: " };
  time_t	t;				/* Time structure */

  t = time(&t);	/* Less than optimal when no time display is required. */
		/* I tried variations on ctime(time()), withou success. */

/*
 * Should message be dropped?
 * This test is only required if this function was called without the macro
 * wrapper - i.e. this is a non-GNU compiler.
 */
#ifndef __GNUC__
  if (level > log_level) return;
#endif

/* Generate actual message string */
  va_start(ap, format);
  vsnprintf(message, LOG_MAX_LEN, format, ap);
  va_end(ap);

/* Call a callback? */
  THREAD_LOCK(log_callback);
  if (log_callback) log_callback(level, func_name, file_name, line_num, message);
  THREAD_UNLOCK(log_callback);

/* Write to file? */
  THREAD_LOCK(log_global);
  if (log_filename)
    {
    if ( !(fh=fopen(log_filename, "a+")) )
      {
      fprintf(stdout, "FATAL: Unable to open logfile \"%s\" for appending.\n", log_filename);
      abort();	/* FIXME: Find more elegant method */
      }

/* Next few lines could be optimised (in terms of number of 'if' statements) */

#if PARALLEL==2
    fprintf(fh, "%d: %s%s%s%s\n",
             mpi_get_rank(),
             log_date?"":ctime(&t), log_date?"":" - ",
             log_text[level], message);
#else
    fprintf(fh, "%s%s%s%s\n",
             log_date?"":ctime(&t), log_date?"":" - ",
             log_text[level], message);
#endif

    if (level >= LOG_FIXME)
      fprintf(fh, "   in %s(), \"%s\", line %d\n",
                       func_name, file_name, line_num);

/*    fflush(fh);*/
    fclose(fh);
    }
  THREAD_UNLOCK(log_global);

/* Write to stdout? */
  if ( !(log_callback || log_filename) )
    {
#if PARALLEL==2
    if (mpi_get_rank() >= 0)
      fprintf(stdout, "%d: %s%s%s%s\n", mpi_get_rank(),
              log_date?"":ctime(&t), log_date?"":" - ",
              log_text[level], message);
    else
#endif
      fprintf(stdout, "%s%s%s%s\n", 
              log_date?"":ctime(&t), log_date?"":" - ",
              log_text[level], message);

    if (level >= LOG_FIXME)
      fprintf(stdout, "   in %s(), \"%s\", line %d\n",
                       func_name, file_name, line_num);

    /*fprintf(stderr, "%s%s\n", log_text[level], message);*/
    fflush(stdout);
    }

  return;
  }


#ifndef __GNUC__
/*
 * This is a reduced form of the above function for non-GNU systems.
 */
void plog(const enum log_level_type level, const char *format, ...)
  {
  va_list       ap;                             /* variable args structure */
  char          message[LOG_MAX_LEN];     /* The text to write */
  const char    log_text[7][9] = {"?????: ", "FATAL: ", "WARNING: ",
                                  "",        "",
                                  "FIXME: ", "DEBUG: " };
  time_t        t;                              /* Time structure */

  t = time(&t);

/* Call a callback? */
  THREAD_LOCK(log_callback);
  if (log_callback) log_callback(level, "unknown", "unknown", 0, message);
  THREAD_UNLOCK(log_callback);

  if ( (level) <= log_level )
    {
    va_start(ap, format);
    vsnprintf(message, LOG_MAX_LEN, format, ap);
    va_end(ap);

#if PARALLEL==2
    if (mpi_get_rank() >= 0)
      printf( "%d: %s%s%s%s\n", mpi_get_rank(),
              log_date?"":ctime(&t), log_date?"":" - ",
              log_text[level], message );
    else
#endif
      printf( "%s%s%s%s\n",
              log_date?"":ctime(&t), log_date?"":" - ",
              log_text[level], message );
    }
  return;
  }
#endif

/*
 * SLang intrinsic wrappers.
 */
#if HAVE_SLANG==1
void log_wrapper(int *level, char *message)
  {
/*
  unsigned int	num = SLang_Num_Function_Args;
*/
  const char	log_text[7][9] = {"?????: ", "FATAL: ", "WARNING: ",
                                  "",        "",
                                  "FIXME: ", "DEBUG: " };
  time_t	t;				/* Time structure. */

  t = time(&t);

/* Call a callback? */
  THREAD_LOCK(log_callback);
  if (log_callback) log_callback(*level, "[SLang]", "unknown", 0, message);
  THREAD_UNLOCK(log_callback);

  if ( *level <= log_level )
    {
    printf("%s%s%s%s\n",
              log_date?"":ctime(&t), log_date?"":" - ",
              log_text[*level], message );
    }

  return;
  }
#endif	/* HAVE_SLANG==1 */

