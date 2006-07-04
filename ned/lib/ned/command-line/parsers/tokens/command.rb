# contents: Command token.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>


# ¶ \subsection{The Parsing Tokens}
#
# The final pieces of our command||line parser is the classes that represent
# our input stream tokens we wish to parse.
#
# The first token that we’ll discuss represents commands.  Commands are simply
# tokens in the input stream that may be preceded by an address specification
# and followed by any number of arguments.  Commands are registered dynamically
# during run||time by providing a central repository with information regarding
# the use of addresses (some commands will not want addresses, some will work
# with our without them, and some require them) and the type of tokens they
# expect as arguments.  This means that the parser is extensible in that it can
# parse new commands without additional work.  The only requirement is that
# they can be expressed using the tokens that are available.

class Ned::CommandLine::Parsers::Tokens::Command
  # ¶ Tokens use the visitor pattern together with the parser, so we need an
  # accept method:
  def accept(visitor, scanner)
    visitor.visit_command(self, scanner)
  end

  # ¶ Parsing a command is done as follows:
  #
  # \startenumerate
  #   \item First, try to parse an address that may appear before the name of
  #     the command
  #
  #   \item Skipping any whitespace,  the command||name is parsed next
  #
  #   \item Then we need to check that a command by such a name exists and, if
  #     so, that the command is being used correctly with regard to the
  #     existence of an address
  #
  #   \item Using the information given in the command description, each token
  #     object expected as argument to the command is parsed
  # \stopenumerate
  #
  # After this parsing, the class associated with the command is instantiated
  # using the parsed arguments.
  def parse(scanner, commands)
    address = parse_address(scanner)

    scanner.skip(/\s*/)
    command = scanner[/[\w]+|[{}]/].intern

    unless commands.has_key? command
      raise Ned::CommandLine::Parsers::ParseError, "Unknown command: #{command}"
    end
    desc = commands[command]
    unless desc.takes_address? or address.nil? or address.is_a? PointAddress
      raise Ned::CommandLine::Parsers::ParseError,
        "Command #{command} doesn’t take an address"
    end
    if desc.takes_address? and address.nil?
      raise Ned::CommandLine::Parsers::ParseError,
        "Command #{command} requires an adress"
    end

    args = desc.takes_address? ? [address] : []
    desc.tokens.each do |token|
      args << token.accept(scanner.parser, scanner)
    end

    desc.klass.new(*args)
  end

private

  # ¶ Parsing an address is in a sense a lot more complicated than parsing a
  # whole command.  The reason for this is that we want to make as many ways of
  # expressing an address available to the user as possible.  The procedure of
  # doing so is broken up into two main methods, \Ruby{parse_address}, which
  # delegates parsing of simple addresses and possibly combines them into
  # compound addresses, and \Ruby{parse_simple_address}, which parses simple
  # addresses.
  # 
  # Complex addresses can be created in many different ways, as listed in
  # \intable[compound addresses].
  #
  # \placetable
  #   [here]
  #   [compound addresses]
  #   {Compound addresses build new addresses by combining other addresses.}
  #   \starttable[|l|l|]
  #     \HL
  #     \NC \bf Specification   \NC \bf Meaning \NC\AR
  #     \HL
  #     \NC $a_1$\type{,}$a_2$  \NC From the beginning of $a_1$ to the end of $a_2$ \NC\AR
  #     \NC $a_1$\type{;}$a_2$  \NC As above, but set point to $a_1$ before evaluating $a_2$ \NC\AR
  #     \HL
  #   \stoptable
  def parse_address(scanner)
    scanner.skip(/\s*/)
    b = parse_simple_address(scanner)
    scanner.skip(/\s*/)
    sep = scanner.scan(/[,;+]/, true)
    if sep.nil?
      b or PointAddress.new
    else
      b = ByteOffsetAddress.new(0) if b.nil?
      scanner.skip(/\s*/)
      e = parse_address(scanner)
      e = EOBAddress.new if e.is_a? PointAddress

      case sep
      when ',': InclusiveCompoundAddress.new(b, e)
      when ';': ExclusiveCompoundAddress.new(b, e)
      else	nil
      end
    end
  end

  # ¶ A simple address can be expressed in any of the ways shown in
  # \intable[simple addresses].
  # \placetable
  #   [here]
  #   [simple addresses]
  #   {Simple addresses can be expressed in a variety of ways.}
  #   \starttable[|l|l|]
  #     \HL
  #     \NC \bf Specification \NC \bf Meaning \NC\AR
  #     \HL
  #     \NC \type{#}$n$       \NC Empty string after symbol $n$ \NC\AR
  #     \NC $n$               \NC Empty string before first symbol of line $n$ \NC\AR
  #     \NC \type{$}          \NC Empty string after the last symbol in buffer \NC\AR
  #     \NC \type{.}          \NC Point \NC\AR
  #     \HL
  #   \stoptable
  def parse_simple_address(scanner)
    b = scanner.scan(/#?\d+|\$|\./, true)
    case b
    when /#\d+/:  ByteOffsetAddress.new(b[1..-1].to_i)
    when /\d+/:	  LineOffsetAddress.new(b.to_i)
    when '$':	  EOBAddress.new
    when '.':	  PointAddress.new
    else	  nil
    end
  end

  # ¶ In the future, the addressing scheme will be extensible.  This will allow
  # users to add their own address specifications to the parser.


  # ¶ The following classes implement the simple and compound addresses already
  # described above; they speak for themselves.

  class ByteOffsetAddress
    def initialize(n)
      @n = n
    end

    def to_point(buffer)
      (@n..@n)
    end
  end

  class LineOffsetAddress
    def initialize(n)
      @n = n
    end

    def to_point(buffer)
      o = buffer.line_offset(@n)
      (o..o)
    end
  end

  class EOBAddress
    def to_point(buffer)
      s = buffer.size
      (s..s)
    end
  end

  class PointAddress
    def to_point(buffer)
      buffer.point
    end
  end

  module CompoundAddress
    def initialize(b, e)
      @begin, @end = b, e
    end
  end

  class InclusiveCompoundAddress
    include CompoundAddress

    def to_point(buffer)
      @begin.to_point(buffer).begin..@end.to_point(buffer).end
    end
  end

  class ExclusiveCompoundAddress
    include CompoundAddress

    def to_point(buffer)
      buffer.point = b = @begin.to_point(buffer)
      b.begin..@end.to_point(buffer).end
    end
  end
end
