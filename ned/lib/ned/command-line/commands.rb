# contents: Command-line commands.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>

# TODO: #commandline_command should be able to define a default initialize
# method.

# ¶ \subsection{Command||Line Commands}
#
# Now that we have introduced the command||line parser, it’s finally time to
# define some command||line commands.  We begin by setting up a service library
# and will then introduce the individual commands:
module Ned::CommandLine
  module Commands
    def self.register_services(container)
      require 'ned/command-line/command'
      container.namespace :commands
      Dir[File.dirname(__FILE__) + '/commands/*.rb'].each do |f|
        require f
      end
    end

    Needle.register_library 'ned/command-line/commands', self
  end
end
