
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
compile_dir = $(DESTDIR)/var/tmp/portage/ruby-1.8.2-r1/work/ruby-1.8.2
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

CFLAGS   =  -fPIC -march=athlon-xp -O3 -msse2 -msse -pipe -funroll-loops -falign-functions=4 -fprefetch-loop-arrays  -fPIC -I$(sitelibdir)/$(sitearch) -g -std=c99 -Wall -W -Wwrite-strings -Waggregate-return -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wundef -Wpointer-arith -Wcast-align -Werror -Wshadow 
CPPFLAGS = -I. -I$(topdir) -I$(hdrdir) -I$(srcdir) -DHAVE_ASSERT_H -DHAVE_STDBOOL_H -DHAVE_STDDEF_H -DHAVE_STDINT_H -DHAVE_STDIO_H -DHAVE_SYS_TYPES_H -DHAVE_NED_UNICODE_H 
CXXFLAGS = $(CFLAGS) 
DLDFLAGS =   
LDSHARED = $(CC) -shared
AR = i686-pc-linux-gnu-ar
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
OBJS = patternmatcher.o parse.o compile.o mempool.o execute.o match.o ast.o
TARGET = patternmatcher
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
install: $(RUBYARCHDIR)/patternmatcher.rb
$(RUBYARCHDIR)/patternmatcher.rb: $(srcdir)/patternmatcher.rb $(RUBYARCHDIR)
	@$(INSTALL_DATA) $(srcdir)/patternmatcher.rb $(RUBYARCHDIR)
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
	@-i686-pc-linux-gnu-ranlib $(DLLIB) 2> /dev/null || true

###
ast.o: ast.c mempool.h ast.h private.h
compile.o: compile.c private.h mempool.h ast.h compile.h
execute.o: execute.c mempool.h private.h ast.h parse.h compile.h \
  execute.h match.h
match.o: match.c private.h match.h
mempool.o: mempool.c private.h mempool.h
parse.o: parse.c mempool.h ast.h parse.h private.h
patternmatcher.o: patternmatcher.c mempool.h private.h ast.h parse.h \
  compile.h execute.h match.h
tags: TAGS
TAGS: ast.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/assert.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdbool.h \
  /usr/include/stdint.h \
  /usr/lib/ruby/site_ruby/1.8/i686-linux/ned/unicode.h mempool.h ast.h \
  private.h
TAGS: compile.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/assert.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdbool.h \
  /usr/include/stdint.h \
  /usr/lib/ruby/site_ruby/1.8/i686-linux/ned/unicode.h private.h \
  mempool.h ast.h compile.h
TAGS: execute.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/assert.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdbool.h \
  /usr/include/stdint.h \
  /usr/lib/ruby/site_ruby/1.8/i686-linux/ned/unicode.h mempool.h \
  private.h ast.h parse.h compile.h execute.h match.h
TAGS: match.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/stdint.h private.h match.h
TAGS: mempool.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/assert.h /usr/include/stdint.h private.h mempool.h
TAGS: parse.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/include/assert.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdbool.h \
  /usr/include/stdint.h \
  /usr/lib/ruby/site_ruby/1.8/i686-linux/ned/unicode.h mempool.h ast.h \
  parse.h private.h
TAGS: patternmatcher.c /usr/lib/ruby/1.8/i686-linux/ruby.h \
  /usr/lib/ruby/1.8/i686-linux/config.h \
  /usr/lib/ruby/1.8/i686-linux/defines.h /usr/include/stdlib.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h /usr/include/alloca.h /usr/include/string.h \
  /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h \
  /usr/include/wchar.h /usr/include/bits/wchar.h /usr/include/gconv.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdarg.h \
  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/limits.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/syslimits.h \
  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
  /usr/include/bits/posix2_lim.h /usr/lib/ruby/1.8/i686-linux/missing.h \
  /usr/include/sys/time.h /usr/lib/ruby/1.8/i686-linux/intern.h \
  /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.5-20050130/include/stdbool.h \
  /usr/include/stdint.h \
  /usr/lib/ruby/site_ruby/1.8/i686-linux/ned/unicode.h mempool.h \
  private.h ast.h parse.h compile.h execute.h match.h
	ctags -f TAGS --declarations --globals -T -w $^
docs:
	rdoc18 --charset utf-8

TEX_SOURCES := $(addsuffix .tex,$(addprefix ../../../../doc/thesis/thesis/sources/patternmatcher/,$(wildcard *.[ch])))

condocs: $(TEX_SOURCES)

../../../../doc/thesis/thesis/sources/patternmatcher/%.tex: %
	{ echo "% This is a generated file.  Please don’t make changes to it."; \
	  echo; \
	  echo "\startcomponent" `echo $@ | sed 's:../../../../doc/thesis/::'`; \
	  echo; \
	  echo "\project masters-project"; \
	  echo "\product thesis"; \
	  echo; \
	  echo; \
	  echo; \
	  condoc.rb $<; \
	  echo; \
	  echo; \
	  echo "\stopcomponent"; \
	} > $@

.PHONY: tags docs condocs
