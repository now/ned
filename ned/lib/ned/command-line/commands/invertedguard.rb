# contents: Inverted-guard command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ And here’s the inverted guard:
module Ned::CommandLine
  class Commands::InvertedGuard < Command
    commandline_command :address, :v do [regex, command] end

    def initialize(address, regex, command)
      @address, @command = address, command
      @matcher = PatternMatcher.new(regex, Ned::Registry.instance.regexes)
    end

    def execute
      $buffer.point = @address
      s = $buffer.new_scanner($buffer.point.begin, $buffer.point.end)
      @command.execute if s.search(@matcher).nil?
    end
  end
end
