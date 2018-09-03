/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_attr_h_
#define dom_internal_core_attr_h_

#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

#include <dom/dom.h>
#include <libwapcaplet/libwapcaplet.h>

#include "utils_p.h"

struct dom_document;
struct dom_type_info;

dom_exception _dom_attr_create(struct dom_document *doc,
		dom_string *name, dom_string *ns,
		dom_string *prefix, bool specified,
		struct dom_attr **result);
void _dom_attr_destroy(struct dom_attr *attr);
dom_exception _dom_attr_initialise(struct dom_attr *a,
		struct dom_document *doc, dom_string *name,
		dom_string *ns, dom_string *prefix,
		bool specified, struct dom_attr **result);
void _dom_attr_finalise(struct dom_attr *attr);

/* Virtual functions for dom_attr */
dom_exception _dom_attr_get_name(struct dom_attr *attr,
				dom_string **result);
dom_exception _dom_attr_get_specified(struct dom_attr *attr, bool *result);
dom_exception _dom_attr_get_value(struct dom_attr *attr,
				dom_string **result);
dom_exception _dom_attr_set_value(struct dom_attr *attr,
				dom_string *value);
dom_exception _dom_attr_get_owner(struct dom_attr *attr,
				struct dom_element **result);
dom_exception _dom_attr_get_schema_type_info(struct dom_attr *attr,
				struct dom_type_info **result);
dom_exception _dom_attr_is_id(struct dom_attr *attr, bool *result);

#define DOM_ATTR_VTABLE 	\
	_dom_attr_get_name, \
	_dom_attr_get_specified, \
	_dom_attr_get_value, \
	_dom_attr_set_value, \
	_dom_attr_get_owner, \
	_dom_attr_get_schema_type_info, \
	_dom_attr_is_id

/* Overloading dom_node functions */
dom_exception _dom_attr_get_node_value(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_attr_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result);
dom_exception _dom_attr_set_prefix(dom_node_internal *node,
		dom_string *prefix);
dom_exception _dom_attr_normalize(dom_node_internal *node);
dom_exception _dom_attr_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result);
dom_exception _dom_attr_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result);
dom_exception _dom_attr_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result);
#define DOM_NODE_VTABLE_ATTR \
	_dom_node_try_destroy, \
	_dom_node_get_node_name, \
	_dom_attr_get_node_value, /*overload*/\
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_node_get_attributes, \
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_attr_clone_node, /*overload*/\
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_attr_set_prefix, /*overload*/\
	_dom_node_get_local_name, \
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_node_get_text_content, \
	_dom_node_set_text_content, \
	_dom_node_is_same, \
	_dom_attr_lookup_prefix, /*overload*/\
	_dom_attr_is_default_namespace, /*overload*/\
	_dom_attr_lookup_namespace, /*overload*/\
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data

/* The protected virtual functions */
void __dom_attr_destroy(dom_node_internal *node);
dom_exception _dom_attr_copy(dom_node_internal *old,
		dom_node_internal **copy);

#define DOM_ATTR_PROTECT_VTABLE \
	__dom_attr_destroy, \
	_dom_attr_copy


void _dom_attr_set_isid(struct dom_attr *attr, bool is_id);
void _dom_attr_set_specified(struct dom_attr *attr, bool specified);
bool _dom_attr_readonly(const dom_attr *a);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_cdatasection_h_
#define dom_internal_core_cdatasection_h_

struct dom_node_internal;
struct dom_document;

dom_exception _dom_cdata_section_create(struct dom_document *doc,
		dom_string *name, dom_string *value,
		dom_cdata_section **result);

void _dom_cdata_section_destroy(dom_cdata_section *cdata);

#define _dom_cdata_section_initialise 	_dom_text_initialise
#define _dom_cdata_section_finalise	_dom_text_finalise

/* Following comes the protected vtable  */
void __dom_cdata_section_destroy(struct dom_node_internal *node);
dom_exception _dom_cdata_section_copy(struct dom_node_internal *old, 
		struct dom_node_internal **copy);

#define DOM_CDATA_SECTION_PROTECT_VTABLE \
	__dom_cdata_section_destroy, \
	_dom_cdata_section_copy

#endif

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_internal_core_string_h_
#define dom_internal_core_string_h_

/* Map the lwc_error to dom_exception */
dom_exception _dom_exception_from_lwc_error(lwc_error err);

enum dom_whitespace_op {
	DOM_WHITESPACE_STRIP_LEADING	= (1 << 0),
	DOM_WHITESPACE_STRIP_TRAILING	= (1 << 1),
	DOM_WHITESPACE_STRIP		= DOM_WHITESPACE_STRIP_LEADING |
					  DOM_WHITESPACE_STRIP_TRAILING,
	DOM_WHITESPACE_COLLAPSE		= (1 << 2),
	DOM_WHITESPACE_STRIP_COLLAPSE	= DOM_WHITESPACE_STRIP |
					  DOM_WHITESPACE_COLLAPSE
};

/** Perform whitespace operations on given string
 *
 * \param s	Given string
 * \param op	Whitespace operation(s) to perform
 * \param ret	New string with whitespace ops performed.  Caller owns ref
 *
 * \return DOM_NO_ERR on success.
 *
 * \note Right now, will return DOM_NOT_SUPPORTED_ERR if ascii_only is false.
 */
