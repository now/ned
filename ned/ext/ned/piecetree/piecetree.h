/*
 * contents: Red-Black Tree backend for our piece table implementation.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */


/*¶ \subsection{It’s time for piece trees.}

We have finally arrived at our piece tree data structure’s own definition.
Before we get started on the details, however, we must write some code that is,
by now, all but boiler||plate: */

#define VALUE2PIECETREE(value, piecetree)       \
        Data_Get_Struct((value), PieceTree, (piecetree))


typedef struct _PieceTree PieceTree;

struct _PieceTree {
        Node *root;
        off_t size;
};

/*¶ The \C{size} is simply the sum of all the sizes of the pieces in the
tree. */


extern VALUE g_cPieceTree;

/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


void piece_tree_free(PieceTree *tree);
