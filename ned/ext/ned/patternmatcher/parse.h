/*
 * contents: Regular expression parser.
 *
 * Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>
 */

ASTNode *pattern_parse(MemPool *pool, VALUE rbpattern, VALUE rules, int *n_nodes, int *n_submatches);
