# contents: Define command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Define Command}
#
# The define command allows us to define new regular||expression rules that we
# may use in subsequent regular expressions.
module Ned::CommandLine
  class Commands::Define < Command
    commandline_command :noaddress, :def do [regex, regex] end

    def initialize(rule, regex)
      Ned::Registry.instance.regexes[rule] = regex
    end

    def execute; end
  end
end
