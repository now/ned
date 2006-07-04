/*
 * contents: Private definitions for the library.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{Prelude}

Before we begin, we need to set up some macros and external symbols.  Ruby’s
\CLang\ interface provides us with quite a few utility macros for dealing with
memory allocation and memory operations, but it’s not complete.  We lack a
clearing allocator and a memset wrapper: */

#define CALLOC_N(type, n)       MEMZERO(ALLOC_N(type, (n)), type, (n))
#define CALLOC(type)            CALLOC_N(type, 1)
#define MEMSET(p, c, type, n)   memset((p), (c), sizeof(type) * (n))


/*¶ These two macros make our life a lot more simple, as we don’t need to worry
about remembering to include \C{sizeof} in our calculations. */


/*¶ Towards the end, when we write our Ruby interface, we will define a method
that will allow users to inspect a given Ruby value.  We need a buffer for
creating the string that will represent our Ruby values, so we define a global
buffer size to be used for such buffers: */

#define INSPECT_BUFFER_SIZE 256


/*¶ The \CLang\ language is rather primitive and doesn’t provide an easy way to
define a $\max$ function that can be used for any numerical type defined in the
language.  We can, however, define a macro that works more or less the same as
a function would: */

#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

/*¶ The main problem with this is that macros in \CLang\ are pure substitution
macros, i.e., whatever text is given as arguments to the \C{MAX} macro will
occur twice in the expansion.  This means that if any of arguments has
side-effects, all hell may break loose; so one needs to be careful in using
macros of this kind. */


/*¶ Ruby doesn’t give us an easy way to turn a \CLang\ boolean into a Ruby
value, so we give ourselves one: */

#define BOOL2VALUE(b)   ((b) ? Qtrue : Qfalse)


/*¶ Unbeknownst to anyone reading this manuscript, the source||code that this
documentation was extracted from is actually split up into a number of files.
This causes some issues as to what symbols will need to be exported between
object files and what symbols will need to be exported from the resulting
library;  we want some of both.  The \GNU\ \CLang\ compiler allows us to decide
exactly what to do with each symbol, so we wrap up this extension in a macro so
that we can disable it for any compiler that doesn't support it: */

#if defined(__GNUC__)
#  define UNUSED(u)   \
        u __attribute__((__unused__))
#  define HIDDEN   \
        __attribute__((visibility("hidden")))
#else
#  define UNUSED(u)   \
        u
#  define HIDDEN(u)
#endif

/*¶ We also sneaked in a nice macro for demarkating unused parameters.  Sadly,
the Ruby interface will have a few of these, as it gives us some arguments that
we don’t actually need. */


/*¶ Finally, we declare two external symbols that are part of the Ruby
interface that will be used at various places in the code: */

extern ID g_ePatternError;
extern ID g_id_read;

/*¶ The first identifies a Ruby error class that we will define and use for
errors that relate to our library.  The second identifies the method named
\type{read}, which will be used for certain arguments to the pattern||matcher.
*/
