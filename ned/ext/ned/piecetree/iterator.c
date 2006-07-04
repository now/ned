/*
 * contents: PieceTree::Iterator class.
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
#include "node.h"
#include "piecetree.h"
#include "iterator.h"
#include "private.h"

/*¶ In the internal functions of an iterator we often need to extract an
iterator from a Ruby value and then verify that it’s valid, thus */

#define SETUP_VALID_ITERATOR(self, iter) \
        Iterator *iter;                  \
        VALUE2ITERATOR(self, iter);      \
        if (!iterator_is_valid(iter))   \
                rb_raise(rb_eScriptError, "iterator has moved outside of tree");

/*¶ does this job for us. */


/*¶ We will often need to tell if the node that a given iterator points to is
the left or right child of its parent, assuming it has one: */

#define is_left_child(node)   ((node) == (node)->parent->left)
#define is_right_child(node)  ((node) == (node)->parent->right)

/*¶ And now, here’s the iterator structure itself: */

struct _Iterator {
        VALUE tree;
        Node *node;
};


HIDDEN VALUE g_cIterator;



/*¶ Next, we define three functions that allocate, mark, and free iterators.
We need a {\em mark} function, as each iterator will belong to a certain piece
tree, and we don’t want ruby to free that tree before our user’s finished with
all iterators pointing to that tree. */

HIDDEN Iterator *
iterator_new(VALUE tree, Node *node)
{
        Iterator *iter = ALLOC(Iterator);

        iter->tree = tree;
        iter->node = node;

        return iter;
}


HIDDEN void
iterator_mark(Iterator *iter)
{
        rb_gc_mark(iter->tree);
}


HIDDEN void
iterator_free(Iterator *iter)
{
        free(iter);
}


/*¶ A related function is one that duplicates a given iterator: */

/*
 * call-seq:
 *      iter.dup → iter
 *
 * Create a deep copy of _iter_.  This is often necessary, as most operations
 * on an iterator are destructive.
 *
 *      iter.dup                ⇒ <PieceTree::Iterator:0xdeadbeef …>
 */
static VALUE
iterator_dup(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);

        return ITERATOR2VALUE(iterator_new(iter->tree, iter->node));
}

/*¶ Duplication is rather straightforward, as you can see. */


/*¶ Being able to move an iterator within a piece tree is an important
functionality of iterators.  Before an iterator can be moved, however, we need
to check that we have somewhere to move to.  The next function performs this
check for moving an iterator forward in the tree: */

/*
 * call-seq:
 *      iter.has_next? → bool
 *
 * Returns +true+ if _iter_ is #valid? and calling #next on it will
 * result in an iterator that is still #valid?.
 *
 *      iter.has_next?          ⇒ true
 *      iter.next while iter.has_next?; iter.has_next?
 *                              ⇒ false
 */
static VALUE
iterator_has_next_p(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);

        return BOOL2VALUE(iter->node != pt_null && node_next(iter->node) != NULL);
}


/*¶ Now, once we have checked that an iterator is movable forward, we can move
it: */

/*
 * call-seq:
 *      iter.next → self
 *
 * Advances _iter_ to point to the next Piece in the PieceTree.  A ScriptError
 * is thrown if _iter_ isn't #valid?.
 *
 *      iter                    ⇒ <PieceTree::Iterator:0xdeadbeef …>
 *      iter.next               ⇒ <PieceTree::Iterator:0xdeadbeef …>
 *      !iter.has_next? && iter.next
 *                              ⇒ raise ScriptError, "…"
 */
static VALUE
iterator_next(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);
        iter->node = node_next(iter->node);

        return self;
}


/*¶ The next two functions implements basically the same functionality as the
previous two, but in the other direction: */

/*
 * call-seq:
 *      iter.has_prev? → bool
 *
 * Returns +true+ if _iter_ is #valid? and calling #prev on it will
 * result in an iterator that is still #valid?.
 *
 *      iter.has_prev?          ⇒ true
 *      iter.prev while iter.has_prev?; iter.has_prev?
 *                              ⇒ false
 */
