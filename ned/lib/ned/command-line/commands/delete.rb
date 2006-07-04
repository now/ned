# contents: Delete command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Delete Command}
#
# Deleting the contents of point is a useful command.  The command itself is
# more or less the same as the change command, replacing the contents of point
# with the empty string instead of a text argument.
module Ned::CommandLine
  class Commands::Delete < Command
    commandline_command :address, :d

    def initialize(address)
      @address = address
    end

    def execute
      $buffer.point = @address
      $buffer.delete
    end
  end
end
