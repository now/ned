/*
 * contents: Execute a TNFA.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <ned/unicode.h>
#include <sys/types.h>

#include "mempool.h"
#include "private.h"
#include "ast.h"
#include "parse.h"
#include "compile.h"
#include "execute.h"
#include "match.h"



/*¶ \section{Execution of a \TNFA}

We’ve reached the final part of our pattern||matching code.  This section
will deal with the actual execution of a compiled \TNFA.  We’ll be using the
modified version of a two||stack simulation of an \NFA. */

/*¶ The stacks in our simulation will consist of the following data
structure: */

typedef struct _Reach Reach;

struct _Reach {
        Transition *state;
        int *tags;
};

/*¶ That is, a given state is reachable (or was reachable during the previous
iteration), from the current state, reading the current input symbol, with the
given tag||value function. */

/*¶ To keep track of what tag||value function to choose when we reach a certain
state, we need to keep track of what position in the input that this state was
previously reached and what tag||value function was chosen that time: */

typedef struct _ReachPos ReachPos;

struct _ReachPos {
        off_t pos;
        int **tags;
};


/*¶ Last in our set||up is the context within which we’ll be executing our
automata: */

typedef struct _ExecutionContext ExecutionContext;

/* TODO: rn_iter could perhaps be called rnp */
struct _ExecutionContext {
        TNFA const * const tnfa;
        VALUE input;
        VALUE str;
        char *p;
        unichar prev;
        off_t pos;
        Reach *reach;
        Reach *reach_next;
        Reach *rn_iter;
        ReachPos *reach_pos;
        bool new_match;
        off_t match_end;
        int *tags;
        int *tmp_tags;
};

/*¶ Quite a few members, eh?  Sadly, they’re all necessary.  The first few
members deal with the input.  The \C{input} member is the base of the input
value from which we’ll be reading our symbols.  Next is \C{str}, which is a
string read from \C{input}.  This will either be equal to \C{input}, if it
itself was a string, or will be the result of invoking the read method on
\C{input}.  Finally, \C{p} is a pointer into the contents of \C{str}, i.e., it
can be thought of as the input cursor that we have been discussing earlier.
The \C{prev} and \C{pos} members are the current (well, depends on how you look
at it) input symbol and current position in the input.  Note that \C{pos} can’t
be a calculated value, as \C{input} may give us many \C{str}s, thus there’s no
chance of doing a simple pointer comparison.  Next are three members that
maintain the two stacks: \C{reach}, \C{reach_next}, and \C{rn_iter}.  The first
two are our stacks and the third is an iterator into \C{reach_next}.  This is
necessary, as this stack will be built incrementally during an execution||step
of the \TNFA.  The role of \C{reach_pos} has already been explained, but the
meaning of the members following it may remain elusive.  The first two are used
whenever a new match has been found, i.e., a final state has been reached.  The
last two maintain areas of memory that may be used for dealing with tags
throughout the code. */


/*¶ Now that we know what we’re going to be dealing with, we may begin writing
some functions that maintain our execution context.  First off are a couple of
functions that deal with the input to our automaton: */

static void
update_input(ExecutionContext *context, VALUE input)
{
        assert(!NIL_P(input));

        /*
        if (!utf_isvalid(RSTRING(input)->ptr))
                rb_raise(rb_eArgError, "input isn’t valid UTF-8-encoded text");
                */

        context->str = input;
        context->p = RSTRING(input)->ptr;
}

/*¶ This function simply checks that the new input is valid and then updates
the appropriate members. */


/*¶ This next function instead deals with retrieving more input from our input
source.  This will only be done when we have reached the end of our current
input and if the input source was of such a nature that it would provide us
with more input through the read method when asked. */

static bool
more_input(ExecutionContext *context)
{
        struct RString *str = RSTRING(context->str);

        if (context->p < str->ptr + str->len)
                return true;

        if (!NIL_P(rb_check_string_type(context->input)))
                return false;

        VALUE input = rb_funcall(context->input, g_id_read, 0);

        if (NIL_P(input))
                return false;

        update_input(context, input);

        return true;
}

/*¶ The return value will be true if there is more input to be read and false
otherwise. */


/*¶ Transitions may require that certain assertions about the state of the
machinery hold before it can be taken.  We thus need a function that checks
these assertions for us: */

