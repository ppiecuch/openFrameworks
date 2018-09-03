/*
 * This file is part of LibParserUtils.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef parserutils_parserutils_h_
#define parserutils_parserutils_h_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#ifndef NDEBUG
# include <stdio.h>
#endif

#ifndef parserutils_errors_h_
#define parserutils_errors_h_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum parserutils_error {
	PARSERUTILS_OK               = 0,

	PARSERUTILS_NOMEM            = 1,
	PARSERUTILS_BADPARM          = 2,
	PARSERUTILS_INVALID          = 3,
	PARSERUTILS_FILENOTFOUND     = 4,
	PARSERUTILS_NEEDDATA         = 5,
	PARSERUTILS_BADENCODING      = 6,
	PARSERUTILS_EOF              = 7
} parserutils_error;

/* Convert a parserutils error value to a string */
const char *parserutils_error_to_string(parserutils_error error);
/* Convert a string to a parserutils error value */
parserutils_error parserutils_error_from_string(const char *str, size_t len);

#ifdef __cplusplus
}
#endif

#endif // parserutils_errors_h_



#ifndef parserutils_utils_buffer_h_
#define parserutils_utils_buffer_h_

#ifdef __cplusplus
extern "C"
{
#endif

struct parserutils_buffer
{
	uint8_t *data;
	size_t length;
	size_t allocated;
};
typedef struct parserutils_buffer parserutils_buffer;

parserutils_error parserutils_buffer_create(parserutils_buffer **buffer);
parserutils_error parserutils_buffer_destroy(parserutils_buffer *buffer);

parserutils_error parserutils_buffer_append(parserutils_buffer *buffer,
		const uint8_t *data, size_t len);
parserutils_error parserutils_buffer_insert(parserutils_buffer *buffer,
		size_t offset, const uint8_t *data, size_t len);
parserutils_error parserutils_buffer_discard(parserutils_buffer *buffer,
		size_t offset, size_t len);

parserutils_error parserutils_buffer_grow(parserutils_buffer *buffer);

parserutils_error parserutils_buffer_randomise(parserutils_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // parserutils_utils_buffer_h_


#ifndef parserutils_utils_stack_h_
#define parserutils_utils_stack_h_

#ifdef __cplusplus
extern "C"
{
#endif

struct parserutils_stack;
typedef struct parserutils_stack parserutils_stack;

parserutils_error parserutils_stack_create(size_t item_size, size_t chunk_size,
		parserutils_stack **stack);
parserutils_error parserutils_stack_destroy(parserutils_stack *stack);

parserutils_error parserutils_stack_push(parserutils_stack *stack,
		const void *item);
parserutils_error parserutils_stack_pop(parserutils_stack *stack, void *item);

void *parserutils_stack_get_current(parserutils_stack *stack);

#ifdef __cplusplus
}
#endif

#endif // parserutils_utils_stack_h_


#ifndef parserutils_utils_vector_h_
#define parserutils_utils_vector_h_

#ifdef __cplusplus
extern "C"
{
#endif

struct parserutils_vector;
typedef struct parserutils_vector parserutils_vector;

parserutils_error parserutils_vector_create(size_t item_size,
		size_t chunk_size, parserutils_vector **vector);
parserutils_error parserutils_vector_destroy(parserutils_vector *vector);

parserutils_error parserutils_vector_append(parserutils_vector *vector,
		void *item);
parserutils_error parserutils_vector_clear(parserutils_vector *vector);
parserutils_error parserutils_vector_remove_last(parserutils_vector *vector);
parserutils_error parserutils_vector_get_length(parserutils_vector *vector, size_t *length);

const void *parserutils_vector_iterate(const parserutils_vector *vector,
		int32_t *ctx);
const void *parserutils_vector_peek(const parserutils_vector *vector,
		int32_t ctx);

#ifdef __cplusplus
}
#endif

#endif // parserutils_utils_vector_h_


#ifndef parserutils_charset_utf8_h_
#define parserutils_charset_utf8_h_

#ifdef __cplusplus
extern "C"
{
#endif

parserutils_error parserutils_charset_utf8_to_ucs4(const uint8_t *s, size_t len,
		uint32_t *ucs4, size_t *clen);
parserutils_error parserutils_charset_utf8_from_ucs4(uint32_t ucs4, uint8_t **s,
		size_t *len);

parserutils_error parserutils_charset_utf8_length(const uint8_t *s, size_t max,
		size_t *len);
parserutils_error parserutils_charset_utf8_char_byte_length(const uint8_t *s,
		size_t *len);

parserutils_error parserutils_charset_utf8_prev(const uint8_t *s, uint32_t off,
		uint32_t *prevoff);
parserutils_error parserutils_charset_utf8_next(const uint8_t *s, uint32_t len,
		uint32_t off, uint32_t *nextoff);

parserutils_error parserutils_charset_utf8_next_paranoid(const uint8_t *s,
		uint32_t len, uint32_t off, uint32_t *nextoff);

#ifdef __cplusplus
}
#endif

#endif // parserutils_charset_utf8_h_


#ifndef parserutils_charset_utf16_h_
#define parserutils_charset_utf16_h_

#ifdef __cplusplus
extern "C"
{
#endif

parserutils_error parserutils_charset_utf16_to_ucs4(const uint8_t *s,
		size_t len, uint32_t *ucs4, size_t *clen);
parserutils_error parserutils_charset_utf16_from_ucs4(uint32_t ucs4,
		uint8_t *s, size_t *len);

parserutils_error parserutils_charset_utf16_length(const uint8_t *s,
		size_t max, size_t *len);
parserutils_error parserutils_charset_utf16_char_byte_length(const uint8_t *s,
		size_t *len);

parserutils_error parserutils_charset_utf16_prev(const uint8_t *s,
		uint32_t off, uint32_t *prevoff);
parserutils_error parserutils_charset_utf16_next(const uint8_t *s,
		uint32_t len, uint32_t off, uint32_t *nextoff);

parserutils_error parserutils_charset_utf16_next_paranoid(const uint8_t *s,
		uint32_t len, uint32_t off, uint32_t *nextoff);

#ifdef __cplusplus
}
#endif

#endif // parserutils_charset_utf16_h_


#ifndef parserutils_charset_codec_h_
#define parserutils_charset_codec_h_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct parserutils_charset_codec parserutils_charset_codec;

#define PARSERUTILS_CHARSET_CODEC_NULL (0xffffffffU)

/**
 * Charset codec error mode
 *
 * A codec's error mode determines its behaviour in the face of:
 *
 * + characters which are unrepresentable in the destination charset (if
 *   encoding data) or which cannot be converted to UCS-4 (if decoding data).
 * + invalid byte sequences (both encoding and decoding)
 *
 * The options provide a choice between the following approaches:
 *
 * + draconian, "stop processing" ("strict")
 * + "replace the unrepresentable character with something else" ("loose")
 * + "attempt to transliterate, or replace if unable" ("translit")
 *
 * The default error mode is "loose".
 *
 *
 * In the "loose" case, the replacement character will depend upon:
 *
 * + Whether the operation was encoding or decoding
 * + If encoding, what the destination charset is.
 *
 * If decoding, the replacement character will be:
 *
 *     U+FFFD (REPLACEMENT CHARACTER)
 *
 * If encoding, the replacement character will be:
 *
 *     U+003F (QUESTION MARK) if the destination charset is not UTF-(8|16|32)
 *     U+FFFD (REPLACEMENT CHARACTER) otherwise.
 *
 *
 * In the "translit" case, the codec will attempt to transliterate into
 * the destination charset, if encoding. If decoding, or if transliteration
 * fails, this option is identical to "loose".
 */
typedef enum parserutils_charset_codec_errormode {
	/** Abort processing if unrepresentable character encountered */
	PARSERUTILS_CHARSET_CODEC_ERROR_STRICT   = 0,
	/** Replace unrepresentable characters with single alternate */
	PARSERUTILS_CHARSET_CODEC_ERROR_LOOSE    = 1,
	/** Transliterate unrepresentable characters, if possible */
	PARSERUTILS_CHARSET_CODEC_ERROR_TRANSLIT = 2
} parserutils_charset_codec_errormode;

/**
 * Charset codec option types
 */
typedef enum parserutils_charset_codec_opttype {
	/** Set codec error mode */
	PARSERUTILS_CHARSET_CODEC_ERROR_MODE  = 1
} parserutils_charset_codec_opttype;

/**
 * Charset codec option parameters
 */
typedef union parserutils_charset_codec_optparams {
	/** Parameters for error mode setting */
	struct {
		/** The desired error handling mode */
		parserutils_charset_codec_errormode mode;
	} error_mode;
} parserutils_charset_codec_optparams;


/* Create a charset codec */
parserutils_error parserutils_charset_codec_create(const char *charset,
		parserutils_charset_codec **codec);
/* Destroy a charset codec */
parserutils_error parserutils_charset_codec_destroy(
		parserutils_charset_codec *codec);

/* Configure a charset codec */
parserutils_error parserutils_charset_codec_setopt(
		parserutils_charset_codec *codec,
		parserutils_charset_codec_opttype type,
		parserutils_charset_codec_optparams *params);

/* Encode a chunk of UCS-4 data into a codec's charset */
parserutils_error parserutils_charset_codec_encode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);

/* Decode a chunk of data in a codec's charset into UCS-4 */
parserutils_error parserutils_charset_codec_decode(
		parserutils_charset_codec *codec,
		const uint8_t **source, size_t *sourcelen,
		uint8_t **dest, size_t *destlen);

/* Reset a charset codec */
parserutils_error parserutils_charset_codec_reset(
		parserutils_charset_codec *codec);

#ifdef __cplusplus
}
#endif

