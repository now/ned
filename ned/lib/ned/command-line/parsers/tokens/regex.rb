# contents: Regex token.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ Many commands demand regular expressions as arguments.  Regular expressions
# are delimited by any character that may appear escaped within the regular
# expression itself.  The slash character (\type{/}) is often used to this end:
# \type{/.*\/.*/} is an example of this use, where the character itself has
# been escaped within the pattern.  This has to be unescaped before being
# passed back as a result of the parse, as this isn’t actually part of the
# regular expression that we’re trying to specify.
class Ned::CommandLine::Parsers::Tokens::Regex
  def accept(visitor, scanner)
    visitor.visit_regex(self, scanner)
  end

  def parse(scanner)
    scanner.skip(/\s*/)
    d = scanner.getch
    scanner[/[^#{d}\\]*(?:\\.[^#{d}\\]*)*#{d}/].chop!.gsub(/\\#{d}/, "#{d}")
  end
end
