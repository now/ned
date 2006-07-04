# contents: Tokens for the parser.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{Token Library}
#
# Tokens are stored in a service library as follows:

module Ned::CommandLine::Parsers
  module Tokens
    def self.register_services(container)
      container.namespace_define :tokens do |b|
        b.command do
          require 'ned/command-line/parsers/tokens/command'
          Command.new
        end

        b.commands(:model => :multiton) do |c, p, delimiter|
          require 'ned/command-line/parsers/tokens/commands'
          Commands.new(delimiter)
        end

        b.regex do
          require 'ned/command-line/parsers/tokens/regex'
          Regex.new
        end

        b.text do
          require 'ned/command-line/parsers/tokens/text'
          Text.new
        end
      end
    end

    Needle.register_library 'ned/command-line/parsers/tokens', self
  end
end