#endif // parserutils_charset_codec_h_


#ifndef parserutils_charset_mibenum_h_
#define parserutils_charset_mibenum_h_

#ifdef __cplusplus
extern "C"
{
#endif

/* Convert an encoding alias to a MIB enum value */
uint16_t parserutils_charset_mibenum_from_name(const char *alias, size_t len);
/* Convert a MIB enum value into an encoding alias */
const char *parserutils_charset_mibenum_to_name(uint16_t mibenum);
/* Determine if a MIB enum value represents a Unicode variant */
bool parserutils_charset_mibenum_is_unicode(uint16_t mibenum);

#ifdef __cplusplus
}
#endif

#endif // parserutils_charset_mibenum_h_


#ifndef parserutils_input_inputstream_h_
#define parserutils_input_inputstream_h_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Type of charset detection function
 */
typedef parserutils_error (*parserutils_charset_detect_func)(
		const uint8_t *data, size_t len,
		uint16_t *mibenum, uint32_t *source);

/**
 * Input stream object
 */
typedef struct parserutils_inputstream
{
	parserutils_buffer *utf8;	/**< Buffer containing UTF-8 data */

	uint32_t cursor;		/**< Byte offset of current position */

	bool had_eof;			/**< Whether EOF has been reached */
} parserutils_inputstream;

