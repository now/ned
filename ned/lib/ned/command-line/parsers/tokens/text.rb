# contents: Text token.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ Often specified in the same manner as regular expressions, text tokens come
# in two flavors.  Either they are specified in precisely the same manner as
# regular as regular expressions, i.e., bounded by a symbol that doesn’t itself
# appear in the text, or it is given as a set of lines terminated by a line
# consisting of a sole dot (\type{.}).
class Ned::CommandLine::Parsers::Tokens::Text
  def accept(visitor, scanner)
    visitor.visit_text(self, scanner)
  end

  def parse(scanner)
    if scanner.scan(/\s*$/, true)
      scanner[/.*\n\.\n/m].slice(0..-4).gsub(/^\.\./, ".")
    else
      scanner.skip(/\s*/)
      d = scanner.getch
      scanner[/[^#{d}\\]*(?:\\.[^#{d}\\]*)*#{d}/].chop!.gsub(/\\#{d}/, "#{d}")
    end
  end
end
