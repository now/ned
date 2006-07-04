# Contents: Compound command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{Compound Commands}
#
# It has already been suggested that we may create commands that consist of
# other commands.  The following code implements a command that groups a set of
# commands delimited by \type/{/\dots\type/}/ together as one command.
module Ned::CommandLine
  module Commands
    class EndCompound < Command
      commandline_command :noaddress, :"}"
    end

    class Compound < Command
      commandline_command :noaddress, :"{" do [commands(EndCompound)] end

      def initialize(commands)
        @commands = commands
      end

      def execute
        @commands.each do |command|
          command.execute
        end
      end
    end
  end
end
