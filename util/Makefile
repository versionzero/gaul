# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

#######################################################################
# util Makefile.am
#######################################################################
#
# util - General collection of C-programming utitily routines.
# Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
#
# The latest version of this program should be available at:
# http://www.stewart-adcock.co.uk/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.  Alternatively, if your project
# is incompatible with the GPL, I will probably agree to requests
# for permission to use the terms of any other license.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY WHATSOEVER.
#
# A full copy of the GNU General Public License should be in the file
# "COPYING" provided with this distribution; if not, see:
# http://www.gnu.org/
#
#######################################################################
#

# require automake 1.4


SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/gaul-devel
pkglibdir = $(libdir)/gaul-devel
pkgincludedir = $(includedir)/gaul-devel

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_alias = 
build_triplet = i686-pc-linux-gnu
host_alias = 
host_triplet = i686-pc-linux-gnu
target_alias = 
target_triplet = i686-pc-linux-gnu
AS = @AS@
CC = icc
DLLTOOL = @DLLTOOL@
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LT_CURRENT = 0
LT_RELEASE = 0.1839
LT_REVISION = 0
MAINT = #
MAJOR_VERSION = 0
MAKEINFO = /u1/gaul-devel/missing makeinfo
MINOR_VERSION = 1839
MPICC = echo
MPIFLAGS = > /dev/null
OBJDUMP = @OBJDUMP@
PACKAGE = gaul-devel
PATCH_VERSION = 0
RANLIB = ranlib
VERSION = 0.1839-0

AUTOMAKE_OPTIONS = 1.4

lib_LIBRARIES = libstuff.a \
	libnn_util.a librandom.a libstr_util.a libtimer_util.a \
        liblog_util.a liblog_util_parallel.a \
	libmpi_util.a libmpi_util_fake.a


lib_LTLIBRARIES = libstuff.la \
	libnn_util.la librandom.la libstr_util.la libtimer_util.la \
        liblog_util.la liblog_util_parallel.la \
	libmpi_util.la libmpi_util_fake.la


bin_PROGRAMS = nn

nn_SOURCES = \
	nn_main.c


libstuff_a_SOURCES = \
	avltree.c \
	compatibility.c \
	linkedlist.c \
	memory_util.c \
	table_util.c


libstuff_la_SOURCES = \
	avltree.c \
	compatibility.c \
	linkedlist.c \
	memory_util.c \
	table_util.c


liblog_util_parallel_a_SOURCES = \
	log_util_parallel.c


liblog_util_parallel_la_SOURCES = \
	log_util_parallel.c


liblog_util_a_SOURCES = \
	log_util.c


liblog_util_la_SOURCES = \
	log_util.c


libmpi_util_fake_a_SOURCES = \
	mpi_util_fake.c 


libmpi_util_fake_la_SOURCES = \
	mpi_util_fake.c 


libmpi_util_a_SOURCES = \
	mpi_util.c 


libmpi_util_la_SOURCES = \
	mpi_util.c 


libnn_util_a_SOURCES = \
	nn_util.c


libnn_util_la_SOURCES = \
	nn_util.c


librandom_a_SOURCES = \
	random_util.c


librandom_la_SOURCES = \
	random_util.c


libstr_util_a_SOURCES = \
	str_util.c \
	dstr_util.c \
	str_match.c


libstr_util_la_SOURCES = \
	str_util.c \
	dstr_util.c \
	str_match.c


libtimer_util_a_SOURCES = \
	timer_util.c


libtimer_util_la_SOURCES = \
	timer_util.c


include_HEADERS = \
     SAA_header.h \
     avltree.h \
     compatibility.h \
     dstr_util.h \
     linkedlist.h \
     log_util.h \
     memory_chunks.h \
     memory_util.h \
     mpi_util.h \
     nn_util.h \
     random_util.h \
     str_match.h \
     str_util.h \
     table_util.h \
     memory_chunks.c \
     timer_util.h


nn_LDADD = -L./ -L../src/ -lnn_util -lmpi_util_fake -lstr_util -lstuff -llog_util -lrandom -lm

libstuff_a_LIBFLAGS = -lm
libstuff_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

libnn_util_a_LIBFLAGS = 
libnn_util_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

liblog_util_a_LIBFLAGS = 
liblog_util_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

liblog_util_parallel_a_LIBFLAGS = 
liblog_util_parallel_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