static VALUE
iterator_has_prev_p(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);
        
        return BOOL2VALUE(iter->node != pt_null && node_prev(iter->node) != NULL);
}


/*
 * call-seq:
 *      iter.prev → self
 *
 * Advances _iter_ to point to the previous Piece in the PieceTree.
 *
 * Raises a ScriptError if _iter_ isn't #valid?.
 *
 *      iter                    ⇒ <PieceTree::Iterator:0xdeadbeef …>
 *      iter.prev               ⇒ <PieceTree::Iterator:0xdeadbeef …>
 *      !iter.has_prev? && iter.prev
 *                              ⇒ raise ScriptError, "…"
 */
static VALUE
iterator_prev(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);
        iter->node = node_prev(iter->node);

        return self;
}


/*¶ We have already seen the \C{iterator_is_valid} function being used to check
that an iterator is valid.  Here’s the test that implements that function: */

/* Internal function to check if an iterator is valid.  An iterator is valid if
 * it points to an actual node. */
static inline bool
iterator_is_valid(Iterator const * const iter)
{
        return iter->node != NULL;
}


/*¶ As we will want to check that iterators are valid on the Ruby side as well,
here’s a wrapper for the above function: */

/*
 * call-seq:
 *      iter.valid? → bool
 *
 * Return +true+ if the iterator is valid.  An iterator is valid if it is still
 * within the tree into which it points.   An invalid iterator will return
 * +false+ for #has_prev? or #has_next? depending on what end the iterator has
 * gone beyond.
 *
 *      iter.valid?             ⇒ true
 */
static VALUE
iterator_valid_p(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);

        return BOOL2VALUE(iterator_is_valid(iter));
}


/*¶ We would like to extract the piece that the node that this iterator points
to houses: */

/*
 * call-seq:
 *      iter.piece → piece
 *
 * Returns the piece that the iterator is currently on.
 *
 *      iter.piece              ⇒ <PieceTree::Piece:0xdeadbeef …>
 */
static VALUE
iterator_get_piece(VALUE self)
{
        SETUP_VALID_ITERATOR(self, iter);

        return PIECE2VALUE(iter->node->piece);
}


/*¶ We would also like a way to tell the position of a node within a buffer,
i.e., the offset within the buffer at which this iterator’s node begins.  This
is a calculated value, taking $\Ordo{\lg n} $ time, so it should be used
sparingly. */

/*
 * call-seq:
 *      iter.pos → bignum
 *
 * Returns the position/offset of the iterator in the PieceTree to which it
 * belongs.  This is an O(lg _n_) operation, so use it sparingly.
 *
 *      iter.pos                ⇒ 0
 *      iter.next; iter.pos     ⇒ 12345678
 */
static VALUE
iterator_get_pos(VALUE self)
{
        SETUP_VALID_ITERATOR(self, iter);

        PieceTree *tree;
        VALUE2PIECETREE(iter->tree, tree);

        off_t pos = iter->node->piece->size_left;

        for (Node *p = iter->node; p != tree->root; p = p->parent)
                if (is_right_child(p))
                        pos += p->parent->piece->size_left +
                                p->parent->piece->size;

        return OFFT2NUM(pos);
}


/*¶ Now that we can get the position of iterators, we can also compare them: */

/*
 * call-seq:
 *      iter <=> other_iter → bignum
 *
 * Returns a positive value if _other_iter_’s position in the PieceTree is
 * smaller than _iter_’s.  Returns a negative value if their relation is
 * reversed.  Returns zero if their positions are equal.
 *
 * An ArgumentError is raised if the PieceTree::Iterator’s point to different
 * trees.
 *
 * This operation is O(lg _n_), as the position of both iterators in the tree
 * must be calculated (see #pos for details), so use it sparingly.
 *
 *      tree[12345678] <=> tree[0]
 *                              ⇒ 12345678
 *      tree[0] <=> tree[0]     ⇒ 0
 *      tree[0] <=> tree[12345678]
 *                              ⇒ -12345678
 */
