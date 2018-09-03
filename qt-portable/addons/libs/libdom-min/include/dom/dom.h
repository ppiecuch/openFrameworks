/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_all_h_
#define dom_all_h_

#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include <libwapcaplet/libwapcaplet.h>


/* Base library headers */

/**
 * The IDL spec(2nd ed) 3.10.5 defines a short type with 16bit range
 */
typedef int16_t dom_short;

/**
 * The IDL spec(2nd ed) 3.10.6 defines an unsigned short type with 16bit range
 */
typedef uint16_t dom_ushort;

/**
 * The IDL spec(2nd ed) 3.10.7 defines a long type with 32bit range
 */
typedef int32_t dom_long;

/**
 * The IDL spec(2nd ed) 3.10.8 defines an unsigned long type with 32bit range
 */
typedef uint32_t dom_ulong;


/**
 * Severity levels for dom_msg function, based on syslog(3)
 */
enum {
	DOM_MSG_DEBUG,
	DOM_MSG_INFO,
	DOM_MSG_NOTICE,
	DOM_MSG_WARNING,
	DOM_MSG_ERROR,
	DOM_MSG_CRITICAL,
	DOM_MSG_ALERT,
	DOM_MSG_EMERGENCY
};

/**
 * Type of DOM message function
 */
typedef void (*dom_msg)(uint32_t severity, void *ctx, const char *msg, ...);


typedef enum dom_namespace {
	DOM_NAMESPACE_NULL    = 0,
	DOM_NAMESPACE_HTML    = 1,
	DOM_NAMESPACE_MATHML  = 2,
	DOM_NAMESPACE_SVG     = 3,
	DOM_NAMESPACE_XLINK   = 4,
	DOM_NAMESPACE_XML     = 5,
	DOM_NAMESPACE_XMLNS   = 6,

	DOM_NAMESPACE_COUNT   = 7
} dom_namespace;

extern struct dom_string *dom_namespaces[DOM_NAMESPACE_COUNT];


#ifndef dom_core_exceptions_h_
#define dom_core_exceptions_h_

/**
 * Class of a DOM exception.
 *
 * The top 8 bits of a dom_exception are unused, the next 8 bits are a bitfield
 * indicating which class the exception belongs to.
 */
typedef enum {
	DOM_EXCEPTION_CLASS_NORMAL = 0,
	DOM_EXCEPTION_CLASS_EVENT = (1<<16),
	DOM_EXCEPTION_CLASS_INTERNAL = (1<<17)
} dom_exception_class;

/* The DOM spec says that this is actually an unsigned short */
typedef enum {
	DOM_NO_ERR			=  0,
	DOM_INDEX_SIZE_ERR		=  1,
	DOM_DOMSTRING_SIZE_ERR		=  2,
	DOM_HIERARCHY_REQUEST_ERR	=  3,
	DOM_WRONG_DOCUMENT_ERR		=  4,
	DOM_INVALID_CHARACTER_ERR	=  5,
	DOM_NO_DATA_ALLOWED_ERR		=  6,
	DOM_NO_MODIFICATION_ALLOWED_ERR	=  7,
	DOM_NOT_FOUND_ERR		=  8,
	DOM_NOT_SUPPORTED_ERR		=  9,
	DOM_INUSE_ATTRIBUTE_ERR		= 10,
	DOM_INVALID_STATE_ERR		= 11,
	DOM_SYNTAX_ERR			= 12,
	DOM_INVALID_MODIFICATION_ERR	= 13,
	DOM_NAMESPACE_ERR		= 14,
	DOM_INVALID_ACCESS_ERR		= 15,
	DOM_VALIDATION_ERR		= 16,
	DOM_TYPE_MISMATCH_ERR		= 17,

	DOM_UNSPECIFIED_EVENT_TYPE_ERR = DOM_EXCEPTION_CLASS_EVENT + 0,
	DOM_DISPATCH_REQUEST_ERR = DOM_EXCEPTION_CLASS_EVENT + 1,

	DOM_NO_MEM_ERR = DOM_EXCEPTION_CLASS_INTERNAL + 0,
	DOM_ATTR_WRONG_TYPE_ERR = DOM_EXCEPTION_CLASS_INTERNAL + 1
			/* our own internal error */
} dom_exception;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_string_h_
#define dom_string_h_

typedef struct dom_string dom_string;
struct dom_string {
	uint32_t refcnt;
} _ALIGNED;


/* Claim a reference on a DOM string */
static inline dom_string *dom_string_ref(dom_string *str)
{
	if (str != NULL)
		str->refcnt++;
	return str;
}

/* Destroy a DOM string */
void dom_string_destroy(dom_string *str);

/* Release a reference on a DOM string */
static inline void dom_string_unref(dom_string *str) 
{
	if ((str != NULL) && (--(str->refcnt) == 0)) {
		dom_string_destroy(str);
	}
}

/* Create a DOM string from a string of characters */
dom_exception dom_string_create(const uint8_t *ptr, size_t len, 
		dom_string **str);
dom_exception dom_string_create_interned(const uint8_t *ptr, size_t len,
		dom_string **str);

/* Obtain an interned representation of a dom string */
dom_exception dom_string_intern(dom_string *str, 
		struct lwc_string_s **lwcstr);

/* Case sensitively compare two DOM strings */
bool dom_string_isequal(const dom_string *s1, const dom_string *s2);
/* Case insensitively compare two DOM strings */
bool dom_string_caseless_isequal(const dom_string *s1, const dom_string *s2);

/* Case sensitively compare DOM string and lwc_string */
bool dom_string_lwc_isequal(const dom_string *s1, lwc_string *s2);
/* Case insensitively compare DOM string and lwc_string */
bool dom_string_caseless_lwc_isequal(const dom_string *s1, lwc_string *s2);

/* Get the index of the first occurrence of a character in a dom string */
uint32_t dom_string_index(dom_string *str, uint32_t chr);
/* Get the index of the last occurrence of a character in a dom string */
uint32_t dom_string_rindex(dom_string *str, uint32_t chr);

/* Get the length, in characters, of a dom string */
uint32_t dom_string_length(dom_string *str);

/**
 * Get the raw character data of the dom_string.
 * @note: This function is just provided for the convenience of accessing the 
 * raw C string character, no change on the result string is allowed.
 */
const char *dom_string_data(const dom_string *str);

/* Get the byte length of this dom_string */
size_t dom_string_byte_length(const dom_string *str);

/* Get the UCS-4 character at position index, the index should be in 
 * [0, length), and length can be get by calling dom_string_length
 */
dom_exception dom_string_at(dom_string *str, uint32_t index, 
		uint32_t *ch);

/* Concatenate two dom strings */
dom_exception dom_string_concat(dom_string *s1, dom_string *s2,
		dom_string **result);

/* Extract a substring from a dom string */
dom_exception dom_string_substr(dom_string *str, 
		uint32_t i1, uint32_t i2, dom_string **result);

/* Insert data into a dom string at the given location */
dom_exception dom_string_insert(dom_string *target,
		dom_string *source, uint32_t offset,
		dom_string **result);

/* Replace a section of a dom string */
dom_exception dom_string_replace(dom_string *target,
		dom_string *source, uint32_t i1, uint32_t i2,
		dom_string **result);

/* Generate an uppercase version of the given string */
dom_exception dom_string_toupper(dom_string *source, bool ascii_only,
				 dom_string **upper);

/* Generate an lowercase version of the given string */
dom_exception dom_string_tolower(dom_string *source, bool ascii_only,
				 dom_string **lower);

/* Calculate a hash value from a dom string */
uint32_t dom_string_hash(dom_string *str);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_node_h_
#define dom_core_node_h_

struct dom_document;
struct dom_nodelist;
struct dom_namednodemap;
struct dom_node;

/**
 * Bits defining position of a node in a document relative to some other node
 */
typedef enum {
	DOM_DOCUMENT_POSITION_DISCONNECTED		= 0x01,
	DOM_DOCUMENT_POSITION_PRECEDING			= 0x02,
	DOM_DOCUMENT_POSITION_FOLLOWING			= 0x04,
	DOM_DOCUMENT_POSITION_CONTAINS			= 0x08,
	DOM_DOCUMENT_POSITION_CONTAINED_BY		= 0x10,
	DOM_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC	= 0x20
} dom_document_position;

/**
 * Type of node operation being notified to user_data_handler
 */
typedef enum {
	DOM_NODE_CLONED		= 1,
	DOM_NODE_IMPORTED	= 2,
	DOM_NODE_DELETED	= 3,
	DOM_NODE_RENAMED	= 4,
	DOM_NODE_ADOPTED	= 5
} dom_node_operation;

/**
 * Type of handler function for user data registered on a DOM node
 */
typedef void (*dom_user_data_handler)(dom_node_operation operation,
		dom_string *key, void *data, struct dom_node *src,
		struct dom_node *dst);

/**
 * Type of a DOM node
 */
