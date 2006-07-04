/*
 * contents: Unicode character properties.
 *
 * Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>
 */

#include <ruby.h>
#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "unicode.h"
#include "private.h"
#include "unicode-data/unicode-character-tables.h"


/* {{{1
 * Macros for accessing the Unicode character attribute table.
 *
 * TODO: Turn these macros into full-fledged functions, as this is rather silly
 * when we have ‹inline› in C99.
 */
#define ATTR_TABLE(page) \
	(((page) <= UNICODE_LAST_PAGE_PART1) \
	 ? attr_table_part1[page] \
	 : attr_table_part2[(page) - 0xe00])

#define ATTTABLE(page, char) \
	((ATTR_TABLE(page) == UNICODE_MAX_TABLE_INDEX) \
	 ? 0 : (attr_data[ATTR_TABLE(page)][char]))


/* {{{1
 * Internal function used for figuring out the type of a given character.
 */
static inline int
s_type(unichar c)
{
	const int16_t *table;
	unsigned int page;

	if (c <= UNICODE_LAST_CHAR_PART1) {
		page = c >> 8;
		table = type_table_part1;
	} else if (c >= 0xe0000 && c <= UNICODE_LAST_CHAR) {
		page = (c - 0xe0000) >> 8;
		table = type_table_part2;
	} else {
		return UNICODE_UNASSIGNED;
	}

	if (table[page] >= UNICODE_MAX_TABLE_INDEX)
		return table[page] - UNICODE_MAX_TABLE_INDEX;
	else
		return type_data[table[page]][c & 0xff];
}


/* {{{1
 * Internal function used to check if the given type represents a digit type.
 */
static inline bool
s_isdigit(int type)
{
	return (type == UNICODE_DECIMAL_NUMBER ||
		type == UNICODE_LETTER_NUMBER ||
		type == UNICODE_OTHER_NUMBER);
}


/* {{{1
 * Internal function used to check if the given type represents an alphabetic
 * type.
 */
static inline bool
s_isalpha(int type)
{
	return (type == UNICODE_LOWERCASE_LETTER ||
		type == UNICODE_UPPERCASE_LETTER ||
		type == UNICODE_TITLECASE_LETTER ||
		type == UNICODE_MODIFIER_LETTER ||
		type == UNICODE_OTHER_LETTER);
}


/* {{{1
 * Internal function used to check if the given type represents a mark type.
 */
static inline bool
s_ismark(int type)
{
	return (type == UNICODE_NON_SPACING_MARK ||
		type == UNICODE_COMBINING_MARK ||
		type == UNICODE_ENCLOSING_MARK);
}


/* {{{1
 * Determine whether ‘c’ is an alphanumeric, such as A, B, C, 0, 1, or 2.
 */
bool
unichar_isalnum(unichar c)
{
	int type = s_type(c);

	return s_isdigit(type) || s_isalpha(type);
}


/* {{{1
 * Determine whether ‘c’ is an alphabetic (i.e. a letter), such as A, B, or C.
 */
bool
unichar_isalpha(unichar c)
{
	return s_isalpha(s_type(c));
}


/* {{{1
 * Determine whether ‘c’ is a control character, such as ‹NUL›.
 */
bool
unichar_iscntrl(unichar c)
{
	return s_type(c) == UNICODE_CONTROL;
}


/* {{{1
 * Determine whether ‘c’ is a digit, such as 0, 1, or 2.
 */
bool
unichar_isdigit(unichar c)
{
	return s_type(c) == UNICODE_DECIMAL_NUMBER;
}


/* {{{1
 * Determine whether ‘c’ is printable and not a space or control character such
 * as tab or <NUL›, such as A, B, or C.
 */
bool
unichar_isgraph(unichar c)
{
	int type = s_type(c);

	return (type != UNICODE_CONTROL &&
		type != UNICODE_FORMAT &&
		type != UNICODE_UNASSIGNED &&
		type != UNICODE_PRIVATE_USE &&
		type != UNICODE_SURROGATE &&
		type != UNICODE_SPACE_SEPARATOR);
}


/* {{{1
 * Determine whether ‘c’ is a lowercase letter, such as a, b, or c.
 */
bool
unichar_islower(unichar c)
{
	return s_type(c) == UNICODE_LOWERCASE_LETTER;
}


/* {{{1
 * Determine whether ‘c’ is printable, which works the same as
 * unichar_isgraph(), except that space characters are also printable.
 */
