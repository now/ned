/*
 * contents: Compile an AST of a regular expression into a TNFA.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <ned/unicode.h>

#include "private.h"
#include "mempool.h"
#include "ast.h"
#include "compile.h"


/*¶ Much of the work that will be performed during the compilation state deals
with sentinel||terminated arrays, i.e., arrays that end with a value that is
not part of the data set.  As we will be traversing such arrays a lot,
especially in finding the end of the array to add another element to it, we
define a macro that simplifies our life somewhat: */

#define array_find(predicate, iter)                             \
        for (iter = 0; !(predicate); iter++)

/*¶ This macro can be used as follows:

\startC
int i;
array_find(ary[i] < 0, i);
ary[i] = …;
\stopC

In this example, \C{ary} is searched for an element that is less than zero and
\C{i} will be set to the index of that element.  It is assumed that \C{ary} is
terminated by a sentinel value that is smaller than zero and thus we have found
the end of the array, so that we may an element to it. */


/*¶ While we’re at it, let’s write two functions for pushing and popping values
of arrays of integers terminated by a sentinel value of $-1$: */

static inline void
array_push(int *ary, int value)
{
        int i;

        array_find(ary[i] < 0, i);

        ary[i] = value;
        ary[i + 1] = -1;
}

static inline void
array_pop(int *ary)
{
        int i;

        array_find(ary[i] < 0, i);

        ary[i - 1] = -1;
}


/*¶ Much of the compilation code is dedicated to annotating the \AST\ with tags
that will be used for submatch addressing.  It is perhaps also the hardest part
of the compilation step to understand, so pay close attention to what’s going
on. */

/*¶ We begin by defining some helper||functions that will aide us in annotating
the \AST with tags: */

static void
ast_node_add_tag_left(MemPool *pool, ASTNode *node, int tag)
{
        Cons *cons = POOL_ALLOC(pool, Cons);
        cons->left = ast_node_tag_new(pool, tag, -1);
        cons->right = ast_node_new(pool, node->type);
        cons->right->data = node->data;
        cons->right->firstid = NULL;
        cons->right->lastid = NULL;
        cons->right->n_tags = 0;

        node->type = AST_NODE_CONS;
        node->data.cons = cons;
}

/*¶ This function transforms the given node into a concatenation node, where
the left operand is the given tag and the right operand is the old node, so to
speak.  The following function does the same thing, but with the tag and the
node reversed: */

static void
ast_node_add_tag_right(MemPool *pool, ASTNode *node, int tag)
{
        Cons *cons = POOL_ALLOC(pool, Cons);
        cons->right = ast_node_tag_new(pool, tag, -1);
        cons->left = ast_node_new(pool, node->type);
        cons->left->data = node->data;
        cons->left->firstid = NULL;
        cons->left->lastid = NULL;
        cons->left->n_tags = 0;

        node->type = AST_NODE_CONS;
        node->data.cons = cons;
}


/*¶ The process of adding tags to our trees is a complicated one and it’s best
to break it down into manageable chunks.  We will therefore need a context to
pass around between the many functions that will be involved in the process. */

typedef struct _AddTagsContext AddTagsContext;

/* TODO: the n_tags field can be removed. */
struct _AddTagsContext {
        MemPool *pool;
        TNFA *tnfa;
        bool first_pass;
        int *regset;
        int n_tags;
        int tag;
        int next_tag;
        int *parents;
        int minimal_tag;
        TagDirection direction;
};


/*¶ The following function updates the set of minimal tags and submatch data
defined for the \TNFA\ that we are constructing.  The \C{regset} field of the
context is a set of submatches that are using the tag currently being added to
the \AST, i.e., \C{context->tag}.  As it is a flat array, each submatch’s id is
doubled and 0 or 1 is added, depending on if it’s the start or end tag of the
given submatch that is being added.  The \C{minimal_tag} field is set to a tag
that marks the beginning of a minimal match, or $-1$ if there is no such tag.
If \C{first_pass} is true, then all we are interested in is counting the number
of tags that will be necessary to tag the tree.  This information will then be
used by the caller to allocate the number of tags necessary and then call us
again; and this time \C{first_pass} will be false.  The \C{n_tags} field  is
the number of tags that will be necessary to tag the tree. */

static void
update_minimal_tags_and_submatch_data(AddTagsContext *context)
{
        if (context->minimal_tag >= 0) {
                /* TODO: this is really just two pushes */
                int i;

                array_find(context->tnfa->minimal_tags[i] < 0, i);

                context->tnfa->minimal_tags[i] = context->tag;
                context->tnfa->minimal_tags[i + 1] = context->minimal_tag;
                context->tnfa->minimal_tags[i + 2] = -1;

                context->minimal_tag = -1;
        }

        SubmatchData *submatch_data = context->tnfa->submatch_data;
        for (int i = 0; context->regset[i] >= 0; i++) {
                int id = context->regset[i] / 2;

                if (context->regset[i] % 2 == 0)
                        submatch_data[id].begin_tag = context->tag;
                else
                        submatch_data[id].end_tag = context->tag;
        }
}


/*¶ Whenever we find a node to be tagged, this next function will deal with
adding the tag and updating the \TNFA\ with any necessary information relating
to this tag: */

static void
tag_and_update(AddTagsContext *context, ASTNode *node, TagDirection direction)
{
        ast_node_add_tag_left(context->pool, node, context->tag);

        context->tnfa->tag_directions[context->tag] = direction;

        update_minimal_tags_and_submatch_data(context);
}


/*¶ We will need to reset the fields dealing with tags in the context whenever
we are done with it.  What this effectively means is that the \C{regset} must
be reset, the \C{tag} is set to the \C{next_tag}, and the \C{n_tags} is
incremented. */

static void
update_tags(AddTagsContext *context)
{
        context->regset[0] = -1;
        context->tag = context->next_tag++;
        context->n_tags++;
}


/*¶ We will need to add submatches to the list of \C{parents} to other
submatches in the tree, as this is information required by the submatch data
structure. */

static void
update_parents(AddTagsContext *context, int submatch_id)
{
        if (submatch_id >= 0)
                array_push(context->parents, submatch_id);
}


/*¶ As the \AST\ is recursive in nature, we need a forward declaration.
(Remember: in \CLang\ we need forward declarations, which is the reason why
everything is done in reverse and you are forced to read it in reverse.) */

static void _ast_node_add_tags(AddTagsContext *context, ASTNode *node);


