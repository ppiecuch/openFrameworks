/*
 * This file is part of LibParserUtils.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <parserutils/parserutils.h>

#include "utils_p.h"
#include "charset_p.h"


#ifndef parserutils_charset_encodings_utf8impl_h_
#define parserutils_charset_encodings_utf8impl_h_

/** \file
 * UTF-8 manipulation macros (implementation).
 */

/** Number of continuation bytes for a given start byte */
extern const uint8_t numContinuations[256];

/**
 * Convert a UTF-8 multibyte sequence into a single UCS-4 character
 *
 * Encoding of UCS values outside the UTF-16 plane has been removed from
 * RFC3629. This macro conforms to RFC2279, however.
 *
 * \param s      The sequence to process
 * \param len    Length of sequence
 * \param ucs4   Pointer to location to receive UCS-4 character (host endian)
 * \param clen   Pointer to location to receive byte length of UTF-8 sequence
 * \param error  Location to receive error code
 */
#define UTF8_TO_UCS4(s, len, ucs4, clen, error)				\
do {									\
	uint32_t c, min;						\
	uint8_t n;							\
	uint8_t i;							\
									\
	error = PARSERUTILS_OK;						\
									\
	if (s == NULL || ucs4 == NULL || clen == NULL) {		\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	if (len == 0) {							\
		error = PARSERUTILS_NEEDDATA;				\
		break;							\
	}								\
									\
	c = s[0];							\
									\
	if (c < 0x80) {							\
		n = 1;							\
		min = 0;						\
	} else if ((c & 0xE0) == 0xC0) {				\
		c &= 0x1F;						\
		n = 2;							\
		min = 0x80;						\
	} else if ((c & 0xF0) == 0xE0) {				\
		c &= 0x0F;						\
		n = 3;							\
		min = 0x800;						\
	} else if ((c & 0xF8) == 0xF0) {				\
		c &= 0x07;						\
		n = 4;							\
		min = 0x10000;						\
	} else if ((c & 0xFC) == 0xF8) {				\
		c &= 0x03;						\
		n = 5;							\
		min = 0x200000;						\
	} else if ((c & 0xFE) == 0xFC) {				\
		c &= 0x01;						\
		n = 6;							\
		min = 0x4000000;					\
	} else {							\
		error = PARSERUTILS_INVALID;				\
		break;							\
	}								\
									\
	if (len < n) {							\
		error = PARSERUTILS_NEEDDATA;				\
		break;							\
	}								\
									\
	for (i = 1; i < n; i++) {					\
		uint32_t t = s[i];					\
									\
		if ((t & 0xC0) != 0x80) {				\
			error = PARSERUTILS_INVALID;			\
			break;						\
		}							\
									\
		c <<= 6;						\
		c |= t & 0x3F;						\
	}								\
									\
	if (error == PARSERUTILS_OK) {					\
		/* Detect overlong sequences, surrogates and fffe/ffff */ \
		if (c < min || (c >= 0xD800 && c <= 0xDFFF) ||		\
				c == 0xFFFE || c == 0xFFFF) {		\
			error = PARSERUTILS_INVALID;			\
			break;						\
		}							\
									\
		*ucs4 = c;						\
		*clen = n;						\
	}								\
} while(0)

/**
 * Convert a single UCS-4 character into a UTF-8 multibyte sequence
 *
 * Encoding of UCS values outside the UTF-16 plane has been removed from
 * RFC3629. This macro conforms to RFC2279, however.
 *
 * \param ucs4   The character to process (0 <= c <= 0x7FFFFFFF) (host endian)
 * \param s      Pointer to pointer to output buffer, updated on exit
 * \param len    Pointer to length, in bytes, of output buffer, updated on exit
 * \param error  Location to receive error code
 */
#define UTF8_FROM_UCS4(ucs4, s, len, error)				\
do {									\
	uint8_t *buf;							\
	uint8_t l = 0;							\
									\
	error = PARSERUTILS_OK;						\
									\
	if (s == NULL || *s == NULL || len == NULL) {			\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	if (ucs4 < 0x80) {						\
		l = 1;							\
	} else if (ucs4 < 0x800) {					\
		l = 2;							\
	} else if (ucs4 < 0x10000) {					\
		l = 3;							\
	} else if (ucs4 < 0x200000) {					\
		l = 4;							\
	} else if (ucs4 < 0x4000000) {					\
		l = 5;							\
	} else if (ucs4 <= 0x7FFFFFFF) {				\
		l = 6;							\
	} else {							\
		error = PARSERUTILS_INVALID;				\
		break;							\
	}								\
									\
	if (l > *len) {							\
		error = PARSERUTILS_NOMEM;				\
		break;							\
	}								\
									\
	buf = *s;							\
									\
	if (l == 1) {							\
		buf[0] = (uint8_t) ucs4;				\
	} else {							\
		uint8_t i;						\
		for (i = l; i > 1; i--) {				\
			buf[i - 1] = 0x80 | (ucs4 & 0x3F);		\
			ucs4 >>= 6;					\
		}							\
		buf[0] = ~((1 << (8 - l)) - 1) | ucs4;			\
	}								\
									\
	*s += l;							\
	*len -= l;							\
} while(0)

/**
 * Calculate the length (in characters) of a bounded UTF-8 string
 *
 * \param s      The string
 * \param max    Maximum length
 * \param len    Pointer to location to receive length of string
 * \param error  Location to receive error code
 */
#define UTF8_LENGTH(s, max, len, error)					\
do {									\
	const uint8_t *end = s + max;					\
	int l = 0;							\
									\
	error = PARSERUTILS_OK;						\
									\
	if (s == NULL || len == NULL) {					\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	while (s < end) {						\
		uint32_t c = s[0];					\
									\
		if ((c & 0x80) == 0x00)					\
			s += 1;						\
		else if ((c & 0xE0) == 0xC0)				\
			s += 2;						\
		else if ((c & 0xF0) == 0xE0)				\
			s += 3;						\
		else if ((c & 0xF8) == 0xF0)				\
			s += 4;						\
		else if ((c & 0xFC) == 0xF8)				\
			s += 5;						\
		else if ((c & 0xFE) == 0xFC)				\
			s += 6;						\
		else {							\
			error = PARSERUTILS_INVALID;			\
			break;						\
		}							\
									\
		l++;							\
	}								\
									\
	if (error == PARSERUTILS_OK)					\
		*len = l;						\
} while(0)

/**
 * Calculate the length (in bytes) of a UTF-8 character
 *
 * \param s      Pointer to start of character
 * \param len    Pointer to location to receive length
 * \param error  Location to receive error code
 */
#define UTF8_CHAR_BYTE_LENGTH(s, len, error)				\
do {									\
	if (s == NULL || len == NULL) {					\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	*len = numContinuations[s[0]] + 1 /* Start byte */;		\
									\
	error = PARSERUTILS_OK;						\
} while(0)

/**
 * Find previous legal UTF-8 char in string
 *
 * \param s        The string
 * \param off      Offset in the string to start at
 * \param prevoff  Pointer to location to receive offset of first byte of
 *                 previous legal character
 * \param error    Location to receive error code
 */
#define UTF8_PREV(s, off, prevoff, error)				\
do {									\
	if (s == NULL || prevoff == NULL) {				\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	while (off != 0 && (s[--off] & 0xC0) == 0x80)			\
		/* do nothing */;					\
									\
	*prevoff = off;							\
									\
	error = PARSERUTILS_OK;						\
} while(0)

/**
 * Find next legal UTF-8 char in string
 *
 * \param s        The string (assumed valid)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \param error    Location to receive error code
 */
#define UTF8_NEXT(s, len, off, nextoff, error)				\
do {									\
	if (s == NULL || off >= len || nextoff == NULL) {		\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	/* Skip current start byte (if present - may be mid-sequence) */\
	if (s[off] < 0x80 || (s[off] & 0xC0) == 0xC0)			\
		off++;							\
									\
	while (off < len && (s[off] & 0xC0) == 0x80)			\
		off++;							\
									\
	*nextoff = off;							\
									\
	error = PARSERUTILS_OK;						\
} while(0)

/**
 * Skip to start of next sequence in UTF-8 input
 *
 * \param s        The string (assumed to be of dubious validity)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \param error    Location to receive error code
 */
#define UTF8_NEXT_PARANOID(s, len, off, nextoff, error)			\
do {									\
	uint8_t c;							\
									\
	error = PARSERUTILS_OK;						\
									\
	if (s == NULL || off >= len || nextoff == NULL) {		\
		error = PARSERUTILS_BADPARM;				\
		break;							\
	}								\
									\
	c = s[off];							\
									\
	/* If we're mid-sequence, simply advance to next byte */	\
	if (!(c < 0x80 || (c & 0xC0) == 0xC0)) {			\
		off++;							\
	} else {							\
		uint32_t nCont = numContinuations[c];			\
		uint32_t nToSkip;					\
									\
		if (off + nCont + 1 >= len) {				\
			error = PARSERUTILS_NEEDDATA;			\
			break;						\
		}							\
									\
		/* Verify continuation bytes */				\
		for (nToSkip = 1; nToSkip <= nCont; nToSkip++) {	\
			if ((s[off + nToSkip] & 0xC0) != 0x80)		\
				break;					\
		}							\
									\
		/* Skip over the valid bytes */				\
		off += nToSkip;						\
	}								\
									\
	*nextoff = off;							\
} while(0)

#endif // parserutils_charset_encodings_utf8impl_h_


#ifndef parserutils_charset_codecs_codecimpl_h_
#define parserutils_charset_codecs_codecimpl_h_

/**
 * Core charset codec definition; implementations extend this
 */
struct parserutils_charset_codec {
	uint16_t mibenum;			/**< MIB enum for charset */

	parserutils_charset_codec_errormode errormode;	/**< error mode */

	struct {
		parserutils_error (*destroy)(parserutils_charset_codec *codec);
		parserutils_error (*encode_charset)(parserutils_charset_codec *codec,
				const uint8_t **source, size_t *sourcelen,
				uint8_t **dest, size_t *destlen);
		parserutils_error (*decode_charset)(parserutils_charset_codec *codec,
				const uint8_t **source, size_t *sourcelen,
				uint8_t **dest, size_t *destlen);
		parserutils_error (*reset)(parserutils_charset_codec *codec);
	} handler; /**< Vtable for handler code */
};

/**
 * Codec factory component definition
 */
typedef struct parserutils_charset_handler {
	bool (*handles_charset)(const char *charset);
	parserutils_error (*create)(const char *charset,
			parserutils_charset_codec **codec);
} parserutils_charset_handler;

#endif


#ifndef parserutils_charset_codecs_ext8tables_h_
#define parserutils_charset_codecs_ext8tables_h_

/* Mapping tables for extended 8bit -> UCS4.
 * Undefined characters are mapped to U+FFFF,
 * which is a guaranteed non-character
 */

static uint32_t w1250[128] = {
	0x20AC, 0xFFFF, 0x201A, 0xFFFF, 0x201E, 0x2026, 0x2020, 0x2021,
	0xFFFF, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0xFFFF, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
	0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
	0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
	0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
	0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
	0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
	0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
	0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
	0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
};

static uint32_t w1251[128] = {
	0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
	0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
	0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0xFFFF, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
	0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
	0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
	0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
	0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
};

static uint32_t w1252[128] = {
	0x20AC, 0xFFFF, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xFFFF, 0x017D, 0xFFFF,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xFFFF, 0x017E, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
};

static uint32_t w1253[128] = {
	0x20AC, 0xFFFF, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0xFFFF, 0x2030, 0xFFFF, 0x2039, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0xFFFF, 0x2122, 0xFFFF, 0x203A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x00A0, 0x0385, 0x0386, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0xFFFF, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x2015,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x00B5, 0x00B6, 0x00B7,
	0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
	0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
	0x03A0, 0x03A1, 0xFFFF, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
	0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
	0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
	0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
	0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0xFFFF,
};

static uint32_t w1254[128] = {
	0x20AC, 0xFFFF, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xFFFF, 0xFFFF, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF,
};

static uint32_t w1255[128] = {
	0x20AC, 0xFFFF, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0xFFFF, 0x2039, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0xFFFF, 0x203A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AA, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x05B0, 0x05B1, 0x05B2, 0x05B3, 0x05B4, 0x05B5, 0x05B6, 0x05B7,
	0x05B8, 0x05B9, 0xFFFF, 0x05BB, 0x05BC, 0x05BD, 0x05BE, 0x05BF,
	0x05C0, 0x05C1, 0x05C2, 0x05C3, 0x05F0, 0x05F1, 0x05F2, 0x05F3,
	0x05F4, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
	0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
	0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
	0x05E8, 0x05E9, 0x05EA, 0xFFFF, 0xFFFF, 0x200E, 0x200F, 0xFFFF,
};

static uint32_t w1256[128] = {
	0x20AC, 0x067E, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0679, 0x2039, 0x0152, 0x0686, 0x0698, 0x0688,
	0x06AF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x06A9, 0x2122, 0x0691, 0x203A, 0x0153, 0x200C, 0x200D, 0x06BA,
	0x00A0, 0x060C, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x06BE, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x061B, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x061F,
	0x06C1, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
	0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00D7,
	0x0637, 0x0638, 0x0639, 0x063A, 0x0640, 0x0641, 0x0642, 0x0643,
	0x00E0, 0x0644, 0x00E2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0649, 0x064A, 0x00EE, 0x00EF,
	0x064B, 0x064C, 0x064D, 0x064E, 0x00F4, 0x064F, 0x0650, 0x00F7,
	0x0651, 0x00F9, 0x0652, 0x00FB, 0x00FC, 0x200E, 0x200F, 0x06D2,
};

static uint32_t w1257[128] = {
	0x20AC, 0xFFFF, 0x201A, 0xFFFF, 0x201E, 0x2026, 0x2020, 0x2021,
	0xFFFF, 0x2030, 0xFFFF, 0x2039, 0xFFFF, 0x00A8, 0x02C7, 0x00B8,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0xFFFF, 0x2122, 0xFFFF, 0x203A, 0xFFFF, 0x00AF, 0x02DB, 0xFFFF,
	0x00A0, 0xFFFF, 0x00A2, 0x00A3, 0x00A4, 0xFFFF, 0x00A6, 0x00A7,
	0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
	0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
	0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
	0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
	0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
	0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
	0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
	0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
	0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x02D9,
};

static uint32_t w1258[128] = {
	0x20AC, 0xFFFF, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0xFFFF, 0x2039, 0x0152, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0xFFFF, 0x203A, 0x0153, 0xFFFF, 0xFFFF, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x0300, 0x00CD, 0x00CE, 0x00CF,
	0x0110, 0x00D1, 0x0309, 0x00D3, 0x00D4, 0x01A0, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x01AF, 0x0303, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0301, 0x00ED, 0x00EE, 0x00EF,
	0x0111, 0x00F1, 0x0323, 0x00F3, 0x00F4, 0x01A1, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x01B0, 0x20AB, 0x00FF,
};

#endif


#ifndef parserutils_charset_codecs_8859tables_h_
#define parserutils_charset_codecs_8859tables_h_

/* Mapping tables for ISO-8859-n -> UCS4.
 * Undefined characters are mapped to U+FFFF,
 * which is a guaranteed non-character
 */

static uint32_t t1[96] = {
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
};

static uint32_t t2[96] = {
	0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
	0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
	0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
	0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
	0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
	0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
	0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
	0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
	0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
	0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
};

static uint32_t t3[96] = {
	0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0xFFFF, 0x0124, 0x00A7,
	0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0xFFFF, 0x017B,
	0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7,
	0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0xFFFF, 0x017C,
	0x00C0, 0x00C1, 0x00C2, 0xFFFF, 0x00C4, 0x010A, 0x0108, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0xFFFF, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7,
	0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0xFFFF, 0x00E4, 0x010B, 0x0109, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0xFFFF, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,
	0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9,
};

static uint32_t t4[96] = {
	0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7,
	0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF,
	0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7,
	0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B,
	0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A,
	0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF,
	0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B,
	0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9,
};

static uint32_t t5[96] = {
	0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
	0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
	0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
	0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F,
};

static uint32_t t6[96] = {
	0x00A0, 0xFFFF, 0xFFFF, 0xFFFF, 0x00A4, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x060C, 0x00AD, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0x061B, 0xFFFF, 0xFFFF, 0xFFFF, 0x061F,
	0xFFFF, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
	0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
	0x0638, 0x0639, 0x063A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,
	0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
	0x0650, 0x0651, 0x0652, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

static uint32_t t7[96] = {
	0x00A0, 0x2018, 0x2019, 0x00A3, 0x20AC, 0x20AF, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x037A, 0x00AB, 0x00AC, 0x00AD, 0xFFFF, 0x2015,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x00B7,
	0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
	0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
	0x03A0, 0x03A1, 0xFFFF, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
	0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
	0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
	0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
	0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0xFFFF,
};

static uint32_t t8[96] = {
	0x00A0, 0xFFFF, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x2017,
	0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
	0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
	0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
	0x05E8, 0x05E9, 0x05EA, 0xFFFF, 0xFFFF, 0x200E, 0x200F, 0xFFFF,
};

static uint32_t t9[96] = {
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF,
};

static uint32_t t10[96] = {
	0x00A0, 0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7,
	0x013B, 0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A,
	0x00B0, 0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7,
	0x013C, 0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B,
	0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168,
	0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169,
	0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138,
};

static uint32_t t11[96] = {
	0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07,
	0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
	0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17,
	0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
	0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27,
	0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
	0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
	0x0E38, 0x0E39, 0x0E3A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0E3F,
	0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47,
	0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
	0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57,
	0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

static uint32_t t13[96] = {
	0x00A0, 0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7,
	0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7,
	0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
	0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
	0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
	0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
	0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
	0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
	0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
	0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
	0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019,
};

static uint32_t t14[96] = {
	0x00A0, 0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7,
	0x1E80, 0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
	0x1E1E, 0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56,
	0x1E81, 0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x1E6A,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x1E6B,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF,
};

static uint32_t t15[96] = {
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7,
	0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7,
	0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
};

static uint32_t t16[96] = {
	0x00A0, 0x0104, 0x0105, 0x0141, 0x20AC, 0x201E, 0x0160, 0x00A7,
	0x0161, 0x00A9, 0x0218, 0x00AB, 0x0179, 0x00AD, 0x017A, 0x017B,
	0x00B0, 0x00B1, 0x010C, 0x0142, 0x017D, 0x201D, 0x00B6, 0x00B7,
	0x017E, 0x010D, 0x0219, 0x00BB, 0x0152, 0x0153, 0x0178, 0x017C,
	0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0106, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x0110, 0x0143, 0x00D2, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x015A,
	0x0170, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0118, 0x021A, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x0107, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x0111, 0x0144, 0x00F2, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x015B,
	0x0171, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0119, 0x021B, 0x00FF,
};

#endif



/* Bring in the aliases tables */
/*
 * This file is part of LibParserUtils.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2010 The NetSurf Project.
 *
 * Note: This file is automatically generated by make-aliases.pl
 *
 * Do not edit file file, changes will be overwritten during build.
 */

static parserutils_charset_aliases_canon canonical_charset_names[] = {
	{ 74, 16, "ANSI_X3.110-1983" },
	{ 65, 8, "ASMO_449" },
	{ 2005, 23, "Adobe-Standard-Encoding" },
	{ 2020, 21, "Adobe-Symbol-Encoding" },
	{ 2104, 10, "Amiga-1251" },
	{ 1020, 6, "BOCU-1" },
	{ 20, 7, "BS_4730" },
	{ 50, 11, "BS_viewdata" },
	{ 2026, 4, "Big5" },
	{ 2101, 10, "Big5-HKSCS" },
	{ 1016, 6, "CESU-8" },
	{ 3004, 5, "CP874" },
	{ 3027, 5, "CP949" },
	{ 78, 17, "CSA_Z243.4-1985-1" },
	{ 79, 17, "CSA_Z243.4-1985-2" },
	{ 80, 18, "CSA_Z243.4-1985-gr" },
	{ 86, 10, "CSN_369103" },
	{ 2008, 7, "DEC-MCS" },
	{ 24, 9, "DIN_66003" },
	{ 99, 7, "DS_2089" },
	{ 2064, 12, "EBCDIC-AT-DE" },
	{ 2065, 14, "EBCDIC-AT-DE-A" },
	{ 2066, 12, "EBCDIC-CA-FR" },
	{ 2067, 12, "EBCDIC-DK-NO" },
	{ 2068, 14, "EBCDIC-DK-NO-A" },
	{ 2074, 9, "EBCDIC-ES" },
	{ 2075, 11, "EBCDIC-ES-A" },
	{ 2076, 11, "EBCDIC-ES-S" },
	{ 2069, 12, "EBCDIC-FI-SE" },
	{ 2070, 14, "EBCDIC-FI-SE-A" },
	{ 2071, 9, "EBCDIC-FR" },
	{ 2072, 9, "EBCDIC-IT" },
	{ 2073, 9, "EBCDIC-PT" },
	{ 2077, 9, "EBCDIC-UK" },
	{ 2078, 9, "EBCDIC-US" },
	{ 77, 13, "ECMA-cyrillic" },
	{ 23, 2, "ES" },
	{ 61, 3, "ES2" },
	{ 18, 6, "EUC-JP" },
	{ 38, 6, "EUC-KR" },
	{ 19, 43, "Extended_UNIX_Code_Fixed_Width_for_Japanese" },
	{ 114, 7, "GB18030" },
	{ 2025, 6, "GB2312" },
	{ 113, 3, "GBK" },
	{ 56, 10, "GB_1988-80" },
	{ 57, 10, "GB_2312-80" },
	{ 94, 13, "GOST_19768-74" },
	{ 2021, 10, "HP-DeskTop" },
	{ 2017, 8, "HP-Legal" },
	{ 2019, 8, "HP-Math8" },
	{ 2018, 10, "HP-Pi-font" },
	{ 2085, 10, "HZ-GB-2312" },
	{ 2015, 11, "IBM-Symbols" },
	{ 2016, 8, "IBM-Thai" },
	{ 2089, 8, "IBM00858" },
	{ 2090, 8, "IBM00924" },
	{ 2091, 8, "IBM01140" },
	{ 2092, 8, "IBM01141" },
	{ 2093, 8, "IBM01142" },
	{ 2094, 8, "IBM01143" },
	{ 2095, 8, "IBM01144" },
	{ 2096, 8, "IBM01145" },
	{ 2097, 8, "IBM01146" },
	{ 2098, 8, "IBM01147" },
	{ 2099, 8, "IBM01148" },
	{ 2100, 8, "IBM01149" },
	{ 2028, 6, "IBM037" },
	{ 2029, 6, "IBM038" },
	{ 2063, 7, "IBM1026" },
	{ 2102, 7, "IBM1047" },
	{ 2030, 6, "IBM273" },
	{ 2031, 6, "IBM274" },
	{ 2032, 6, "IBM275" },
	{ 2033, 6, "IBM277" },
	{ 2034, 6, "IBM278" },
	{ 2035, 6, "IBM280" },
	{ 2036, 6, "IBM281" },
	{ 2037, 6, "IBM284" },
	{ 2038, 6, "IBM285" },
	{ 2039, 6, "IBM290" },
	{ 2040, 6, "IBM297" },
	{ 2041, 6, "IBM420" },
	{ 2042, 6, "IBM423" },
	{ 2043, 6, "IBM424" },
	{ 2011, 6, "IBM437" },
	{ 2044, 6, "IBM500" },
	{ 2087, 6, "IBM775" },
	{ 2009, 6, "IBM850" },
	{ 2045, 6, "IBM851" },
	{ 2010, 6, "IBM852" },
	{ 2046, 6, "IBM855" },
	{ 2047, 6, "IBM857" },
	{ 2048, 6, "IBM860" },
	{ 2049, 6, "IBM861" },
	{ 2013, 6, "IBM862" },
	{ 2050, 6, "IBM863" },
	{ 2051, 6, "IBM864" },
	{ 2052, 6, "IBM865" },
	{ 2086, 6, "IBM866" },
	{ 2053, 6, "IBM868" },
	{ 2054, 6, "IBM869" },
	{ 2055, 6, "IBM870" },
	{ 2056, 6, "IBM871" },
	{ 2057, 6, "IBM880" },
	{ 2058, 6, "IBM891" },
	{ 2059, 6, "IBM903" },
	{ 2060, 6, "IBM904" },
	{ 2061, 6, "IBM905" },
	{ 2062, 6, "IBM918" },
	{ 88, 9, "IEC_P27-1" },
	{ 51, 4, "INIS" },
	{ 52, 6, "INIS-8" },
	{ 53, 13, "INIS-cyrillic" },
	{ 29, 9, "INVARIANT" },
	{ 1000, 15, "ISO-10646-UCS-2" },
	{ 1001, 15, "ISO-10646-UCS-4" },
	{ 1002, 19, "ISO-10646-UCS-Basic" },
	{ 27, 15, "ISO-10646-UTF-1" },
	{ 1003, 24, "ISO-10646-Unicode-Latin1" },
	{ 104, 11, "ISO-2022-CN" },
	{ 105, 15, "ISO-2022-CN-EXT" },
	{ 39, 11, "ISO-2022-JP" },
	{ 4008, 13, "ISO-2022-JP-1" },
	{ 40, 13, "ISO-2022-JP-2" },
	{ 37, 11, "ISO-2022-KR" },
	{ 4, 10, "ISO-8859-1" },
	{ 2000, 30, "ISO-8859-1-Windows-3.0-Latin-1" },
	{ 2001, 30, "ISO-8859-1-Windows-3.1-Latin-1" },
	{ 13, 11, "ISO-8859-10" },
	{ 4014, 11, "ISO-8859-11" },
	{ 109, 11, "ISO-8859-13" },
	{ 110, 11, "ISO-8859-14" },
	{ 111, 11, "ISO-8859-15" },
	{ 112, 11, "ISO-8859-16" },
	{ 5, 10, "ISO-8859-2" },
	{ 2002, 26, "ISO-8859-2-Windows-Latin-2" },
	{ 6, 10, "ISO-8859-3" },
	{ 7, 10, "ISO-8859-4" },
	{ 8, 10, "ISO-8859-5" },
	{ 9, 10, "ISO-8859-6" },
	{ 81, 12, "ISO-8859-6-E" },
	{ 82, 12, "ISO-8859-6-I" },
	{ 10, 10, "ISO-8859-7" },
	{ 11, 10, "ISO-8859-8" },
	{ 84, 12, "ISO-8859-8-E" },
	{ 85, 12, "ISO-8859-8-I" },
	{ 12, 10, "ISO-8859-9" },
	{ 2003, 26, "ISO-8859-9-Windows-Latin-5" },
	{ 4000, 10, "ISO-IR-182" },
	{ 4002, 10, "ISO-IR-197" },
	{ 1005, 20, "ISO-Unicode-IBM-1261" },
	{ 1008, 20, "ISO-Unicode-IBM-1264" },
	{ 1009, 20, "ISO-Unicode-IBM-1265" },
	{ 1006, 20, "ISO-Unicode-IBM-1268" },
	{ 1007, 20, "ISO-Unicode-IBM-1276" },
	{ 96, 13, "ISO_10367-box" },
	{ 73, 13, "ISO_2033-1983" },
	{ 48, 8, "ISO_5427" },
	{ 54, 13, "ISO_5427:1981" },
	{ 55, 13, "ISO_5428:1980" },
	{ 28, 18, "ISO_646.basic:1983" },
	{ 30, 16, "ISO_646.irv:1983" },
	{ 93, 13, "ISO_6937-2-25" },
	{ 14, 14, "ISO_6937-2-add" },
	{ 95, 13, "ISO_8859-supp" },
	{ 22, 2, "IT" },
	{ 41, 17, "JIS_C6220-1969-jp" },
	{ 42, 17, "JIS_C6220-1969-ro" },
	{ 49, 14, "JIS_C6226-1978" },
	{ 63, 14, "JIS_C6226-1983" },
	{ 67, 16, "JIS_C6229-1984-a" },
	{ 68, 16, "JIS_C6229-1984-b" },
	{ 69, 20, "JIS_C6229-1984-b-add" },
	{ 70, 19, "JIS_C6229-1984-hand" },
	{ 71, 23, "JIS_C6229-1984-hand-add" },
	{ 72, 19, "JIS_C6229-1984-kana" },
	{ 16, 12, "JIS_Encoding" },
	{ 15, 9, "JIS_X0201" },
	{ 98, 14, "JIS_X0212-1990" },
	{ 4012, 5, "JOHAB" },
	{ 87, 12, "JUS_I.B1.002" },
	{ 90, 16, "JUS_I.B1.003-mac" },
	{ 89, 17, "JUS_I.B1.003-serb" },
	{ 2105, 13, "KOI7-switched" },
	{ 2084, 6, "KOI8-R" },
	{ 2088, 6, "KOI8-U" },
	{ 102, 7, "KSC5636" },
	{ 36, 14, "KS_C_5601-1987" },
	{ 47, 13, "Latin-greek-1" },
	{ 4011, 16, "MACCENTRALEUROPE" },
	{ 4009, 11, "MACCYRILLIC" },
	{ 4010, 10, "MACUKRAINE" },
	{ 2081, 4, "MNEM" },
	{ 2080, 8, "MNEMONIC" },
	{ 62, 10, "MSZ_7795.3" },
	{ 2023, 20, "Microsoft-Publishing" },
	{ 33, 9, "NATS-DANO" },
	{ 34, 13, "NATS-DANO-ADD" },
	{ 31, 9, "NATS-SEFI" },
	{ 32, 13, "NATS-SEFI-ADD" },
	{ 92, 13, "NC_NC00-10:81" },
	{ 26, 11, "NF_Z_62-010" },
	{ 46, 18, "NF_Z_62-010_(1973)" },
	{ 25, 9, "NS_4551-1" },
	{ 58, 9, "NS_4551-2" },
	{ 116, 19, "OSD_EBCDIC_DF03_IRV" },
	{ 117, 17, "OSD_EBCDIC_DF04_1" },
	{ 115, 18, "OSD_EBCDIC_DF04_15" },
	{ 2012, 20, "PC8-Danish-Norwegian" },
	{ 2014, 11, "PC8-Turkish" },
	{ 43, 2, "PT" },
	{ 60, 3, "PT2" },
	{ 2103, 7, "PTCP154" },
	{ 1011, 4, "SCSU" },
	{ 35, 12, "SEN_850200_B" },
	{ 21, 12, "SEN_850200_C" },
	{ 17, 9, "Shift_JIS" },
	{ 83, 8, "T.101-G2" },
	{ 75, 9, "T.61-7bit" },
	{ 76, 9, "T.61-8bit" },
	{ 2259, 7, "TIS-620" },
	{ 1010, 11, "UNICODE-1-1" },
	{ 103, 17, "UNICODE-1-1-UTF-7" },
	{ 2079, 12, "UNKNOWN-8BIT" },
	{ 3, 8, "US-ASCII" },
	{ 1015, 6, "UTF-16" },
	{ 1013, 8, "UTF-16BE" },
	{ 1014, 8, "UTF-16LE" },
	{ 1017, 6, "UTF-32" },
	{ 1018, 8, "UTF-32BE" },
	{ 1019, 8, "UTF-32LE" },
	{ 1012, 5, "UTF-7" },
	{ 106, 5, "UTF-8" },
	{ 2083, 4, "VIQR" },
	{ 2082, 6, "VISCII" },
	{ 2007, 21, "Ventura-International" },
	{ 2022, 12, "Ventura-Math" },
	{ 2006, 10, "Ventura-US" },
	{ 2024, 11, "Windows-31J" },
	{ 5002, 13, "X-ACORN-FUZZY" },
	{ 5001, 14, "X-ACORN-LATIN1" },
	{ 4999, 9, "X-CURRENT" },
	{ 101, 5, "dk-us" },
	{ 91, 11, "greek-ccitt" },
	{ 64, 6, "greek7" },
	{ 44, 10, "greek7-old" },
	{ 2004, 9, "hp-roman8" },
	{ 66, 9, "iso-ir-90" },
	{ 45, 11, "latin-greek" },
	{ 97, 9, "latin-lap" },
	{ 2027, 9, "macintosh" },
	{ 100, 5, "us-dk" },
	{ 59, 14, "videotex-suppl" },
	{ 2250, 12, "windows-1250" },
	{ 2251, 12, "windows-1251" },
	{ 2252, 12, "windows-1252" },
	{ 2253, 12, "windows-1253" },
	{ 2254, 12, "windows-1254" },
	{ 2255, 12, "windows-1255" },
	{ 2256, 12, "windows-1256" },
	{ 2257, 12, "windows-1257" },
	{ 2258, 12, "windows-1258" },
};

static const uint16_t charset_aliases_canon_count = 262;

typedef struct {
	uint16_t name_len;
	const char *name;
	parserutils_charset_aliases_canon *canon;
} parserutils_charset_aliases_alias;

static parserutils_charset_aliases_alias charset_aliases[] = {
	{ 3, "437", &canonical_charset_names[84] },
	{ 3, "850", &canonical_charset_names[87] },
	{ 3, "851", &canonical_charset_names[88] },
	{ 3, "852", &canonical_charset_names[89] },
	{ 3, "855", &canonical_charset_names[90] },
	{ 3, "857", &canonical_charset_names[91] },
	{ 3, "860", &canonical_charset_names[92] },
	{ 3, "861", &canonical_charset_names[93] },
	{ 3, "862", &canonical_charset_names[94] },
	{ 3, "863", &canonical_charset_names[95] },
	{ 3, "865", &canonical_charset_names[97] },
	{ 3, "866", &canonical_charset_names[98] },
	{ 3, "869", &canonical_charset_names[100] },
	{ 5, "88591", &canonical_charset_names[125] },
	{ 6, "885910", &canonical_charset_names[128] },
	{ 6, "885911", &canonical_charset_names[129] },
	{ 6, "885913", &canonical_charset_names[130] },
	{ 6, "885914", &canonical_charset_names[131] },
	{ 6, "885915", &canonical_charset_names[132] },
	{ 5, "88592", &canonical_charset_names[134] },
	{ 5, "88593", &canonical_charset_names[136] },
	{ 5, "88594", &canonical_charset_names[137] },
	{ 5, "88595", &canonical_charset_names[138] },
	{ 5, "88597", &canonical_charset_names[142] },
	{ 5, "88598", &canonical_charset_names[143] },
	{ 5, "88599", &canonical_charset_names[146] },
	{ 3, "904", &canonical_charset_names[106] },
	{ 21, "adobestandardencoding", &canonical_charset_names[2] },
	{ 19, "adobesymbolencoding", &canonical_charset_names[3] },
	{ 7, "ami1251", &canonical_charset_names[4] },
	{ 9, "amiga1251", &canonical_charset_names[4] },
	{ 13, "ansix31101983", &canonical_charset_names[0] },
	{ 11, "ansix341968", &canonical_charset_names[224] },
	{ 11, "ansix341986", &canonical_charset_names[224] },
	{ 6, "arabic", &canonical_charset_names[139] },
	{ 7, "arabic7", &canonical_charset_names[1] },
	{ 5, "ascii", &canonical_charset_names[224] },
	{ 7, "asmo449", &canonical_charset_names[1] },
	{ 7, "asmo708", &canonical_charset_names[139] },
	{ 4, "big5", &canonical_charset_names[8] },
	{ 9, "big5hkscs", &canonical_charset_names[9] },
	{ 7, "bigfive", &canonical_charset_names[8] },
	{ 5, "bocu1", &canonical_charset_names[5] },
	{ 6, "bs4730", &canonical_charset_names[6] },
	{ 10, "bsviewdata", &canonical_charset_names[7] },
	{ 2, "ca", &canonical_charset_names[13] },
	{ 10, "ccsid00858", &canonical_charset_names[54] },
	{ 10, "ccsid00924", &canonical_charset_names[55] },
	{ 10, "ccsid01140", &canonical_charset_names[56] },
	{ 10, "ccsid01141", &canonical_charset_names[57] },
	{ 10, "ccsid01142", &canonical_charset_names[58] },
	{ 10, "ccsid01143", &canonical_charset_names[59] },
	{ 10, "ccsid01144", &canonical_charset_names[60] },
	{ 10, "ccsid01145", &canonical_charset_names[61] },
	{ 10, "ccsid01146", &canonical_charset_names[62] },
	{ 10, "ccsid01147", &canonical_charset_names[63] },
	{ 10, "ccsid01148", &canonical_charset_names[64] },
	{ 10, "ccsid01149", &canonical_charset_names[65] },
	{ 5, "cesu8", &canonical_charset_names[10] },
	{ 7, "chinese", &canonical_charset_names[45] },
	{ 2, "cn", &canonical_charset_names[44] },
	{ 6, "cnbig5", &canonical_charset_names[8] },
	{ 4, "cngb", &canonical_charset_names[42] },
	{ 7, "cp00858", &canonical_charset_names[54] },
	{ 7, "cp00924", &canonical_charset_names[55] },
	{ 7, "cp01140", &canonical_charset_names[56] },
	{ 7, "cp01141", &canonical_charset_names[57] },
	{ 7, "cp01142", &canonical_charset_names[58] },
	{ 7, "cp01143", &canonical_charset_names[59] },
	{ 7, "cp01144", &canonical_charset_names[60] },
	{ 7, "cp01145", &canonical_charset_names[61] },
	{ 7, "cp01146", &canonical_charset_names[62] },
	{ 7, "cp01147", &canonical_charset_names[63] },
	{ 7, "cp01148", &canonical_charset_names[64] },
	{ 7, "cp01149", &canonical_charset_names[65] },
	{ 5, "cp037", &canonical_charset_names[66] },
	{ 5, "cp038", &canonical_charset_names[67] },
	{ 6, "cp1026", &canonical_charset_names[68] },
	{ 6, "cp1250", &canonical_charset_names[253] },
	{ 6, "cp1251", &canonical_charset_names[254] },
	{ 6, "cp1252", &canonical_charset_names[255] },
	{ 6, "cp1253", &canonical_charset_names[256] },
	{ 6, "cp1254", &canonical_charset_names[257] },
	{ 6, "cp1256", &canonical_charset_names[259] },
	{ 6, "cp1257", &canonical_charset_names[260] },
	{ 5, "cp154", &canonical_charset_names[212] },
	{ 5, "cp273", &canonical_charset_names[70] },
	{ 5, "cp274", &canonical_charset_names[71] },
	{ 5, "cp275", &canonical_charset_names[72] },
	{ 5, "cp278", &canonical_charset_names[74] },
	{ 5, "cp280", &canonical_charset_names[75] },
	{ 5, "cp281", &canonical_charset_names[76] },
	{ 5, "cp284", &canonical_charset_names[77] },
	{ 5, "cp285", &canonical_charset_names[78] },
	{ 5, "cp290", &canonical_charset_names[79] },
	{ 5, "cp297", &canonical_charset_names[80] },
	{ 5, "cp367", &canonical_charset_names[224] },
	{ 5, "cp420", &canonical_charset_names[81] },
	{ 5, "cp423", &canonical_charset_names[82] },
	{ 5, "cp424", &canonical_charset_names[83] },
	{ 5, "cp437", &canonical_charset_names[84] },
	{ 5, "cp500", &canonical_charset_names[85] },
	{ 5, "cp775", &canonical_charset_names[86] },
	{ 5, "cp819", &canonical_charset_names[125] },
	{ 5, "cp850", &canonical_charset_names[87] },
	{ 5, "cp851", &canonical_charset_names[88] },
	{ 5, "cp852", &canonical_charset_names[89] },
	{ 5, "cp855", &canonical_charset_names[90] },
	{ 5, "cp857", &canonical_charset_names[91] },
	{ 5, "cp860", &canonical_charset_names[92] },
	{ 5, "cp861", &canonical_charset_names[93] },
	{ 5, "cp862", &canonical_charset_names[94] },
	{ 5, "cp863", &canonical_charset_names[95] },
	{ 5, "cp864", &canonical_charset_names[96] },
	{ 5, "cp865", &canonical_charset_names[97] },
	{ 5, "cp866", &canonical_charset_names[98] },
	{ 5, "cp868", &canonical_charset_names[99] },
	{ 5, "cp869", &canonical_charset_names[100] },
	{ 5, "cp870", &canonical_charset_names[101] },
	{ 5, "cp871", &canonical_charset_names[102] },
	{ 5, "cp874", &canonical_charset_names[11] },
	{ 5, "cp880", &canonical_charset_names[103] },
	{ 5, "cp891", &canonical_charset_names[104] },
	{ 5, "cp903", &canonical_charset_names[105] },
	{ 5, "cp904", &canonical_charset_names[106] },
	{ 5, "cp905", &canonical_charset_names[107] },
	{ 5, "cp918", &canonical_charset_names[108] },
	{ 5, "cp936", &canonical_charset_names[43] },
	{ 5, "cp949", &canonical_charset_names[12] },
	{ 4, "cpar", &canonical_charset_names[99] },
	{ 4, "cpgr", &canonical_charset_names[100] },
	{ 4, "cpis", &canonical_charset_names[93] },
	{ 5, "csa71", &canonical_charset_names[13] },
	{ 5, "csa72", &canonical_charset_names[14] },
	{ 23, "csadobestandardencoding", &canonical_charset_names[2] },
	{ 7, "csascii", &canonical_charset_names[224] },
	{ 11, "csat5001983", &canonical_charset_names[0] },
	{ 13, "csaz243419851", &canonical_charset_names[13] },
	{ 13, "csaz243419852", &canonical_charset_names[14] },
	{ 14, "csaz24341985gr", &canonical_charset_names[15] },
	{ 6, "csbig5", &canonical_charset_names[8] },
	{ 7, "csbocu1", &canonical_charset_names[5] },
	{ 7, "cscesu8", &canonical_charset_names[10] },
	{ 8, "csdecmcs", &canonical_charset_names[17] },
	{ 6, "csdkus", &canonical_charset_names[242] },
	{ 13, "csebcdicatdea", &canonical_charset_names[21] },
	{ 12, "csebcdiccafr", &canonical_charset_names[22] },
	{ 12, "csebcdicdkno", &canonical_charset_names[23] },
	{ 13, "csebcdicdknoa", &canonical_charset_names[24] },
	{ 10, "csebcdices", &canonical_charset_names[25] },
	{ 11, "csebcdicesa", &canonical_charset_names[26] },
	{ 11, "csebcdicess", &canonical_charset_names[27] },
	{ 12, "csebcdicfise", &canonical_charset_names[28] },
	{ 13, "csebcdicfisea", &canonical_charset_names[29] },
	{ 10, "csebcdicfr", &canonical_charset_names[30] },
	{ 10, "csebcdicit", &canonical_charset_names[31] },
	{ 10, "csebcdicpt", &canonical_charset_names[32] },
	{ 10, "csebcdicuk", &canonical_charset_names[33] },
	{ 10, "csebcdicus", &canonical_charset_names[34] },
	{ 19, "cseucfixwidjapanese", &canonical_charset_names[40] },
	{ 7, "cseuckr", &canonical_charset_names[39] },
	{ 19, "cseucpkdfmtjapanese", &canonical_charset_names[38] },
	{ 8, "csgb2312", &canonical_charset_names[42] },
	{ 19, "cshalfwidthkatakana", &canonical_charset_names[177] },
	{ 11, "cshpdesktop", &canonical_charset_names[47] },
	{ 9, "cshplegal", &canonical_charset_names[48] },
	{ 9, "cshpmath8", &canonical_charset_names[49] },
	{ 10, "cshppifont", &canonical_charset_names[50] },
	{ 10, "cshppsmath", &canonical_charset_names[3] },
	{ 10, "cshproman8", &canonical_charset_names[246] },
	{ 9, "csibbm904", &canonical_charset_names[106] },
	{ 8, "csibm037", &canonical_charset_names[66] },
	{ 8, "csibm038", &canonical_charset_names[67] },
	{ 9, "csibm1026", &canonical_charset_names[68] },
	{ 8, "csibm273", &canonical_charset_names[70] },
	{ 8, "csibm274", &canonical_charset_names[71] },
	{ 8, "csibm275", &canonical_charset_names[72] },
	{ 8, "csibm277", &canonical_charset_names[73] },
	{ 8, "csibm278", &canonical_charset_names[74] },
	{ 8, "csibm280", &canonical_charset_names[75] },
	{ 8, "csibm281", &canonical_charset_names[76] },
	{ 8, "csibm284", &canonical_charset_names[77] },
	{ 8, "csibm285", &canonical_charset_names[78] },
	{ 8, "csibm290", &canonical_charset_names[79] },
	{ 8, "csibm297", &canonical_charset_names[80] },
	{ 8, "csibm420", &canonical_charset_names[81] },
	{ 8, "csibm423", &canonical_charset_names[82] },
	{ 8, "csibm424", &canonical_charset_names[83] },
	{ 8, "csibm500", &canonical_charset_names[85] },
	{ 8, "csibm851", &canonical_charset_names[88] },
	{ 8, "csibm855", &canonical_charset_names[90] },
	{ 8, "csibm857", &canonical_charset_names[91] },
	{ 8, "csibm860", &canonical_charset_names[92] },
	{ 8, "csibm861", &canonical_charset_names[93] },
	{ 8, "csibm863", &canonical_charset_names[95] },
	{ 8, "csibm864", &canonical_charset_names[96] },
	{ 8, "csibm865", &canonical_charset_names[97] },
	{ 8, "csibm866", &canonical_charset_names[98] },
	{ 8, "csibm868", &canonical_charset_names[99] },
	{ 8, "csibm869", &canonical_charset_names[100] },
	{ 8, "csibm870", &canonical_charset_names[101] },
	{ 8, "csibm871", &canonical_charset_names[102] },
	{ 8, "csibm880", &canonical_charset_names[103] },
	{ 8, "csibm891", &canonical_charset_names[104] },
	{ 8, "csibm903", &canonical_charset_names[105] },
	{ 8, "csibm905", &canonical_charset_names[107] },
	{ 8, "csibm918", &canonical_charset_names[108] },
	{ 15, "csibmebcdicatde", &canonical_charset_names[20] },
	{ 12, "csibmsymbols", &canonical_charset_names[52] },
	{ 9, "csibmthai", &canonical_charset_names[53] },
	{ 11, "csinvariant", &canonical_charset_names[113] },
	{ 15, "csiso102t617bit", &canonical_charset_names[218] },
	{ 13, "csiso10367box", &canonical_charset_names[155] },
	{ 15, "csiso103t618bit", &canonical_charset_names[219] },
	{ 14, "csiso10646utf1", &canonical_charset_names[117] },
	{ 14, "csiso10swedish", &canonical_charset_names[214] },
	{ 20, "csiso111ecmacyrillic", &canonical_charset_names[35] },
	{ 22, "csiso11swedishfornames", &canonical_charset_names[215] },
	{ 17, "csiso121canadian1", &canonical_charset_names[13] },
	{ 17, "csiso122canadian2", &canonical_charset_names[14] },
	{ 22, "csiso123csaz24341985gr", &canonical_charset_names[15] },
	{ 14, "csiso128t101g2", &canonical_charset_names[217] },
	{ 17, "csiso139csn369103", &canonical_charset_names[16] },
	{ 17, "csiso13jisc6220jp", &canonical_charset_names[166] },
	{ 17, "csiso141jusib1002", &canonical_charset_names[180] },
	{ 15, "csiso143iecp271", &canonical_charset_names[109] },
	{ 15, "csiso146serbian", &canonical_charset_names[182] },
	{ 18, "csiso147macedonian", &canonical_charset_names[181] },
	{ 17, "csiso14jisc6220ro", &canonical_charset_names[167] },
	{ 8, "csiso150", &canonical_charset_names[243] },
	{ 18, "csiso150greekccitt", &canonical_charset_names[243] },
	{ 12, "csiso151cuba", &canonical_charset_names[200] },
	{ 19, "csiso153gost1976874", &canonical_charset_names[46] },
	{ 11, "csiso158lap", &canonical_charset_names[249] },
	{ 20, "csiso159jisx02121990", &canonical_charset_names[178] },
	{ 14, "csiso15italian", &canonical_charset_names[165] },
	{ 17, "csiso16portuguese", &canonical_charset_names[210] },
	{ 14, "csiso17spanish", &canonical_charset_names[36] },
	{ 16, "csiso18greek7old", &canonical_charset_names[245] },
	{ 17, "csiso19latingreek", &canonical_charset_names[248] },
	{ 11, "csiso2022jp", &canonical_charset_names[121] },
	{ 12, "csiso2022jp2", &canonical_charset_names[123] },
	{ 11, "csiso2022kr", &canonical_charset_names[124] },
	{ 9, "csiso2033", &canonical_charset_names[156] },
	{ 13, "csiso21german", &canonical_charset_names[18] },
	{ 13, "csiso25french", &canonical_charset_names[202] },
	{ 18, "csiso27latingreek1", &canonical_charset_names[188] },
	{ 20, "csiso2intlrefversion", &canonical_charset_names[161] },
	{ 19, "csiso42jisc62261978", &canonical_charset_names[168] },
	{ 17, "csiso47bsviewdata", &canonical_charset_names[7] },
	{ 11, "csiso49inis", &canonical_charset_names[110] },
	{ 19, "csiso4unitedkingdom", &canonical_charset_names[6] },
	{ 12, "csiso50inis8", &canonical_charset_names[111] },
	{ 19, "csiso51iniscyrillic", &canonical_charset_names[112] },
	{ 17, "csiso5427cyrillic", &canonical_charset_names[157] },
	{ 14, "csiso5428greek", &canonical_charset_names[159] },
	{ 13, "csiso57gb1988", &canonical_charset_names[44] },
	{ 15, "csiso58gb231280", &canonical_charset_names[45] },
	{ 22, "csiso60danishnorwegian", &canonical_charset_names[203] },
	{ 17, "csiso60norwegian1", &canonical_charset_names[203] },
	{ 17, "csiso61norwegian2", &canonical_charset_names[204] },
	{ 17, "csiso646basic1983", &canonical_charset_names[160] },
	{ 14, "csiso646danish", &canonical_charset_names[19] },
	{ 12, "csiso6937add", &canonical_charset_names[162] },
	{ 13, "csiso69french", &canonical_charset_names[201] },
	{ 20, "csiso70videotexsupp1", &canonical_charset_names[252] },
	{ 18, "csiso84portuguese2", &canonical_charset_names[211] },
	{ 15, "csiso85spanish2", &canonical_charset_names[37] },
	{ 16, "csiso86hungarian", &canonical_charset_names[194] },
	{ 15, "csiso87jisx0208", &canonical_charset_names[169] },
	{ 11, "csiso88596e", &canonical_charset_names[140] },
	{ 11, "csiso88596i", &canonical_charset_names[141] },
	{ 11, "csiso88598e", &canonical_charset_names[144] },
	{ 11, "csiso88598i", &canonical_charset_names[145] },
	{ 13, "csiso8859supp", &canonical_charset_names[164] },
	{ 13, "csiso88greek7", &canonical_charset_names[244] },
	{ 14, "csiso89asmo449", &canonical_charset_names[1] },
	{ 7, "csiso90", &canonical_charset_names[247] },
	{ 20, "csiso91jisc62291984a", &canonical_charset_names[170] },
	{ 20, "csiso92jisc62991984b", &canonical_charset_names[171] },
	{ 22, "csiso93jis62291984badd", &canonical_charset_names[172] },
	{ 22, "csiso94jis62291984hand", &canonical_charset_names[173] },
	{ 25, "csiso95jis62291984handadd", &canonical_charset_names[174] },
	{ 23, "csiso96jisc62291984kana", &canonical_charset_names[175] },
	{ 13, "csiso99naplps", &canonical_charset_names[0] },
	{ 11, "csisolatin1", &canonical_charset_names[125] },
	{ 11, "csisolatin2", &canonical_charset_names[134] },
	{ 11, "csisolatin3", &canonical_charset_names[136] },
	{ 11, "csisolatin4", &canonical_charset_names[137] },
	{ 11, "csisolatin5", &canonical_charset_names[146] },
	{ 11, "csisolatin6", &canonical_charset_names[128] },
	{ 16, "csisolatinarabic", &canonical_charset_names[139] },
	{ 18, "csisolatincyrillic", &canonical_charset_names[138] },
	{ 15, "csisolatingreek", &canonical_charset_names[142] },
	{ 16, "csisolatinhebrew", &canonical_charset_names[143] },
	{ 13, "csisotextcomm", &canonical_charset_names[163] },
	{ 13, "csjisencoding", &canonical_charset_names[176] },
	{ 7, "cskoi8r", &canonical_charset_names[184] },
	{ 13, "csksc56011987", &canonical_charset_names[187] },
	{ 9, "csksc5636", &canonical_charset_names[186] },
	{ 11, "csmacintosh", &canonical_charset_names[250] },
	{ 21, "csmicrosoftpublishing", &canonical_charset_names[195] },
	{ 6, "csmnem", &canonical_charset_names[192] },
	{ 10, "csmnemonic", &canonical_charset_names[193] },
	{ 9, "csn369103", &canonical_charset_names[16] },
	{ 10, "csnatsdano", &canonical_charset_names[196] },
	{ 13, "csnatsdanoadd", &canonical_charset_names[197] },
	{ 10, "csnatssefi", &canonical_charset_names[198] },
	{ 13, "csnatssefiadd", &canonical_charset_names[199] },
	{ 13, "cspc775baltic", &canonical_charset_names[86] },
	{ 19, "cspc850multilingual", &canonical_charset_names[87] },
	{ 18, "cspc862latinhebrew", &canonical_charset_names[94] },
	{ 16, "cspc8codepage437", &canonical_charset_names[84] },
	{ 20, "cspc8danishnorwegian", &canonical_charset_names[208] },
	{ 12, "cspc8turkish", &canonical_charset_names[209] },
	{ 8, "cspcp852", &canonical_charset_names[89] },
	{ 9, "csptcp154", &canonical_charset_names[212] },
	{ 10, "csshiftjis", &canonical_charset_names[216] },
	{ 6, "csucs4", &canonical_charset_names[115] },
	{ 9, "csunicode", &canonical_charset_names[114] },
	{ 11, "csunicode11", &canonical_charset_names[221] },
	{ 15, "csunicode11utf7", &canonical_charset_names[222] },
	{ 14, "csunicodeascii", &canonical_charset_names[116] },
	{ 16, "csunicodeibm1261", &canonical_charset_names[150] },
	{ 16, "csunicodeibm1264", &canonical_charset_names[151] },
	{ 16, "csunicodeibm1265", &canonical_charset_names[152] },
	{ 16, "csunicodeibm1268", &canonical_charset_names[153] },
	{ 16, "csunicodeibm1276", &canonical_charset_names[154] },
	{ 15, "csunicodelatin1", &canonical_charset_names[118] },
	{ 13, "csunknown8bit", &canonical_charset_names[223] },
	{ 6, "csusdk", &canonical_charset_names[251] },
	{ 22, "csventurainternational", &canonical_charset_names[235] },
	{ 13, "csventuramath", &canonical_charset_names[236] },
	{ 11, "csventuraus", &canonical_charset_names[237] },
	{ 6, "csviqr", &canonical_charset_names[233] },
	{ 8, "csviscii", &canonical_charset_names[234] },
	{ 17, "cswindows30latin1", &canonical_charset_names[126] },
	{ 12, "cswindows31j", &canonical_charset_names[238] },
	{ 17, "cswindows31latin1", &canonical_charset_names[127] },
	{ 17, "cswindows31latin2", &canonical_charset_names[135] },
	{ 17, "cswindows31latin5", &canonical_charset_names[147] },
	{ 4, "cuba", &canonical_charset_names[200] },
	{ 8, "cyrillic", &canonical_charset_names[138] },
	{ 13, "cyrillicasian", &canonical_charset_names[212] },
	{ 2, "de", &canonical_charset_names[18] },
	{ 3, "dec", &canonical_charset_names[17] },
	{ 6, "decmcs", &canonical_charset_names[17] },
	{ 8, "din66003", &canonical_charset_names[18] },
	{ 2, "dk", &canonical_charset_names[19] },
	{ 4, "dkus", &canonical_charset_names[242] },
	{ 6, "ds2089", &canonical_charset_names[19] },
	{ 4, "e13b", &canonical_charset_names[156] },
	{ 10, "ebcdicatde", &canonical_charset_names[20] },
	{ 11, "ebcdicatdea", &canonical_charset_names[21] },
	{ 8, "ebcdicbe", &canonical_charset_names[71] },
	{ 8, "ebcdicbr", &canonical_charset_names[72] },
	{ 10, "ebcdiccafr", &canonical_charset_names[22] },
	{ 11, "ebcdiccpar1", &canonical_charset_names[81] },
	{ 11, "ebcdiccpar2", &canonical_charset_names[108] },
	{ 10, "ebcdiccpbe", &canonical_charset_names[85] },
	{ 10, "ebcdiccpca", &canonical_charset_names[66] },
	{ 10, "ebcdiccpch", &canonical_charset_names[85] },
	{ 10, "ebcdiccpdk", &canonical_charset_names[73] },
	{ 10, "ebcdiccpes", &canonical_charset_names[77] },
	{ 10, "ebcdiccpfi", &canonical_charset_names[74] },
	{ 10, "ebcdiccpfr", &canonical_charset_names[80] },
	{ 10, "ebcdiccpgb", &canonical_charset_names[78] },
	{ 10, "ebcdiccpgr", &canonical_charset_names[82] },
	{ 10, "ebcdiccphe", &canonical_charset_names[83] },
	{ 10, "ebcdiccpis", &canonical_charset_names[102] },
	{ 10, "ebcdiccpit", &canonical_charset_names[75] },
	{ 10, "ebcdiccpnl", &canonical_charset_names[66] },
	{ 10, "ebcdiccpno", &canonical_charset_names[73] },
	{ 13, "ebcdiccproece", &canonical_charset_names[101] },
	{ 10, "ebcdiccpse", &canonical_charset_names[74] },
	{ 10, "ebcdiccptr", &canonical_charset_names[107] },
	{ 10, "ebcdiccpus", &canonical_charset_names[66] },
	{ 10, "ebcdiccpwt", &canonical_charset_names[66] },
	{ 10, "ebcdiccpyu", &canonical_charset_names[101] },
	{ 14, "ebcdiccyrillic", &canonical_charset_names[103] },
	{ 15, "ebcdicde273euro", &canonical_charset_names[57] },
	{ 15, "ebcdicdk277euro", &canonical_charset_names[58] },
	{ 10, "ebcdicdkno", &canonical_charset_names[23] },
	{ 11, "ebcdicdknoa", &canonical_charset_names[24] },
	{ 8, "ebcdices", &canonical_charset_names[25] },
	{ 15, "ebcdices284euro", &canonical_charset_names[61] },
	{ 9, "ebcdicesa", &canonical_charset_names[26] },
	{ 9, "ebcdicess", &canonical_charset_names[27] },
	{ 15, "ebcdicfi278euro", &canonical_charset_names[59] },
	{ 10, "ebcdicfise", &canonical_charset_names[28] },
	{ 11, "ebcdicfisea", &canonical_charset_names[29] },
	{ 8, "ebcdicfr", &canonical_charset_names[30] },
	{ 15, "ebcdicfr297euro", &canonical_charset_names[63] },
	{ 15, "ebcdicgb285euro", &canonical_charset_names[62] },
	{ 9, "ebcdicint", &canonical_charset_names[67] },
	{ 26, "ebcdicinternational500euro", &canonical_charset_names[64] },
	{ 15, "ebcdicis871euro", &canonical_charset_names[65] },
	{ 8, "ebcdicit", &canonical_charset_names[31] },
	{ 15, "ebcdicit280euro", &canonical_charset_names[60] },
	{ 9, "ebcdicjpe", &canonical_charset_names[76] },
	{ 12, "ebcdicjpkana", &canonical_charset_names[79] },
	{ 16, "ebcdiclatin9euro", &canonical_charset_names[55] },
	{ 15, "ebcdicno277euro", &canonical_charset_names[58] },
	{ 8, "ebcdicpt", &canonical_charset_names[32] },
	{ 15, "ebcdicse278euro", &canonical_charset_names[59] },
	{ 8, "ebcdicuk", &canonical_charset_names[33] },
	{ 8, "ebcdicus", &canonical_charset_names[34] },
	{ 14, "ebcdicus37euro", &canonical_charset_names[56] },
	{ 7, "ecma114", &canonical_charset_names[139] },
	{ 7, "ecma118", &canonical_charset_names[142] },
	{ 12, "ecmacyrillic", &canonical_charset_names[35] },
	{ 7, "elot928", &canonical_charset_names[142] },
	{ 2, "es", &canonical_charset_names[36] },
	{ 3, "es2", &canonical_charset_names[37] },
	{ 5, "euccn", &canonical_charset_names[42] },
	{ 5, "eucjp", &canonical_charset_names[38] },
	{ 5, "euckr", &canonical_charset_names[39] },
	{ 37, "extendedunixcodefixedwidthforjapanese", &canonical_charset_names[40] },
	{ 39, "extendedunixcodepackedformatforjapanese", &canonical_charset_names[38] },
	{ 2, "fi", &canonical_charset_names[214] },
	{ 2, "fr", &canonical_charset_names[201] },
	{ 2, "gb", &canonical_charset_names[6] },
	{ 7, "gb18030", &canonical_charset_names[41] },
	{ 8, "gb198880", &canonical_charset_names[44] },
	{ 6, "gb2312", &canonical_charset_names[42] },
	{ 8, "gb231280", &canonical_charset_names[45] },
	{ 3, "gbk", &canonical_charset_names[43] },
	{ 11, "gost1976874", &canonical_charset_names[46] },
	{ 5, "greek", &canonical_charset_names[142] },
	{ 6, "greek7", &canonical_charset_names[244] },
	{ 9, "greek7old", &canonical_charset_names[245] },
	{ 6, "greek8", &canonical_charset_names[142] },
	{ 10, "greekccitt", &canonical_charset_names[243] },
	{ 6, "hebrew", &canonical_charset_names[143] },
	{ 9, "hpdesktop", &canonical_charset_names[47] },
	{ 7, "hplegal", &canonical_charset_names[48] },
	{ 7, "hpmath8", &canonical_charset_names[49] },
	{ 8, "hppifont", &canonical_charset_names[50] },
	{ 8, "hproman8", &canonical_charset_names[246] },
	{ 2, "hu", &canonical_charset_names[194] },
	{ 8, "hzgb2312", &canonical_charset_names[51] },
	{ 8, "ibm00858", &canonical_charset_names[54] },
	{ 8, "ibm00924", &canonical_charset_names[55] },
	{ 8, "ibm01140", &canonical_charset_names[56] },
	{ 8, "ibm01141", &canonical_charset_names[57] },
	{ 8, "ibm01142", &canonical_charset_names[58] },
	{ 8, "ibm01143", &canonical_charset_names[59] },
	{ 8, "ibm01144", &canonical_charset_names[60] },
	{ 8, "ibm01145", &canonical_charset_names[61] },
	{ 8, "ibm01146", &canonical_charset_names[62] },
	{ 8, "ibm01147", &canonical_charset_names[63] },
	{ 8, "ibm01148", &canonical_charset_names[64] },
	{ 8, "ibm01149", &canonical_charset_names[65] },
	{ 6, "ibm037", &canonical_charset_names[66] },
	{ 6, "ibm038", &canonical_charset_names[67] },
	{ 7, "ibm1026", &canonical_charset_names[68] },
	{ 7, "ibm1047", &canonical_charset_names[69] },
	{ 6, "ibm273", &canonical_charset_names[70] },
	{ 6, "ibm274", &canonical_charset_names[71] },
	{ 6, "ibm275", &canonical_charset_names[72] },
	{ 6, "ibm277", &canonical_charset_names[73] },
	{ 6, "ibm278", &canonical_charset_names[74] },
	{ 6, "ibm280", &canonical_charset_names[75] },
	{ 6, "ibm281", &canonical_charset_names[76] },
	{ 6, "ibm284", &canonical_charset_names[77] },
	{ 6, "ibm285", &canonical_charset_names[78] },
	{ 6, "ibm290", &canonical_charset_names[79] },
	{ 6, "ibm297", &canonical_charset_names[80] },
	{ 6, "ibm367", &canonical_charset_names[224] },
	{ 6, "ibm420", &canonical_charset_names[81] },
	{ 6, "ibm423", &canonical_charset_names[82] },
	{ 6, "ibm424", &canonical_charset_names[83] },
	{ 6, "ibm437", &canonical_charset_names[84] },
	{ 6, "ibm500", &canonical_charset_names[85] },
	{ 6, "ibm775", &canonical_charset_names[86] },
	{ 6, "ibm819", &canonical_charset_names[125] },
	{ 6, "ibm850", &canonical_charset_names[87] },
	{ 6, "ibm851", &canonical_charset_names[88] },
	{ 6, "ibm852", &canonical_charset_names[89] },
	{ 6, "ibm855", &canonical_charset_names[90] },
	{ 6, "ibm857", &canonical_charset_names[91] },
	{ 6, "ibm860", &canonical_charset_names[92] },
	{ 6, "ibm861", &canonical_charset_names[93] },
	{ 6, "ibm862", &canonical_charset_names[94] },
	{ 6, "ibm863", &canonical_charset_names[95] },
	{ 6, "ibm864", &canonical_charset_names[96] },
	{ 6, "ibm865", &canonical_charset_names[97] },
	{ 6, "ibm866", &canonical_charset_names[98] },
	{ 6, "ibm868", &canonical_charset_names[99] },
	{ 6, "ibm869", &canonical_charset_names[100] },
	{ 6, "ibm870", &canonical_charset_names[101] },
	{ 6, "ibm871", &canonical_charset_names[102] },
	{ 6, "ibm880", &canonical_charset_names[103] },
	{ 6, "ibm891", &canonical_charset_names[104] },
	{ 6, "ibm903", &canonical_charset_names[105] },
	{ 6, "ibm904", &canonical_charset_names[106] },
	{ 6, "ibm905", &canonical_charset_names[107] },
	{ 6, "ibm918", &canonical_charset_names[108] },
	{ 10, "ibmsymbols", &canonical_charset_names[52] },
	{ 7, "ibmthai", &canonical_charset_names[53] },
	{ 7, "iecp271", &canonical_charset_names[109] },
	{ 4, "inis", &canonical_charset_names[110] },
	{ 5, "inis8", &canonical_charset_names[111] },
	{ 12, "iniscyrillic", &canonical_charset_names[112] },
	{ 9, "invariant", &canonical_charset_names[113] },
	{ 3, "irv", &canonical_charset_names[161] },
	{ 11, "iso10367box", &canonical_charset_names[155] },
	{ 8, "iso10646", &canonical_charset_names[118] },
	{ 12, "iso10646ucs2", &canonical_charset_names[114] },
	{ 12, "iso10646ucs4", &canonical_charset_names[115] },
	{ 16, "iso10646ucsbasic", &canonical_charset_names[116] },
	{ 21, "iso10646unicodelatin1", &canonical_charset_names[118] },
	{ 12, "iso10646utf1", &canonical_charset_names[117] },
	{ 9, "iso2022cn", &canonical_charset_names[119] },
	{ 12, "iso2022cnext", &canonical_charset_names[120] },
	{ 9, "iso2022jp", &canonical_charset_names[121] },
	{ 10, "iso2022jp1", &canonical_charset_names[122] },
	{ 10, "iso2022jp2", &canonical_charset_names[123] },
	{ 9, "iso2022kr", &canonical_charset_names[124] },
	{ 11, "iso20331983", &canonical_charset_names[156] },
	{ 7, "iso5427", &canonical_charset_names[157] },
	{ 11, "iso54271981", &canonical_charset_names[158] },
	{ 19, "iso5427cyrillic1981", &canonical_charset_names[158] },
	{ 11, "iso54281980", &canonical_charset_names[159] },
	{ 15, "iso646basic1983", &canonical_charset_names[160] },
	{ 8, "iso646ca", &canonical_charset_names[13] },
	{ 9, "iso646ca2", &canonical_charset_names[14] },
	{ 8, "iso646cn", &canonical_charset_names[44] },
	{ 8, "iso646cu", &canonical_charset_names[200] },
	{ 8, "iso646de", &canonical_charset_names[18] },
	{ 8, "iso646dk", &canonical_charset_names[19] },
	{ 8, "iso646es", &canonical_charset_names[36] },
	{ 9, "iso646es2", &canonical_charset_names[37] },
	{ 8, "iso646fi", &canonical_charset_names[214] },
	{ 8, "iso646fr", &canonical_charset_names[201] },
	{ 9, "iso646fr1", &canonical_charset_names[202] },
	{ 8, "iso646gb", &canonical_charset_names[6] },
	{ 8, "iso646hu", &canonical_charset_names[194] },
	{ 13, "iso646irv1983", &canonical_charset_names[161] },
	{ 13, "iso646irv1991", &canonical_charset_names[224] },
	{ 8, "iso646it", &canonical_charset_names[165] },
	{ 8, "iso646jp", &canonical_charset_names[167] },
	{ 12, "iso646jpocrb", &canonical_charset_names[171] },
	{ 8, "iso646kr", &canonical_charset_names[186] },
	{ 8, "iso646no", &canonical_charset_names[203] },
	{ 9, "iso646no2", &canonical_charset_names[204] },
	{ 8, "iso646pt", &canonical_charset_names[210] },
	{ 9, "iso646pt2", &canonical_charset_names[211] },
	{ 8, "iso646se", &canonical_charset_names[214] },
	{ 9, "iso646se2", &canonical_charset_names[215] },
	{ 8, "iso646us", &canonical_charset_names[224] },
	{ 8, "iso646yu", &canonical_charset_names[180] },
	{ 10, "iso6937225", &canonical_charset_names[162] },
	{ 11, "iso69372add", &canonical_charset_names[163] },
	{ 8, "iso88591", &canonical_charset_names[125] },
	{ 9, "iso885910", &canonical_charset_names[128] },
	{ 13, "iso8859101992", &canonical_charset_names[128] },
	{ 9, "iso885911", &canonical_charset_names[129] },
	{ 12, "iso885911987", &canonical_charset_names[125] },
	{ 9, "iso885913", &canonical_charset_names[130] },
	{ 9, "iso885914", &canonical_charset_names[131] },
	{ 13, "iso8859141998", &canonical_charset_names[131] },
	{ 9, "iso885915", &canonical_charset_names[132] },
	{ 9, "iso885916", &canonical_charset_names[133] },
	{ 13, "iso8859162001", &canonical_charset_names[133] },
	{ 23, "iso88591windows30latin1", &canonical_charset_names[126] },
	{ 23, "iso88591windows31latin1", &canonical_charset_names[127] },
	{ 8, "iso88592", &canonical_charset_names[134] },
	{ 12, "iso885921987", &canonical_charset_names[134] },
	{ 21, "iso88592windowslatin2", &canonical_charset_names[135] },
	{ 8, "iso88593", &canonical_charset_names[136] },
	{ 12, "iso885931988", &canonical_charset_names[136] },
	{ 8, "iso88594", &canonical_charset_names[137] },
	{ 12, "iso885941988", &canonical_charset_names[137] },
	{ 8, "iso88595", &canonical_charset_names[138] },
	{ 12, "iso885951988", &canonical_charset_names[138] },
	{ 8, "iso88596", &canonical_charset_names[139] },
	{ 12, "iso885961987", &canonical_charset_names[139] },
	{ 9, "iso88596e", &canonical_charset_names[140] },
	{ 9, "iso88596i", &canonical_charset_names[141] },
	{ 8, "iso88597", &canonical_charset_names[142] },
	{ 12, "iso885971987", &canonical_charset_names[142] },
	{ 8, "iso88598", &canonical_charset_names[143] },
	{ 12, "iso885981988", &canonical_charset_names[143] },
	{ 9, "iso88598e", &canonical_charset_names[144] },
	{ 9, "iso88598i", &canonical_charset_names[145] },
	{ 8, "iso88599", &canonical_charset_names[146] },
	{ 12, "iso885991989", &canonical_charset_names[146] },
	{ 21, "iso88599windowslatin5", &canonical_charset_names[147] },
	{ 11, "iso8859supp", &canonical_charset_names[164] },
	{ 7, "iso9036", &canonical_charset_names[1] },
	{ 9, "isoceltic", &canonical_charset_names[131] },
	{ 7, "isoir10", &canonical_charset_names[214] },
	{ 8, "isoir100", &canonical_charset_names[125] },
	{ 8, "isoir101", &canonical_charset_names[134] },
	{ 8, "isoir102", &canonical_charset_names[218] },
	{ 8, "isoir103", &canonical_charset_names[219] },
	{ 8, "isoir109", &canonical_charset_names[136] },
	{ 7, "isoir11", &canonical_charset_names[215] },
	{ 8, "isoir110", &canonical_charset_names[137] },
	{ 8, "isoir111", &canonical_charset_names[35] },
	{ 8, "isoir121", &canonical_charset_names[13] },
	{ 8, "isoir122", &canonical_charset_names[14] },
	{ 8, "isoir123", &canonical_charset_names[15] },
	{ 8, "isoir126", &canonical_charset_names[142] },
	{ 8, "isoir127", &canonical_charset_names[139] },
	{ 8, "isoir128", &canonical_charset_names[217] },
	{ 7, "isoir13", &canonical_charset_names[166] },
	{ 8, "isoir138", &canonical_charset_names[143] },
	{ 8, "isoir139", &canonical_charset_names[16] },
	{ 7, "isoir14", &canonical_charset_names[167] },
	{ 8, "isoir141", &canonical_charset_names[180] },
	{ 8, "isoir142", &canonical_charset_names[163] },
	{ 8, "isoir143", &canonical_charset_names[109] },
	{ 8, "isoir144", &canonical_charset_names[138] },
	{ 8, "isoir146", &canonical_charset_names[182] },
	{ 8, "isoir147", &canonical_charset_names[181] },
	{ 8, "isoir148", &canonical_charset_names[146] },
	{ 8, "isoir149", &canonical_charset_names[187] },
	{ 7, "isoir15", &canonical_charset_names[165] },
	{ 8, "isoir150", &canonical_charset_names[243] },
	{ 8, "isoir151", &canonical_charset_names[200] },
	{ 8, "isoir152", &canonical_charset_names[162] },
	{ 8, "isoir153", &canonical_charset_names[46] },
	{ 8, "isoir154", &canonical_charset_names[164] },
	{ 8, "isoir155", &canonical_charset_names[155] },
	{ 8, "isoir157", &canonical_charset_names[128] },
	{ 8, "isoir158", &canonical_charset_names[249] },
	{ 8, "isoir159", &canonical_charset_names[178] },
	{ 7, "isoir16", &canonical_charset_names[210] },
	{ 8, "isoir166", &canonical_charset_names[129] },
	{ 7, "isoir17", &canonical_charset_names[36] },
	{ 7, "isoir18", &canonical_charset_names[245] },
	{ 8, "isoir182", &canonical_charset_names[148] },
	{ 7, "isoir19", &canonical_charset_names[248] },
	{ 8, "isoir197", &canonical_charset_names[149] },
	{ 8, "isoir199", &canonical_charset_names[131] },
	{ 6, "isoir2", &canonical_charset_names[161] },
	{ 7, "isoir21", &canonical_charset_names[18] },
	{ 8, "isoir226", &canonical_charset_names[133] },
	{ 7, "isoir25", &canonical_charset_names[202] },
	{ 7, "isoir27", &canonical_charset_names[188] },
	{ 7, "isoir37", &canonical_charset_names[157] },
	{ 6, "isoir4", &canonical_charset_names[6] },
	{ 7, "isoir42", &canonical_charset_names[168] },
	{ 7, "isoir47", &canonical_charset_names[7] },
	{ 7, "isoir49", &canonical_charset_names[110] },
	{ 7, "isoir50", &canonical_charset_names[111] },
	{ 7, "isoir51", &canonical_charset_names[112] },
	{ 7, "isoir54", &canonical_charset_names[158] },
	{ 7, "isoir55", &canonical_charset_names[159] },
	{ 7, "isoir57", &canonical_charset_names[44] },
	{ 7, "isoir58", &canonical_charset_names[45] },
	{ 6, "isoir6", &canonical_charset_names[224] },
	{ 7, "isoir60", &canonical_charset_names[203] },
	{ 7, "isoir61", &canonical_charset_names[204] },
	{ 7, "isoir69", &canonical_charset_names[201] },
	{ 7, "isoir70", &canonical_charset_names[252] },
	{ 7, "isoir81", &canonical_charset_names[198] },
	{ 7, "isoir82", &canonical_charset_names[199] },
	{ 7, "isoir84", &canonical_charset_names[211] },
	{ 7, "isoir85", &canonical_charset_names[37] },
	{ 7, "isoir86", &canonical_charset_names[194] },
	{ 7, "isoir87", &canonical_charset_names[169] },
	{ 7, "isoir88", &canonical_charset_names[244] },
	{ 7, "isoir89", &canonical_charset_names[1] },
	{ 7, "isoir90", &canonical_charset_names[247] },
	{ 7, "isoir91", &canonical_charset_names[196] },
	{ 7, "isoir92", &canonical_charset_names[197] },
	{ 7, "isoir93", &canonical_charset_names[172] },
	{ 7, "isoir94", &canonical_charset_names[173] },
	{ 7, "isoir95", &canonical_charset_names[174] },
	{ 7, "isoir96", &canonical_charset_names[175] },
	{ 7, "isoir98", &canonical_charset_names[156] },
	{ 7, "isoir99", &canonical_charset_names[0] },
	{ 17, "isounicodeibm1261", &canonical_charset_names[150] },
	{ 17, "isounicodeibm1264", &canonical_charset_names[151] },
	{ 17, "isounicodeibm1265", &canonical_charset_names[152] },
	{ 17, "isounicodeibm1268", &canonical_charset_names[153] },
	{ 17, "isounicodeibm1276", &canonical_charset_names[154] },
	{ 2, "it", &canonical_charset_names[165] },
	{ 12, "jisc62201969", &canonical_charset_names[166] },
	{ 14, "jisc62201969jp", &canonical_charset_names[166] },
	{ 14, "jisc62201969ro", &canonical_charset_names[167] },
	{ 12, "jisc62261978", &canonical_charset_names[168] },
	{ 12, "jisc62261983", &canonical_charset_names[169] },
	{ 13, "jisc62291984a", &canonical_charset_names[170] },
	{ 13, "jisc62291984b", &canonical_charset_names[171] },
	{ 16, "jisc62291984badd", &canonical_charset_names[172] },
	{ 16, "jisc62291984hand", &canonical_charset_names[173] },
	{ 19, "jisc62291984handadd", &canonical_charset_names[174] },
	{ 16, "jisc62291984kana", &canonical_charset_names[175] },
	{ 11, "jisencoding", &canonical_charset_names[176] },
	{ 8, "jisx0201", &canonical_charset_names[177] },
	{ 12, "jisx02081983", &canonical_charset_names[169] },
	{ 12, "jisx02121990", &canonical_charset_names[178] },
	{ 5, "johab", &canonical_charset_names[179] },
	{ 2, "jp", &canonical_charset_names[167] },
	{ 6, "jpocra", &canonical_charset_names[170] },
	{ 6, "jpocrb", &canonical_charset_names[171] },
	{ 9, "jpocrbadd", &canonical_charset_names[172] },
	{ 9, "jpocrhand", &canonical_charset_names[173] },
	{ 12, "jpocrhandadd", &canonical_charset_names[174] },
	{ 2, "js", &canonical_charset_names[180] },
	{ 9, "jusib1002", &canonical_charset_names[180] },
	{ 12, "jusib1003mac", &canonical_charset_names[181] },
	{ 13, "jusib1003serb", &canonical_charset_names[182] },
	{ 8, "katakana", &canonical_charset_names[166] },
	{ 12, "koi7switched", &canonical_charset_names[183] },
	{ 5, "koi8e", &canonical_charset_names[35] },
	{ 5, "koi8r", &canonical_charset_names[184] },
	{ 5, "koi8u", &canonical_charset_names[185] },
	{ 6, "korean", &canonical_charset_names[187] },
	{ 7, "ksc5601", &canonical_charset_names[187] },
	{ 11, "ksc56011987", &canonical_charset_names[187] },
	{ 11, "ksc56011989", &canonical_charset_names[187] },
	{ 7, "ksc5636", &canonical_charset_names[186] },
	{ 2, "l1", &canonical_charset_names[125] },
	{ 3, "l10", &canonical_charset_names[133] },
	{ 2, "l2", &canonical_charset_names[134] },
	{ 2, "l3", &canonical_charset_names[136] },
	{ 2, "l4", &canonical_charset_names[137] },
	{ 2, "l5", &canonical_charset_names[146] },
	{ 2, "l6", &canonical_charset_names[128] },
	{ 2, "l8", &canonical_charset_names[131] },
	{ 3, "lap", &canonical_charset_names[249] },
	{ 6, "latin1", &canonical_charset_names[125] },
	{ 7, "latin10", &canonical_charset_names[133] },
	{ 8, "latin125", &canonical_charset_names[164] },
	{ 6, "latin2", &canonical_charset_names[134] },
	{ 6, "latin3", &canonical_charset_names[136] },
	{ 6, "latin4", &canonical_charset_names[137] },
	{ 6, "latin5", &canonical_charset_names[146] },
	{ 6, "latin6", &canonical_charset_names[128] },
	{ 6, "latin8", &canonical_charset_names[131] },
	{ 6, "latin9", &canonical_charset_names[132] },
	{ 10, "latingreek", &canonical_charset_names[248] },
	{ 11, "latingreek1", &canonical_charset_names[188] },
	{ 8, "latinlap", &canonical_charset_names[249] },
	{ 3, "mac", &canonical_charset_names[250] },
	{ 16, "maccentraleurope", &canonical_charset_names[189] },
	{ 18, "maccentraleurroman", &canonical_charset_names[189] },
	{ 11, "maccyrillic", &canonical_charset_names[190] },
	{ 10, "macedonian", &canonical_charset_names[181] },
	{ 9, "macintosh", &canonical_charset_names[250] },
	{ 8, "macroman", &canonical_charset_names[250] },
	{ 10, "macukraine", &canonical_charset_names[191] },
	{ 12, "macukrainian", &canonical_charset_names[191] },
	{ 19, "microsoftpublishing", &canonical_charset_names[195] },
	{ 4, "mnem", &canonical_charset_names[192] },
	{ 8, "mnemonic", &canonical_charset_names[193] },
	{ 5, "ms936", &canonical_charset_names[43] },
	{ 6, "msansi", &canonical_charset_names[255] },
	{ 6, "msarab", &canonical_charset_names[259] },
	{ 6, "mscyrl", &canonical_charset_names[254] },
	{ 4, "msee", &canonical_charset_names[253] },
	{ 7, "msgreek", &canonical_charset_names[256] },
	{ 7, "mskanji", &canonical_charset_names[216] },
	{ 6, "msturk", &canonical_charset_names[257] },
	{ 8, "msz77953", &canonical_charset_names[194] },
	{ 6, "naplps", &canonical_charset_names[0] },
	{ 8, "natsdano", &canonical_charset_names[196] },
	{ 11, "natsdanoadd", &canonical_charset_names[197] },
	{ 8, "natssefi", &canonical_charset_names[198] },
	{ 11, "natssefiadd", &canonical_charset_names[199] },
	{ 10, "ncnc001081", &canonical_charset_names[200] },
	{ 8, "nfz62010", &canonical_charset_names[201] },
	{ 12, "nfz620101973", &canonical_charset_names[202] },
	{ 2, "no", &canonical_charset_names[203] },
	{ 3, "no2", &canonical_charset_names[204] },
	{ 7, "ns45511", &canonical_charset_names[203] },
	{ 7, "ns45512", &canonical_charset_names[204] },
	{ 16, "osdebcdicdf03irv", &canonical_charset_names[205] },
	{ 14, "osdebcdicdf041", &canonical_charset_names[206] },
	{ 15, "osdebcdicdf0415", &canonical_charset_names[207] },
	{ 18, "pc8danishnorwegian", &canonical_charset_names[208] },
	{ 10, "pc8turkish", &canonical_charset_names[209] },
	{ 21, "pcmultilingual850euro", &canonical_charset_names[54] },
	{ 2, "pt", &canonical_charset_names[210] },
	{ 5, "pt154", &canonical_charset_names[212] },
	{ 3, "pt2", &canonical_charset_names[211] },
	{ 7, "ptcp154", &canonical_charset_names[212] },
	{ 2, "r8", &canonical_charset_names[246] },
	{ 3, "ref", &canonical_charset_names[160] },
	{ 6, "roman8", &canonical_charset_names[246] },
	{ 4, "scsu", &canonical_charset_names[213] },
	{ 2, "se", &canonical_charset_names[214] },
	{ 3, "se2", &canonical_charset_names[215] },
	{ 10, "sen850200b", &canonical_charset_names[214] },
	{ 10, "sen850200c", &canonical_charset_names[215] },
	{ 7, "serbian", &canonical_charset_names[182] },
	{ 8, "shiftjis", &canonical_charset_names[216] },
	{ 10, "stsev35888", &canonical_charset_names[46] },
	{ 6, "t101g2", &canonical_charset_names[217] },
	{ 3, "t61", &canonical_charset_names[219] },
	{ 7, "t617bit", &canonical_charset_names[218] },
	{ 7, "t618bit", &canonical_charset_names[219] },
	{ 6, "tis620", &canonical_charset_names[220] },
	{ 4, "ucs2", &canonical_charset_names[114] },
	{ 4, "ucs4", &canonical_charset_names[115] },
	{ 2, "uk", &canonical_charset_names[6] },
	{ 9, "unicode11", &canonical_charset_names[221] },
	{ 13, "unicode11utf7", &canonical_charset_names[222] },
	{ 13, "unicode11utf8", &canonical_charset_names[232] },
	{ 13, "unicode20utf8", &canonical_charset_names[232] },
	{ 11, "unknown8bit", &canonical_charset_names[223] },
	{ 2, "us", &canonical_charset_names[224] },
	{ 7, "usascii", &canonical_charset_names[224] },
	{ 4, "usdk", &canonical_charset_names[251] },
	{ 5, "utf16", &canonical_charset_names[225] },
	{ 7, "utf16be", &canonical_charset_names[226] },
	{ 7, "utf16le", &canonical_charset_names[227] },
	{ 5, "utf32", &canonical_charset_names[228] },
	{ 7, "utf32be", &canonical_charset_names[229] },
	{ 7, "utf32le", &canonical_charset_names[230] },
	{ 4, "utf7", &canonical_charset_names[231] },
	{ 4, "utf8", &canonical_charset_names[232] },
	{ 20, "venturainternational", &canonical_charset_names[235] },
	{ 11, "venturamath", &canonical_charset_names[236] },
	{ 9, "venturaus", &canonical_charset_names[237] },
	{ 13, "videotexsuppl", &canonical_charset_names[252] },
	{ 4, "viqr", &canonical_charset_names[233] },
	{ 6, "viscii", &canonical_charset_names[234] },
	{ 10, "winbaltrim", &canonical_charset_names[260] },
	{ 11, "windows1250", &canonical_charset_names[253] },
	{ 11, "windows1251", &canonical_charset_names[254] },
	{ 11, "windows1252", &canonical_charset_names[255] },
	{ 11, "windows1253", &canonical_charset_names[256] },
	{ 11, "windows1254", &canonical_charset_names[257] },
	{ 11, "windows1255", &canonical_charset_names[258] },
	{ 11, "windows1256", &canonical_charset_names[259] },
	{ 11, "windows1257", &canonical_charset_names[260] },
	{ 11, "windows1258", &canonical_charset_names[261] },
	{ 10, "windows31j", &canonical_charset_names[238] },
	{ 10, "windows874", &canonical_charset_names[11] },
	{ 10, "windows936", &canonical_charset_names[43] },
	{ 10, "windows949", &canonical_charset_names[12] },
	{ 5, "x0201", &canonical_charset_names[177] },
	{ 6, "x02017", &canonical_charset_names[166] },
	{ 5, "x0208", &canonical_charset_names[169] },
	{ 5, "x0212", &canonical_charset_names[178] },
	{ 11, "xacornfuzzy", &canonical_charset_names[239] },
	{ 12, "xacornlatin1", &canonical_charset_names[240] },
	{ 8, "xcurrent", &canonical_charset_names[241] },
	{ 19, "xmaccentraleurroman", &canonical_charset_names[189] },
	{ 12, "xmaccyrillic", &canonical_charset_names[190] },
	{ 9, "xmacroman", &canonical_charset_names[250] },
	{ 13, "xmacukrainian", &canonical_charset_names[191] },
	{ 5, "xsjis", &canonical_charset_names[216] },
	{ 7, "xsystem", &canonical_charset_names[241] },
	{ 6, "xxbig5", &canonical_charset_names[8] },
	{ 2, "yu", &canonical_charset_names[180] },
};

static const uint16_t charset_aliases_count = 852;

#define MIBENUM_IS_UNICODE(x) (((x) == 1000) || ((x) == 1001) || ((x) == 1015) || ((x) == 1013) || ((x) == 1014) || ((x) == 1017) || ((x) == 1018) || ((x) == 1019) || ((x) == 106))

/* End aliases tables */

typedef struct {
        size_t slen;
        const char *s;
} lengthed_string;


#define IS_PUNCT_OR_SPACE(x)                    \
        (!(((x) >= 'A' && (x) <= 'Z') ||        \
           ((x) >= 'a' && (x) <= 'z') ||        \
           ((x) >= '0' && (x) <= '9')))


static int parserutils_charset_alias_match(const void *a, const void *b)
{
        lengthed_string *s = (lengthed_string *)a;
        parserutils_charset_aliases_alias *alias = (parserutils_charset_aliases_alias*)b;
        size_t key_left = s->slen;
        size_t alias_left = alias->name_len;
        const char *s_alias = alias->name;
        const char *s_key = s->s;
        int cmpret;
        
        while ((key_left > 0) && (alias_left > 0)) {
                while ((key_left > 0) && IS_PUNCT_OR_SPACE(*s_key)) {
                        key_left--; s_key++;
                }
                
                if (key_left == 0)
                        break;
                
                cmpret = tolower(*s_key) - *s_alias;
                
                if (cmpret != 0) {
                        return cmpret;
                }
                
                key_left--;
                s_key++;
                alias_left--;
                s_alias++;
        }
        
        while ((key_left > 0) && IS_PUNCT_OR_SPACE(*s_key)) {
          key_left--; s_key++;
        }
        
        return key_left - alias_left;
}

/**
 * Retrieve the canonical form of an alias name
 *
 * \param alias  The alias name
 * \param len    The length of the alias name
 * \return Pointer to canonical form or NULL if not found
 */
parserutils_charset_aliases_canon *parserutils__charset_alias_canonicalise(
		const char *alias, size_t len)
{
        parserutils_charset_aliases_alias *c;
        lengthed_string s;
        
        s.slen = len;
        s.s = alias;

        c = (parserutils_charset_aliases_alias*)bsearch(&s,
		&charset_aliases[0],
		charset_aliases_count,
		sizeof(parserutils_charset_aliases_alias),
		parserutils_charset_alias_match);
        
        if (c == NULL)
                return NULL;
        
        return c->canon;
}

/**
 * Retrieve the MIB enum value assigned to an encoding name
 *
 * \param alias  The alias to lookup
 * \param len    The length of the alias string
 * \return The MIB enum value, or 0 if not found
 */
uint16_t parserutils_charset_mibenum_from_name(const char *alias, size_t len)
{
	parserutils_charset_aliases_canon *c;

	if (alias == NULL)
		return 0;

	c = parserutils__charset_alias_canonicalise(alias, len);
	if (c == NULL)
		return 0;

	return c->mib_enum;
}

/**
 * Retrieve the canonical name of an encoding from the MIB enum
 *
 * \param mibenum The MIB enum value
 * \return Pointer to canonical name, or NULL if not found
 */
const char *parserutils_charset_mibenum_to_name(uint16_t mibenum)
{
	int i;
	parserutils_charset_aliases_canon *c;
        
        for (i = 0; i < charset_aliases_canon_count; ++i) {
                c = &canonical_charset_names[i];
                if (c->mib_enum == mibenum)
                        return c->name;
        }
        
        return NULL;
}

/**
 * Detect if a parserutils_charset is Unicode
 *
 * \param mibenum  The MIB enum to consider
 * \return true if a Unicode variant, false otherwise
 */
bool parserutils_charset_mibenum_is_unicode(uint16_t mibenum)
{
        return MIBENUM_IS_UNICODE(mibenum);
}



extern const parserutils_charset_handler charset_ascii_codec_handler;
extern const parserutils_charset_handler charset_8859_codec_handler;
extern const parserutils_charset_handler charset_ext8_codec_handler;
extern const parserutils_charset_handler charset_utf8_codec_handler;
extern const parserutils_charset_handler charset_utf16_codec_handler;

static const parserutils_charset_handler *handler_table[] = {
	&charset_utf8_codec_handler,
	&charset_utf16_codec_handler,
	&charset_8859_codec_handler,
	&charset_ext8_codec_handler,
	&charset_ascii_codec_handler,
	NULL,
};

/**
 * Create a charset codec
 *
 * \param charset  Target charset
 * \param codec    Pointer to location to receive codec instance
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhaustion,
 *         PARSERUTILS_BADENCODING on unsupported charset
 */
parserutils_error parserutils_charset_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	parserutils_charset_codec *c;
	const parserutils_charset_handler **handler;
	const parserutils_charset_aliases_canon * canon;
	parserutils_error error;

	if (charset == NULL || codec == NULL)
		return PARSERUTILS_BADPARM;

	/* Canonicalise parserutils_charset name. */
	canon = parserutils__charset_alias_canonicalise(charset,
			strlen(charset));
	if (canon == NULL)
		return PARSERUTILS_BADENCODING;

	/* Search for handler class */
	for (handler = handler_table; *handler != NULL; handler++) {
		if ((*handler)->handles_charset(canon->name))
			break;
	}

	/* None found */
	if ((*handler) == NULL)
		return PARSERUTILS_BADENCODING;

	/* Instantiate class */
	error = (*handler)->create(canon->name, &c);
	if (error != PARSERUTILS_OK)
		return error;

	/* and initialise it */
	c->mibenum = canon->mib_enum;

	c->errormode = PARSERUTILS_CHARSET_CODEC_ERROR_LOOSE;

	*codec = c;

	return PARSERUTILS_OK;
}

/**
 * Destroy a charset codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_codec_destroy(
		parserutils_charset_codec *codec)
{
	if (codec == NULL)
		return PARSERUTILS_BADPARM;

	codec->handler.destroy(codec);

	free(codec);

	return PARSERUTILS_OK;
}

/**
 * Configure a charset codec
 *
 * \param codec   The codec to configure
 * \param type    The codec option type to configure
 * \param params  Option-specific parameters
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_codec_setopt(
		parserutils_charset_codec *codec,
		parserutils_charset_codec_opttype type,
		parserutils_charset_codec_optparams *params)
{
	if (codec == NULL || params == NULL)
		return PARSERUTILS_BADPARM;

	switch (type) {
	case PARSERUTILS_CHARSET_CODEC_ERROR_MODE:
		codec->errormode = params->error_mode.mode;
		break;
	}

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 data into a codec's charset
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK on success, appropriate error otherwise.
 *
 * source, sourcelen, dest and destlen will be updated appropriately on exit
 */
parserutils_error parserutils_charset_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	if (codec == NULL || source == NULL || *source == NULL ||
			sourcelen == NULL || dest == NULL || *dest == NULL ||
			destlen == NULL)
		return PARSERUTILS_BADPARM;

	return codec->handler.encode_charset(codec, source, sourcelen, dest, destlen);
}

/**
 * Decode a chunk of data in a codec's charset into UCS-4
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK on success, appropriate error otherwise.
 *
 * source, sourcelen, dest and destlen will be updated appropriately on exit
 *
 * Call this with a source length of 0 to flush any buffers.
 */
parserutils_error parserutils_charset_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	if (codec == NULL || source == NULL || *source == NULL ||
			sourcelen == NULL || dest == NULL || *dest == NULL ||
			destlen == NULL)
		return PARSERUTILS_BADPARM;

	return codec->handler.decode_charset(codec, source, sourcelen, dest, destlen);
}

/**
 * Clear a charset codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_codec_reset(
		parserutils_charset_codec *codec)
{
	if (codec == NULL)
		return PARSERUTILS_BADPARM;

	return codec->handler.reset(codec);
}



/** \file
 * UTF-8 manipulation functions (implementation).
 */


/** Number of continuation bytes for a given start byte */
const uint8_t numContinuations[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
};

/**
 * Convert a UTF-8 multibyte sequence into a single UCS-4 character
 *
 * Encoding of UCS values outside the UTF-16 plane has been removed from
 * RFC3629. This function conforms to RFC2279, however.
 *
 * \param s     The sequence to process
 * \param len   Length of sequence
 * \param ucs4  Pointer to location to receive UCS-4 character (host endian)
 * \param clen  Pointer to location to receive byte length of UTF-8 sequence
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_to_ucs4(const uint8_t *s, size_t len,
		uint32_t *ucs4, size_t *clen)
{
	parserutils_error error;

	UTF8_TO_UCS4(s, len, ucs4, clen, error);

	return error;
}

/**
 * Convert a single UCS-4 character into a UTF-8 multibyte sequence
 *
 * Encoding of UCS values outside the UTF-16 plane has been removed from
 * RFC3629. This function conforms to RFC2279, however.
 *
 * \param ucs4  The character to process (0 <= c <= 0x7FFFFFFF) (host endian)
 * \param s     Pointer to pointer to output buffer, updated on exit
 * \param len   Pointer to length, in bytes, of output buffer, updated on exit
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_from_ucs4(uint32_t ucs4,
		uint8_t **s, size_t *len)
{
	parserutils_error error;

	UTF8_FROM_UCS4(ucs4, s, len, error);

	return error;
}

/**
 * Calculate the length (in characters) of a bounded UTF-8 string
 *
 * \param s    The string
 * \param max  Maximum length
 * \param len  Pointer to location to receive length of string
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_length(const uint8_t *s, size_t max,
		size_t *len)
{
	parserutils_error error;

	UTF8_LENGTH(s, max, len, error);

	return error;
}

/**
 * Calculate the length (in bytes) of a UTF-8 character
 *
 * \param s    Pointer to start of character
 * \param len  Pointer to location to receive length
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_char_byte_length(const uint8_t *s,
		size_t *len)
{
	parserutils_error error;

	UTF8_CHAR_BYTE_LENGTH(s, len, error);

	return error;
}

/**
 * Find previous legal UTF-8 char in string
 *
 * \param s        The string
 * \param off      Offset in the string to start at
 * \param prevoff  Pointer to location to receive offset of first byte of
 *                 previous legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_prev(const uint8_t *s, uint32_t off,
		uint32_t *prevoff)
{
	parserutils_error error;

	UTF8_PREV(s, off, prevoff, error);

	return error;
}

/**
 * Find next legal UTF-8 char in string
 *
 * \param s        The string (assumed valid)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_next(const uint8_t *s, uint32_t len,
		uint32_t off, uint32_t *nextoff)
{
	parserutils_error error;

	UTF8_NEXT(s, len, off, nextoff, error);

	return error;
}

/**
 * Find next legal UTF-8 char in string
 *
 * \param s        The string (assumed to be of dubious validity)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf8_next_paranoid(const uint8_t *s,
		uint32_t len, uint32_t off, uint32_t *nextoff)
{
	parserutils_error error;

	UTF8_NEXT_PARANOID(s, len, off, nextoff, error);

	return error;
}



/** \file
 * UTF-16 manipulation functions (implementation).
 */


/**
 * Convert a UTF-16 sequence into a single UCS-4 character
 *
 * \param s     The sequence to process
 * \param len   Length of sequence in bytes
 * \param ucs4  Pointer to location to receive UCS-4 character (host endian)
 * \param clen  Pointer to location to receive byte length of UTF-16 sequence
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_to_ucs4(const uint8_t *s,
		size_t len, uint32_t *ucs4, size_t *clen)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;

	if (s == NULL || ucs4 == NULL || clen == NULL)
		return PARSERUTILS_BADPARM;

	if (len < 2)
		return PARSERUTILS_NEEDDATA;

	if (*ss < 0xD800 || *ss > 0xDFFF) {
		*ucs4 = *ss;
		*clen = 2;
	} else if (0xD800 <= *ss && *ss <= 0xDBFF) {
		/* High-surrogate code unit.  */
		if (len < 4)
			return PARSERUTILS_NEEDDATA;

		if (0xDC00 <= ss[1] && ss[1] <= 0xDFFF) {
			/* We have a valid surrogate pair.  */
			*ucs4 = (((ss[0] & 0x3FF) << 10) | (ss[1] & 0x3FF))
				+ (1<<16);
			*clen = 4;
		} else {
			return PARSERUTILS_INVALID;
		}
	} else {
		/* Low-surrogate code unit.  */
		return PARSERUTILS_INVALID;
	}

	return PARSERUTILS_OK;
}

/**
 * Convert a single UCS-4 character into a UTF-16 sequence
 *
 * \param ucs4  The character to process (0 <= c <= 0x7FFFFFFF) (host endian)
 * \param s     Pointer to 4 byte long output buffer
 * \param len   Pointer to location to receive length of multibyte sequence
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_from_ucs4(uint32_t ucs4, uint8_t *s,
		size_t *len)
{
	uint16_t *ss = (uint16_t *) (void *) s;
	uint32_t l = 0;

	if (s == NULL || len == NULL)
		return PARSERUTILS_BADPARM;
	else if (ucs4 < 0x10000) {
		*ss = (uint16_t) ucs4;
		l = 2;
	} else if (ucs4 < 0x110000) {
		ss[0] = 0xD800 | (((ucs4 >> 16) & 0x1f) - 1) | (ucs4 >> 10);
		ss[1] = 0xDC00 | (ucs4 & 0x3ff);
		l = 4;
	} else {
		return PARSERUTILS_INVALID;
	}

	*len = l;

	return PARSERUTILS_OK;
}

/**
 * Calculate the length (in characters) of a bounded UTF-16 string
 *
 * \param s    The string
 * \param max  Maximum length
 * \param len  Pointer to location to receive length of string
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_length(const uint8_t *s, size_t max,
		size_t *len)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;
	const uint16_t *end = (const uint16_t *) (const void *) (s + max);
	int l = 0;

	if (s == NULL || len == NULL)
		return PARSERUTILS_BADPARM;

	while (ss < end) {
		if (*ss < 0xD800 || 0xDFFF < *ss)
			ss++;
		else
			ss += 2;

		l++;
	}

	*len = l;

	return PARSERUTILS_OK;
}

/**
 * Calculate the length (in bytes) of a UTF-16 character
 *
 * \param s    Pointer to start of character
 * \param len  Pointer to location to receive length
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_char_byte_length(const uint8_t *s,
		size_t *len)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;

	if (s == NULL || len == NULL)
		return PARSERUTILS_BADPARM;

	if (*ss < 0xD800 || 0xDFFF < *ss)
		*len = 2;
	else
		*len = 4;

	return PARSERUTILS_OK;
}

/**
 * Find previous legal UTF-16 char in string
 *
 * \param s        The string
 * \param off      Offset in the string to start at
 * \param prevoff  Pointer to location to receive offset of first byte of
 *                 previous legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_prev(const uint8_t *s, uint32_t off,
		uint32_t *prevoff)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;

	if (s == NULL || prevoff == NULL)
		return PARSERUTILS_BADPARM;

	if (off < 2)
		*prevoff = 0;
	else if (ss[-1] < 0xDC00 || ss[-1] > 0xDFFF)
		*prevoff = off - 2;
	else
		*prevoff = (off < 4) ? 0 : off - 4;

	return PARSERUTILS_OK;
}

/**
 * Find next legal UTF-16 char in string
 *
 * \param s        The string (assumed valid)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_next(const uint8_t *s, uint32_t len,
		uint32_t off, uint32_t *nextoff)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;

	if (s == NULL || off >= len || nextoff == NULL)
		return PARSERUTILS_BADPARM;

	if (len - off < 4)
		*nextoff = len;
	else if (ss[1] < 0xD800 || ss[1] > 0xDBFF)
		*nextoff = off + 2;
	else
		*nextoff = (len - off < 6) ? len : off + 4;

	return PARSERUTILS_OK;
}

/**
 * Find next legal UTF-16 char in string
 *
 * \param s        The string (assumed to be of dubious validity)
 * \param len      Maximum offset in string
 * \param off      Offset in the string to start at
 * \param nextoff  Pointer to location to receive offset of first byte of
 *                 next legal character
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error parserutils_charset_utf16_next_paranoid(const uint8_t *s,
		uint32_t len, uint32_t off, uint32_t *nextoff)
{
	const uint16_t *ss = (const uint16_t *) (const void *) s;

	if (s == NULL || off >= len || nextoff == NULL)
		return PARSERUTILS_BADPARM;

	while (1) {
		if (len - off < 4) {
			return PARSERUTILS_NEEDDATA;
		} else if (ss[1] < 0xD800 || ss[1] > 0xDFFF) {
			*nextoff = off + 2;
			break;
		} else if (ss[1] >= 0xD800 && ss[1] <= 0xDBFF) {
			if (len - off < 6)
				return PARSERUTILS_NEEDDATA;

			if (ss[2] >= 0xDC00 && ss[2] <= 0xDFFF) {
				*nextoff = off + 4;
				break;
			} else {
				ss++;
				off += 2;
			}
		}
	}

	return PARSERUTILS_OK;
}


static struct {
	uint16_t mib;
	const char *name;
	size_t len;
	uint32_t *table;
} known_iso_charsets[] = {
	{ 0, "ISO-8859-1", SLEN("ISO-8859-1"), t1 },
	{ 0, "ISO-8859-2", SLEN("ISO-8859-2"), t2 },
	{ 0, "ISO-8859-3", SLEN("ISO-8859-3"), t3 },
	{ 0, "ISO-8859-4", SLEN("ISO-8859-4"), t4 },
	{ 0, "ISO-8859-5", SLEN("ISO-8859-5"), t5 },
	{ 0, "ISO-8859-6", SLEN("ISO-8859-6"), t6 },
	{ 0, "ISO-8859-7", SLEN("ISO-8859-7"), t7 },
	{ 0, "ISO-8859-8", SLEN("ISO-8859-8"), t8 },
	{ 0, "ISO-8859-9", SLEN("ISO-8859-9"), t9 },
	{ 0, "ISO-8859-10", SLEN("ISO-8859-10"), t10 },
	{ 0, "ISO-8859-11", SLEN("ISO-8859-11"), t11 },
	{ 0, "ISO-8859-13", SLEN("ISO-8859-13"), t13 },
	{ 0, "ISO-8859-14", SLEN("ISO-8859-14"), t14 },
	{ 0, "ISO-8859-15", SLEN("ISO-8859-15"), t15 },
	{ 0, "ISO-8859-16", SLEN("ISO-8859-16"), t16 }
};

/**
 * ISO-8859-n charset codec
 */
typedef struct charset_8859_codec {
	parserutils_charset_codec base;	/**< Base class */

	uint32_t *table;		/**< Mapping table for 0xA0-0xFF */

#define READ_BUFSIZE (8)
	uint32_t read_buf[READ_BUFSIZE];	/**< Buffer for partial
						 * output sequences (decode)
						 * (host-endian) */
	size_t read_len;		/**< Character length of read_buf */

#define WRITE_BUFSIZE (8)
	uint32_t write_buf[WRITE_BUFSIZE];	/**< Buffer for partial
						 * output sequences (encode)
						 * (host-endian) */
	size_t write_len;		/**< Character length of write_buf */

} charset_8859_codec;

static bool charset_8859_codec_handles_charset(const char *charset);
static parserutils_error charset_8859_codec_create(const char *charset,
		parserutils_charset_codec **codec);
static parserutils_error charset_8859_codec_destroy(
		parserutils_charset_codec *codec);
static parserutils_error charset_8859_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_8859_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_8859_codec_reset(
		parserutils_charset_codec *codec);
static inline parserutils_error charset_8859_codec_read_char(
		charset_8859_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_8859_codec_output_decoded_char(
		charset_8859_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_8859_from_ucs4(charset_8859_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len);
static inline parserutils_error charset_8859_to_ucs4(charset_8859_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4);

/**
 * Determine whether this codec handles a specific charset
 *
 * \param charset  Charset to test
 * \return true if handleable, false otherwise
 */
bool charset_8859_codec_handles_charset(const char *charset)
{
	uint32_t i;
	uint16_t match = parserutils_charset_mibenum_from_name(charset,
			strlen(charset));

	if (known_iso_charsets[0].mib == 0) {
		for (i = 0; i < N_ELEMENTS(known_iso_charsets); i++) {
			known_iso_charsets[i].mib =
				parserutils_charset_mibenum_from_name(
						known_iso_charsets[i].name,
						known_iso_charsets[i].len);
		}
	}

	for (i = 0; i < N_ELEMENTS(known_iso_charsets); i++) {
		if (known_iso_charsets[i].mib == match)
			return true;
	}

	return false;
}

/**
 * Create an ISO-8859-n codec
 *
 * \param charset  The charset to read from / write to
 * \param codec    Pointer to location to receive codec
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhausion
 */
parserutils_error charset_8859_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	uint32_t i;
	charset_8859_codec *c;
	uint16_t match = parserutils_charset_mibenum_from_name(
			charset, strlen(charset));
	uint32_t *table = NULL;

	for (i = 0; i < N_ELEMENTS(known_iso_charsets); i++) {
		if (known_iso_charsets[i].mib == match) {
			table = known_iso_charsets[i].table;
			break;
		}
	}

	assert(table != NULL);

	c = (charset_8859_codec *)malloc(sizeof(charset_8859_codec));
	if (c == NULL)
		return PARSERUTILS_NOMEM;

	c->table = table;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	/* Finally, populate vtable */
	c->base.handler.destroy = charset_8859_codec_destroy;
	c->base.handler.encode_charset = charset_8859_codec_encode;
	c->base.handler.decode_charset = charset_8859_codec_decode;
	c->base.handler.reset = charset_8859_codec_reset;

	*codec = (parserutils_charset_codec *) c;

	return PARSERUTILS_OK;
}

/**
 * Destroy an ISO-8859-n codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_8859_codec_destroy (parserutils_charset_codec *codec)
{
	UNUSED(codec);

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 (big endian) data into ISO-8859-n
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read. Any remaining output for the character will be buffered by the
 * codec for writing on the next call.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_8859_codec_encode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_8859_codec *c = (charset_8859_codec *) codec;
	uint32_t ucs4;
	uint32_t *towrite;
	size_t towritelen;
	parserutils_error error;

	/* Process any outstanding characters from the previous call */
	if (c->write_len > 0) {
		uint32_t *pwrite = c->write_buf;

		while (c->write_len > 0) {
			error = charset_8859_from_ucs4(c, pwrite[0],
					dest, destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				assert(error == PARSERUTILS_NOMEM);

				for (len = 0; len < c->write_len; len++) {
					c->write_buf[len] = pwrite[len];
				}

				return error;
			}

			pwrite++;
			c->write_len--;
		}
	}

	/* Now process the characters for this call */
	while (*sourcelen > 0) {
		ucs4 = endian_big_to_host(*((uint32_t *) (void *) *source));
		towrite = &ucs4;
		towritelen = 1;

		/* Output current characters */
		while (towritelen > 0) {
			error = charset_8859_from_ucs4(c, towrite[0], dest,
					destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				if (error != PARSERUTILS_NOMEM) {
					return error;
				}

				/* Insufficient output space */
				assert(towritelen < WRITE_BUFSIZE);

				c->write_len = towritelen;

				/* Copy pending chars to save area, for
				 * processing next call. */
				for (len = 0; len < towritelen; len++)
					c->write_buf[len] = towrite[len];

				/* Claim character we've just buffered,
				 * so it's not reprocessed */
				*source += 4;
				*sourcelen -= 4;

				return PARSERUTILS_NOMEM;
			}

			towrite++;
			towritelen--;
		}

		*source += 4;
		*sourcelen -= 4;
	}

	return PARSERUTILS_OK;
}

/**
 * Decode a chunk of ISO-8859-n data into UCS-4 (big endian)
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * If STRICT error handling is configured and an illegal sequence is split
 * over two calls, then _INVALID will be returned from the second call,
 * but ::source will point mid-way through the invalid sequence (i.e. it
 * will be unmodified over the second call). In addition, the internal
 * incomplete-sequence buffer will be emptied, such that subsequent calls
 * will progress, rather than re-evaluating the same invalid sequence.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 *
 * Call this with a source length of 0 to flush the output buffer.
 */
parserutils_error charset_8859_codec_decode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_8859_codec *c = (charset_8859_codec *) codec;
	parserutils_error error;

	if (c->read_len > 0) {
		/* Output left over from last decode */
		uint32_t *pread = c->read_buf;

		while (c->read_len > 0 && *destlen >= c->read_len * 4) {
			*((uint32_t *) (void *) *dest) =
					endian_host_to_big(pread[0]);

			*dest += 4;
			*destlen -= 4;

			pread++;
			c->read_len--;
		}

		if (*destlen < c->read_len * 4) {
			/* Ran out of output buffer */
			size_t i;

			/* Shuffle remaining output down */
			for (i = 0; i < c->read_len; i++)
				c->read_buf[i] = pread[i];

			return PARSERUTILS_NOMEM;
		}
	}

	/* Finally, the "normal" case; process all outstanding characters */
	while (*sourcelen > 0) {
		error = charset_8859_codec_read_char(c,
				source, sourcelen, dest, destlen);
		if (error != PARSERUTILS_OK) {
			return error;
		}
	}

	return PARSERUTILS_OK;
}

/**
 * Clear an ISO-8859-n codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_8859_codec_reset(parserutils_charset_codec *codec)
{
	charset_8859_codec *c = (charset_8859_codec *) codec;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	return PARSERUTILS_OK;
}


/**
 * Read a character from the ISO-8859-n to UCS-4 (big endian)
 *
 * \param c          The codec
 * \param source     Pointer to pointer to source buffer (updated on exit)
 * \param sourcelen  Pointer to length of source buffer (updated on exit)
 * \param dest       Pointer to pointer to output buffer (updated on exit)
 * \param destlen    Pointer to length of output buffer (updated on exit)
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_8859_codec_read_char(charset_8859_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	uint32_t ucs4;
	parserutils_error error;

	/* Convert a single character */
	error = charset_8859_to_ucs4(c, *source, *sourcelen, &ucs4);
	if (error == PARSERUTILS_OK) {
		/* Read a character */
		error = charset_8859_codec_output_decoded_char(c,
				ucs4, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	} else if (error == PARSERUTILS_NEEDDATA) {
		/* Can only happen if sourcelen == 0 */
		return error;
	} else if (error == PARSERUTILS_INVALID) {
		/* Illegal input sequence */

		/* Strict errormode; simply flag invalid character */
		if (c->base.errormode ==
				PARSERUTILS_CHARSET_CODEC_ERROR_STRICT) {
			return PARSERUTILS_INVALID;
		}

		/* output U+FFFD and continue processing. */
		error = charset_8859_codec_output_decoded_char(c,
				0xFFFD, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	}

	return PARSERUTILS_OK;
}

/**
 * Output a UCS-4 character (big endian)
 *
 * \param c        Codec to use
 * \param ucs4     UCS-4 character (host endian)
 * \param dest     Pointer to pointer to output buffer
 * \param destlen  Pointer to output buffer length
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 */
parserutils_error charset_8859_codec_output_decoded_char(charset_8859_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen)
{
	if (*destlen < 4) {
		/* Run out of output buffer */
		c->read_len = 1;
		c->read_buf[0] = ucs4;

		return PARSERUTILS_NOMEM;
	}

	*((uint32_t *) (void *) *dest) = endian_host_to_big(ucs4);
	*dest += 4;
	*destlen -= 4;

	return PARSERUTILS_OK;
}

/**
 * Convert a UCS4 (host endian) character to ISO-8859-n
 *
 * \param c     The codec instance
 * \param ucs4  The UCS4 character to convert
 * \param s     Pointer to pointer to destination buffer
 * \param len   Pointer to destination buffer length
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM if there's insufficient space in the output buffer,
 *         PARSERUTILS_INVALID if the character cannot be represented
 *
 * _INVALID will only be returned if the codec's conversion mode is STRICT.
 * Otherwise, '?' will be output.
 *
 * On successful conversion, *s and *len will be updated.
 */
parserutils_error charset_8859_from_ucs4(charset_8859_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len)
{
	uint8_t out = 0;

	if (*len < 1)
		return PARSERUTILS_NOMEM;

	if (ucs4 < 0x80) {
		/* ASCII */
		out = ucs4;
	} else {
		uint32_t i;

		for (i = 0; i < 96; i++) {
			if (ucs4 == c->table[i])
				break;
		}

		if (i == 96) {
			if (c->base.errormode ==
					PARSERUTILS_CHARSET_CODEC_ERROR_STRICT)
				return PARSERUTILS_INVALID;
			else
				out = '?';
		} else {
			out = 0xA0 + i;
		}
	}

	*(*s) = out;
	(*s)++;
	(*len)--;

	return PARSERUTILS_OK;
}

/**
 * Convert an ISO-8859-n character to UCS4 (host endian)
 *
 * \param c     The codec instance
 * \param s     Pointer to source buffer
 * \param len   Source buffer length
 * \param ucs4  Pointer to destination buffer
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NEEDDATA if there's insufficient input data
 *         PARSERUTILS_INVALID if the character cannot be represented
 */
parserutils_error charset_8859_to_ucs4(charset_8859_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4)
{
	uint32_t out;

	if (len < 1)
		return PARSERUTILS_NEEDDATA;

	if (*s < 0x80) {
		out = *s;
	} else if (*s >= 0xA0) {
		if (c->table[*s - 0xA0] == 0xFFFF)
			return PARSERUTILS_INVALID;

		out = c->table[*s - 0xA0];
	} else {
		return PARSERUTILS_INVALID;
	}

	*ucs4 = out;

	return PARSERUTILS_OK;
}

const parserutils_charset_handler charset_8859_codec_handler = {
	charset_8859_codec_handles_charset,
	charset_8859_codec_create
};


/**
 * US-ASCII charset codec
 */
typedef struct charset_ascii_codec {
	parserutils_charset_codec base;	/**< Base class */

#define READ_BUFSIZE (8)
	uint32_t read_buf[READ_BUFSIZE];	/**< Buffer for partial
						 * output sequences (decode)
						 * (host-endian) */
	size_t read_len;		/**< Character length of read_buf */

#define WRITE_BUFSIZE (8)
	uint32_t write_buf[WRITE_BUFSIZE];	/**< Buffer for partial
						 * output sequences (encode)
						 * (host-endian) */
	size_t write_len;		/**< Character length of write_buf */

} charset_ascii_codec;

static bool charset_ascii_codec_handles_charset(const char *charset);
static parserutils_error charset_ascii_codec_create(
		const char *charset, parserutils_charset_codec **codec);
static parserutils_error charset_ascii_codec_destroy(
		parserutils_charset_codec *codec);
static parserutils_error charset_ascii_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_ascii_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_ascii_codec_reset(
		parserutils_charset_codec *codec);
static inline parserutils_error charset_ascii_codec_read_char(
		charset_ascii_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_ascii_codec_output_decoded_char(
		charset_ascii_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_ascii_from_ucs4(charset_ascii_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len);
static inline parserutils_error charset_ascii_to_ucs4(charset_ascii_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4);

/**
 * Determine whether this codec handles a specific charset
 *
 * \param charset  Charset to test
 * \return true if handleable, false otherwise
 */
bool charset_ascii_codec_handles_charset(const char *charset)
{
	static uint16_t ascii;
	uint16_t match = parserutils_charset_mibenum_from_name(charset,
			strlen(charset));

	if (ascii == 0) {
		ascii = parserutils_charset_mibenum_from_name(
				"US-ASCII", SLEN("US-ASCII"));
	}

	if (ascii != 0 && ascii == match)
		return true;

	return false;
}

/**
 * Create a US-ASCII codec
 *
 * \param charset  The charset to read from / write to
 * \param codec    Pointer to location to receive codec
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhausion
 */
parserutils_error charset_ascii_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	charset_ascii_codec *c;

	UNUSED(charset);

	c = (charset_ascii_codec *)malloc(sizeof(charset_ascii_codec));
	if (c == NULL)
		return PARSERUTILS_NOMEM;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	/* Finally, populate vtable */
	c->base.handler.destroy = charset_ascii_codec_destroy;
	c->base.handler.encode_charset = charset_ascii_codec_encode;
	c->base.handler.decode_charset = charset_ascii_codec_decode;
	c->base.handler.reset = charset_ascii_codec_reset;

	*codec = (parserutils_charset_codec *) c;

	return PARSERUTILS_OK;
}

/**
 * Destroy a US-ASCII codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_ascii_codec_destroy (parserutils_charset_codec *codec)
{
	UNUSED(codec);

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 (big endian) data into US-ASCII
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read. Any remaining output for the character will be buffered by the
 * codec for writing on the next call.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_ascii_codec_encode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_ascii_codec *c = (charset_ascii_codec *) codec;
	uint32_t ucs4;
	uint32_t *towrite;
	size_t towritelen;
	parserutils_error error;

	/* Process any outstanding characters from the previous call */
	if (c->write_len > 0) {
		uint32_t *pwrite = c->write_buf;

		while (c->write_len > 0) {
			error = charset_ascii_from_ucs4(c, pwrite[0],
					dest, destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				assert(error == PARSERUTILS_NOMEM);

				for (len = 0; len < c->write_len; len++) {
					c->write_buf[len] = pwrite[len];
				}

				return error;
			}

			pwrite++;
			c->write_len--;
		}
	}

	/* Now process the characters for this call */
	while (*sourcelen > 0) {
		ucs4 = endian_big_to_host(*((uint32_t *) (void *) *source));
		towrite = &ucs4;
		towritelen = 1;

		/* Output current characters */
		while (towritelen > 0) {
			error = charset_ascii_from_ucs4(c, towrite[0], dest,
					destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				if (error != PARSERUTILS_NOMEM) {
					return error;
				}

				/* Insufficient output space */
				assert(towritelen < WRITE_BUFSIZE);

				c->write_len = towritelen;

				/* Copy pending chars to save area, for
				 * processing next call. */
				for (len = 0; len < towritelen; len++)
					c->write_buf[len] = towrite[len];

				/* Claim character we've just buffered,
				 * so it's not reprocessed */
				*source += 4;
				*sourcelen -= 4;

				return PARSERUTILS_NOMEM;
			}

			towrite++;
			towritelen--;
		}

		*source += 4;
		*sourcelen -= 4;
	}

	return PARSERUTILS_OK;
}

/**
 * Decode a chunk of US-ASCII data into UCS-4 (big endian)
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * If STRICT error handling is configured and an illegal sequence is split
 * over two calls, then _INVALID will be returned from the second call,
 * but ::source will point mid-way through the invalid sequence (i.e. it
 * will be unmodified over the second call). In addition, the internal
 * incomplete-sequence buffer will be emptied, such that subsequent calls
 * will progress, rather than re-evaluating the same invalid sequence.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 *
 * Call this with a source length of 0 to flush the output buffer.
 */
parserutils_error charset_ascii_codec_decode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_ascii_codec *c = (charset_ascii_codec *) codec;
	parserutils_error error;

	if (c->read_len > 0) {
		/* Output left over from last decode */
		uint32_t *pread = c->read_buf;

		while (c->read_len > 0 && *destlen >= c->read_len * 4) {
			*((uint32_t *) (void *) *dest) =
					endian_host_to_big(pread[0]);

			*dest += 4;
			*destlen -= 4;

			pread++;
			c->read_len--;
		}

		if (*destlen < c->read_len * 4) {
			/* Ran out of output buffer */
			size_t i;

			/* Shuffle remaining output down */
			for (i = 0; i < c->read_len; i++)
				c->read_buf[i] = pread[i];

			return PARSERUTILS_NOMEM;
		}
	}

	/* Finally, the "normal" case; process all outstanding characters */
	while (*sourcelen > 0) {
		error = charset_ascii_codec_read_char(c,
				source, sourcelen, dest, destlen);
		if (error != PARSERUTILS_OK) {
			return error;
		}
	}

	return PARSERUTILS_OK;
}

/**
 * Clear a US-ASCII codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_ascii_codec_reset(parserutils_charset_codec *codec)
{
	charset_ascii_codec *c = (charset_ascii_codec *) codec;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	return PARSERUTILS_OK;
}


/**
 * Read a character from US-ASCII to UCS-4 (big endian)
 *
 * \param c          The codec
 * \param source     Pointer to pointer to source buffer (updated on exit)
 * \param sourcelen  Pointer to length of source buffer (updated on exit)
 * \param dest       Pointer to pointer to output buffer (updated on exit)
 * \param destlen    Pointer to length of output buffer (updated on exit)
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_ascii_codec_read_char(charset_ascii_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	uint32_t ucs4;
	parserutils_error error;

	/* Convert a single character */
	error = charset_ascii_to_ucs4(c, *source, *sourcelen, &ucs4);
	if (error == PARSERUTILS_OK) {
		/* Read a character */
		error = charset_ascii_codec_output_decoded_char(c,
				ucs4, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	} else if (error == PARSERUTILS_NEEDDATA) {
		/* Can only happen if sourcelen == 0 */
		return error;
	} else if (error == PARSERUTILS_INVALID) {
		/* Illegal input sequence */

		/* Strict errormode; simply flag invalid character */
		if (c->base.errormode ==
				PARSERUTILS_CHARSET_CODEC_ERROR_STRICT) {
			return PARSERUTILS_INVALID;
		}

		/* output U+FFFD and continue processing. */
		error = charset_ascii_codec_output_decoded_char(c,
				0xFFFD, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	}

	return PARSERUTILS_OK;
}

/**
 * Output a UCS-4 character (big endian)
 *
 * \param c        Codec to use
 * \param ucs4     UCS-4 character (host endian)
 * \param dest     Pointer to pointer to output buffer
 * \param destlen  Pointer to output buffer length
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 */
parserutils_error charset_ascii_codec_output_decoded_char(
		charset_ascii_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen)
{
	if (*destlen < 4) {
		/* Run out of output buffer */
		c->read_len = 1;
		c->read_buf[0] = ucs4;

		return PARSERUTILS_NOMEM;
	}

	*((uint32_t *) (void *) *dest) = endian_host_to_big(ucs4);
	*dest += 4;
	*destlen -= 4;

	return PARSERUTILS_OK;
}

/**
 * Convert a UCS4 (host endian) character to US-ASCII
 *
 * \param c     The codec instance
 * \param ucs4  The UCS4 character to convert
 * \param s     Pointer to pointer to destination buffer
 * \param len   Pointer to destination buffer length
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM if there's insufficient space in the output buffer,
 *         PARSERUTILS_INVALID if the character cannot be represented
 *
 * _INVALID will only be returned if the codec's conversion mode is STRICT.
 * Otherwise, '?' will be output.
 *
 * On successful conversion, *s and *len will be updated.
 */
parserutils_error charset_ascii_from_ucs4(charset_ascii_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len)
{
	uint8_t out = 0;

	if (*len < 1)
		return PARSERUTILS_NOMEM;

	if (ucs4 < 0x80) {
		/* ASCII */
		out = ucs4;
	} else {
		if (c->base.errormode == PARSERUTILS_CHARSET_CODEC_ERROR_STRICT)
			return PARSERUTILS_INVALID;
		else
			out = '?';
	}

	*(*s) = out;
	(*s)++;
	(*len)--;

	return PARSERUTILS_OK;
}

/**
 * Convert a US-ASCII character to UCS4 (host endian)
 *
 * \param c     The codec instance
 * \param s     Pointer to source buffer
 * \param len   Source buffer length
 * \param ucs4  Pointer to destination buffer
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NEEDDATA if there's insufficient input data
 *         PARSERUTILS_INVALID if the character cannot be represented
 */
parserutils_error charset_ascii_to_ucs4(charset_ascii_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4)
{
	uint32_t out;

	UNUSED(c);

	if (len < 1)
		return PARSERUTILS_NEEDDATA;

	if (*s < 0x80) {
		out = *s;
	} else {
		return PARSERUTILS_INVALID;
	}

	*ucs4 = out;

	return PARSERUTILS_OK;
}

const parserutils_charset_handler charset_ascii_codec_handler = {
	charset_ascii_codec_handles_charset,
	charset_ascii_codec_create
};


static struct {
	uint16_t mib;
	const char *name;
	size_t len;
	uint32_t *table;
} known_win_charsets[] = {
	{ 0, "Windows-1250", SLEN("Windows-1250"), w1250 },
	{ 0, "Windows-1251", SLEN("Windows-1251"), w1251 },
	{ 0, "Windows-1252", SLEN("Windows-1252"), w1252 },
	{ 0, "Windows-1253", SLEN("Windows-1253"), w1253 },
	{ 0, "Windows-1254", SLEN("Windows-1254"), w1254 },
	{ 0, "Windows-1255", SLEN("Windows-1255"), w1255 },
	{ 0, "Windows-1256", SLEN("Windows-1256"), w1256 },
	{ 0, "Windows-1257", SLEN("Windows-1257"), w1257 },
	{ 0, "Windows-1258", SLEN("Windows-1258"), w1258 },
};

/**
 * Windows charset codec
 */
typedef struct charset_ext8_codec {
	parserutils_charset_codec base;	/**< Base class */

	uint32_t *table;		/**< Mapping table for 0x80-0xFF */

#define READ_BUFSIZE (8)
	uint32_t read_buf[READ_BUFSIZE];	/**< Buffer for partial
						 * output sequences (decode)
						 * (host-endian) */
	size_t read_len;		/**< Character length of read_buf */

#define WRITE_BUFSIZE (8)
	uint32_t write_buf[WRITE_BUFSIZE];	/**< Buffer for partial
						 * output sequences (encode)
						 * (host-endian) */
	size_t write_len;		/**< Character length of write_buf */

} charset_ext8_codec;

static bool charset_ext8_codec_handles_charset(const char *charset);
static parserutils_error charset_ext8_codec_create(const char *charset,
		parserutils_charset_codec **codec);
static parserutils_error charset_ext8_codec_destroy(
		parserutils_charset_codec *codec);
static parserutils_error charset_ext8_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_ext8_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_ext8_codec_reset(
		parserutils_charset_codec *codec);
static inline parserutils_error charset_ext8_codec_read_char(
		charset_ext8_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_ext8_codec_output_decoded_char(
		charset_ext8_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_ext8_from_ucs4(charset_ext8_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len);
static inline parserutils_error charset_ext8_to_ucs4(charset_ext8_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4);

/**
 * Determine whether this codec handles a specific charset
 *
 * \param charset  Charset to test
 * \return true if handleable, false otherwise
 */
bool charset_ext8_codec_handles_charset(const char *charset)
{
	uint32_t i;
	uint16_t match = parserutils_charset_mibenum_from_name(charset,
			strlen(charset));

	if (known_win_charsets[0].mib == 0) {
		for (i = 0; i < N_ELEMENTS(known_win_charsets); i++) {
			known_win_charsets[i].mib =
				parserutils_charset_mibenum_from_name(
						known_win_charsets[i].name,
						known_win_charsets[i].len);
		}
	}

	for (i = 0; i < N_ELEMENTS(known_win_charsets); i++) {
		if (known_win_charsets[i].mib == match)
			return true;
	}

	return false;
}

/**
 * Create an extended 8bit codec
 *
 * \param charset  The charset to read from / write to
 * \param codec    Pointer to location to receive codec
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhausion
 */
parserutils_error charset_ext8_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	uint32_t i;
	charset_ext8_codec *c;
	uint16_t match = parserutils_charset_mibenum_from_name(
			charset, strlen(charset));
	uint32_t *table = NULL;

	for (i = 0; i < N_ELEMENTS(known_win_charsets); i++) {
		if (known_win_charsets[i].mib == match) {
			table = known_win_charsets[i].table;
			break;
		}
	}

	assert(table != NULL);

	c = (charset_ext8_codec *)malloc(sizeof(charset_ext8_codec));
	if (c == NULL)
		return PARSERUTILS_NOMEM;

	c->table = table;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	/* Finally, populate vtable */
	c->base.handler.destroy = charset_ext8_codec_destroy;
	c->base.handler.encode_charset = charset_ext8_codec_encode;
	c->base.handler.decode_charset = charset_ext8_codec_decode;
	c->base.handler.reset = charset_ext8_codec_reset;

	*codec = (parserutils_charset_codec *) c;

	return PARSERUTILS_OK;
}

/**
 * Destroy an extended 8bit codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_ext8_codec_destroy (parserutils_charset_codec *codec)
{
	UNUSED(codec);

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 (big endian) data into extended 8bit
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read. Any remaining output for the character will be buffered by the
 * codec for writing on the next call.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_ext8_codec_encode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_ext8_codec *c = (charset_ext8_codec *) codec;
	uint32_t ucs4;
	uint32_t *towrite;
	size_t towritelen;
	parserutils_error error;

	/* Process any outstanding characters from the previous call */
	if (c->write_len > 0) {
		uint32_t *pwrite = c->write_buf;

		while (c->write_len > 0) {
			error = charset_ext8_from_ucs4(c, pwrite[0],
					dest, destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				assert(error == PARSERUTILS_NOMEM);

				for (len = 0; len < c->write_len; len++) {
					c->write_buf[len] = pwrite[len];
				}

				return error;
			}

			pwrite++;
			c->write_len--;
		}
	}

	/* Now process the characters for this call */
	while (*sourcelen > 0) {
		ucs4 = endian_big_to_host(*((uint32_t *) (void *) *source));
		towrite = &ucs4;
		towritelen = 1;

		/* Output current characters */
		while (towritelen > 0) {
			error = charset_ext8_from_ucs4(c, towrite[0], dest,
					destlen);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				if (error != PARSERUTILS_NOMEM) {
					return error;
				}

				/* Insufficient output space */
				assert(towritelen < WRITE_BUFSIZE);

				c->write_len = towritelen;

				/* Copy pending chars to save area, for
				 * processing next call. */
				for (len = 0; len < towritelen; len++)
					c->write_buf[len] = towrite[len];

				/* Claim character we've just buffered,
				 * so it's not reprocessed */
				*source += 4;
				*sourcelen -= 4;

				return PARSERUTILS_NOMEM;
			}

			towrite++;
			towritelen--;
		}

		*source += 4;
		*sourcelen -= 4;
	}

	return PARSERUTILS_OK;
}

/**
 * Decode a chunk of extended 8bit data into UCS-4 (big endian)
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * If STRICT error handling is configured and an illegal sequence is split
 * over two calls, then _INVALID will be returned from the second call,
 * but ::source will point mid-way through the invalid sequence (i.e. it
 * will be unmodified over the second call). In addition, the internal
 * incomplete-sequence buffer will be emptied, such that subsequent calls
 * will progress, rather than re-evaluating the same invalid sequence.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 *
 * Call this with a source length of 0 to flush the output buffer.
 */
parserutils_error charset_ext8_codec_decode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_ext8_codec *c = (charset_ext8_codec *) codec;
	parserutils_error error;

	if (c->read_len > 0) {
		/* Output left over from last decode */
		uint32_t *pread = c->read_buf;

		while (c->read_len > 0 && *destlen >= c->read_len * 4) {
			*((uint32_t *) (void *) *dest) =
					endian_host_to_big(pread[0]);

			*dest += 4;
			*destlen -= 4;

			pread++;
			c->read_len--;
		}

		if (*destlen < c->read_len * 4) {
			/* Ran out of output buffer */
			size_t i;

			/* Shuffle remaining output down */
			for (i = 0; i < c->read_len; i++)
				c->read_buf[i] = pread[i];

			return PARSERUTILS_NOMEM;
		}
	}

	/* Finally, the "normal" case; process all outstanding characters */
	while (*sourcelen > 0) {
		error = charset_ext8_codec_read_char(c,
				source, sourcelen, dest, destlen);
		if (error != PARSERUTILS_OK) {
			return error;
		}
	}

	return PARSERUTILS_OK;
}

/**
 * Clear an extended 8bit codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_ext8_codec_reset(parserutils_charset_codec *codec)
{
	charset_ext8_codec *c = (charset_ext8_codec *) codec;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	return PARSERUTILS_OK;
}


/**
 * Read a character from the extended 8bit to UCS-4 (big endian)
 *
 * \param c          The codec
 * \param source     Pointer to pointer to source buffer (updated on exit)
 * \param sourcelen  Pointer to length of source buffer (updated on exit)
 * \param dest       Pointer to pointer to output buffer (updated on exit)
 * \param destlen    Pointer to length of output buffer (updated on exit)
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                                 codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_ext8_codec_read_char(charset_ext8_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	uint32_t ucs4;
	parserutils_error error;

	/* Convert a single character */
	error = charset_ext8_to_ucs4(c, *source, *sourcelen, &ucs4);
	if (error == PARSERUTILS_OK) {
		/* Read a character */
		error = charset_ext8_codec_output_decoded_char(c,
				ucs4, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	} else if (error == PARSERUTILS_NEEDDATA) {
		/* Can only happen if sourcelen == 0 */
		return error;
	} else if (error == PARSERUTILS_INVALID) {
		/* Illegal input sequence */

		/* Strict errormode; simply flag invalid character */
		if (c->base.errormode ==
				PARSERUTILS_CHARSET_CODEC_ERROR_STRICT) {
			return PARSERUTILS_INVALID;
		}

		/* output U+FFFD and continue processing. */
		error = charset_ext8_codec_output_decoded_char(c,
				0xFFFD, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += 1;
			*sourcelen -= 1;
		}

		return error;
	}

	return PARSERUTILS_OK;
}

/**
 * Output a UCS-4 character (big endian)
 *
 * \param c        Codec to use
 * \param ucs4     UCS-4 character (host endian)
 * \param dest     Pointer to pointer to output buffer
 * \param destlen  Pointer to output buffer length
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 */
parserutils_error charset_ext8_codec_output_decoded_char(charset_ext8_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen)
{
	if (*destlen < 4) {
		/* Run out of output buffer */
		c->read_len = 1;
		c->read_buf[0] = ucs4;

		return PARSERUTILS_NOMEM;
	}

	*((uint32_t *) (void *) *dest) = endian_host_to_big(ucs4);
	*dest += 4;
	*destlen -= 4;

	return PARSERUTILS_OK;
}

/**
 * Convert a UCS4 (host endian) character to extended 8bit
 *
 * \param c     The codec instance
 * \param ucs4  The UCS4 character to convert
 * \param s     Pointer to pointer to destination buffer
 * \param len   Pointer to destination buffer length
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM if there's insufficient space in the output buffer,
 *         PARSERUTILS_INVALID if the character cannot be represented
 *
 * _INVALID will only be returned if the codec's conversion mode is STRICT.
 * Otherwise, '?' will be output.
 *
 * On successful conversion, *s and *len will be updated.
 */
parserutils_error charset_ext8_from_ucs4(charset_ext8_codec *c,
		uint32_t ucs4, uint8_t **s, size_t *len)
{
	uint8_t out = 0;

	if (*len < 1)
		return PARSERUTILS_NOMEM;

	if (ucs4 < 0x80) {
		/* ASCII */
		out = ucs4;
	} else {
		uint32_t i;

		for (i = 0; i < 128; i++) {
			if (ucs4 == c->table[i])
				break;
		}

		if (i == 128) {
			if (c->base.errormode ==
					PARSERUTILS_CHARSET_CODEC_ERROR_STRICT)
				return PARSERUTILS_INVALID;
			else
				out = '?';
		} else {
			out = 0x80 + i;
		}
	}

	*(*s) = out;
	(*s)++;
	(*len)--;

	return PARSERUTILS_OK;
}

/**
 * Convert an extended 8bit character to UCS4 (host endian)
 *
 * \param c     The codec instance
 * \param s     Pointer to source buffer
 * \param len   Source buffer length
 * \param ucs4  Pointer to destination buffer
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NEEDDATA if there's insufficient input data
 *         PARSERUTILS_INVALID if the character cannot be represented
 */
parserutils_error charset_ext8_to_ucs4(charset_ext8_codec *c,
		const uint8_t *s, size_t len, uint32_t *ucs4)
{
	uint32_t out;

	if (len < 1)
		return PARSERUTILS_NEEDDATA;

	if (*s < 0x80) {
		out = *s;
	} else {
		if (c->table[*s - 0x80] == 0xFFFF)
			return PARSERUTILS_INVALID;

		out = c->table[*s - 0x80];
	}

	*ucs4 = out;

	return PARSERUTILS_OK;
}

const parserutils_charset_handler charset_ext8_codec_handler = {
	charset_ext8_codec_handles_charset,
	charset_ext8_codec_create
};


/**
 * UTF-8 charset codec
 */
typedef struct charset_utf8_codec {
	parserutils_charset_codec base;	/**< Base class */

#define INVAL_BUFSIZE (32)
	uint8_t inval_buf[INVAL_BUFSIZE];	/**< Buffer for fixing up
						 * incomplete input
						 * sequences */
	size_t inval_len;		/*< Byte length of inval_buf **/

#define READ_BUFSIZE (8)
	uint32_t read_buf[READ_BUFSIZE];	/**< Buffer for partial
						 * output sequences (decode)
						 * (host-endian) */
	size_t read_len;		/**< Character length of read_buf */

#define WRITE_BUFSIZE (8)
	uint32_t write_buf[WRITE_BUFSIZE];	/**< Buffer for partial
						 * output sequences (encode)
						 * (host-endian) */
	size_t write_len;		/**< Character length of write_buf */

} charset_utf8_codec;

static bool charset_utf8_codec_handles_charset(const char *charset);
static parserutils_error charset_utf8_codec_create(const char *charset,
		parserutils_charset_codec **codec);
static parserutils_error charset_utf8_codec_destroy(
		parserutils_charset_codec *codec);
static parserutils_error charset_utf8_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_utf8_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_utf8_codec_reset(
		parserutils_charset_codec *codec);
static inline parserutils_error charset_utf8_codec_read_char(
		charset_utf8_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_utf8_codec_output_decoded_char(
		charset_utf8_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen);

/**
 * Determine whether this codec handles a specific charset
 *
 * \param charset  Charset to test
 * \return true if handleable, false otherwise
 */
bool charset_utf8_codec_handles_charset(const char *charset)
{
	return parserutils_charset_mibenum_from_name(charset,
				strlen(charset)) ==
			parserutils_charset_mibenum_from_name("UTF-8",
				SLEN("UTF-8"));
}

/**
 * Create a UTF-8 codec
 *
 * \param charset  The charset to read from / write to
 * \param codec    Pointer to location to receive codec
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhausion
 */
parserutils_error charset_utf8_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	charset_utf8_codec *c;

	UNUSED(charset);

	c = (charset_utf8_codec *)malloc(sizeof(charset_utf8_codec));
	if (c == NULL)
		return PARSERUTILS_NOMEM;

	c->inval_buf[0] = '\0';
	c->inval_len = 0;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	/* Finally, populate vtable */
	c->base.handler.destroy = charset_utf8_codec_destroy;
	c->base.handler.encode_charset = charset_utf8_codec_encode;
	c->base.handler.decode_charset = charset_utf8_codec_decode;
	c->base.handler.reset = charset_utf8_codec_reset;

	*codec = (parserutils_charset_codec *) c;

	return PARSERUTILS_OK;
}

/**
 * Destroy a UTF-8 codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_utf8_codec_destroy (parserutils_charset_codec *codec)
{
	UNUSED(codec);

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 (big endian) data into UTF-8
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read. Any remaining output for the character will be buffered by the
 * codec for writing on the next call.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_utf8_codec_encode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_utf8_codec *c = (charset_utf8_codec *) codec;
	uint32_t ucs4;
	uint32_t *towrite;
	size_t towritelen;
	parserutils_error error;

	/* Process any outstanding characters from the previous call */
	if (c->write_len > 0) {
		uint32_t *pwrite = c->write_buf;

		while (c->write_len > 0) {
			UTF8_FROM_UCS4(pwrite[0], dest, destlen, error);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				assert(error == PARSERUTILS_NOMEM);

				/* Insufficient output buffer space */
				for (len = 0; len < c->write_len; len++) {
					c->write_buf[len] = pwrite[len];
				}

				return PARSERUTILS_NOMEM;
			}

			pwrite++;
			c->write_len--;
		}
	}

	/* Now process the characters for this call */
	while (*sourcelen > 0) {
		ucs4 = endian_big_to_host(*((uint32_t *) (void *) *source));
		towrite = &ucs4;
		towritelen = 1;

		/* Output current characters */
		while (towritelen > 0) {
			UTF8_FROM_UCS4(towrite[0], dest, destlen, error);
			if (error != PARSERUTILS_OK) {
				uint32_t len;
				assert(error == PARSERUTILS_NOMEM);

				/* Insufficient output space */
				assert(towritelen < WRITE_BUFSIZE);

				c->write_len = towritelen;

				/* Copy pending chars to save area, for
				 * processing next call. */
				for (len = 0; len < towritelen; len++)
					c->write_buf[len] = towrite[len];

				/* Claim character we've just buffered,
				 * so it's not reprocessed */
				*source += 4;
				*sourcelen -= 4;

				return PARSERUTILS_NOMEM;
			}

			towrite++;
			towritelen--;
		}

		*source += 4;
		*sourcelen -= 4;
	}

	return PARSERUTILS_OK;
}

