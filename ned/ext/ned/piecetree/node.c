/*
 * contents: PieceTree::Node class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>

#include "piece.h"
#include "node.h"
#include "private.h"


HIDDEN Node g_null_node = {
        &g_null_node, &g_null_node, &g_null_node, BLACK, NULL
};


/*¶ We need functions that create and destroy nodes for us: */

HIDDEN Node *
node_new(Node *left, Node *right, Node *parent, NodeColor color, Piece *piece)
{
        Node *node = ALLOC(Node);

        node->left = left;
        node->right = right;
        node->parent = parent;
        node->color = color;
        node->piece = piece;

        return node;
}


/*¶ The \C{node_free} function is a bit more interesting.  We need to decide
whether we want to free only this node, or this node and the piece it holds
plus its children: */

HIDDEN void
node_free(Node *node, bool deep)
{
        if (node == pt_null)
                return;

        if (deep) {
                node_free(node->right, true);
                node_free(node->left, true);

                free(node->piece);
        }

        free(node);
}


/*¶ The final two functions that operate directly on nodes figure out what node
is right before or right after a given node in the tree. */

/* TODO: the error-message isn't quite right */
HIDDEN Node *
node_prev(Node *node)
{
        if (node == NULL)
                rb_raise(rb_eScriptError, "moving outside of tree");

        if (node == pt_null)
                return node;

        if (node->left != pt_null) {
                node = node->left;
                while (node->right != pt_null)
                        node = node->right;

                return node;
        } else {
                for ( ; node->parent != NULL; node = node->parent)
                        if (node->parent->right == node)
                                return node->parent;

                return NULL;
        }
}

/*¶ The algorithm is rather straightforward.  If we have a left child, the
“previous” node must be the node furthest to the right within the sub||tree
that begins with that node.  Otherwise, the first parent within which this node
is part of the right sub||tree is the node laying right before this one. */


/*¶ The algorithm, and thus the code, for the other case is analogous: */

HIDDEN Node *
node_next(Node *node)
{
        if (node == NULL)
                rb_raise(rb_eScriptError, "moving outside of tree");

        if (node == pt_null)
                return node;

        if (node->right != pt_null) {
                node = node->right;
                while (node->left != pt_null)
                        node = node->left;

                return node;
        } else {
                for ( ; node->parent != NULL; node = node->parent)
                        if (node->parent->left == node)
                                return node->parent;

                return NULL;
        }
}