/*¶ This function adds tags to leaf nodes of the literal kind. */

static void
_ast_node_add_tags_leaf(AddTagsContext *context, ASTNode *node)
{
        assert(node->data.leaf->type != LEAF_TAG);

        if (node->data.leaf->type == LEAF_LITERAL && context->regset[0] >= 0) {
                if (!context->first_pass)
                        tag_and_update(context, node, context->direction);
                else
                        node->n_tags = 1;

                update_tags(context);
        }

        update_parents(context, node->submatch_id);
}

/*¶ The process of adding tags is actually a two||pass procedure.  During the
first pass through the tree, we just figure out how many tags and submatch data
structures that will be needed.  During the second pass, we actually add the
tags and update the submatch data as appropriate. */


/*¶ For a concatenation we must tag the two operands.  This is a bit
complicated, as we need to keep track of what tag to use next: */

static void
_ast_node_add_tags_cons(AddTagsContext *context, ASTNode *node)
{
        Cons *cons = node->data.cons;
        int reserved_tag = -1;
        int next_tag = context->next_tag + node->data.cons->left->n_tags;

        if (cons->left->n_tags > 0 && cons->right->n_tags > 0)
                reserved_tag = context->next_tag++;

        update_parents(context, node->submatch_id);

        _ast_node_add_tags(context, cons->left);

        context->next_tag = next_tag;
        if (reserved_tag >= 0)
                context->tag = reserved_tag;

        _ast_node_add_tags(context, cons->right);

        if (context->first_pass)
                node->n_tags = node->data.cons->left->n_tags +
                               node->data.cons->right->n_tags;
}


/*¶ There’s a lot going on in the process of adding tasks to iteration
nodes: */

static void
_ast_node_add_tags_iter(AddTagsContext *context, ASTNode *node)
{
        Iter *iter = node->data.iter;
        bool add_tag = (context->regset[0] >= 0 || iter->minimal);
        bool minimal;
        int saved_tag;

        if (context->first_pass) {
                minimal = false;
                saved_tag = -1;
        } else {
                minimal = iter->minimal;
                saved_tag = context->tag;
        }

        if (add_tag) {
                if (!context->first_pass) {
                        TagDirection direction = iter->minimal ?
                                TAG_MAXIMIZE : context->direction;

                        tag_and_update(context, node, direction);
                }

                update_tags(context);
        }

        context->direction = TAG_MINIMIZE;

        update_parents(context, node->submatch_id);

        _ast_node_add_tags(context, iter->atom);

        if (context->first_pass) {
                node->n_tags = node->data.iter->atom->n_tags + (add_tag ? 1 : 0);
                context->minimal_tag = -1;
        } else {
                if (minimal) {
                        context->minimal_tag = saved_tag;
                        context->direction = TAG_MINIMIZE;
                } else {
                        context->direction = TAG_MAXIMIZE;
                }
        }
}


/*¶ By far the most difficult node to tag is the union node.  We need to keep
track of what tags to use like for concatenation nodes, but it’s much worse
this time.  We also need to make sure that \C{regset} doesn’t become messed up
while we’re processing the operands. */

static void
_ast_node_add_tags_union(AddTagsContext *context, ASTNode *node)
{
        Union *uni = node->data.uni;
        ASTNode *left = uni->left, *right = uni->right;
        bool add_tag = (context->regset[0] >= 0);
        int left_tag, right_tag;

        if (add_tag) {
                left_tag = context->next_tag;
                right_tag = context->next_tag + 1;
        } else {
                left_tag = context->tag;
                right_tag = context->next_tag;
        }

        if (add_tag) {
                if (!context->first_pass)
                        tag_and_update(context, node, context->direction);

                update_tags(context);
        }

        if (node->n_submatches > 0) {
                /* the next two tags are reserved for markers. */
                context->next_tag++;
                context->tag = context->next_tag;
                context->next_tag++;
        }

        update_parents(context, node->submatch_id);

        _ast_node_add_tags(context, left);

        /* move regset upwards so that we can work on it without messing it up */
        int *regset_base = context->regset;
        while (*context->regset >= 0)
                context->regset++;

        _ast_node_add_tags(context, right);

        context->regset = regset_base;

        if (context->first_pass) {
                node->n_tags = node->data.uni->left->n_tags
                        + node->data.uni->right->n_tags
                        + (add_tag ? 1 : 0)
                        + (node->n_submatches > 0 ? 2 : 0);
        }

        if (node->n_submatches > 0) {
                if (!context->first_pass) {
                        ast_node_add_tag_right(context->pool, left, left_tag);
                        context->tnfa->tag_directions[left_tag] = TAG_MAXIMIZE;
                        ast_node_add_tag_right(context->pool, right, right_tag);
                        context->tnfa->tag_directions[right_tag] = TAG_MAXIMIZE;
                }

                context->n_tags += 2;
        }

        context->direction = TAG_MAXIMIZE;
}


/*¶ Before we can write the machinery that invokes the correct tagging
functions we have to define two functions that push and pop a submatch that we
are going to tag: */

static int s_parents_empty[] = { -1 };

HIDDEN int *g_parents_empty = s_parents_empty;


static void
_push_submatch(AddTagsContext *context, int id)
{
        array_push(context->regset, id * 2);

        if (context->first_pass)
                return;

        int i;
        array_find(context->parents[i] < 0, i);

        if (i > 0) {
                int *p = ALLOC_N(int, i + 1);
                MEMCPY(p, context->parents, int, i + 1);
                context->tnfa->submatch_data[id].parents = p;
        } else {
                context->tnfa->submatch_data[id].parents = PARENTS_EMPTY;
        }
}

/*¶ The \C{regset} and \C{parents} are updated appropriately.  As with the
empty set of tags, we have an empty set of parents. */

static void
_pop_submatch(AddTagsContext *context, int id)
{
        array_push(context->regset, id * 2 + 1);
        array_pop(context->parents);
}


/*¶ Now to the machinery that invokes the correct tagging functions: */

static void
_ast_node_add_tags(AddTagsContext *context, ASTNode *node)
{
        int id = node->submatch_id;

        if (id >= 0)
                _push_submatch(context, id);

        static void (*dispatch[])(AddTagsContext *, ASTNode *) = {
                [AST_NODE_LEAF] = _ast_node_add_tags_leaf,
                [AST_NODE_CONS] = _ast_node_add_tags_cons,
                [AST_NODE_ITER] = _ast_node_add_tags_iter,
                [AST_NODE_UNION] = _ast_node_add_tags_union,
        };

        assert(node->type < sizeof(dispatch) / sizeof(dispatch[0]));
        assert(dispatch[node->type] != NULL);
        dispatch[node->type](context, node);

        if (id >= 0)
                _pop_submatch(context, id);
}