/**
 * Decode a chunk of UTF-8 data into UCS-4 (big endian)
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * If STRICT error handling is configured and an illegal sequence is split
 * over two calls, then _INVALID will be returned from the second call,
 * but ::source will point mid-way through the invalid sequence (i.e. it
 * will be unmodified over the second call). In addition, the internal
 * incomplete-sequence buffer will be emptied, such that subsequent calls
 * will progress, rather than re-evaluating the same invalid sequence.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 *
 * Call this with a source length of 0 to flush the output buffer.
 */
parserutils_error charset_utf8_codec_decode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_utf8_codec *c = (charset_utf8_codec *) codec;
	parserutils_error error;

	if (c->read_len > 0) {
		/* Output left over from last decode */
		uint32_t *pread = c->read_buf;

		while (c->read_len > 0 && *destlen >= c->read_len * 4) {
			*((uint32_t *) (void *) *dest) =
					endian_host_to_big(pread[0]);

			*dest += 4;
			*destlen -= 4;

			pread++;
			c->read_len--;
		}

		if (*destlen < c->read_len * 4) {
			/* Ran out of output buffer */
			size_t i;

			/* Shuffle remaining output down */
			for (i = 0; i < c->read_len; i++)
				c->read_buf[i] = pread[i];

			return PARSERUTILS_NOMEM;
		}
	}

	if (c->inval_len > 0) {
		/* The last decode ended in an incomplete sequence.
		 * Fill up inval_buf with data from the start of the
		 * new chunk and process it. */
		uint8_t *in = c->inval_buf;
		size_t ol = c->inval_len;
		size_t l = min(INVAL_BUFSIZE - ol - 1, *sourcelen);
		size_t orig_l = l;

		memcpy(c->inval_buf + ol, *source, l);

		l += c->inval_len;

		error = charset_utf8_codec_read_char(c,
				(const uint8_t **) &in, &l, dest, destlen);
		if (error != PARSERUTILS_OK && error != PARSERUTILS_NOMEM) {
			return error;
		}

		/* And now, fix up source pointers */
		*source += MAX((signed) (orig_l - l), 0);
		*sourcelen -= MAX((signed) (orig_l - l), 0);

		/* Failed to resolve an incomplete character and
		 * ran out of buffer space. No recovery strategy
		 * possible, so explode everywhere. */
		assert((orig_l + ol) - l != 0);

		/* Report memory exhaustion case from above */
		if (error != PARSERUTILS_OK)
			return error;
	}

	/* Finally, the "normal" case; process all outstanding characters */
	while (*sourcelen > 0) {
		error = charset_utf8_codec_read_char(c,
				source, sourcelen, dest, destlen);
		if (error != PARSERUTILS_OK) {
			return error;
		}
	}

	return PARSERUTILS_OK;
}

