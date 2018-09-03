/*
 * This file is part of LibParserUtils.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef parserutils_utils_h_
#define parserutils_utils_h_

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef SLEN
/* Calculate length of a string constant */
#define SLEN(s) (sizeof((s)) - 1) /* -1 for '\0' */
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#ifndef N_ELEMENTS
#define N_ELEMENTS(s) (sizeof((s)) / sizeof((s)[0]))
#endif

#ifndef ALIGN
#define ALIGN(val) (((val) + 3) & ~(3))
#endif

#endif // parserutils_utils_h_


#ifndef parserutils_endian_h_
#define parserutils_endian_h_

static inline bool endian_host_is_le(void)
{
	static uint32_t magic = 0x10000002;

	return (((uint8_t *) &magic)[0] == 0x02);
}

static inline uint32_t endian_swap(uint32_t val)
{
	return ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) |
		((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
}

static inline uint32_t endian_host_to_big(uint32_t host)
{
	if (endian_host_is_le())
		return endian_swap(host);

	return host;
}

static inline uint32_t endian_big_to_host(uint32_t big)
{
	if (endian_host_is_le())
		return endian_swap(big);

	return big;
}

#endif // parserutils_endian_h_