/*¶ Finally, we define the function that sets up the context and starts
the whole thing off.  It also makes sure that any submatch data and minimal
tags that need updating are updated before returning. */

static void
ast_node_add_tags(MemPool *pool, ASTNode *tree, TNFA *tnfa)
{
        AddTagsContext context = {
                .pool = pool,
                .tnfa = tnfa,
                .first_pass = (pool == NULL),
                .n_tags = 0,
                .tag = 0,
                .next_tag = 1,
                .minimal_tag = -1,
                .direction = TAG_MINIMIZE
        };

        int regset_base[(tnfa->n_submatches + 1) * 2];
        context.regset = regset_base;
        context.regset[0] = -1;

        int parents[tnfa->n_submatches + 1];
        context.parents = parents;
        context.parents[0] = -1;

        if (!context.first_pass)
                tnfa->minimal_tags[0] = -1;

        _ast_node_add_tags(&context, tree);

        if (!context.first_pass) {
                context.tag = context.n_tags;
                update_minimal_tags_and_submatch_data(&context);
        }

        assert(tree->n_tags == context.n_tags);

        tnfa->end_tag = context.n_tags;
        tnfa->n_tags = context.n_tags;
}


/*¶ The bounded iteration operator needs to be dealt with in the \AST.  What we
do is perform an expansion of the operand of such an iterator, i.e., we create
the number of copies needed for the number of bounded repetitions of it and
concatenate them together. */

/*¶ We will provide some options to tell how the expansion is to be
performed. */ 

typedef enum {
        DUPLICATE_LEAVE_TAGS,
        DUPLICATE_REMOVE_TAGS,
        DUPLICATE_MAXIMIZE_FIRST_TAG
} DuplicateOption;


/*¶ As with most of the functions lately, we create a context to work
within. */

typedef struct _DuplicateContext DuplicateContext;

struct _DuplicateContext {
        MemPool *pool;
        DuplicateOption option;
        int id_add;
        TagDirection *tag_directions;
        int max_id;
        bool first_tag;
        int n_copied;
};


/*¶ And, again, we need a forward declaration: */

static ASTNode *_ast_node_duplicate(DuplicateContext *context, ASTNode *node);


/*¶ Now, let’s copy literals: */

static ASTNode *
_duplicate_literal(DuplicateContext *context, const Literal *literal, int id)
{
        switch (literal->type) {
        case LITERAL_TYPE_CHAR:
                return ast_node_literal_new_char(context->pool,
                                                 literal->data.c,
                                                 id);
        case LITERAL_TYPE_RANGE:
                return ast_node_literal_new_range(context->pool,
                                                  literal->data.range.begin,
                                                  literal->data.range.end,
                                                  id);
        case LITERAL_TYPE_PREDICATE:
                return ast_node_literal_new_predicate(context->pool,
                                                      literal->data.is_ctype,
                                                      id);
        default:
                assert(false);
                return NULL;
        }
}


/*¶ Nothing very spectacular going on here, and nothing very spectacular going
on when copying tags either: */

static ASTNode *
_duplicate_tag(DuplicateContext *context, int tag, int id)
{
        switch (context->option) {
        case DUPLICATE_REMOVE_TAGS:
                return ast_node_empty_new(context->pool, -1);
        case DUPLICATE_MAXIMIZE_FIRST_TAG:
                if (context->first_tag) {
                        context->tag_directions[tag] = TAG_MAXIMIZE;
                        context->first_tag = false;
                }
                return ast_node_tag_new(context->pool, tag, id);
        case DUPLICATE_LEAVE_TAGS:
                return ast_node_tag_new(context->pool, tag, id);
        default:
                assert(false);
                return NULL;
        }
}

/*¶ What we do do is allow our caller to say that they don’t want any tags in
the expansion or that they wish to maximize the first tag found during
expansion.  This is what we need our \C{DuplicateOption} for.  Note how we
could have let the \C{DUPLICATE_MAXIMIZE_FIRST_TAG} case fall through to the
\C{DUPLICATE_LEAVE_TAGS} case.  Note also how we didn’t do that.  Falling
through is evil. */


/*¶ Copying leaves is easy enough as well. */

static ASTNode *
_duplicate_leaf(DuplicateContext *context, const Leaf *leaf)
{
        ASTNode *dup;
        int id = leaf->id;

        switch (leaf->type) {
        case LEAF_LITERAL:
                id += context->id_add;
                dup = _duplicate_literal(context, &leaf->data.literal, id);
                context->n_copied++;
                break;
        case LEAF_TAG:
                dup = _duplicate_tag(context, leaf->data.tag, id);
                break;
        case LEAF_ASSERTION:
                dup = ast_node_assertion_new(context->pool,
                                             leaf->data.assertion, id);
                break;
        case LEAF_EMPTY:
                dup = ast_node_empty_new(context->pool, id);
                break;
        default:
                assert(false);
                dup = NULL;
                break;
        }

        context->max_id = MAX(id, context->max_id);

        return dup;
}

/*¶ We do, however, need to do some calculations that will make sense later on
when we perform the actual expansion. */


/*¶ Copying a concatenation node is very straightforward.  All we need to do is
copy the node itself, and its two children: */

static ASTNode *
_duplicate_cons(DuplicateContext *context, const Cons *cons)
{
        ASTNode *dup = ast_node_cons_new(context->pool, cons->left, cons->right);

        dup->data.cons->left = _ast_node_duplicate(context, cons->left);
        dup->data.cons->right = _ast_node_duplicate(context, cons->right);

        return dup;
}


/*¶ The same goes for iteration nodes: */

static ASTNode *
_duplicate_iter(DuplicateContext *context, const Iter *iter)
{
        ASTNode *dup = ast_node_iter_new(context->pool, iter->atom, iter->min,
                                         iter->max, iter->minimal);

        dup->data.iter->atom = _ast_node_duplicate(context, iter->atom);

        return dup;
}


/*¶ And for iteration nodes: */

static ASTNode *
_duplicate_union(DuplicateContext *context, const Union *uni)
{
        ASTNode *dup = ast_node_union_new(context->pool, uni->left, uni->right);

        dup->data.uni->left = _ast_node_duplicate(context, uni->left);
        dup->data.uni->right = _ast_node_duplicate(context, uni->right);

        return dup;
}

