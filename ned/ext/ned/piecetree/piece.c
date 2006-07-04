/*
 * contents: PieceTree::Piece Class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#include "piece.h"
#include "private.h"


/*¶ Actually, we will need some more “Ruby to \CLang\ and back”||related
code: */

static ID s_id_original;
static ID s_id_added;

HIDDEN VALUE g_cPiece;


static PieceOrigin
symbol_to_origin(VALUE symbol)
{
        if (!SYMBOL_P(symbol))
                rb_raise(rb_eTypeError, "not a symbol");

        ID id = SYM2ID(symbol);
        if (id == s_id_original)
                return ORIGINAL;
        else if (id == s_id_added)
                return ADDED;
        else
                rb_raise(rb_eArgError, "unknown symbol");
}

/*¶ This function turns a symbol into a piece||origin as appropriate; the next
does the exact opposite: */

static VALUE
origin_to_id(PieceOrigin origin)
{
        switch (origin) {
        case ORIGINAL:
                return s_id_original;
        case ADDED:
                return s_id_added;
        default:
                assert(false);
                return Qnil;
        }
}


/*¶ How highly uninteresting.  This next function isn’t much better: */

static VALUE
piece_s_allocate(UNUSED(VALUE class))
{
        Piece *piece = ALLOC(Piece);

        piece->origin = ORIGINAL;
        piece->offset = 0;
        piece->size = 0;
        piece->size_left = 0;

        return PIECE2VALUE(piece);
}

/*¶ Yes, that’s right, all it does is allocate and sets up a new piece. */


/*¶ As the default values aren’t that useful, the initialization code takes a
few arguments and sets up a more interesting piece: */

/*
 * call-seq:
 *      Piece.new(origin, offset, size, size_left) → piece
 *
 * Create a new piece.
 *
 *      Piece.new(:original, 0, 1, 0)
 *                              ⇒ <PieceTree::Piece:0xdeadbeef …>
 *
 * TODO: make this a lot easier to create and perhaps even make it a true Ruby
 * object.
 */
static VALUE
piece_initialize(VALUE self, VALUE rborigin, VALUE rboffset, VALUE rbsize,
                 VALUE rbsize_left)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        piece->origin = symbol_to_origin(rborigin);
        piece->offset = NUM2OFFT(rboffset);
        piece->size = NUM2OFFT(rbsize);
        piece->size_left = NUM2OFFT(rbsize_left);

        return self;
}


/*¶ The next couple of functions define accessors for the fields of a piece
data structure and will not be discussed in any detail. */

/*
 * call-seq:
 *      piece.origin → { :original, :added }
 *
 * Retrieve the origin of _piece_.  The origin is either :original, for a piece
 * pointing into the original file, or :added, for a piece pointing into the
 * add-file.
 *
 *      piece.origin            ⇒ :original
 */
static VALUE
piece_get_origin(VALUE self)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        return ID2SYM(origin_to_id(piece->origin));
}


/*
 * call-seq:
 *      piece.offset → bignum
 *
 * Retrieve the offset of _piece_.  The offset of a piece is the offset from
 * the beginning of the file into which it points, which is not necessarily the
 * offset within the PieceTree in which it resides.
 *
 *      piece.offset            ⇒ 0
 */
static VALUE
piece_get_offset(VALUE self)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        return OFFT2NUM(piece->offset);
}


/*
 * call-seq:
 *      piece.offset = bignum → bignum
 *
 * Set the offset of _piece_ to _bignum_ and return it.  See the #offset method
 * for a longer discussion on what the offset of a piece means.
 *
 *      piece.offset = 5        ⇒ 5
 */
static VALUE
piece_set_offset(VALUE self, VALUE offset)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        piece->offset = NUM2OFFT(offset);

        return offset;
}


/*
 * call-seq:
 *      piece.size → bignum
 *
 * Retrieve the size (in bytes) of _piece_.
 *
 *      piece.size              ⇒ 1
 *
 * TODO: this shouldn't be in bytes but in symbols.  There will have to be a
 * byte_size method later on.
 */
static VALUE
piece_get_size(VALUE self)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        return OFFT2NUM(piece->size);
}


/*
 * call-seq:
 *      piece.size = bignum → bignum
 *
 * Set the size of _piece_ (in bytes) to _bignum_ and return it.
 *
 *      piece.size = 10         ⇒ 10
 */
static VALUE
piece_set_size(VALUE self, VALUE size)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        piece->size = NUM2OFFT(size);

        return size;
}


/*
 * call-seq:
 *      piece.size → bignum
 *
 * Retrieve the size_left (in bytes) of _piece_.
 *
 *      piece.size_left         ⇒ 0
 */
static VALUE
piece_get_size_left(VALUE self)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        return OFFT2NUM(piece->size_left);
}


/*
 * call-seq:
 *      piece.size_left = bignum → bignum
 *
 * Set the size_left of _piece_ (in bytes) to _bignum_ and return it.
 *
 *      piece.size_left = 10    ⇒ 10
 *
 * TODO: this will actually be ripped out of the Piece class and moved into
 * something internal instead.
 */
static VALUE
piece_set_size_left(VALUE self, VALUE size_left)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        piece->size_left = NUM2OFFT(size_left);

        return size_left;
}


/*¶ As with all data structures accessible from Ruby, we define a function to
inspect the contents of such a structure: */

/*
 * call-seq:
 *      piece.inspect → string
 *
 * Return a string representation of the piece.  Used by Kernel::p and mainly
 * for debugging purposes.
 *
 *      piece.inspect           ⇒ "<PieceTree::Piece:0xdeadbeef …>"
 */
static VALUE
piece_inspect(VALUE self)
{
        Piece *piece;

        VALUE2PIECE(self, piece);

        char buf[INSPECT_BUFFER_SIZE];
        int len = snprintf(buf, INSPECT_BUFFER_SIZE,
                           "#<PieceTree::Piece:%p origin=:%s offset=%jd "
                           "size=%jd size_left=%jd>",
                           piece,
                           rb_id2name(origin_to_id(piece->origin)),
                           (intmax_t)piece->offset,
                           (intmax_t)piece->size,
                           (intmax_t)piece->size_left);
        return rb_str_new(buf, len);
}


/*¶ Finally, we need a function to set up the Ruby side of the interface.
Again, this isn’t exactly interesting reading: */

/*
 * Document-class: PieceTree::Piece
 *
 * A PieceTree::Piece represents a piece of the sequence represented by the
 * PieceTree.  It's more or less just a descriptor containing the necessary
 * data.
 */
void HIDDEN
Init_Piece(VALUE g_cPieceTree)
{
        s_id_original = rb_intern("original");
        s_id_added = rb_intern("added");

        g_cPiece = rb_define_class_under(g_cPieceTree, "Piece", rb_cData);
        rb_define_alloc_func(g_cPiece, piece_s_allocate);
        rb_define_private_method(g_cPiece, "initialize", piece_initialize, 4);

        rb_include_module(g_cPiece, rb_mComparable);

        rb_define_method(g_cPiece, "origin", piece_get_origin, 0);
        rb_define_method(g_cPiece, "offset", piece_get_offset, 0);
        rb_define_method(g_cPiece, "offset=", piece_set_offset, 1);
        rb_define_method(g_cPiece, "size", piece_get_size, 0);
        rb_define_method(g_cPiece, "size=", piece_set_size, 1);
        rb_define_method(g_cPiece, "size_left", piece_get_size_left, 0);
        rb_define_method(g_cPiece, "size_left=", piece_set_size_left, 1);
        rb_define_method(g_cPiece, "inspect", piece_inspect, 0);
}
