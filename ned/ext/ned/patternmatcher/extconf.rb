require 'mkmf'

have_header('assert.h')
have_header('stdbool.h')
have_header('stddef.h')
have_header('stdint.h')
have_header('stdio.h')
have_header('sys/types.h')

$INCFLAGS += " -I#{$sitearchdir}"
$CFLAGS += " -I#{$sitearchdir} -g"

have_header('ned/unicode.h')

def try_compiler_option(opt, &b)
  checking_for "‘#{opt}’ option to compiler" do
    if try_compile('', opt, &b)
      $CFLAGS += " #{opt}"
      true
    else
      false
    end
  end
end

try_compiler_option('-std=c99')
try_compiler_option('-Wall')
try_compiler_option('-W')
try_compiler_option('-Wwrite-strings')
try_compiler_option('-Waggregate-return')
try_compiler_option('-Wmissing-prototypes')
try_compiler_option('-Wmissing-declarations')
try_compiler_option('-Wnested-externs')
try_compiler_option('-Wundef')
try_compiler_option('-Wpointer-arith')
try_compiler_option('-Wcast-align')
try_compiler_option('-Werror')
try_compiler_option('-Wshadow')

$INSTALLFILES ||= []
$INSTALLFILES << ['patternmatcher.rb', '$(RUBYARCHDIR)', 'lib']

create_makefile('ned/patternmatcher')

File.open('Makefile', 'a') do |f|
  f.puts <<EOF
tags: TAGS
#{`#{Config.expand("$(CC) -c #$INCFLAGS -I#{$hdrdir} -M -MT TAGS *.c")}`}	ctags -f TAGS --declarations --globals -T -w $^
EOF
  f.puts <<EOF
docs:
	rdoc18 --charset utf-8

TEX_SOURCES := $(addsuffix .tex,$(addprefix ../../../../doc/thesis/thesis/sources/patternmatcher/,$(wildcard *.[ch])))

condocs: $(TEX_SOURCES)

../../../../doc/thesis/thesis/sources/patternmatcher/%.tex: %
	{ echo "% This is a generated file.  Please don’t make changes to it."; \\
	  echo; \\
	  echo "\\startcomponent" `echo $@ | sed 's:../../../../doc/thesis/::'`; \\
	  echo; \\
	  echo "\\project masters-project"; \\
	  echo "\\product thesis"; \\
	  echo; \\
	  echo; \\
	  echo; \\
	  condoc.rb $<; \\
	  echo; \\
	  echo; \\
	  echo "\\stopcomponent"; \\
	} > $@

.PHONY: tags docs condocs
EOF
end