/* Create an input stream */
parserutils_error parserutils_inputstream_create(const char *enc,
		uint32_t encsrc, parserutils_charset_detect_func csdetect,
		parserutils_inputstream **stream);
/* Destroy an input stream */
parserutils_error parserutils_inputstream_destroy(
		parserutils_inputstream *stream);

/* Append data to an input stream */
parserutils_error parserutils_inputstream_append(
		parserutils_inputstream *stream,
		const uint8_t *data, size_t len);
/* Insert data into stream at current location */
parserutils_error parserutils_inputstream_insert(
		parserutils_inputstream *stream,
		const uint8_t *data, size_t len);

/* Slow form of css_inputstream_peek. */
parserutils_error parserutils_inputstream_peek_slow(
		parserutils_inputstream *stream,
		size_t offset, const uint8_t **ptr, size_t *length);

/**
 * Look at the character in the stream that starts at
 * offset bytes from the cursor
 *
 * \param stream  Stream to look in
 * \param offset  Byte offset of start of character
 * \param ptr     Pointer to location to receive pointer to character data
 * \param length  Pointer to location to receive character length (in bytes)
 * \return PARSERUTILS_OK on success,
 *                    _NEEDDATA on reaching the end of available input,
 *                    _EOF on reaching the end of all input,
 *                    _BADENCODING if the input cannot be decoded,
 *                    _NOMEM on memory exhaustion,
 *                    _BADPARM if bad parameters are passed.
 *
 * Once the character pointed to by the result of this call has been advanced
 * past (i.e. parserutils_inputstream_advance has caused the stream cursor to
 * pass over the character), then no guarantee is made as to the validity of
 * the data pointed to. Thus, any attempt to dereference the pointer after
 * advancing past the data it points to is a bug.
 */