dom_exception dom_string_whitespace_op(dom_string *s,
		enum dom_whitespace_op op, dom_string **ret);

#endif

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_node_h_
#define dom_internal_core_node_h_

/**
 * User data context attached to a DOM node
 */
struct dom_user_data {
	dom_string *key;		/**< Key for data */
	void *data;			/**< Client-specific data */
	dom_user_data_handler handler;	/**< Callback function */

	struct dom_user_data *next;	/**< Next in list */
	struct dom_user_data *prev;	/**< Previous in list */
};
typedef struct dom_user_data dom_user_data;

/**
 * The internally used virtual function table.
 */
typedef struct dom_node_protect_vtable {

	void (*destroy)(dom_node_internal *n);
					/**< The destroy virtual function, it 
					 * should be private to client */
	dom_exception (*copy)(dom_node_internal *old, dom_node_internal **copy);
				/**< Copy the old to new as well as 
				 * all its attributes, but not its children */
} dom_node_protect_vtable; 

/**
 * The real DOM node object
 *
 * DOM nodes are reference counted
 */
struct dom_node_internal {
	struct dom_node base;		/**< The vtable base */
	void *vtable;			/**< The protected vtable */

	dom_string *name;		/**< Node name (this is the local part
		 			 * of a QName in the cases where a
		 			 * namespace exists) */
	dom_string *value;		/**< Node value */
	dom_node_type type;		/**< Node type */
	dom_node_internal *parent;	/**< Parent node */
	dom_node_internal *first_child;	/**< First child node */
	dom_node_internal *last_child;	/**< Last child node */
	dom_node_internal *previous;	/**< Previous sibling */
	dom_node_internal *next;		/**< Next sibling */

	struct dom_document *owner;	/**< Owning document */

	dom_string *ns;		/**< Namespace URI */
	dom_string *prefix;		/**< Namespace prefix */

	struct dom_user_data *user_data;	/**< User data list */

	struct list_entry pending_list; /**< The document delete pending list */
};

dom_node_internal * _dom_node_create(void);

dom_exception _dom_node_initialise(struct dom_node_internal *node,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value,
		dom_string *ns, dom_string *prefix);

void _dom_node_finalise(dom_node_internal *node);

bool _dom_node_readonly(const dom_node_internal *node);

