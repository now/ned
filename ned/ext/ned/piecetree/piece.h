/*
 * contents: PieceTree::Piece Class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{The Piece||Tree Data Structure}

In this section we’ll cover the data structure that maintains the contents of
buffers in our text editor.  It is implemented using the piece||table strategy
discussed in \insection[text editors - internal:piece table buffering strategy]
with a red||black tree maintaining the pieces.
*/

/*¶ \subsection{The pieces of our data||structure puzzle.}

Appropriate to such a horrible section||title, the data structure that
represents pieces in our piece table is also rather dull: */

typedef enum {
        ORIGINAL,
        ADDED
} PieceOrigin;

typedef struct _Piece Piece;

struct _Piece {
        PieceOrigin origin;
        off_t offset;
        off_t size;
        off_t size_left;
};

/*¶ The \C{origin} defines in what location this piece resides.  The \C{offset}
is the offset within the \C{origin} at which this piece begins and the \C{size}
says how long this piece is within the \C{origin}.  The last field,
\C{size_left}, is a bit special and is a derived value used by the red||black
tree to keep track of how large the left sub||tree of a node owning this
piece is.  This will be instrumental in making sure that the operations on the
piece||tree remain $\Ordo{\lg n}$. */


/*¶ Pieces are accessible from Ruby|<|actually, they will be created on the
Ruby side of the code|>|so we need a couple of wrapper and de||wrapper macros
to aid us in dealing with the conversion between \CLang\ and Ruby: */

extern VALUE g_cPiece;


#define PIECE2VALUE(piece)                              \
        Data_Wrap_Struct(g_cPiece, NULL, NULL, (piece))

#define VALUE2PIECE(value, piece)       \
        Data_Get_Struct((value), Piece, (piece))

/*¶ Nothing that we haven’t already seen in the pattern||matcher
code\dots. */


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


void Init_Piece(VALUE g_cPieceTree);
