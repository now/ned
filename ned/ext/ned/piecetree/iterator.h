/*
 * contents: PieceTree::Iterator class.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */



/*¶ \subsection{Piece||tree iterators: an easy way to navigate a piece||tree.}

More or less all operations on a piece tree are performed through an iterator.
An iterator marks a location within the piece tree|<|actually, it points to a
node in said tree|>|and can be moved backwards and forwards in the tree.
Performing modifying operations on the tree through an iterator saves us some
work, as any localized modifications can all be performed using the same
iterator.  Thus, $n$ operations can be performed in $\Ordo{\lg n} + n\Ordo{1}$
time instead of $n\Ordo{\lg n}$ time. */

/*¶ We, again, begin with the external interface to the iterator object: */

typedef struct _Iterator Iterator;

extern VALUE g_cIterator;

#define ITERATOR2VALUE(iter)            \
        Data_Wrap_Struct(g_cIterator, iterator_mark, iterator_free, (iter))

#define VALUE2ITERATOR(value, iter)     \
        Data_Get_Struct((value), Iterator, (iter))


/*¶ ————————————————————————————————— EOD —————————————————————————————————— */


void Init_Iterator(void);

Iterator *iterator_new(VALUE tree, Node *node);
void iterator_mark(Iterator *iter);
void iterator_free(Iterator *iter);
