# contents: Command-line-mode parser and commands.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \section{The Command||Line}
#
# The command||line is responsible for reading input from the user, parsing it,
# and executing the resulting commands.  The parser is extensible and new
# commands can easily be added.  We will soon discuss the details.
#
# The command||line service library actually only consists of two other
# service libraries, namely one defining a library for command||line parsers
# and one defining a library for command||line commands.

module Ned
  module CommandLine
    def self.register_services(container)
      container.namespace :command_line
      container.command_line.define do |b|
        b.require_library 'ned/command-line/parsers'
        b.require_library 'ned/command-line/commands'
      end
    end

    Needle.register_library 'ned/command-line', self
  end
end
