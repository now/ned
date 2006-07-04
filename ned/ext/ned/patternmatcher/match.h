/*
 * contents: Match class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{Matches in Ruby}

As has already been shown, we set up matches to be returned by our pattern
matcher as a result whenever we find a match.  This section will cover the
match class that represent these matches in Ruby. */


/*¶ We begin by setting up a macro that will, given a Ruby value, extract the
\CLang\ data structure: */

#define VALUE2MATCH(value, match)       \
        Data_Get_Struct((value), Match, (match))

/*¶ This macro is simply a convenience wrapper around a macro provided by the
Ruby \CLang\ interface. */


/*¶ The match data structure itself is simply a pair of offsets defining the
beginning and the end of a range within the input: */

typedef struct _Match Match;

struct _Match {
        off_t begin;
        off_t end;
};


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


VALUE match_new(off_t begin, off_t end);
void Init_Match(VALUE cPatternMatcher);