bool
unichar_isprint(unichar c)
{
	int type = s_type(c);

	return (type != UNICODE_CONTROL &&
		type != UNICODE_FORMAT &&
		type != UNICODE_UNASSIGNED &&
		type != UNICODE_PRIVATE_USE &&
		type != UNICODE_SURROGATE);
}


/* {{{1
 * Determine whether ‘c’ is some form of punctuation or other symbol.
 */
bool
unichar_ispunct(unichar c)
{
	int type = s_type(c);

	return (type == UNICODE_CONNECT_PUNCTUATION ||
		type == UNICODE_DASH_PUNCTUATION ||
		type == UNICODE_OPEN_PUNCTUATION ||
		type == UNICODE_CLOSE_PUNCTUATION ||
		type == UNICODE_INITIAL_PUNCTUATION ||
		type == UNICODE_FINAL_PUNCTUATION ||
		type == UNICODE_MODIFIER_SYMBOL ||
		type == UNICODE_MATH_SYMBOL ||
		type == UNICODE_CURRENCY_SYMBOL ||
		type == UNICODE_OTHER_SYMBOL ||
		type == UNICODE_OTHER_SYMBOL);
}


/* {{{1
 * Determine whether ‘c’ is some form of whitespace, such as space, tab or a
 * line separator (newline, carriage return, etc.).
 */
bool
unichar_isspace(unichar c)
{
	switch (c) {
	case '\t':
	case '\n':
	case '\r':
	case '\f':
		return true;
	default: {
		int type = s_type(c);

		return (type == UNICODE_SPACE_SEPARATOR ||
			type == UNICODE_LINE_SEPARATOR ||
			type == UNICODE_PARAGRAPH_SEPARATOR);
	}
	}
}


/* {{{1
 * Determine whether ‘c’ is an uppeercase letter, such as A, B, or C
 */
bool
unichar_isupper(unichar c)
{
	return s_type(c) == UNICODE_UPPERCASE_LETTER;
}


/* {{{1
 * Determine whether ‘c’ is a titlecase letter, such as the slavic digraph Ǳ,
 * which at the beginning of a word is written as ǲ, where only the initial D
 * is capitalized.  (Complicated huh?)
 */
bool
unichar_istitle(unichar c)
{
	/* TODO: binary search helpful? */
	for (size_t i = 0; i < lengthof(title_table); i++) {
		if (title_table[i][0] == c)
			return true;
	}

	return false;
}


/* {{{1
 * Determine whether ‘c’ is a hexadecimal digit, such as 0, 1, ..., 9, a, b,
 * ..., f, or A, B, ..., F.
 */
