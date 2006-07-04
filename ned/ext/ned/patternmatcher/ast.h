/*
 * contents: Routines dealing with the creation of abstract syntax trees (AST) of regular expressions.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{Representing {\NRE}s as {\AST}s}

Before we can convert an \NRE\ to an \NFA\ we need to understand its structure.
We therefore parse the \NRE\ to an intermediate representation of this
structure; an abstract syntax tree, or \AST.  

An \AST\ is a recursive data structure that represents the actual meaning of
the \NRE\ from which it was created.  Repeating
\infigure[construction:figure:thompson example ast], here as
\infigure[figure:example ast], we see the \AST\ that represents the regular
expression \TypedRegex{[01*0|1]*}.

\placefigure
  []
  [figure:example ast]
  {The abstract syntax tree for \TypedRegex{[01*0}\type{|}\TypedRegex{1]*}.}
  {\externalfigure[construction:thompson example ast]}

To create an \AST\ for an \NRE\ we need a way of representing the four
types of nodes in such a tree, namely: {\em leaf nodes}, i.e., the 1s and 0s in
our example; {\em concatenation nodes}, i.e., the two $\circ$’s; {\em iteration
nodes}, i.e., the two $^\KStar$’s; and {\em union nodes}, i.e., the $|$.  This
leaves\footnote{No pun intended, but now that it's been pointed out it’s not
really unintentional anymore, right?} us with the following \C{typedef} of our
four types of nodes: */

typedef enum {
        AST_NODE_LEAF,
        AST_NODE_CONS,
        AST_NODE_ITER,
        AST_NODE_UNION,
} ASTNodeType;


/*¶ Leaf nodes, i.e., nodes of type \C{AST_NODE_LEAF}, come in four flavors:
{\em empty}, for when we need $\epsilon$'s; {\em literal}, which covers
\Unicode\ characters, ranges of such characters, and others (see
\CRef[LiteralType]); {\em assertion} for input position assertion operators
like \TypedRegex{^} and \TypedRegex{$}; and {\em tags} that are attached to the
\AST\ in the compilation step for submatch addressing purposes. */

typedef enum {
        LEAF_EMPTY,
        LEAF_LITERAL,
        LEAF_ASSERTION,
        LEAF_TAG,
} LeafType;


/*¶ As empty leaves contain no additional data, we can skip to discussing
literals instead.  Literals come in four varieties\footnote{Four seems to be a
magic number.  This is the last instance, though.}.  \CDef[LiteralType] */

typedef enum {
        LITERAL_TYPE_NONE,
        LITERAL_TYPE_CHAR,
        LITERAL_TYPE_RANGE,
        LITERAL_TYPE_PREDICATE,
} LiteralType;


/*¶ The only value in this enumeration that needs further explanation is the
last one, \C{LITERAL_TYPE_PREDICATE}.  A literal can be defined by a set
membership predicate.  This is useful for defining rules such as
\TypedRegex{<digit>} as a predicate test such as \C{unichar_isdigit}. In fact,
let's define the type of such functions right now: */

typedef bool (*CharTypePredicate)(unichar c);


/*¶ (The \C{CharType} prefix is derived from \CLang's standard header
\filename{ctype.h}.)  Finally, a literal is defined as the following
structure: */

typedef struct _Literal Literal;

struct _Literal {
        LiteralType type;
        union {
                unichar c;
                struct {
                        unichar begin;
                        unichar end;
                } range;
                CharTypePredicate is_ctype;
        } data;
};


/*¶ The \NRE\ syntax has a couple of operators that act as input position
assertions, i.e., they test the position of an imaginary cursor that moves
along the input and they may also make tests based on the character that this
cursor is positioned over.  The \NRE\ syntax defines four such operators:
\TypedRegex{^}, \TypedRegex{$}, \TypedRegex{^^}, and \TypedRegex{$$}.  Thus, we
need to define four values for each of these assertions (plus one for the case
that there are no assertions to be made): */

typedef enum {
        ASSERTION_NONE       = 0 << 0,
        ASSERTION_BOS        = 1 << 0,
        ASSERTION_EOS        = 1 << 1,
        ASSERTION_BOL        = 1 << 2,
        ASSERTION_EOL        = 1 << 3,
} Assertion;

/*¶ The values act like a bitmap, as more than one kind of assertion can be
made at any given point in the input. */


/*¶ Now that we have defined all the necessary types of literals, let’s define
the actual node type: */

