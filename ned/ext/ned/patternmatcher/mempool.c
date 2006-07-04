/*
 * contents: Memory pools allow for memory allocation saving (space/time) and deallocate contents en masse.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

#include <ruby.h>
#include <assert.h>
#include <stdint.h>

#include "private.h"
#include "mempool.h"

/*¶ Enough with covering the interface.  It's time to actually implement the
memory||pool data type.  We will represent it as a linked list of memory blocks
that will then be divided to fill allocation requests.  To accommodate any size
of allocation, we won’t use fixed||size memory blocks, but as most request will
be for small blocks, we try to allocate enough memory to fill quite a few such
requests.  Thus, we will have a minimum size for memory blocks that will be
allocated and then shelled out to fill these requests: */

#define MEM_POOL_BLOCK_SIZE     4096


/*¶ The following macro rounds off sizes to the next memory-boundary, which is
good, as allocated memory wouldn't be very useful otherwise.  As Ruby requires
that \C{SIZEOF_VOIDP} be equal to \C{SIZEOF_LONG}, we aren’t causing ourselves
any trouble with the following definition: */

#define POINTER_ALIGN(size) \
        (((size) % SIZEOF_VOIDP) ? (SIZEOF_VOIDP - ((size) % SIZEOF_VOIDP)) : 0)


/*¶ Allocated memory blocks are represented by the following data structure: */
typedef struct _MemBlock MemBlock;

struct _MemBlock {
        MemBlock *next;
        uint8_t mem[];
};

/*¶ We thus maintain a linked||list (\C{next}) of memory blocks (\C{mem}).
Note how we use the notorious “\C{struct} hack” that is actually part of the
’99 revision of the \CLang\ standard.  */


/*¶ Before we can display the only function that is defined on memory blocks,
we need to introduce the memory pool.  It's simply a set of already allocated
blocks, a byte||sized pointer to the top||most block, i.e., the one that we are
currently allocating from, and the remaining number of bytes in that block: */

struct _MemPool {
        MemBlock *blocks;
        uint8_t *base;
        size_t free;
};


/*¶ Now then, here's the only function that operates on memory blocks: */

static void
mem_block_new(MemPool *pool, size_t size)
{
        MemBlock *block;

        pool->free = MAX(size * 8, MEM_POOL_BLOCK_SIZE);
        block = (MemBlock *)malloc(sizeof(MemBlock) +
                                   sizeof(uint8_t) * pool->free);
        pool->base = block->mem;
        block->next = pool->blocks;
        pool->blocks = block;
}

/*¶ We thus more or less only need to figure out an appropriate number of bytes
to allocate, allocate them, and link in this new block into the pool with which
it is to be associated. */


/*¶ There's really nothing interesting going on in the creation of a memory
pool: */

HIDDEN MemPool *
mem_pool_new(void)
{
        return CALLOC(MemPool);
}


/*¶ Once all the memory associated with a memory pool is of no more use, we
will need to free it: */

HIDDEN void
mem_pool_free(MemPool *pool)
{
        assert(pool != NULL);

        for (MemBlock *p = pool->blocks, *t; p != NULL; p = t) {
                t = p->next;
                free(p);
        }

        free(pool);
}

/*¶ We have to make sure to release all the memory pointed to by the memory
blocks associated with the memory pool and then finally the memory pool itself.
*/


/*¶ To allocate memory from a memory pool, we begin by checking whether the
requested number of bytes fits within the number of free bytes remaining in the
current memory block.  If so, we simply go on to use those bytes.  If not, we
create a new block and begin allocating memory from that block instead. */

HIDDEN void *
mem_pool_alloc(MemPool *pool, size_t size)
{
        assert(pool != NULL);

        if (pool->free < size)
                mem_block_new(pool, size);

        size += POINTER_ALIGN(size);
        pool->base += size;
        pool->free -= size;

        return pool->base - size;
}

/*¶ We must make sure that the next allocation from the pool will be on an
appropriate memory boundary, so we align \C{size} before we use it in the
operations. */