/*¶ We also need a function that we can call recursively and will delegate its
work to the proper handler as defined above: */

static ASTNode *
_ast_node_duplicate(DuplicateContext *context, ASTNode *node)
{
        ASTNode *dup;

        switch (node->type) {
        case AST_NODE_LEAF:
                dup = _duplicate_leaf(context, node->data.leaf);
                break;
        case AST_NODE_CONS:
                dup = _duplicate_cons(context, node->data.cons);
                break;
        case AST_NODE_ITER:
                dup = _duplicate_iter(context, node->data.iter);
                break;
        case AST_NODE_UNION:
                dup = _duplicate_union(context, node->data.uni);
                break;
        default:
                assert(false);
                dup = NULL;
                break;
        }

        return dup;
}


/*¶ Finally, we need a function that sets the ball in motion: */

static ASTNode *
ast_node_duplicate(MemPool *pool, ASTNode *node, DuplicateOption option,
                   int *id_add, TagDirection *tag_directions, int *max_id)
{
        DuplicateContext context = {
                .pool = pool,
                .option = option,
                .id_add = *id_add,
                .tag_directions = tag_directions,
                .max_id = *max_id,
                .first_tag = true,
                .n_copied = 0
        };

        ASTNode *dup = _ast_node_duplicate(&context, node);

        *id_add = context.id_add + context.n_copied;
        *max_id = context.max_id;

        return dup;
}

/*¶ The expansion code needs some extra data about what happened during the
expansion, so we take care of that. */

/*¶ The expansion code needs a context as well. */

typedef struct _ExpandContext ExpandContext;

struct _ExpandContext {
        MemPool *pool;
        TagDirection *tag_directions;
        int max_id;
        int id_add;
        int id_add_total;
        int iter_depth;
};

/*¶ Most of the fields are used to keep track of what ids to use for new and
old literal nodes. */


static void _ast_expand(ExpandContext *context, ASTNode *node);


/*¶ The expansion code expands all pieces that have a bounded iteration.  That
is, any \TypedRegex{a<3,5>} is expanded to \TypedRegex{aaaa?a?} and similar for
arbitrary boundaries.

We begin by dealing with expanding the lower bound, if any: */

static ASTNode *
_ast_expand_iter_lower(ExpandContext *context, Iter *iter, int *saved_id_add)
{
        ASTNode *seq = NULL;
        DuplicateOption option = DUPLICATE_REMOVE_TAGS;

        for (int i = 0; i < iter->min; i++) {
                *saved_id_add = context->id_add;

                if (i + 1 == iter->min)
                        option = DUPLICATE_MAXIMIZE_FIRST_TAG;

                ASTNode *dup = ast_node_duplicate(context->pool, iter->atom,
                                                  option, &context->id_add,
                                                  context->tag_directions,
                                                  &context->max_id);
                seq = ast_node_cons_new_or_other(context->pool, seq, dup);
        }

        return seq;
}

/*¶ For all but the last copy, we remove the tags in the atom being duplicated.
For the last one, we maximize the first tag. */


/*¶ Next, we expand the upper bound: */

static ASTNode *
_ast_expand_iter_upper(ExpandContext *context, Iter *iter, int *saved_id_add)
{
        ASTNode *seq = NULL;

        for (int i = iter->min; i < iter->max; i++) {
                *saved_id_add = context->id_add;

                ASTNode *dup = ast_node_duplicate(context->pool, iter->atom,
                                                  DUPLICATE_LEAVE_TAGS,
                                                  &context->id_add, NULL,
                                                  &context->max_id);
                seq = ast_node_cons_new_or_other(context->pool, dup, seq);
                ASTNode *empty = ast_node_empty_new(context->pool, -1);
                seq = ast_node_union_new(context->pool, empty, seq);
        }

        return seq;
}


/*¶ Finally, we combine the two into one function that does both: */

static void
_ast_expand_iter_lower_and_upper(ExpandContext *context, ASTNode *node)
{
        Iter *iter = node->data.iter;
        int saved_id_add = context->id_add;

        ASTNode *seq1 = _ast_expand_iter_lower(context, iter, &saved_id_add);

        ASTNode *seq2 = NULL;
        if (iter->max != -1) {
                seq2 = _ast_expand_iter_upper(context, iter, &saved_id_add);
        } else {
                saved_id_add = context->id_add;

                seq2 = ast_node_duplicate(context->pool, iter->atom,
                                          DUPLICATE_LEAVE_TAGS,
                                          &context->id_add, NULL,
                                          &context->max_id);
                seq2 = ast_node_iter_new(context->pool, seq2, 0, -1, false);
        }

        context->id_add = saved_id_add;

        ASTNode *seq = ast_node_cons_new_or_other(context->pool, seq1, seq2);

        node->data = seq->data;
        node->type = seq->type;
}

/*¶ Note how we end the sequence with an unbounded iteration if there was no
upper boundary specified. */


/*¶ Now that we can expand the lower and upper boundaries, let’s write down the
function that will actually use this fact: */

static void
_ast_expand_iter(ExpandContext *context, ASTNode *node)
{
        Iter *iter = node->data.iter;
        int saved_id_add = context->id_add;

        if (iter->min > 1 || iter->max > 1)
                context->id_add = 0;

        context->iter_depth++;

        _ast_expand(context, iter->atom);

        context->id_add = saved_id_add;
        int id_add_last = context->id_add;

        if (iter->min > 1 || iter->max > 1)
                _ast_expand_iter_lower_and_upper(context, node);

        context->id_add_total += context->id_add - id_add_last;
        context->iter_depth--;
        if (context->iter_depth == 0)
                context->id_add = context->id_add_total;
}

/*¶ We need to keep track of some ids and the depth of the iteration so that
the nodes that we create during the expansion will get correct ids assigned to
them. */


/*¶ Finally, we set up two functions that invoke the whole procedure.  The
first is private and will be called recursively.  The second is the interface
to the whole thing. */

