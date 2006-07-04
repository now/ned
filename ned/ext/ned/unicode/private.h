/*
 * contents: Private Unicode related information.
 *
 * Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>
 */

#ifndef PRIVATE_H
#define PRIVATE_H


#define NUL '\0'
#define lengthof(ary)   (sizeof(ary) / sizeof((ary)[0]))


unichar *_utf_normalize_wc(const char *str, size_t max_len, bool use_len, NormalizeMode mode);
inline int _unichar_combining_class(unichar c);


#endif /* PRIVATE_H */
