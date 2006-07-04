/*
 * contents: Routines dealing with the creation of abstract syntax trees (AST) of regular expressions.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <ned/unicode.h>

#include "mempool.h"
#include "ast.h"
#include "private.h"

/* TODO: add assertions */


/*¶ \section{Representing {\NRE}s as {\AST}s (Continued)}

Now that we have a way of allocating memory to store the nodes of our {\AST}s
in, it's time to write functions that use this fact.  We begin with two
general functions: */

HIDDEN ASTNode *
ast_node_new(MemPool *pool, ASTNodeType type)
{
        ASTNode *node = POOL_CALLOC(pool, ASTNode);

        node->type = type;
        node->nullable = true;
        node->submatch_id = -1;

        return node;
}


/*¶ This function creates a new node and initializes its fields to values that
make most sense overall.  As leaf nodes make up a large subset of all nodes that
we'll create, we can also use a helper||function for such nodes: */

static Leaf *
leaf_new(MemPool *pool, LeafType type, int id)
{
        Leaf *leaf = POOL_ALLOC(pool, Leaf);

        leaf->type = type;
        leaf->id = id;

        return leaf;
}


/*¶ We need one function for each leaf||node type.  There's quite a few, but
they all work more or less the same, so we won't discuss them in any detail,
letting them speak for themselves.  \CDef[ast_node_literal_new_char] */

HIDDEN ASTNode *
ast_node_literal_new_char(MemPool *pool, unichar c, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_LITERAL, id);
        node->data.leaf->data.literal.type = LITERAL_TYPE_CHAR;
        node->data.leaf->data.literal.data.c = c;

        return node;
}


HIDDEN ASTNode *
ast_node_literal_new_range(MemPool *pool, unichar begin, unichar end, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_LITERAL, id);
        node->data.leaf->data.literal.type = LITERAL_TYPE_RANGE;
        node->data.leaf->data.literal.data.range.begin = begin;
        node->data.leaf->data.literal.data.range.end = end;

        return node;
}

/*¶ It can be noted that accessing the end||points of a range is an incredibly
long procedure.  It doesn't really matter much, though, as they will only be
directly accessed in the function above and at one point in the execution code
later on. */


HIDDEN ASTNode *
ast_node_literal_new_predicate(MemPool *pool, CharTypePredicate is_ctype, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_LITERAL, id);
        node->data.leaf->data.literal.type = LITERAL_TYPE_PREDICATE;
        node->data.leaf->data.literal.data.is_ctype = is_ctype;

        return node;
}


HIDDEN ASTNode *
ast_node_assertion_new(MemPool *pool, Assertion assertion, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_ASSERTION, id);
        node->data.leaf->data.assertion = assertion;

        return node;
}


HIDDEN ASTNode *
ast_node_tag_new(MemPool *pool, int tag, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_TAG, id);
        node->data.leaf->data.tag = tag;

        return node;
}


HIDDEN ASTNode *
ast_node_empty_new(MemPool *pool, int id)
{
        ASTNode *node = ast_node_new(pool, AST_NODE_LEAF);

        node->data.leaf = leaf_new(pool, LEAF_EMPTY, id);

        return node;
}


/*¶ Now we come to the internal nodes.  They too are very similar in nature and
it's really only interesting to remember that certain fields are inherited by
the internal nodes from their children. */

HIDDEN ASTNode *
ast_node_cons_new(MemPool *pool, ASTNode *left, ASTNode *right)
{
        Cons *cons = POOL_ALLOC(pool, Cons);

        cons->left = left;
        cons->right = right;

        ASTNode *node = ast_node_new(pool, AST_NODE_CONS);

        node->data.cons = cons;
        node->n_submatches = left->n_submatches + right->n_submatches;

        return node;
}


/*¶ Sometimes we’ll want to concatenate two nodes even though one of them may
be \C{NULL}.  In this case, we'll simply return the node that isn’t
\C{NULL}.  */

HIDDEN ASTNode *
ast_node_cons_new_or_other(MemPool *pool, ASTNode *left, ASTNode *right)
{
        if (left == NULL)
                return right;
        else if (right == NULL)
                return left;
        else
                return ast_node_cons_new(pool, left, right);
}


/*¶ Here you can see how it was necessary for us to set the \C{n_submatches}
field to the sum of a concatenation node's two children. */