static void
_ast_expand(ExpandContext *context, ASTNode *node)
{
        switch (node->type) {
        case AST_NODE_LEAF:
                if (node->data.leaf->type == LEAF_LITERAL)
                        node->data.leaf->id += context->id_add;
                context->max_id = MAX(node->data.leaf->id, context->max_id);
                break;
        case AST_NODE_CONS:
                _ast_expand(context, node->data.cons->left);
                _ast_expand(context, node->data.cons->right);
                break;
        case AST_NODE_ITER:
                _ast_expand_iter(context, node);
                break;
        case AST_NODE_UNION:
                _ast_expand(context, node->data.uni->left);
                _ast_expand(context, node->data.uni->right);
                break;
        default:
                assert(false);
                break;
        }
}

static void
ast_expand(MemPool *pool, ASTNode *node, int *id, TagDirection *tag_directions)
{
        ExpandContext context = {
                .pool = pool,
                .tag_directions = tag_directions,
                .id_add = 0,
                .id_add_total = 0,
                .iter_depth = 0
        };

        _ast_expand(&context, node);

        *id += context.id_add_total;

        assert(*id >= context.max_id);
}

/*¶ Thus ends our \AST\ expansion code. */

/* TODO: move the discussion of the rules for turning an \AST\ into a \TNFA\
 * here.  Have tables for computing nullable, firstpos, lastpos, and emptymatch
 * here. */

/*¶ Now we are ready to generate an $\epsilon$||free \TNFA\ from our \AST.  The
method that we’ll use is based on the \NFA||to||\DFA conversion||algorithm that
can be found in, for example, \cite[AhoSethiUllman87].  We won’t go any further
than removing the ε||transitions so we aren’t creating a deterministic
automaton, as we can still make multiple transitions on the same input symbol
from a given state, but we’ll gain some of the speed||advantages of {\DFA}s
nonetheless, as we won’t have to calculate the ε||closure after each iteration
of the automaton.

Before we begin, we’ll augment the \AST\ by concatenating a unique leaf that
isn’t part of the input alphabet to it.  This leaf, denoted by \#, gives us a
unique final state with which we can operate.

% TODO: expand?

Now, for each node in the \AST, we calculate three functions,
\Function{nullable}, \Function{firstpos}, and \Function{lastpos}.  The
\Function{nullable} function states whether a subexpression rooted at a given
node $n$ generates the empty string ε or not; $\Function{firstpos}(n)$ then
gives the set of id’s of nodes that can match the first symbol of a string
generated by the subexpression rooted at $n$; $\Function{lastpos}(n)$ will
conversely give the set of id’s of nodes that can match the last symbol in such
a string.  These functions can be calculated by walking the nodes of the \AST\
in a bottom||up manner (postfix order).  The inductive rules for the functions’
values are shown in \intable[table:nullable and firstpos].  \Function{lastpos}
is the same as \Function{firstpos}, but with $c_{`left}$ and $c_{`right}$
reversed.

\placetable
  [][table:nullable and firstpos]
  {The inductive rules for calculating the values of the \Function{nullable}
   and \Function{firstpos} functions.}
  \starttable[|c|c|c|]
  \HL
  \NC \bf Node $n$ \NC \bf $\Function{nullable}(n)$ \NC \bf $\Function{firstpos}(n)$ \NC\AR
  \HL
  \NC leaf labeled ε     \NC true  \NC ∅           \NC\AR
  \NC leaf labeled $t_x$ \NC true  \NC ∅           \NC\AR
  \NC leaf with id $i$   \NC false \NC $\{(i,∅)\}$ \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - alternation] \NC
      $\Function{nullable}(c_l) ∨ \Function{nullable}(c_r)$ \NC
      $\Function{firstpos}(c_l) ∪ \Function{firstpos}(c_r)$ \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - concatenation] \NC 
      $\Function{nullable}(c_l) ∧ \Function{nullable}(c_r)$ \NC
      \interdisplayskip=0.4ex
      \startnathequation
        \wall \text{\bf if } \Function{nullable}(c_l) \text{\bf\ then} \\
                \quad \wall \Function{firstpos}(c_l) ∪ \\
                            \Function{addtags}(\wall \Function{firstpos}(c_r), \\
                                                     \Function{emptymatch}(c_l))
                                               \return \return \\
              \text{\bf else} \\
                \quad \Function{firstpos}(c_l)
        \return
      \stopnathequation
      \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - closure] \NC 
      true \NC
      $\Function{firstpos}(c)$ \NC\AR
  \HL
  \stoptable

The function \Function{emptymatch} used in the calculation of
\Function{firstpos} and \Function{lastpos} for concatenation nodes is defined
by the rules in \intable[table:emptymatch].  The \Function{addtags} function
takes a set of pairs $(i,U)$ called $P$ and a set of tags $T$, where $i$ is a
node id and $t$ is a set of tags.  The function returns a new set of pairs
where the sets of tags have been joined:

  \startnathequation
    \{\,(p,U') \mid (p,U) ∈ P ∧ U' = U ∪ T\,\}
  \stopnathequation

\placetable
  [][table:emptymatch]
  {The inductive rules for computing the \Function{emptymatch} function.}
  \starttable[|c|c|]
  \HL
  \NC \bf Node $n$ \NC \bf $\Function{emptymatch}(n)$ \NC\AR
  \HL
  \NC leaf labeled ε     \NC ∅      \NC\AR
  \NC leaf labeled $t_x$ \NC $t_x$  \NC\AR
  \NC leaf with id $i$   \NC ∅      \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - alternation] \NC
      \interdisplayskip=0.4ex
      \startnathequation
        \wall \text{\bf if } \Function{nullable}(c_l) \text{\bf\ then} \\
                \quad \Function{emptymatch}(c_l) \\
              \text{\bf else} \\
                \quad \Function{emptymatch}(c_r)
        \return
      \stopnathequation
      \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - concatenation] \NC 
      $\Function{firstpos}(c_l) ∪ \Function{firstpos}(c_r)$ \NC\AR
  \NC \externalfigure[source:patternmatcher:functions - closure] \NC 
      \interdisplayskip=0.4ex
      \startnathequation
        \wall \text{\bf if } \Function{nullable}(c_l) \text{\bf\ then} \\
                \quad \Function{emptymatch}(c_l) \\
              \text{\bf else} \\
                \quad ∅
        \return
      \stopnathequation
      \NC\AR
  \HL
  \stoptable
*/


/*¶ We can finally define the \C{IDAndTags} structure, now that we know what it
must contain: */

struct _IDAndTags {
        int id;
        int *tags;
        Assertion assertions;
        Literal literal;
};

/*¶ The \C{id} and \C{tags} fields are obvious.  The \C{assertions} and
\C{literal} fields will be needed when we turn the \AST\ into the set of
transitions that will represent our automaton. */

