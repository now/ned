# contents: Change command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Change Command}
#
# The change command replaces the contents of point with the contents of its
# text argument.  Point is then set to point to the new text.
module Ned::CommandLine
  class Commands::Change < Command
    commandline_command :address, :c do [text] end

    def initialize(address, text)
      @address, @text = address, text
    end

    def execute
      $buffer.point = @address
      $buffer.delete
      $buffer.insert(@text, :after)
      $buffer.point = ($buffer.point.begin..($buffer.point.end + @text.length))
    end
  end
end
