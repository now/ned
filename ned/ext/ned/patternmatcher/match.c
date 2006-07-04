/*
 * contents: Match class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <stdint.h>
#include <sys/types.h>

#include "private.h"
#include "match.h"



/*¶ We also need a way of turning a match \CLang\ data structure into a Ruby
value: */

#define MATCH2VALUE(match)              \
        Data_Wrap_Struct(s_cMatch, NULL, free, (match))


/*¶ The Ruby identifier for our match class will be stored in the following
variable (and is used in the macro above): */

static ID s_cMatch;


/*¶ To create a match, all we need is two offsets: */

HIDDEN VALUE
match_new(off_t begin, off_t end)
{
        Match *match = ALLOC(Match);

        match->begin = begin;
        match->end = end;

        return MATCH2VALUE(match);
}

/*¶ We wrap the data structure up and return it, giving a Ruby variable as a
result. */


/*¶ The rest of the functions defined here are only invoked from the Ruby side,
i.e., they implement the Ruby interface to a match.  They aren’t very
interesting, but are nonetheless included here for completeness: */


/*¶ This function simply retrieves the \C{begin} member of the match.  The next
one retrieves the \C{end} member. */

static VALUE
match_begin(VALUE self)
{
        Match *match;

        VALUE2MATCH(self, match);

        return OFFT2NUM(match->begin);
}


static VALUE
match_end(VALUE self)
{
        Match *match;

        VALUE2MATCH(self, match);

        return OFFT2NUM(match->end);
}

/*¶ Here we have a function that generates a textual representation of a match,
mainly used for debugging purposes: */

static VALUE
match_inspect(VALUE self)
{
        Match *match;

        VALUE2MATCH(self, match);

        char buf[INSPECT_BUFFER_SIZE];
        int len = snprintf(buf, INSPECT_BUFFER_SIZE,
                           "#<PatternMatcher::Match:%p begin=%jd end=%jd>",
                           match,
                           (intmax_t)match->begin,
                           (intmax_t)match->end);
        return rb_str_new(buf, len);
}


/*¶ We need a way to tell Ruby what the interface should be, so here it is: */

HIDDEN void
Init_Match(VALUE cPatternMatcher)
{
        s_cMatch = rb_define_class_under(cPatternMatcher, "Match", rb_cObject);
        rb_undef_method(CLASS_OF(s_cMatch), "new");

        rb_define_method(s_cMatch, "begin", match_begin, 0);
        rb_define_method(s_cMatch, "end", match_end, 0);
        rb_define_method(s_cMatch, "inspect", match_inspect, 0);
}