/**
 * Clear a UTF-8 codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_utf8_codec_reset(parserutils_charset_codec *codec)
{
	charset_utf8_codec *c = (charset_utf8_codec *) codec;

	c->inval_buf[0] = '\0';
	c->inval_len = 0;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	return PARSERUTILS_OK;
}


/**
 * Read a character from the UTF-8 to UCS-4 (big endian)
 *
 * \param c          The codec
 * \param source     Pointer to pointer to source buffer (updated on exit)
 * \param sourcelen  Pointer to length of source buffer (updated on exit)
 * \param dest       Pointer to pointer to output buffer (updated on exit)
 * \param destlen    Pointer to length of output buffer (updated on exit)
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_utf8_codec_read_char(charset_utf8_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	uint32_t ucs4;
	size_t sucs4;
	parserutils_error error;

	/* Convert a single character */
	{
		const uint8_t *src = *source;
		size_t srclen = *sourcelen;
		uint32_t *uptr = &ucs4;
		size_t *usptr = &sucs4;
		UTF8_TO_UCS4(src, srclen, uptr, usptr, error);
	}
	if (error == PARSERUTILS_OK) {
		/* Read a character */
		error = charset_utf8_codec_output_decoded_char(c,
				ucs4, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += sucs4;
			*sourcelen -= sucs4;
		}

		/* Clear inval buffer */
		c->inval_buf[0] = '\0';
		c->inval_len = 0;

		return error;
	} else if (error == PARSERUTILS_NEEDDATA) {
		/* Incomplete input sequence */
		assert(*sourcelen < INVAL_BUFSIZE);

		memmove(c->inval_buf, *source, *sourcelen);
		c->inval_buf[*sourcelen] = '\0';
		c->inval_len = *sourcelen;

		*source += *sourcelen;
		*sourcelen = 0;

		return PARSERUTILS_OK;
	} else if (error == PARSERUTILS_INVALID) {
		/* Illegal input sequence */
		uint32_t nextchar;

		/* Strict errormode; simply flag invalid character */
		if (c->base.errormode ==
				PARSERUTILS_CHARSET_CODEC_ERROR_STRICT) {
			/* Clear inval buffer */
			c->inval_buf[0] = '\0';
			c->inval_len = 0;

			return PARSERUTILS_INVALID;
		}

		/* Find next valid UTF-8 sequence.
		 * We're processing client-provided data, so let's
		 * be paranoid about its validity. */
		{
			const uint8_t *src = *source;
			size_t srclen = *sourcelen;
			uint32_t off = 0;
			uint32_t *ncptr = &nextchar;

			UTF8_NEXT_PARANOID(src, srclen, off, ncptr, error);
		}
		if (error != PARSERUTILS_OK) {
			if (error == PARSERUTILS_NEEDDATA) {
				/* Need more data to be sure */
				assert(*sourcelen < INVAL_BUFSIZE);

				memmove(c->inval_buf, *source, *sourcelen);
				c->inval_buf[*sourcelen] = '\0';
				c->inval_len = *sourcelen;

				*source += *sourcelen;
				*sourcelen = 0;

				nextchar = 0;
			} else {
				return error;
			}
		}

		/* Clear inval buffer */
		c->inval_buf[0] = '\0';
		c->inval_len = 0;

		/* output U+FFFD and continue processing. */
		error = charset_utf8_codec_output_decoded_char(c,
				0xFFFD, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += nextchar;
			*sourcelen -= nextchar;
		}

		return error;
	}

	return PARSERUTILS_OK;
}