static inline bool
passes_assertions(ExecutionContext *context, Assertion assertions)
{
        if (assertions == ASSERTION_NONE)
                return true;
        if ((assertions & ASSERTION_BOS) && context->pos != 0)
                return false;
        if ((assertions & ASSERTION_BOL) &&
            (context->pos != 0 || context->prev != '\n'))
                return false;
        if ((assertions & ASSERTION_EOL) && *context->p != '\n')
                return false;
        if (assertions & ASSERTION_EOS)
                return !more_input(context);

        return true;
}


/*¶ We need a way of updating a subset of a set of tags to the current position
in the input: */

static void
update_tags(ExecutionContext *context, int *tags, int *which)
{
        int n_tags = context->tnfa->n_tags;
        off_t pos = context->pos;

        for (int *tag = which; *tag >= 0; tag++)
                if (*tag < n_tags)
                        tags[*tag] = pos;
}


/*¶ We also need a way to mark that a new match has been found: */

static void
new_match(ExecutionContext *context, int *tags)
{
        context->new_match = true;
        context->match_end = context->pos;
        MEMCPY(context->tags, tags, int, context->tnfa->n_tags);
}

/*¶ We also copy over any tags associated with this new match, i.e., the
tag||value function. */


/*¶ This next function is used to push a new state onto the \C{reach_next}
stack.  There are a couple different ways of doing this, but we want to keep it
all within one function, so we need some flags to tell it what to do: */

typedef enum {
        SWAP_TAGS = 1 << 0,
        UPDATE_REACH_POS = 1 << 1
} PushFlags;


static void
reach_push(ExecutionContext *context, Transition *t, int **tags, PushFlags flags)
{
        if (flags & SWAP_TAGS) {
                int *tmp = context->rn_iter->tags;
                context->rn_iter->tags = *tags;
                *tags = tmp;

                context->rn_iter->state = t;
        }

        if (flags & UPDATE_REACH_POS) {
                context->reach_pos[t->state_id].pos = context->pos;
                context->reach_pos[t->state_id].tags = &context->rn_iter->tags;

                context->rn_iter->state = t->state;
        }

        context->rn_iter++;
}


/*¶ If we haven’t found a match yet, we add the initial states to the
\C{reach_next} stack: */

static void
add_initial(ExecutionContext *context)
{
        for (Transition *t = context->tnfa->initial; t->state != NULL; t++) {
                if (context->reach_pos[t->state_id].pos >= context->pos)
                        continue;

                if (!passes_assertions(context, t->assertions))
                        continue;

                MEMSET(context->rn_iter->tags, -1, int, context->tnfa->n_tags);

                update_tags(context, context->rn_iter->tags, t->tags);

                if (t->state == context->tnfa->final)
                        new_match(context, context->rn_iter->tags);

                reach_push(context, t, NULL, UPDATE_REACH_POS);
        }

        context->rn_iter->state = NULL;
}


/*¶ This next function implements the rules that we set out for deciding the
winner between two sets of tags: */

static inline bool
tags_win(int n_tags, TagDirection const * const tag_directions,
         int const * const a, int const * const b)
{
        for (int i = 0; i < n_tags; i++) {
                if (a[i] > b[i])
                        return (tag_directions[i] == TAG_MAXIMIZE);
                if (a[i] < b[i])
                        return (tag_directions[i] == TAG_MINIMIZE);
        }

        return false;
}


/*¶ Whenever we find a transition that should be taken, we need to update tags
associated with it, check if we have reached a final state and whether we
should mark it as a new match, and perhaps decide what tags we should use or
push the transition on the \C{reach_next} state. */

static void
deal_with_transition(ExecutionContext *context, Transition *t, Reach *r)
{
        MEMCPY(context->tmp_tags, r->tags, int, context->tnfa->n_tags);

        update_tags(context, context->tmp_tags, t->tags);

        if (context->reach_pos[t->state_id].pos < context->pos) {
                if (t->state == context->tnfa->final &&
                    (context->match_end == -1 ||
                     (context->tnfa->n_tags > 0 &&
                      context->tmp_tags[0] <= context->tags[0])))
                        new_match(context, context->tmp_tags);

                reach_push(context, t, &context->tmp_tags,
                           SWAP_TAGS | UPDATE_REACH_POS);
        } else {
                assert(context->reach_pos[t->state_id].pos == context->pos);

                if (tags_win(context->tnfa->n_tags,
                             context->tnfa->tag_directions,
                             context->tmp_tags,
                             *context->reach_pos[t->state_id].tags)) {
                        int *tmp = *context->reach_pos[t->state_id].tags;
                        *context->reach_pos[t->state_id].tags = context->tmp_tags;

                        if (t->state == context->tnfa->final)
                                new_match(context, context->tmp_tags);

                        context->tmp_tags = tmp;
                }
        }
}

