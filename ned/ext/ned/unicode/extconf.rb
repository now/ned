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
have_header('locale.h')
have_header('stdbool.h')
have_header('stddef.h')
have_header('stdint.h')
have_header('stdio.h')
have_header('stdlib.h')
have_header('string.h')
have_header('sys/types.h')
have_header('wchar.h')

$INSTALLFILES ||= []
$INSTALLFILES << ['unicode.h', '$(RUBYARCHDIR)', 'lib']

create_makefile('ned/unicode')