/**
 * Output a UCS-4 character (big endian)
 *
 * \param c        Codec to use
 * \param ucs4     UCS-4 character (host endian)
 * \param dest     Pointer to pointer to output buffer
 * \param destlen  Pointer to output buffer length
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 */
parserutils_error charset_utf8_codec_output_decoded_char(charset_utf8_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen)
{
	if (*destlen < 4) {
		/* Run out of output buffer */
		c->read_len = 1;
		c->read_buf[0] = ucs4;

		return PARSERUTILS_NOMEM;
	}

	*((uint32_t *) (void *) *dest) = endian_host_to_big(ucs4);
	*dest += 4;
	*destlen -= 4;

	return PARSERUTILS_OK;
}


const parserutils_charset_handler charset_utf8_codec_handler = {
	charset_utf8_codec_handles_charset,
	charset_utf8_codec_create
};



/**
 * UTF-16 charset codec
 */
typedef struct charset_utf16_codec {
	parserutils_charset_codec base;	/**< Base class */

#define INVAL_BUFSIZE (32)
	uint8_t inval_buf[INVAL_BUFSIZE];	/**< Buffer for fixing up
						 * incomplete input
						 * sequences */
	size_t inval_len;		/*< Byte length of inval_buf **/

#define READ_BUFSIZE (8)
	uint32_t read_buf[READ_BUFSIZE];	/**< Buffer for partial
						 * output sequences (decode)
						 * (host-endian) */
	size_t read_len;		/**< Character length of read_buf */

#define WRITE_BUFSIZE (8)
	uint32_t write_buf[WRITE_BUFSIZE];	/**< Buffer for partial
						 * output sequences (encode)
						 * (host-endian) */
	size_t write_len;		/**< Character length of write_buf */

} charset_utf16_codec;