/*¶ If we have reached a state and the current position is beyond the previous
one, i.e., we have an unvisited node, we check if we need to mark a new match
and then push the state onto the \C{reach_next} stack.  Otherwise, we check
what set of tags to use by using our rules for determining what set of tags
wins over the other.  If the new set of tags win, we need to choose that set of
tags and mark a new match. */


/*¶ We need a function that checks if a given transition is labeled with the
current input symbol: */

static inline bool
has_literal(ExecutionContext *context, Transition *t)
{
        switch (t->literal.type) {
        case LITERAL_TYPE_CHAR:
                return t->literal.data.c == context->prev;
        case LITERAL_TYPE_RANGE:
                return context->prev >= t->literal.data.range.begin &&
                       context->prev <= t->literal.data.range.end;
        case LITERAL_TYPE_PREDICATE:
                return t->literal.data.is_ctype(context->prev);
        default:
                /* TODO: assert(false) ? */
                return false;
        }
}


/*¶ For each state in the \C{reach} stack, check if there’s a transition
leaving it, labeled with the current input symbol and passes any assertions
associated with it, to a state not in the \C{reach_next} stack.  If so, add the
destination state to the \C{reach_next} stack. */

static void
check_reach(ExecutionContext *context)
{
        context->rn_iter = context->reach_next;

        for (Reach *r = context->reach; r->state != NULL; r++)
                for (Transition *t = r->state; t->state != NULL; t++)
                        if (has_literal(context, t) &&
                            passes_assertions(context, t->assertions))
                                deal_with_transition(context, t, r);

        context->rn_iter->state = NULL;
}


/*¶ The following four functions all deal with setting up a set of matches,
once we have found them.  The first one decides whether to use the value of the
given tag as defined by the tag||value function associated with the match, or
the end of the match if it is the end||tag: */

static inline off_t
submatch_tag_or_end(ExecutionContext *context, int tag)
{
        return (tag == context->tnfa->end_tag)
                ? context->match_end
                : context->tags[tag];
}


/*¶ We also need a function that creates the matches from the submatch data
associated with the \TNFA: */

static VALUE
new_matches_from_submatch_data(ExecutionContext *context)
{
        int n_submatches = context->tnfa->n_submatches;
        SubmatchData *data = context->tnfa->submatch_data;

        VALUE matches = rb_ary_new2(n_submatches);

        for (int i = 0; i < n_submatches; i++) {
                off_t begin = submatch_tag_or_end(context, data[i].begin_tag);
                off_t end = submatch_tag_or_end(context, data[i].end_tag);

                if (begin == -1 || end == -1)
                        begin = end = -1;

                rb_ary_push(matches, match_new(begin, end));
        }

        return matches;
}


/*¶ As it may be the case that some matches aren’t contained within their
parents, we need to go through the set of matches and reset all such
matches: */

static void
update_from_parents(VALUE const * const matches, int submatch,
                    int const * const parents)
{
        Match *match;
        
        VALUE2MATCH(matches[submatch], match);

        assert(match->end == -1 ? match->begin == -1 : true);
        assert(match->begin <= match->end);

        for (int j = 0; parents[j] >= 0; j++) {
                Match *parent;

                VALUE2MATCH(matches[parents[j]], parent);

                if (match->begin < parent->begin || match->end > parent->end)
                        match->begin = match->end = -1;
        }
}


/*¶ Finally, here’s the function that puts together the previous three
functions into one function for creating the set of matches: */

static VALUE
create_matches(ExecutionContext *context)
{
        VALUE matches = new_matches_from_submatch_data(context);
        SubmatchData *data = context->tnfa->submatch_data;

        for (int i = 0; i < context->tnfa->n_submatches; i++)
                update_from_parents(RARRAY(matches)->ptr, i, data[i].parents);

        return matches;
}


/*¶ We will swap the role of the two stacks upon each iteration: */

static inline void
swap_stacks(ExecutionContext *context)
{
        Reach *r = context->reach;

        context->reach = context->reach_next;
        context->reach_next = r;
}


/*¶ In the function following this next one we will be removing states that
don’t match the minimal matching conditions set up for a set of tags.  This
helper||function goes through a set of such tags and checks if a given set of
tags contains any such tags.  If this is the case, the return value will be
true; false otherwise. */

