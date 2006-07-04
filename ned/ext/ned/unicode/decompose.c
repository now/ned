/*
 * contents: Unicode decomposition handling.
 *
 * Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>
 */

#include <ruby.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "unicode.h"
#include "private.h"
#include "unicode-data/unicode-decompose.h"
#include "unicode-data/unicode-compose.h"


/* {{{1
 * Macros for accessing the combining class property tables for a given
 * character.
 *
 * TODO: Turn these macros into full-fledged functions, as this is rather silly
 * when we have ‹inline› in C99.
 */
#define CC_PART1(page, char) \
	((combining_class_table_part1[page] >= UNICODE_MAX_TABLE_INDEX) \
	 ? (combining_class_table_part1[page] - UNICODE_MAX_TABLE_INDEX) \
	 : (cclass_data[combining_class_table_part1[page]][char]))

#define CC_PART2(page, char) \
	((combining_class_table_part2[page] >= UNICODE_MAX_TABLE_INDEX) \
	 ? (combining_class_table_part2[page] - UNICODE_MAX_TABLE_INDEX) \
	 : (cclass_data[combining_class_table_part2[page]][char]))

#define COMBINING_CLASS(char) \
	(((char) <= UNICODE_LAST_CHAR_PART1) \
	 ? CC_PART1((char) >> 8, (char) & 0xff) \
	 : (((char) >= 0xe0000 && (char) <= UNICODE_LAST_CHAR) \
	    ? CC_PART2(((char) - 0xe0000) >> 8, (char) & 0xff) \
	    : 0))


/* {{{1
 * Hangul syllable [de]composition constants. A lot of work I'd say.
 */
enum {
	SBase = 0xac00,
	LBase = 0x1100,
	VBase = 0x1161,
	TBase = 0x11a7,
	LCount = 19,
	VCount = 21,
	TCount = 28,
	NCount = (VCount * TCount),
	SCount = (LCount * NCount)
};


/* {{{1
 * Return the combinging class of ‘c’.
 */
inline int
_unichar_combining_class(unichar c)
{
	return COMBINING_CLASS(c);
}


/* {{{1
 * Rearrange ‘str’ so that decomposed characters are arranged according to
 * their combining class.  Do this for at most ‘len’ bytes of data.
 */
void
unicode_canonical_ordering(unichar *str, size_t len)
{
	bool swapped = true;

	while (swapped) {
		swapped = false;

		int prev = COMBINING_CLASS(str[0]);

		for (size_t i = 0; i < len - 1; i++) {
			int next = COMBINING_CLASS(str[i + 1]);

			if (next != 0 && prev > next) {
				for (size_t j = i + 1; j > 0 && COMBINING_CLASS(str[j - 1]) <= next; j--) {
					unichar c = str[j];
					str[j] = str[j - 1];
					str[j - 1] = c;
					swapped = true;
				}

				next = prev;
			}

			prev = next;
		}
	}
}


/* {{{1
 * Decompose the character ‘s’ according to the rules outlined in
 * http://www.unicode.org/unicode/reports/tr15/#Hangul.  ‘r’ should be ‹NULL›
 * or of sufficient length to store the decomposition of ‘s’.  The number of
 * characters stored (or would be if it were non-‹NULL›) in ‘r’ is put in
 * ‘r_len’.
 */
static void
decompose_hangul(unichar s, unichar *r, size_t *r_len)
{
	int SIndex = s - SBase;

	/* not a hangul syllable */
	if (SIndex < 0 || SIndex >= SCount) {
		if (r != NULL)
			r[0] = s;
		*r_len = 1;
	} else {
		unichar L = LBase + SIndex / NCount;
		unichar V = VBase + (SIndex % NCount) / TCount;
		unichar T = TBase + SIndex % TCount;

		if (r != NULL) {
			r[0] = L;
			r[1] = V;
		}

		if (T != TBase) {
			if (r != NULL)
				r[2] = T;
			*r_len = 3;
		} else {
			*r_len = 2;
		}
	}
}


/* {{{1
 * Search the Unicode decomposition table for ‘c’ and depending on the boolean
 * value of ‘compat’, return its compatibility or canonical decomposition if
 * found.
 */
