
SHELL = /bin/sh

#### Start of system configuration section. ####

srcdir = .
topdir = /usr/lib/ruby/1.8/i686-linux
hdrdir = $(topdir)
VPATH = $(srcdir)
prefix = $(DESTDIR)/usr
exec_prefix = $(prefix)
sitedir = $(prefix)/lib/ruby/site_ruby
rubylibdir = $(libdir)/ruby/$(ruby_version)
builddir = $(ac_builddir)
archdir = $(rubylibdir)/$(arch)
sbindir = $(exec_prefix)/sbin
compile_dir = $(DESTDIR)/var/tmp/portage/ruby-1.8.2/work/ruby-1.8.2
datadir = $(DESTDIR)/usr/share
includedir = $(prefix)/include
infodir = $(DESTDIR)/usr/share/info
top_builddir = $(ac_top_builddir)
sysconfdir = $(DESTDIR)/etc
mandir = $(DESTDIR)/usr/share/man
libdir = $(exec_prefix)/lib
sharedstatedir = $(prefix)/com
oldincludedir = $(DESTDIR)/usr/include
sitearchdir = $(sitelibdir)/$(sitearch)
bindir = $(exec_prefix)/bin
localstatedir = $(DESTDIR)/var/lib
sitelibdir = $(sitedir)/$(ruby_version)
libexecdir = $(exec_prefix)/libexec

CC = i686-pc-linux-gnu-gcc
LIBRUBY = $(LIBRUBY_SO)
LIBRUBY_A = lib$(RUBY_SO_NAME)-static.a
LIBRUBYARG_SHARED = -Wl,-R -Wl,$(libdir) -L$(libdir) -L. -l$(RUBY_SO_NAME)
LIBRUBYARG_STATIC = -l$(RUBY_SO_NAME)-static

CFLAGS   =  -fPIC -march=athlon-xp -O3 -msse2 -msse -pipe -funroll-loops -falign-functions=4 -fprefetch-loop-arrays  -fPIC -std=c99 -Wall -W -Wwrite-strings -Waggregate-return -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wundef -Wpointer-arith -Wcast-align -Werror -Wshadow 
CPPFLAGS = -I. -I$(topdir) -I$(hdrdir) -I$(srcdir) -DHAVE_ASSERT_H -DHAVE_LOCALE_H -DHAVE_STDBOOL_H -DHAVE_STDDEF_H -DHAVE_STDINT_H -DHAVE_STDIO_H -DHAVE_STDLIB_H -DHAVE_STRING_H -DHAVE_SYS_TYPES_H -DHAVE_WCHAR_H 
CXXFLAGS = $(CFLAGS) 
DLDFLAGS =   
LDSHARED = $(CC) -shared
AR = ar
EXEEXT = 

RUBY_INSTALL_NAME = ruby18
RUBY_SO_NAME = $(RUBY_INSTALL_NAME)
arch = i686-linux
sitearch = i686-linux
ruby_version = 1.8
ruby = /usr/bin/ruby18
RUBY = $(ruby)
RM = $(RUBY) -run -e rm -- -f
MAKEDIRS = $(RUBY) -run -e mkdir -- -p
INSTALL_PROG = $(RUBY) -run -e install -- -vpm 0755
INSTALL_DATA = $(RUBY) -run -e install -- -vpm 0644

#### End of system configuration section. ####


LIBPATH =  -L'$(libdir)' -Wl,-R'$(libdir)'
DEFFILE = 

CLEANFILES = 
DISTCLEANFILES = 

target_prefix = /ned
LOCAL_LIBS = 
LIBS = $(LIBRUBYARG_SHARED)  -ldl -lcrypt -lm   -lc
OBJS = break.o decompose.o properties.o str.o unicode.o utf.o
TARGET = unicode
DLLIB = $(TARGET).so
STATIC_LIB = $(TARGET).a

RUBYCOMMONDIR = $(sitedir)$(target_prefix)
RUBYLIBDIR    = $(sitelibdir)$(target_prefix)
RUBYARCHDIR   = $(sitearchdir)$(target_prefix)

CLEANLIBS     = "$(TARGET).{lib,exp,il?,tds,map}" $(DLLIB)
CLEANOBJS     = "*.{o,a,s[ol],pdb,bak}"

all:		$(DLLIB)
static:		$(STATIC_LIB)

clean:
		@$(RM) $(CLEANLIBS) $(CLEANOBJS) $(CLEANFILES)

distclean:	clean
		@$(RM) Makefile extconf.h conftest.* mkmf.log
		@$(RM) core ruby$(EXEEXT) *~ $(DISTCLEANFILES)

realclean:	distclean
install: $(RUBYARCHDIR)
install: $(RUBYARCHDIR)/$(DLLIB)
$(RUBYARCHDIR)/$(DLLIB): $(DLLIB) $(RUBYARCHDIR)
	@$(INSTALL_PROG) $(DLLIB) $(RUBYARCHDIR)
install: $(RUBYARCHDIR)/unicode.h
$(RUBYARCHDIR)/unicode.h: $(srcdir)/unicode.h $(RUBYARCHDIR)
	@$(INSTALL_DATA) $(srcdir)/unicode.h $(RUBYARCHDIR)
$(RUBYARCHDIR):
	@$(MAKEDIRS) $(RUBYARCHDIR)

site-install: install

.SUFFIXES: .c .cc .m .cxx .cpp .C .o

.cc.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

.cxx.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

.C.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

$(DLLIB): $(OBJS)
	@-$(RM) $@
	$(LDSHARED) $(DLDFLAGS) $(LIBPATH) -o $(DLLIB) $(OBJS) $(LOCAL_LIBS) $(LIBS)

$(STATIC_LIB): $(OBJS)
	$(AR) cru $@ $(OBJS)
	@-ranlib $(DLLIB) 2> /dev/null || true

###
break.o: break.c unicode.h unicode-data/unicode-break.h
decompose.o: decompose.c unicode.h private.h \
  unicode-data/unicode-decompose.h unicode-data/unicode-compose.h
properties.o: properties.c unicode.h private.h \
  unicode-data/unicode-character-tables.h
str.o: str.c str.h
unicode.o: unicode.c unicode.h
utf.o: utf.c unicode.h private.h str.h