static bool charset_utf16_codec_handles_charset(const char *charset);
static parserutils_error charset_utf16_codec_create(const char *charset,
		parserutils_charset_codec **codec);
static parserutils_error charset_utf16_codec_destroy(
		parserutils_charset_codec *codec);
static parserutils_error charset_utf16_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_utf16_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static parserutils_error charset_utf16_codec_reset(
		parserutils_charset_codec *codec);
static inline parserutils_error charset_utf16_codec_read_char(
		charset_utf16_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);
static inline parserutils_error charset_utf16_codec_output_decoded_char(
		charset_utf16_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen);

/**
 * Determine whether this codec handles a specific charset
 *
 * \param charset  Charset to test
 * \return true if handleable, false otherwise
 */
bool charset_utf16_codec_handles_charset(const char *charset)
{
	return parserutils_charset_mibenum_from_name(charset, strlen(charset))
		==
		parserutils_charset_mibenum_from_name("UTF-16", SLEN("UTF-16"));
}

/**
 * Create a UTF-16 codec
 *
 * \param charset  The charset to read from / write to
 * \param codec    Pointer to location to receive codec
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_BADPARM on bad parameters,
 *         PARSERUTILS_NOMEM on memory exhausion
 */
parserutils_error charset_utf16_codec_create(const char *charset,
		parserutils_charset_codec **codec)
{
	charset_utf16_codec *c;

	UNUSED(charset);

	c = (charset_utf16_codec *)malloc(sizeof(charset_utf16_codec));
	if (c == NULL)
		return PARSERUTILS_NOMEM;

	c->inval_buf[0] = '\0';
	c->inval_len = 0;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	/* Finally, populate vtable */
	c->base.handler.destroy = charset_utf16_codec_destroy;
	c->base.handler.encode_charset = charset_utf16_codec_encode;
	c->base.handler.decode_charset = charset_utf16_codec_decode;
	c->base.handler.reset = charset_utf16_codec_reset;

	*codec = (parserutils_charset_codec *) c;

	return PARSERUTILS_OK;
}