typedef struct _Leaf Leaf;

struct _Leaf {
        LeafType type;
        union {
                Literal literal;
                Assertion assertion;
                int tag;
        } data;
        int id;
};

/*¶ The \C{id} field is used during the compilation step to handle the removal
of $\epsilon$||transitions; more on this on \CRef[ast_node_compute_nfl]. */


/*¶ Concatenation nodes are simple, all they need to keep track of is their
two operands, that is, their two children, left and right.  We also need to
declare the type of our nodes and will define it soon: */

typedef struct _ASTNode ASTNode;

typedef struct _Cons Cons;

struct _Cons {
        ASTNode *left;
        ASTNode *right;
};


/*¶ Iteration nodes are somewhat more complex.  The minimum
and maximum number of iterations possible needs to be specified, as the \NRE\
syntax provides the bounded iteration operator
\TypedRegex{<}$n$\TypedRegex{,}$m$\TypedRegex{>}.  For the two unbounded
iteration operators, \TypedRegex{*} and \TypedRegex{+}, \C{min} is set to $0$
or $1$ and \C{max} is set to $-1$.  As \C{min} will never be negative, we could
have defined it as \C{unsigned}, but one shouldn’t use \C{unsigned} just
because one can.  In fact, many argue that \C{unsigned} values should be
reserved for bit||fields only---where they do make a lot of sense.  Anyway,
returning to our iteration node, we also need a way to tell if the iteration
should be greedy or non||greedy, thus we add a boolean value for that.
Finally, we need to keep track of our operand as well: */

typedef struct _Iter Iter;

struct _Iter {
        int min;
        int max;
        bool minimal;
        ASTNode *atom;
};


/*¶ Union nodes are as simple as concatenation nodes, as they only keep track
of their operands: */

typedef struct _Union Union;

struct _Union {
        ASTNode *left;
        ASTNode *right;
};


/*¶ All the types of nodes have now been defined and what remains is to define
the actual node type: */

typedef struct _IDAndTags IDAndTags;

struct _ASTNode {
        ASTNodeType type;
        union {
                Leaf *leaf;
                Cons *cons;
                Iter *iter;
                Union *uni;
        } data;
/*
 * TODO: The rest should really be split of into a separate
 * data structure that is specific for each user of the \AST.
 * A good name may be ‘annotations’ or something to that effect.
 */
        int submatch_id;
        int n_submatches;
        int n_tags;
        bool nullable;
        IDAndTags *firstid;
        IDAndTags *lastid;
};

/*¶ The roles of the first two fields should be obvious by now, but the rest
need some explanation.  \C{submatch_id} identifies the submatch that
encompasses the sub||tree rooted at this node, if any (it's set to $-1$ if
there is none).  \C{n_submatches} is set to the number of submatches that are
contained within the sub||tree rooted at this node, if any.  \C{n_tags} is
similar, but counts the number of tags instead.

The final three fields, \C{nullable}, \C{firstid}, \C{lastid}, are used in the
compilation step in the removal of $\epsilon$||transitions; more on this when
we introduce the functionality of \CRef[ast_node_compute_nfl].

A possible optimization would be to not use pointers in the \C{data} field,
which saves us some allocation calls at the cost of a larger node structure.
Perhaps it would simplify the structure of the code as well if this change was
made. */


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


ASTNode *ast_node_new(MemPool *pool, ASTNodeType type);
ASTNode *ast_node_literal_new_char(MemPool *pool, unichar c, int id);
ASTNode *ast_node_literal_new_range(MemPool *pool, unichar begin, unichar end,
                                    int id);
ASTNode *ast_node_literal_new_predicate(MemPool *pool,
                                        CharTypePredicate is_ctype, int id);
ASTNode *ast_node_assertion_new(MemPool *pool, Assertion assertion, int id);
ASTNode *ast_node_tag_new(MemPool *pool, int tag, int id);
ASTNode *ast_node_empty_new(MemPool *pool, int id);
ASTNode *ast_node_cons_new(MemPool *pool, ASTNode *left, ASTNode *right);
ASTNode *ast_node_cons_new_or_other(MemPool *pool, ASTNode *left, ASTNode *right);
ASTNode *ast_node_iter_new(MemPool *pool, ASTNode *atom, int min, int max,
                           bool minimal);
ASTNode *ast_node_union_new(MemPool *pool, ASTNode *left, ASTNode *right);

void ast_node_print(ASTNode *node);