/*¶ Many sets of ids and tags will actually have empty sets of tags, so we
define the empty set of tags that can be used whenever we find that no tags
will be needed.  We have already shown you the declaration of this variable
and the associated macro, \C{TAGS_EMPTY}. */

static int s_tags_empty[] = { -1 };

HIDDEN int * g_tags_empty = s_tags_empty;


/*¶ Next follows a set of functions that operate on this structure.  We need
ways of creating it and way to take the union of two of them, for the firstid
function. */

static IDAndTags *
id_and_tags_new_empty(MemPool *pool)
{
        IDAndTags *set = POOL_ALLOC_N(pool, IDAndTags, 1);
        set[0].id = -1;
        set[0].tags = TAGS_EMPTY;
        set[0].assertions = ASSERTION_NONE;
        set[0].literal.type = LITERAL_TYPE_NONE;

        return set;
}

/*¶ This function allows us to create an empty set of \C{IDAndTags}; the next a
set of one: */

static IDAndTags *
id_and_tags_new_one(MemPool *pool, int id, Literal *literal)
{
        IDAndTags *set = POOL_ALLOC_N(pool, IDAndTags, 2);
        set[0].id = id;
        set[0].tags = TAGS_EMPTY;
        set[0].assertions = ASSERTION_NONE;
        set[0].literal = *literal;
        set[1].id = -1;
        set[1].tags = TAGS_EMPTY;
        set[1].assertions = ASSERTION_NONE;
        set[1].literal.type = LITERAL_TYPE_NONE;

        return set;
}

/*¶ Nothing spectacular going on here. */


/*¶ Joining two sets using the union operator is a bit more complex, though.
We split it up in a couple of helper||functions.  The first will unite two sets
of tags, that is, it does the work of the addtags function. */

static int *
_tags_new_union(MemPool *pool, int *t1, int *t2)
{
        assert(t1 != NULL);
        assert(t2 != NULL);

        int n_t1 = 0, n_t2 = 0;

        array_find(t1[n_t1] < 0, n_t1);
        array_find(t2[n_t2] < 0, n_t2);

        if (n_t1 + n_t2 == 0)
                return TAGS_EMPTY;
        
        int *uni = POOL_ALLOC_N(pool, int, n_t1 + n_t2 + 1);

        MEMCPY(uni, t1, int, n_t1);
        MEMCPY(&uni[n_t1], t2, int, n_t2);
        uni[n_t1 + n_t2] = -1;

        return uni;
}


/*¶ Creating the union of two sets will be done by copying the two sets into a
new one, just large enough to fit both.  We need a function that does the
actual copying: */

static void
_id_and_tags_copy(MemPool *pool, IDAndTags *dest, IDAndTags *src, int *tags,
                  Assertion assertions)
{
        for (IDAndTags *p = src, *q = dest; p->id >= 0; p++, q++) {
                q->id = p->id;
                q->literal = p->literal;
                q->assertions = p->assertions | assertions;
                q->tags = _tags_new_union(pool, p->tags, tags);
        }
}


/*¶ Finally, we can write a function that figures out the size of the two sets
and allocates memory for the new one that will contain both.  We then copy both
into the new one: */

static IDAndTags *
id_and_tags_new_union(MemPool *pool, IDAndTags *set1, IDAndTags *set2,
                      int *tags, Assertion assertions)
{
        int n_set1 = 0, n_set2;

        array_find(set1[n_set1].id < 0, n_set1);
        array_find(set2[n_set2].id < 0, n_set2);

        IDAndTags *uni = POOL_ALLOC_N(pool, IDAndTags, n_set1 + n_set2 + 1);

        _id_and_tags_copy(pool, uni, set1, tags, assertions);
        _id_and_tags_copy(pool, &uni[n_set1], set2, TAGS_EMPTY, assertions);

        uni[n_set1 + n_set2].id = -1;
        uni[n_set1 + n_set2].tags = TAGS_EMPTY;

        return uni;
}


/*¶ Our next task is to write the emptymatch function.  We again break it up
into functions that deal with each kind of node.  The first will deal with leaf
nodes.  For leaves we need to deal with assertions and tags.  Assertions are
ORed onto those already defined for this emptymatch.  A tag is added to the set
of tags that can start a match at this node. */

static void ast_node_match_empty(ASTNode *node, int *tags,
                                 Assertion *assertions, int *n_tags);

static void
_ast_node_match_empty_leaf(ASTNode *node, int *tags, Assertion *assertions,
                          int *n_tags)
{
        Leaf *leaf = node->data.leaf;

        switch (leaf->type) {
        case LEAF_EMPTY:
                break;
        case LEAF_ASSERTION:
                if (assertions != NULL)
                        *assertions |= leaf->data.assertion;
                break;
        case LEAF_TAG:
                if (leaf->data.tag >= 0) {
                        if (tags != NULL) {
                                int i;

                                for (i = 0; tags[i] >= 0; i++)
                                        if (tags[i] == leaf->data.tag)
                                                break;

                                if (tags[i] < 0) {
                                        tags[i] = leaf->data.tag;
                                        tags[i + 1] = -1;
                                }
                        }

                        if (n_tags != NULL)
                                (*n_tags)++;
                }
                break;
        default:
                assert(false);
                break;
        }
}

/*¶ As you perhaps can tell, this procedure will be invoked twice, once to
figure out how many tags will be needed, and once for setting up the tags and
assertions. */


/*¶ For concatenations, there’s really not much more to do than invoke
recursively. */

static void
_ast_node_match_empty_cons(ASTNode *node, int *tags, Assertion *assertions,
                          int *n_tags)
{
        ASTNode *left = node->data.cons->left, *right = node->data.cons->right;

        assert(left->nullable);
        assert(right->nullable);

        ast_node_match_empty(left, tags, assertions, n_tags);
        ast_node_match_empty(right, tags, assertions, n_tags);
}


/*¶ The same goes for iteration nodes, except that we only care to do so if
their operand is nullable: */

static void
_ast_node_match_empty_iter(ASTNode *node, int *tags, Assertion *assertions,
                          int *n_tags)
{
        ASTNode *atom = node->data.iter->atom;

        if (atom->nullable)
                ast_node_match_empty(atom, tags, assertions, n_tags);
}


/*¶ For a union, again there’s really not much happening: */