typedef enum {
	DOM_ELEMENT_NODE		= 1,
	DOM_ATTRIBUTE_NODE		= 2,
	DOM_TEXT_NODE			= 3,
	DOM_CDATA_SECTION_NODE		= 4,
	DOM_ENTITY_REFERENCE_NODE	= 5,
	DOM_ENTITY_NODE			= 6,
	DOM_PROCESSING_INSTRUCTION_NODE	= 7,
	DOM_COMMENT_NODE		= 8,
	DOM_DOCUMENT_NODE		= 9,
	DOM_DOCUMENT_TYPE_NODE		= 10,
	DOM_DOCUMENT_FRAGMENT_NODE	= 11,
	DOM_NOTATION_NODE		= 12,

	/* And a count of the number of node types */
	DOM_NODE_TYPE_COUNT
} dom_node_type;

typedef struct dom_node_internal dom_node_internal;

/**
 * DOM node type
 */
typedef struct dom_node {
	void *vtable;
	uint32_t refcnt;
} dom_node;

/* DOM node vtable */
typedef struct dom_node_vtable {
	/* pre-destruction hook */
	dom_exception (*dom_node_try_destroy)(dom_node_internal *node);
	/* The DOM level 3 node's oprations */
	dom_exception (*dom_node_get_node_name)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_get_node_value)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_set_node_value)(dom_node_internal *node,
			dom_string *value);
	dom_exception (*dom_node_get_node_type)(dom_node_internal *node,
			dom_node_type *result);
	dom_exception (*dom_node_get_parent_node)(dom_node_internal *node,
			dom_node_internal **result);
	dom_exception (*dom_node_get_child_nodes)(dom_node_internal *node,
			struct dom_nodelist **result);
	dom_exception (*dom_node_get_first_child)(dom_node_internal *node,
			dom_node_internal **result);
	dom_exception (*dom_node_get_last_child)(dom_node_internal *node,
			dom_node_internal **result);
	dom_exception (*dom_node_get_previous_sibling)(dom_node_internal *node,
			dom_node_internal **result);
	dom_exception (*dom_node_get_next_sibling)(dom_node_internal *node,
			dom_node_internal **result);
	dom_exception (*dom_node_get_attributes)(dom_node_internal *node,
			struct dom_namednodemap **result);
	dom_exception (*dom_node_get_owner_document)(dom_node_internal *node,
			struct dom_document **result);
	dom_exception (*dom_node_insert_before)(dom_node_internal *node,
			dom_node_internal *new_child, 
			dom_node_internal *ref_child,
			dom_node_internal **result);
	dom_exception (*dom_node_replace_child)(dom_node_internal *node,
			dom_node_internal *new_child, 
			dom_node_internal *old_child,
			dom_node_internal **result);
	dom_exception (*dom_node_remove_child)(dom_node_internal *node,
			dom_node_internal *old_child,
			dom_node_internal **result);
	dom_exception (*dom_node_append_child)(dom_node_internal *node,
			dom_node_internal *new_child,
			dom_node_internal **result);
	dom_exception (*dom_node_has_child_nodes)(dom_node_internal *node, 
			bool *result);
	dom_exception (*dom_node_clone_node)(dom_node_internal *node, bool deep,
			dom_node_internal **result);
	dom_exception (*dom_node_normalize)(dom_node_internal *node);
	dom_exception (*dom_node_is_supported)(dom_node_internal *node,
			dom_string *feature, dom_string *version,
			bool *result);
	dom_exception (*dom_node_get_namespace)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_get_prefix)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_set_prefix)(dom_node_internal *node,
			dom_string *prefix);
	dom_exception (*dom_node_get_local_name)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_has_attributes)(dom_node_internal *node, 
			bool *result);
	dom_exception (*dom_node_get_base)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_compare_document_position)(
			dom_node_internal *node, dom_node_internal *other,
			uint16_t *result);
	dom_exception (*dom_node_get_text_content)(dom_node_internal *node,
			dom_string **result);
	dom_exception (*dom_node_set_text_content)(dom_node_internal *node,
			dom_string *content);
	dom_exception (*dom_node_is_same)(dom_node_internal *node, 
			dom_node_internal *other, bool *result);
	dom_exception (*dom_node_lookup_prefix)(dom_node_internal *node,
			dom_string *ns,
			dom_string **result);
	dom_exception (*dom_node_is_default_namespace)(dom_node_internal *node,
			dom_string *ns, bool *result);
	dom_exception (*dom_node_lookup_namespace)(dom_node_internal *node,
			dom_string *prefix, dom_string **result);
	dom_exception (*dom_node_is_equal)(dom_node_internal *node,
			dom_node_internal *other, bool *result);
	dom_exception (*dom_node_get_feature)(dom_node_internal *node,
			dom_string *feature, dom_string *version,
			void **result);
	dom_exception (*dom_node_set_user_data)(dom_node_internal *node,
			dom_string *key, void *data,
			dom_user_data_handler handler, void **result);
	dom_exception (*dom_node_get_user_data)(dom_node_internal *node,
			dom_string *key, void **result);
} dom_node_vtable;

/* The ref/unref methods define */

static inline dom_node *dom_node_ref(dom_node *node)
{
	if (node != NULL)
		node->refcnt++;
	
	return node;
}

#define dom_node_ref(n) dom_node_ref((dom_node *) (n))

static inline dom_exception dom_node_try_destroy(dom_node *node)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_try_destroy(
			(dom_node_internal *) node);
}
#define dom_node_try_destroy(n) dom_node_try_destroy((dom_node *) (n))

static inline void dom_node_unref(dom_node *node)
{
	if (node != NULL) {
		if (--node->refcnt == 0)
			dom_node_try_destroy(node);
	}
		
}
#define dom_node_unref(n) dom_node_unref((dom_node *) (n))

static inline dom_exception dom_node_get_node_name(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_node_name(
			(dom_node_internal *) node, result);
}
#define dom_node_get_node_name(n, r) dom_node_get_node_name((dom_node *) (n), (r))

static inline dom_exception dom_node_get_node_value(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_node_value(
			(dom_node_internal *) node, result);
}
#define dom_node_get_node_value(n, r) dom_node_get_node_value( \
		(dom_node *) (n), (r))

static inline dom_exception dom_node_set_node_value(struct dom_node *node,
		dom_string *value)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_set_node_value(
			(dom_node_internal *) node, value);
}
#define dom_node_set_node_value(n, v) dom_node_set_node_value( \
		(dom_node *) (n), (v))

static inline dom_exception dom_node_get_node_type(struct dom_node *node,
		dom_node_type *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_node_type(
			(dom_node_internal *) node, result);
}
#define dom_node_get_node_type(n, r) dom_node_get_node_type( \
		(dom_node *) (n), (dom_node_type *) (r))

static inline dom_exception dom_node_get_parent_node(struct dom_node *node,
		dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_parent_node(
			(dom_node_internal *) node, 
			(dom_node_internal **) result);
}
#define dom_node_get_parent_node(n, r) dom_node_get_parent_node( \
		(dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_node_get_child_nodes(struct dom_node *node,
		struct dom_nodelist **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_child_nodes(
			(dom_node_internal *) node, result);
}
#define dom_node_get_child_nodes(n, r) dom_node_get_child_nodes( \
		(dom_node *) (n), (struct dom_nodelist **) (r))

static inline dom_exception dom_node_get_first_child(struct dom_node *node,
		dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_first_child(
			(dom_node_internal *) node, 
			(dom_node_internal **) result);
}
#define dom_node_get_first_child(n, r) dom_node_get_first_child( \
		(dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_node_get_last_child(struct dom_node *node,
		dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_last_child(
			(dom_node_internal *) node, 
			(dom_node_internal **) result);
}
#define dom_node_get_last_child(n, r) dom_node_get_last_child( \
		(dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_node_get_previous_sibling(
		struct dom_node *node, dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->
			dom_node_get_previous_sibling(
			(dom_node_internal *) node, 
			(dom_node_internal **) result);
}
#define dom_node_get_previous_sibling(n, r) dom_node_get_previous_sibling( \
		(dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_node_get_next_sibling(struct dom_node *node,
		dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_next_sibling(
			(dom_node_internal *) node, 
			(dom_node_internal **) result);
}
#define dom_node_get_next_sibling(n, r) dom_node_get_next_sibling( \
		(dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_node_get_attributes(struct dom_node *node,
		struct dom_namednodemap **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_attributes(
			(dom_node_internal *) node, result);
}
#define dom_node_get_attributes(n, r) dom_node_get_attributes( \
		(dom_node *) (n), (struct dom_namednodemap **) (r))

static inline dom_exception dom_node_get_owner_document(struct dom_node *node,
		struct dom_document **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_owner_document(
			(dom_node_internal *) node, result);
}
#define dom_node_get_owner_document(n, r) dom_node_get_owner_document( \
		(dom_node *) (n), (struct dom_document **) (r))

static inline dom_exception dom_node_insert_before(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *ref_child,
		struct dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_insert_before(
			(dom_node_internal *) node,
			(dom_node_internal *) new_child,
			(dom_node_internal *) ref_child,
			(dom_node_internal **) result);
}
#define dom_node_insert_before(n, nn, ref, ret) dom_node_insert_before( \
		(dom_node *) (n), (dom_node *) (nn), (dom_node *) (ref),\
		(dom_node **) (ret))

static inline dom_exception dom_node_replace_child(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *old_child,
		struct dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_replace_child( 
			(dom_node_internal *) node,
			(dom_node_internal *) new_child,
			(dom_node_internal *) old_child,
			(dom_node_internal **) result);
}
#define dom_node_replace_child(n, nn, old, ret) dom_node_replace_child( \
		(dom_node *) (n), (dom_node *) (nn), (dom_node *) (old),\
		(dom_node **) (ret))