libmpi_util_a_LIBFLAGS = 
libmpi_util_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

libmpi_util_fake_a_LIBFLAGS = 
libmpi_util_fake_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

librandom_a_LIBFLAGS = -lm
librandom_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

libstr_util_a_LIBFLAGS = 
libstr_util_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0

libtimer_util_a_LIBFLAGS = 
libtimer_util_la_LDFLAGS = -version-info ${MAJOR_VERSION}:${MINOR_VERSION}:0
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(lib_LIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS =  -lslang -lslang
libstuff_a_LIBADD = 
libstuff_a_OBJECTS =  avltree.o compatibility.o linkedlist.o \
memory_util.o table_util.o
libnn_util_a_LIBADD = 
libnn_util_a_OBJECTS =  nn_util.o
librandom_a_LIBADD = 
librandom_a_OBJECTS =  random_util.o
libstr_util_a_LIBADD = 
libstr_util_a_OBJECTS =  str_util.o dstr_util.o str_match.o
libtimer_util_a_LIBADD = 
libtimer_util_a_OBJECTS =  timer_util.o
liblog_util_a_LIBADD = 
liblog_util_a_OBJECTS =  log_util.o
liblog_util_parallel_a_LIBADD = 
liblog_util_parallel_a_OBJECTS =  log_util_parallel.o
libmpi_util_a_LIBADD = 
libmpi_util_a_OBJECTS =  mpi_util.o
libmpi_util_fake_a_LIBADD = 
libmpi_util_fake_a_OBJECTS =  mpi_util_fake.o
AR = ar
LTLIBRARIES =  $(lib_LTLIBRARIES)

libstuff_la_LIBADD = 
libstuff_la_OBJECTS =  avltree.lo compatibility.lo linkedlist.lo \
memory_util.lo table_util.lo
libnn_util_la_LIBADD = 
libnn_util_la_OBJECTS =  nn_util.lo
librandom_la_LIBADD = 
librandom_la_OBJECTS =  random_util.lo
libstr_util_la_LIBADD = 
libstr_util_la_OBJECTS =  str_util.lo dstr_util.lo str_match.lo
libtimer_util_la_LIBADD = 
libtimer_util_la_OBJECTS =  timer_util.lo
liblog_util_la_LIBADD = 
liblog_util_la_OBJECTS =  log_util.lo
liblog_util_parallel_la_LIBADD = 
liblog_util_parallel_la_OBJECTS =  log_util_parallel.lo
libmpi_util_la_LIBADD = 
libmpi_util_la_OBJECTS =  mpi_util.lo
libmpi_util_fake_la_LIBADD = 
libmpi_util_fake_la_OBJECTS =  mpi_util_fake.lo
PROGRAMS =  $(bin_PROGRAMS)

nn_OBJECTS =  nn_main.o
nn_DEPENDENCIES = 
nn_LDFLAGS = 
CFLAGS = -g -I/usr/include/slang/ -tpp6 -w1 -O2 -I../util/ -I../src/
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(include_HEADERS)

DIST_COMMON =  README Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
DEP_FILES =  .deps/avltree.P .deps/compatibility.P .deps/dstr_util.P \
.deps/linkedlist.P .deps/log_util.P .deps/log_util_parallel.P \
.deps/memory_util.P .deps/mpi_util.P .deps/mpi_util_fake.P \
.deps/nn_main.P .deps/nn_util.P .deps/random_util.P .deps/str_match.P \
.deps/str_util.P .deps/table_util.P .deps/timer_util.P
SOURCES = $(libstuff_a_SOURCES) $(libnn_util_a_SOURCES) $(librandom_a_SOURCES) $(libstr_util_a_SOURCES) $(libtimer_util_a_SOURCES) $(liblog_util_a_SOURCES) $(liblog_util_parallel_a_SOURCES) $(libmpi_util_a_SOURCES) $(libmpi_util_fake_a_SOURCES) $(libstuff_la_SOURCES) $(libnn_util_la_SOURCES) $(librandom_la_SOURCES) $(libstr_util_la_SOURCES) $(libtimer_util_la_SOURCES) $(liblog_util_la_SOURCES) $(liblog_util_parallel_la_SOURCES) $(libmpi_util_la_SOURCES) $(libmpi_util_fake_la_SOURCES) $(nn_SOURCES)
OBJECTS = $(libstuff_a_OBJECTS) $(libnn_util_a_OBJECTS) $(librandom_a_OBJECTS) $(libstr_util_a_OBJECTS) $(libtimer_util_a_OBJECTS) $(liblog_util_a_OBJECTS) $(liblog_util_parallel_a_OBJECTS) $(libmpi_util_a_OBJECTS) $(libmpi_util_fake_a_OBJECTS) $(libstuff_la_OBJECTS) $(libnn_util_la_OBJECTS) $(librandom_la_OBJECTS) $(libstr_util_la_OBJECTS) $(libtimer_util_la_OBJECTS) $(liblog_util_la_OBJECTS) $(liblog_util_parallel_la_OBJECTS) $(libmpi_util_la_OBJECTS) $(libmpi_util_fake_la_OBJECTS) $(nn_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu util/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLIBRARIES:

clean-libLIBRARIES:
	-test -z "$(lib_LIBRARIES)" || rm -f $(lib_LIBRARIES)

distclean-libLIBRARIES:

maintainer-clean-libLIBRARIES:

install-libLIBRARIES: $(lib_LIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done
	@$(POST_INSTALL)
	@list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(RANLIB) $(DESTDIR)$(libdir)/$$p"; \
	    $(RANLIB) $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LIBRARIES)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(libdir)/$$p; \
	done

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libstuff.a: $(libstuff_a_OBJECTS) $(libstuff_a_DEPENDENCIES)
	-rm -f libstuff.a
	$(AR) cru libstuff.a $(libstuff_a_OBJECTS) $(libstuff_a_LIBADD)
	$(RANLIB) libstuff.a

libnn_util.a: $(libnn_util_a_OBJECTS) $(libnn_util_a_DEPENDENCIES)
	-rm -f libnn_util.a
	$(AR) cru libnn_util.a $(libnn_util_a_OBJECTS) $(libnn_util_a_LIBADD)
	$(RANLIB) libnn_util.a

librandom.a: $(librandom_a_OBJECTS) $(librandom_a_DEPENDENCIES)
	-rm -f librandom.a
	$(AR) cru librandom.a $(librandom_a_OBJECTS) $(librandom_a_LIBADD)
	$(RANLIB) librandom.a

libstr_util.a: $(libstr_util_a_OBJECTS) $(libstr_util_a_DEPENDENCIES)
	-rm -f libstr_util.a
	$(AR) cru libstr_util.a $(libstr_util_a_OBJECTS) $(libstr_util_a_LIBADD)
	$(RANLIB) libstr_util.a

libtimer_util.a: $(libtimer_util_a_OBJECTS) $(libtimer_util_a_DEPENDENCIES)
	-rm -f libtimer_util.a
	$(AR) cru libtimer_util.a $(libtimer_util_a_OBJECTS) $(libtimer_util_a_LIBADD)
	$(RANLIB) libtimer_util.a

liblog_util.a: $(liblog_util_a_OBJECTS) $(liblog_util_a_DEPENDENCIES)
	-rm -f liblog_util.a
	$(AR) cru liblog_util.a $(liblog_util_a_OBJECTS) $(liblog_util_a_LIBADD)
	$(RANLIB) liblog_util.a

liblog_util_parallel.a: $(liblog_util_parallel_a_OBJECTS) $(liblog_util_parallel_a_DEPENDENCIES)
	-rm -f liblog_util_parallel.a
	$(AR) cru liblog_util_parallel.a $(liblog_util_parallel_a_OBJECTS) $(liblog_util_parallel_a_LIBADD)
	$(RANLIB) liblog_util_parallel.a

libmpi_util.a: $(libmpi_util_a_OBJECTS) $(libmpi_util_a_DEPENDENCIES)
	-rm -f libmpi_util.a
	$(AR) cru libmpi_util.a $(libmpi_util_a_OBJECTS) $(libmpi_util_a_LIBADD)
	$(RANLIB) libmpi_util.a

libmpi_util_fake.a: $(libmpi_util_fake_a_OBJECTS) $(libmpi_util_fake_a_DEPENDENCIES)
	-rm -f libmpi_util_fake.a
	$(AR) cru libmpi_util_fake.a $(libmpi_util_fake_a_OBJECTS) $(libmpi_util_fake_a_LIBADD)
	$(RANLIB) libmpi_util_fake.a

mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

libstuff.la: $(libstuff_la_OBJECTS) $(libstuff_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libstuff_la_LDFLAGS) $(libstuff_la_OBJECTS) $(libstuff_la_LIBADD) $(LIBS)

libnn_util.la: $(libnn_util_la_OBJECTS) $(libnn_util_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libnn_util_la_LDFLAGS) $(libnn_util_la_OBJECTS) $(libnn_util_la_LIBADD) $(LIBS)

librandom.la: $(librandom_la_OBJECTS) $(librandom_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(librandom_la_LDFLAGS) $(librandom_la_OBJECTS) $(librandom_la_LIBADD) $(LIBS)

libstr_util.la: $(libstr_util_la_OBJECTS) $(libstr_util_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libstr_util_la_LDFLAGS) $(libstr_util_la_OBJECTS) $(libstr_util_la_LIBADD) $(LIBS)

libtimer_util.la: $(libtimer_util_la_OBJECTS) $(libtimer_util_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libtimer_util_la_LDFLAGS) $(libtimer_util_la_OBJECTS) $(libtimer_util_la_LIBADD) $(LIBS)

liblog_util.la: $(liblog_util_la_OBJECTS) $(liblog_util_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(liblog_util_la_LDFLAGS) $(liblog_util_la_OBJECTS) $(liblog_util_la_LIBADD) $(LIBS)

liblog_util_parallel.la: $(liblog_util_parallel_la_OBJECTS) $(liblog_util_parallel_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(liblog_util_parallel_la_LDFLAGS) $(liblog_util_parallel_la_OBJECTS) $(liblog_util_parallel_la_LIBADD) $(LIBS)

libmpi_util.la: $(libmpi_util_la_OBJECTS) $(libmpi_util_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libmpi_util_la_LDFLAGS) $(libmpi_util_la_OBJECTS) $(libmpi_util_la_LIBADD) $(LIBS)

libmpi_util_fake.la: $(libmpi_util_fake_la_OBJECTS) $(libmpi_util_fake_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libmpi_util_fake_la_LDFLAGS) $(libmpi_util_fake_la_OBJECTS) $(libmpi_util_fake_la_LIBADD) $(LIBS)

mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done

nn: $(nn_OBJECTS) $(nn_DEPENDENCIES)
	@rm -f nn
	$(LINK) $(nn_LDFLAGS) $(nn_OBJECTS) $(nn_LDADD) $(LIBS)

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = util

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu util/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

-include $(DEP_FILES)

mostlyclean-depend:

clean-depend:

distclean-depend:
	-rm -rf .deps

maintainer-clean-depend:

%.o: %.c
	@echo '$(COMPILE) -c $<'; \
	$(COMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.c
	@echo '$(LTCOMPILE) -c $<'; \
	$(LTCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-libLIBRARIES install-libLTLIBRARIES \
		install-binPROGRAMS
install-exec: install-exec-am

install-data-am: install-includeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLIBRARIES uninstall-libLTLIBRARIES \
		uninstall-binPROGRAMS uninstall-includeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LIBRARIES) $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(libdir) \
		$(DESTDIR)$(bindir) $(DESTDIR)$(includedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-libLTLIBRARIES \
		mostlyclean-binPROGRAMS mostlyclean-tags \
		mostlyclean-depend mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLIBRARIES clean-compile clean-libtool \
		clean-libLTLIBRARIES clean-binPROGRAMS clean-tags \
		clean-depend clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLIBRARIES distclean-compile \
		distclean-libtool distclean-libLTLIBRARIES \
		distclean-binPROGRAMS distclean-tags distclean-depend \
		distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-libLTLIBRARIES \
		maintainer-clean-binPROGRAMS maintainer-clean-tags \
		maintainer-clean-depend maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLIBRARIES distclean-libLIBRARIES \
clean-libLIBRARIES maintainer-clean-libLIBRARIES uninstall-libLIBRARIES \
install-libLIBRARIES mostlyclean-compile distclean-compile \
clean-compile maintainer-clean-compile mostlyclean-libtool \
distclean-libtool clean-libtool maintainer-clean-libtool \
mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-binPROGRAMS \
distclean-binPROGRAMS clean-binPROGRAMS maintainer-clean-binPROGRAMS \
uninstall-binPROGRAMS install-binPROGRAMS uninstall-includeHEADERS \
install-includeHEADERS tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir mostlyclean-depend distclean-depend \
clean-depend maintainer-clean-depend info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