static void
_ast_node_match_empty_union(ASTNode *node, int *tags, Assertion *assertions,
                          int *n_tags)
{
        ASTNode *left = node->data.uni->left, *right = node->data.uni->right;

        assert(left->nullable || right->nullable);

        if (left->nullable)
                ast_node_match_empty(left, tags, assertions, n_tags);
        else
                ast_node_match_empty(right, tags, assertions, n_tags);
}


/*¶ Finally, the \C{ast_node_match_empty} function that is called initially and
recursively, which is simply does function dispatching. */

static void
ast_node_match_empty(ASTNode *node, int *tags, Assertion *assertions,
                     int *n_tags)
{
        static void (*dispatch[])(ASTNode *, int *, Assertion *, int *) = {
                [AST_NODE_LEAF] = _ast_node_match_empty_leaf,
                [AST_NODE_CONS] = _ast_node_match_empty_cons,
                [AST_NODE_ITER] = _ast_node_match_empty_iter,
                [AST_NODE_UNION] = _ast_node_match_empty_union,
        };

        assert(node->type < sizeof(dispatch) / sizeof(dispatch[0]));
        assert(dispatch[node->type] != NULL);
        dispatch[node->type](node, tags, assertions, n_tags);
}


/*¶ Now that we’ve defined our emptymatch function we can compute the nullable,
firstid, and lastid functions.  As these can be done concurrently, we do so.
Yet again, we break our work up into helper||functions, and deal with our nodes
in the same order as always, leaf, concatenation, iteration, and finally union.
\CDef[ast_node_compute_nfl]
*/

static void ast_node_compute_nfl(MemPool *pool, ASTNode *node);

/*¶ For leaf nodes, literals will not be nullable and will have firstid and lastid
equal to the leaf’s id.  For any other leaf||node type, the node will be
nullable and will have empty sets of ids for firstid and lastid. */

static void
_ast_node_compute_nfl_leaf(MemPool *pool, ASTNode *node)
{
        Leaf *leaf = node->data.leaf;

        if (leaf->type == LEAF_LITERAL) {
                node->nullable = false;
                node->firstid = id_and_tags_new_one(pool,
                                                    leaf->id,
                                                    &leaf->data.literal);
                node->lastid = id_and_tags_new_one(pool,
                                                   leaf->id,
                                                   &leaf->data.literal);
        } else {
                node->nullable = true;
                node->firstid = id_and_tags_new_empty(pool);
                node->lastid = id_and_tags_new_empty(pool);
        }
}


/*¶ The nullable, firstid, and lastid functions of a concatenation node depends
on its children.  As firstid and lastid are the same but with the subtrees
reversed, we write a helper||function that computes these functions for us: */

static IDAndTags *
_nfl_cons_id(MemPool *pool, ASTNode *node, IDAndTags *set1, IDAndTags *set2)
{
        int n_tags = 0;

        ast_node_match_empty(node, NULL, NULL, &n_tags);

        int tags[n_tags + 1];
        tags[0] = -1;
        Assertion assertions = ASSERTION_NONE;

        ast_node_match_empty(node, tags, &assertions, NULL);

        return id_and_tags_new_union(pool, set1, set2, tags, assertions);
}


/*¶ And now for concatenation nodes: */

static void
_ast_node_compute_nfl_cons(MemPool *pool, ASTNode *node)
{
        Cons *cons = node->data.cons;

        ast_node_compute_nfl(pool, cons->left);
        ast_node_compute_nfl(pool, cons->right);

        node->nullable = cons->left->nullable && cons->right->nullable;

        if (cons->left->nullable)
                node->firstid = _nfl_cons_id(pool, cons->left,
                                             cons->right->firstid,
                                             cons->left->firstid);
        else
                node->firstid = cons->left->firstid;

        if (cons->right->nullable)
                node->lastid = _nfl_cons_id(pool, cons->right,
                                            cons->left->lastid,
                                            cons->right->lastid);
        else
                node->lastid = cons->right->lastid;
}

/*¶ Iteration nodes are simple: */

static void
_ast_node_compute_nfl_iter(MemPool *pool, ASTNode *node)
{
        Iter *iter = node->data.iter;

        ast_node_compute_nfl(pool, iter->atom);

        node->nullable = (iter->min == 0 || iter->atom->nullable);
        node->firstid = iter->atom->firstid;
        node->lastid = iter->atom->lastid;
}


/*¶ Unions are easy too: */

static void
_ast_node_compute_nfl_union(MemPool *pool, ASTNode *node)
{
        ASTNode *left = node->data.uni->left, *right = node->data.uni->right;

        ast_node_compute_nfl(pool, left);
        ast_node_compute_nfl(pool, right);

        node->nullable = left->nullable || right->nullable;
        node->firstid = id_and_tags_new_union(pool, left->firstid,
                                              right->firstid, TAGS_EMPTY,
                                              ASSERTION_NONE);
        node->lastid = id_and_tags_new_union(pool, left->lastid,
                                             right->lastid, TAGS_EMPTY,
                                             ASSERTION_NONE);
}


/*¶ Finally, we need a dispatcher for these functions: */

static void
ast_node_compute_nfl(MemPool *pool, ASTNode *node)
{
        static void (*dispatch[])(MemPool *, ASTNode *) = {
                [AST_NODE_LEAF] = _ast_node_compute_nfl_leaf,
                [AST_NODE_CONS] = _ast_node_compute_nfl_cons,
                [AST_NODE_ITER] = _ast_node_compute_nfl_iter,
                [AST_NODE_UNION] = _ast_node_compute_nfl_union,
        };

        assert(node->type < sizeof(dispatch) / sizeof(dispatch[0]));
        assert(dispatch[node->type] != NULL);
        dispatch[node->type](pool, node);
}


/* TODO: possible optimization here */

/*¶ The last part of our \AST\ to \TNFA\ transformation is to turn sets of ids
and tags into transitions.  We first need a function that unites two sets of
tags that will later be part of a transition. */

static int *
ast_add_transition_create_tags(int *a, int *b)
{
        int n_a = 0, n_b = 0;

        array_find(a[n_a] < 0, n_a);
        array_find(b[n_b] < 0, n_b);

        if (n_a + n_b == 0)
                return TAGS_EMPTY;

        int *tags = ALLOC_N(int, n_a + n_b + 1);

        MEMCPY(tags, a, int, n_a);

        int t = n_a;
        /* copy, skipping duplicates */
        for (int i = 0; i < n_b; i++) {
                bool found = false;

                for (int j = 0; !found && j < n_a; j++)
                        if (b[i] == tags[j])
                                found = true;

                if (!found)
                        tags[t++] = b[i];
        }

        tags[t++] = -1;

        if (t < n_a + n_b + 1) {
                int *re = REALLOC_N(tags, int, t);
                tags = re;
        }

        return tags;
}