/* The DOM Node's vtable methods */
dom_exception _dom_node_get_node_name(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_get_node_value(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_set_node_value(dom_node_internal *node,
		dom_string *value);
dom_exception _dom_node_get_node_type(dom_node_internal *node,
		dom_node_type *result);
dom_exception _dom_node_get_parent_node(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_child_nodes(dom_node_internal *node,
		struct dom_nodelist **result);
dom_exception _dom_node_get_first_child(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_last_child(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_previous_sibling(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_next_sibling(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result);
dom_exception _dom_node_get_owner_document(dom_node_internal *node,
		struct dom_document **result);
dom_exception _dom_node_insert_before(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *ref_child,
		dom_node_internal **result);
dom_exception _dom_node_replace_child(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *old_child,
		dom_node_internal **result);
dom_exception _dom_node_remove_child(dom_node_internal *node,
		dom_node_internal *old_child,
		dom_node_internal **result);
dom_exception _dom_node_append_child(dom_node_internal *node,
		dom_node_internal *new_child,
		dom_node_internal **result);
dom_exception _dom_node_has_child_nodes(dom_node_internal *node, bool *result);
dom_exception _dom_node_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result);
dom_exception _dom_node_normalize(dom_node_internal *node);
dom_exception _dom_node_is_supported(dom_node_internal *node,
		dom_string *feature, dom_string *version,
		bool *result);
dom_exception _dom_node_get_namespace(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_get_prefix(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_set_prefix(dom_node_internal *node,
		dom_string *prefix);
dom_exception _dom_node_get_local_name(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_has_attributes(dom_node_internal *node, bool *result);
dom_exception _dom_node_get_base(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_compare_document_position(dom_node_internal *node,
		dom_node_internal *other, uint16_t *result);
dom_exception _dom_node_get_text_content(dom_node_internal *node,
		dom_string **result);
dom_exception _dom_node_set_text_content(dom_node_internal *node,
		dom_string *content);
dom_exception _dom_node_is_same(dom_node_internal *node,
		dom_node_internal *other, bool *result);
dom_exception _dom_node_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result);
dom_exception _dom_node_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result);
dom_exception _dom_node_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result);
dom_exception _dom_node_is_equal(dom_node_internal *node,
		dom_node_internal *other, bool *result);
dom_exception _dom_node_get_feature(dom_node_internal *node,
		dom_string *feature, dom_string *version,
		void **result);
dom_exception _dom_node_set_user_data(dom_node_internal *node,
		dom_string *key, void *data,
		dom_user_data_handler handler, void **result);
dom_exception _dom_node_get_user_data(dom_node_internal *node,
		dom_string *key, void **result);

#define DOM_NODE_VTABLE \
	_dom_node_try_destroy, \
	_dom_node_get_node_name,	  \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_node_get_attributes, \
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_node_get_text_content, \
	_dom_node_set_text_content, \
	_dom_node_is_same, \
	_dom_node_lookup_prefix, \
	_dom_node_is_default_namespace, \
	_dom_node_lookup_namespace, \
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data


/* Following comes the protected vtable */
void _dom_node_destroy(struct dom_node_internal *node);
dom_exception _dom_node_copy(struct dom_node_internal *old, 
		struct dom_node_internal **copy);

#define DOM_NODE_PROTECT_VTABLE \
	_dom_node_destroy, \
	_dom_node_copy


/* The destroy API should be used inside DOM module */
static inline void dom_node_destroy(struct dom_node_internal *node)
{
	((dom_node_protect_vtable *) node->vtable)->destroy(node);
}
#define dom_node_destroy(n) dom_node_destroy((dom_node_internal *) (n))

/* Copy the Node old to new */
static inline dom_exception dom_node_copy(struct dom_node_internal *old, 
		struct dom_node_internal **copy)
{
	return ((dom_node_protect_vtable *) old->vtable)->copy(old, copy);
}
#define dom_node_copy(o,c) dom_node_copy((dom_node_internal *) (o), \
		(dom_node_internal **) (c))

/* Following are some helper functions */
dom_exception _dom_node_copy_internal(dom_node_internal *old, 
		dom_node_internal *nod);
#define dom_node_copy_internal(o, n) _dom_node_copy_internal( \
		(dom_node_internal *) (o), (dom_node_internal *) (n))

#define dom_node_get_owner(n) ((dom_node_internal *) (n))->owner

#define dom_node_set_owner(n, d) ((dom_node_internal *) (n))->owner = \
		(struct dom_document *) (d)

#define dom_node_get_parent(n) ((dom_node_internal *) (n))->parent

#define dom_node_set_parent(n, p) ((dom_node_internal *) (n))->parent = \
		(dom_node_internal *) (p)

#define dom_node_get_refcount(n) ((dom_node_internal *) (n))->refcnt

dom_exception _dom_merge_adjacent_text(dom_node_internal *p,
		dom_node_internal *n);

/* Try to destroy the node, if its refcnt is not zero, then append it to the
 * owner document's pending list */
dom_exception _dom_node_try_destroy(dom_node_internal *node);

/* To add some node to the pending list */
void _dom_node_mark_pending(dom_node_internal *node);
#define dom_node_mark_pending(n) _dom_node_mark_pending(\
		(dom_node_internal *) (n))
/* To remove the node from the pending list, this may happen when
 * a node is removed and then appended to another parent */
void _dom_node_remove_pending(dom_node_internal *node);
#define dom_node_remove_pending(n) _dom_node_remove_pending(\
		(dom_node_internal *) (n))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_nodelist_h_
#define dom_internal_core_nodelist_h_

struct dom_document;
struct dom_node;
struct dom_nodelist;

/**
 * The NodeList type
 */
typedef enum { 
	DOM_NODELIST_CHILDREN,
	DOM_NODELIST_BY_NAME,
	DOM_NODELIST_BY_NAMESPACE,
	DOM_NODELIST_BY_NAME_CASELESS,
	DOM_NODELIST_BY_NAMESPACE_CASELESS
} nodelist_type;

/* Create a nodelist */
dom_exception _dom_nodelist_create(struct dom_document *doc, nodelist_type type,
		struct dom_node_internal *root, dom_string *tagname,
		dom_string *ns, dom_string *localname,
		struct dom_nodelist **list);

/* Match a nodelist instance against a set of nodelist creation parameters */
bool _dom_nodelist_match(struct dom_nodelist *list, nodelist_type type,
		struct dom_node_internal *root, dom_string *tagname, 
		dom_string *ns, dom_string *localname);

bool _dom_nodelist_equal(struct dom_nodelist *l1, struct dom_nodelist *l2);
#define dom_nodelist_equal(l1, l2) _dom_nodelist_equal( \
		(struct dom_nodelist *) (l1), (struct dom_nodelist *) (l2))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_characterdata_h_
#define dom_internal_core_characterdata_h_

/**
 * DOM character data node
 */
struct dom_characterdata {
	struct dom_node_internal base;		/**< Base node */
};

/* The CharacterData is a intermediate node type, so the following function
 * may never be used */
dom_characterdata *_dom_characterdata_create(void);
dom_exception _dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value);

void _dom_characterdata_finalise(struct dom_characterdata *cdata);

/* The virtual functions for dom_characterdata */
dom_exception _dom_characterdata_get_data(struct dom_characterdata *cdata,
		dom_string **data);
dom_exception _dom_characterdata_set_data(struct dom_characterdata *cdata,
		dom_string *data);
dom_exception _dom_characterdata_get_length(struct dom_characterdata *cdata,
		uint32_t *length);
dom_exception _dom_characterdata_substring_data(
		struct dom_characterdata *cdata, uint32_t offset,
		uint32_t count, dom_string **data);
dom_exception _dom_characterdata_append_data(struct dom_characterdata *cdata,
		dom_string *data);
dom_exception _dom_characterdata_insert_data(struct dom_characterdata *cdata,
		uint32_t offset, dom_string *data);