static VALUE
iterator_cmp(VALUE self, VALUE other)
{
        Iterator *a, *b;

        VALUE2ITERATOR(self, a);
        VALUE2ITERATOR(other, b);

        if (a->tree != b->tree)
                rb_raise(rb_eArgError, "iterators must be from same tree");

        return OFFT2NUM(NUM2OFFT(iterator_get_pos(self)) -
                        NUM2OFFT(iterator_get_pos(other)));
}

/*¶ This isn’t very useful, actually, but it’s simple enough to implement and
it doesn’t hurt anyone to do it either. */


/*¶ The next couple of functions are related to fixing up the structure of a
red||black tree and the sizes of the nodes within it.  We begin with a simple
function that calculates the total size of a (sub||)tree: */

static off_t
calculate_size(Node const * const node)
{
        off_t size = 0;

        for (Node const *p = node; p != pt_null; p = p->right)
                size += p->piece->size_left + p->piece->size;

        return size;
}


/*¶ Whenever we alter the size of a piece, all pieces in parent nodes to the
node containing that piece will need their \C{size_left} field updated to agree
with the new size.  This is a rather straightforward procedure, but still needs
a bit of fiddling to get it right: */

static void
fix_size(Node *node, Node *root)
{
        if (node == root)
                return;

        off_t delta = 0;
        if (node->parent->left == node->parent->right &&
            node->parent->piece != NULL) {
                node = node->parent;
                delta = -node->piece->size_left;
                node->piece->size_left = 0;
        }

        if (delta == 0) {
                while (node != root && is_right_child(node))
                        node = node->parent;

                if (node == root)
                        return;

                node = node->parent;
                delta = calculate_size(node->left) - node->piece->size_left;
                node->piece->size_left += delta;
        }

        if (delta != 0)
                for ( ; node != root; node = node->parent)
                        if (is_left_child(node))
                                node->parent->piece->size_left += delta;
}


/*¶ We wrap up the previous function in a Ruby binding as well: */

/*
 * call-seq:
 *      iter.fix_size → self
 *
 * If the size of _iter_’s #piece has changed we must update the PieceTree that
 * it’s stored in.  Otherwise, the tree will be in an undefined state.
 *
 * Raises a ScriptError if _iter_ isn't #valid?.
 *
 *      iter.fix_size           ⇒ <PieceTree::Iterator:0xdeadbeef …>
 */
static VALUE
iterator_fix_size(VALUE self)
{
        SETUP_VALID_ITERATOR(self, iter);

        PieceTree *tree;
        VALUE2PIECETREE(iter->tree, tree);

        fix_size(iter->node, tree->root);

        return self;
}


/*¶ The next thing that needs to be done for updates to the tree is to perform
rotations so that the red||black properties will still hold afterward. */

/*¶ Firstly, we need a function that sets up things so that two nodes may be
swapped easily: */

static void
set_new_child(PieceTree *tree, Node *x, Node *y)
{
        y->parent = x->parent;

        if (x->parent == NULL)
                tree->root = y;
        else if (is_left_child(x))
                x->parent->left = y;
        else
                x->parent->right = y;
}

/*¶ This function updates the parent of the node $x$ so that it instead will
have $y$ as a child. */


/*¶ Now that we have our child||switching function, we can write a function for
rotating a tree leftwards. */

static void
rotate_left(PieceTree *tree, Node *x)
{
        Node *y = x->right;

        y->piece->size_left += x->piece->size + x->piece->size_left;

        x->right = y->left;

        if (y->left != pt_null)
                y->left->parent = x;

        set_new_child(tree, x, y);

        y->left = x;
        x->parent = y;
}

/*¶ \infigure[figure:piecetree:rotate left]\ gives a graphical representation
of what’s going on when we rotate to the left.  The node $x$ in our algorithm
is the node labeled $N_2$ in the figure.

\placedescribedfigure
  []
  [figure:piecetree:rotate left]
  {Rotating a red||black tree to the left.  In this set||up, $N_2$ and $N_4$
   are internal nodes, and $N_1$, $N_3$, and $N_5$ are sub||trees.  After the
   rotation, $N_2$ and $N_4$ will have swapped roles, as $N_4$ is now the
   parent of $N_2$ and $N_2$ will have $N_4$’s left child as its right child
   (keeping the relationship between parent and child intact).}
  {\startcombination[2*1]
     {\externalfigure[source:piecetree:rotate left (before)]} {(a) Before}
     {\externalfigure[source:piecetree:rotate left (after)]} {(b) After}
   \stopcombination}
*/