static const char *
find_decomposition(unichar c, bool compat)
{
	int begin = 0;
	int end = lengthof(decomp_table);

	if (c < decomp_table[begin].ch || c > decomp_table[end - 1].ch)
		return NULL;

	int mid = (begin + end) / 2;
	bool found = false;
	while (true) {
		if (c == decomp_table[mid].ch) {
			found = true;
			break;
		} else if (mid == begin) {
			break;
		} else if (c > decomp_table[mid].ch) {
			begin = mid;
		} else {
			end = mid;
		}
	}

	if (found) {
		int offset;

		if (compat) {
			offset = decomp_table[mid].compat_offset;
			if (offset == UNICODE_NOT_PRESENT_OFFSET)
				offset = decomp_table[mid].canon_offset;
		} else {
			offset = decomp_table[mid].canon_offset;
			if (offset == UNICODE_NOT_PRESENT_OFFSET)
				return NULL;
		}

		return &decomp_expansion_string[offset];
	}

	return NULL;
}


/* {{{1
 * Generate the canonical decomposition of ‘c’.  The length of the
 * decomposition is stored in ‘r_len’.
 */
unichar *
unicode_canonical_decomposition(unichar c, size_t *len)
{
	const char *decomp;
	unichar *r;

	/* Hangul syllable */
	if (c >= 0xac00 && c <= 0xd7af) {
		decompose_hangul(c, NULL, len);
		r = ALLOC_N(unichar, *len);
		decompose_hangul(c, r, len);
	} else if ((decomp = find_decomposition(c, false)) != NULL) {
		*len = utf_length(decomp);
		r = ALLOC_N(unichar, *len);

		int i;
		const char *p;
		for (p = decomp, i = 0; *p != NUL; p = utf_next(p), i++)
			r[i] = utf_char(p);
	} else {
		r = ALLOC(unichar);
		*r = c;
		*len = 1;
	}

	/* Supposedly following the Unicode 2.1.9 table means that the
	 * decompositions come out in canonical order.  I haven't tested this,
	 * but we rely on it here. */
	return r;
}


/* {{{1
 * Combine Hangul characters ‘a’ and ‘b’ if possible, and store the result in
 * ‘result’.  The combinations tried are L,V => LV and LV,T => LVT in that
 * order.
 */
static bool
combine_hangul(unichar a, unichar b, unichar *result)
{
	int LIndex = a - LBase;
	int SIndex = a - SBase;
	int VIndex = b - VBase;
	int TIndex = b - TBase;

	if (0 <= LIndex && LIndex < LCount && 0 <= VIndex && VIndex < VCount) {
		*result = SBase + (LIndex * VCount + VIndex) * TCount;
		return true;
	} else if (0 <= SIndex && SIndex < SCount && (SIndex % TCount) == 0 && 0 <= TIndex && TIndex <= TCount) {
		*result = a + TIndex;
		return true;
	} else {
		return false;
	}
}


/* {{{1
 * Macros used by combine() below for looking up information in the
 * Unicode composition table.
 */
#define CI(page, char) \
	((compose_table[page] >= UNICODE_MAX_TABLE_INDEX) \
	 ? (compose_table[page] - UNICODE_MAX_TABLE_INDEX) \
	 : (compose_data[compose_table[page]][char]))

#define COMPOSE_INDEX(char) \
	(((char >> 8) > (COMPOSE_TABLE_LAST)) \
	 ? 0 \
	 : CI((char) >> 8, (char) & 0xff))


/* {{{1
 * Try to combine the Unicode characters ‘a’ and ‘b’ storing the result in
 * ‘result’.
 */
static bool
combine(unichar a, unichar b, unichar *result)
{
	if (combine_hangul(a, b, result))
		return true;

	unsigned short index_a = COMPOSE_INDEX(a);
	if (index_a >= COMPOSE_FIRST_SINGLE_START && index_a < COMPOSE_SECOND_START) {
		if (b == compose_first_single[index_a - COMPOSE_FIRST_SINGLE_START][0]) {
			*result = compose_first_single[index_a - COMPOSE_FIRST_SINGLE_START][1];
			return true;
		} else {
			return false;
		}
	}

	unsigned short index_b = COMPOSE_INDEX(b);
	if (index_b >= COMPOSE_SECOND_SINGLE_START) {
		if (a == compose_second_single[index_b - COMPOSE_SECOND_SINGLE_START][0]) {
			*result = compose_second_single[index_b - COMPOSE_SECOND_SINGLE_START][1];
			return true;
		} else {
			return false;
		}
	}

	if (index_a >= COMPOSE_FIRST_START &&
	    index_a < COMPOSE_FIRST_SINGLE_START &&
	    index_b >= COMPOSE_SECOND_START &&
	    index_b < COMPOSE_SECOND_SINGLE_START) {
		unichar r = compose_array[index_a - COMPOSE_FIRST_START][index_b - COMPOSE_SECOND_START];

		if (r != 0) {
			*result = r;
			return true;
		} else {
			return false;
		}
	}

	return false;
}