static inline dom_exception dom_node_remove_child(struct dom_node *node,
		struct dom_node *old_child,
		struct dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_remove_child( 
			(dom_node_internal *) node,
			(dom_node_internal *) old_child,
			(dom_node_internal **) result);
}
#define dom_node_remove_child(n, old, ret) dom_node_remove_child( \
		(dom_node *) (n), (dom_node *) (old), (dom_node **) (ret))

static inline dom_exception dom_node_append_child(struct dom_node *node,
		struct dom_node *new_child,
		struct dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_append_child(
			(dom_node_internal *) node,
			(dom_node_internal *) new_child,
			(dom_node_internal **) result);
}
#define dom_node_append_child(n, nn, ret) dom_node_append_child( \
		(dom_node *) (n), (dom_node *) (nn), (dom_node **) (ret))

static inline dom_exception dom_node_has_child_nodes(struct dom_node *node, 
		bool *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_has_child_nodes(
			(dom_node_internal *) node, result);
}
#define dom_node_has_child_nodes(n, r) dom_node_has_child_nodes( \
		(dom_node *) (n), (bool *) (r))

static inline dom_exception dom_node_clone_node(struct dom_node *node, 
		bool deep, struct dom_node **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_clone_node(
			(dom_node_internal *) node, deep,
			(dom_node_internal **) result);
}
#define dom_node_clone_node(n, d, r) dom_node_clone_node((dom_node *) (n), \
		(bool) (d), (dom_node **) (r))

static inline dom_exception dom_node_normalize(struct dom_node *node)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_normalize(
			(dom_node_internal *) node);
}
#define dom_node_normalize(n) dom_node_normalize((dom_node *) (n))

static inline dom_exception dom_node_is_supported(struct dom_node *node,
		dom_string *feature, dom_string *version,
		bool *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_is_supported(
			(dom_node_internal *) node, feature, 
			version, result);
}
#define dom_node_is_supported(n, f, v, r) dom_node_is_supported( \
		(dom_node *) (n), (f), (v), (bool *) (r))

static inline dom_exception dom_node_get_namespace(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_namespace(
			(dom_node_internal *) node, result);
}
#define dom_node_get_namespace(n, r) dom_node_get_namespace((dom_node *) (n), (r))

static inline dom_exception dom_node_get_prefix(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_prefix(
			(dom_node_internal *) node, result);
}
#define dom_node_get_prefix(n, r) dom_node_get_prefix((dom_node *) (n), (r))

static inline dom_exception dom_node_set_prefix(struct dom_node *node,
		dom_string *prefix)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_set_prefix(
			(dom_node_internal *) node, prefix);
}
#define dom_node_set_prefix(n, p) dom_node_set_prefix((dom_node *) (n), (p))

static inline dom_exception dom_node_get_local_name(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_local_name(
			(dom_node_internal *) node, result);
}
#define dom_node_get_local_name(n, r) dom_node_get_local_name((dom_node *) (n), (r))

static inline dom_exception dom_node_has_attributes(struct dom_node *node, 
		bool *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_has_attributes(
			(dom_node_internal *) node, result);
}
#define dom_node_has_attributes(n, r) dom_node_has_attributes( \
		(dom_node *) (n), (bool *) (r))

static inline dom_exception dom_node_get_base(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_base(
			(dom_node_internal *) node, result);
}
#define dom_node_get_base(n, r) dom_node_get_base((dom_node *) (n), (r))

static inline dom_exception dom_node_compare_document_position(
		struct dom_node *node, struct dom_node *other,
		uint16_t *result)
{
	return ((dom_node_vtable *) node->vtable)->
			dom_node_compare_document_position(
			(dom_node_internal *) node,
			(dom_node_internal *) other, result);
}
#define dom_node_compare_document_position(n, o, r) \
		dom_node_compare_document_position((dom_node *) (n), \
		(dom_node *) (o), (uint16_t *) (r))

static inline dom_exception dom_node_get_text_content(struct dom_node *node,
		dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_text_content(
			(dom_node_internal *) node, result);
}
#define dom_node_get_text_content(n, r) dom_node_get_text_content( \
		(dom_node *) (n), (r))

static inline dom_exception dom_node_set_text_content(struct dom_node *node,
		dom_string *content)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_set_text_content(
			(dom_node_internal *) node, content);
}
#define dom_node_set_text_content(n, c) dom_node_set_text_content( \
		(dom_node *) (n), (c))

static inline dom_exception dom_node_is_same(struct dom_node *node, 
		struct dom_node *other, bool *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_is_same(
			(dom_node_internal *) node,
			(dom_node_internal *) other,
			result);
}
#define dom_node_is_same(n, o, r) dom_node_is_same((dom_node *) (n), \
		(dom_node *) (o), (bool *) (r))

static inline dom_exception dom_node_lookup_prefix(struct dom_node *node,
		dom_string *ns, dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_lookup_prefix(
			(dom_node_internal *) node, ns, result);
}
#define dom_node_lookup_prefix(n, ns, r) dom_node_lookup_prefix( \
		(dom_node *) (n), (ns), (r))

static inline dom_exception dom_node_is_default_namespace(
		struct dom_node *node, dom_string *ns,
		bool *result)
{
	return ((dom_node_vtable *) node->vtable)->
			dom_node_is_default_namespace(
			(dom_node_internal *) node, ns, result);
}
#define dom_node_is_default_namespace(n, ns, r) dom_node_is_default_namespace(\
		(dom_node *) (n), (ns), (bool *) (r))

static inline dom_exception dom_node_lookup_namespace(struct dom_node *node,
		dom_string *prefix, dom_string **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_lookup_namespace(
			(dom_node_internal *) node, prefix, result);
}
#define dom_node_lookup_namespace(n, p, r) dom_node_lookup_namespace( \
		(dom_node *) (n), (p), (r))

static inline dom_exception dom_node_is_equal(struct dom_node *node,
		struct dom_node *other, bool *result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_is_equal(
			(dom_node_internal *) node,
			(dom_node_internal *) other,
			result);
}
#define dom_node_is_equal(n, o, r) dom_node_is_equal((dom_node *) (n), \
		(dom_node *) (o), (bool *) (r))

static inline dom_exception dom_node_get_feature(struct dom_node *node,
		dom_string *feature, dom_string *version,
		void **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_feature(
			(dom_node_internal *) node, feature, version, result);
}
#define dom_node_get_feature(n, f, v, r) dom_node_get_feature( \
		(dom_node *) (n), (f), (v), (void **) (r))

static inline dom_exception dom_node_set_user_data(struct dom_node *node,
		dom_string *key, void *data,
		dom_user_data_handler handler, void **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_set_user_data(
			(dom_node_internal *) node, key, data, handler,
			result);
}
#define dom_node_set_user_data(n, k, d, h, r) dom_node_set_user_data( \
		(dom_node *) (n), (k), (void *) (d), \
		(dom_user_data_handler) h, (void **) (r))

static inline dom_exception dom_node_get_user_data(struct dom_node *node,
		dom_string *key, void **result)
{
	return ((dom_node_vtable *) node->vtable)->dom_node_get_user_data(
			(dom_node_internal *) node, key, result);
}
#define dom_node_get_user_data(n, k, r) dom_node_get_user_data( \
		(dom_node *) (n), (k), (void **) (r))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_attr_h_
#define dom_core_attr_h_

struct dom_element;
struct dom_type_info;
struct dom_node;
struct dom_attr;

typedef struct dom_attr dom_attr;

/**
 * The attribute type
 */
typedef enum {
	DOM_ATTR_UNSET = 0,
	DOM_ATTR_STRING,
	DOM_ATTR_BOOL,
	DOM_ATTR_SHORT,
	DOM_ATTR_INTEGER
} dom_attr_type;

/* DOM Attr vtable */
typedef struct dom_attr_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_attr_get_name)(struct dom_attr *attr,
			dom_string **result);
	dom_exception (*dom_attr_get_specified)(struct dom_attr *attr,
			bool *result);
	dom_exception (*dom_attr_get_value)(struct dom_attr *attr,
			dom_string **result);
	dom_exception (*dom_attr_set_value)(struct dom_attr *attr,
			dom_string *value);
	dom_exception (*dom_attr_get_owner_element)(struct dom_attr *attr,
			struct dom_element **result);
	dom_exception (*dom_attr_get_schema_type_info)(struct dom_attr *attr,
			struct dom_type_info **result);
	dom_exception (*dom_attr_is_id)(struct dom_attr *attr, bool *result);
} dom_attr_vtable;

static inline dom_exception dom_attr_get_name(struct dom_attr *attr,
		dom_string **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_name(attr, result);
}
#define dom_attr_get_name(a, r) dom_attr_get_name((struct dom_attr *) (a), (r))