/*¶ We also need to be able to rotate rightwards.  The procedure is shown in
\infigure[figure:piecetree:rotate right]\ and is analogous to the procedure of
rotating to the left.

\placedescribedfigure
  []
  [figure:piecetree:rotate right]
  {Rotating a red||black tree to the right.  In this set||up, $N_2$ and $N_4$
   are internal nodes, and $N_1$, $N_3$, and $N_5$ are sub||trees.  After the
   rotation, $N_4$ and $N_2$ will have swapped roles, as $N_2$ is now the
   parent of $N_4$ and $N_4$ will have $N_2$’s right child as its left child
   (keeping the relationship between parent and child intact).}
  {\startcombination[2*1]
     {\externalfigure[source:piecetree:rotate right (before)]} {(a) Before}
     {\externalfigure[source:piecetree:rotate right (after)]} {(b) After}
   \stopcombination}
*/

static void
rotate_right(PieceTree *tree, Node *x)
{
        Node *y = x->left;

        x->piece->size_left -= y->piece->size + y->piece->size_left;
        
        x->left = y->right;

        if (y->right != pt_null)
                y->right->parent = x;

        set_new_child(tree, x, y);

        y->right = x;
        x->parent = y;
}

/*¶ The node $x$ is the node labeled $N_4$ in the figure. */

/*¶ Now that we have our two rotating functions, we define the type of such
functions so that we can use them as values in the next function. */

typedef void (*RotateFunc)(PieceTree *, Node *);


/*¶ What we want to do is that whenever a node is inserted, we need to update
the tree so that the properties of a red||black tree are maintained within the
tree as a whole.  We won’t go into the details of this so, without further ado,
here’s the code: */

static Node *
insert_fixup_node(PieceTree *tree, Node *x)
{
        bool left = is_left_child(x->parent);

        RotateFunc rotate_parent = left ? rotate_left : rotate_right;
        RotateFunc rotate_parent_parent = left ? rotate_right : rotate_left;

        Node *y = left ? x->parent->parent->right : x->parent->parent->left;

        if (y != NULL && y->color == RED) {
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
        } else {
                bool is_child = left ? is_right_child(x) : is_left_child(x);

                if (is_child) {
                        x = x->parent;
                        rotate_parent(tree, x);
                }

                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotate_parent_parent(tree, x->parent->parent);
        }

        return x;
}


static void
insert_fixup(PieceTree *tree, Node *x)
{
        assert(x != NULL);

        fix_size(x, tree->root);

        while (x != tree->root && x->parent->color == RED)
                x = insert_fixup_node(tree, x);

        tree->root->color = BLACK;
}


/*¶ Before we can write a function that will insert new pieces in our piece
tree, we need a function that turns a Ruby symbol into a boolean saying whether
an insertion should be to the left (before) or right (after) the node that the
iterator is pointing at. */

static bool
where_to_is_left(VALUE where)
{
        static ID id_before = 0;
        static ID id_after = 0;

        if (id_before == 0)
                id_before = rb_intern("before");
        if (id_after == 0)
                id_after = rb_intern("after");

        if (!SYMBOL_P(where))
                rb_raise(rb_eTypeError, "not a symbol");

        ID id = SYM2ID(where);
        if (id == id_before)
                return true;
        else if (id == id_after)
                return false;
        else
                rb_raise(rb_eArgError, "unknown symbol");
}


/*¶ Now, then, here’s how we insert a new piece through an iterator: */

/*
 * call-seq:
 *      iter.insert(piece, { :before, :after}) → self
 *
 * Insert _piece_ before or after the piece that _iter_ points to.
 *
 * Raises a ScriptError if _iter_ isn't #valid? and the PieceTree into which it
 * points isn't empty.
 *
 *      iter.insert(PieceTree::Piece.new(…), :before)
 *                              ⇒ <PieceTree::Iterator:0xdeadbeef …>
 */