/* {{{1
 * Normalize (compose/decompose) characters in ‘str˚ so that strings that
 * actually contain the same characters will be recognized as equal for
 * comparison for example.
 */
unichar *
_utf_normalize_wc(const char *str, size_t max_len, bool use_len, NormalizeMode mode)
{
	bool do_compat = (mode == NORMALIZE_NFKC || mode == NORMALIZE_NFKD);
	bool do_compose = (mode = NORMALIZE_NFC || mode == NORMALIZE_NFKC);

	size_t n = 0;
	const char *p = str;
	while ((!use_len || p < str + max_len) && *p != NUL) {
		unichar c = utf_char(p);

		if (c >= 0xac00 && c <= 0xd7af) {
			size_t len;

			decompose_hangul(c, NULL, &len);
			n += len;
		} else {
			const char *decomp = find_decomposition(c, do_compat);

			n += (decomp != NULL) ? utf_length(decomp) : 1;
		}

		p = utf_next(p);
	}

	unichar *buf = ALLOC_N(unichar, n + 1);
	size_t prev_start;
	for (p = str, prev_start = 0, n = 0; (!use_len || p < str + max_len) && *p != NUL; p = utf_next(p)) {
		unichar c = utf_char(p);
		size_t prev_n = n;

		if (c >= 0xac00 && c <= 0xd7af) {
			size_t len;

			decompose_hangul(c, buf + n, &len);
			n += len;
		} else {
			const char *decomp = find_decomposition(c, do_compat);

			if (decomp != NULL) {
				for ( ; *decomp != NUL; decomp = utf_next(decomp))
					buf[n++] = utf_char(decomp);
			} else {
				buf[n++] = c;
			}
		}

		if (n > 0 && COMBINING_CLASS(buf[prev_n]) == 0) {
			unicode_canonical_ordering(buf + prev_start, n - prev_start);
			prev_start = prev_n;
		}
	}

	if (n > 0) {
		unicode_canonical_ordering(buf + prev_start, n - prev_start);
		prev_start = n;
	}

	buf[n] = NUL;

	/* done with decomposition and reordering */

	if (do_compose && n > 0) {
		prev_start = 0;
		int prev_cc = 0;
		for (size_t i = 0; i < n; i++) {
			int cc = COMBINING_CLASS(buf[i]);

			if (i > 0 && (prev_cc == 0 || prev_cc != cc) && combine(buf[prev_start], buf[i], &buf[prev_start])) {
				for (size_t j = i + 1; j < n; j++)
					buf[j - 1] = buf[j];

				n--;
				i--;
				prev_cc = (i == prev_start) ? 0 : COMBINING_CLASS(buf[i - 1]);
			} else {
				if (cc == 0)
					prev_start = i;

				prev_cc = cc;
			}
		}

		buf[n] = NUL;
	}

	return buf;
}


/* {{{1
 * Normalize (compose/decompose) characters in ‘str˚ so that strings that
 * actually contain the same characters will be recognized as equal for
 * comparison for example.
 */
char *
utf_normalize(const char *str, NormalizeMode mode)
{
	unichar *wcs = _utf_normalize_wc(str, 0, false, mode);
	char *utf = ucs4_to_utf8(wcs, NULL, NULL);

	free(wcs);
	return utf;
}


/* {{{1
 * This function is the same as utf_normalize() except that at most ‘len˚
 * bytes are normalized from ‘str’.
 */
char *
utf_normalize_n(const char *str, NormalizeMode mode, size_t len)
{
	unichar *wcs = _utf_normalize_wc(str, len, true, mode);
	char *utf = ucs4_to_utf8(wcs, NULL, NULL);

	free(wcs);
	return utf;
}


/* }}}1 */
