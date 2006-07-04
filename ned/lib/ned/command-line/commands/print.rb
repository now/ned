# contents: Print command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Print Command}
#
# The most basic command of the commands that we’ll be defining allows a user
# to print the contents of point.
module Ned::CommandLine
  class Commands::Print < Command
    commandline_command :address, :p

    def initialize(address)
      @address = address
    end

    def execute
      $buffer.point = @address
      puts $buffer[$buffer.point]
    end
  end
end