HIDDEN ASTNode *
ast_node_iter_new(MemPool *pool, ASTNode *atom, int min, int max, bool minimal)
{
        Iter *iter = POOL_ALLOC(pool, Iter);

        iter->atom = atom;
        iter->min = min;
        iter->max = max;
        iter->minimal = minimal;

        ASTNode *node = ast_node_new(pool, AST_NODE_ITER);

        node->data.iter = iter;
        node->n_submatches = atom->n_submatches;

        return node;
}

HIDDEN ASTNode *
ast_node_union_new(MemPool *pool, ASTNode *left, ASTNode *right)
{
        Union *uni = POOL_ALLOC(pool, Union);

        uni->left = left;
        uni->right = right;

        ASTNode *node = ast_node_new(pool, AST_NODE_UNION);

        node->data.uni = uni;
        node->n_submatches = left->n_submatches + right->n_submatches;

        return node;
}


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


static char *
char_to_printable(unichar c, char *s, int len)
{
        if (unichar_isprint(c))
                snprintf(s, len, "%lc", c);
        else
                snprintf(s, len, "0x%x", c);

        return s;
}


static void
ast_node_print_simple(ASTNode *node, int indent)
{
        printf("%*s", indent, "");
        switch (node->type) {
        case AST_NODE_LEAF: {
                Leaf *leaf = node->data.leaf;

                switch (leaf->type) {
                case LEAF_LITERAL:
                        printf("(literal ");
                        switch (leaf->data.literal.type) {
                        case LITERAL_TYPE_CHAR: {
                                char s[20];

                                printf("‘%s’ :pos %d :sub %d :tags %d",
                                       char_to_printable(leaf->data.literal.data.c, s, 20),
                                       leaf->id,
                                       node->submatch_id,
                                       node->n_tags);
                                break;
                        }
                        case LITERAL_TYPE_RANGE: {
                                char s1[20];
                                char s2[20];

                                printf("‘%s’-‘%s’ :pos %d :sub %d :tags %d",
                                       char_to_printable(leaf->data.literal.data.range.begin, s1, 20),
                                       char_to_printable(leaf->data.literal.data.range.end, s2, 20),
                                       leaf->id,
                                       node->submatch_id,
                                       node->n_tags);
                                break;
                        }
                        case LITERAL_TYPE_PREDICATE:
                                printf("predicate :pos %d :sub %d :tags %d",
                                       leaf->id, node->submatch_id, node->n_tags);
                                break;
                        case LITERAL_TYPE_NONE:
                                printf("none");
                                break;
                        default:
                                assert(false);
                                break;
                        }
                        printf(")");
                        break;
                case LEAF_EMPTY:
                        printf("(literal ε)");
                        break;
                case LEAF_ASSERTION: {
                        printf("(assertions '(");
                        if (leaf->data.assertion & ASSERTION_BOS) {
                                printf("bos ");
                        }
                        if (leaf->data.assertion & ASSERTION_EOS) {
                                printf("eos ");
                        }
                        if (leaf->data.assertion & ASSERTION_BOL) {
                                printf("bol ");
                        }
                        if (leaf->data.assertion & ASSERTION_EOL) {
                                printf("eol ");
                        }
                        printf(")");
                        break;
                }
                case LEAF_TAG:
                        printf("(tag %d)", leaf->data.tag);
                        break;
                default:
                        assert(false);
                        break;
                }
                break;
        }
        case AST_NODE_CONS: {
                Cons *cons = node->data.cons;

                printf("(concatenation :sub %d :tags %d\n",
                       node->submatch_id, node->n_tags);
                ast_node_print_simple(cons->left, indent + 2);
                printf("\n");
                ast_node_print_simple(cons->right, indent + 2);
                printf(")");
                break;
        }
        case AST_NODE_ITER: {
                Iter *iter = node->data.iter;

                printf("(iteration :min %d :max %d :greedy %s :sub %d :tags %d\n",
                       iter->min, iter->max, iter->minimal ? "no" : "yes",
                       node->submatch_id, node->n_tags);
                ast_node_print_simple(iter->atom, indent + 2);
                printf(")");
                break;
        }
        case AST_NODE_UNION: {
                Union *uni = node->data.uni;

                printf("(union :sub %d :tags %d\n",
                       node->submatch_id, node->n_tags);
                ast_node_print_simple(uni->left, indent + 2);
                printf("\n");
                ast_node_print_simple(uni->right, indent + 2);
                printf(")");
                break;
        }
        default:
                assert(false);
                break;
        }
}


HIDDEN void
ast_node_print(ASTNode *node)
{
        ast_node_print_simple(node, 0);
        printf("\n");
}
