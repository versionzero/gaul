# Generated automatically from Makefile.in by configure.
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
# methods Makefile.am
#######################################################################
#
# methods - General collection of c-programming utitily routines.
# Copyright ©2000-2001, Stewart Adcock <stewart@linux-domain.com>
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
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_alias = i686-pc-linux
build_triplet = i686-pc-linux-gnu
host_alias = i686-pc-linux
host_triplet = i686-pc-linux-gnu
target_alias = i686-pc-linux
target_triplet = i686-pc-linux-gnu
AS = @AS@
CC = gcc
DLLTOOL = @DLLTOOL@
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LT_CURRENT = 0
LT_RELEASE = 0.1837
LT_REVISION = 2
MAINT = #
MAJOR_VERSION = 0
MAKEINFO = /u1/gaul-devel/missing makeinfo
MINOR_VERSION = 1837
MPIFLAGS = -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil
OBJDUMP = @OBJDUMP@
PACKAGE = gaul-devel
PATCH_VERSION = 2
RANLIB = ranlib
VERSION = 0.1837

AUTOMAKE_OPTIONS = 1.4

lib_LIBRARIES = libstuff.a \
	libmpi_util.a libnn_util.a librandom.a libstr_util.a libtimer_util.a


libstuff_a_SOURCES = \
	avltree.c \
	compatibility.c \
	log_util.c \
	linkedlist.c \
	memory_util.c \
	table_util.c


libmpi_util_a_SOURCES = \
	mpi_util.c 


libnn_util_a_SOURCES = \
	nn_util.c


librandom_a_SOURCES = \
	random_util.c


libstr_util_a_SOURCES = \
	str_util.c \
	dstr_util.c \
	str_match.c


libtimer_util_a_SOURCES = \
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


libstuff_a_LIBFLAGS = -lm -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil

libnn_util_a_LIBFLAGS = -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil

libmpi_util_a_LIBFLAGS = -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil

librandom_a_LIBFLAGS = -lm -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil

libstr_util_a_LIBFLAGS = -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil

libtimer_util_a_LIBFLAGS = -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(lib_LIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS =  -lslang -lslang
libstuff_a_LIBADD = 
libstuff_a_OBJECTS =  avltree.o compatibility.o log_util.o linkedlist.o \
memory_util.o table_util.o
libmpi_util_a_LIBADD = 
libmpi_util_a_OBJECTS =  mpi_util.o
libnn_util_a_LIBADD = 
libnn_util_a_OBJECTS =  nn_util.o
librandom_a_LIBADD = 
librandom_a_OBJECTS =  random_util.o
libstr_util_a_LIBADD = 
libstr_util_a_OBJECTS =  str_util.o dstr_util.o str_match.o
libtimer_util_a_LIBADD = 
libtimer_util_a_OBJECTS =  timer_util.o
AR = ar
CFLAGS = -g -O2 -I/usr/include/slang/ -Wall -O2 -g -I../util/ -I../src/
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
.deps/linkedlist.P .deps/log_util.P .deps/memory_util.P \
.deps/mpi_util.P .deps/nn_util.P .deps/random_util.P .deps/str_match.P \
.deps/str_util.P .deps/table_util.P .deps/timer_util.P
SOURCES = $(libstuff_a_SOURCES) $(libmpi_util_a_SOURCES) $(libnn_util_a_SOURCES) $(librandom_a_SOURCES) $(libstr_util_a_SOURCES) $(libtimer_util_a_SOURCES)
OBJECTS = $(libstuff_a_OBJECTS) $(libmpi_util_a_OBJECTS) $(libnn_util_a_OBJECTS) $(librandom_a_OBJECTS) $(libstr_util_a_OBJECTS) $(libtimer_util_a_OBJECTS)

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

libmpi_util.a: $(libmpi_util_a_OBJECTS) $(libmpi_util_a_DEPENDENCIES)
	-rm -f libmpi_util.a
	$(AR) cru libmpi_util.a $(libmpi_util_a_OBJECTS) $(libmpi_util_a_LIBADD)
	$(RANLIB) libmpi_util.a

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
install-exec-am: install-libLIBRARIES
install-exec: install-exec-am

install-data-am: install-includeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLIBRARIES uninstall-includeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LIBRARIES) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(includedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags mostlyclean-depend \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLIBRARIES clean-compile clean-libtool clean-tags \
		clean-depend clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-depend \
		distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-depend \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLIBRARIES distclean-libLIBRARIES \
clean-libLIBRARIES maintainer-clean-libLIBRARIES uninstall-libLIBRARIES \
install-libLIBRARIES mostlyclean-compile distclean-compile \
clean-compile maintainer-clean-compile mostlyclean-libtool \
distclean-libtool clean-libtool maintainer-clean-libtool \
uninstall-includeHEADERS install-includeHEADERS tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir \
mostlyclean-depend distclean-depend clean-depend \
maintainer-clean-depend info-am info dvi-am dvi check check-am \
installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
