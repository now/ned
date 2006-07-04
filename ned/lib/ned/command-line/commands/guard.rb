# contents: Guard command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Guard Commands}
#
# There are two commands for applying a guard to another command.  The idea is
# the same as in \SAM: the guard tries to match a regular expression against
# the contents of point.  If it matches, the “guarded” command is executed or
# not, depending on if the guard was positive or inverted.  We begin with the
# positive version:
module Ned::CommandLine
  class Commands::Guard < Command
    commandline_command :address, :g do [regex, command] end

    def initialize(address, regex, command)
      @address, @command = address, command
      @matcher = PatternMatcher.new(regex, Ned::Registry.instance.regexes)
    end

    def execute
      $buffer.point = @address
      s = $buffer.new_scanner($buffer.point.begin, $buffer.point.end)
      @command.execute unless s.search(@matcher).nil?
    end
  end
end
