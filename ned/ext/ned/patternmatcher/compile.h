/*
 * contents: Compile an AST of a regular expression into a TNFA.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{Compiling {\AST}s to {\TNFA}s}

Now that we can construct an \AST\ from our \NRE, it’s time that we write some
functions to turn that \AST\ into something that we can use for our
pattern||matching, namely a \TNFA\ representing that \AST.

Before we begin, though, we need a couple of data structures to help us along
the way. */


/*¶ We begin with a declaration for a variable that will represent an empty set
of tags.  This will be save some space for transitions that don’t have any tags
associated with them and is easier to deal with than a \C{NULL} pointer. */

extern int *g_tags_empty;

#define TAGS_EMPTY (g_tags_empty)


/*¶ The first such data structure is the one that will represent transitions in
a \TNFA: */

typedef struct _Transition Transition;

struct _Transition {
        Transition *state;
        int state_id;
        int *tags;
        Assertion assertions;
        Literal literal;
};

/*¶ The fields should speak for themselves.  We point to a target state with
the \C{state} field, and this state is uniquely identified by \C{state_id} in
the automaton.  \C{tags} is a set of tags that should be updated whenever this
transition is taken.  The last two fields make assertions about the input, both
the position of the input iterator and what it is over. */


/*¶ We need to tell whether a tag is to be minimized or maximized: */

typedef enum {
        TAG_MINIMIZE,
        TAG_MAXIMIZE,
} TagDirection;


/*¶ Every submatch to be addressed needs some extra information: */

typedef struct _SubmatchData SubmatchData;

struct _SubmatchData {
        int begin_tag;
        int end_tag;
        int *parents;
};

/*¶ We will, again, declare a variable that will represent an empty set; this
time an empty set of parents for use with the \C{parents} field of a submatch
data structure: */

extern int *g_parents_empty;

#define PARENTS_EMPTY (g_parents_empty)



/*¶ The two first tags identify what tags begin and end the submatch.  The last
field identifies any tags that are part of a submatch this submatch is a part
of. */


/*¶ Now we can define our \TNFA\ structure: */

typedef struct _TNFA TNFA;

struct _TNFA {
        Transition *transitions;
        int n_transitions;
        Transition *initial;
        Transition *final;
        SubmatchData *submatch_data;
        int n_submatches;
        TagDirection *tag_directions;
        int *minimal_tags;
        int n_tags;
        int end_tag;
        int n_states;
};

/*¶ Complicated, yes, but most of the fields are actually only used during
initialization of the execution code later on. */


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


void ast_compile(MemPool *pool, TNFA *tnfa, ASTNode *tree, int n_nodes, int n_submatches);