bool
unichar_isxdigit(unichar c)
{
	return ((c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F') ||
		s_isdigit(s_type(c)));
}


/* {{{1
 * Determine whether code point ‘c’ has been assigned a code value.
 */
bool
unichar_isassigned(unichar c)
{
	return s_type(c) != UNICODE_UNASSIGNED;
}


/* {{{1
 * Determine whether ‘c’ is a wide character, thus is typically rendered in a
 * double-width cell on a terminal.
 */
bool
unichar_iswide(unichar c)
{
	if (c < 0x1100) {
		return false;
	} else {
		return (c <= 0x115f || 				/* Hangul Jamo init. consonants */
			c == 0x2329 || c == 0x232a || 		/* angle brackets */
			(c >= 0x2e80 && c <= 0xa4cf && 		/* CJK ... Yi */
			 (c < 0x302a || c > 0x302f) &&
			 c != 0x303f && c != 0x3099 && c != 0x309a) ||
			(c >= 0xac00 && c <= 0xd7a3) || 	/* Hangul syllables */
			(c >= 0xf900 && c <= 0xfaff) || 	/* CJK comp. graphs */
			(c >= 0xfe30 && c <= 0xfe6f) || 	/* CJK comp. forms */
			(c >= 0xff00 && c <= 0xff60) || 	/* fullwidth forms */
			(c >= 0xffe0 && c <= 0xffe6) || 	/*       -"-       */
			(c >= 0x20000 && c <= 0x2fffd) || 	/* CJK extra stuff */
			(c >= 0x30000 && c <= 0x3fffd));    	/*       -"-       */
	}
}


/* {{{1
 * Convert ‘c’ to its uppercase representation (if any).
 */
unichar
unichar_toupper(unichar c)
{
	int type = s_type(c);

	if (type == UNICODE_LOWERCASE_LETTER) {
		unichar tv = ATTTABLE(c >> 8, c & 0xff);

		if (tv >= 0x1000000)
			return utf_char(special_case_table + tv - 0x1000000);
		else
			return (tv != NUL) ? tv : c;
	} else if (type == UNICODE_TITLECASE_LETTER) {
		for (size_t i = 0; i < lengthof(title_table); i++) {
			if (title_table[i][0] == c)
				return title_table[i][1];
		}

		return c;
	} else {
		return c;
	}
}


/* {{{1
 * Convert ‘c’ to its lowercase representation (if any).
 */
unichar
unichar_tolower(unichar c)
{
	int type = s_type(c);

	if (type == UNICODE_UPPERCASE_LETTER) {
		unichar tv = ATTTABLE(c >> 8, c & 0xff);

		if (tv >= 0x1000000)
			return utf_char(special_case_table + tv - 0x1000000);
		else
			return (tv != NUL) ? tv : c;
	} else if (type == UNICODE_TITLECASE_LETTER) {
		for (size_t i = 0; i < lengthof(title_table); i++) {
			if (title_table[i][0] == c)
				return title_table[i][2];
		}

		return c;
	} else {
		return c;
	}
}


/* {{{1
 * Convert ‘c’ to its titlecase representation (if any).
 */
unichar
unichar_totitle(unichar c)
{
	for (size_t i = 0; i < lengthof(title_table); i++) {
		if (title_table[i][0] == c || title_table[i][1] == c || title_table[i][2] == c)
			return title_table[i][0];
	}

	return (s_type(c) == UNICODE_LOWERCASE_LETTER ? ATTTABLE(c >> 8, c & 0xff) : c);
}


/* {{{1
 * Return the numeric value of ‘c’ if it's a decimal digit, or -1 if not.
 */
int
unichar_digit_value(unichar c)
{
	if (s_type(c) == UNICODE_DECIMAL_NUMBER)
		return ATTTABLE(c >> 8, c & 0xff);
	else
		return -1;
}


/* {{{1
 * Return the numeric value of ‘c’ if it's a hexadecimal digit, or -1 if not.
 */
int
unichar_xdigit_value(unichar c)
{
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return unichar_digit_value(c);
}


/* {{{1
 * Determine the Unicode character type of ‘c’.
 */
UnicodeType
unichar_type(unichar c)
{
	return s_type(c);
}


/* {{{1
 * LocaleType: This ‹enum› is used for dealing with different locales for
 * turning strings into uppercase or lowercase.
 */
typedef enum {
	LOCALE_NORMAL,
	LOCALE_TURKIC,
	LOCALE_LITHUANIAN
} LocaleType;


/* {{{1
 * Retrieve the locale type from the environment (LC_CTYPE).
 */
static LocaleType
get_locale_type(void)
{
	const char *locale = setlocale(LC_CTYPE, NULL);

	if ((locale[0] == 'a' && locale[1] == 'z') ||
	    (locale[0] == 't' && locale[1] == 'r'))
		return LOCALE_TURKIC;
	else if (locale[0] == 'l' && locale[1] == 't')
		return LOCALE_LITHUANIAN;
	else
		return LOCALE_NORMAL;
}


/* {{{1
 * Put character marks found in ‘p_inout’ into itself.  If ‘remove_dot’ is
 * true, remove the dot over an uppercase I for a turkish locale.
 */
static size_t
output_marks(const char **p_inout, char *buf, bool remove_dot)
{
	size_t len = 0;
	const char *p = *p_inout;

	for ( ; *p != NUL; p = utf_next(p)) {
		unichar c = utf_char(p);

		if (s_ismark(s_type(c))) {
			if (!remove_dot || c != 0x307)
				len += unichar_to_utf(c, (buf != NUL) ? buf + len : NULL);
		} else {
			break;
		}
	}

	*p_inout = p;

	return len;
}

/* {{{1
 * Output titlecases where appropriate.
 */
static size_t
output_special_case(char *buf, int offset, int type, int which)
{
	const char *p = special_case_table + offset;

	if (type != UNICODE_TITLECASE_LETTER)
		p = utf_next(p);

	if (which == 1)
		p += utf_byte_length(p) + 1;

	size_t len = utf_byte_length(p);

	if (buf != NULL)
		memcpy(buf, p, len);

	return len;
}

/* {{{1
 * Do uppercasing of ‘p’ for Lithuanian locales.
 */
static bool
real_toupper_lithuanian(const char *p, unichar c, int type, size_t *len, char *buf, bool *was_i)
{
	if (c == 'i') {
		*was_i = true;
		return false;
	}

	if (*was_i) {
		/* Nasty, need to remove any dot above.  Though I think only E
		 * WITH DOT ABOVE occurs in practice which could simplify this
		 * considerably. */
		size_t decomp_len;
		unichar *decomp = unicode_canonical_decomposition(c, &decomp_len);

		for (size_t i = 0; i < decomp_len; i++) {
			/* COMBINING DOT ABOVE */
			if (decomp[i] != 0x307)
				*len += unichar_to_utf(unichar_toupper(decomp[i]), (buf != NULL) ? buf + *len : NULL);
		}

		free(decomp);

		*len += output_marks(&p, (buf != NULL) ? buf  + *len : NULL, true);
		return true;
	}

	if (!s_ismark(type))
		*was_i = false;

	return false;
}

/* {{{1
 * Do real upcasing. */
static inline void
real_do_toupper(unichar c, int type, int *len, char *buf)
{
	int which = (type == UNICODE_LOWERCASE_LETTER) ? 0 : 1;
	unichar tv = ATTTABLE(c >> 8, c & 0xff);

	if (tv >= 0x1000000) {
		*len += output_special_case((buf != NULL) ? buf + *len : NULL, tv - 0x1000000, type, which);
	} else {
		if (type == UNICODE_TITLECASE_LETTER) {
			for (size_t i = 0; i < lengthof(title_table); i++) {
				if (title_table[i][0] == c)
					tv = title_table[i][1];
			}
		}

		*len += unichar_to_utf(tv, (buf != NULL) ? buf + *len : NULL);
	}
}

/* {{{1
 * Do real uppercasing of ‘str’.
 */
static size_t
real_toupper(const char *str,
	     size_t max,
	     bool use_max,
	     char *buf,
	     LocaleType locale_type)
{
	const char *p = str;
	const char *prev;
	size_t len = 0;
	bool p_was_i = false;

	while ((!use_max || p < str + max) && *p != NUL) {
		unichar c = utf_char(p);
		int type = s_type(c);

		prev = p;
		p = utf_next(p);

		if (locale_type == LOCALE_LITHUANIAN &&
		    real_toupper_lithuanian(p, c, type, &len, buf, &p_was_i))
			continue;

		if (locale_type == LOCALE_TURKIC && c == 'i') {
			/* i => LATIN CAPITAL LETTER I WITH DOT ABOVE */
			len += unichar_to_utf(0x130, (buf != NULL) ? buf + len : NULL);
		} else if (c == 0x0345) { /* COMBINING GREEK YPOGEGRAMMENI */
			/* Nasty, need to move it after other combining
			 * marks...this would go away if we normalized first.
			 */
			len += output_marks(&p, (buf != NULL) ? buf + len : NULL, false);
			/* and output as GREEK CAPITAL LETTER IOTA */
			len += unichar_to_utf(0x399, (buf != NULL) ? buf + len : NULL);
		} else if (type == UNICODE_LOWERCASE_LETTER || type == UNICODE_TITLECASE_LETTER) {
			real_do_toupper(c, type, &len, buf);
		} else {
			size_t c_len = s_utf_skip_lengths[*(const unsigned char *)prev];

			if (buf != NULL)
				memcpy(buf + len, prev, c_len);

			len += c_len;
		}
	}

	return len;
}

/* {{{1
 * Wrapper around real_toupper() for dealing with memory allocation and such.
 */
static char *
utf_upcase_impl(const char *str, size_t max, bool use_max)
{
	assert(str != NULL);

	LocaleType locale_type = get_locale_type();

	size_t len = real_toupper(str, max, use_max, NULL, locale_type);
	char *result = ALLOC_N(char, len + 1);
	real_toupper(str, max, use_max, result, locale_type);
	result[len] = NUL;

	return result;
}


/* {{{1
 * Convert all characters in ‘str’ to their uppercase representation if
 * applicable.  Returns the freshly allocated representation.
 */
char *
utf_upcase(const char *str)
{
	return utf_upcase_impl(str, 0, false);
}


/* {{{1
 * Convert all characters in ‘str’ to their uppercase representation if
 * applicable.  Returns the freshly allocated representation.  Do this for at
 * most ‘len˚ bytes from ‘str’.
 */
char *
utf_upcase_n(const char *str, size_t len)
{
	return utf_upcase_impl(str, len, true);
}


/* {{{1
 * Traverse the string checking for characters with combining class == 230
 * until a base character is found.
 */ 
static bool
has_more_above(const char *str)
{
	for (const char *p = str; *p != NUL; p = utf_next(p)) {
		int c_class = _unichar_combining_class(utf_char(p));

		if (c_class == 230)
			return true;
		else if (c_class == 0)
			return false;
	}

	return false;
}

/* {{{1
 * The real implementation of downcase.
 *
 * TODO: this needs a cleanup.
 */
static size_t
real_tolower(const char *str, size_t max, bool use_max, char *buf, LocaleType locale_type)
{
	const char *p = str;
	const char *prev;
	size_t len = 0;

	while ((!use_max || p < str + max) && *p != NUL) {
		unichar c = utf_char(p);
		int type = s_type(c);

		prev = p;
		p = utf_next(p);

		if (locale_type == LOCALE_TURKIC && c == 'I') {
			if (utf_char(p) == 0x0307) {
				/* I + COMBINING DOT ABOVE => i (U+0069) */
				len += unichar_to_utf(0x0069, (buf != NULL) ? buf + len : NULL);
				p = utf_next(p);
			} else {
				/* I => LATIN SMALL LETTER DOTLESS I */
				len += unichar_to_utf(0x131, (buf != NULL) ? buf + len : NULL);
			}
		} else if (locale_type == LOCALE_LITHUANIAN &&
			   (c == 0x00cc || c == 0x00cd || c == 0x0128)) {
			/* Introduce an explicit dot above the lowercasing
			 * capital I's and J's whenever there are more accents
			 * above. [SpecialCasing.txt]
			 */
			len += unichar_to_utf(0x0069, (buf != NULL) ? buf + len : NULL);
			len += unichar_to_utf(0x0307, (buf != NULL) ? buf + len : NULL);
			switch (c) {
			case 0x00cc:
				len += unichar_to_utf(0x0300, (buf != NULL) ? buf + len : NULL);
				break;
			case 0x00cd:
				len += unichar_to_utf(0x0301, (buf != NULL) ? buf + len : NULL);
				break;
			case 0x0128:
				len += unichar_to_utf(0x0303, (buf != NULL) ? buf + len : NULL);
				break;
			}
		} else if (locale_type == LOCALE_LITHUANIAN &&
			   (c == 'I' || c == 'J' || c == 0x012e) &&
			   has_more_above(p)) {
			len += unichar_to_utf(unichar_tolower(c), (buf != NULL) ? buf + len : NULL);
			len += unichar_to_utf(0x0307, (buf != NULL) ? buf + len : NULL);
		} else if (c == 0x03a3) { /* GREEK CAPITAL LETTER SIGMA */
			unichar tv;

			if ((!use_max || p < str + max) && *p != NUL) {
				unichar next_c = utf_char(p);
				int next_type = s_type(next_c);

				/* SIGMA maps differently depending on whether
				 * it is final or not.  The following
				 * simplified test would fail in the case of
				 * combining marks following the sigma, but I
				 * don't think that occurs in real text.  The
				 * test here matches that in ICU.
				 */
				if (s_isalpha(next_type))
					tv = 0x3c3; /* GREEK SMALL SIGMA */
				else
					tv = 0x3c2; /*GREEK SMALL FINAL SIGMA*/
			} else {
				tv = 0x3c2; /* GREEK SMALL FINAL SIGMA */
			}

			len += unichar_to_utf(tv, (buf != NULL) ? buf + len : NULL);
		} else if (type == UNICODE_UPPERCASE_LETTER ||
			   type == UNICODE_TITLECASE_LETTER) {
			unichar tv = ATTTABLE(c >> 8, c & 0xff);

			if (tv >= 0x1000000) {
				len += output_special_case((buf != NULL) ? buf + len : NULL, tv - 0x1000000, type, 0);
			} else {
				if (type == UNICODE_TITLECASE_LETTER) {
					for (size_t i = 0; i < lengthof(title_table); i++) {
						if (title_table[i][0] == c)
							tv = title_table[i][2];
					}
				}

				len += unichar_to_utf(tv, (buf != NULL) ? buf + len : NULL);
			}
		} else {
			size_t c_len = s_utf_skip_lengths[*(const unsigned char *)prev];

			if (buf != NULL)
				memcpy(buf + len, prev, c_len);

			len += c_len;
		}
	}

	return len;
}


/* {{{1 */
static char *
utf_downcase_impl(const char *str, size_t max, bool use_max)
{
	assert(str != NULL);

	LocaleType locale_type = get_locale_type();

	size_t len = real_tolower(str, max, use_max, NULL, locale_type);
	char *result = ALLOC_N(char, len + 1);
	real_tolower(str, max, use_max, result, locale_type);
	result[len] = NUL;

	return result;
}


/* {{{1
 * Convert all characters in ‘str’ to their lowercase representation if
 * applicable.  Returns the freshly allocated representation.
 */
char *
utf_downcase(const char *str)
{
	return utf_downcase_impl(str, 0, false);
}


/* {{{1
 * Convert all characters in ‘str’ to their lowercase representation if
 * applicable.  Returns the freshly allocated representation.  Do this for at
 * most ‘len˚ bytes from ‘str’.
 */
char *
utf_downcase_n(const char *str, size_t len)
{
	return utf_downcase_impl(str, len, true);
}


/* {{{1
 * The real implementation of case folding below.
 */
static char *
utf_foldcase_impl(const char *str, size_t len, bool use_len)
{
	assert(str != NULL);

	char *result = NULL;
	int size = 0;

again:
	for (const char *p = str; (!use_len || p < str + len) && *p != NUL; p = utf_next(p)) {
		unichar c = utf_char(p);
		int begin = 0;
		int end = lengthof(casefold_table);

		if (c >= casefold_table[begin].ch &&
		    c <= casefold_table[end - 1].ch) {
			while (true) {
				int mid = (begin + end) / 2;

				if (c == casefold_table[mid].ch) {
					if (result != NULL)
						strcpy(result + size, casefold_table[mid].data);
					size += strlen(casefold_table[mid].data);
					goto next;
				} else if (mid == begin) {
					break;
				} else if (c > casefold_table[mid].ch) {
					begin = mid;
				} else {
					end = mid;
				}
			}
		}

		size += unichar_to_utf(unichar_tolower(c), (result != NULL) ? result + size : NULL);
next:
		continue;
	}

	if (result == NULL) {
		result = ALLOC_N(char, size + 1);
		result[0] = NUL;
		size = 0;
		goto again;
	}

	result[size] = NUL;

	return result;
}


/* {{{1
 * Convert a string into a form that is independent of case.  Return the
 * freshly allocated representation.
 */
char *
utf_foldcase(const char *str)
{
	return utf_foldcase_impl(str, 0, false);
}


/* {{{1
 * Convert a string into a form that is independent of case.  Return the
 * freshly allocated representation.  Do this for at most ‘len’ bytes from the
 * string.
 */
char *
utf_foldcase_n(const char *str, size_t len)
{
	return utf_foldcase_impl(str, len, true);
}


/* {{{1
 * The real implementation of utf_width() and utf_width_n() below.
 */
static size_t
utf_width_impl(const char *str, size_t len, bool use_len)
{
	assert(str != NULL);

	size_t width = 0;

	for (const char *p = str; (!use_len || p < str + len) && *p != NUL; p = utf_next(p))
		width += unichar_iswide(utf_char(p)) ? 2 : 1;

	return width;
}


/* {{{1
 * Calculate the width in cells of ‘str’.
 */
size_t
utf_width(const char *str)
{
	return utf_width_impl(str, 0, false);
}


/* {{{1
 * Calculate the width in cells of ‘str’, which is of length ‘len’.
 */
size_t
utf_width_n(const char *str, size_t len)
{
	return utf_width_impl(str, len, true);
}


/* {{{1
 * Retrieve the mirrored representation of ‘c’ (if any) and store it in
 * ‘mirrored’.
 */
bool
unichar_mirror(unichar c, unichar *mirrored)
{
	int begin = 0;
	int end = lengthof(bidi_mirroring_table);

	while (true) {
		int mid = (begin + end) / 2;

		if (c == bidi_mirroring_table[mid].ch) {
			if (mirrored != NULL)
				*mirrored = bidi_mirroring_table[mid].mirrored_ch;
			return true;
		} else if (mid == begin) {
			return false;
		} else if (c > bidi_mirroring_table[mid].ch) {
			begin = mid;
		} else {
			end = mid;
		}
	}
}


/* }}}1 */