static inline dom_exception dom_attr_get_specified(struct dom_attr *attr,
		bool *result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_specified(attr, result);
}
#define dom_attr_get_specified(a, r) dom_attr_get_specified( \
		(struct dom_attr *) (a), (bool *) (r))

static inline dom_exception dom_attr_get_value(struct dom_attr *attr,
		dom_string **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_value(attr, result);
}
#define dom_attr_get_value(a, r) dom_attr_get_value((struct dom_attr *) (a), (r))

static inline dom_exception dom_attr_set_value(struct dom_attr *attr,
		dom_string *value)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_set_value(attr, value);
}
#define dom_attr_set_value(a, v) dom_attr_set_value((struct dom_attr *) (a), (v))

static inline dom_exception dom_attr_get_owner_element(struct dom_attr *attr,
		struct dom_element **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_owner_element(attr, result);
}
#define dom_attr_get_owner_element(a, r) dom_attr_get_owner_element(\
		(struct dom_attr *) (a), (struct dom_element **) (r))

static inline dom_exception dom_attr_get_schema_type_info(
		struct dom_attr *attr, struct dom_type_info **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_schema_type_info(attr, result);
}
#define dom_attr_get_schema_type_info(a, r) dom_attr_get_schema_type_info( \
		(struct dom_attr *) (a), (struct dom_type_info **) (r))

static inline dom_exception dom_attr_is_id(struct dom_attr *attr, bool *result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_is_id(attr, result);
}
#define dom_attr_is_id(a, r) dom_attr_is_id((struct dom_attr *) (a), \
		(bool *) (r))

/*-----------------------------------------------------------------------*/
/**
 * Following are our implementation specific APIs.
 *
 * These APIs are defined for the purpose that there are some attributes in
 * HTML and other DOM module whose type is not DOMString, but uint32_t or
 * boolean, for those types of attributes, clients should call one of the
 * following APIs to set it. 
 *
 * When an Attr node is created, its type is unset and it can be turned into
 * any of the four types. Once the type is fixed by calling any of the four
 * APIs:
 * dom_attr_set_value
 * dom_attr_set_integer
 * dom_attr_set_short
 * dom_attr_set_bool
 * it can't be modified in future. 
 *
 * For integer/short/bool type of attributes, we provide no string
 * repensentation of them, so when you call dom_attr_get_value on these
 * three type of attribute nodes, you will always get a empty dom_string.
 * If you want to do something with Attr node, you must know its type
 * firstly by calling dom_attr_get_type before you decide to call other
 * dom_attr_get_* functions.
 */
dom_attr_type dom_attr_get_type(dom_attr *a);
dom_exception dom_attr_get_integer(dom_attr *a, uint32_t *value);
dom_exception dom_attr_set_integer(dom_attr *a, uint32_t value);
dom_exception dom_attr_get_short(dom_attr *a, unsigned short *value);
dom_exception dom_attr_set_short(dom_attr *a, unsigned short value);
dom_exception dom_attr_get_bool(dom_attr *a, bool *value);
dom_exception dom_attr_set_bool(dom_attr *a, bool value);
/* Make a attribute node readonly */
void dom_attr_mark_readonly(dom_attr *a);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

#ifndef dom_core_cdatasection_h_
#define dom_core_cdatasection_h_

typedef struct dom_cdata_section dom_cdata_section;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_characterdata_h_
#define dom_core_characterdata_h_

typedef struct dom_characterdata dom_characterdata;

/* The vtable for characterdata */
typedef struct dom_characterdata_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_characterdata_get_data)(
			struct dom_characterdata *cdata,
			dom_string **data);
	dom_exception (*dom_characterdata_set_data)(
			struct dom_characterdata *cdata,
			dom_string *data);
	dom_exception (*dom_characterdata_get_length)(
			struct dom_characterdata *cdata,
			uint32_t *length);
	dom_exception (*dom_characterdata_substring_data)(
			struct dom_characterdata *cdata, uint32_t offset,
			uint32_t count, dom_string **data);
	dom_exception (*dom_characterdata_append_data)(
			struct dom_characterdata *cdata,
			dom_string *data);
	dom_exception (*dom_characterdata_insert_data)(
			struct dom_characterdata *cdata,
			uint32_t offset, dom_string *data);
	dom_exception (*dom_characterdata_delete_data)(
			struct dom_characterdata *cdata,
			uint32_t offset, uint32_t count);
	dom_exception (*dom_characterdata_replace_data)(
			struct dom_characterdata *cdata, uint32_t offset,
			uint32_t count, dom_string *data);
} dom_characterdata_vtable;


static inline dom_exception dom_characterdata_get_data(
		struct dom_characterdata *cdata, dom_string **data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_get_data(cdata, data);
}
#define dom_characterdata_get_data(c, d) dom_characterdata_get_data( \
		(struct dom_characterdata *) (c), (d))

static inline dom_exception dom_characterdata_set_data(
		struct dom_characterdata *cdata, dom_string *data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_set_data(cdata, data);
}
#define dom_characterdata_set_data(c, d) dom_characterdata_set_data( \
		(struct dom_characterdata *) (c), (d))

static inline dom_exception dom_characterdata_get_length(
		struct dom_characterdata *cdata, dom_ulong *length)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_get_length(cdata, length);
}
#define dom_characterdata_get_length(c, l) dom_characterdata_get_length( \
		(struct dom_characterdata *) (c), (uint32_t *) (l))

static inline dom_exception dom_characterdata_substring_data(
		struct dom_characterdata *cdata, uint32_t offset,
		uint32_t count, dom_string **data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_substring_data(cdata, offset, count,
			data);
}
#define dom_characterdata_substring_data(c, o, ct, d) \
		dom_characterdata_substring_data( \
		(struct dom_characterdata *) (c), (uint32_t) (o), \
		(uint32_t) (ct), (d))

static inline dom_exception dom_characterdata_append_data(
		struct dom_characterdata *cdata, dom_string *data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_append_data(cdata, data);
}
#define dom_characterdata_append_data(c, d) dom_characterdata_append_data( \
		(struct dom_characterdata *) (c), (d))

static inline dom_exception dom_characterdata_insert_data(
		struct dom_characterdata *cdata, uint32_t offset, 
		dom_string *data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_insert_data(cdata, offset, data);
}
#define dom_characterdata_insert_data(c, o, d) dom_characterdata_insert_data( \
		(struct dom_characterdata *) (c), (uint32_t) (o), (d))

static inline dom_exception dom_characterdata_delete_data(
		struct dom_characterdata *cdata, uint32_t offset, 
		uint32_t count)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_delete_data(cdata, offset, count);
}
#define dom_characterdata_delete_data(c, o, ct) dom_characterdata_delete_data(\
		(struct dom_characterdata *) (c), (uint32_t) (o), \
		(uint32_t) (ct))

static inline dom_exception dom_characterdata_replace_data(
		struct dom_characterdata *cdata, uint32_t offset, 
		uint32_t count, dom_string *data)
{
	return ((dom_characterdata_vtable *) ((dom_node *) cdata)->vtable)->
			dom_characterdata_replace_data(cdata, offset, count, 
			data);
}
#define dom_characterdata_replace_data(c, o, ct, d) \
		dom_characterdata_replace_data(\
		(struct dom_characterdata *) (c), (uint32_t) (o),\
		(uint32_t) (ct), (d))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_core_comment_h_
#define dom_core_comment_h_

typedef struct dom_comment dom_comment;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

#ifndef dom_core_documentfragment_h_
#define dom_core_documentfragment_h_

typedef struct dom_document_fragment dom_document_fragment;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_implementation_h_
#define dom_core_implementation_h_

struct dom_document;
struct dom_document_type;

typedef const char dom_implementation;

typedef enum dom_implementation_type {
	DOM_IMPLEMENTATION_CORE = 0,
	DOM_IMPLEMENTATION_XML  = (1 << 0),	/* not implemented */
	DOM_IMPLEMENTATION_HTML = (1 << 1),

	DOM_IMPLEMENTATION_ALL  = DOM_IMPLEMENTATION_CORE |
				  DOM_IMPLEMENTATION_XML  |
				  DOM_IMPLEMENTATION_HTML
} dom_implementation_type;

dom_exception dom_implementation_has_feature(
		const char *feature, const char *version,
		bool *result);

dom_exception dom_implementation_create_document_type(
		const char *qname,
		const char *public_id, const char *system_id,
		struct dom_document_type **doctype);

dom_exception dom_implementation_create_document(
		uint32_t impl_type,
		const char *ns, const char *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc);

dom_exception dom_implementation_get_feature(
		const char *feature, const char *version,
		void **object);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_document_h_
#define dom_core_document_h_

struct dom_attr;
struct dom_cdata_section;
struct dom_characterdata;
struct dom_comment;
struct dom_configuration;
struct dom_document_fragment;
struct dom_document_type;
struct dom_element;
struct dom_entity_reference;
struct dom_node;
struct dom_nodelist;
struct dom_processing_instruction;
struct dom_text;
struct lwc_string_s;

typedef struct dom_document dom_document;

