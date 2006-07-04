# contents: Input scanner.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \subsection{Input Scanner}
#
# Before we begin discussing the input tokens, let’s take a look at the input
# scanner.  The input scanner is rather simple, having only three methods in
# its public interface:
#
# \startitemize
#   \item \Ruby{scan}, which checks if a regular expression matches at the
#   current input location
#   \item \Ruby{skip}, which skips over a given regular expression in the input
#   \item \Ruby{getch}, which returns a single character of input
# \stopitemize
#
# All the above methods will request more input from the user if they need it.
#
# The actual scanning is done by the Ruby \Ruby{strscan} library.
require 'strscan'



class Ned::CommandLine::Parsers::Scanner

  # ¶ A somewhat ugly thing with the current implementation of the
  # command||line parsing is that certain input tokens need to control the
  # parser explicitly.  We include it in the scanner so that they may easily
  # access it.
  def initialize(parser, io)
    @parser, @io = parser, io
    @s = StringScanner.new("")
    ensure_input
  end

  # ¶ This method scans for a regular expression in the input, retrieving more
  # unless the match is optional.
  def scan(regex, optional = false)
    if @s.match?(regex)
      @s.scan(regex)
    elsif not optional
      add_input
      scan(regex)
    end
  end

  alias :[] :scan

  # ¶ It should also be possible to skip a given regular expression in the
  # input, such as one matching whitespace or other non||essentials. 
  def skip(regex)
    ensure_input
    @s.skip(regex)
    skip(regex) if @s.eos?
  end

  # ¶ Getting a single character is straightforward.
  def getch
    ensure_input
    @s.getch
  end

  # ¶ As we said during initialization, the parser should be made available for
  # reading by the tokens.
  attr_reader :parser

private

  def ensure_input
    add_input if @s.eos?
  end

  def add_input
    unless @io.eof?
      @s << @io.readline
    else
      raise EOFError, "Ran out of input while looking for token"
    end
  end
end
