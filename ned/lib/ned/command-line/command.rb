# contents: Command-line command class.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{The {\em Base} Command}
#
# Actually, before we write our commands, let’s define a class that will act as
# a parent to all other command||line commands.
class Ned::CommandLine::Command

  # ¶ The notion of a description of commands to be parsed by the command||line
  # parser has already been discussed.  The following class embodies it.
  class CommandDescription
    def initialize(takes_address, klass, tokens)
      @takes_address, @klass, @tokens = takes_address, klass, tokens
    end

    def takes_address?
      @takes_address
    end

    attr_reader :klass, :tokens
  end

  # ¶ We have now arrived at the method that ties everything concerning the
  # command||line together.  It provides the coupling between the parser and
  # the definition of new commands.  The idea is simple.  Define a class method
  # that subclasses can invoke to register themselves with the command||line
  # part of the dependency||injection registry, setting up a description of the
  # command that the command||line parser can fetch from the registry during
  # parsing.  This uses a sweet trick \Ruby{instance_eval} to make the
  # definition of commands in the subclasses straightforward.  We’ll see it how
  # it’s used\dots right now in fact.
  def self.commandline_command(takes_range, name, &block)
    command_line = Ned::Registry.instance[:command_line]
    command_line[:commands].register(name) do
      args = block_given? ? command_line.parsers.tokens.instance_eval(&block) : []
      CommandDescription.new(takes_range == :address, self, args)
    end
  end

  private_class_method :commandline_command
end
