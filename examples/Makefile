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
# gaul/examples/Makefile.am
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
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
LT_RELEASE = 0.1836
LT_REVISION = 1
MAINT = #
MAJOR_VERSION = 0
MAKEINFO = /net/home/adcock/Code_development/gaul-devel/missing makeinfo
MINOR_VERSION = 1836
OBJDUMP = @OBJDUMP@
PACKAGE = gaul-devel
PATCH_VERSION = 1
RANLIB = ranlib
VERSION = 0.1836

AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = diagnostics \
	pingpong wildfire \
	goldberg1 goldberg2 \
	royalroad royalroad_ss \
	struggle struggle2 struggle3 struggle4 struggle5 struggle5_mp struggle_ss \
	nnevolve


diagnostics_SOURCES = diagnostics.c
pingpong_SOURCES = pingpong.c
wildfire_SOURCES = wildfire.c
goldberg1_SOURCES = goldberg1.c
goldberg2_SOURCES = goldberg2.c
royalroad_SOURCES = royalroad.c
royalroad_ss_SOURCES = royalroad_ss.c
struggle_SOURCES = struggle.c
struggle2_SOURCES = struggle2.c
struggle3_SOURCES = struggle3.c
struggle4_SOURCES = struggle4.c
struggle5_SOURCES = struggle5.c
struggle5_mp_SOURCES = struggle5_mp.c
struggle_ss_SOURCES = struggle_ss.c
nnevolve_SOURCES = diagnostics.c

lib_LIBRARIES = libbtree.a

libbtree_a_SOURCES = btree.c

include_HEADERS = btree.h pingpong.h wildfire.h goldberg1.h goldberg2.h

DEPENDENCIES = \
	../util/librandom.a \
	../util/libmethods.a \
	../util/libstr_util.a \
	../util/libnn_util.a \
	../src/libgaul.la


EXTRA_DIST = \
	chromostubs.c \
	chromostubs.h


diagnostics_DEPENDENCIES = ${DEPENDENCIES}
pingpong_DEPENDENCIES = ${DEPENDENCIES}
wildfire_DEPENDENCIES = ${DEPENDENCIES}
goldberg1_DEPENDENCIES = ${DEPENDENCIES}
goldberg2_DEPENDENCIES = ${DEPENDENCIES}
royalroad_DEPENDENCIES = ${DEPENDENCIES}
royalroad_ss_DEPENDENCIES = ${DEPENDENCIES}
struggle_DEPENDENCIES = ${DEPENDENCIES}
struggle2_DEPENDENCIES = ${DEPENDENCIES}
struggle3_DEPENDENCIES = ${DEPENDENCIES}
struggle4_DEPENDENCIES = ${DEPENDENCIES}
struggle5_DEPENDENCIES = ${DEPENDENCIES}
struggle5_mp_DEPENDENCIES = ${DEPENDENCIES}
struggle_ss_DEPENDENCIES = ${DEPENDENCIES}
nnevolve_DEPENDENCIES = ${DEPENDENCIES}

