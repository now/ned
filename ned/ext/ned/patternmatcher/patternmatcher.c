/*
 * contents: PatternMatcher class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

/* NOTE: always include ruby.h before anything else, as it toggles on some
 * various flags in the headers that it includes. */
#include <ruby.h>
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


/*¶ \section{The Ruby Pattern||Matcher}

The final part of this discussion is devoted to the Ruby interface to the
pattern||matcher.  */

/*¶ We begin, again, with a few macros and type definitions that we’ll use in
the rest of the code: */

#define PATTERNMATCHER2VALUE(pm)            \
        Data_Wrap_Struct(s_cPatternMatcher, NULL, pattern_matcher_free, (pm))

#define VALUE2PATTERNMATCHER(value, pm)     \
        Data_Get_Struct((value), PatternMatcher, (pm))


typedef struct _PatternMatcher PatternMatcher;

struct _PatternMatcher {
        TNFA *tnfa;
        char *source;
};

static ID s_cPatternMatcher;

HIDDEN ID g_ePatternError;
HIDDEN ID g_id_read;

/*¶ The \C{source} field of the pattern||matcher data structure is simply the
textual representation of the regular expression used as an input pattern. */


/*¶ Ruby needs a way to free data structures associated with Ruby objects
whenever they are about to be claimed by the garbage collector.  We have
already used the function above, in the definition of the
\C{PATTERNMATCHER2VALUE} macro, but we’ve deferred defining it until now. */

static void
pattern_matcher_free(PatternMatcher *pm)
{
        TNFA *tnfa = pm->tnfa;

        for (int i = 0; i < tnfa->n_transitions; i++)
                if (tnfa->transitions[i].state != NULL &&
                    tnfa->transitions[i].tags != TAGS_EMPTY)
                        free(tnfa->transitions[i].tags);

        free(tnfa->transitions);

        if (tnfa->initial != NULL) {
                for (Transition *t = tnfa->initial; t->state != NULL; t++)
                        if (t->tags != TAGS_EMPTY)
                                free(t->tags);
                free(tnfa->initial);
        }

        if (tnfa->submatch_data != NULL) {
                for (int i = 0; i < tnfa->n_submatches; i++)
                        if (tnfa->submatch_data[i].parents != PARENTS_EMPTY)
                                free(tnfa->submatch_data[i].parents);
                free(tnfa->submatch_data);
        }

        free(tnfa->tag_directions);
        free(tnfa->minimal_tags);
        free(tnfa);

        free(pm);
}

/*¶ Phew, a lot of stuff that needs to be freed.  We also need to be careful
not to try to free the values that represent our empty set of tags and parents.
Other than that, though, it’s more or less routine. */


/*¶ Just as we need a way for Ruby to free up resources when a pattern||matcher
is going out of scope, we need a way for Ruby to allocate those resources in
the first place: */

static VALUE
pattern_matcher_s_allocate(UNUSED(VALUE class))
{
        PatternMatcher *pm = ALLOC(PatternMatcher);

        pm->tnfa = CALLOC_N(TNFA, 1);
        pm->source = NULL;

        return PATTERNMATCHER2VALUE(pm);
}

/*¶ This isn’t the whole story, though.  The rest of the allocation actually
takes place during initialization of the object: */

static VALUE
pattern_matcher_initialize(int argc, VALUE *argv, VALUE self)
{
        VALUE pattern, rules;

        rb_scan_args(argc, argv, "11", &pattern, &rules);

        MemPool *pool = mem_pool_new();

        int n_nodes, n_submatches;
        ASTNode *tree = pattern_parse(pool, pattern, rules,
                                      &n_nodes, &n_submatches);

        PatternMatcher *pm;
        VALUE2PATTERNMATCHER(self, pm);

        ast_compile(pool, pm->tnfa, tree, n_nodes, n_submatches);

        /*
        ast_node_print(tree);
        */

        mem_pool_free(pool);

        int len = RSTRING(pattern)->len;
        pm->source = ALLOC_N(char, len + 1);
        MEMCPY(pm->source, RSTRING(pattern)->ptr, char, len);
        pm->source[len] = '\0';

        return self;
}

/*¶ This more or less invokes the proper functions for parsing and compiling a
pattern.  We also make sure to save a copy of the pattern so that we can allow
a user to extract it later on. */


/*¶ Most of the functions that follow will be executing the pattern||matcher on
a given input source.  We need a function that checks that this source of input
is valid, so that we don’t wind up doing weird stuff: */

static VALUE
check_input(VALUE input, VALUE if_nil)
{
        if (NIL_P(input))
                return if_nil;

        if (NIL_P(rb_check_string_type(input)) &&
            !rb_respond_to(input, g_id_read))
                rb_raise(rb_eArgError,
                     "argument isn’t a string or doesn’t respond to “read”");

        return Qtrue;
}

/*¶ We require the input source to be either a string or an object that
responds to the “read” message. */

/*¶ Now, then, we can define a function that does pattern||matching by
executing a \TNFA: */

static VALUE
pattern_matcher_match(VALUE self, VALUE input)
{
        VALUE checked = check_input(input, Qnil);
        if (!RTEST(checked))
                return checked;

        PatternMatcher *pm;
        VALUE2PATTERNMATCHER(self, pm);

        VALUE matches = Qnil;
        return tnfa_matches(pm->tnfa, input, &matches) ? matches : Qnil;
}

