# contents: Insert command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Insert Command}
#
# The following class defines a command for inserting text before point:
module Ned::CommandLine
  class Commands::Insert < Command
    commandline_command :address, :i do [text] end

    def initialize(address, text)
      @address, @text = address, text
    end

    def execute
      $buffer.point = @address
      $buffer.insert(@text, :before)
    end
  end
end