/**
 * Destroy a UTF-16 codec
 *
 * \param codec  The codec to destroy
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_utf16_codec_destroy (parserutils_charset_codec *codec)
{
	UNUSED(codec);

	return PARSERUTILS_OK;
}

/**
 * Encode a chunk of UCS-4 (big endian) data into UTF-16
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read. Any remaining output for the character will be buffered by the
 * codec for writing on the next call.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_utf16_codec_encode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_utf16_codec *c = (charset_utf16_codec *) codec;
	uint32_t ucs4;
	uint32_t *towrite;
	size_t towritelen;
	parserutils_error error;

	/* Process any outstanding characters from the previous call */
	if (c->write_len > 0) {
		uint32_t *pwrite = c->write_buf;
		uint8_t buf[4];
		size_t len;

		while (c->write_len > 0) {
			error = parserutils_charset_utf16_from_ucs4(
					pwrite[0], buf, &len);
			assert(error == PARSERUTILS_OK);

			if (*destlen < len) {
				/* Insufficient output buffer space */
				for (len = 0; len < c->write_len; len++)
					c->write_buf[len] = pwrite[len];

				return PARSERUTILS_NOMEM;
			}

			memcpy(*dest, buf, len);

			*dest += len;
			*destlen -= len;

			pwrite++;
			c->write_len--;
		}
	}

	/* Now process the characters for this call */
	while (*sourcelen > 0) {
		ucs4 = endian_big_to_host(*((uint32_t *) (void *) *source));
		towrite = &ucs4;
		towritelen = 1;

		/* Output current characters */
		while (towritelen > 0) {
			uint8_t buf[4];
			size_t len;

			error = parserutils_charset_utf16_from_ucs4(
					towrite[0], buf, &len);
			assert(error == PARSERUTILS_OK);

			if (*destlen < len) {
				/* Insufficient output space */
				assert(towritelen < WRITE_BUFSIZE);

				c->write_len = towritelen;

				/* Copy pending chars to save area, for
				 * processing next call. */
				for (len = 0; len < towritelen; len++)
					c->write_buf[len] = towrite[len];

				/* Claim character we've just buffered,
				 * so it's not reprocessed */
				*source += 4;
				*sourcelen -= 4;

				return PARSERUTILS_NOMEM;
			}

			memcpy(*dest, buf, len);

			*dest += len;
			*destlen -= len;

			towrite++;
			towritelen--;
		}

		*source += 4;
		*sourcelen -= 4;
	}

	(void) error;

	return PARSERUTILS_OK;
}

