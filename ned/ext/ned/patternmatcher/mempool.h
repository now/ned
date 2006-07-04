/*
 * contents: Memory pools allow for memory allocation saving (space/time) and deallocate contents en masse.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \section{Intermission: Allocating Memory Efficiently}

We will continue our discussion on creating an \AST\ from an \NRE\ briefly.
First, however, we need to introduce a method for allocating memory that we
will use when creating such an \AST.

Memory allocation and deallocation can be troublesome business.  Both tasks
must be performed at just the right time and done as efficiently as possible,
both in terms of space and time.

When allocating memory for an \AST\ we can save ourselves a lot of trouble by
allocating all necessary memory from a common pool of memory.  This allows us
to free all allocated memory en masse, saving us a lot of time as memory will
only need to be allocated when the pool becomes empty, and may also save us
some space as allocated objects are kept together, thus reducing memory
fragmentation.  We do pay a price for this, however, as more memory than
necessary will be allocated at any given instance, and objects may not be
fitted into the memory pool in a space||optimal fashion.  The simplicity of
this scheme, coupled with its positive attributes, outweighs these issues.

Let's begin by declaring our memory||pool data type: */

typedef struct _MemPool MemPool;

/*¶ The specifics of this data type will be shown soon.  First, here is the
interface to the memory||pool module: */

MemPool *mem_pool_new(void);
void mem_pool_free(MemPool *pool);

void *mem_pool_alloc(MemPool *pool, size_t size);

/*¶ That's it; that's all.  There's a way to create a new memory pool, a way
to release it from memory, and a way to allocate memory within such a pool.
This way of allocating memory isn't very programmer friendly, though, so
there's also a set of utility macros that aid us in the use of this
interface: */

#define POOL_ALLOC(pool, type)          \
        ((type *)mem_pool_alloc(pool, sizeof(type)))
#define POOL_ALLOC_N(pool, type, n)     \
        ((type *)MEMZERO(mem_pool_alloc(pool, sizeof(type) * (size_t)(n)), \
                         type, (n)))
#define POOL_CALLOC(pool, type)         \
        ((type *)MEMZERO(mem_pool_alloc(pool, sizeof(type)), type, 1))

/*¶ Here are some examples of how to use these macros:

\startC
MemPool *pool = mem_pool_new();

int *p = POOL_ALLOC(pool, int);
*p = 10;

int *ary = POOL_ALLOC(pool, int, *p);
for (int i = 0; i < *p; i++)
        ary[i] = i;

*p = POOL_CALLOC(pool, int);    // *p will be set to 0.
\stopC

It may seem that the interface is lacking a \C{POOL_CALLOC_N} macro, but it
turns out that \C{POOL_ALLOC_N} is good enough, as all our arrays will need
initialization beyond being cleared with zeros, anyway. */
