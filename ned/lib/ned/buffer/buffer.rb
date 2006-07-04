# contents: Buffer class.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ Before we begin with our buffer class, we need to load our piece||tree
# library.

require 'ned/piecetree'


# ¶ Now, then, we can begin thinking of how we want to implement our buffer
# class.  We’ll try and constrain the interface as much as possible, as a
# simple interface will be a lot easier to make right and maintain, than a
# large one.  The most important methods have already been suggested.  These
# are methods to move point, insert a string at point, deleting the contents of
# point, and extracting the contents of the buffer pointed at by point.
#
# Other methods that we would like to have is a way to create a
# pattern||matcher that acts upon the buffer and a way to turn a line offset
# into a buffer offset.  These are utility methods that will be useful in
# implementing our commands later on.

class Ned::Buffer::Buffer

  # ¶ We begin with an initializer.  We will need a source of input to
  # represent our original file.  We create an initial piece that points to
  # this file and set point to point to the beginning of the buffer.
  def initialize(io = nil)
    @pieces = PieceTree.new
    @original = Original.new(io)
    @added = ''

    raise NotImplementedError if io.nil?

    io.seek(0, IO::SEEK_END)
    @pieces[0].insert(PieceTree::Piece.new(:original, 0, io.pos, 0), :after)
    iter = @pieces[0]
    @point = Point.new((0..0), iter, iter)
  end

  # ¶ The following two methods deal with updating and retrieving the value of
  # point.  We begin with the setter method.  The new value may be specified in
  # a range\footnote{It’s facinating how languages invite you to write these
  # things\dots.} of ways.  If the parameter is a Range object, we’ll use it
  # as||is.  If it’s an Integer, we create a range covering this sole point in
  # the buffer.  Otherwise, we assume that the parameter responds to the
  # \Ruby{to_point} message and we allow it to convert itself to a valid range,
  # given our buffer.  We will use this last possibility a lot when defining
  # addresses in our command||language.
  def point=(range)
    case range
    when Range:   @point.range = range
    when Integer: @point.range = (range..range)
    else          @point.range = range.to_point(self)
    end
    @point.first = @point.last = nil
    return range
  end

  # ¶ Retrieving the value of point is actually a bit more complicated than one
  # might guess.  The reason is that we need to make sure that all the
  # components of point are valid.  As you may have spotted in the getter
  # above, point consists of three fields, \Ruby{range}, \Ruby{first}, and
  # \Ruby{last}.  The two last\footnote{Again with the unintentional
  # repetition\dots.} fields will be made to point to iterators as soon as an
  # edit takes place at either of the two ends of point.  This is so because an
  # iterator always points to a node in the piece tree and a node always points
  # to a piece and a piece always points to a span in one of the two files.
  # This means that we can make sure that point easily follows along with edits
  # that take place before it without worrying about updating the range, as we
  # can simply invalidate it and create it anew from the position of the two
  # iterators when necessary.  Thus, edits will have to take place at the
  # boundaries of pieces---creating them first if necessary.  We don’t want to
  # create new pieces whenever point is moved, though, so this is delayed for
  # as long as possible.
  def point
    if @point.range.nil?
      update_point :first
      update_point :last
      @point.range = (@point.first.pos..@point.last.pos)
    end
    @point.range
  end

  # ¶ Our next method extracts a subsequence of our buffer and returns it as a
  # String object.  This is, a bit surprisingly, a rather complicated
  # procedure, as we, depending on the parameters that we’re given, need to
  # first extract a subsequence of the piece that contains the position that we
  # wish to begin extracting at, then iterate through the tree, appending the
  # contents of the pieces that we pass to the subsequence, until we don’t want
  # to retrieve more, in which case we only want to read a subsequence of that
  # last piece.  We also need to set up an initial state in which to work that
  # depends heavily on the arguments that we were passed.
  def [](pos, len = nil)
    if pos.is_a? PieceTree::Iterator
      iter = pos
      pos = iter.pos
      len = iter.piece.size if len.nil?
    elsif pos.respond_to? :begin and pos.respond_to? :end
      iter = @pieces[pos.begin]
      len = pos.end - pos.begin
      pos = pos.begin
    else
      iter = @pieces[pos]
      while iter.valid? && iter.has_next? && iter.piece.size == 0
        iter.next
      end
      raise Error, "fuck!" unless iter.valid?
      len = 1 if len.nil?
    end

    actual_pos = iter.piece.offset + pos - iter.pos
    plen = [iter.piece.size - (actual_pos - iter.piece.offset), len].min
    ret = origin2file(iter.piece.origin)[actual_pos, plen]

    len -= plen
    while len > 0 and iter.has_next?
      iter.next
      plen = [iter.piece.size, len].min
      ret << origin2file(iter.piece.origin)[iter.piece.offset, plen]
      len -= plen
    end

    ret
  end

  def origin2file(origin)
    instance_variable_get("@#{origin}")
  end

  private :origin2file

  # ¶ In the future, it will be possible for zero||sized pieces to appear
  # within the buffer|<|in fact, such pieces will probably not have a size
  # field and there will be some sort of abstraction||layer in place so that we
  # don’t have to worry about this|>|and we must thus make sure that we find a
  # piece that actually contains something.  That’s what the first while loop
  # is for in the previous method.
  
  # ¶ Now we’ll write a method for inserting a string to the left (before) or
  # right (after) of point.  This is done by adding the contents of the given
  # string to the add||file and then inserting a new piece in the table or,
  # alternatively, updating an old one if possible.  The specifics of the rules
  # have already been discussed in
  # \insection[text editors - internal:piece table buffering strategy], but
  # lets give them again here, in the context of our implementation.
  #
  # There are, as already stated, two cases to deal with.  Either we insert the
  # new piece before or after point.  When inserting before point, if the piece
  # right before the first one in point fulfills the following conditions, we
  # simply extend the length of it to include the newly added string:
  #
  # \startenumerate
  #   \item There must be a piece laying before the first one in point
  #   \item This piece must originate in the add||file
  #   \item The piece must extend to the end of the add||file, as it was before
  #     the addition of the contents of the new string
  # \stopenumerate
  #
  # Complicated, but perhaps it’s easier to see it expressed in code.
  #
  # When adding text after point, things get a lot more complicated.  We have
  # more or less the same rules available to us as for inserting before point,
  # {\em if} point extends to the end of the buffer.  Otherwise, all we can do
  # is extend the last piece in point, if it is of length zero.  This happens
  # very seldom and is perhaps of little merit.
  #
  # Actually, the first case|<|at the end of the buffer|>|could be seen as a
  # special case of inserting before a sentinel piece in the piece table,
  # representing the end of the buffer.  In a future revision, when we will
  # have many different kinds of pieces in the piece table, for denoting marks
  # and so on, we can add such a sentinel piece and simplify this code a lot.
  def insert(str, where = :before)
    @added << str

    case where
    when :before
      update_point(:first)
      prev = @point.first.has_prev? ? @point.first.dup.prev : nil
      if prev and prev.valid? and prev.piece.origin == :added and
         prev.piece.offset + prev.piece.size == @added.size - str.size
        extend_piece(iter, str.size)
      else
        add_piece(@point.first.dup, str.size, :before)
      end

      @point.range = ((@point.range.begin+str.size)..(@point.range.end+str.size))
    when :after
      if @point.range.end == @pieces.size
        iter = @pieces[@pieces.size - 1]
        if iter and iter.valid? and iter.piece.origin == :added and
           iter.piece.offset + iter.piece.size == @added.size - str.size
          extend_piece(iter, str.size)
        else
          add_piece(@pieces[@pieces.size - 1], str.size, :after)
        end
      else
        update_point(:last)
        if @point.first == @point.last and
           @point.last.piece.origin == :added and @point.last.piece.size == 0
          extend_piece(@point.last, str.size)
        else
          add_piece(@point.last.dup, str.size, :before)
        end
      end
    else
      raise ArgumentError, "unknown insertion point ‘#{where}’"
    end

    return self
  end

  # ¶ We need a method that will extend a given iterator’s piece to include the
  # added string contents:
  def extend_piece(iter, size)
    iter.piece.size += size
    @pieces.size += size
    iter.fix_size
  end

  # ¶ Inserting a new piece pointing at the add||file is rather straightforward:
  def add_piece(iter, size, where)
    iter.insert(PieceTree::Piece.new(:added, @added.size - size, size, 0), where)
  end

  private :extend_piece, :add_piece

  # ¶ Deleting the contents of point is a rather straightforward procedure.  We
  # must update the first and last fields of point and then decide, based on
  # their position within the buffer, what to do.  If they are equal, i.e.,
  # they are at the same position, we assume that our caller wanted to remove
  # one symbol to the left of point.  Otherwise, we iterate through the pieces
  # between first and last, deleting them as we go along.
  def delete
    update_point(:first)
    update_point(:last)

    if @point.first == @point.last
      piece = @point.last.piece
      if piece.size.zero?
        raise IndexError, "trying to delete beyond end of buffer"
      end
      piece.offset += 1
      piece.size -= 1
      @pieces.size -= 1
      if piece.size == 0
        last = @point.last.dup
        @point.last.prev
        last.delete
      else
        @point.last.fix_size
      end
    else
      pos = @point.first.pos
      until @point.last.pos == pos
        prev = @point.first.dup
        @point.first.next
        prev.delete
      end
    end

    # TODO: this can be delayed to actual access of the range
    @point.range = (@point.first.pos...@point.last.pos)

    return self
  end
 
  # ¶ There are still some things to decide about the semantics of this method.
  # The big question is if we should leave zero||sized pieces in the table, as
  # there can be other data structures pointing to them, or if we, as we do
  # above, delete them when we come across them.  This will all depend on what
  # future work brings to the text editor and will be decided at that time.
  # Until then, we remove them, as this makes for a smaller and tidier table.
  #
  # Another thing to note is that we can actually defer calculating the new
  # range of point and can save some work in the deletion loop by storing the
  # positions and keeping track of how much has been removed.  This way of
  # doing it is a bit more clear and, until some profiling has been done, it
  # stay the way it is.

  # ¶ We now come to the utility functions.  We provide a way for our users to
  # request the size of the buffer through the methods \Ruby{size} and
  # \Ruby{length}:
  def size
    @pieces.size
  end

  alias :length :size

  # ¶ Related to the two methods above, the \Ruby{empty?} method tells our
  # caller whether our size is zero or not.
  def empty?
    size.zero?
  end

  # ¶ For debugging purposes mainly, we sometimes want to get a string
  # representation of a buffer:
  def to_s
    self[0, self.size]
  end

  # ¶ Being able to search our buffers is amongst the most important features
  # that we can provide our users.  The interface to the searching capabilities
  # is simple.  All the user has to do is request a scanner that should begin
  # scanning at a given position and, possibly, end at another position.  This
  # scanner can then be used to search the buffer within these parameters.
  def new_scanner(pos = self.point.end, end_pos = nil)
    if end_pos and pos > end_pos
      raise ArgumentError, "end_pos must be greater than or equal to pos"
    end
    Scanner.new(self, pos, end_pos)
  end

  # ¶ We actually have a use for such a scanner in our buffer class in
  # converting line offsets to true offsets.  The idea is that given a line
  # number, say $5$, we figure out the position of the symbol that begins this
  # line.  (Actually, the line offsets are zero||based, so it’ll be the symbol
  # that begins the next line in this case.).  We use a regular expression that
  # matches lines to do the work for us.  This isn’t going to scale well, but
  # it is simple and straightforward, so we’ll use it for now.
  def line_offset(n)
    raise ArgumentError, "line numbers must be non-negative" if n < 0
    s = new_scanner(0)
    m = PatternMatcher.new("[.*?\\n]<#{n}>")
    matches = s.search(m)
    raise RangeError, "line %d beyond end of buffer" % n if matches.nil?
    matches[0].end
  end

