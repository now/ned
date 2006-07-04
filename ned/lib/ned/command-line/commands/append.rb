# contents: Append command.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The Append Command}
#
# An excellent command to begin with is the append command.  This command
# inserts a given text token after point in our buffer.  It is a good example
# of how the \Ruby{commandline_command} method can be used, as it contains all
# the available components:

module Ned::CommandLine
  class Commands::Append < Command
    commandline_command :address, :a do [text] end

    def initialize(address, text)
      @address, @text = address, text
    end

    def execute
      $buffer.point = @address
      $buffer.insert(@text, :after)
    end
  end
end

# ¶ One can read the invocation of \Ruby{commandline_command} as a
# specification of the command if one likes.  This one reads something like
# “We’re defining a command||line command (taking an address) named \type{a}
# and requires one argument, a text token”.  We then see how the initializer
# takes an address and a text as arguments.  These have been created by the
# parser already and are then passed along to us.  Executing the append command
# is done by setting point to the given address and then inserting the text
# after it.  Very straightforward indeed.
#
# You may have noticed that we’re using a global variable for accessing the
# buffer we wish to operate upon.  This is of course only a temporary solution
# until a satisfactory model for managing buffers has been thought of.  There
# are some alternatives to how this should be done, and I have been unable to
# decide on which one to use.  Either commands are passed the buffer they
# should operate upon as an argument to the \Ruby{execute} method or they have
# to figure it out for themselves by querying some kind of buffer||manager.
# The latter solution is most flexible and removes some additional complexity
# by avoiding the need of passing extra arguments.  It may, however, sometimes
# be of value to dictate to the command what buffer to operate upon.