dom_exception _dom_characterdata_delete_data(struct dom_characterdata *cdata,
		uint32_t offset, uint32_t count);
dom_exception _dom_characterdata_replace_data(struct dom_characterdata *cdata,
		uint32_t offset, uint32_t count,
		dom_string *data);
dom_exception _dom_characterdata_get_text_content(
		dom_node_internal *node,
		dom_string **result);
dom_exception _dom_characterdata_set_text_content(
		dom_node_internal *node,
		dom_string *content);

#define DOM_CHARACTERDATA_VTABLE \
	_dom_characterdata_get_data, \
	_dom_characterdata_set_data, \
	_dom_characterdata_get_length, \
	_dom_characterdata_substring_data, \
	_dom_characterdata_append_data, \
	_dom_characterdata_insert_data, \
	_dom_characterdata_delete_data, \
	_dom_characterdata_replace_data 

#define DOM_NODE_VTABLE_CHARACTERDATA \
	_dom_node_try_destroy, \
	_dom_node_get_node_name, \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_node_get_attributes, \
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_characterdata_get_text_content, /* override */ \
	_dom_characterdata_set_text_content, /* override */ \
	_dom_node_is_same, \
	_dom_node_lookup_prefix, \
	_dom_node_is_default_namespace, \
	_dom_node_lookup_namespace, \
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data

/* Following comes the protected vtable 
 *
 * Only the _copy function can be used by sub-class of this.
 */
void _dom_characterdata_destroy(dom_node_internal *node);
dom_exception _dom_characterdata_copy(dom_node_internal *old, 
		dom_node_internal **copy);

#define DOM_CHARACTERDATA_PROTECT_VTABLE \
	_dom_characterdata_destroy, \
	_dom_characterdata_copy

extern struct dom_characterdata_vtable characterdata_vtable;

dom_exception _dom_characterdata_copy_internal(dom_characterdata *old, 
		dom_characterdata *dat);
#define dom_characterdata_copy_internal(o, n) \
		_dom_characterdata_copy_internal( \
		(dom_characterdata *) (o), (dom_characterdata *) (n))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_comment_h_
#define dom_internal_core_comment_h_

struct dom_comment;
struct dom_document;

dom_exception _dom_comment_create(struct dom_document *doc,
		dom_string *name, dom_string *value,
		dom_comment **result);

#define  _dom_comment_initialise _dom_characterdata_initialise
#define  _dom_comment_finalise _dom_characterdata_finalise

void _dom_comment_destroy(dom_comment *comment);

/* Following comes the protected vtable  */
void __dom_comment_destroy(dom_node_internal *node);
dom_exception _dom_comment_copy(dom_node_internal *old, 
		dom_node_internal **copy);

#define DOM_COMMENT_PROTECT_VTABLE \
	__dom_comment_destroy, \
	_dom_comment_copy

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_documentfragment_h_
#define dom_internal_core_documentfragment_h_

dom_exception _dom_document_fragment_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_document_fragment **result);

void _dom_document_fragment_destroy(dom_document_fragment *frag);

#define _dom_document_fragment_initialise	_dom_node_initialise
#define _dom_document_fragment_finalise		_dom_node_finalise


/* Following comes the protected vtable */
void _dom_df_destroy(dom_node_internal *node);
dom_exception _dom_df_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_DF_PROTECT_VTABLE \
	_dom_df_destroy, \
	_dom_df_copy

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_h_
#define dom_internal_core_document_h_

struct dom_doc_nl;

/**
 * DOM document
 * This should be protected, because later the HTMLDocument will inherit from
 * this. 
 */
struct dom_document {
	dom_node_internal base;		/**< Base node */

	struct dom_doc_nl *nodelists;	/**< List of active nodelists */

	dom_string *uri;		/**< The uri of this document */

	struct list_entry pending_nodes;
			/**< The deletion pending list */

	dom_string *id_name;		/**< The ID attribute's name */

	dom_string *class_string;	/**< The string "class". */

	dom_string *script_string;	/**< The string "script". */

	dom_document_quirks_mode quirks;
				/**< Document is in quirks mode */
	dom_string *_memo_empty;	/**< The string ''. */

	/* Memoised event strings */
	dom_string *_memo_domnodeinserted; /**< DOMNodeInserted */
	dom_string *_memo_domnoderemoved;  /**< DOMNodeRemoved */
	dom_string *_memo_domnodeinsertedintodocument; /**< DOMNodeInsertedIntoDocument */
	dom_string *_memo_domnoderemovedfromdocument;  /**< DOMNodeRemovedFromDocument */
	dom_string *_memo_domattrmodified; /**< DOMAttrModified */
	dom_string *_memo_domcharacterdatamodified; /**< DOMCharacterDataModified */
	dom_string *_memo_domsubtreemodified; /**< DOMSubtreeModified */
};

/* Create a DOM document */
dom_exception _dom_document_create(dom_document **doc);

/* Initialise the document */
dom_exception _dom_document_initialise(dom_document *doc);

/* Finalise the document */
bool _dom_document_finalise(dom_document *doc);

/* Begin the virtual functions */
dom_exception _dom_document_get_doctype(dom_document *doc,
		dom_document_type **result);
dom_exception _dom_document_get_implementation(dom_document *doc,
		dom_implementation **result);
dom_exception _dom_document_get_document_element(dom_document *doc,
		dom_element **result);
dom_exception _dom_document_create_element(dom_document *doc,
		dom_string *tag_name, dom_element **result);
dom_exception _dom_document_create_document_fragment(dom_document *doc,
		dom_document_fragment **result);
dom_exception _dom_document_create_text_node(dom_document *doc,
		dom_string *data, dom_text **result);
dom_exception _dom_document_create_comment(dom_document *doc,
		dom_string *data, dom_comment **result);
dom_exception _dom_document_create_cdata_section(dom_document *doc,
		dom_string *data, dom_cdata_section **result);
dom_exception _dom_document_create_processing_instruction(
		dom_document *doc, dom_string *target,
		dom_string *data,
		dom_processing_instruction **result);
dom_exception _dom_document_create_attribute(dom_document *doc,
		dom_string *name, dom_attr **result);
dom_exception _dom_document_create_entity_reference(dom_document *doc,
		dom_string *name,
		dom_entity_reference **result);
dom_exception _dom_document_get_elements_by_tag_name(dom_document *doc,
		dom_string *tagname, dom_nodelist **result);
dom_exception _dom_document_import_node(dom_document *doc,
		dom_node *node, bool deep, dom_node **result);
dom_exception _dom_document_create_element_ns(dom_document *doc,
		dom_string *ns, dom_string *qname,
		dom_element **result);
dom_exception _dom_document_create_attribute_ns(dom_document *doc,
		dom_string *ns, dom_string *qname,
		dom_attr **result);
dom_exception _dom_document_get_elements_by_tag_name_ns(
		dom_document *doc, dom_string *ns,
		dom_string *localname, dom_nodelist **result);
dom_exception _dom_document_get_element_by_id(dom_document *doc,
		dom_string *id, dom_element **result);
dom_exception _dom_document_get_input_encoding(dom_document *doc,
		dom_string **result);
dom_exception _dom_document_get_xml_encoding(dom_document *doc,
		dom_string **result);
dom_exception _dom_document_get_xml_standalone(dom_document *doc,
		bool *result);
dom_exception _dom_document_set_xml_standalone(dom_document *doc,
		bool standalone);
dom_exception _dom_document_get_xml_version(dom_document *doc,
		dom_string **result);
dom_exception _dom_document_set_xml_version(dom_document *doc,
		dom_string *version);
dom_exception _dom_document_get_strict_error_checking(
		dom_document *doc, bool *result);
dom_exception _dom_document_set_strict_error_checking(
		dom_document *doc, bool strict);
dom_exception _dom_document_get_uri(dom_document *doc,
		dom_string **result);
dom_exception _dom_document_set_uri(dom_document *doc,
		dom_string *uri);
dom_exception _dom_document_adopt_node(dom_document *doc,
		dom_node *node, dom_node **result);
dom_exception _dom_document_get_dom_config(dom_document *doc,
		struct dom_configuration **result);
dom_exception _dom_document_normalize(dom_document *doc);
dom_exception _dom_document_rename_node(dom_document *doc,
		dom_node *node,
		dom_string *ns, dom_string *qname,
		dom_node **result);
dom_exception _dom_document_get_quirks_mode(dom_document *doc,
		dom_document_quirks_mode *result);
dom_exception _dom_document_set_quirks_mode(dom_document *doc,
		dom_document_quirks_mode result);


dom_exception _dom_document_get_text_content(dom_node_internal *node,
					     dom_string **result);
dom_exception _dom_document_set_text_content(dom_node_internal *node,
					     dom_string *content);

#define DOM_DOCUMENT_VTABLE \
	_dom_document_get_doctype, \
	_dom_document_get_implementation, \
	_dom_document_get_document_element, \
	_dom_document_create_element, \
	_dom_document_create_document_fragment, \
	_dom_document_create_text_node, \
	_dom_document_create_comment, \
	_dom_document_create_cdata_section, \
	_dom_document_create_processing_instruction, \
	_dom_document_create_attribute, \
	_dom_document_create_entity_reference, \
	_dom_document_get_elements_by_tag_name, \
	_dom_document_import_node, \
	_dom_document_create_element_ns, \
	_dom_document_create_attribute_ns, \
	_dom_document_get_elements_by_tag_name_ns, \
	_dom_document_get_element_by_id, \
	_dom_document_get_input_encoding, \
	_dom_document_get_xml_encoding, \
	_dom_document_get_xml_standalone, \
	_dom_document_set_xml_standalone, \
	_dom_document_get_xml_version, \
	_dom_document_set_xml_version, \
	_dom_document_get_strict_error_checking, \
	_dom_document_set_strict_error_checking, \
	_dom_document_get_uri, \
	_dom_document_set_uri, \
	_dom_document_adopt_node, \
	_dom_document_get_dom_config, \
	_dom_document_normalize, \
	_dom_document_rename_node, \
	_dom_document_get_quirks_mode, \
	_dom_document_set_quirks_mode

/* End of vtable */

#define DOM_NODE_VTABLE_DOCUMENT \
	_dom_node_try_destroy, \
	_dom_node_get_node_name, \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_node_get_attributes, \
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_document_get_text_content, \
	_dom_document_set_text_content, \
	_dom_node_is_same, \
	_dom_node_lookup_prefix, \
	_dom_node_is_default_namespace, \
	_dom_node_lookup_namespace, \
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data

/* Following comes the protected vtable  */
void _dom_document_destroy(dom_node_internal *node);
dom_exception _dom_document_copy(dom_node_internal *old, 
		dom_node_internal **copy);

#define DOM_DOCUMENT_PROTECT_VTABLE \
	_dom_document_destroy, \
	_dom_document_copy


/*---------------------------- Helper functions ---------------------------*/

/* Try to destroy the document:
 * When the refcnt is zero and the pending list is empty, we can destroy this
 * document. */
void _dom_document_try_destroy(dom_document *doc);

/* Get a nodelist, creating one if necessary */
dom_exception _dom_document_get_nodelist(dom_document *doc,
		nodelist_type type, dom_node_internal *root,
		dom_string *tagname, dom_string *ns,
		dom_string *localname, dom_nodelist **list);
/* Remove a nodelist */
void _dom_document_remove_nodelist(dom_document *doc, dom_nodelist *list);

/* Find element with certain ID in the subtree rooted at root */
dom_exception _dom_find_element_by_id(dom_node_internal *root, 
		dom_string *id, dom_element **result);

/* Set the ID attribute name of this document */
void _dom_document_set_id_name(dom_document *doc, dom_string *name);

#define _dom_document_get_id_name(d) (d->id_name)

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_type_h_
#define dom_internal_core_document_type_h_

struct dom_namednodemap;

/* Create a DOM document type */
dom_exception _dom_document_type_create(dom_string *qname,
		dom_string *public_id, 
		dom_string *system_id,
		dom_document_type **doctype);
/* Destroy a document type */
void _dom_document_type_destroy(dom_node_internal *doctypenode);
dom_exception _dom_document_type_initialise(dom_document_type *doctype,
		dom_string *qname, dom_string *public_id,
		dom_string *system_id);
void _dom_document_type_finalise(dom_document_type *doctype);

/* The virtual functions of DocumentType */
dom_exception _dom_document_type_get_name(dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_entities(
		dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_notations(
		dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_public_id(
		dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_system_id(
		dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_internal_subset(
		dom_document_type *doc_type,
		dom_string **result);

dom_exception _dom_document_type_get_text_content(dom_node_internal *node,
                                                  dom_string **result);
dom_exception _dom_document_type_set_text_content(dom_node_internal *node,
                                                  dom_string *content);

#define DOM_DOCUMENT_TYPE_VTABLE \
	_dom_document_type_get_name, \
	_dom_document_type_get_entities, \
	_dom_document_type_get_notations, \
	_dom_document_type_get_public_id, \
	_dom_document_type_get_system_id, \
	_dom_document_type_get_internal_subset

#define DOM_NODE_VTABLE_DOCUMENT_TYPE \
	_dom_node_try_destroy, \
	_dom_node_get_node_name, \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_node_get_attributes, \
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_document_type_get_text_content, \
	_dom_document_type_set_text_content, \
	_dom_node_is_same, \
	_dom_node_lookup_prefix, \
	_dom_node_is_default_namespace, \
	_dom_node_lookup_namespace, \
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data

/* Following comes the protected vtable  */
void _dom_dt_destroy(dom_node_internal *node);
dom_exception _dom_dt_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_DT_PROTECT_VTABLE \
	_dom_dt_destroy, \
	_dom_dt_copy

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_element_h_
#define dom_internal_core_element_h_

struct dom_document;
struct dom_element;
struct dom_namednodemap;
struct dom_node;
struct dom_attr;
struct dom_attr_list;
struct dom_type_info;
struct dom_hash_table;

/**
 * DOM element node
 */
struct dom_element {
	struct dom_node_internal base;		/**< Base node */

	struct dom_attr_list *attributes;	/**< Element attributes */

	dom_string *id_ns;	/**< The id attribute's namespace */

	dom_string *id_name; 	/**< The id attribute's name */

	struct dom_type_info *schema_type_info;	/**< Type information */

	lwc_string **classes;
	uint32_t n_classes;
};

dom_exception _dom_element_create(struct dom_document *doc,
		dom_string *name, dom_string *ns,
		dom_string *prefix, struct dom_element **result);

dom_exception _dom_element_initialise(struct dom_document *doc,
		struct dom_element *el, dom_string *name, 
		dom_string *ns, dom_string *prefix);

void _dom_element_finalise(struct dom_element *ele);

void _dom_element_destroy(struct dom_element *element);


/* The virtual functions of dom_element */
dom_exception _dom_element_get_tag_name(struct dom_element *element,
		dom_string **name);
dom_exception _dom_element_get_attribute(struct dom_element *element,
		dom_string *name, dom_string **value);
dom_exception _dom_element_set_attribute(struct dom_element *element,
		dom_string *name, dom_string *value);
dom_exception _dom_element_remove_attribute(struct dom_element *element, 
		dom_string *name);
dom_exception _dom_element_get_attribute_node(struct dom_element *element, 
		dom_string *name, struct dom_attr **result);
dom_exception _dom_element_set_attribute_node(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_remove_attribute_node(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_get_elements_by_tag_name(
		struct dom_element *element, dom_string *name,
		struct dom_nodelist **result);

dom_exception _dom_element_get_attribute_ns(struct dom_element *element, 
		dom_string *ns, dom_string *localname,
		dom_string **value);
dom_exception _dom_element_set_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *qname,
		dom_string *value);
dom_exception _dom_element_remove_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname);
dom_exception _dom_element_get_attribute_node_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		struct dom_attr **result);
dom_exception _dom_element_set_attribute_node_ns(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, struct dom_nodelist **result);
dom_exception _dom_element_has_attribute(struct dom_element *element,
		dom_string *name, bool *result);
dom_exception _dom_element_has_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		bool *result);
dom_exception _dom_element_get_schema_type_info(struct dom_element *element, 
		struct dom_type_info **result);
dom_exception _dom_element_set_id_attribute(struct dom_element *element, 
		dom_string *name, bool is_id);
dom_exception _dom_element_set_id_attribute_ns(struct dom_element *element, 
		dom_string *ns, dom_string *localname,
		bool is_id);
dom_exception _dom_element_set_id_attribute_node(struct dom_element *element,
		struct dom_attr *id_attr, bool is_id);
dom_exception _dom_element_get_classes(struct dom_element *element,
		lwc_string ***classes, uint32_t *n_classes);
dom_exception _dom_element_has_class(struct dom_element *element,
		lwc_string *name, bool *match);

#define DOM_ELEMENT_VTABLE \
	_dom_element_get_tag_name, \
	_dom_element_get_attribute, \
	_dom_element_set_attribute, \
	_dom_element_remove_attribute, \
	_dom_element_get_attribute_node, \
	_dom_element_set_attribute_node, \
	_dom_element_remove_attribute_node, \
	_dom_element_get_elements_by_tag_name, \
	_dom_element_get_attribute_ns, \
	_dom_element_set_attribute_ns, \
	_dom_element_remove_attribute_ns, \
	_dom_element_get_attribute_node_ns, \
	_dom_element_set_attribute_node_ns, \
	_dom_element_get_elements_by_tag_name_ns, \
	_dom_element_has_attribute, \
	_dom_element_has_attribute_ns, \
	_dom_element_get_schema_type_info, \
	_dom_element_set_id_attribute, \
	_dom_element_set_id_attribute_ns, \
	_dom_element_set_id_attribute_node, \
	_dom_element_get_classes, \
	_dom_element_has_class

/* Overloading dom_node functions */
dom_exception _dom_element_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result);
dom_exception _dom_element_has_attributes(dom_node_internal *node,
		bool *result);
dom_exception _dom_element_normalize(dom_node_internal *node);
dom_exception _dom_element_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result);
dom_exception _dom_element_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result);
dom_exception _dom_element_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result);
#define DOM_NODE_VTABLE_ELEMENT \
	_dom_node_try_destroy, \
	_dom_node_get_node_name, \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_element_get_attributes, /*overload*/\
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_element_has_attributes, /*overload*/\
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_node_get_text_content, \
	_dom_node_set_text_content, \
	_dom_node_is_same, \
	_dom_element_lookup_prefix, /*overload*/\
	_dom_element_is_default_namespace, /*overload*/\
	_dom_element_lookup_namespace, /*overload*/\
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data

/**
 * The internal used vtable for element
 */
struct dom_element_protected_vtable {
	struct dom_node_protect_vtable base;

	dom_exception (*dom_element_parse_attribute)(dom_element *ele,
			dom_string *name, dom_string *value,
			dom_string **parsed);
			/**< Called by dom_attr_set_value, and used to check
			 *   whether the new attribute value is valid and 
			 *   return a valid on if it is not
			 */
};

typedef struct dom_element_protected_vtable dom_element_protected_vtable;

/* Parse the attribute's value */
static inline dom_exception dom_element_parse_attribute(dom_element *ele,
		dom_string *name, dom_string *value,
		dom_string **parsed)
{
	struct dom_node_internal *node = (struct dom_node_internal *) ele;
	return ((dom_element_protected_vtable *) node->vtable)->
			dom_element_parse_attribute(ele, name, value, parsed);
}
#define dom_element_parse_attribute(e, n, v, p) dom_element_parse_attribute( \
		(dom_element *) (e), (dom_string *) (n), \
		(dom_string *) (v), (dom_string **) (p))


/* The protected virtual function */
dom_exception _dom_element_parse_attribute(dom_element *ele,
		dom_string *name, dom_string *value,
		dom_string **parsed);
void __dom_element_destroy(dom_node_internal *node);
dom_exception _dom_element_copy(dom_node_internal *old, 
		dom_node_internal **copy);

#define DOM_ELEMENT_PROTECT_VTABLE \
	_dom_element_parse_attribute

#define DOM_NODE_PROTECT_VTABLE_ELEMENT \
	__dom_element_destroy, \
	_dom_element_copy

/* Helper functions*/
dom_exception _dom_element_copy_internal(dom_element *old,
		dom_element *ele);