/*¶ We remove any duplicate tags, as there’s no point in having more than one
of the same tag in the set. */


/*¶ Next, let’s write a function that creates a transition between two
id||and||tag sets: */

static void
ast_add_transition(IDAndTags *from, IDAndTags *to, Transition *transitions,
                   int *offsets)
{
        Transition *t = &transitions[offsets[from->id]];
        while (t->state != NULL)
                t++;

        if (t->state == NULL)
                t[1].state = NULL;

        if (t->tags != TAGS_EMPTY)
                free(t->tags);

        t->literal = from->literal;
        t->state = &transitions[offsets[to->id]];
        t->state_id = to->id;
        t->assertions = from->assertions | to->assertions;
        t->tags = ast_add_transition_create_tags(from->tags, to->tags);
}


/*¶ We also need a function that does this for a set of such sets: */

static void
ast_add_transitions(IDAndTags *from, IDAndTags *to, Transition *transitions,
                    int *counts, int *offsets)
{
        if (transitions == NULL) {
                for (IDAndTags *a = from; a->id >= 0; a++)
                        for (IDAndTags *b = to; b->id >= 0; b++)
                                counts[a->id]++;

                return;
        } 

        for (IDAndTags *a = from; a->id >= 0; a++) {
                int prev_b_id = -1;

                for (IDAndTags *b = to; b->id >= 0; b++) {
                        if (b->id == prev_b_id)
                                continue;

                        prev_b_id = b->id;

                        ast_add_transition(a, b, transitions, offsets);
                }
        }
}

/*¶ This is, yet again, one of those functions that will either count the
number of structures that will be needed or actually update such structures. */


/*¶ Now, then, we are ready to write a function that will use the above
function to turn an \AST\ into a \TNFA: */

static void
ast_to_tnfa(ASTNode *node, Transition *transitions, int *counts, int *offsets)
{
        switch (node->type) {
        case AST_NODE_LEAF:
                break;
        case AST_NODE_CONS: {
                Cons *cons = node->data.cons;

                ast_add_transitions(cons->left->lastid, cons->right->firstid,
                                    transitions, counts, offsets);
                ast_to_tnfa(cons->left, transitions, counts, offsets);
                ast_to_tnfa(cons->right, transitions, counts, offsets);
                break;
        }
        case AST_NODE_ITER: {
                Iter *iter = node->data.iter;

                assert(iter->max == -1 || iter->max == 1);

                if (iter->max == -1) {
                        assert(iter->min == 0 || iter->min == 1);
                        ast_add_transitions(iter->atom->lastid,
                                            iter->atom->firstid,
                                            transitions, counts, offsets);
                }

                ast_to_tnfa(iter->atom, transitions, counts, offsets);
                break;
        }
        case AST_NODE_UNION: {
                Union *uni = node->data.uni;

                ast_to_tnfa(uni->left, transitions, counts, offsets);
                ast_to_tnfa(uni->right, transitions, counts, offsets);
                break;
        }
        default:
                assert(false);
                break;
        }
}


/*¶ We also need a function that will create the set of transitions that will
represent the initial states of our \TNFA. */

static Transition *
create_initial(TNFA *tnfa, IDAndTags *first, int *offsets)
{
        int count = 0;

        array_find(first[count].id < 0, count);

        Transition *initial = CALLOC_N(Transition, count + 1);
        Transition *t = initial;

        for (IDAndTags *p = first; p->id >= 0; p++, t++) {
                t->state = &tnfa->transitions[offsets[p->id]];
                t->state_id = p->id;
                t->assertions = p->assertions;

                int n;
                array_find(p->tags[n] < 0, n);

                if (n > 0) {
                        t->tags = ALLOC_N(int, n + 1);
                        MEMCPY(t->tags, p->tags, int, n + 1);
                } else {
                        t->tags = TAGS_EMPTY;
                }
        }

        t->state = NULL;

        return initial;
}


/*¶ We are almost done.  The final function simply brings together all the
functions that operate on our \AST\ in transforming it into a \TNFA: */

/* TODO: does submatch_data need to be cleared here? */
HIDDEN void
ast_compile(MemPool *pool, TNFA *tnfa, ASTNode *tree, int n_nodes,
            int n_submatches)
{
        tnfa->n_submatches = n_submatches;

        ast_node_add_tags(NULL, tree, tnfa);

        if (tnfa->n_tags > 0) {
                tnfa->tag_directions = ALLOC_N(TagDirection, tnfa->n_tags + 1);
                MEMSET(tnfa->tag_directions, -1, TagDirection, tnfa->n_tags + 1);
        } else {
                tnfa->tag_directions = NULL;
        }

        tnfa->minimal_tags = ALLOC_N(int, tnfa->n_tags * 2 + 1);
        tnfa->submatch_data = ALLOC_N(SubmatchData, tnfa->n_submatches);

        ast_node_add_tags(pool, tree, tnfa);

        int n_minimals;
        array_find(tnfa->minimal_tags[n_minimals] < 0, n_minimals);
        int *r = REALLOC_N(tnfa->minimal_tags, int, n_minimals + 1);
        tnfa->minimal_tags = r;

        ast_expand(pool, tree, &n_nodes, tnfa->tag_directions);

        ASTNode *hash = ast_node_literal_new_char(pool, '\0', n_nodes++);
        tree = ast_node_cons_new(pool, tree, hash);

        ast_node_compute_nfl(pool, tree);

        int counts[n_nodes];
        MEMZERO(counts, int, n_nodes);

        ast_to_tnfa(tree, NULL, counts, NULL);

        int sum = 0;
        int offsets[n_nodes];
        for (int i = 0; i < n_nodes; i++) {
                offsets[i] = sum;
                sum += counts[i] + 1;
        }

        tnfa->transitions = CALLOC_N(Transition, sum + 1);
        tnfa->n_transitions = sum;

        ast_to_tnfa(tree, tnfa->transitions, NULL, offsets);

        tnfa->initial = create_initial(tnfa, tree->firstid, offsets);
        tnfa->final = &tnfa->transitions[offsets[tree->lastid[0].id]];
        tnfa->n_states = n_nodes;
}
