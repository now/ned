# contents: Subsitute command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Substitute Command}
#
# The substitute command will replace every match of a regular expression of
# point with a given string.  This string may contain references to submatches
# of the matched text in point, just as the substitute command in \SAM.
module Ned::CommandLine
  class Commands::Substitute < Command
    commandline_command :address, :s do [regex, text] end

    def initialize(address, regex, text)
      @address, @text = address, text
      @matcher = PatternMatcher.new(regex, Ned::Registry.instance.regexes)
    end

    def execute
      $buffer.point = @address
      s = $buffer.new_scanner($buffer.point.begin, $buffer.point.end)
      unless (m = s.search(@matcher)).nil?
        if m[0].begin == 46 && m[0].end == 47
          p $buffer.point
          p $buffer[$buffer.point]
        end
        $buffer.point = m[0].begin..m[0].end
        subst = make_substitution(m)
        $buffer.delete
        $buffer.insert(subst, :after)
        $buffer.point = ($buffer.point.begin..($buffer.point.end + subst.length))
      end
    end

    private

    def make_substitution(matches)
      @text.gsub(/\\\\|\\\d+/) do |c|
      case c
      when "\\\\"
        "\\"
      else
        i = c[1..-1].to_i
        if i >= matches.length
          raise RuntimeError, "not enough captures to make substitution"
        end
        $buffer[matches[i]]
      end
      end
    end
  end
end
