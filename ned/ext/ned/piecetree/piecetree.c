/*
 * contents: Red-Black Tree backend for our piece table implementation.
 *
 * Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>
 */


#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "piece.h"
#include "node.h"
#include "piecetree.h"
#include "iterator.h"
#include "private.h"


/*¶ And here’s some more boilerplate code: */

#define PIECETREE2VALUE(piecetree)              \
        Data_Wrap_Struct(g_cPieceTree, NULL, piece_tree_free, (piecetree))


HIDDEN VALUE g_cPieceTree;


/*¶ As with everything else related to the Ruby interface, we need a way to
allocate and free piece trees: */

HIDDEN void
piece_tree_free(PieceTree *tree)
{
        node_free(tree->root, true);
        free(tree);
}


static VALUE
piece_tree_s_allocate(UNUSED(VALUE class))
{
        PieceTree *tree = ALLOC(PieceTree);

        tree->root = pt_null;
        tree->size = 0;

        return PIECETREE2VALUE(tree);
}


/*
 * call-seq:
 *      PieceTree.new → tree
 *
 * Create a new PieceTree.
 *
 *      PieceTree.new           ⇒ <PieceTree:0xdeadbeef …>
 */
static VALUE
piece_tree_initialize(VALUE self)
{
        return self;
}

/*¶ Pretty dull reading, eh?  The next two functions that provide accessor
methods for the \C{size} field aren’t much more entertaining.  The function
following them, however, is, so, please, do read on\footnote{Five commas in a
sentence of twelve words.  That must be some kind of record!}. */


/*
 * call-seq:
 *      tree.size → bignum
 *
 * Returns the size/length of _tree_.
 *
 *      tree.size               ⇒ 0
 */
static VALUE
piece_tree_get_size(VALUE self)
{
        PieceTree *tree;

        VALUE2PIECETREE(self, tree);

        return OFFT2NUM(tree->size);
}


/*
 * call-seq:
 *      tree.size = bignum → bignum
 *
 * Sets the size of _tree_.  This method should be used whenever _tree_ is
 * modified, i.e., a piece is added, removed, or altered, so that this number
 * agrees with the actual state of the tree.
 *
 *      tree.size = 10          ⇒ 10
 */
static VALUE
piece_tree_set_size(VALUE self, VALUE rbsize)
{
        off_t size = NUM2OFFT(rbsize);

        if (size < 0)
                rb_raise(rb_eScriptError, "size must be greater than zero");

        PieceTree *tree;

        VALUE2PIECETREE(self, tree);

        tree->size = size;

        return rbsize;
}


/*¶ The most important method of a piece tree object is to create a new
iterator.  As has already been pointed out, all access to the actual tree goes
through the use of an iterator, so this is the only way of modifying the tree.
Anyway, what we do is find the node that contains a given offset and return a
new iterator that points to this node.  It’s rather straightforward actually:
*/

/*
 * call-seq:
 *      tree.new_iter(bignum)   → iter
 *      tree[bignum]            → iter
 *
 * Create a new PieceTree::Iterator that is associated with _tree_ and the
 * Piece that contains the symbol at offset _bignum_ in the sequence.
 *
 *      tree.new_iter(0)        ⇒ <PieceTree::Iterator:0xdeadbeef …>
 */
static VALUE
piece_tree_new_iter(VALUE self, VALUE rbpos)
{
        PieceTree *tree;

        VALUE2PIECETREE(self, tree);

        Node *x = tree->root;

        off_t pos = NUM2OFFT(rbpos);
        if (pos < 0)
                pos += tree->size + 1;
        else if (pos > tree->size)
                rb_raise(rb_eRangeError, "position %d beyond end of buffer",
                         pos);

        while (x != pt_null) {
                if (x->piece->size_left > pos) {
                        x = x->left;
                } else if (x->piece->size_left + x->piece->size > pos) {
                        return ITERATOR2VALUE(iterator_new(self, x));
                } else {
                        pos -= x->piece->size_left + x->piece->size;
                        x = x->right;
                }
        }

        return ITERATOR2VALUE(iterator_new(self, NULL));
}

/*¶ Note how we allow users to specify a negative offset.  Such an offset will
be calculated from the “end” of the buffer.  The algorithm for finding the
given offset is simple.  If the current node’s left child’s size is greater than
the current {\em position}, i.e., the initial offset minus the sizes of any
pieces we have already traversed, then the piece we’re looking for is in the
left sub||tree.  If it isn’t but the size of the left sub||tree plus the size of
the current piece is, then we have found or node and we return it.  If neither
of the previous two conditions hold, it must be in the right sub||tree.  We
must decrement the size of the left sub||tree and the current node from the
current position and then delve into the right sub||tree to find our node. */


/*¶ A Ruby idiom is to always provide an \C{each} method that iterates over a
compound data structure, such as an array or a hash table.  As the piece tree
is such a data structure, we’ll provide an \C{each} method for it. */

static void
_piece_tree_each(Node *node)
{
        if (node == pt_null)
                return;

        _piece_tree_each(node->left);
        rb_yield(PIECE2VALUE(node->piece));
/* TODO: remove tail-call */
        _piece_tree_each(node->right);
}


/*
 * call-seq:
 *      tree.each{ |piece| … } → self
 *
 * Iterate over the Piece's in _tree_.
 *
 *      tree.each{ |piece| p piece}
 *                              ⇒ <PieceTree:0xdeadbeef …>
 */
static VALUE
piece_tree_each(VALUE self)
{
        PieceTree *tree;

        VALUE2PIECETREE(self, tree);

        _piece_tree_each(tree->root);

        return self;
}


/*¶ The last two functions implement the \C{inspect} method and define the Ruby
interface to our piece tree class.  As with the other classes defined so far,
there’s really not much fun to see here and there’s really not much fun to say
about it either. */

/*
 * call-seq:
 *      tree.inspect → string
 *
 * Returns a textual representation of _tree_.  This method is generally called
 * by the Kernel::p method and is used mainly for debugging purposes.
 *
 *      tree.inspect            ⇒ "<PieceTree:0xdeadbeef …>"
 */
static VALUE
piece_tree_inspect(VALUE self)
{
        PieceTree *tree;

        VALUE2PIECETREE(self, tree);

        char buf[INSPECT_BUFFER_SIZE];
        int len = snprintf(buf, INSPECT_BUFFER_SIZE,
                           "#<PieceTree:%p size=%jd>",
                           tree,
                           (intmax_t)tree->size);
        return rb_str_new(buf, len);
}



void Init_piecetree(void);

/*
 * Document-class: PieceTree
 *
 * A PieceTree is an efficient data structure for representing a sequence.
 */
void
Init_piecetree(void)
{
        g_cPieceTree = rb_define_class("PieceTree", rb_cData);
        rb_define_alloc_func(g_cPieceTree, piece_tree_s_allocate);
        rb_define_private_method(g_cPieceTree, "initialize",
                                 piece_tree_initialize, 0);

        rb_include_module(g_cPieceTree, rb_mEnumerable);

        rb_define_method(g_cPieceTree, "new_iter", piece_tree_new_iter, 1);
        rb_define_method(g_cPieceTree, "[]", piece_tree_new_iter, 1);
        rb_define_method(g_cPieceTree, "size", piece_tree_get_size, 0);
        rb_define_method(g_cPieceTree, "size=", piece_tree_set_size, 1);
        rb_define_method(g_cPieceTree, "each", piece_tree_each, 0);
        rb_define_method(g_cPieceTree, "inspect", piece_tree_inspect, 0);

        Init_Piece(g_cPieceTree);
        Init_Iterator();
}