static VALUE
iterator_insert(VALUE self, VALUE rbpiece, VALUE where)
{
        Iterator *iter;
        Piece *piece;
        PieceTree *tree;

        VALUE2ITERATOR(self, iter);
        VALUE2PIECE(rbpiece, piece);
        VALUE2PIECETREE(iter->tree, tree);

        if (tree->root != pt_null && !iterator_is_valid(iter))
                rb_raise(rb_eScriptError, "iterator has moved outside of tree");

        bool left = where_to_is_left(where);

        piece->size_left = 0;

        tree->size += piece->size;

        Node *new_node = node_new(pt_null, pt_null, NULL, RED, piece);
        Node *node = iter->node;
        Node **child = NULL;
        if (!iterator_is_valid(iter)) {
                iter->node = tree->root = new_node;
        } else if (left ? node->left == pt_null : node->right == pt_null) {
                child = left ? &node->left : &node->right;
        } else {
                iter->node = left ? node_prev(node) : node_next(node);
                assert(iterator_is_valid(iter));

                node = iter->node;
                assert(left ? node->right == pt_null : node->left == pt_null);

                child = left ? &node->right : &node->left;
        }

        if (child != NULL) {
                *child = new_node;
                new_node->parent = node;
        }

        insert_fixup(tree, new_node); 

        return self;
}

/*¶ If our tree is empty, the iterator will in fact be invalid.  This is fine,
though, and we will set the root of the tree to point to the new node
containing the given piece.  We also update the iterator to point to the new
node so that it becomes valid.  Otherwise, we check if the iterator’s node’s
left or right child (depending on where our user wants to enter the new piece)
is empty.  If so, we’ll enter our new node there.  Otherwise, we find the 
the iterator’s previous or next neighbor and will insert it there instead. */


/*¶ When deleting pieces from the piece tree, we must do more or less the same
fixup routine as when inserting new pieces.
% TODO: expand on this
*/

static Node *
delete_fixup_node(PieceTree *tree, Node *x)
{
        bool left = is_left_child(x);

        RotateFunc rotate_parent = left ? rotate_left : rotate_right;
        RotateFunc rotate_y = left ? rotate_right : rotate_left;

        Node *y = left ? x->parent->right : x->parent->left;

        if (y->color == RED) {
                y->color = BLACK;
                x->parent->color = RED;
                rotate_parent(tree, x->parent);
                y = left ? x->parent->right : x->parent->left;
        }

        if (y->left->color == BLACK && y->right->color == BLACK) {
                y->color = RED;
                x = x->parent;
        } else {
                if ((left ? y->right->color : y->left->color) == BLACK) {
                        if (left)
                                y->left->color = BLACK;
                        else
                                y->right->color = BLACK;
                        y->color = RED;
                        rotate_y(tree, y);
                        y = left ? x->parent->right : x->parent->left;
                }

                y->color = x->parent->color;
                x->parent->color = BLACK;
                if (left)
                        y->right->color = BLACK;
                else
                        y->left->color = BLACK;
                rotate_parent(tree, x->parent);
                x = tree->root;
        }

        return x;
}


static void
delete_fixup(PieceTree *tree, Node *x)
{
        assert(x != NULL);

        while (x != tree->root && x->color == BLACK)
                x = delete_fixup_node(tree, x);

        x->color = BLACK;
}


/*¶ Deleting the node itself is quite a complex procedure actually, and quite
some code is needed to get it right: */

/*
 * call-seq:
 *      iter.delete → self
 *
 * Delete the piece at which _iter_ points.
 *
 *      iter.delete             ⇒ <PieceTree::Iterator:0xdeadbeef>
 */
/* TODO: should we throw an error on invalid iterators?  Should this stuff be
 * reference counted? */