/**
 * Quirks mode flag
 */
typedef enum dom_document_quirks_mode {
	DOM_DOCUMENT_QUIRKS_MODE_NONE,
	DOM_DOCUMENT_QUIRKS_MODE_LIMITED,
	DOM_DOCUMENT_QUIRKS_MODE_FULL
} dom_document_quirks_mode;


/* DOM Document vtable */
typedef struct dom_document_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_document_get_doctype)(struct dom_document *doc,
			struct dom_document_type **result);
	dom_exception (*dom_document_get_implementation)(
			struct dom_document *doc, 
			dom_implementation **result);
	dom_exception (*dom_document_get_document_element)(
			struct dom_document *doc, struct dom_element **result);
	dom_exception (*dom_document_create_element)(struct dom_document *doc,
			dom_string *tag_name, 
			struct dom_element **result);
	dom_exception (*dom_document_create_document_fragment)(
			struct dom_document *doc, 
			struct dom_document_fragment **result);
	dom_exception (*dom_document_create_text_node)(struct dom_document *doc,
			dom_string *data, struct dom_text **result);
	dom_exception (*dom_document_create_comment)(struct dom_document *doc,
			dom_string *data, struct dom_comment **result);
	dom_exception (*dom_document_create_cdata_section)(
			struct dom_document *doc, dom_string *data, 
			struct dom_cdata_section **result);
	dom_exception (*dom_document_create_processing_instruction)(
			struct dom_document *doc, dom_string *target,
			dom_string *data,
			struct dom_processing_instruction **result);
	dom_exception (*dom_document_create_attribute)(struct dom_document *doc,
			dom_string *name, struct dom_attr **result);
	dom_exception (*dom_document_create_entity_reference)(
			struct dom_document *doc, dom_string *name,
			struct dom_entity_reference **result);
	dom_exception (*dom_document_get_elements_by_tag_name)(
			struct dom_document *doc, dom_string *tagname, 
			struct dom_nodelist **result);
	dom_exception (*dom_document_import_node)(struct dom_document *doc,
			struct dom_node *node, bool deep, 
			struct dom_node **result);
	dom_exception (*dom_document_create_element_ns)(
			struct dom_document *doc, dom_string *ns,
			dom_string *qname, struct dom_element **result);
	dom_exception (*dom_document_create_attribute_ns)(
			struct dom_document *doc, dom_string *ns,
			dom_string *qname, struct dom_attr **result);
	dom_exception (*dom_document_get_elements_by_tag_name_ns)(
			struct dom_document *doc, dom_string *ns,
			dom_string *localname, 
			struct dom_nodelist **result);
	dom_exception (*dom_document_get_element_by_id)(
			struct dom_document *doc, dom_string *id, 
			struct dom_element **result);
	dom_exception (*dom_document_get_input_encoding)(
			struct dom_document *doc, dom_string **result);
	dom_exception (*dom_document_get_xml_encoding)(struct dom_document *doc,
			dom_string **result);
	dom_exception (*dom_document_get_xml_standalone)(
			struct dom_document *doc, bool *result);
	dom_exception (*dom_document_set_xml_standalone)(
			struct dom_document *doc, bool standalone);
	dom_exception (*dom_document_get_xml_version)(struct dom_document *doc,
			dom_string **result);
	dom_exception (*dom_document_set_xml_version)(struct dom_document *doc,
			dom_string *version);
	dom_exception (*dom_document_get_strict_error_checking)(
			struct dom_document *doc, bool *result);
	dom_exception (*dom_document_set_strict_error_checking)(
			struct dom_document *doc, bool strict);
	dom_exception (*dom_document_get_uri)(struct dom_document *doc,
			dom_string **result);
	dom_exception (*dom_document_set_uri)(struct dom_document *doc,
			dom_string *uri);
	dom_exception (*dom_document_adopt_node)(struct dom_document *doc,
			struct dom_node *node, struct dom_node **result);
	dom_exception (*dom_document_get_dom_config)(struct dom_document *doc,
			struct dom_configuration **result);
	dom_exception (*dom_document_normalize)(struct dom_document *doc);
	dom_exception (*dom_document_rename_node)(struct dom_document *doc,
			struct dom_node *node, dom_string *ns,
			dom_string *qname, struct dom_node **result);
	dom_exception (*get_quirks_mode)(dom_document *doc,
					 dom_document_quirks_mode *result);
	dom_exception (*set_quirks_mode)(dom_document *doc,
					 dom_document_quirks_mode quirks);
} dom_document_vtable;

static inline dom_exception dom_document_get_doctype(struct dom_document *doc,
		struct dom_document_type **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_doctype(doc, result);
}
#define dom_document_get_doctype(d, r) dom_document_get_doctype( \
		(dom_document *) (d), (struct dom_document_type **) (r))

static inline dom_exception dom_document_get_implementation(
		struct dom_document *doc, dom_implementation **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_implementation(doc, result);
}
#define dom_document_get_implementation(d, r) dom_document_get_implementation(\
		(dom_document *) (d), (dom_implementation **) (r))

static inline dom_exception dom_document_get_document_element(
		struct dom_document *doc, struct dom_element **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_document_element(doc, result);
}
#define dom_document_get_document_element(d, r) \
		dom_document_get_document_element((dom_document *) (d), \
		(struct dom_element **) (r))

static inline dom_exception dom_document_create_element(
		struct dom_document *doc, dom_string *tag_name, 
		struct dom_element **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_element(doc, tag_name, result);
}
#define dom_document_create_element(d, t, r) dom_document_create_element( \
		(dom_document *) (d), (t), \
		(struct dom_element **) (r))

static inline dom_exception dom_document_create_document_fragment(
		struct dom_document *doc, 
		struct dom_document_fragment **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_document_fragment(doc, result);
}
#define dom_document_create_document_fragment(d, r) \
		dom_document_create_document_fragment((dom_document *) (d), \
		(struct dom_document_fragment **) (r))

static inline dom_exception dom_document_create_text_node(
		struct dom_document *doc, dom_string *data, 
		struct dom_text **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_text_node(doc, data, result);
}
#define dom_document_create_text_node(d, data, r) \
		dom_document_create_text_node((dom_document *) (d), \
		 (data), (struct dom_text **) (r))

static inline dom_exception dom_document_create_comment(
		struct dom_document *doc, dom_string *data, 
		struct dom_comment **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_comment(doc, data, result);
}
#define dom_document_create_comment(d, data, r) dom_document_create_comment( \
		(dom_document *) (d), (data), \
		(struct dom_comment **) (r))

static inline dom_exception dom_document_create_cdata_section(
		struct dom_document *doc, dom_string *data, 
		struct dom_cdata_section **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_cdata_section(doc, data, result);
}
#define dom_document_create_cdata_section(d, data, r) \
		dom_document_create_cdata_section((dom_document *) (d), \
		(data), (struct dom_cdata_section **) (r))

static inline dom_exception dom_document_create_processing_instruction(
		struct dom_document *doc, dom_string *target,
		dom_string *data,
		struct dom_processing_instruction **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_processing_instruction(doc, target,
			data, result);
}
#define dom_document_create_processing_instruction(d, t, data, r) \
		dom_document_create_processing_instruction( \
		(dom_document *) (d), (t), (data), \
		(struct dom_processing_instruction **) (r))

static inline dom_exception dom_document_create_attribute(
		struct dom_document *doc, dom_string *name, 
		struct dom_attr **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_attribute(doc, name, result);
}
#define dom_document_create_attribute(d, n, r) dom_document_create_attribute( \
		(dom_document *) (d), (n), \
		(struct dom_attr **) (r))

static inline dom_exception dom_document_create_entity_reference(
		struct dom_document *doc, dom_string *name,
		struct dom_entity_reference **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_entity_reference(doc, name, 
			result);
}
#define dom_document_create_entity_reference(d, n, r) \
		dom_document_create_entity_reference((dom_document *) (d), \
		(n), (struct dom_entity_reference **) (r))

static inline dom_exception dom_document_get_elements_by_tag_name(
		struct dom_document *doc, dom_string *tagname, 
		struct dom_nodelist **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_elements_by_tag_name(doc, tagname,
			result);
}
#define dom_document_get_elements_by_tag_name(d, t, r) \
		dom_document_get_elements_by_tag_name((dom_document *) (d), \
		(t), (struct dom_nodelist **) (r))

static inline dom_exception dom_document_import_node(struct dom_document *doc,
		struct dom_node *node, bool deep, struct dom_node **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_import_node(doc, node, deep, result);
}
#define dom_document_import_node(d, n, deep, r) dom_document_import_node( \
		(dom_document *) (d), (dom_node *) (n), (bool) deep, \
		(dom_node **) (r))

static inline dom_exception dom_document_create_element_ns(
		struct dom_document *doc, dom_string *ns,
		dom_string *qname, struct dom_element **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_element_ns(doc, ns,
			qname, result);
}
#define dom_document_create_element_ns(d, n, q, r) \
		dom_document_create_element_ns((dom_document *) (d), \
		(n), (q), \
		(struct dom_element **) (r))

