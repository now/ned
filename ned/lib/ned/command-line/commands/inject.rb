# contents: Inject command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Inject Command}
#
# Injecting is the reverse of extracting, so this command works in reverse of
# the extract command.  What it does is executes its command argument {\em
# between} matches of a given regular expression.  It thus does the opposite of
# what the extract command does.
module Ned::CommandLine
  class Commands::Inject < Command
    commandline_command :address, :y do [regex, command] end

    def initialize(address, regex, command)
      @address, @command = address, command
      @matcher = PatternMatcher.new(regex, Ned::Registry.instance.regexes)
    end

    def execute
      $buffer.point = @address
        puts 'here'
      b = $buffer.point.begin
      s = $buffer.new_scanner($buffer.point.begin, $buffer.point.end)
      matches = []
      loop do
        break if (m = s.search(@matcher)).nil?
        matches << m[0]
        s.pos += 1 if m[0].end - m[0].begin == 0
      end
      oe = $buffer.point.end
      matches.reverse_each do |match|
        $buffer.point = (match.end..oe)
        @command.execute
        oe = match.begin
      end
      $buffer.point = (b..oe)
      @command.execute
    end
  end
end
