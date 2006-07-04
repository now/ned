/*
 * contents: PieceTree::Node class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \subsection{Dealing with red||black tree nodes.}

Now that we have our pieces, let’s define the nodes of the red||black trees
that will house them. */

/*¶ A red||black tree node in our piece||tree will have a left, right, and
parent node, a color (red or black), and a piece associated with it.  The left
and right nodes are the children of the node and may point to a special “null”
value.  The parent is the node that this node is a child of, or \C{NULL} if
this node is the root of the tree.  The rest of the fields should be obvious. */

typedef enum {
        BLACK,
        RED
} NodeColor;


typedef struct _Node Node;

struct _Node {
        Node *left;
        Node *right;
        Node *parent;
        unsigned int color : 1;
        Piece *piece;
};


/*¶ Let’s now define the special “null” value that was just mentioned.  It will
be a pointer to a node that has its field set up so that it acts like a perfect
sentinel for our data structure: */

extern Node g_null_node;
#define pt_null (&g_null_node)

/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


Node *node_new(Node *left, Node *right, Node *parent, NodeColor color, Piece *piece);
void node_free(Node *node, bool deep);
Node *node_prev(Node *node);
Node *node_next(Node *node);
