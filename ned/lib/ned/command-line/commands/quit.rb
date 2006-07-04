# contents: Quit command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Quit Command}
#
# We of course need a way to tell \NED\ that we’re done editing a buffer.
module Ned::CommandLine
  class Commands::Quit < Command
    commandline_command :noaddress, :q

    def execute
      throw :quit
    end
  end
end