/*¶ Very straightforward. */


/*¶ We also write a function that does more or less the same, but only returns
the beginning offset of the whole match upon a successful match, or nil
otherwise.  This is useful for doing simple checks for matches within an input
source. */

static VALUE
pattern_matcher_match_simple(VALUE self, VALUE input)
{
        VALUE checked = check_input(input, Qfalse);
        if (!RTEST(checked))
                return checked;

        PatternMatcher *pm;
        VALUE2PATTERNMATCHER(self, pm);

        VALUE matches = Qnil;
        if (!tnfa_matches(pm->tnfa, input, &matches))
            return Qnil;

        Match *match;
        VALUE2MATCH(RARRAY(matches)->ptr[0], match);

        return OFFT2NUM(match->begin);
}


/*¶ The final matching function returns a boolean that states whether there was
a match or not: */

static VALUE
pattern_matcher_match_eqq(VALUE self, VALUE input)
{
        VALUE checked = check_input(input, Qfalse);
        if (!RTEST(checked))
                return checked;

        PatternMatcher *pm;
        VALUE2PATTERNMATCHER(self, pm);

        return BOOL2VALUE(tnfa_matches(pm->tnfa, input, NULL));
}


/*¶ We mentioned earlier that we would provide a way for the user to retrieve
the source pattern.  Let’s write a function that does just that now: */

static VALUE
pattern_matcher_source(VALUE self)
{
        PatternMatcher *pm;

        VALUE2PATTERNMATCHER(self, pm);

        VALUE str;
        str = rb_str_new2(pm->source);
        if (OBJ_TAINTED(pm))
                OBJ_TAINT(str);

        return str;
}


/*¶ As for the match object, we also provide a way to inspect the contents of
a pattern||matcher object: */

static VALUE
pattern_matcher_inspect(VALUE self)
{
        PatternMatcher *pm;

        VALUE2PATTERNMATCHER(self, pm);

        char buf[INSPECT_BUFFER_SIZE];
        int len = snprintf(buf, INSPECT_BUFFER_SIZE,
                           "#<PatternMatcher:%p source=%s>",
                           pm, pm->source);
        return rb_str_new(buf, len);
}

/*¶ Again, this is more or less only useful for debugging. */


/*¶ Our final function will provide our users with a method to escape a string
literal so that it may be used as input to the pattern||matcher without
harm.  This is useful if the source pattern may contain symbols that would be
treated specially by the pattern||parser and this is undesirable. */

static VALUE
pattern_matcher_s_escape(UNUSED(VALUE class), VALUE str)
{
        StringValue(str);

        char *p = RSTRING(str)->ptr;

        if (!utf_isvalid_n(p, RSTRING(str)->len, NULL))
            rb_raise(rb_eArgError, "argument isn't valid UTF-8");

        char *pend = p + RSTRING(str)->len;

        size_t skip = strcspn(p, ".<[()]>*+?^$|\\");

        VALUE ret = rb_str_new(NULL, skip + (RSTRING(str)->len - skip) * 2);
        char *r = RSTRING(ret)->ptr;
        MEMCPY(r, p, char, skip);
        r += skip;
        p += skip;

        while (p < pend) {
                unichar c = utf_char(p);
                char *pnext = utf_next(p);

                switch (c) {
                case '.': case '<': case '[': case '(':
                case ')': case ']': case '>': case '*':
                case '+': case '?': case '^': case '$':
                case '|': case '\\':
                        *r++ = '\\';
                        *r++ = c;
                        break;
                default:
                        MEMCPY(r, p, char, pnext - p);
                        r += pnext - p;
                        break;
                }

                p = pnext;
        }

        rb_str_resize(ret, r - RSTRING(ret)->ptr);
        OBJ_INFECT(ret, str);

        return ret;
}

/*¶ We use a few tricks to avoid doing unnecessary work. */


/*¶ And, yet again, we define an interface to our code for Ruby.  This is
highly uninteresting, though, and don’t linger in reading through it: */

void Init_patternmatcher(void);

void Init_patternmatcher(void)
{
        g_id_read = rb_intern("read");

        s_cPatternMatcher = rb_define_class("PatternMatcher", rb_cObject);
        rb_define_alloc_func(s_cPatternMatcher, pattern_matcher_s_allocate);

        rb_define_singleton_method(s_cPatternMatcher, "compile",
                                   rb_class_new_instance, -1);
        rb_define_singleton_method(s_cPatternMatcher, "quote",
                                   pattern_matcher_s_escape, 1);
        rb_define_singleton_method(s_cPatternMatcher, "escape",
                                   pattern_matcher_s_escape, 1);

        rb_define_method(s_cPatternMatcher, "initialize",
                         pattern_matcher_initialize, -1);
        rb_define_method(s_cPatternMatcher, "match", pattern_matcher_match, 1);
        rb_define_method(s_cPatternMatcher, "=~",
                         pattern_matcher_match_simple, 1);
        rb_define_method(s_cPatternMatcher, "===",
                         pattern_matcher_match_eqq, 1);
        rb_define_method(s_cPatternMatcher, "source",
                         pattern_matcher_source, 0);
        rb_define_method(s_cPatternMatcher, "inspect",
                         pattern_matcher_inspect, 0);

        g_ePatternError = rb_define_class_under(s_cPatternMatcher,
                                                "PatternError",
                                                rb_eStandardError);

        Init_Match(s_cPatternMatcher);
}