static inline dom_exception dom_document_create_attribute_ns
		(struct dom_document *doc, dom_string *ns,
		dom_string *qname, struct dom_attr **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_create_attribute_ns(doc, ns,
			qname, result);
}
#define dom_document_create_attribute_ns(d, n, q, r) \
		dom_document_create_attribute_ns((dom_document *) (d), \
		(n), (q), (struct dom_attr **) (r))

static inline dom_exception dom_document_get_elements_by_tag_name_ns(
		struct dom_document *doc, dom_string *ns,
		dom_string *localname, struct dom_nodelist **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_elements_by_tag_name_ns(doc,
			ns, localname, result);
}
#define dom_document_get_elements_by_tag_name_ns(d, n, l, r) \
		dom_document_get_elements_by_tag_name_ns((dom_document *) (d),\
		(n), (l), (struct dom_nodelist **) (r))

static inline dom_exception dom_document_get_element_by_id(
		struct dom_document *doc, dom_string *id, 
		struct dom_element **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_element_by_id(doc, id, result);
}
#define dom_document_get_element_by_id(d, i, r) \
		dom_document_get_element_by_id((dom_document *) (d), \
		(i), (struct dom_element **) (r))

static inline dom_exception dom_document_get_input_encoding(
		struct dom_document *doc, dom_string **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_input_encoding(doc, result);
}
#define dom_document_get_input_encoding(d, r) dom_document_get_input_encoding(\
		(dom_document *) (d), (r))

static inline dom_exception dom_document_get_xml_encoding(
		struct dom_document *doc, dom_string **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_xml_encoding(doc, result);
}
#define dom_document_get_xml_encoding(d, r) dom_document_get_xml_encoding( \
		(dom_document *) (d), (r))

static inline dom_exception dom_document_get_xml_standalone(
		struct dom_document *doc, bool *result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_xml_standalone(doc, result);
}
#define dom_document_get_xml_standalone(d, r) dom_document_get_xml_standalone(\
		(dom_document *) (d), (bool *) (r))

static inline dom_exception dom_document_set_xml_standalone(
		struct dom_document *doc, bool standalone)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_set_xml_standalone(doc, standalone);
}
#define dom_document_set_xml_standalone(d, s) dom_document_set_xml_standalone(\
		(dom_document *) (d), (bool) (s))

static inline dom_exception dom_document_get_xml_version(
		struct dom_document *doc, dom_string **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_xml_version(doc, result);
}
#define dom_document_get_xml_version(d, r) dom_document_get_xml_version( \
		(dom_document *) (d), (r))

static inline dom_exception dom_document_set_xml_version(
		struct dom_document *doc, dom_string *version)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_set_xml_version(doc, version);
}
#define dom_document_set_xml_version(d, v) dom_document_set_xml_version( \
		(dom_document *) (d), (v))

static inline dom_exception dom_document_get_strict_error_checking(
		struct dom_document *doc, bool *result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_strict_error_checking(doc, result);
}
#define dom_document_get_strict_error_checking(d, r) \
		dom_document_get_strict_error_checking((dom_document *) (d), \
		(bool *) (r))

static inline dom_exception dom_document_set_strict_error_checking(
		struct dom_document *doc, bool strict)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_set_strict_error_checking(doc, strict);
}
#define dom_document_set_strict_error_checking(d, s) \
		dom_document_set_strict_error_checking((dom_document *) (d), \
		(bool) (s))

static inline dom_exception dom_document_get_uri(struct dom_document *doc,
		dom_string **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_uri(doc, result);
}
#define dom_document_get_uri(d, r) dom_document_get_uri((dom_document *) (d), \
		(r))

static inline dom_exception dom_document_set_uri(struct dom_document *doc,
		dom_string *uri)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_set_uri(doc, uri);
}
#define dom_document_set_uri(d, u) dom_document_set_uri((dom_document *) (d), \
		(u))

static inline dom_exception dom_document_adopt_node(struct dom_document *doc,
		struct dom_node *node, struct dom_node **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_adopt_node(doc, node, result);
}
#define dom_document_adopt_node(d, n, r) dom_document_adopt_node( \
		(dom_document *) (d), (dom_node *) (n), (dom_node **) (r))

static inline dom_exception dom_document_get_dom_config(
		struct dom_document *doc, struct dom_configuration **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_get_dom_config(doc, result);
}
#define dom_document_get_dom_config(d, r) dom_document_get_dom_config( \
		(dom_document *) (d), (struct dom_configuration **) (r))

static inline dom_exception dom_document_normalize(struct dom_document *doc)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_normalize(doc);
}
#define dom_document_normalize(d) dom_document_normalize((dom_document *) (d))

static inline dom_exception dom_document_rename_node(struct dom_document *doc,
		struct dom_node *node,
		dom_string *ns, dom_string *qname,
		struct dom_node **result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
			dom_document_rename_node(doc, node, ns, qname,
			result);
}
#define dom_document_rename_node(d, n, ns, q, r) dom_document_rename_node( \
		(dom_document *) (d), (ns), \
		(q), (dom_node **) (r))

static inline dom_exception dom_document_get_quirks_mode(
	dom_document *doc, dom_document_quirks_mode *result)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
		get_quirks_mode(doc, result);
}
#define dom_document_get_quirks_mode(d, r) \
	dom_document_get_quirks_mode((dom_document *) (d), (r))

static inline dom_exception dom_document_set_quirks_mode(
	dom_document *doc, dom_document_quirks_mode quirks)
{
	return ((dom_document_vtable *) ((dom_node *) doc)->vtable)->
		set_quirks_mode(doc, quirks);
}
#define dom_document_set_quirks_mode(d, q) \
	dom_document_set_quirks_mode((dom_document *) (d), (q))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#ifndef dom_core_document_type_h_
#define dom_core_document_type_h_

struct dom_namednodemap;

typedef  struct dom_document_type dom_document_type;
/* The Dom DocumentType vtable */
typedef struct dom_document_type_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_document_type_get_name)(
			struct dom_document_type *doc_type, 
			dom_string **result);
	dom_exception (*dom_document_type_get_entities)(
			struct dom_document_type *doc_type,
			struct dom_namednodemap **result);
	dom_exception (*dom_document_type_get_notations)(
			struct dom_document_type *doc_type,
			struct dom_namednodemap **result);
	dom_exception (*dom_document_type_get_public_id)(
			struct dom_document_type *doc_type,
			dom_string **result);
	dom_exception (*dom_document_type_get_system_id)(
			struct dom_document_type *doc_type,
			dom_string **result);
	dom_exception (*dom_document_type_get_internal_subset)(
			struct dom_document_type *doc_type,
			dom_string **result);
} dom_document_type_vtable;

static inline dom_exception dom_document_type_get_name(
		struct dom_document_type *doc_type, dom_string **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_name(doc_type, result);
}
#define dom_document_type_get_name(dt, r) dom_document_type_get_name( \
		(dom_document_type *) (dt), (r))

static inline dom_exception dom_document_type_get_entities(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_entities(doc_type, result);
}
#define dom_document_type_get_entities(dt, r) dom_document_type_get_entities( \
		(dom_document_type *) (dt), (struct dom_namednodemap **) (r))

static inline dom_exception dom_document_type_get_notations(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_notations(doc_type, result);
}
#define dom_document_type_get_notations(dt, r) dom_document_type_get_notations(\
		(dom_document_type *) (dt), (struct dom_namednodemap **) (r))

static inline dom_exception dom_document_type_get_public_id(
		struct dom_document_type *doc_type,
		dom_string **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_public_id(doc_type, result);
}
#define dom_document_type_get_public_id(dt, r) \
		dom_document_type_get_public_id((dom_document_type *) (dt), \
		(r))

static inline dom_exception dom_document_type_get_system_id(
		struct dom_document_type *doc_type,
		dom_string **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_system_id(doc_type, result);
}
#define dom_document_type_get_system_id(dt, r) \
		dom_document_type_get_system_id((dom_document_type *) (dt), \
		(r))

static inline dom_exception dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		dom_string **result)
{
	return ((dom_document_type_vtable *) ((dom_node *) (doc_type))->vtable)
			->dom_document_type_get_internal_subset(doc_type,
			result);
}
#define dom_document_type_get_internal_subset(dt, r) \
		dom_document_type_get_internal_subset( \
		(dom_document_type *) (dt), (r))


#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_element_h_
#define dom_core_element_h_

struct dom_attr;
struct dom_nodelist;
struct dom_type_info;

typedef struct dom_element dom_element;

