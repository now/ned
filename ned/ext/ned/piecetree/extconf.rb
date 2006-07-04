require 'mkmf'

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

have_header('assert.h')
have_header('ruby.h')
have_header('stdbool.h')
have_header('stdint.h')
have_header('sys/types.h')

create_makefile('ned/piecetree')

SOURCES = Dir['*.c'].join(' ')

File.open('Makefile', 'a') do |f|
  f.puts <<EOF
tags: TAGS
#{`#{Config.expand("$(CC) -c #$INCFLAGS -I#{$hdrdir} -M -MT TAGS #{SOURCES}")}`}	ctags -f TAGS --declarations --globals -T -w $^
EOF
  f.puts <<EOF
docs:
	rdoc18 --charset utf-8 --op ,doc

TEX_SOURCES := $(addsuffix .tex,$(addprefix ../../../../doc/thesis/thesis/sources/piecetree/,$(wildcard *.[ch])))

condocs: $(TEX_SOURCES)

../../../../doc/thesis/thesis/sources/piecetree/%.tex: %
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
