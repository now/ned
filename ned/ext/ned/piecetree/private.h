/*
 * contents: Private stuff for the library.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



#define INSPECT_BUFFER_SIZE     256

#define BOOL2VALUE(b)   ((b) ? Qtrue : Qfalse)

#if defined(__GNUC__)
#  define UNUSED(u)   \
        u __attribute__((__unused__))
#  define HIDDEN   \
        __attribute__((visibility("hidden")))
#else
#  define UNUSED(u)   \
        u
#  define HIDDEN
#endif
