# contents: Commands token.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ Our next token deals with compound commands.  It allows us to define a
# command that puts together several commands and executes them as a unit.
#
# The {\em commands} token reads commands until it encounters one that has been
# set as a delimiter for the token instance and then returns the set of
# commands parsed.
class Ned::CommandLine::Parsers::Tokens::Commands
  def initialize(delimiter)
    @delimiter = delimiter
  end

  def accept(visitor, scanner)
    visitor.visit_commands(self, scanner)
  end

  def parse(scanner, tokens)
    commands = []
    loop do
      command = tokens.command.accept(scanner.parser, scanner)
      break if command.is_a? @delimiter
      commands << command
    end
    commands
  end
end