static bool
has_minimal(ExecutionContext *context, int *tags)
{
        int *minimal_tags = context->tnfa->minimal_tags;

        for (int i = 0; minimal_tags[i] >= 0; i += 2) {
                int end = minimal_tags[i];
                int begin = minimal_tags[i + 1];

                /*
                if ((end >= context->tnfa->n_tags && end !=
                     context->tnfa->end_tag) ||
                    (tags[begin] == context->tags[begin] &&
                     tags[end] < context->tags[end]))
                        return true;
                */
                if (end >= context->tnfa->n_tags ||
                    (tags[begin] == context->tags[begin] &&
                     tags[end] < context->tags[end]))
                        return true;
        }

        return false;
}


/*¶ We need a way to weed out any states that don’t fulfill the minimal
matching associated with such states’ tags from the \C{reach} stack. */

static void
weed_out_nonminimals(ExecutionContext *context)
{
        context->new_match = false;
        context->rn_iter = context->reach_next;

        for (Reach *r = context->reach; r->state != NULL; r++)
                if (!has_minimal(context, r->tags))
                        reach_push(context, r->state, &r->tags, SWAP_TAGS);

        context->rn_iter->state = NULL;

        swap_stacks(context);
}


/*¶ Before we begin executing our automaton, we need to set up the initial
input: */

/* TODO: the first check should be unnecessary, do assert() instead */
static bool
setup_initial_input(ExecutionContext *context, VALUE input)
{
        VALUE str;

        if (NIL_P(input))
                return false;

        if (NIL_P(str = rb_check_string_type(input)))
                str = rb_funcall(input, g_id_read, 0);

        if (NIL_P(str))
                return false;

        context->input = input;
        update_input(context, str);

        return true;
}

/*¶ If the input isn’t a string, we assume that it responds to the read message
and we get our input string that way.  Anyway, we update the appropriate
members and return a boolean stating whether we got any input to read or not. */


/*¶ The next function simply eats an input symbol and updates the appropriate
members of the execution context: */

static inline void
eat_input(ExecutionContext *context)
{
        context->prev = utf_char(context->p);
        context->p = utf_next(context->p);
        context->pos++;
}


/*¶ This is the main loop of our matching.  We check if we have to add the
initial states or whether we have found a match.  Next, we check if we have
more input to read and, if so, read one symbol’s worth.  We then swap the
stacks, weed out any states reached by taking non||minimal transitions, and
finally check for new states reached; then we begin again. */

static void
do_matches(ExecutionContext *context)
{
        int n_tags = context->tnfa->n_tags;
        bool has_minimal_tags = (context->tnfa->minimal_tags[0] != -1);

        while (true) {
                if (context->match_end == -1)
                        add_initial(context);
                else
                        if (n_tags == 0 ||
                            context->rn_iter == context->reach_next)
                                break;

                if (!more_input(context))
                        break;

                eat_input(context);

                swap_stacks(context);

                if (has_minimal_tags && context->new_match)
                        weed_out_nonminimals(context);

                check_reach(context);
        }
}


/* TODO: actually, we don’t need to MEMZERO anything, as they will be
 * initialized appropriately before they are used. */
HIDDEN bool
tnfa_matches(TNFA const * const tnfa, VALUE input, VALUE *matches)
{
        Reach reach[tnfa->n_states + 1];
        Reach reach_next[tnfa->n_states + 1];
        ReachPos reach_pos[tnfa->n_states];

        int r_and_rn_tags[2][tnfa->n_states][tnfa->n_tags];

        for (int i = 0; i < tnfa->n_states; i++) {
                reach[i].tags = r_and_rn_tags[0][i];
                reach_next[i].tags = r_and_rn_tags[1][i];
                reach_pos[i].pos = -1;
        }

        int tags[tnfa->n_tags];
        int tmp_tags[tnfa->n_tags];

        ExecutionContext context = {
                .tnfa = tnfa,
                .prev = '\0',
                .pos = 0,
                .reach = reach,
                .reach_next = reach_next,
                .rn_iter = reach_next,
                .reach_pos = reach_pos,
                .tags = tags,
                .tmp_tags = tmp_tags,
                .new_match = false,
                .match_end = -1
        };

        if (!setup_initial_input(&context, input))
                return false;

        do_matches(&context);

        if (context.match_end < 0)
                return false;

        if (matches != NULL)
                *matches = create_matches(&context);

        return true;
}
