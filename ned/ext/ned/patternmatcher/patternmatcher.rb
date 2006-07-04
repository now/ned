=begin
  :contents: Initialization of the patternmatcher library.
  :arch-tag: 4086b5fd-49c3-4bc5-a268-0d6d41e640aa

  Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
=end



require 'ned/unicode'
require 'ned/patternmatcher.so'

class PatternMatcher::Match
  def length
    self.end - self.begin + 1
  end
  alias size length
end



# vim: set sts=2 sw=2:
