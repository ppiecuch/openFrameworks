/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_utils_all_h_
#define dom_utils_all_h_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <dom/dom.h>

#ifndef dom_utils_namespace_h_
#define dom_utils_namespace_h_

struct dom_document;

/* Ensure a QName is valid */
dom_exception _dom_namespace_validate_qname(dom_string *qname,
		dom_string *ns);

/* Split a QName into a namespace prefix and localname string */
dom_exception _dom_namespace_split_qname(dom_string *qname,
		dom_string **prefix, dom_string **localname);

/* Get the XML prefix dom_string */
dom_string *_dom_namespace_get_xml_prefix(void);

/* Get the XMLNS prefix dom_string */
dom_string *_dom_namespace_get_xmlns_prefix(void);

#endif

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 *
 * This file contains the API used to validate whether certain character in
 * name/value is legal according the XML 1.0 standard. See 
 *
 * http://www.w3.org/TR/2004/REC-xml-20040204/
 * http://www.w3.org/TR/REC-xml/
 * 
 * for detail.
 */

#ifndef dom_utils_character_valid_h_
#define dom_utils_character_valid_h_

struct xml_char_range {
	unsigned int start;
	unsigned int end;
};

struct xml_char_group {
	size_t len;
	const struct xml_char_range *range;
};

/* The groups */
extern const struct xml_char_group base_char_group;
extern const struct xml_char_group char_group;
extern const struct xml_char_group combining_char_group;
extern const struct xml_char_group digit_char_group;
extern const struct xml_char_group extender_group;
extern const struct xml_char_group ideographic_group;

bool _dom_is_character_in_group(unsigned int ch,
		const struct xml_char_group *group);

#define is_base_char(ch) _dom_is_character_in_group((ch), &base_char_group)
#define is_char(ch) _dom_is_character_in_group((ch), &char_group)
#define is_combining_char(ch) _dom_is_character_in_group((ch), \
		&combining_char_group)
#define is_digit(ch) _dom_is_character_in_group((ch), &digit_char_group)
#define is_extender(ch) _dom_is_character_in_group((ch), &extender_group)
#define is_ideographic(ch) _dom_is_character_in_group((ch), &ideographic_group)

#define is_letter(ch)  (is_base_char(ch) || is_ideographic(ch))

#endif

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2006 Rob Kendrick <rjek@rjek.com>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_utils_hashtable_h_
#define dom_utils_hashtable_h_

typedef struct dom_hash_table dom_hash_table;

typedef struct dom_hash_vtable {
	uint32_t (*hash)(void *key, void *pw);
	void *(*clone_key)(void *key, void *pw);
	void (*destroy_key)(void *key, void *pw);
	void *(*clone_value)(void *value, void *pw);
	void (*destroy_value)(void *value, void *pw);
	bool (*key_isequal)(void *key1, void *key2, void *pw);
} dom_hash_vtable;

dom_hash_table *_dom_hash_create(unsigned int chains, 
		const dom_hash_vtable *vtable, void *pw);
dom_hash_table *_dom_hash_clone(dom_hash_table *ht);
void _dom_hash_destroy(dom_hash_table *ht);
bool _dom_hash_add(dom_hash_table *ht, void *key, void *value, 
		bool replace);
void *_dom_hash_get(dom_hash_table *ht, void *key);
void *_dom_hash_del(dom_hash_table *ht, void *key);
void *_dom_hash_iterate(dom_hash_table *ht, uintptr_t *c1, uintptr_t **c2);
uint32_t _dom_hash_get_length(dom_hash_table *ht);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 *
 * This file contains the list structure used to compose lists. 
 * 
 * Note: This is a implementation of a doubld-linked cyclar list.
 */

#ifndef dom_utils_list_h_
#define dom_utils_list_h_

struct list_entry {
	struct list_entry *prev;
	struct list_entry *next;
};

/**
 * Initialise a list_entry structure
 *
 * \param ent  The entry to initialise
 */
static inline void list_init(struct list_entry *ent)
{
	ent->prev = ent;
	ent->next = ent;
}

/**
 * Append a new list_entry after the list
 *
 * \param head  The list header
 * \param ent   The new entry
 */
static inline void list_append(struct list_entry *head, struct list_entry *ent)
{
	ent->next = head;
	ent->prev = head->prev;
	head->prev->next = ent;
	head->prev = ent;
}

/**
 * Delete a list_entry from the list
 *
 * \param entry  The entry need to be deleted from the list
 */
static inline void list_del(struct list_entry *ent)
{
	ent->prev->next = ent->next;
	ent->next->prev = ent->prev;

	ent->prev = ent;
	ent->next = ent;
}

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_utils_utils_h_
#define dom_utils_utils_h_

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

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 *
 * This file contains the API used to validate whether certain element's 
 * name/namespace are legal according the XML 1.0 standard. See 
 *
 * http://www.w3.org/TR/2004/REC-xml-20040204/
 * 
 * for detail.
 */

#ifndef  dom_utils_valid_h_
#define  dom_utils_valid_h_

bool _dom_validate_name(dom_string *name);
bool _dom_validate_ncname(dom_string *name);

#endif

#endif // dom_utils_all_h_