diagnostics_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
pingpong_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
wildfire_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
goldberg1_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm 
goldberg2_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
royalroad_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
royalroad_ss_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle2_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle3_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle4_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle5_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle5_mp_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
struggle_ss_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lm
nnevolve_LDADD = -L../src/ -L../util/ -lgaul -lmethods -lrandom -lstr_util -lnn_util -lm
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(lib_LIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I..
CPPFLAGS = 
LDFLAGS = 
LIBS =  -lslang -lslang
libbtree_a_LIBADD = 
libbtree_a_OBJECTS =  btree.o
AR = ar
PROGRAMS =  $(bin_PROGRAMS)

diagnostics_OBJECTS =  diagnostics.o
diagnostics_LDFLAGS = 
pingpong_OBJECTS =  pingpong.o
pingpong_LDFLAGS = 
wildfire_OBJECTS =  wildfire.o
wildfire_LDFLAGS = 
goldberg1_OBJECTS =  goldberg1.o
goldberg1_LDFLAGS = 
goldberg2_OBJECTS =  goldberg2.o
goldberg2_LDFLAGS = 
royalroad_OBJECTS =  royalroad.o
royalroad_LDFLAGS = 
royalroad_ss_OBJECTS =  royalroad_ss.o
royalroad_ss_LDFLAGS = 
struggle_OBJECTS =  struggle.o
struggle_LDFLAGS = 
struggle2_OBJECTS =  struggle2.o
struggle2_LDFLAGS = 
struggle3_OBJECTS =  struggle3.o
struggle3_LDFLAGS = 
struggle4_OBJECTS =  struggle4.o
struggle4_LDFLAGS = 
struggle5_OBJECTS =  struggle5.o
struggle5_LDFLAGS = 
struggle5_mp_OBJECTS =  struggle5_mp.o
struggle5_mp_LDFLAGS = 
struggle_ss_OBJECTS =  struggle_ss.o
struggle_ss_LDFLAGS = 
nnevolve_OBJECTS =  diagnostics.o
nnevolve_LDFLAGS = 
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
DEP_FILES =  .deps/btree.P .deps/diagnostics.P .deps/goldberg1.P \
.deps/goldberg2.P .deps/pingpong.P .deps/royalroad.P \
.deps/royalroad_ss.P .deps/struggle.P .deps/struggle2.P \
.deps/struggle3.P .deps/struggle4.P .deps/struggle5.P \
.deps/struggle5_mp.P .deps/struggle_ss.P .deps/wildfire.P
SOURCES = $(libbtree_a_SOURCES) $(diagnostics_SOURCES) $(pingpong_SOURCES) $(wildfire_SOURCES) $(goldberg1_SOURCES) $(goldberg2_SOURCES) $(royalroad_SOURCES) $(royalroad_ss_SOURCES) $(struggle_SOURCES) $(struggle2_SOURCES) $(struggle3_SOURCES) $(struggle4_SOURCES) $(struggle5_SOURCES) $(struggle5_mp_SOURCES) $(struggle_ss_SOURCES) $(nnevolve_SOURCES)
OBJECTS = $(libbtree_a_OBJECTS) $(diagnostics_OBJECTS) $(pingpong_OBJECTS) $(wildfire_OBJECTS) $(goldberg1_OBJECTS) $(goldberg2_OBJECTS) $(royalroad_OBJECTS) $(royalroad_ss_OBJECTS) $(struggle_OBJECTS) $(struggle2_OBJECTS) $(struggle3_OBJECTS) $(struggle4_OBJECTS) $(struggle5_OBJECTS) $(struggle5_mp_OBJECTS) $(struggle_ss_OBJECTS) $(nnevolve_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu examples/Makefile

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

libbtree.a: $(libbtree_a_OBJECTS) $(libbtree_a_DEPENDENCIES)
	-rm -f libbtree.a
	$(AR) cru libbtree.a $(libbtree_a_OBJECTS) $(libbtree_a_LIBADD)
	$(RANLIB) libbtree.a

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

diagnostics: $(diagnostics_OBJECTS) $(diagnostics_DEPENDENCIES)
	@rm -f diagnostics
	$(LINK) $(diagnostics_LDFLAGS) $(diagnostics_OBJECTS) $(diagnostics_LDADD) $(LIBS)

pingpong: $(pingpong_OBJECTS) $(pingpong_DEPENDENCIES)
	@rm -f pingpong
	$(LINK) $(pingpong_LDFLAGS) $(pingpong_OBJECTS) $(pingpong_LDADD) $(LIBS)

wildfire: $(wildfire_OBJECTS) $(wildfire_DEPENDENCIES)
	@rm -f wildfire
	$(LINK) $(wildfire_LDFLAGS) $(wildfire_OBJECTS) $(wildfire_LDADD) $(LIBS)

goldberg1: $(goldberg1_OBJECTS) $(goldberg1_DEPENDENCIES)
	@rm -f goldberg1
	$(LINK) $(goldberg1_LDFLAGS) $(goldberg1_OBJECTS) $(goldberg1_LDADD) $(LIBS)

goldberg2: $(goldberg2_OBJECTS) $(goldberg2_DEPENDENCIES)
	@rm -f goldberg2
	$(LINK) $(goldberg2_LDFLAGS) $(goldberg2_OBJECTS) $(goldberg2_LDADD) $(LIBS)

royalroad: $(royalroad_OBJECTS) $(royalroad_DEPENDENCIES)
	@rm -f royalroad
	$(LINK) $(royalroad_LDFLAGS) $(royalroad_OBJECTS) $(royalroad_LDADD) $(LIBS)

royalroad_ss: $(royalroad_ss_OBJECTS) $(royalroad_ss_DEPENDENCIES)
	@rm -f royalroad_ss
	$(LINK) $(royalroad_ss_LDFLAGS) $(royalroad_ss_OBJECTS) $(royalroad_ss_LDADD) $(LIBS)

struggle: $(struggle_OBJECTS) $(struggle_DEPENDENCIES)
	@rm -f struggle
	$(LINK) $(struggle_LDFLAGS) $(struggle_OBJECTS) $(struggle_LDADD) $(LIBS)

struggle2: $(struggle2_OBJECTS) $(struggle2_DEPENDENCIES)
	@rm -f struggle2
	$(LINK) $(struggle2_LDFLAGS) $(struggle2_OBJECTS) $(struggle2_LDADD) $(LIBS)

struggle3: $(struggle3_OBJECTS) $(struggle3_DEPENDENCIES)
	@rm -f struggle3
	$(LINK) $(struggle3_LDFLAGS) $(struggle3_OBJECTS) $(struggle3_LDADD) $(LIBS)

struggle4: $(struggle4_OBJECTS) $(struggle4_DEPENDENCIES)
	@rm -f struggle4
	$(LINK) $(struggle4_LDFLAGS) $(struggle4_OBJECTS) $(struggle4_LDADD) $(LIBS)

struggle5: $(struggle5_OBJECTS) $(struggle5_DEPENDENCIES)
	@rm -f struggle5
	$(LINK) $(struggle5_LDFLAGS) $(struggle5_OBJECTS) $(struggle5_LDADD) $(LIBS)

struggle5_mp: $(struggle5_mp_OBJECTS) $(struggle5_mp_DEPENDENCIES)
	@rm -f struggle5_mp
	$(LINK) $(struggle5_mp_LDFLAGS) $(struggle5_mp_OBJECTS) $(struggle5_mp_LDADD) $(LIBS)

struggle_ss: $(struggle_ss_OBJECTS) $(struggle_ss_DEPENDENCIES)
	@rm -f struggle_ss
	$(LINK) $(struggle_ss_LDFLAGS) $(struggle_ss_OBJECTS) $(struggle_ss_LDADD) $(LIBS)

nnevolve: $(nnevolve_OBJECTS) $(nnevolve_DEPENDENCIES)
	@rm -f nnevolve
	$(LINK) $(nnevolve_LDFLAGS) $(nnevolve_OBJECTS) $(nnevolve_LDADD) $(LIBS)

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

subdir = examples

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu examples/Makefile
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
install-exec-am: install-libLIBRARIES install-binPROGRAMS
install-exec: install-exec-am

install-data-am: install-includeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLIBRARIES uninstall-binPROGRAMS \
		uninstall-includeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LIBRARIES) $(PROGRAMS) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(bindir) \
		$(DESTDIR)$(includedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-binPROGRAMS \
		mostlyclean-tags mostlyclean-depend mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLIBRARIES clean-compile clean-libtool \
		clean-binPROGRAMS clean-tags clean-depend clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLIBRARIES distclean-compile \
		distclean-libtool distclean-binPROGRAMS distclean-tags \
		distclean-depend distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
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
mostlyclean-binPROGRAMS distclean-binPROGRAMS clean-binPROGRAMS \
maintainer-clean-binPROGRAMS uninstall-binPROGRAMS install-binPROGRAMS \
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