/* The DOMElement vtable */
typedef struct dom_element_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_element_get_tag_name)(struct dom_element *element,
			dom_string **name);
	dom_exception (*dom_element_get_attribute)(struct dom_element *element,
			dom_string *name, dom_string **value);
	dom_exception (*dom_element_set_attribute)(struct dom_element *element,
			dom_string *name, dom_string *value);
	dom_exception (*dom_element_remove_attribute)(
			struct dom_element *element, dom_string *name);
	dom_exception (*dom_element_get_attribute_node)(
			struct dom_element *element, dom_string *name, 
			struct dom_attr **result);
	dom_exception (*dom_element_set_attribute_node)(
			struct dom_element *element, struct dom_attr *attr, 
			struct dom_attr **result);
	dom_exception (*dom_element_remove_attribute_node)(
			struct dom_element *element, struct dom_attr *attr, 
			struct dom_attr **result);
	dom_exception (*dom_element_get_elements_by_tag_name)(
			struct dom_element *element, dom_string *name,
			struct dom_nodelist **result);
	dom_exception (*dom_element_get_attribute_ns)(
			struct dom_element *element, 
			dom_string *ns,
			dom_string *localname,
			dom_string **value);
	dom_exception (*dom_element_set_attribute_ns)(
			struct dom_element *element,
			dom_string *ns, dom_string *qname,
			dom_string *value);
	dom_exception (*dom_element_remove_attribute_ns)(
			struct dom_element *element,
			dom_string *ns,
			dom_string *localname);
	dom_exception (*dom_element_get_attribute_node_ns)(
			struct dom_element *element,
			dom_string *ns,
			dom_string *localname, struct dom_attr **result);
	dom_exception (*dom_element_set_attribute_node_ns)(
			struct dom_element *element, struct dom_attr *attr, 
			struct dom_attr **result);
	dom_exception (*dom_element_get_elements_by_tag_name_ns)(
			struct dom_element *element, 
			dom_string *ns,
			dom_string *localname, 
			struct dom_nodelist **result);
	dom_exception (*dom_element_has_attribute)(struct dom_element *element,
			dom_string *name, bool *result);
	dom_exception (*dom_element_has_attribute_ns)(
			struct dom_element *element,
			dom_string *ns,
			dom_string *localname, bool *result);
	dom_exception (*dom_element_get_schema_type_info)(
			struct dom_element *element, 
			struct dom_type_info **result);
	dom_exception (*dom_element_set_id_attribute)(
			struct dom_element *element, dom_string *name, 
			bool is_id);
	dom_exception (*dom_element_set_id_attribute_ns)(
			struct dom_element *element, 
			dom_string *ns,
			dom_string *localname, bool is_id);
	dom_exception (*dom_element_set_id_attribute_node)(
			struct dom_element *element,
			struct dom_attr *id_attr, bool is_id);

	/* These two are for the benefit of bindings to libcss */
	dom_exception (*dom_element_get_classes)(
			struct dom_element *element,
			lwc_string ***classes, uint32_t *n_classes);
	dom_exception (*dom_element_has_class)(
			struct dom_element *element,
			lwc_string *name, bool *match);
} dom_element_vtable;

static inline dom_exception dom_element_get_tag_name(
		struct dom_element *element, dom_string **name)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_tag_name(element, name);
}
#define dom_element_get_tag_name(e, n) dom_element_get_tag_name( \
		(dom_element *) (e), (n))

static inline dom_exception dom_element_get_attribute(
		struct dom_element *element, dom_string *name, 
		dom_string **value)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_attribute(element, name, value);
}
#define dom_element_get_attribute(e, n, v) dom_element_get_attribute( \
		(dom_element *) (e), (n), (v))

static inline dom_exception dom_element_set_attribute(
		struct dom_element *element, dom_string *name, 
		dom_string *value)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_attribute(element, name, value);
}
#define dom_element_set_attribute(e, n, v) dom_element_set_attribute( \
		(dom_element *) (e), (n), (v))

static inline dom_exception dom_element_remove_attribute(
		struct dom_element *element, dom_string *name)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_remove_attribute(element, name);
}
#define dom_element_remove_attribute(e, n) dom_element_remove_attribute( \
		(dom_element *) (e), (n))

static inline dom_exception dom_element_get_attribute_node(
		struct dom_element *element, dom_string *name, 
		struct dom_attr **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_attribute_node(element, name, result);
}
#define dom_element_get_attribute_node(e, n, r)  \
		dom_element_get_attribute_node((dom_element *) (e), \
		(n), (struct dom_attr **) (r))

static inline dom_exception dom_element_set_attribute_node(
		struct dom_element *element, struct dom_attr *attr, 
		struct dom_attr **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_attribute_node(element, attr, result);
}
#define dom_element_set_attribute_node(e, a, r) \
		dom_element_set_attribute_node((dom_element *) (e), \
		(struct dom_attr *) (a), (struct dom_attr **) (r))

static inline dom_exception dom_element_remove_attribute_node(
		struct dom_element *element, struct dom_attr *attr, 
		struct dom_attr **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_remove_attribute_node(element, attr, 
			result);
}
#define dom_element_remove_attribute_node(e, a, r) \
		dom_element_remove_attribute_node((dom_element *) (e), \
		(struct dom_attr *) (a), (struct dom_attr **) (r))


static inline dom_exception dom_element_get_elements_by_tag_name(
		struct dom_element *element, dom_string *name,
		struct dom_nodelist **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_elements_by_tag_name(element, name,
			result);
}
#define dom_element_get_elements_by_tag_name(e, n, r) \
		dom_element_get_elements_by_tag_name((dom_element *) (e), \
		(n), (struct dom_nodelist **) (r))

static inline dom_exception dom_element_get_attribute_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, dom_string **value)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_attribute_ns(element, ns,
			localname, value);
}
#define dom_element_get_attribute_ns(e, n, l, v) \
		dom_element_get_attribute_ns((dom_element *) (e), \
		(n), (l), (v))

static inline dom_exception dom_element_set_attribute_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *qname, dom_string *value)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_attribute_ns(element, ns,
			qname, value);
}
#define dom_element_set_attribute_ns(e, n, l, v) \
		dom_element_set_attribute_ns((dom_element *) (e), \
		(n), (l), (v))


static inline dom_exception dom_element_remove_attribute_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_remove_attribute_ns(element, ns,
			localname);
}
#define dom_element_remove_attribute_ns(e, n, l) \
		dom_element_remove_attribute_ns((dom_element *) (e), \
		(n), (l))


static inline dom_exception dom_element_get_attribute_node_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, struct dom_attr **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_attribute_node_ns(element, ns,
			localname, result);
}
#define dom_element_get_attribute_node_ns(e, n, l, r) \
		dom_element_get_attribute_node_ns((dom_element *) (e), \
		(n), (l), \
		(struct dom_attr **) (r))

static inline dom_exception dom_element_set_attribute_node_ns(
		struct dom_element *element, struct dom_attr *attr, 
		struct dom_attr **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_attribute_node_ns(element, attr,
			result);
}
#define dom_element_set_attribute_node_ns(e, a, r) \
		dom_element_set_attribute_node_ns((dom_element *) (e), \
		(struct dom_attr *) (a), (struct dom_attr **) (r))

static inline dom_exception dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, struct dom_nodelist **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_elements_by_tag_name_ns(element, 
			ns, localname, result);
}
#define dom_element_get_elements_by_tag_name_ns(e, n, l, r) \
		dom_element_get_elements_by_tag_name_ns((dom_element *) (e), \
		(n), (l), (struct dom_nodelist **) (r))

static inline dom_exception dom_element_has_attribute(
		struct dom_element *element, dom_string *name, 
		bool *result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_has_attribute(element, name, result);
}
#define dom_element_has_attribute(e, n, r) dom_element_has_attribute( \
		(dom_element *) (e), (n), (bool *) (r))

static inline dom_exception dom_element_has_attribute_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, bool *result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_has_attribute_ns(element, ns,
			localname, result);
}
#define dom_element_has_attribute_ns(e, n, l, r) dom_element_has_attribute_ns(\
		(dom_element *) (e), (n), (l), (bool *) (r))

static inline dom_exception dom_element_get_schema_type_info(
		struct dom_element *element, struct dom_type_info **result)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_schema_type_info(element, result);
}
#define dom_element_get_schema_type_info(e, r) \
		dom_element_get_schema_type_info((dom_element *) (e), \
		(struct dom_type_info **) (r))

static inline dom_exception dom_element_set_id_attribute(
		struct dom_element *element, dom_string *name, 
		bool is_id)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_id_attribute(element, name, is_id);
}
#define dom_element_set_id_attribute(e, n, i) \
		dom_element_set_id_attribute((dom_element *) (e), \
		(n), (bool) (i))

static inline dom_exception dom_element_set_id_attribute_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, bool is_id)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_id_attribute_ns(element, ns,
			localname, is_id);
}
#define dom_element_set_id_attribute_ns(e, n, l, i) \
		dom_element_set_id_attribute_ns((dom_element *) (e), \
		(n), (l), (bool) (i))

static inline dom_exception dom_element_set_id_attribute_node(
		struct dom_element *element, struct dom_attr *id_attr, 
		bool is_id)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_set_id_attribute_node(element, id_attr,
			is_id);
}
#define dom_element_set_id_attribute_node(e, a, i) \
		dom_element_set_id_attribute_node((dom_element *) (e), \
		(struct dom_attr *) (a), (bool) (i))

static inline dom_exception dom_element_get_classes(
		struct dom_element *element,
		lwc_string ***classes, uint32_t *n_classes)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_get_classes(element, classes, n_classes);
}
#define dom_element_get_classes(e, c, n) \
		dom_element_get_classes((dom_element *) (e), \
		(lwc_string ***) (c), (uint32_t *) (n))

