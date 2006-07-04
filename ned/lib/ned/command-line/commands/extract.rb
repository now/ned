# contents: Extract command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Extract Command}
#
# Ned’s extract command works in the same manner as the command with the same
# name in the \SAM\ editor does, see
# \insection[regexes in text editors:repeating commands].  It iterates over
# every match of a given regular expression, executing a command on the match.
module Ned::CommandLine
  class Commands::Extract < Command
    commandline_command :address, :x do [regex, command] end

    def initialize(address, regex, command)
      @address, @command = address, command
      @matcher = PatternMatcher.new(regex, Ned::Registry.instance.regexes)
    end

    def execute
      $buffer.point = @address
      s = $buffer.new_scanner($buffer.point.begin, $buffer.point.end)
      matches = []
      loop do
        break if (m = s.search(@matcher)).nil?
        matches << m[0]
        s.pos += 1 if m[0].end - m[0].begin == 0
      end
      matches.reverse_each do |match|
        $buffer.point = (match.begin..match.end)
        @command.execute
      end
    end
  end
end
