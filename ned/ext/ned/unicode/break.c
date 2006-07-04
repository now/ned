/*
 * contents: Unicode line-breaking properties.
 *
 * Copyright (C) 2004 Nikolai Weibull <source@pcppopper.org>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "unicode.h"
#include "stdint.h"
#include "unicode-data/unicode-break.h"


/* Figure out what break type the Unicode character ‘c’ possesses, if any.
 * This information is used for finding word and line boundaries, which is
 * useful when displaying Unicode text on screen. */
UnicodeBreakType
unichar_break_type(unichar c)
{
	if (c <= UNICODE_LAST_CHAR_PART1) {
		unsigned int page = c >> 8;

		if (break_property_table_part1[page] >= UNICODE_MAX_TABLE_INDEX)
			return break_property_table_part1[page] - UNICODE_MAX_TABLE_INDEX;
		else
			return break_property_data[break_property_table_part1[page]][c & 0xff];
	} else {
		if (c >= 0xe0000 && c <= UNICODE_LAST_CHAR) {
			unsigned int page = (c - 0xe0000) >> 8;

			if  (break_property_table_part2[page] >= UNICODE_MAX_TABLE_INDEX) 
				return break_property_table_part2[page] - UNICODE_MAX_TABLE_INDEX;
			else
				return break_property_data[break_property_table_part2[page]][c & 0xff];
		} else {
			return UNICODE_BREAK_UNKNOWN;
		}
	}
}