/**
 * Decode a chunk of UTF-16 data into UCS-4 (big endian)
 *
 * \param codec      The codec to use
 * \param source     Pointer to pointer to source data
 * \param sourcelen  Pointer to length (in bytes) of source data
 * \param dest       Pointer to pointer to output buffer
 * \param destlen    Pointer to length (in bytes) of output buffer
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * Note that, if failure occurs whilst attempting to write any output
 * buffered by the last call, then ::source and ::sourcelen will remain
 * unchanged (as nothing more has been read).
 *
 * If STRICT error handling is configured and an illegal sequence is split
 * over two calls, then _INVALID will be returned from the second call,
 * but ::source will point mid-way through the invalid sequence (i.e. it
 * will be unmodified over the second call). In addition, the internal
 * incomplete-sequence buffer will be emptied, such that subsequent calls
 * will progress, rather than re-evaluating the same invalid sequence.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 *
 * Call this with a source length of 0 to flush the output buffer.
 */
parserutils_error charset_utf16_codec_decode(parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	charset_utf16_codec *c = (charset_utf16_codec *) codec;
	parserutils_error error;

	if (c->read_len > 0) {
		/* Output left over from last decode */
		uint32_t *pread = c->read_buf;

		while (c->read_len > 0 && *destlen >= c->read_len * 4) {
			*((uint32_t *) (void *) *dest) =
					endian_host_to_big(pread[0]);

			*dest += 4;
			*destlen -= 4;

			pread++;
			c->read_len--;
		}

		if (*destlen < c->read_len * 4) {
			/* Ran out of output buffer */
			size_t i;

			/* Shuffle remaining output down */
			for (i = 0; i < c->read_len; i++)
				c->read_buf[i] = pread[i];

			return PARSERUTILS_NOMEM;
		}
	}

	if (c->inval_len > 0) {
		/* The last decode ended in an incomplete sequence.
		 * Fill up inval_buf with data from the start of the
		 * new chunk and process it. */
		uint8_t *in = c->inval_buf;
		size_t ol = c->inval_len;
		size_t l = min(INVAL_BUFSIZE - ol - 1, *sourcelen);
		size_t orig_l = l;

		memcpy(c->inval_buf + ol, *source, l);

		l += c->inval_len;

		error = charset_utf16_codec_read_char(c,
				(const uint8_t **) &in, &l, dest, destlen);
		if (error != PARSERUTILS_OK && error != PARSERUTILS_NOMEM) {
			return error;
		}

		/* And now, fix up source pointers */
		*source += MAX((signed) (orig_l - l), 0);
		*sourcelen -= MAX((signed) (orig_l - l), 0);

		/* Failed to resolve an incomplete character and
		 * ran out of buffer space. No recovery strategy
		 * possible, so explode everywhere. */
		assert((orig_l + ol) - l != 0);

		/* Report memory exhaustion case from above */
		if (error != PARSERUTILS_OK)
			return error;
	}

	/* Finally, the "normal" case; process all outstanding characters */
	while (*sourcelen > 0) {
		error = charset_utf16_codec_read_char(c,
				source, sourcelen, dest, destlen);
		if (error != PARSERUTILS_OK) {
			return error;
		}
	}

	return PARSERUTILS_OK;
}

