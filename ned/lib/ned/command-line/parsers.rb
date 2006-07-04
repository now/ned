# contents: Command-line parser services.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{Command||Line Parsers}
#
# A command||line parser will, when invoked, request input from the user until
# a complete command has been given.  The parser incrementally creates itself
# an idea of what is to come and can thus provide the user with this
# information before they type it.
#
# The parser defines a set of tokens that may be found in the input stream,
# such as commands, ordered sets of commands, regular expressions, strings, and
# so on.  Whenever a command token is read, which is used as an entry point by
# the parser, information about what parameters the command needs is retrieved
# from a command database and the parser continues to try to read the tokens
# representing these parameters from the input.  A parse is only considered
# complete when there is no more tokens to be read.  This means that, unlike
# many contemporary text editors, the command||line parser can decide for
# itself when the input has ended, not the user.
#
# The parsers are implemented using the strategy software pattern that apply
# the visitor pattern on the input tokens.  This allows for many different
# kinds of parsers to be created, such as a simple parser that simply reads the
# input or a more user||friendly one that tells the user what it kind of tokens
# it expects to find next.  This latter type of parser can be instrumental in
# the process of learning how to operate the text editor and can later be
# substituted for the quiet alternative, once the user has become familiar with
# the command||set.
#
# We begin by defining a service library for our parsers.  The only one
# included at this time is a simple unfriendly parser that just goes about its
# business.  We also require a service library of tokens and take the
# opportunity to define an error||class that may be used during parsing.
module Ned::CommandLine
  module Parsers
    class ParseError < StandardError; end

    def self.register_services(container)
      container.namespace_define :parsers do |b|
        b.require_library 'ned/command-line/parsers/tokens'
        b.simple do
          require 'ned/command-line/parsers/simple'
          Simple.new(container.commands, b.tokens)
        end
      end
    end

    Needle.register_library 'ned/command-line/parsers', self
  end
end
