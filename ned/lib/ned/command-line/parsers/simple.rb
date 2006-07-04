# contents: Simple command-line parser.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Simple Parser}
#
# The simple parser does nothing but parse, which is good, as it gets the jobs
# done, but it’s not very interesting to discuss.  Anyway, the common input
# scanner to be used by all parsers is kept separate:
require 'ned/command-line/parsers/scanner'


# ¶ The scanner is implemented using the visitor pattern, as can be seen in the
# following code:

module Ned::CommandLine::Parsers
  class Simple
    def initialize(commands, tokens)
      @commands, @tokens = commands, tokens
    end

    def parse(io)
      @tokens.command.accept(self, Scanner.new(self, io))
    end

    def visit_command(command, scanner)
      command.parse(scanner, @commands)
    end

    def visit_commands(commands, scanner)
      commands.parse(scanner, @tokens)
    end

    def visit_regex(regex, scanner)
      regex.parse(scanner)
    end

    def visit_text(text, scanner)
      text.parse(scanner)
    end
  end
end

# ¶ We pass along the scanner to each type of token so that it may parse itself
# from the input.  Some commands need additional information and have it passed
# as additional arguments to their \Ruby{parse} methods.