static inline parserutils_error parserutils_inputstream_peek(
		parserutils_inputstream *stream, size_t offset,
		const uint8_t **ptr, size_t *length)
{
	parserutils_error error = PARSERUTILS_OK;
	const parserutils_buffer *utf8;
	const uint8_t *utf8_data;
	size_t len, off, utf8_len;

	if (stream == NULL || ptr == NULL || length == NULL)
		return PARSERUTILS_BADPARM;

#ifndef NDEBUG
#ifdef VERBOSE_INPUTSTREAM
	fprintf(stdout, "Peek: len: %zu cur: %u off: %zu\n",
			stream->utf8->length, stream->cursor, offset);
#endif
#ifdef RANDOMISE_INPUTSTREAM
	parserutils_buffer_randomise(stream->utf8);
#endif
#endif

	utf8 = stream->utf8;
	utf8_data = utf8->data;
	utf8_len = utf8->length;
	off = stream->cursor + offset;

#define IS_ASCII(x) (((x) & 0x80) == 0)

	if (off < utf8_len) {
		if (IS_ASCII(utf8_data[off])) {
			/* Early exit for ASCII case */
			(*length) = 1;
			(*ptr) = (utf8_data + off);
			return PARSERUTILS_OK;
		} else {
			error = parserutils_charset_utf8_char_byte_length(
				utf8_data + off, &len);

			if (error == PARSERUTILS_OK) {
				(*length) = len;
				(*ptr) = (utf8_data + off);
				return PARSERUTILS_OK;
			} else if (error != PARSERUTILS_NEEDDATA) {
				return error;
			}
		}
	}

#undef IS_ASCII

	return parserutils_inputstream_peek_slow(stream, offset, ptr, length);
}

/**
 * Advance the stream's current position
 *
 * \param stream  The stream whose position to advance
 * \param bytes   The number of bytes to advance
 */
static inline void parserutils_inputstream_advance(
		parserutils_inputstream *stream, size_t bytes)
{
	if (stream == NULL)
		return;

#if !defined(NDEBUG) && defined(VERBOSE_INPUTSTREAM)
	fprintf(stdout, "Advance: len: %zu cur: %u bytes: %zu\n",
			stream->utf8->length, stream->cursor, bytes);
#endif

	if (bytes > stream->utf8->length - stream->cursor)
		bytes = stream->utf8->length - stream->cursor;

	if (stream->cursor == stream->utf8->length)
		return;

	stream->cursor += bytes;
}

/* Read the document charset */
const char *parserutils_inputstream_read_charset(
		parserutils_inputstream *stream, uint32_t *source);
/* Change the document charset */
parserutils_error parserutils_inputstream_change_charset(
		parserutils_inputstream *stream,
		const char *enc, uint32_t source);

#ifdef __cplusplus
}
#endif

#endif // parserutils_input_inputstream_h_


#endif // parserutils_parserutils_h_