static VALUE
iterator_delete(VALUE self)
{
        Iterator *iter;

        VALUE2ITERATOR(self, iter);

        if (!iterator_is_valid(iter))
                return self;

        PieceTree *tree;
        VALUE2PIECETREE(iter->tree, tree);

        tree->size -= iter->node->piece->size;

        Node *y = (iter->node->left == pt_null || iter->node->right == pt_null)
                ? iter->node : node_prev(iter->node);
        assert(y->left == pt_null || y->right == pt_null);

        Node *son = (y->left != pt_null) ? y->left : y->right;
        assert(son != NULL);

        set_new_child(tree, y, son);

        if (y != iter->node) {
                free(iter->node->piece);
                iter->node->piece = y->piece;
                fix_size(iter->node, tree->root);
        }

        if (son->parent != NULL)
                fix_size(son, tree->root);

        if (y->color == BLACK)
                delete_fixup(tree, son);

        node_free(y, false);

        return self;
}


/*¶ The last part of the iterator||related code is an inspect method and a
function for setting up the iterator class.  There’s really nothing interesting to say
about it actually, so let’s just leave it be: */

/*
 * call-seq:
 *      iter.inspect → string
 *
 * Returns a textual representation of _iter_.  This method is generally called
 * by the Kernel::p method and is used mainly for debugging purposes.
 *
 *      iter.inspect            ⇒ "<PieceTree::Iterator:0xdeadbeef …>"
 */
static VALUE
iterator_inspect(VALUE self)
{
        Iterator *iter;
        PieceTree *tree;

        VALUE2ITERATOR(self, iter);
        VALUE2PIECETREE(iter->tree, tree);

        char buf[INSPECT_BUFFER_SIZE];
        int len;
        if (iterator_is_valid(iter)) {
                len = snprintf(buf, INSPECT_BUFFER_SIZE,
                               "#<PieceTree::Iterator:%p tree=%p "
                               "piece=%p pos=%jd>",
                               iter,
                               tree,
                               iter->node->piece,
                               (intmax_t)NUM2OFFT(iterator_get_pos(self)));
        } else {
                len = snprintf(buf, INSPECT_BUFFER_SIZE,
                               "#<PieceTree::Iterator:%p tree=%p (invalid)>",
                               iter,
                               tree);
        }

        return rb_str_new(buf, len);
}


/*
 * Document-class: PieceTree::Iterator
 *
 * A PieceTree::Iterator stores a location in a PieceTree.  Use a
 * PieceTree::Iterator whenever you are going to walk the PieceTree in some
 * way, e.g., performing a search or similar.  A PieceTree::Iterator more or
 * less acts as a very simple cache.
 *
 * Most operations on a PieceTree::Iterator are O(1).  For operations that
 * aren't, their asymptotic property is given; it is also worth noting that _n_
 * is the number of PieceTree::Piece’s in the PieceTree.
 *
 * A PieceTree::Iterator can only be created by calling PieceTree#[] or
 * PieceTree#new_iter.
 */
/* TODO: Decide whether delete should be available or not. */
void HIDDEN
Init_Iterator(void)
{
        g_cIterator = rb_define_class_under(g_cPieceTree, "Iterator", rb_cData);
        rb_undef_method(CLASS_OF(g_cIterator), "new");

        rb_include_module(g_cIterator, rb_mComparable);

        rb_define_method(g_cIterator, "dup", iterator_dup, 0);
        rb_define_method(g_cIterator, "has_next?", iterator_has_next_p, 0);
        rb_define_method(g_cIterator, "next", iterator_next, 0);
        rb_define_method(g_cIterator, "has_prev?", iterator_has_prev_p, 0);
        rb_define_method(g_cIterator, "prev", iterator_prev, 0);
        rb_define_method(g_cIterator, "valid?", iterator_valid_p, 0);
        rb_define_method(g_cIterator, "piece", iterator_get_piece, 0);
        rb_define_method(g_cIterator, "pos", iterator_get_pos, 0);
        rb_define_method(g_cIterator, "<=>", iterator_cmp, 1);
        rb_define_method(g_cIterator, "fix_size", iterator_fix_size, 0);
        rb_define_method(g_cIterator, "insert", iterator_insert, 2);
        rb_define_method(g_cIterator, "delete", iterator_delete, 0);
        rb_define_method(g_cIterator, "inspect", iterator_inspect, 0);
}