#define dom_element_copy_internal(o, n) _dom_element_copy_internal( \
		(dom_element *) (o), (dom_element *) (n))

dom_exception _dom_element_get_id(struct dom_element *ele, dom_string **id);

extern struct dom_element_vtable _dom_element_vtable;

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_entityrererence_h_
#define dom_internal_core_entityrererence_h_

dom_exception _dom_entity_reference_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_entity_reference **result);

void _dom_entity_reference_destroy(dom_entity_reference *entity);

#define _dom_entity_reference_initialise _dom_node_initialise
#define _dom_entity_reference_finalise	_dom_node_finalise

/* Following comes the protected vtable  */
void _dom_er_destroy(dom_node_internal *node);
dom_exception _dom_er_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_ER_PROTECT_VTABLE \
	_dom_er_destroy, \
	_dom_er_copy

/* Helper functions */
dom_exception _dom_entity_reference_get_textual_representation(
		dom_entity_reference *entity,
		dom_string **result);

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_namednodemap_h_
#define dom_internal_core_namednodemap_h_

struct dom_document;
struct dom_node;
struct dom_namednodemap;

struct nnm_operation {
	dom_exception (*namednodemap_get_length)(void *priv,
			dom_ulong *length);

	dom_exception (*namednodemap_get_named_item)(void *priv,
			dom_string *name, struct dom_node **node);

	dom_exception (*namednodemap_set_named_item)(void *priv,
			struct dom_node *arg, struct dom_node **node);

	dom_exception (*namednodemap_remove_named_item)(
			void *priv, dom_string *name,
			struct dom_node **node);

	dom_exception (*namednodemap_item)(void *priv,
			dom_ulong index, struct dom_node **node);

	dom_exception (*namednodemap_get_named_item_ns)(
			void *priv, dom_string *ns,
			dom_string *localname, struct dom_node **node);

	dom_exception (*namednodemap_set_named_item_ns)(
			void *priv, struct dom_node *arg,
			struct dom_node **node);

	dom_exception (*namednodemap_remove_named_item_ns)(
			void *priv, dom_string *ns,
			dom_string *localname, struct dom_node **node);

	void (*namednodemap_destroy)(void *priv);

	bool (*namednodemap_equal)(void *p1, void *p2);
};

/* Create a namednodemap */
dom_exception _dom_namednodemap_create(struct dom_document *doc,
		void *priv, struct nnm_operation *opt,
		struct dom_namednodemap **map);

/* Update the private data */
void _dom_namednodemap_update(struct dom_namednodemap *map, void *priv);

/* Test whether two maps are equal */
bool _dom_namednodemap_equal(struct dom_namednodemap *m1, 
		struct dom_namednodemap *m2);

#define dom_namednodemap_equal(m1, m2) _dom_namednodemap_equal( \
		(struct dom_namednodemap *) (m1), \
		(struct dom_namednodemap *) (m2))

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_processinginstruction_h_
#define dom_internal_core_processinginstruction_h_

dom_exception _dom_processing_instruction_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_processing_instruction **result);

void _dom_processing_instruction_destroy(dom_processing_instruction *pi);

#define _dom_processing_instruction_initialise	_dom_node_initialise
#define _dom_processing_instruction_finalise 	_dom_node_finalise

/* Following comes the protected vtable  */
void _dom_pi_destroy(dom_node_internal *node);
dom_exception _dom_pi_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_PI_PROTECT_VTABLE \
	_dom_pi_destroy, \
	_dom_pi_copy

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_text_h_
#define dom_internal_core_text_h_

struct dom_document;

/**
 * A DOM text node
 */
struct dom_text {
	dom_characterdata base;	/**< Base node */

	bool element_content_whitespace;	/**< This node is element
						 * content whitespace */
};

/* Constructor and Destructor */
dom_exception _dom_text_create(struct dom_document *doc,
		dom_string *name, dom_string *value,
		dom_text **result);

void _dom_text_destroy(dom_text *text);

dom_exception _dom_text_initialise(dom_text *text,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value);

void _dom_text_finalise(dom_text *text);


/* Virtual functions for dom_text */
dom_exception _dom_text_split_text(dom_text *text,
		uint32_t offset, dom_text **result);
dom_exception _dom_text_get_is_element_content_whitespace(
		dom_text *text, bool *result);
dom_exception _dom_text_get_whole_text(dom_text *text,
		dom_string **result);
dom_exception _dom_text_replace_whole_text(dom_text *text,
		dom_string *content, dom_text **result);

#define DOM_TEXT_VTABLE \
	_dom_text_split_text, \
	_dom_text_get_is_element_content_whitespace, \
	_dom_text_get_whole_text, \
	_dom_text_replace_whole_text


/* Following comes the protected vtable  */
void __dom_text_destroy(struct dom_node_internal *node);
dom_exception _dom_text_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_TEXT_PROTECT_VTABLE \
	__dom_text_destroy, \
	_dom_text_copy

extern struct dom_text_vtable text_vtable;

dom_exception _dom_text_copy_internal(dom_text *old, dom_text *txt);
#define dom_text_copy_internal(o, n) \
		_dom_text_copy_internal((dom_text *) (o), (dom_text *) (n))

#endif