/**
 * Clear a UTF-16 codec's encoding state
 *
 * \param codec  The codec to reset
 * \return PARSERUTILS_OK on success, appropriate error otherwise
 */
parserutils_error charset_utf16_codec_reset(parserutils_charset_codec *codec)
{
	charset_utf16_codec *c = (charset_utf16_codec *) codec;

	c->inval_buf[0] = '\0';
	c->inval_len = 0;

	c->read_buf[0] = 0;
	c->read_len = 0;

	c->write_buf[0] = 0;
	c->write_len = 0;

	return PARSERUTILS_OK;
}


/**
 * Read a character from the UTF-16 to UCS-4 (big endian)
 *
 * \param c          The codec
 * \param source     Pointer to pointer to source buffer (updated on exit)
 * \param sourcelen  Pointer to length of source buffer (updated on exit)
 * \param dest       Pointer to pointer to output buffer (updated on exit)
 * \param destlen    Pointer to length of output buffer (updated on exit)
 * \return PARSERUTILS_OK on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 *         PARSERUTILS_INVALID     if a character cannot be represented and the
 *                            codec's error handling mode is set to STRICT,
 *
 * On exit, ::source will point immediately _after_ the last input character
 * read, if the result is _OK or _NOMEM. Any remaining output for the
 * character will be buffered by the codec for writing on the next call.
 *
 * In the case of the result being _INVALID, ::source will point _at_ the
 * last input character read; nothing will be written or buffered for the
 * failed character. It is up to the client to fix the cause of the failure
 * and retry the decoding process.
 *
 * ::sourcelen will be reduced appropriately on exit.
 *
 * ::dest will point immediately _after_ the last character written.
 *
 * ::destlen will be reduced appropriately on exit.
 */
parserutils_error charset_utf16_codec_read_char(charset_utf16_codec *c,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen)
{
	uint32_t ucs4;
	size_t sucs4;
	parserutils_error error;

	/* Convert a single character */
	error = parserutils_charset_utf16_to_ucs4(*source, *sourcelen,
			&ucs4, &sucs4);
	if (error == PARSERUTILS_OK) {
		/* Read a character */
		error = charset_utf16_codec_output_decoded_char(c,
				ucs4, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += sucs4;
			*sourcelen -= sucs4;
		}

		/* Clear inval buffer */
		c->inval_buf[0] = '\0';
		c->inval_len = 0;

		return error;
	} else if (error == PARSERUTILS_NEEDDATA) {
		/* Incomplete input sequence */
		assert(*sourcelen < INVAL_BUFSIZE);

		memmove(c->inval_buf, *source, *sourcelen);
		c->inval_buf[*sourcelen] = '\0';
		c->inval_len = *sourcelen;

		*source += *sourcelen;
		*sourcelen = 0;

		return PARSERUTILS_OK;
	} else if (error == PARSERUTILS_INVALID) {
		/* Illegal input sequence */
		uint32_t nextchar;

		/* Clear inval buffer */
		c->inval_buf[0] = '\0';
		c->inval_len = 0;

		/* Strict errormode; simply flag invalid character */
		if (c->base.errormode ==
				PARSERUTILS_CHARSET_CODEC_ERROR_STRICT) {
			return PARSERUTILS_INVALID;
		}

		/* Find next valid UTF-16 sequence.
		 * We're processing client-provided data, so let's
		 * be paranoid about its validity. */
		error = parserutils_charset_utf16_next_paranoid(
				*source, *sourcelen, 0, &nextchar);
		if (error != PARSERUTILS_OK) {
			if (error == PARSERUTILS_NEEDDATA) {
				/* Need more data to be sure */
				assert(*sourcelen < INVAL_BUFSIZE);

				memmove(c->inval_buf, *source, *sourcelen);
				c->inval_buf[*sourcelen] = '\0';
				c->inval_len = *sourcelen;

				*source += *sourcelen;
				*sourcelen = 0;

				nextchar = 0;
			} else {
				return error;
			}
		}

		/* output U+FFFD and continue processing. */
		error = charset_utf16_codec_output_decoded_char(c,
				0xFFFD, dest, destlen);
		if (error == PARSERUTILS_OK || error == PARSERUTILS_NOMEM) {
			/* output succeeded; update source pointers */
			*source += nextchar;
			*sourcelen -= nextchar;
		}

		return error;
	}

	return PARSERUTILS_OK;
}

/**
 * Output a UCS-4 character (big endian)
 *
 * \param c        Codec to use
 * \param ucs4     UCS-4 character (host endian)
 * \param dest     Pointer to pointer to output buffer
 * \param destlen  Pointer to output buffer length
 * \return PARSERUTILS_OK          on success,
 *         PARSERUTILS_NOMEM       if output buffer is too small,
 */
parserutils_error charset_utf16_codec_output_decoded_char(charset_utf16_codec *c,
		uint32_t ucs4, uint8_t **dest, size_t *destlen)
{
	if (*destlen < 4) {
		/* Run out of output buffer */
		c->read_len = 1;
		c->read_buf[0] = ucs4;

		return PARSERUTILS_NOMEM;
	}

	*((uint32_t *) (void *) *dest) = endian_host_to_big(ucs4);
	*dest += 4;
	*destlen -= 4;

	return PARSERUTILS_OK;
}


const parserutils_charset_handler charset_utf16_codec_handler = {
	charset_utf16_codec_handles_charset,
	charset_utf16_codec_create
};