private

  # ¶ Before we get to our Scanner class, let’s write that \Ruby{update_point}
  # method that has been so heavily used in the methods defined above.  As
  # already stated, the idea is to defer creating new pieces for point to point
  # at until we perform an edit.  Once an edit is going to take place, however,
  # we may have to set up new pieces in the piece tree for point to point at,
  # so as to be able to perform the edit in question.  The procedure is
  # somewhat complicated and depends on what end of point we are updating.
  # After figuring out what position to use, we check if the piece at that
  # position needs to be split, i.e., we aren’t at a piece boundary.  If so, we
  # create a new piece, shrink the old one and insert the new one after it.
  def update_point(p)
    return unless @point[p].nil?
    pos = (p == :first) ? @point.range.begin : @point.range.end
    iter = @pieces[pos == $buffer.size ? pos - 1 : pos]
    piece = iter.piece
    doc_pos = iter.pos
    if pos > doc_pos and pos <= doc_pos + piece.size
      right = PieceTree::Piece.new(piece.origin, piece.offset + pos - doc_pos,
                                   piece.size + doc_pos - pos, 0)
      piece.size -= right.size
      @pieces.size -= right.size
      iter.dup.insert(right, :after)
      iter.next
    end
    @point[p] = iter
  end

  # ¶ The Scanner class will be responsible for managing a buffered read method
  # of the buffer it’s associated with.  This read method will then be used by
  # the our pattern||matcher library for fetching input to the automaton.
  class Scanner

    # ¶ Initialization isn’t all that interesting.  The \Ruby{@str} instance
    # variable will act as a cache for reads.  \Ruby{@pos_used} and
    # \Ruby{@base} are used in managing the buffer and keeping track of
    # transforming offsets of matches.
    def initialize(buffer, pos, end_pos)
      @buffer, @pos, @end_pos = buffer, pos, end_pos
      @str = nil
      @base = @pos
    end

    # ¶ The search method itself is straightforward.  It simply invokes the
    # matcher on ourselves, checks for any matches, updates them to correspond
    # to the correct offsets within the editor buffer (as opposed to the cache
    # used by read), and returns the resulting ranges, if any.
    def search(matcher)
      mbase = @pos
      ms = matcher.match(self)
      return nil if ms.nil?
      ret = []
      ms.each{ |m| ret << ((m.begin + mbase)..(m.end + mbase)) }
      @pos = ret[0].end
      ret
    end

    # ¶ The read method is a somewhat more complicated.  The algorithm can be
    # outlined as follows.
    #
    # \startenumerate
    #   \item If we have gone beyond the given limit of our search or the end
    #     of the buffer, we terminate.
    #
    #   \item Otherwise, if any data remains in our cache, we will use it.
    #
    #   \item Given that neither of these cases hold, we fill the buffer with a
    #     fixed amount of new data and advance our position pointer.
    # \stopenumerate
    #
    # This algorithm can be improved by including minimum and maximum lengths
    # of the cache, so that we may concatenate the contents of many short
    # pieces together, while making sure that very long ones don’t get read in
    # all at once into our cache.
    def read
      return nil if (@end_pos and @pos >= @end_pos) or @pos >= @buffer.size

      read_of_str = @pos - @base
      if read_of_str > 0 and read_of_str < @str.length
        @str = @str[read_of_str..-1]
      else
        len = @end_pos ? [4096, @end_pos - @pos].min : 4096
        @str = @buffer[@pos, len]
        @pos += 4096
      end
      @base = @pos

      @str
    end

    attr :pos, true
  end

  # ¶ Our point class is nothing but a structure with three fields, whose roles
  # have already been explained.
  Point = Struct.new(:range, :first, :last)

  # ¶ The original file is represented by the following class.  It uses a
  # somewhat complicated caching||scheme to speed up accesses.  The idea is to
  # always keep a region before and after the last current access point (that
  # didn’t hit the cache) so that future accesses may hit the cache with good
  # probability.  We need to keep a region both before and after, as both
  # access||patterns are likely to occur (think window||scrolling if that makes
  # any sense to you).  The blocks that we read are $2^16$ in size, so we keep
  # a rather large cache around.  Again, this value will be optimized in the
  # future, when more data has been collected regarding its operation.
  class Original
    def initialize(io)
      @io = io
      @buffer = ''
      @mapped_offset = 0
    end

    def [](pos, len = 1)
      ret = ""
      while len > 0
        assure_buffer(pos)
        b = (len > BLOCK_SIZE) ? BLOCK_SIZE : len
        ret << @buffer[pos - @mapped_offset, b]
        len -= b
        pos += b
      end
      ret
    end

  private

    BLOCK_SIZE = 2 << 16

    def calculate_base(offset)
      (offset < BLOCK_SIZE / 2) ?
        0 :
        ((offset + BLOCK_SIZE / 4) & ~(BLOCK_SIZE / 2 - 1)) - BLOCK_SIZE / 2
    end

    def assure_buffer(offset)
      base = calculate_base(offset)
      return unless @buffer.empty? or @mapped_offset != base

      @mapped_offset = base
      @io.seek(@mapped_offset)
      @io.read(BLOCK_SIZE, @buffer)
    end
  end
end