static inline dom_exception dom_element_has_class(
		struct dom_element *element, lwc_string *name, bool *match)
{
	return ((dom_element_vtable *) ((dom_node *) element)->vtable)->
			dom_element_has_class(element, name, match);
}
#define dom_element_has_class(e, n, m) \
		dom_element_has_class((dom_element *) (e), \
		(lwc_string *) (n), (bool *) (m))


/* Functions for implementing some libcss selection callbacks.
 * Note that they don't take a reference to the returned element, as such they
 * are UNSAFE if you require the returned element to live beyond the next time
 * the DOM gets a chance to change. */
dom_exception dom_element_named_ancestor_node(dom_element *element,
		lwc_string *name, dom_element **ancestor);
dom_exception dom_element_named_parent_node(dom_element *element,
		lwc_string *name, dom_element **parent);
dom_exception dom_element_parent_node(dom_element *element,
		dom_element **parent);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_entityreference_h_
#define dom_core_entityreference_h_

typedef struct dom_entity_reference dom_entity_reference;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_namednodemap_h_
#define dom_core_namednodemap_h_

struct dom_node;

typedef struct dom_namednodemap dom_namednodemap;

void dom_namednodemap_ref(struct dom_namednodemap *map);
void dom_namednodemap_unref(struct dom_namednodemap *map);

dom_exception dom_namednodemap_get_length(struct dom_namednodemap *map,
		dom_ulong *length);

dom_exception _dom_namednodemap_get_named_item(struct dom_namednodemap *map,
		dom_string *name, struct dom_node **node);

#define dom_namednodemap_get_named_item(m, n, r)  \
		_dom_namednodemap_get_named_item((dom_namednodemap *) (m), \
		(n), (dom_node **) (r))
		

dom_exception _dom_namednodemap_set_named_item(struct dom_namednodemap *map,
		struct dom_node *arg, struct dom_node **node);

#define dom_namednodemap_set_named_item(m, a, n) \
		_dom_namednodemap_set_named_item((dom_namednodemap *) (m), \
		(dom_node *) (a), (dom_node **) (n))


dom_exception _dom_namednodemap_remove_named_item(
		struct dom_namednodemap *map, dom_string *name,
		struct dom_node **node);

#define dom_namednodemap_remove_named_item(m, n, r) \
		_dom_namednodemap_remove_named_item((dom_namednodemap *) (m), \
		(n), (dom_node **) (r))


dom_exception _dom_namednodemap_item(struct dom_namednodemap *map,
		dom_ulong index, struct dom_node **node);

#define dom_namednodemap_item(m, i, n) _dom_namednodemap_item( \
		(dom_namednodemap *) (m), (uint32_t) (i), \
		(dom_node **) (n))


dom_exception _dom_namednodemap_get_named_item_ns(
		struct dom_namednodemap *map, dom_string *ns,
		dom_string *localname, struct dom_node **node);

#define dom_namednodemap_get_named_item_ns(m, n, l, r) \
		_dom_namednodemap_get_named_item_ns((dom_namednodemap *) (m), \
		(n), (l), (dom_node **) (r))


dom_exception _dom_namednodemap_set_named_item_ns(
		struct dom_namednodemap *map, struct dom_node *arg,
		struct dom_node **node);

#define dom_namednodemap_set_named_item_ns(m, a, n) \
		_dom_namednodemap_set_named_item_ns((dom_namednodemap *) (m), \
		(dom_node *) (a), (dom_node **) (n))


dom_exception _dom_namednodemap_remove_named_item_ns(
		struct dom_namednodemap *map, dom_string *ns,
		dom_string *localname, struct dom_node **node);

#define dom_namednodemap_remove_named_item_ns(m, n, l, r) \
		_dom_namednodemap_remove_named_item_ns(\
		(dom_namednodemap *) (m), (n),(l), (dom_node **) (r))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_nodelist_h_
#define dom_core_nodelist_h_

struct dom_node;

typedef struct dom_nodelist dom_nodelist;

void dom_nodelist_ref(struct dom_nodelist *list);
void dom_nodelist_unref(struct dom_nodelist *list);

dom_exception dom_nodelist_get_length(struct dom_nodelist *list,
		uint32_t *length);
dom_exception _dom_nodelist_item(struct dom_nodelist *list,
		uint32_t index, struct dom_node **node);

#define dom_nodelist_item(l, i, n) _dom_nodelist_item((dom_nodelist *) (l), \
		(uint32_t) (i), (dom_node **) (n))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_processinginstruction_h_
#define dom_core_processinginstruction_h_

typedef struct dom_processing_instruction dom_processing_instruction;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_text_h_
#define dom_core_text_h_

struct dom_characterdata;

typedef struct dom_text dom_text;

typedef struct dom_text_vtable {
	struct dom_characterdata_vtable base;

	dom_exception (*dom_text_split_text)(struct dom_text *text,
			uint32_t offset, struct dom_text **result);
	dom_exception (*dom_text_get_is_element_content_whitespace)(
			struct dom_text *text, bool *result);
	dom_exception (*dom_text_get_whole_text)(struct dom_text *text,
			dom_string **result);
	dom_exception (*dom_text_replace_whole_text)(struct dom_text *text,
			dom_string *content, struct dom_text **result);
} dom_text_vtable;

static inline dom_exception dom_text_split_text(struct dom_text *text,
		uint32_t offset, struct dom_text **result)
{
	return ((dom_text_vtable *) ((dom_node *) text)->vtable)->
			dom_text_split_text(text, offset, result);
}
#define dom_text_split_text(t, o, r) dom_text_split_text((dom_text *) (t), \
		(uint32_t) (o), (dom_text **) (r))

static inline dom_exception dom_text_get_is_element_content_whitespace(
		struct dom_text *text, bool *result)
{
	return ((dom_text_vtable *) ((dom_node *) text)->vtable)->
			dom_text_get_is_element_content_whitespace(text, 
			result);
}
#define dom_text_get_is_element_content_whitespace(t, r) \
		dom_text_get_is_element_content_whitespace((dom_text *) (t), \
		(bool *) (r))

static inline dom_exception dom_text_get_whole_text(struct dom_text *text,
		dom_string **result)
{
	return ((dom_text_vtable *) ((dom_node *) text)->vtable)->
			dom_text_get_whole_text(text, result);
}
#define dom_text_get_whole_text(t, r) dom_text_get_whole_text((dom_text *) (t), (r))

static inline dom_exception dom_text_replace_whole_text(struct dom_text *text,
		dom_string *content, struct dom_text **result)
{
	return ((dom_text_vtable *) ((dom_node *) text)->vtable)->
			dom_text_replace_whole_text(text, content, result);
}
#define dom_text_replace_whole_text(t, c, r) dom_text_replace_whole_text( \
		(dom_text *) (t), (c), (dom_text **) (r))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_core_typeinfo_h_
#define dom_core_typeinfo_h_

typedef struct dom_type_info dom_type_info;

typedef enum {
	DOM_TYPE_INFO_DERIVATION_RESTRICTION	= 0x00000001,
	DOM_TYPE_INFO_DERIVATION_EXTENSION		= 0x00000002,
	DOM_TYPE_INFO_DERIVATION_UNION			= 0x00000004,
	DOM_TYPE_INFO_DERIVATION_LIST			= 0x00000008
} dom_type_info_derivation_method;

dom_exception _dom_type_info_get_type_name(dom_type_info *ti, 
		dom_string **ret);
#define dom_type_info_get_type_name(t, r) _dom_type_info_get_type_name( \
		(dom_type_info *) (t), (r))


dom_exception _dom_type_info_get_type_namespace(dom_type_info *ti,
		dom_string **ret);
#define dom_type_info_get_type_namespace(t, r) \
		_dom_type_info_get_type_namespace((dom_type_info *) (t), (r))


dom_exception _dom_type_info_is_derived(dom_type_info *ti,
		dom_string *ns, dom_string *name,
		dom_type_info_derivation_method method, bool *ret);
#define dom_type_info_is_derived(t, s, n, m, r)  _dom_type_info_is_derived(\
		(dom_type_info *) (t), (s), (n), \
		(dom_type_info_derivation_method) (m), (bool *) (r))


#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_xmlerror_h_
#define xml_xmlerror_h_

typedef enum {
	DOM_XML_OK              = 0,

	DOM_XML_NOMEM           = 1,

	DOM_XML_EXTERNAL_ERR      = (1<<16),
} dom_xml_error;

#endif


/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_xmlparser_h_
#define xml_xmlparser_h_

typedef struct dom_xml_parser dom_xml_parser;

/* Create an XML parser instance */
dom_xml_parser *dom_xml_parser_create(const char *enc, const char *int_enc,
		dom_msg msg, void *mctx, dom_document **document);

/* Destroy an XML parser instance */
void dom_xml_parser_destroy(dom_xml_parser *parser);

/* Parse a chunk of data */
dom_xml_error dom_xml_parser_parse_chunk(dom_xml_parser *parser,
		uint8_t *data, size_t len);

/* Notify parser that datastream is empty */
dom_xml_error dom_xml_parser_completed(dom_xml_parser *parser);

#endif

#endif // dom_all_h_
