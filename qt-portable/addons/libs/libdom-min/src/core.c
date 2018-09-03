/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <libwapcaplet/libwapcaplet.h>
#include <parserutils/parserutils.h>

#include <dom/dom.h>

#include "core_p.h"
#include "utils_p.h"

struct dom_element;

/**
 * DOM attribute node
 */
struct dom_attr {
	struct dom_node_internal base;	/**< Base node */

	struct dom_type_info *schema_type_info;	/**< Type information */

	dom_attr_type type;	/**< The type of this attribute */
	
	union {
		uint32_t lvalue;
		unsigned short svalue;
		bool bvalue;
	} value;	/**< The special type value of this attribute */

	bool specified;	/**< Whether the attribute is specified or default */

	bool is_id;	/**< Whether this attribute is a ID attribute */

	bool read_only;	/**< Whether this attribute is readonly */
};

/* The vtable for dom_attr node */
static struct dom_attr_vtable attr_vtable = {
	{
		DOM_NODE_VTABLE_ATTR
	},
	DOM_ATTR_VTABLE
};

/* The protected vtable for dom_attr */
static struct dom_node_protect_vtable attr_protect_vtable = {
	DOM_ATTR_PROTECT_VTABLE
};


/* -------------------------------------------------------------------- */

/* Constructor and destructor */

/**
 * Create an attribute node
 *
 * \param doc        The owning document
 * \param name       The (local) name of the node to create
 * \param ns         The namespace URI of the attribute, or NULL
 * \param prefix     The namespace prefix of the attribute, or NULL
 * \param specified  Whether this attribute is specified
 * \param result     Pointer to location to receive created attribute
 * \return DOM_NO_ERR     on success,
 *         DOM_NO_MEM_ERR on memory exhaustion.
 *
 * \p doc and \p name will have their reference counts increased. The 
 * caller should make sure that \p name is a valid NCName here.
 *
 * The returned attribute will already be referenced.
 */
dom_exception _dom_attr_create(struct dom_document *doc,
		dom_string *name, dom_string *ns,
		dom_string *prefix, bool specified, 
		struct dom_attr **result)
{
	struct dom_attr *a;
	dom_exception err;

	/* Allocate the attribute node */
	a = (struct dom_attr *)malloc(sizeof(struct dom_attr));
	if (a == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtable */
	a->base.base.vtable = &attr_vtable;
	a->base.vtable = &attr_protect_vtable;

	/* Initialise the class */
	err = _dom_attr_initialise(a, doc, name, ns, prefix, specified,
			result);
	if (err != DOM_NO_ERR) {
		free(a);
		return err;
	}

	return DOM_NO_ERR;
}

/**
 * Initialise a dom_attr
 *
 * \param a          The dom_attr
 * \param doc        The document
 * \param name       The name of this attribute node
 * \param ns         The namespace of this attribute
 * \param prefix     The prefix
 * \param specified  Whether this node is a specified one
 * \param result     The returned node
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_attr_initialise(dom_attr *a, 
		struct dom_document *doc, dom_string *name,
		dom_string *ns, dom_string *prefix,
		bool specified, struct dom_attr **result)
{
	dom_exception err;

	err = _dom_node_initialise(&a->base, doc, DOM_ATTRIBUTE_NODE,
			name, NULL, ns, prefix);
	if (err != DOM_NO_ERR) {
		return err;
	}

	a->specified = specified;
	a->schema_type_info = NULL;
	a->is_id = false;
	/* The attribute type is unset when it is created */
	a->type = DOM_ATTR_UNSET;
	a->read_only = false;

	*result = a;

	return DOM_NO_ERR;
}

/**
 * The destructor of dom_attr
 *
 * \param attr  The attribute
 */
void _dom_attr_finalise(dom_attr *attr)
{
	/* Now, clean up this node and destroy it */

	if (attr->schema_type_info != NULL) {
		/** \todo destroy schema type info */
	}

	_dom_node_finalise(&attr->base);
}

/**
 * Destroy an attribute node
 *
 * \param attr  The attribute to destroy
 *
 * The contents of \p attr will be destroyed and \p attr will be freed
 */
void _dom_attr_destroy(struct dom_attr *attr)
{
	_dom_attr_finalise(attr);

	free(attr);
}

/*-----------------------------------------------------------------------*/
/* Following are our implementation specific APIs */

/**
 * Get the Attr Node type
 *
 * \param a  The attribute node
 * \return the type
 */
dom_attr_type dom_attr_get_type(dom_attr *a)
{
	return a->type;
}

/**
 * Get the integer value of this attribute
 *
 * \param a      The attribute object
 * \param value  The returned value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a integer
 *                                 attribute
 */
dom_exception dom_attr_get_integer(dom_attr *a, uint32_t *value)
{
	if (a->type != DOM_ATTR_INTEGER)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	*value = a->value.lvalue;

	return DOM_NO_ERR;
}

/**
 * Set the integer value of this attribute
 *
 * \param a      The attribute object
 * \param value  The new value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a integer
 *                                 attribute
 */
dom_exception dom_attr_set_integer(dom_attr *a, uint32_t value)
{
	bool success = true;
	dom_exception err;

	/* If this is the first set method, we should fix this attribute
	 * type */
	if (a->type == DOM_ATTR_UNSET)
		a->type = DOM_ATTR_INTEGER;

	if (a->type != DOM_ATTR_INTEGER)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	if (a->value.lvalue == value)
		return DOM_NO_ERR;
	
	a->value.lvalue = value;

    return DOM_NO_ERR;
}

/**
 * Get the short value of this attribute
 *
 * \param a      The attribute object
 * \param value  The returned value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a short
 *                                 attribute
 */
dom_exception dom_attr_get_short(dom_attr *a, unsigned short *value)
{
	if (a->type != DOM_ATTR_SHORT)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	*value = a->value.svalue;

	return DOM_NO_ERR;
}

/**
 * Set the short value of this attribute
 *
 * \param a      The attribute object
 * \param value  The new value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a short
 *                                 attribute
 */
dom_exception dom_attr_set_short(dom_attr *a, unsigned short value)
{
	struct dom_document *doc;
	struct dom_node_internal *ele;
	bool success = true;
	dom_exception err;

	/* If this is the first set method, we should fix this attribute
	 * type */
	if (a->type == DOM_ATTR_UNSET)
		a->type = DOM_ATTR_SHORT;

	if (a->type != DOM_ATTR_SHORT)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	if (a->value.svalue == value)
		return DOM_NO_ERR;
	
	a->value.svalue = value;

    return DOM_NO_ERR;
}

/**
 * Get the bool value of this attribute
 *
 * \param a      The attribute object
 * \param value  The returned value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a bool
 *                                 attribute
 */
dom_exception dom_attr_get_bool(dom_attr *a, bool *value)
{
	if (a->type != DOM_ATTR_BOOL)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	*value = a->value.bvalue;

	return DOM_NO_ERR;
}

/**
 * Set the bool value of this attribute
 *
 * \param a      The attribute object
 * \param value  The new value
 * \return DOM_NO_ERR on success,
 *         DOM_ATTR_WRONG_TYPE_ERR if the attribute node is not a bool
 *                                 attribute
 */
dom_exception dom_attr_set_bool(dom_attr *a, bool value)
{
	struct dom_document *doc;
	struct dom_node_internal *ele;
	bool success = true;
	dom_exception err;

	/* If this is the first set method, we should fix this attribute
	 * type */
	if (a->type == DOM_ATTR_UNSET)
		a->type = DOM_ATTR_BOOL;

	if (a->type != DOM_ATTR_BOOL)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	if (a->value.bvalue == value)
		return DOM_NO_ERR;
	
	a->value.bvalue = value;

    return DOM_NO_ERR;
}

/**
 * Set the node as a readonly attribute
 *
 * \param a  The attribute
 */
void dom_attr_mark_readonly(dom_attr *a)
{
	a->read_only = true;
}

/* -------------------------------------------------------------------- */

/* The public virtual functions */

/**
 * Retrieve an attribute's name
 *
 * \param attr    Attribute to retrieve name from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_attr_get_name(struct dom_attr *attr,
		dom_string **result)
{
	/* This is the same as nodeName */
	return dom_node_get_node_name(attr, result);
}

/**
 * Determine if attribute was specified or default
 *
 * \param attr    Attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_attr_get_specified(struct dom_attr *attr, bool *result)
{
	*result = attr->specified;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's value
 *
 * \param attr    Attribute to retrieve value from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_attr_get_value(struct dom_attr *attr,
		dom_string **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_node_internal *c;
	dom_string *value, *temp;
	dom_exception err;
        
	/* Attempt to shortcut for a single text node child with value */
	if ((a->first_child != NULL) && 
	    (a->first_child == a->last_child) &&
	    (a->first_child->type == DOM_TEXT_NODE) &&
	    (a->first_child->value != NULL)) {
		*result = dom_string_ref(a->first_child->value);
		return DOM_NO_ERR;
	}
	
	err = dom_string_create(NULL, 0, &value);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Force unknown types to strings, if necessary */
	if (attr->type == DOM_ATTR_UNSET && a->first_child != NULL) {
		attr->type = DOM_ATTR_STRING;
	}

	/* If this attribute node is not a string one, we just return an empty
	 * string */
	if (attr->type != DOM_ATTR_STRING) {
		*result = value;
		return DOM_NO_ERR;
	}

	/* Traverse children, building a string representation as we go */
	for (c = a->first_child; c != NULL; c = c->next) {
		if (c->type == DOM_TEXT_NODE && c->value != NULL) {
			/* Append to existing value */
			err = dom_string_concat(value, c->value, &temp);
			if (err != DOM_NO_ERR) {
				dom_string_unref(value);
				return err;
			}

			/* Finished with previous value */
			dom_string_unref(value);

			/* Claim new value */
			value = temp;
		} else if (c->type == DOM_ENTITY_REFERENCE_NODE) {
			dom_string *tr;

			/* Get textual representation of entity */
			err = _dom_entity_reference_get_textual_representation(
					(struct dom_entity_reference *) c,
					&tr);
			if (err != DOM_NO_ERR) {
				dom_string_unref(value);
				return err;
			}

			/* Append to existing value */
			err = dom_string_concat(value, tr, &temp);
			if (err != DOM_NO_ERR) {
				dom_string_unref(tr);
				dom_string_unref(value);
				return err;
			}

			/* No int32_ter need textual representation */
			dom_string_unref(tr);

			/* Finished with previous value */
			dom_string_unref(value);

			/* Claim new value */
			value = temp;
		}
	}

	*result = value;

	return DOM_NO_ERR;
}

/**
 * Set an attribute's value
 *
 * \param attr   Attribute to retrieve value from
 * \param value  New value for attribute
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if attribute is readonly.
 */
dom_exception _dom_attr_set_value(struct dom_attr *attr,
		dom_string *value)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_node_internal *c, *d;
	struct dom_text *text;
	dom_exception err;
	dom_string *name = NULL;
	dom_string *parsed = NULL;

	/* Ensure attribute is writable */
	if (_dom_node_readonly(a))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* If this is the first set method, we should fix this attribute
	 * type */
	if (attr->type == DOM_ATTR_UNSET)
		attr->type = DOM_ATTR_STRING;
	
	if (attr->type != DOM_ATTR_STRING)
		return DOM_ATTR_WRONG_TYPE_ERR;
	
	err = _dom_attr_get_name(attr, &name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_element_parse_attribute(a->parent, name, value, &parsed);
	dom_string_unref(name);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Create text node containing new value */
	err = dom_document_create_text_node(a->owner, parsed, &text);
	dom_string_unref(parsed);
	if (err != DOM_NO_ERR)
		return err;
	
	/* Destroy children of this node */
	for (c = a->first_child; c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_try_destroy(c);
	}

	/* And insert the text node as the value */
	((struct dom_node_internal *) text)->parent = a;
	a->first_child = a->last_child = (struct dom_node_internal *) text;
	dom_node_unref(text);
	dom_node_remove_pending(text);

	/* Now the attribute node is specified */
	attr->specified = true;

	return DOM_NO_ERR;
}

/**
 * Retrieve the owning element of an attribute
 *
 * \param attr    The attribute to extract owning element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception _dom_attr_get_owner(struct dom_attr *attr,
		struct dom_element **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;

	/* If there is an owning element, increase its reference count */
	if (a->parent != NULL)
		dom_node_ref(a->parent);

	*result = (struct dom_element *) a->parent;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's type information
 *
 * \param attr    The attribute to extract type information from
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned type info will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception _dom_attr_get_schema_type_info(struct dom_attr *attr,
		struct dom_type_info **result)
{
	UNUSED(attr);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if an attribute if of type ID
 *
 * \param attr    The attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_attr_is_id(struct dom_attr *attr, bool *result)
{
	*result = attr->is_id;

	return DOM_NO_ERR;
}

/*------------- The overload virtual functions ------------------------*/

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_get_node_value(dom_node_internal *node,
		dom_string **result)
{
	dom_attr *attr = (dom_attr *) node;

	return _dom_attr_get_value(attr, result);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result)
{
	dom_exception err;
	dom_attr *attr;

	/* Discard the warnings */
	UNUSED(deep);

	/* Clone an Attr always clone all its children */
	err = _dom_node_clone_node(node, true, result);
	if (err != DOM_NO_ERR)
		return err;
	
	attr = (dom_attr *) *result;
	/* Clone an Attr always result a specified Attr, 
	 * see DOM Level 3 Node.cloneNode */
	attr->specified = true;

	return DOM_NO_ERR;
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_set_prefix(dom_node_internal *node,
		dom_string *prefix)
{
	/* Really I don't know whether there should something
	 * special to do here */
	return _dom_node_set_prefix(node, prefix);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	return dom_node_lookup_prefix(owner, ns, result);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = false;
		return DOM_NO_ERR;
	}

	return dom_node_is_default_namespace(owner, ns, result);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	return dom_node_lookup_namespace(owner, prefix, result);
}


/*----------------------------------------------------------------------*/

/* The protected virtual functions */

/* The virtual destroy function of this class */
void __dom_attr_destroy(dom_node_internal *node)
{
	_dom_attr_destroy((dom_attr *) node);
}

/* The memory allocator of this class */
dom_exception _dom_attr_copy(dom_node_internal *n, dom_node_internal **copy)
{
	dom_attr *old = (dom_attr *) n;
	dom_attr *a;
	dom_exception err;
	
	a = (dom_attr *)malloc(sizeof(struct dom_attr));
	if (a == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_node_copy_internal(n, a);
	if (err != DOM_NO_ERR) {
		free(a);
		return err;
	}
	
	a->specified = old->specified;

	/* TODO: deal with dom_type_info, it get no definition ! */
	a->schema_type_info = NULL;

	a->is_id = old->is_id;

	a->type = old->type;

	a->value = old->value;

	/* TODO: is this correct? */
	a->read_only = false;

	*copy = (dom_node_internal *) a;

	return DOM_NO_ERR;
}


/**
 * Set/Unset whether this attribute is a ID attribute 
 *
 * \param attr   The attribute
 * \param is_id  Whether it is a ID attribute
 */
void _dom_attr_set_isid(struct dom_attr *attr, bool is_id)
{
	attr->is_id = is_id;
}

/**
 * Set/Unset whether the attribute is a specified one.
 *
 * \param attr       The attribute node
 * \param specified  Whether this attribute is a specified one
 */
void _dom_attr_set_specified(struct dom_attr *attr, bool specified)
{
	attr->specified = specified;
}

/**
 * Whether this attribute node is readonly
 *
 * \param a  The node
 * \return true if this Attr is readonly, false otherwise
 */
bool _dom_attr_readonly(const dom_attr *a)
{
	return a->read_only;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com> 
 */

/**
 * A DOM CDATA section
 */
struct dom_cdata_section {
	dom_text base;		/**< Base node */
};

static struct dom_node_protect_vtable cdata_section_protect_vtable = {
	DOM_CDATA_SECTION_PROTECT_VTABLE
};

/**
 * Create a CDATA section
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR on success or DOM_NO_MEM_ERR on memory exhaustion.
 *
 * \p doc, \p name and \p value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_cdata_section_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_cdata_section **result)
{
	dom_cdata_section *c;
	dom_exception err;

	/* Allocate the comment node */
	c = (dom_cdata_section *)malloc(sizeof(dom_cdata_section));
	if (c == NULL)
		return DOM_NO_MEM_ERR;
	
	/* Set up vtable */
	((dom_node_internal *) c)->base.vtable = &text_vtable;
	((dom_node_internal *) c)->vtable = &cdata_section_protect_vtable;

	/* And initialise the node */
	err = _dom_cdata_section_initialise(&c->base, doc,
			DOM_CDATA_SECTION_NODE, name, value);
	if (err != DOM_NO_ERR) {
		free(c);
		return err;
	}

	*result = c;

	return DOM_NO_ERR;
}

/**
 * Destroy a CDATA section
 *
 * \param cdata  The cdata section to destroy
 *
 * The contents of \p cdata will be destroyed and \p cdata will be freed.
 */
void _dom_cdata_section_destroy(dom_cdata_section *cdata)
{
	/* Clean up base node contents */
	_dom_cdata_section_finalise(&cdata->base);

	/* Destroy the node */
	free(cdata);
}

/*--------------------------------------------------------------------------*/

/* The protected virtual functions */

/* The virtual destroy function of this class */
void __dom_cdata_section_destroy(dom_node_internal *node)
{
	_dom_cdata_section_destroy((dom_cdata_section *) node);
}

/* The copy constructor of this class */
dom_exception _dom_cdata_section_copy(dom_node_internal *old, 
		dom_node_internal **copy)
{
	dom_cdata_section *new_cdata;
	dom_exception err;

	new_cdata = (dom_cdata_section *)malloc(sizeof(dom_cdata_section));
	if (new_cdata == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_text_copy_internal(old, new_cdata);
	if (err != DOM_NO_ERR) {
		free(new_cdata);
		return err;
	}

	*copy = (dom_node_internal *) new_cdata;

	return DOM_NO_ERR;	
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/* The virtual functions for dom_characterdata, we make this vtable
 * public to each child class */
struct dom_characterdata_vtable characterdata_vtable = {
	{
		DOM_NODE_VTABLE_CHARACTERDATA
	},
	DOM_CHARACTERDATA_VTABLE
};


/* Create a DOM characterdata node and compose the vtable */
dom_characterdata *_dom_characterdata_create(void)
{
	dom_characterdata *cdata = (dom_characterdata *)malloc(sizeof(struct dom_characterdata));
	if (cdata == NULL)
		return NULL;

	cdata->base.base.vtable = &characterdata_vtable;
	cdata->base.vtable = NULL;

	return cdata;
}

/**
 * Initialise a character data node
 *
 * \param cdata  The character data node to initialise
 * \param doc    The document which owns the node
 * \param type   The node type required
 * \param name   The node name, or NULL
 * \param value  The node value, or NULL
 * \return DOM_NO_ERR on success.
 *
 * \p doc, \p name and \p value will have their reference counts increased.
 */
dom_exception _dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value)
{
	return _dom_node_initialise(&cdata->base, doc, type, 
			name, value, NULL, NULL);
}

/**
 * Finalise a character data node
 *
 * \param cdata  The node to finalise
 *
 * The contents of \p cdata will be cleaned up. \p cdata will not be freed.
 */
void _dom_characterdata_finalise(struct dom_characterdata *cdata)
{
	_dom_node_finalise(&cdata->base);
}


/*----------------------------------------------------------------------*/

/* The public virtual functions */

/**
 * Retrieve data from a character data node
 *
 * \param cdata  Character data node to retrieve data from
 * \param data   Pointer to location to receive data
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_characterdata_get_data(struct dom_characterdata *cdata,
		dom_string **data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;

	if (c->value != NULL) {
		dom_string_ref(c->value);
	}
	*data = c->value;

	return DOM_NO_ERR;
}

/**
 * Set the content of a character data node
 *
 * \param cdata  Node to set the content of
 * \param data   New value for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p cdata is readonly.
 *
 * The new content will have its reference count increased, so the caller
 * should unref it after the call (as the caller should have already claimed
 * a reference on the string). The node's existing content will be unrefed.
 */
dom_exception _dom_characterdata_set_data(struct dom_characterdata *cdata,
		dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	dom_exception err;
	struct dom_document *doc;
	bool success = true;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

    return DOM_NO_ERR;
}

/**
 * Get the length (in characters) of a character data node's content
 *
 * \param cdata   Node to read content length of
 * \param length  Pointer to location to receive character length of content
 * \return DOM_NO_ERR.
 */
dom_exception _dom_characterdata_get_length(struct dom_characterdata *cdata,
		uint32_t *length)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;

	if (c->value != NULL) {
		*length = dom_string_length(c->value);
	} else {
		*length = 0;
	}

	return DOM_NO_ERR;
}

/**
 * Extract a range of data from a character data node
 *
 * \param cdata   The node to extract data from
 * \param offset  The character offset of substring to extract
 * \param count   The number of characters to extract
 * \param data    Pointer to location to receive substring
 * \return DOM_NO_ERR         on success,
 *         DOM_INDEX_SIZE_ERR if \p offset is negative or greater than the 
 *                            number of characters in \p cdata or 
 *                            \p count is negative.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_characterdata_substring_data(
		struct dom_characterdata *cdata, uint32_t offset,
		uint32_t count, dom_string **data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	uint32_t len, end;

	if ((int32_t) offset < 0 || (int32_t) count < 0) {
		return DOM_INDEX_SIZE_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	return dom_string_substr(c->value, offset, end, data);
}

/**
 * Append data to the end of a character data node's content
 *
 * \param cdata  The node to append data to
 * \param data   The data to append
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p cdata is readonly.
 */
dom_exception _dom_characterdata_append_data(struct dom_characterdata *cdata,
		dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	dom_string *temp;
	dom_exception err;
	bool success = true;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	err = dom_string_concat(c->value, data, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

    return DOM_NO_ERR;
}

/**
 * Insert data into a character data node's content
 *
 * \param cdata   The node to insert into
 * \param offset  The character offset to insert at
 * \param data    The data to insert
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if \p offset is negative or greater 
 *                                         than the number of characters in 
 *                                         \p cdata,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p cdata is readonly.
 */
dom_exception _dom_characterdata_insert_data(struct dom_characterdata *cdata,
		uint32_t offset, dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	dom_string *temp;
	uint32_t len;
	dom_exception err;
	bool success = true;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if ((int32_t) offset < 0) {
		return DOM_INDEX_SIZE_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	err = dom_string_insert(c->value, data, offset, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

    return DOM_NO_ERR;
}

/**
 * Delete data from a character data node's content
 *
 * \param cdata   The node to delete from
 * \param offset  The character offset to start deletion from
 * \param count   The number of characters to delete
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if \p offset is negative or greater 
 *                                         than the number of characters in 
 *                                         \p cdata or \p count is negative,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p cdata is readonly.
 */
dom_exception _dom_characterdata_delete_data(struct dom_characterdata *cdata,
		uint32_t offset, uint32_t count)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	dom_string *temp;
	uint32_t len, end;
	dom_exception err;
	struct dom_document *doc;
	bool success = true;
	dom_string *empty;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if ((int32_t) offset < 0 || (int32_t) count < 0) {
		return DOM_INDEX_SIZE_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	empty = ((struct dom_document *)
		 ((struct dom_node_internal *)c)->owner)->_memo_empty;

	return dom_string_replace(c->value, empty, offset, end, &temp);
}

/**
 * Replace a section of a character data node's content
 *
 * \param cdata   The node to modify
 * \param offset  The character offset of the sequence to replace
 * \param count   The number of characters to replace
 * \param data    The replacement data
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if \p offset is negative or greater 
 *                                         than the number of characters in 
 *                                         \p cdata or \p count is negative,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p cdata is readonly.
 */
dom_exception _dom_characterdata_replace_data(struct dom_characterdata *cdata,
		uint32_t offset, uint32_t count,
		dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	dom_string *temp;
	uint32_t len, end;
	dom_exception err;
	struct dom_document *doc;
	bool success = true;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if ((int32_t) offset < 0 || (int32_t) count < 0) {
		return DOM_INDEX_SIZE_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	return dom_string_replace(c->value, data, offset, end, &temp);
}

dom_exception _dom_characterdata_get_text_content(dom_node_internal *node,
						  dom_string **result)
{
	dom_characterdata *cdata = (dom_characterdata *)node;
	
	return dom_characterdata_get_data(cdata, result);
}

dom_exception _dom_characterdata_set_text_content(dom_node_internal *node,
						  dom_string *content)
{
	dom_characterdata *cdata = (dom_characterdata *)node;
	
	return dom_characterdata_set_data(cdata, content);
}

/*----------------------------------------------------------------------*/

/* The protected virtual functions of Node, see core/node.h for details */
void _dom_characterdata_destroy(struct dom_node_internal *node)
{
	assert("Should never be here" == NULL);
	UNUSED(node);
}

/* The copy constructor of this class */
dom_exception _dom_characterdata_copy(dom_node_internal *old, 
		dom_node_internal **copy)
{
	dom_characterdata *new_node;
	dom_exception err;

	new_node = (dom_characterdata *)malloc(sizeof(dom_characterdata));
	if (new_node == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_characterdata_copy_internal(old, new_node);
	if (err != DOM_NO_ERR) {
		free(new_node);
		return err;
	}

	*copy = (dom_node_internal *) new_node;

	return DOM_NO_ERR;
}

dom_exception _dom_characterdata_copy_internal(dom_characterdata *old,
		dom_characterdata *dat)
{
	return dom_node_copy_internal(old, dat);
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * A DOM Comment node
 */
struct dom_comment {
	dom_characterdata base;	/**< Base node */
};

static struct dom_node_protect_vtable comment_protect_vtable = {
	DOM_COMMENT_PROTECT_VTABLE
};

/**
 * Create a comment node
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_comment_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_comment **result)
{
	dom_comment *c;
	dom_exception err;

	/* Allocate the comment node */
	c = (dom_comment *)malloc(sizeof(dom_comment));
	if (c == NULL)
		return DOM_NO_MEM_ERR;

	/* Set the virtual table */
	((dom_node_internal *) c)->base.vtable = &characterdata_vtable;
	((dom_node_internal *) c)->vtable = &comment_protect_vtable;

	/* And initialise the node */
	err = _dom_characterdata_initialise(&c->base, doc, DOM_COMMENT_NODE,
			name, value);
	if (err != DOM_NO_ERR) {
		free(c);
		return err;
	}

	*result = c;

	return DOM_NO_ERR;
}

/**
 * Destroy a comment node
 *
 * \param comment  The node to destroy
 *
 * The contents of ::comment will be destroyed and ::comment will be freed
 */
void _dom_comment_destroy(dom_comment *comment)
{
	/* Finalise base class contents */
	_dom_characterdata_finalise(&comment->base);

	/* Free node */
	free(comment);
}


/*-----------------------------------------------------------------------*/
/* The protected virtual functions */

/* The virtual destroy function */
void __dom_comment_destroy(dom_node_internal *node)
{
	_dom_comment_destroy((dom_comment *) node);
}

/* The copy constructor of this class */
dom_exception _dom_comment_copy(dom_node_internal *old, 
		dom_node_internal **copy)
{
	dom_comment *new_comment;
	dom_exception err;

	new_comment = (dom_comment *)malloc(sizeof(dom_comment));
	if (new_comment == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_characterdata_copy_internal(old, new_comment);
	if (err != DOM_NO_ERR) {
		free(new_comment);
		return err;
	}

	*copy = (dom_node_internal *) new_comment;

	return DOM_NO_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * A DOM document fragment
 */
struct dom_document_fragment {
	dom_node_internal base;		/**< Base node */
};

static struct dom_node_vtable df_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable df_protect_vtable = {
	DOM_DF_PROTECT_VTABLE
};

/**
 * Create a document fragment
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_document_fragment_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_document_fragment **result)
{
	dom_document_fragment *f;
	dom_exception err;

	f = (dom_document_fragment *)malloc(sizeof(dom_document_fragment));
	if (f == NULL)
		return DOM_NO_MEM_ERR;

	f->base.base.vtable = &df_vtable;
	f->base.vtable = &df_protect_vtable;

	/* And initialise the node */
	err = _dom_document_fragment_initialise(&f->base, doc, 
			DOM_DOCUMENT_FRAGMENT_NODE, name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		free(f);
		return err;
	}

	*result = f;

	return DOM_NO_ERR;
}

/**
 * Destroy a document fragment
 *
 * \param frag  The document fragment to destroy
 *
 * The contents of ::frag will be destroyed and ::frag will be freed.
 */
void _dom_document_fragment_destroy(dom_document_fragment *frag)
{
	/* Finalise base class */
	_dom_document_fragment_finalise(&frag->base);

	/* Destroy fragment */
	free(frag);
}

/*-----------------------------------------------------------------------*/

/* Overload protected functions */

/* The virtual destroy function of this class */
void _dom_df_destroy(dom_node_internal *node)
{
	_dom_document_fragment_destroy((dom_document_fragment *) node);
}

/* The copy constructor of this class */
dom_exception _dom_df_copy(dom_node_internal *old, dom_node_internal **copy)
{
	dom_document_fragment *new_f;
	dom_exception err;

	new_f = (dom_document_fragment *)malloc(sizeof(dom_document_fragment));
	if (new_f == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_node_copy_internal(old, new_f);
	if (err != DOM_NO_ERR) {
		free(new_f);
		return err;
	}

	*copy = (dom_node_internal *) new_f;

	return DOM_NO_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * Item in list of active nodelists
 */
struct dom_doc_nl {
	dom_nodelist *list;	/**< Nodelist */

	struct dom_doc_nl *next;	/**< Next item */
	struct dom_doc_nl *prev;	/**< Previous item */
};

/* The virtual functions of this dom_document */
static struct dom_document_vtable document_vtable = {
	{
		DOM_NODE_VTABLE_DOCUMENT
	},
	DOM_DOCUMENT_VTABLE
};

static struct dom_node_protect_vtable document_protect_vtable = {
	DOM_DOCUMENT_PROTECT_VTABLE
};


/*----------------------------------------------------------------------*/

/* Internally used helper functions */
static dom_exception dom_document_dup_node(dom_document *doc, 
		dom_node *node, bool deep, dom_node **result, 
		dom_node_operation opt);


/*----------------------------------------------------------------------*/

/* The constructors and destructors */

/**
 * Create a Document
 *
 * \param doc    Pointer to location to receive created document
 * \param daf    The default action fetcher
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The returned document will already be referenced.
 */
dom_exception _dom_document_create(
				   dom_document **doc)
{
	dom_document *d;
	dom_exception err;

	/* Create document */
	d = (dom_document *)malloc(sizeof(dom_document));
	if (d == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the virtual table */
	d->base.base.vtable = &document_vtable;
	d->base.vtable = &document_protect_vtable;

	/* Initialise base class -- the Document has no parent, so
	 * destruction will be attempted as soon as its reference count
	 * reaches zero. Documents own themselves (this simplifies the 
	 * rest of the code, as it doesn't need to special case Documents)
	 */
	err = _dom_document_initialise(d);
	if (err != DOM_NO_ERR) {
		/* Clean up document */
		free(d);
		return err;
	}

	*doc = d;

	return DOM_NO_ERR;
}

/* Initialise the document */
dom_exception _dom_document_initialise(dom_document *doc)
{
	dom_exception err;
	dom_string *name;

	err = dom_string_create((const uint8_t *) "#document", 
			SLEN("#document"), &name);
	if (err != DOM_NO_ERR)
		return err;

	err = dom_string_create_interned((const uint8_t *) "about:blank",
			SLEN("about:blank"), &doc->uri);
	if (err != DOM_NO_ERR) {
		dom_string_unref(name);
		return err;
	}

	doc->nodelists = NULL;

	err = _dom_node_initialise(&doc->base, doc, DOM_DOCUMENT_NODE,
			name, NULL, NULL, NULL);
	dom_string_unref(name);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->uri);
		return err;
	}

	list_init(&doc->pending_nodes);

	err = dom_string_create_interned((const uint8_t *) "id",
					 SLEN("id"), &doc->id_name);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->uri);
		return err;
	}
	doc->quirks = DOM_DOCUMENT_QUIRKS_MODE_NONE;

	err = dom_string_create_interned((const uint8_t *) "class",
			SLEN("class"), &doc->class_string);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "script",
			SLEN("script"), &doc->script_string);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		return err;
	}

	/* Intern the empty string. The use of a space in the constant
	 * is to prevent the compiler warning about an empty string.
	 */
	err = dom_string_create_interned((const uint8_t *) " ", 0,
					 &doc->_memo_empty);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMNodeInserted",
					 SLEN("DOMNodeInserted"),
					 &doc->_memo_domnodeinserted);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMNodeRemoved",
					 SLEN("DOMNodeRemoved"),
					 &doc->_memo_domnoderemoved);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMNodeInsertedIntoDocument",
					 SLEN("DOMNodeInsertedIntoDocument"),
					 &doc->_memo_domnodeinsertedintodocument);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domnoderemoved);
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMNodeRemovedFromDocument",
					 SLEN("DOMNodeRemovedFromDocument"),
					 &doc->_memo_domnoderemovedfromdocument);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domnodeinsertedintodocument);
		dom_string_unref(doc->_memo_domnoderemoved);
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMAttrModified",
					 SLEN("DOMAttrModified"),
					 &doc->_memo_domattrmodified);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domnoderemovedfromdocument);
		dom_string_unref(doc->_memo_domnodeinsertedintodocument);
		dom_string_unref(doc->_memo_domnoderemoved);
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMCharacterDataModified",
					 SLEN("DOMCharacterDataModified"),
					 &doc->_memo_domcharacterdatamodified);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domattrmodified);
		dom_string_unref(doc->_memo_domnoderemovedfromdocument);
		dom_string_unref(doc->_memo_domnodeinsertedintodocument);
		dom_string_unref(doc->_memo_domnoderemoved);
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	err = dom_string_create_interned((const uint8_t *) "DOMSubtreeModified",
					 SLEN("DOMSubtreeModified"),
					 &doc->_memo_domsubtreemodified);
	if (err != DOM_NO_ERR) {
		dom_string_unref(doc->_memo_domcharacterdatamodified);
		dom_string_unref(doc->_memo_domattrmodified);
		dom_string_unref(doc->_memo_domnoderemovedfromdocument);
		dom_string_unref(doc->_memo_domnodeinsertedintodocument);
		dom_string_unref(doc->_memo_domnoderemoved);
		dom_string_unref(doc->_memo_domnodeinserted);
		dom_string_unref(doc->_memo_empty);
		dom_string_unref(doc->uri);
		dom_string_unref(doc->id_name);
		dom_string_unref(doc->class_string);
		dom_string_unref(doc->script_string);
		return err;
	}

	/* We should not pass a NULL when all things hook up */
	return DOM_NO_ERR;
}


/* Finalise the document */
bool _dom_document_finalise(dom_document *doc)
{
	/* Finalise base class, delete the tree in force */
	_dom_node_finalise(&doc->base);

	/* Now, the first_child and last_child should be null */
	doc->base.first_child = NULL;
	doc->base.last_child = NULL;

	/* Ensure list of nodes pending deletion is empty. If not,
	 * then we can't yet destroy the document (its destruction will
	 * have to wait until the pending nodes are destroyed) */
	if (doc->pending_nodes.next != &doc->pending_nodes)
		return false;

	/* Ok, the document tree is empty, as is the list of nodes pending
	 * deletion. Therefore, it is safe to destroy the document. */

	/* This is paranoia -- if there are any remaining nodelists,
	 * then the document's reference count will be
	 * non-zero as these data structures reference the document because
	 * they are held by the client. */
	doc->nodelists = NULL;

	if (doc->id_name != NULL)
		dom_string_unref(doc->id_name);

	dom_string_unref(doc->uri);
	dom_string_unref(doc->class_string);
	dom_string_unref(doc->script_string);
	dom_string_unref(doc->_memo_empty);
	dom_string_unref(doc->_memo_domnodeinserted);
	dom_string_unref(doc->_memo_domnoderemoved);
	dom_string_unref(doc->_memo_domnodeinsertedintodocument);
	dom_string_unref(doc->_memo_domnoderemovedfromdocument);
	dom_string_unref(doc->_memo_domattrmodified);
	dom_string_unref(doc->_memo_domcharacterdatamodified);
	dom_string_unref(doc->_memo_domsubtreemodified);
	
	return true;
}



/*----------------------------------------------------------------------*/

/* Public virtual functions */

/**
 * Retrieve the doctype of a document
 *
 * \param doc     The document to retrieve the doctype from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_doctype(dom_document *doc,
		dom_document_type **result)
{
	dom_node_internal *c;

	for (c = doc->base.first_child; c != NULL; c = c->next) {
		if (c->type == DOM_DOCUMENT_TYPE_NODE)
			break;
	}

	if (c != NULL)
		dom_node_ref(c);

	*result = (dom_document_type *) c;

	return DOM_NO_ERR;
}

/**
 * Retrieve the DOM implementation that handles this document
 *
 * \param doc     The document to retrieve the implementation from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned implementation will have its reference count increased.
 * It is the responsibility of the caller to unref the implementation once
 * it has finished with it.
 */
dom_exception _dom_document_get_implementation(dom_document *doc,
		dom_implementation **result)
{
	UNUSED(doc);

	*result = (dom_implementation *) "libdom";

	return DOM_NO_ERR;
}

/**
 * Retrieve the document element of a document
 *
 * \param doc     The document to retrieve the document element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_document_element(dom_document *doc,
		dom_element **result)
{
	dom_node_internal *root;

	/* Find the first element node in child list */
	for (root = doc->base.first_child; root != NULL; root = root->next) {
		if (root->type == DOM_ELEMENT_NODE)
			break;
	}

	if (root != NULL)
		dom_node_ref(root);

	*result = (dom_element *) root;

	return DOM_NO_ERR;
}

/**
 * Create an element
 *
 * \param doc       The document owning the element
 * \param tag_name  The name of the element
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p tag_name is invalid.
 *
 * \p doc and \p tag_name will have their reference counts increased.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_element(dom_document *doc,
		dom_string *tag_name, dom_element **result)
{
	if (_dom_validate_name(tag_name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	return _dom_element_create(doc, tag_name, NULL, NULL, result);
}

/**
 * Create a document fragment
 *
 * \param doc     The document owning the fragment
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_document_fragment(dom_document *doc,
		dom_document_fragment **result)
{
	dom_string *name;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "#document-fragment", 
			SLEN("#document-fragment"), &name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = _dom_document_fragment_create(doc, name, NULL, result);
	dom_string_unref(name);

	return err;
}

/**
 * Create a text node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_text_node(dom_document *doc,
		dom_string *data, dom_text **result)
{
	dom_string *name;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "#text", 
			SLEN("#text"), &name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = _dom_text_create(doc, name, data, result);
	dom_string_unref(name);

	return err;
}

/**
 * Create a comment node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_comment(dom_document *doc,
		dom_string *data, dom_comment **result)
{
	dom_string *name;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "#comment", SLEN("#comment"),
			&name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = _dom_comment_create(doc, name, data, result);
	dom_string_unref(name);

	return err;
}

/**
 * Create a CDATA section
 *
 * \param doc     The document owning the section
 * \param data    The data for the section contents
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_cdata_section(dom_document *doc,
		dom_string *data, dom_cdata_section **result)
{
	dom_string *name;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "#cdata-section", 
			SLEN("#cdata-section"), &name);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_cdata_section_create(doc, name, data, result);
	dom_string_unref(name);

	return err;
}

/**
 * Create a processing instruction
 *
 * \param doc     The document owning the instruction
 * \param target  The instruction target
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p target is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_processing_instruction(
		dom_document *doc, dom_string *target,
		dom_string *data,
		dom_processing_instruction **result)
{
	if (_dom_validate_name(target) == false)
		return DOM_INVALID_CHARACTER_ERR;

	return _dom_processing_instruction_create(doc, target, data, result);
}

/**
 * Create an attribute
 *
 * \param doc     The document owning the attribute
 * \param name    The name of the attribute
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p name is invalid.
 *
 * The constructed attribute will always be classified as 'specified'.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_attribute(dom_document *doc,
		dom_string *name, dom_attr **result)
{
	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	return _dom_attr_create(doc, name, NULL, NULL, true, result);
}

/**
 * Create an entity reference
 *
 * \param doc     The document owning the reference
 * \param name    The name of the entity to reference
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p name is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_entity_reference(dom_document *doc,
		dom_string *name,
		dom_entity_reference **result)
{
	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	return _dom_entity_reference_create(doc, name, NULL, result);
}

/**
 * Retrieve a list of all elements with a given tag name
 *
 * \param doc      The document to search in
 * \param tagname  The tag name to search for ("*" for all)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_elements_by_tag_name(dom_document *doc,
		dom_string *tagname, dom_nodelist **result)
{
	return _dom_document_get_nodelist(doc, DOM_NODELIST_BY_NAME, 
			(dom_node_internal *) doc,  tagname, NULL, NULL, 
			result);
}

/**
 * Import a node from another document into this one
 *
 * \param doc     The document to import into
 * \param node    The node to import
 * \param deep    Whether to copy the node's subtree
 * \param result  Pointer to location to receive imported node in this document.
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if any of the names are invalid,
 *         DOM_NOT_SUPPORTED_ERR     if the type of \p node is unsupported
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_import_node(dom_document *doc,
		dom_node *node, bool deep, dom_node **result)
{
	/* TODO: The DOM_INVALID_CHARACTER_ERR exception */

	return dom_document_dup_node(doc, node, deep, result,
			DOM_NODE_IMPORTED);
}

/**
 * Create an element from the qualified name and namespace URI
 *
 * \param doc        The document owning the element
 * \param ns         The namespace URI to use, or NULL for none
 * \param qname      The qualified name of the element
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p qname is invalid,
 *         DOM_NAMESPACE_ERR         if \p qname is malformed, or it has a
 *                                   prefix and \p namespace is NULL, or
 *                                   \p qname has a prefix "xml" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or \p qname has a prefix "xmlns" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   \p namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   \p qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if \p doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_element_ns(dom_document *doc,
		dom_string *ns, dom_string *qname,
		dom_element **result)
{
	dom_string *prefix, *localname;
	dom_exception err;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, ns);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Attempt to create element */
	err = _dom_element_create(doc, localname, ns, prefix, result);

	/* Tidy up */
	if (localname != NULL) {
		dom_string_unref(localname);
	}

	if (prefix != NULL) {
		dom_string_unref(prefix);
	}

	return err;
}

/**
 * Create an attribute from the qualified name and namespace URI
 *
 * \param doc        The document owning the attribute
 * \param ns         The namespace URI to use
 * \param qname      The qualified name of the attribute
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p qname is invalid,
 *         DOM_NAMESPACE_ERR         if \p qname is malformed, or it has a
 *                                   prefix and \p namespace is NULL, or
 *                                   \p qname has a prefix "xml" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or \p qname has a prefix "xmlns" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   \p namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   \p qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if \p doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_attribute_ns(dom_document *doc,
		dom_string *ns, dom_string *qname,
		dom_attr **result)
{
	dom_string *prefix, *localname;
	dom_exception err;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, ns);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Attempt to create attribute */
	err = _dom_attr_create(doc, localname, ns, prefix, true, result);

	/* Tidy up */
	if (localname != NULL) {
		dom_string_unref(localname);
	}

	if (prefix != NULL) {
		dom_string_unref(prefix);
	}

	return err;
}

/**
 * Retrieve a list of all elements with a given local name and namespace URI
 *
 * \param doc        The document to search in
 * \param ns         The namespace URI
 * \param localname  The local name
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_elements_by_tag_name_ns(
		dom_document *doc, dom_string *ns,
		dom_string *localname, dom_nodelist **result)
{
	return _dom_document_get_nodelist(doc, DOM_NODELIST_BY_NAMESPACE, 
			(dom_node_internal *) doc, NULL, ns, localname,
			result);
}

/**
 * Retrieve the element that matches the specified ID
 *
 * \param doc     The document to search in
 * \param id      The ID to search for
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_element_by_id(dom_document *doc,
		dom_string *id, dom_element **result)
{
	dom_node_internal *root;
	dom_exception err;

	*result = NULL;

	err = dom_document_get_document_element(doc, (void *) &root);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_find_element_by_id(root, id, result);
	dom_node_unref(root);

	if (*result != NULL)
		dom_node_ref(*result);

	return err;
}

/**
 * Retrieve the input encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_input_encoding(dom_document *doc,
		dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_xml_encoding(dom_document *doc,
		dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the standalone status of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_get_xml_standalone(dom_document *doc,
		bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the standalone status of the document
 *
 * \param doc         The document to query
 * \param standalone  Standalone status to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_set_xml_standalone(dom_document *doc,
		bool standalone)
{
	UNUSED(doc);
	UNUSED(standalone);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML version of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_get_xml_version(dom_document *doc,
		dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the XML version of the document
 *
 * \param doc      The document to query
 * \param version  XML version to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_set_xml_version(dom_document *doc,
		dom_string *version)
{
	UNUSED(doc);
	UNUSED(version);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the error checking mode of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_get_strict_error_checking(
		dom_document *doc, bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the error checking mode of the document
 *
 * \param doc     The document to query
 * \param strict  Whether to use strict error checking
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_set_strict_error_checking(
		dom_document *doc, bool strict)
{
	UNUSED(doc);
	UNUSED(strict);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the URI of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_uri(dom_document *doc,
		dom_string **result)
{
	*result = dom_string_ref(doc->uri);

	return DOM_NO_ERR;
}

/**
 * Set the URI of the document
 *
 * \param doc  The document to query
 * \param uri  The URI to use
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_set_uri(dom_document *doc,
		dom_string *uri)
{
	dom_string_unref(doc->uri);

	doc->uri = dom_string_ref(uri);

	return DOM_NO_ERR;
}

/**
 * Attempt to adopt a node from another document into this document
 *
 * \param doc     The document to adopt into
 * \param node    The node to adopt
 * \param result  Pointer to location to receive adopted node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if \p node is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if \p node is of type Document or
 *                                         DocumentType
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 *
 * @note: The spec said adoptNode may be light weight than the importNode
 *	  because the former need no Node creation. But in our implementation
 *	  this can't be ensured. Both adoptNode and importNode create new
 *	  nodes using the importing/adopting document's resource manager. So,
 *	  generally, the adoptNode and importNode call the same function
 *	  dom_document_dup_node.
 */
dom_exception _dom_document_adopt_node(dom_document *doc,
		dom_node *node, dom_node **result)
{
	dom_node_internal *n = (dom_node_internal *) node;
	dom_exception err;
	dom_node_internal *parent;
	dom_node_internal *tmp;
	
	*result = NULL;

	if (n->type == DOM_DOCUMENT_NODE ||
			n->type == DOM_DOCUMENT_TYPE_NODE) {
		return DOM_NOT_SUPPORTED_ERR;		
	}

	if (n->type == DOM_ENTITY_NODE ||
			n->type == DOM_NOTATION_NODE ||
			n->type == DOM_PROCESSING_INSTRUCTION_NODE ||
			n->type == DOM_TEXT_NODE ||
			n->type == DOM_CDATA_SECTION_NODE ||
			n->type == DOM_COMMENT_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* Support XML when necessary */
	if (n->type == DOM_ENTITY_REFERENCE_NODE) {
		return DOM_NOT_SUPPORTED_ERR;
	}

	err = dom_document_dup_node(doc, node, true, result, DOM_NODE_ADOPTED);
	if (err != DOM_NO_ERR) {
		*result = NULL;
		return err;
	}

	parent = n->parent;
	if (parent != NULL) {
		err = dom_node_remove_child(parent, node, (void *) &tmp);
		if (err != DOM_NO_ERR) {
			dom_node_unref(*result);
			*result = NULL;
			return err;
		}
                dom_node_unref(tmp);
	}

	return DOM_NO_ERR;
}

/**
 * Retrieve the DOM configuration associated with a document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned object will have its reference count increased. It is
 * the responsibility of the caller to unref the object once it has
 * finished with it.
 */
dom_exception _dom_document_get_dom_config(dom_document *doc,
		struct dom_configuration **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Normalize a document
 *
 * \param doc  The document to normalize
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_normalize(dom_document *doc)
{
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Rename a node in a document
 *
 * \param doc        The document containing the node
 * \param node       The node to rename
 * \param namespace  The new namespace for the node
 * \param qname      The new qualified name for the node
 * \param result     Pointer to location to receive renamed node
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if \p tag_name is invalid,
 *         DOM_WRONG_DOCUMENT_ERR    if \p node was created in a different
 *                                   document
 *         DOM_NAMESPACE_ERR         if \p qname is malformed, or it has a
 *                                   prefix and \p namespace is NULL, or
 *                                   \p qname has a prefix "xml" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or \p qname has a prefix "xmlns" and
 *                                   \p namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   \p namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   \p qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if \p doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_rename_node(dom_document *doc,
		dom_node *node,
		dom_string *ns, dom_string *qname,
		dom_node **result)
{
	UNUSED(doc);
	UNUSED(node);
	UNUSED(ns);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_document_get_text_content(dom_node_internal *node,
					     dom_string **result)
{
	UNUSED(node);
	
	*result = NULL;
	
	return DOM_NO_ERR;
}

dom_exception _dom_document_set_text_content(dom_node_internal *node,
					     dom_string *content)
{
	UNUSED(node);
	UNUSED(content);
	
	return DOM_NO_ERR;
}

/*-----------------------------------------------------------------------*/

/* Overload protected virtual functions */

/* The virtual destroy function of this class */
void _dom_document_destroy(dom_node_internal *node)
{
	dom_document *doc = (dom_document *) node;

	if (_dom_document_finalise(doc) == true) {
		free(doc);
	}
}

/* The copy constructor function of this class */
dom_exception _dom_document_copy(dom_node_internal *old, 
		dom_node_internal **copy)
{
	UNUSED(old);
	UNUSED(copy);

	return DOM_NOT_SUPPORTED_ERR;
}


/* ----------------------------------------------------------------------- */

/* Helper functions */

/**
 * Get a nodelist, creating one if necessary
 *
 * \param doc        The document to get a nodelist for
 * \param type	     The type of the NodeList
 * \param root       Root node of subtree that list applies to
 * \param tagname    Name of nodes in list (or NULL)
 * \param ns         Namespace part of nodes in list (or NULL)
 * \param localname  Local part of nodes in list (or NULL)
 * \param list       Pointer to location to receive list
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_nodelist(dom_document *doc,
		nodelist_type type, dom_node_internal *root,
		dom_string *tagname, dom_string *ns,
		dom_string *localname, dom_nodelist **list)
{
	struct dom_doc_nl *l;
	dom_exception err;

	for (l = doc->nodelists; l; l = l->next) {
		if (_dom_nodelist_match(l->list, type, root, tagname,
				ns, localname))
			break;
	}

	if (l != NULL) {
		/* Found an existing list, so use it */
		dom_nodelist_ref(l->list);
	} else {
		/* No existing list */

		/* Create active list entry */
		l = (struct dom_doc_nl *)malloc(sizeof(struct dom_doc_nl));
		if (l == NULL)
			return DOM_NO_MEM_ERR;

		/* Create nodelist */
		err = _dom_nodelist_create(doc, type, root, tagname, ns,
				localname, &l->list);
		if (err != DOM_NO_ERR) {
			free(l);
			return err;
		}

		/* Add to document's list of active nodelists */
		l->prev = NULL;
		l->next = doc->nodelists;
		if (doc->nodelists)
			doc->nodelists->prev = l;
		doc->nodelists = l;
	}

	/* Note: the document does not claim a reference on the nodelist
	 * If it did, the nodelist's reference count would never reach zero,
	 * and the list would remain indefinitely. This is not a problem as
	 * the list notifies the document of its destruction via
	 * _dom_document_remove_nodelist. */

	*list = l->list;

	return DOM_NO_ERR;
}

/**
 * Remove a nodelist from a document
 *
 * \param doc   The document to remove the list from
 * \param list  The list to remove
 */
void _dom_document_remove_nodelist(dom_document *doc,
		dom_nodelist *list)
{
	struct dom_doc_nl *l;

	for (l = doc->nodelists; l; l = l->next) {
		if (l->list == list)
			break;
	}

	if (l == NULL) {
		/* This should never happen; we should probably abort here */
		return;
	}

	/* Remove from list */
	if (l->prev != NULL)
		l->prev->next = l->next;
	else
		doc->nodelists = l->next;

	if (l->next != NULL)
		l->next->prev = l->prev;

	/* And free item */
	free(l);
}

/**
 * Find element with certain ID in the subtree rooted at root 
 *
 * \param root    The root element from where we start
 * \param id      The ID of the target element
 * \param result  The result element
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_find_element_by_id(dom_node_internal *root, 
		dom_string *id, dom_element **result)
{
	dom_node_internal *node = root;

	*result = NULL;

	while (node != NULL) {
		if (node->type == DOM_ELEMENT_NODE) {
			dom_string *real_id;

			_dom_element_get_id((dom_element *) node, &real_id);
			if (real_id != NULL) {
				if (dom_string_isequal(real_id, id)) {
					dom_string_unref(real_id);
					*result = (dom_element *) node;
					return DOM_NO_ERR;
				}

				dom_string_unref(real_id);
			}
		}

		if (node->first_child != NULL) {
			/* Move to child */
			node = node->first_child;
		} else {
			while (node != NULL) {
				if (node->next != NULL) {
					/* Move to next sibling */
					node = node->next;
					break;
				} else if (node->parent != root) {
					/* Move back up to ancestors to
					 * get to next siblings */
					node = node->parent;
				} else {
					/* No more nodes below root. */
					node = NULL;
				}
			}
		}
	}

	return DOM_NO_ERR;
}

/**
 * Duplicate a Node
 *
 * \param doc     The documen
 * \param node    The node to duplicate
 * \param deep    Whether to make a deep copy
 * \param result  The returned node
 * \param opt     Whether this is adopt or import operation
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_document_dup_node(dom_document *doc, dom_node *node,
		bool deep, dom_node **result, dom_node_operation opt)
{
	dom_node_internal *n = (dom_node_internal *) node;
	dom_node_internal *ret;
	dom_exception err;
	dom_node_internal *child, *r;
	dom_user_data *ud;

	if (opt == DOM_NODE_ADOPTED && _dom_node_readonly(n))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	
	if (n->type == DOM_DOCUMENT_NODE ||
			n->type == DOM_DOCUMENT_TYPE_NODE)
		return DOM_NOT_SUPPORTED_ERR;

	err = dom_node_copy(node, &ret);
	if (err != DOM_NO_ERR)
		return err;

	if (n->type == DOM_ATTRIBUTE_NODE) {
		_dom_attr_set_specified((dom_attr *) node, true);
		deep = true;
	}

	if (n->type == DOM_ENTITY_REFERENCE_NODE) {
		deep = false;
	}

	if (n->type == DOM_ELEMENT_NODE) {
		/* Specified attributes are copyied but not default attributes,
		 * if the document object hold all the default attributes, we 
		 * have nothing to do here */
	}

	if (opt == DOM_NODE_ADOPTED && (n->type == DOM_ENTITY_NODE ||
			n->type == DOM_NOTATION_NODE)) {
		/* We did not support XML now */
		return DOM_NOT_SUPPORTED_ERR;
	}

	if (deep == true) {
		child = ((dom_node_internal *) node)->first_child;
		while (child != NULL) {
			err = dom_document_import_node(doc, child, deep,
					(void *) &r);
			if (err != DOM_NO_ERR) {
				dom_node_unref(ret);
				return err;
			}

			err = dom_node_append_child(ret, r, (void *) &r);
			if (err != DOM_NO_ERR) {
				dom_node_unref(ret);
				dom_node_unref(r);
				return err;
			}
			dom_node_unref(r);

			child = child->next;
		}
	}

	/* Call the dom_user_data_handlers */
	ud = n->user_data;
	while (ud != NULL) {
		if (ud->handler != NULL) {
			ud->handler(opt, ud->key, ud->data, node, 
					(dom_node *) ret);
		}
		ud = ud->next;
	}

	*result = (dom_node *) ret;

	return DOM_NO_ERR;
}

/**
 * Try to destroy the document. 
 *
 * \param doc  The instance of Document
 *
 * Delete the document if:
 * 1. The refcnt reach zero
 * 2. The pending list is empty
 *
 * else, do nothing.
 */
void _dom_document_try_destroy(dom_document *doc)
{
	if (doc->base.base.refcnt != 0 || doc->base.parent != NULL)
		return;

	_dom_document_destroy((dom_node_internal *) doc);
}

/**
 * Set the ID attribute name of this document
 *
 * \param doc   The document object
 * \param name  The ID name of the elements in this document
 */
void _dom_document_set_id_name(dom_document *doc, dom_string *name)
{
	if (doc->id_name != NULL)
		dom_string_unref(doc->id_name);
	doc->id_name = dom_string_ref(name);
}

/*-----------------------------------------------------------------------*/
/* Semi-internal API extensions for NetSurf */

dom_exception _dom_document_get_quirks_mode(dom_document *doc,
		dom_document_quirks_mode *result)
{
	*result = doc->quirks;
	return DOM_NO_ERR;
}

dom_exception _dom_document_set_quirks_mode(dom_document *doc,
		dom_document_quirks_mode quirks)
{
	doc->quirks = quirks;
	return DOM_NO_ERR;
}
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * DOM DocumentType node
 */
struct dom_document_type {
	dom_node_internal base;		/**< Base node */

	dom_string *public_id;	/**< Doctype public ID */
	dom_string *system_id;	/**< Doctype system ID */
};

static struct dom_document_type_vtable document_type_vtable = {
	{
		DOM_NODE_VTABLE_DOCUMENT_TYPE
	},
	DOM_DOCUMENT_TYPE_VTABLE
};

static struct dom_node_protect_vtable dt_protect_vtable = {
	DOM_DT_PROTECT_VTABLE
};


/*----------------------------------------------------------------------*/

/* Constructors and destructors */

/**
 * Create a document type node
 *
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param alloc      Memory (de)allocation function
 * \param pw         Pointer to client-specific private data
 * \param doctype    Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The doctype will be referenced, so the client need not do so
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception _dom_document_type_create(dom_string *qname,
		dom_string *public_id, dom_string *system_id,
		dom_document_type **doctype)
{
	dom_document_type *result;
	dom_exception err;

	/* Create node */
	result = (dom_document_type *)malloc(sizeof(dom_document_type));
	if (result == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtable */
	result->base.base.vtable = &document_type_vtable;
	result->base.vtable = &dt_protect_vtable;
	
	err = _dom_document_type_initialise(result, qname, 
			public_id, system_id);
	if (err != DOM_NO_ERR) {
		free(result);
		return err;
	}

	*doctype = result;

	return DOM_NO_ERR;
}

/**
 * Destroy a DocumentType node
 *
 * \param doctype  The DocumentType node to destroy
 *
 * The contents of ::doctype will be destroyed and ::doctype will be freed.
 */
void _dom_document_type_destroy(dom_node_internal *doctypenode)
{
	dom_document_type *doctype = (dom_document_type *) doctypenode;

	/* Finalise base class */
	_dom_document_type_finalise(doctype);

	/* Free doctype */
	free(doctype);
}

/* Initialise this document_type */
dom_exception _dom_document_type_initialise(dom_document_type *doctype,
		dom_string *qname, dom_string *public_id,
		dom_string *system_id)
{
	dom_string *prefix, *localname;
	dom_exception err;

	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR)
		return err;

	/* TODO: I should figure out how the namespaceURI can be got */

	/* Initialise base node */
	err = _dom_node_initialise(&doctype->base, NULL,
			DOM_DOCUMENT_TYPE_NODE, localname, NULL, NULL, prefix);
	if (err != DOM_NO_ERR) {
		dom_string_unref(prefix);
		dom_string_unref(localname);
		return err;
	}

	/* Get public and system IDs */
	if (public_id != NULL)
		dom_string_ref(public_id);
	doctype->public_id = public_id;

	if (system_id != NULL)
		dom_string_ref(system_id);
	doctype->system_id = system_id;

	if (prefix != NULL)
		dom_string_unref(prefix);
	if (localname != NULL)
		dom_string_unref(localname);

	return DOM_NO_ERR;
}

/* The destructor function of dom_document_type */
void _dom_document_type_finalise(dom_document_type *doctype)
{
	if (doctype->public_id != NULL)
		dom_string_unref(doctype->public_id);
	if (doctype->system_id != NULL)
		dom_string_unref(doctype->system_id);
	
	assert(doctype->base.owner != NULL || doctype->base.user_data == NULL);
	
	_dom_node_finalise(&doctype->base);
}


/*----------------------------------------------------------------------*/

/* Virtual functions */

/**
 * Retrieve a document type's name
 *
 * \param doc_type  Document type to retrieve name from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_name(dom_document_type *doc_type,
		dom_string **result)
{
	return dom_node_get_node_name(doc_type, result);
}

/**
 * Retrieve a document type's entities
 *
 * \param doc_type  Document type to retrieve entities from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_entities(
		dom_document_type *doc_type,
		dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's notations
 *
 * \param doc_type  Document type to retrieve notations from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_notations(
		dom_document_type *doc_type,
		dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's public id
 *
 * \param doc_type  Document type to retrieve public id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_public_id(
		dom_document_type *doc_type,
		dom_string **result)
{
	if (doc_type->public_id != NULL)
		*result = dom_string_ref(doc_type->public_id);
	else
		*result = NULL;
	
	return DOM_NO_ERR;
}

/**
 * Retrieve a document type's system id
 *
 * \param doc_type  Document type to retrieve system id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_system_id(
		dom_document_type *doc_type,
		dom_string **result)
{
	if (doc_type->system_id != NULL)
		*result = dom_string_ref(doc_type->system_id);
	else
		*result = NULL;
	
	return DOM_NO_ERR;
}

/**
 * Retrieve a document type's internal subset
 *
 * \param doc_type  Document type to retrieve internal subset from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_internal_subset(
		dom_document_type *doc_type,
		dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_document_type_get_text_content(dom_node_internal *node,
                                                  dom_string **result)
{
	UNUSED(node);
	
	*result = NULL;
	
	return DOM_NO_ERR;
}

dom_exception _dom_document_type_set_text_content(dom_node_internal *node,
                                                  dom_string *content)
{
	UNUSED(node);
	UNUSED(content);
	
	return DOM_NO_ERR;
}

/*-----------------------------------------------------------------------*/

/* Overload protected virtual functions */

/* The virtual destroy function of this class */
void _dom_dt_destroy(dom_node_internal *node)
{
	_dom_document_type_destroy(node);
}

/* The copy constructor of this class */
dom_exception _dom_dt_copy(dom_node_internal *old, dom_node_internal **copy)
{
	UNUSED(old);
	UNUSED(copy);

	return DOM_NOT_SUPPORTED_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

struct dom_element_vtable _dom_element_vtable = {
	{
		DOM_NODE_VTABLE_ELEMENT
	},
	DOM_ELEMENT_VTABLE
};

static struct dom_element_protected_vtable element_protect_vtable = {
	{
		DOM_NODE_PROTECT_VTABLE_ELEMENT
	},
	DOM_ELEMENT_PROTECT_VTABLE
};


typedef struct dom_attr_list {
	struct list_entry list; /**< Linked list links to prev/next entries */
	
	struct dom_attr *attr;

	struct dom_string *name;
	struct dom_string *ns;
} dom_attr_list;


/**
 * Destroy element's class cache
 *
 * \param ele  The element
 */
static void _dom_element_destroy_classes(struct dom_element *ele)
{
	/* Destroy the pre-separated class names */
	if (ele->classes != NULL) {
		unsigned int cls;
		for (cls = 0; cls < ele->n_classes; cls++) {
			lwc_string_unref(ele->classes[cls]);
		}
		free(ele->classes);
	}

	ele->n_classes = 0;
	ele->classes = NULL;
}


/**
 * Create element's class cache from class attribute value
 *
 * \param ele    The element
 * \param value  The class attribute value
 *
 * Destroys any existing cached classes.
 */
static dom_exception _dom_element_create_classes(struct dom_element *ele,
		const char *value)
{
	const char *pos;
	lwc_string **classes = NULL;
	uint32_t n_classes = 0;

	/* Any existing cached classes are replaced; destroy them */
	_dom_element_destroy_classes(ele);

	/* Count number of classes */
	for (pos = value; *pos != '\0'; ) {
		if (*pos != ' ') {
			while (*pos != ' ' && *pos != '\0')
				pos++;
			n_classes++;
		} else {
			while (*pos == ' ')
				pos++;
		}
	}

	/* If there are some, unpack them */
	if (n_classes > 0) {
		classes = (lwc_string **)malloc(n_classes * sizeof(lwc_string *));
		if (classes == NULL)
			return DOM_NO_MEM_ERR;

		for (pos = value, n_classes = 0;
				*pos != '\0'; ) {
			if (*pos != ' ') {
				const char *s = pos;
				while (*pos != ' ' && *pos != '\0')
					pos++;
				if (lwc_intern_string(s, pos - s, 
						&classes[n_classes++]) !=
						lwc_error_ok)
					goto error;
			} else {
				while (*pos == ' ')
					pos++;
			}
		}
	}

	ele->n_classes = n_classes;
	ele->classes = classes;

	return DOM_NO_ERR;
error:
	while (n_classes > 0)
		lwc_string_unref(classes[--n_classes]);

	free(classes);
		
	return DOM_NO_MEM_ERR;
}

/* Attribute linked list releated functions */

/**
 * Get the next attribute in the list
 *
 * \param n  The attribute list node
 * \return The next attribute node
 */
static dom_attr_list * _dom_element_attr_list_next(const dom_attr_list *n)
{
	return (dom_attr_list *)(n->list.next);
}

/**
 * Unlink an attribute list node from its linked list
 *
 * \param n  The attribute list node
 */
static void _dom_element_attr_list_node_unlink(dom_attr_list *n)
{
	if (n == NULL)
		return;

	list_del(&n->list);
}

/**
 * Insert attribute list node into attribute list
 *
 * \param list      The list header
 * \param new_attr  The attribute node to insert
 */
static void _dom_element_attr_list_insert(dom_attr_list *list,
		dom_attr_list *new_attr)
{
	assert(list != NULL);
	assert(new_attr != NULL);
	list_append(&list->list, &new_attr->list);
}

/**
 * Get attribute from attribute list, which matches given name
 *
 * \param list  The attribute list to search
 * \param name  The name of the attribute to search for
 * \param ns    The namespace of the attribute to search for (may be NULL)
 * \return the matching attribute, or NULL if none found
 */
static dom_attr_list * _dom_element_attr_list_find_by_name(
		dom_attr_list *list, dom_string *name, dom_string *ns)
{
	dom_attr_list *attr = list;

	if (list == NULL || name == NULL)
		return NULL;

	do {
		if (((ns == NULL && attr->ns == NULL) ||
				(ns != NULL && attr->ns != NULL &&
						dom_string_isequal(ns,
						attr->ns))) &&
				dom_string_isequal(name, attr->name)) {
			/* Both have NULL namespace or matching namespace,
			 * and both have same name */
			return attr;
		}

		attr = _dom_element_attr_list_next(attr);
		assert(attr != NULL);
	} while (attr != list);

	return NULL;
}

/**
 * Get the number of elements in this attribute list
 *
 * \param list  The attribute list
 * \return the number attribute list node elements
 */
static unsigned int _dom_element_attr_list_length(dom_attr_list *list)
{
	dom_attr_list *attr = list;
	unsigned int count = 0;

	if (list == NULL)
		return count;

	do {
		count++;

		attr = _dom_element_attr_list_next(attr);
	} while (attr != list);

	return count;
}

/**
 * Get the attribute list node at the given index
 *
 * \param list   The attribute list
 * \param index  The index number
 * \return the attribute list node at given index
 */
static dom_attr_list * _dom_element_attr_list_get_by_index(dom_attr_list *list,
		unsigned int index)
{
	dom_attr_list *attr = list;

	if (list == NULL)
		return NULL;

	do {
		if (--index == 0)
			return attr;

		attr = _dom_element_attr_list_next(attr);
	} while (attr != list);

	return NULL;
}

/**
 * Destroy an attribute list node, and its attribute
 *
 * \param n  The attribute list node to destroy
 */
static void _dom_element_attr_list_node_destroy(dom_attr_list *n)
{
	dom_node_internal *a;
	dom_document *doc;

	assert(n != NULL);
	assert(n->attr != NULL);
	assert(n->name != NULL);

	a = (dom_node_internal *) n->attr;

	/* Need to destroy classes cache, when removing class attribute */
	doc = a->owner;
	if (n->ns == NULL &&
			dom_string_isequal(n->name, doc->class_string)) {
		_dom_element_destroy_classes((dom_element *)(a->parent));
	}

	/* Destroy rest of list node */
	dom_string_unref(n->name);

	if (n->ns != NULL)
		dom_string_unref(n->ns);

	a->parent = NULL;
	dom_node_try_destroy(a);

	free(n);
}

/**
 * Create an attribute list node
 *
 * \param attr       The attribute to create a list node for
 * \param name       The attribute name
 * \param ns         The attribute namespace (may be NULL)
 * \return the new attribute list node, or NULL on failure
 */
static dom_attr_list * _dom_element_attr_list_node_create(dom_attr *attr,
		dom_element *ele, dom_string *name, dom_string *ns)
{
	dom_attr_list *new_list_node;
	dom_node_internal *a;
	dom_document *doc;

	if (attr == NULL || name == NULL)
		return NULL;

	new_list_node = (dom_attr_list *)malloc(sizeof(*new_list_node));
	if (new_list_node == NULL)
		return NULL;

	list_init(&new_list_node->list);

	new_list_node->attr = attr;
	new_list_node->name = name;
	new_list_node->ns = ns;

	a = (dom_node_internal *) attr;
	doc = a->owner;
	if (ns == NULL &&
			dom_string_isequal(name, doc->class_string)) {
		dom_string *value;

		if (DOM_NO_ERR != _dom_attr_get_value(attr, &value)) {
			_dom_element_attr_list_node_destroy(new_list_node);
			return NULL;
		}

		if (DOM_NO_ERR != _dom_element_create_classes(ele,
				dom_string_data(value))) {
			_dom_element_attr_list_node_destroy(new_list_node);
			dom_string_unref(value);
			return NULL;
		}

		dom_string_unref(value);
	}

	return new_list_node;
}

/**
 * Destroy an entire attribute list, and its attributes
 *
 * \param list  The attribute list to destroy
 */
static void _dom_element_attr_list_destroy(dom_attr_list *list)
{
	dom_attr_list *attr = list;
	dom_attr_list *next = list;

	if (list == NULL)
		return;

	do {
		attr = next;
		next = _dom_element_attr_list_next(attr);

		_dom_element_attr_list_node_unlink(attr);
		_dom_element_attr_list_node_destroy(attr);
	} while (next != attr);

	return;
}

/**
 * Clone an attribute list node, and its attribute
 *
 * \param n     The attribute list node to clone
 * \param newe  Element to clone attribute for
 * \return the new attribute list node, or NULL on failure
 */
static dom_attr_list *_dom_element_attr_list_node_clone(dom_attr_list *n,
		dom_element *newe)
{
	dom_attr *clone = NULL;
	dom_attr_list *new_list_node;
	dom_exception err;

	assert(n != NULL);
	assert(n->attr != NULL);
	assert(n->name != NULL);

	new_list_node = (dom_attr_list *)malloc(sizeof(*new_list_node));
	if (new_list_node == NULL)
		return NULL;

	list_init(&new_list_node->list);

	new_list_node->name = NULL;
	new_list_node->ns = NULL;

	err = dom_node_clone_node(n->attr, true, (void *) &clone);
	if (err != DOM_NO_ERR) {
		free(new_list_node);
		return NULL;
	}

	dom_node_set_parent(clone, newe);
	dom_node_remove_pending(clone);
	dom_node_unref(clone);
	new_list_node->attr = clone;

	if (n->name != NULL)
		new_list_node->name = dom_string_ref(n->name);

	if (n->ns != NULL)
		new_list_node->ns = dom_string_ref(n->ns);

	return new_list_node;
}

/**
 * Clone an entire attribute list, and its attributes
 *
 * \param list  The attribute list to clone
 * \param newe  Element to clone list for
 * \return the new attribute list, or NULL on failure
 */
static dom_attr_list *_dom_element_attr_list_clone(dom_attr_list *list,
		dom_element *newe)
{
	dom_attr_list *attr = list;

	dom_attr_list *new_list = NULL;
	dom_attr_list *new_list_node = NULL;

	if (list == NULL)
		return NULL;

	do {
		new_list_node = _dom_element_attr_list_node_clone(attr, newe);
		if (new_list_node == NULL) {
			if (new_list != NULL)
				_dom_element_attr_list_destroy(new_list);
			return NULL;
		}

		if (new_list == NULL) {
			new_list = new_list_node;
		} else {
			_dom_element_attr_list_insert(new_list, new_list_node);
		}

		attr = _dom_element_attr_list_next(attr);
	} while (attr != list);

	return new_list;
}

static dom_exception _dom_element_get_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, dom_string **value);
static dom_exception _dom_element_set_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, dom_string *value);
static dom_exception _dom_element_remove_attr(struct dom_element *element,
		dom_string *ns, dom_string *name);

static dom_exception _dom_element_get_attr_node(struct dom_element *element,
		dom_string *ns, dom_string *name,
		struct dom_attr **result);
static dom_exception _dom_element_set_attr_node(struct dom_element *element,
		dom_string *ns, struct dom_attr *attr,
		struct dom_attr **result);
static dom_exception _dom_element_remove_attr_node(struct dom_element *element,
		dom_string *ns, struct dom_attr *attr,
		struct dom_attr **result);

static dom_exception _dom_element_has_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, bool *result);
static dom_exception _dom_element_set_id_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, bool is_id);


/* The operation set for namednodemap */
static dom_exception attributes_get_length(void *priv,
		uint32_t *length);
static dom_exception attributes_get_named_item(void *priv,
		dom_string *name, struct dom_node **node);
static dom_exception attributes_set_named_item(void *priv,
		struct dom_node *arg, struct dom_node **node);
static dom_exception attributes_remove_named_item(
		void *priv, dom_string *name,
		struct dom_node **node);
static dom_exception attributes_item(void *priv,
		uint32_t index, struct dom_node **node);
static dom_exception attributes_get_named_item_ns(
		void *priv, dom_string *ns,
		dom_string *localname, struct dom_node **node);
static dom_exception attributes_set_named_item_ns(
		void *priv, struct dom_node *arg,
		struct dom_node **node);
static dom_exception attributes_remove_named_item_ns(
		void *priv, dom_string *ns,
		dom_string *localname, struct dom_node **node);
static void attributes_destroy(void *priv);
static bool attributes_equal(void *p1, void *p2);

static struct nnm_operation attributes_opt = {
	attributes_get_length,
	attributes_get_named_item,
	attributes_set_named_item,
	attributes_remove_named_item,
	attributes_item,
	attributes_get_named_item_ns,
	attributes_set_named_item_ns,
	attributes_remove_named_item_ns,
	attributes_destroy,
	attributes_equal
};

/*----------------------------------------------------------------------*/
/* Constructors and Destructors */

/**
 * Create an element node
 *
 * \param doc        The owning document
 * \param name       The (local) name of the node to create
 * \param ns         The namespace URI of the element, or NULL
 * \param prefix     The namespace prefix of the element, or NULL
 * \param result     Pointer to location to receive created element
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name, ::namespace and ::prefix will have their 
 * reference counts increased.
 *
 * The returned element will already be referenced.
 */
dom_exception _dom_element_create(struct dom_document *doc,
		dom_string *name, dom_string *ns,
		dom_string *prefix, struct dom_element **result)
{
	/* Allocate the element */
	*result = (struct dom_element *)malloc(sizeof(struct dom_element));
	if (*result == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtables */
	(*result)->base.base.vtable = &_dom_element_vtable;
	(*result)->base.vtable = &element_protect_vtable;

	return _dom_element_initialise(doc, *result, name, ns, prefix);
}

/**
 * Initialise an element node
 *
 * \param doc        The owning document
 * \param el	     The element
 * \param name       The (local) name of the node to create
 * \param ns         The namespace URI of the element, or NULL
 * \param prefix     The namespace prefix of the element, or NULL
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * The caller should make sure that ::name is a valid NCName.
 *
 * ::doc, ::name, ::namespace and ::prefix will have their 
 * reference counts increased.
 *
 * The returned element will already be referenced.
 */
dom_exception _dom_element_initialise(struct dom_document *doc,
		struct dom_element *el, dom_string *name, 
		dom_string *ns, dom_string *prefix)
{
	dom_exception err;

	assert(doc != NULL);

	el->attributes = NULL;

	/* Initialise the base class */
	err = _dom_node_initialise(&el->base, doc, DOM_ELEMENT_NODE,
			name, NULL, ns, prefix);
	if (err != DOM_NO_ERR) {
		free(el);
		return err;
	}

	/* Perform our type-specific initialisation */
	el->id_ns = NULL;
	el->id_name = NULL;
	el->schema_type_info = NULL;

	el->n_classes = 0;
	el->classes = NULL;

	return DOM_NO_ERR;
}

/**
 * Finalise a dom_element
 *
 * \param ele  The element
 */
void _dom_element_finalise(struct dom_element *ele)
{
	/* Destroy attributes attached to this node */
	if (ele->attributes != NULL) {
		_dom_element_attr_list_destroy(ele->attributes);
		ele->attributes = NULL;
	}

	if (ele->schema_type_info != NULL) {
		/** \todo destroy schema type info */
	}

	/* Destroy the pre-separated class names */
	_dom_element_destroy_classes(ele);

	/* Finalise base class */
	_dom_node_finalise(&ele->base);
}

/**
 * Destroy an element
 *
 * \param element  The element to destroy
 *
 * The contents of ::element will be destroyed and ::element will be freed.
 */
void _dom_element_destroy(struct dom_element *element)
{
	_dom_element_finalise(element);

	/* Free the element */
	free(element);
}

/*----------------------------------------------------------------------*/

/* The public virtual functions */

/**
 * Retrieve an element's tag name
 *
 * \param element  The element to retrieve the name from
 * \param name     Pointer to location to receive name
 * \return DOM_NO_ERR      on success,
 *         DOM_NO_MEM_ERR  on memory exhaustion.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_tag_name(struct dom_element *element,
		dom_string **name)
{
	/* This is the same as nodeName */
	return dom_node_get_node_name((struct dom_node *) element, name);
}

/**
 * Retrieve an attribute from an element by name
 *
 * \param element  The element to retrieve attribute from
 * \param name     The attribute's name
 * \param value    Pointer to location to receive attribute's value
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute(struct dom_element *element,
		dom_string *name, dom_string **value)
{
	return _dom_element_get_attr(element, NULL, name, value);
}

/**
 * Set an attribute on an element by name
 *
 * \param element  The element to set attribute on
 * \param name     The attribute's name
 * \param value    The attribute's value
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if ::name is invalid,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_set_attribute(struct dom_element *element,
		dom_string *name, dom_string *value)
{
	return _dom_element_set_attr(element, NULL, name, value);
}

/**
 * Remove an attribute from an element by name
 *
 * \param element  The element to remove attribute from
 * \param name     The name of the attribute to remove
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_remove_attribute(struct dom_element *element,
		dom_string *name)
{
	return _dom_element_remove_attr(element, NULL, name);
}

/**
 * Retrieve an attribute node from an element by name
 *
 * \param element  The element to retrieve attribute node from
 * \param name     The attribute's name
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_node(struct dom_element *element, 
		dom_string *name, struct dom_attr **result)
{
	return _dom_element_get_attr_node(element, NULL, name, result);
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to receive previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	return _dom_element_set_attr_node(element, NULL, attr, result);
}

/**
 * Remove an attribute node from an element
 *
 * \param element  The element to remove attribute node from
 * \param attr     The attribute node to remove
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if ::attr is not an attribute of
 *                                         ::element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_remove_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	return _dom_element_remove_attr_node(element, NULL, attr, result);
}

/**
 * Retrieve a list of descendant elements of an element which match a given
 * tag name
 *
 * \param element  The root of the subtree to search
 * \param name     The tag name to match (or "*" for all tags)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned nodelist will have its reference count increased. It is
 * the responsibility of the caller to unref the nodelist once it has
 * finished with it.
 */
dom_exception _dom_element_get_elements_by_tag_name(
		struct dom_element *element, dom_string *name,
		struct dom_nodelist **result)
{
	dom_exception err;
	dom_node_internal *base = (dom_node_internal *) element;
	
	assert(base->owner != NULL);

	err = _dom_document_get_nodelist(base->owner, DOM_NODELIST_BY_NAME,
			(struct dom_node_internal *) element, name, NULL, 
			NULL, result);

	return err;
}

/**
 * Retrieve an attribute from an element by namespace/localname
 *
 * \param element    The element to retrieve attribute from
 * \param ns         The attribute's namespace URI, or NULL
 * \param localname  The attribute's local name
 * \param value      Pointer to location to receive attribute's value
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		dom_string **value)
{
	return _dom_element_get_attr(element, ns, localname, value);
}

/**
 * Set an attribute on an element by namespace/qualified name
 *
 * \param element    The element to set attribute on
 * \param ns         The attribute's namespace URI
 * \param qname      The attribute's qualified name
 * \param value      The attribute's value
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if ::qname is invalid,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NAMESPACE_ERR               if ::qname is malformed, or
 *                                         ::qname has a prefix and
 *                                         ::namespace is null, or ::qname
 *                                         has a prefix "xml" and
 *                                         ::namespace is not
 *                                         "http://www.w3.org/XML/1998/namespace",
 *                                         or ::qname has a prefix "xmlns"
 *                                         and ::namespace is not
 *                                         "http://www.w3.org/2000/xmlns",
 *                                         or ::namespace is
 *                                         "http://www.w3.org/2000/xmlns"
 *                                         and ::qname is not prefixed
 *                                         "xmlns",
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 */
dom_exception _dom_element_set_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *qname,
		dom_string *value)
{
	dom_exception err;
	dom_string *localname;
	dom_string *prefix;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_namespace_validate_qname(qname, ns);
	if (err != DOM_NO_ERR)
		return DOM_NAMESPACE_ERR;

	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR)
		return err;

	/* If there is no namespace, must have a prefix */
	if (ns == NULL && prefix != NULL) {
		dom_string_unref(prefix);
		dom_string_unref(localname);
		return DOM_NAMESPACE_ERR;
	}

	err = _dom_element_set_attr(element, ns, localname, value);

	dom_string_unref(prefix);
	dom_string_unref(localname);

	return err;
}

/**
 * Remove an attribute from an element by namespace/localname
 *
 * \param element    The element to remove attribute from
 * \param ns         The attribute's namespace URI
 * \param localname  The attribute's local name
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 */
dom_exception _dom_element_remove_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname)
{
	return _dom_element_remove_attr(element, ns, localname);
}

/**
 * Retrieve an attribute node from an element by namespace/localname
 *
 * \param element    The element to retrieve attribute from
 * \param ns         The attribute's namespace URI
 * \param localname  The attribute's local name
 * \param result     Pointer to location to receive attribute node
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_node_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		struct dom_attr **result)
{
	return _dom_element_get_attr_node(element, ns, localname, result);
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to recieve previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attribute_node_ns(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	dom_exception err;
	dom_string *ns;

	err = dom_node_get_namespace(attr, &ns);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_element_set_attr_node(element, ns, attr, result);

	if (ns != NULL)
		dom_string_unref(ns);

	return err;
}

/**
 * Retrieve a list of descendant elements of an element which match a given
 * namespace/localname pair.
 *
 * \param element  The root of the subtree to search
 * \param ns       The namespace URI to match (or "*" for all)
 * \param localname  The local name to match (or "*" for all)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned nodelist will have its reference count increased. It is
 * the responsibility of the caller to unref the nodelist once it has
 * finished with it.
 */
dom_exception _dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, dom_string *ns,
		dom_string *localname, struct dom_nodelist **result)
{
	dom_exception err;

	/** \todo ensure XML feature is supported */

	err = _dom_document_get_nodelist(element->base.owner,
			DOM_NODELIST_BY_NAMESPACE,
			(struct dom_node_internal *) element, NULL, 
			ns, localname, result);

	return err;
}

/**
 * Determine if an element possesses and attribute with the given name
 *
 * \param element  The element to query
 * \param name     The attribute name to look for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_element_has_attribute(struct dom_element *element,
		dom_string *name, bool *result)
{
	return _dom_element_has_attr(element, NULL, name, result);
}

/**
 * Determine if an element possesses and attribute with the given
 * namespace/localname pair.
 *
 * \param element    The element to query
 * \param ns  The attribute namespace URI to look for
 * \param localname  The attribute local name to look for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 */
dom_exception _dom_element_has_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		bool *result)
{
	return _dom_element_has_attr(element, ns, localname, result);
}

/**
 * Retrieve the type information associated with an element
 *
 * \param element  The element to retrieve type information from
 * \param result   Pointer to location to receive type information
 * \return DOM_NO_ERR.
 *
 * The returned typeinfo will have its reference count increased. It is
 * the responsibility of the caller to unref the typeinfo once it has
 * finished with it.
 */
dom_exception _dom_element_get_schema_type_info(struct dom_element *element,
		struct dom_type_info **result)
{
	UNUSED(element);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * (Un)declare an attribute as being an element's ID by name
 *
 * \param element  The element containing the attribute
 * \param name     The attribute's name
 * \param is_id    Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 *
 * @note: The DOM spec does not say: how to deal with when there are two or 
 * more isId attribute nodes. Here, the implementation just maintain only
 * one such attribute node.
 */
dom_exception _dom_element_set_id_attribute(struct dom_element *element,
		dom_string *name, bool is_id)
{
	return _dom_element_set_id_attr(element, NULL, name, is_id);
}

/**
 * (Un)declare an attribute as being an element's ID by namespace/localname
 *
 * \param element    The element containing the attribute
 * \param ns         The attribute's namespace URI
 * \param localname  The attribute's local name
 * \param is_id      Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 */
dom_exception _dom_element_set_id_attribute_ns(struct dom_element *element,
		dom_string *ns, dom_string *localname,
		bool is_id)
{
	dom_exception err;

	err = _dom_element_set_id_attr(element, ns, localname, is_id);
	
	element->id_ns = dom_string_ref(ns);

	return err;
}

/**
 * (Un)declare an attribute node as being an element's ID
 *
 * \param element  The element containing the attribute
 * \param id_attr  The attribute node
 * \param is_id    Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 */
dom_exception _dom_element_set_id_attribute_node(struct dom_element *element,
		struct dom_attr *id_attr, bool is_id)
{
	dom_exception err;
	dom_string *ns;
	dom_string *localname;

	err = dom_node_get_namespace(id_attr, &ns);
	if (err != DOM_NO_ERR)
		return err;
	err = dom_node_get_local_name(id_attr, &localname);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_element_set_id_attr(element, ns, localname, is_id);
	if (err != DOM_NO_ERR)
		return err;
	
	element->id_ns = ns;

	return DOM_NO_ERR;

}

/**
 * Obtain a pre-parsed array of class names for an element
 *
 * \param element    Element containing classes
 * \param classes    Pointer to location to receive libdom-owned array
 * \param n_classes  Pointer to location to receive number of classes
 * \return DOM_NO_ERR on success,
 *         DOM_NO_MEM_ERR on memory exhaustion
 */
dom_exception _dom_element_get_classes(struct dom_element *element,
		lwc_string ***classes, uint32_t *n_classes)
{	
	if (element->n_classes > 0) {
		uint32_t classnr;

		*classes = element->classes;
		*n_classes = element->n_classes;

		for (classnr = 0; classnr < element->n_classes; classnr++)
			(void) lwc_string_ref((*classes)[classnr]);

	} else {
		*n_classes = 0;
		*classes = NULL;
	}

	return DOM_NO_ERR;
}

/**
 * Determine if an element has an associated class
 *
 * \param element  Element to consider
 * \param name     Class name to look for
 * \param match    Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_element_has_class(struct dom_element *element,
		lwc_string *name, bool *match)
{
	dom_exception err;
	unsigned int cls;
	struct dom_node_internal *node = (struct dom_node_internal *)element;
	dom_document_quirks_mode quirks_mode;
	
	/* Short-circuit case where we have no classes */
	if (element->n_classes == 0) {
		*match = false;
		return DOM_NO_ERR;
	}

	err = dom_document_get_quirks_mode(node->owner, &quirks_mode);
	if (err != DOM_NO_ERR)
		return err;

	if (quirks_mode != DOM_DOCUMENT_QUIRKS_MODE_NONE) {
		/* Quirks mode: case insensitively match */
		for (cls = 0; cls < element->n_classes; cls++) {
			if (lwc_error_ok == lwc_string_caseless_isequal(name,
					element->classes[cls], match) &&
					*match == true)
				return DOM_NO_ERR;
		}
	} else {
		/* Standards mode: case sensitively match */
		for (cls = 0; cls < element->n_classes; cls++) {
			if (lwc_error_ok == lwc_string_isequal(name,
					element->classes[cls], match) &&
					*match == true)
				return DOM_NO_ERR;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Get a named ancestor node
 *
 * \param element   Element to consider
 * \param name      Node name to look for
 * \param ancestor  Pointer to location to receive node pointer
 * \return DOM_NO_ERR.
 */
dom_exception dom_element_named_ancestor_node(dom_element *element,
		lwc_string *name, dom_element **ancestor)
{
	dom_node_internal *node = (dom_node_internal *)element;

	*ancestor = NULL;

	for (node = node->parent; node != NULL; node = node->parent) {
		if (node->type != DOM_ELEMENT_NODE)
			continue;

		assert(node->name != NULL);

		if (dom_string_caseless_lwc_isequal(node->name, name)) {
			*ancestor = (dom_element *)node;
			break;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Get a named parent node
 *
 * \param element  Element to consider
 * \param name     Node name to look for
 * \param parent   Pointer to location to receive node pointer
 * \return DOM_NO_ERR.
 */
dom_exception dom_element_named_parent_node(dom_element *element,
		lwc_string *name, dom_element **parent)
{
	dom_node_internal *node = (dom_node_internal *)element;

	*parent = NULL;

	for (node = node->parent; node != NULL; node = node->parent) {
		if (node->type != DOM_ELEMENT_NODE)
			continue;

		assert(node->name != NULL);

		if (dom_string_caseless_lwc_isequal(node->name, name)) {
			*parent = (dom_element *)node;
		}
		break;
	}

	return DOM_NO_ERR;
}

/**
 * Get a named parent node
 *
 * \param element  Element to consider
 * \param name     Node name to look for
 * \param parent   Pointer to location to receive node pointer
 * \return DOM_NO_ERR.
 */
dom_exception dom_element_parent_node(dom_element *element,
		dom_element **parent)
{
	dom_node_internal *node = (dom_node_internal *)element;

	*parent = NULL;

	for (node = node->parent; node != NULL; node = node->parent) {
		if (node->type != DOM_ELEMENT_NODE)
			continue;

		*parent = (dom_element *)node;
		break;
	}

	return DOM_NO_ERR;
}

/*------------- The overload virtual functions ------------------------*/

/* Overload function of Node, please refer src/core/node.c for detail */
dom_exception _dom_element_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result)
{
	dom_exception err;
	dom_document *doc;

	doc = dom_node_get_owner(node);
	assert(doc != NULL);

	err = _dom_namednodemap_create(doc, node, &attributes_opt, result);
	if (err != DOM_NO_ERR)
		return err;
	
	dom_node_ref(node);
	
	return DOM_NO_ERR;
}

/* Overload function of Node, please refer src/core/node.c for detail */
dom_exception _dom_element_has_attributes(dom_node_internal *node, bool *result)
{
	UNUSED(node);
	*result = true;

	return DOM_NO_ERR;
}

/* For the following namespace related algorithm take a look at:
 * http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/namespaces-algorithms.html
 */

/**
 * Look up the prefix which matches the namespace.
 *
 * \param node       The current Node in which we search for
 * \param ns         The namespace for which we search a prefix
 * \param result     The returned prefix
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	return dom_node_lookup_prefix(owner, ns, result);
}

/**
 * Test whether certain namespace is the default namespace of some node.
 *
 * \param node       The Node to test
 * \param ns         The namespace to test
 * \param result     true is the namespace is default namespace
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result)
{
	struct dom_element *ele = (struct dom_element *) node;
	dom_string *value;
	dom_exception err;
	bool has;
	dom_string *xmlns;

	if (node->prefix == NULL) {
		*result = dom_string_isequal(node->ns, ns);
		return DOM_NO_ERR;
	}

	xmlns = _dom_namespace_get_xmlns_prefix();
	err = dom_element_has_attribute(ele, xmlns, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true) {
		err = dom_element_get_attribute(ele, xmlns, &value);
		if (err != DOM_NO_ERR)
			return err;

		*result = dom_string_isequal(value, ns);

		dom_string_unref(value);

		return DOM_NO_ERR;
	}

	return dom_node_is_default_namespace(node->parent, ns, result);
}

/**
 * Look up the namespace with certain prefix.
 *
 * \param node    The current node in which we search for the prefix
 * \param prefix  The prefix to search
 * \param result  The result namespace if found
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result)
{
	dom_exception err;
	bool has;
	dom_string *xmlns;

	if (node->ns != NULL &&
			dom_string_isequal(node->prefix, prefix)) {
		*result = dom_string_ref(node->ns);
		return DOM_NO_ERR;
	}
	
	xmlns = _dom_namespace_get_xmlns_prefix();
	err = dom_element_has_attribute_ns(node, xmlns, prefix, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true)
		return dom_element_get_attribute_ns(node,
				dom_namespaces[DOM_NAMESPACE_XMLNS], prefix,
				result);

	err = dom_element_has_attribute(node, xmlns, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true) {
		return dom_element_get_attribute(node, xmlns, result);
	}

	return dom_node_lookup_namespace(node->parent, prefix, result);
}


/*----------------------------------------------------------------------*/
/* The protected virtual functions */

/**
 * The virtual function to parse some dom attribute
 *
 * \param ele     The element object
 * \param name    The name of the attribute
 * \param value   The new value of the attribute
 * \param parsed  The parsed value of the attribute
 * \return DOM_NO_ERR on success.
 *
 * @note: This virtual method is provided to serve as a template method. 
 * When any attribute is set or added, the attribute's value should be
 * checked to make sure that it is a valid one. And the child class of 
 * dom_element may to do some special stuff on the attribute is set. Take
 * some integer attribute as example:
 *
 * 1. The client call dom_element_set_attribute("size", "10.1"), but the
 *    size attribute may only accept an integer, and only the specific
 *    dom_element know this. And the dom_attr_set_value method, which is
 *    called by dom_element_set_attribute should call the this virtual
 *    template method.
 * 2. The overload virtual function of following one will truncate the
 *    "10.1" to "10" to make sure it is a integer. And of course, the 
 *    overload method may also save the integer as a 'int' C type for 
 *    later easy accessing by any client.
 */
dom_exception _dom_element_parse_attribute(dom_element *ele,
		dom_string *name, dom_string *value,
		dom_string **parsed)
{
	UNUSED(ele);
	UNUSED(name);

	dom_string_ref(value);
	*parsed = value;

	return DOM_NO_ERR;
}

/* The destroy virtual function of dom_element */
void __dom_element_destroy(struct dom_node_internal *node)
{
	_dom_element_destroy((struct dom_element *) node);
}

/* TODO: How to deal with default attribue:
 *
 *  Ask a language binding for default attributes.	
 *
 *	So, when we copy a element we copy all its attributes because they
 *	are all specified. For the methods like importNode and adoptNode, 
 *	this will make _dom_element_copy can be used in them.
 */
dom_exception _dom_element_copy(dom_node_internal *old,
		dom_node_internal **copy)
{
	dom_element *new_node;
	dom_exception err;

	new_node = (dom_element *)malloc(sizeof(dom_element));
	if (new_node == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_element_copy_internal(old, new_node);
	if (err != DOM_NO_ERR) {
		free(new_node);
		return err;
	}

	*copy = (dom_node_internal *) new_node;

	return DOM_NO_ERR;
}

dom_exception _dom_element_copy_internal(dom_element *old, dom_element *newe)
{
	dom_exception err;
	uint32_t classnr;

	if (old->attributes != NULL) {
		/* Copy the attribute list */
		newe->attributes = _dom_element_attr_list_clone(
				old->attributes, newe);
	} else {
		newe->attributes = NULL;
	}

	if (old->n_classes > 0) {
		newe->n_classes = old->n_classes;
		newe->classes = (lwc_string **)malloc(sizeof(lwc_string *) * newe->n_classes);
		if (newe->classes == NULL) {
			err = DOM_NO_MEM_ERR;
			goto error;
		}
		for (classnr = 0; classnr < newe->n_classes; ++classnr)
			newe->classes[classnr] =
				lwc_string_ref(old->classes[classnr]);
	} else {
		newe->n_classes = 0;
		newe->classes = NULL;
	}

	err = dom_node_copy_internal(old, newe);
	if (err != DOM_NO_ERR) {
		goto error;
	}

	newe->id_ns = NULL;
	newe->id_name = NULL;

	/* TODO: deal with dom_type_info, it get no definition ! */

	return DOM_NO_ERR;

error:
	free(newe->classes);
	return err;
}



/*--------------------------------------------------------------------------*/

/* Helper functions */

/**
 * The internal helper function for getAttribute/getAttributeNS.
 *
 * \param element    The element
 * \param ns         The namespace to look for attribute in.  May be NULL.
 * \param name       The name of the attribute
 * \param value      The value of the attribute
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_get_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, dom_string **value)
{
	dom_attr_list *match;
	dom_exception err = DOM_NO_ERR;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	/* Fill in value */
	if (match == NULL) {
		*value = NULL;
	} else {
		err = dom_attr_get_value(match->attr, value);
	}

	return err;
}

/**
 * The internal helper function for setAttribute and setAttributeNS.
 *
 * \param element    The element
 * \param ns         The namespace to set attribute for.  May be NULL.
 * \param name       The name of the new attribute
 * \param value      The value of the new attribute
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_set_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, dom_string *value)
{
	dom_attr_list *match;
	dom_node_internal *e = (dom_node_internal *) element;
	dom_exception err;

	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Ensure element can be written */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	if (match != NULL) {
		/* Found an existing attribute, so replace its value */
		err = dom_attr_set_value(match->attr, value);
		if (err != DOM_NO_ERR)
			return err;
	} else {
		/* No existing attribute, so create one */
		struct dom_attr *attr;
		struct dom_attr_list *list_node;
		struct dom_document *doc;
		bool success = true;

		err = _dom_attr_create(e->owner, name, ns, NULL,
				true, &attr);
		if (err != DOM_NO_ERR)
			return err;

		/* Set its parent, so that value parsing works */
		dom_node_set_parent(attr, element);

		/* Set its value */
		err = dom_attr_set_value(attr, value);
		if (err != DOM_NO_ERR) {
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return err;
		}

		/* Create attribute list node */
		list_node = _dom_element_attr_list_node_create(attr, element,
				name, ns);
		if (list_node == NULL) {
			/* If we failed at this step, there must be no memory */
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return DOM_NO_MEM_ERR;
		}
		dom_string_ref(name);
		dom_string_ref(ns);

		/* Link into element's attribute list */
		if (element->attributes == NULL)
			element->attributes = list_node;
		else
			_dom_element_attr_list_insert(element->attributes,
					list_node);

		dom_node_unref(attr);
		dom_node_remove_pending(attr);
	}

	return DOM_NO_ERR;
}

/**
 * Remove an attribute from an element by name
 *
 * \param element  The element to remove attribute from
 * \param name     The name of the attribute to remove
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_remove_attr(struct dom_element *element,
		dom_string *ns, dom_string *name)
{
	dom_attr_list *match;
	dom_exception err;
	dom_node_internal *e = (dom_node_internal *) element;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	/* Detach attr node from list */
	if (match != NULL) {
		/* Disptach DOMNodeRemoval event */
		bool success = true;
		dom_attr *a = match->attr;
		struct dom_document *doc = dom_node_get_owner(element);
		dom_string *old = NULL;

		/* Delete the attribute node */
		if (element->attributes == match) {
			element->attributes =
					_dom_element_attr_list_next(match);
		}
		if (element->attributes == match) {
			/* match must be sole attribute */
			element->attributes = NULL;
		}
		_dom_element_attr_list_node_unlink(match);
		_dom_element_attr_list_node_destroy(match);
	}

	/** \todo defaulted attribute handling */

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute node from an element by name
 *
 * \param element  The element to retrieve attribute node from
 * \param name     The attribute's name
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attr_node(struct dom_element *element,
		dom_string *ns, dom_string *name,
		struct dom_attr **result)
{
	dom_attr_list *match;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	/* Fill in value */
	if (match == NULL) {
		*result = NULL;
	} else {
		*result = match->attr;
		dom_node_ref(*result);
	}

	return DOM_NO_ERR;
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to receive previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attr_node(struct dom_element *element,
		dom_string *ns, struct dom_attr *attr,
		struct dom_attr **result)
{
	dom_attr_list *match;
	dom_exception err;
	dom_string *name = NULL;
	dom_node_internal *e = (dom_node_internal *) element;
	dom_node_internal *attr_node = (dom_node_internal *) attr;
	dom_attr *old_attr;
	struct dom_document *doc;
	bool success = true;

	/** \todo validate name */

	/* Ensure element and attribute belong to the same document */
	if (e->owner != attr_node->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure attribute isn't attached to another element */
	if (attr_node->parent != NULL && attr_node->parent != e)
		return DOM_INUSE_ATTRIBUTE_ERR;

	err = dom_node_get_local_name(attr, &name);
	if (err != DOM_NO_ERR)
		return err;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	*result = NULL;
	if (match != NULL) {
		_dom_element_attr_list_node_unlink(match);
		_dom_element_attr_list_node_destroy(match);
	}

	match = _dom_element_attr_list_node_create(attr, element,
			name, ns);
	if (match == NULL) {
		dom_string_unref(name);
		/* If we failed at this step, there must be no memory */
		return DOM_NO_MEM_ERR;
	}

	dom_string_ref(name);
	dom_string_ref(ns);
	dom_node_set_parent(attr, element);
	dom_node_remove_pending(attr);

	/* Cleanup */
	dom_string_unref(name);

	/* Link into element's attribute list */
	if (element->attributes == NULL)
		element->attributes = match;
	else
		_dom_element_attr_list_insert(element->attributes, match);

	return DOM_NO_ERR;
}

/**
 * Remove an attribute node from an element
 *
 * \param element  The element to remove attribute node from
 * \param attr     The attribute node to remove
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if ::attr is not an attribute of
 *                                         ::element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_remove_attr_node(struct dom_element *element,
		dom_string *ns, struct dom_attr *attr,
		struct dom_attr **result)
{
	dom_attr_list *match;
	dom_exception err;
	dom_string *name;
	dom_node_internal *e = (dom_node_internal *) element;
	dom_attr *a;
	bool success = true;
	struct dom_document *doc;
	dom_string *old = NULL;
	
	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	
	err = dom_node_get_node_name(attr, &name);
	if (err != DOM_NO_ERR)
		return err;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	/** \todo defaulted attribute handling */

	if (match == NULL || match->attr != attr) {
		dom_string_unref(name);
		return DOM_NOT_FOUND_ERR;
	}

	a = match->attr;

	dom_node_ref(a);

	/* Delete the attribute node */
	if (element->attributes == match) {
		element->attributes = _dom_element_attr_list_next(match);
	}
	if (element->attributes == match) {
		/* match must be sole attribute */
		element->attributes = NULL;
	}
	_dom_element_attr_list_node_unlink(match);
	_dom_element_attr_list_node_destroy(match);

	/* Now, cleaup the dom_string */
	dom_string_unref(name);

	return DOM_NO_ERR;
}

/**
 * Test whether certain attribute exists on the element
 *
 * \param element    The element
 * \param ns         The namespace to look for attribute in.  May be NULL.
 * \param name       The attribute's name
 * \param result     The return value
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_has_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, bool *result)
{
	dom_attr_list *match;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);

	/* Fill in result */
	if (match == NULL) {
		*result = false;
	} else {
		*result = true;
	}

	return DOM_NO_ERR;
}

/**
 * (Un)set an attribute Node as a ID.
 *
 * \param element    The element contains the attribute
 * \param ns         The namespace of the attribute node
 * \param name       The name of the attribute
 * \param is_id      true for set the node as a ID attribute, false unset it
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_set_id_attr(struct dom_element *element,
		dom_string *ns, dom_string *name, bool is_id)
{
	
	dom_attr_list *match;

	match = _dom_element_attr_list_find_by_name(element->attributes,
			name, ns);
	if (match == NULL)
		return DOM_NOT_FOUND_ERR;
	
	if (is_id == true) {
		/* Clear the previous id attribute if there is one */
		dom_attr_list *old = _dom_element_attr_list_find_by_name(
				element->attributes, element->id_name,
				element->id_ns);

		if (old != NULL) {
			_dom_attr_set_isid(old->attr, false);
		}

		/* Set up the new id attr stuff */
		element->id_name = dom_string_ref(name);
		element->id_ns = dom_string_ref(ns);
	}

	_dom_attr_set_isid(match->attr, is_id);

	return DOM_NO_ERR;
}

/**
 * Get the ID string of the element
 *
 * \param ele  The element
 * \param id   The ID of this element
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_get_id(struct dom_element *ele, dom_string **id)
{
	dom_exception err;
	dom_string *ret = NULL;
	dom_document *doc;
	dom_string *name;

	*id = NULL;

	if (ele->id_ns != NULL && ele->id_name != NULL) {
		/* There is user specific ID attribute */
		err = _dom_element_get_attribute_ns(ele, ele->id_ns, 
				ele->id_name, &ret);
		if (err != DOM_NO_ERR) {
			return err;
		}

		*id = ret;
		return err;
	}

	doc = dom_node_get_owner(ele);
	assert(doc != NULL);

	if (ele->id_name != NULL) {
		name = ele->id_name;
	} else {
		name = _dom_document_get_id_name(doc);

		if (name == NULL) {
			/* No ID attribute at all, just return NULL */
			*id = NULL;
			return DOM_NO_ERR;
		}
	}

	err = _dom_element_get_attribute(ele, name, &ret);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (ret != NULL) {
		*id = ret;
	} else {
		*id = NULL;
	}

	return err;
}



/*-------------- The dom_namednodemap functions -------------------------*/

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_length(void *priv, uint32_t *length)
{
	dom_element *e = (dom_element *) priv;

	*length = _dom_element_attr_list_length(e->attributes);

	return DOM_NO_ERR;
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_named_item(void *priv,
		dom_string *name, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;

	return _dom_element_get_attribute_node(e, name, (dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_set_named_item(void *priv,
		struct dom_node *arg, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_node_internal *n = (dom_node_internal *) arg;

	if (n->type != DOM_ATTRIBUTE_NODE)
		return DOM_HIERARCHY_REQUEST_ERR;

	return _dom_element_set_attribute_node(e, (dom_attr *) arg,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_remove_named_item(
		void *priv, dom_string *name,
		struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_exception err;

	err = _dom_element_get_attribute_node(e, name, (dom_attr **) node);
	if (err != DOM_NO_ERR)
		return err;

	if (*node == NULL) {
		return DOM_NOT_FOUND_ERR;
	}
	
	return _dom_element_remove_attribute(e, name);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_item(void *priv,
		uint32_t index, struct dom_node **node)
{
	dom_attr_list * match = NULL;
	unsigned int num = index + 1;
	dom_element *e = (dom_element *) priv;

	match = _dom_element_attr_list_get_by_index(e->attributes, num);

	if (match != NULL) {
		*node = (dom_node *) match->attr;
		dom_node_ref(*node);
	} else {
		*node = NULL;
	}

	return DOM_NO_ERR;
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_named_item_ns(
		void *priv, dom_string *ns,
		dom_string *localname, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;

	return _dom_element_get_attribute_node_ns(e, ns, localname,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_set_named_item_ns(
		void *priv, struct dom_node *arg,
		struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_node_internal *n = (dom_node_internal *) arg;

	if (n->type != DOM_ATTRIBUTE_NODE)
		return DOM_HIERARCHY_REQUEST_ERR;

	return _dom_element_set_attribute_node_ns(e, (dom_attr *) arg,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_remove_named_item_ns(
		void *priv, dom_string *ns,
		dom_string *localname, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_exception err;
	
	err = _dom_element_get_attribute_node_ns(e, ns, localname,
			(dom_attr **) node);
	if (err != DOM_NO_ERR)
		return err;

	if (*node == NULL) {
		return DOM_NOT_FOUND_ERR;
	}

	return _dom_element_remove_attribute_ns(e, ns, localname);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
void attributes_destroy(void *priv)
{
	dom_element *e = (dom_element *) priv;

	dom_node_unref(e);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
bool attributes_equal(void *p1, void *p2)
{
	/* We have passed the pointer to this element as the private data,
	 * and here we just need to compare whether the two elements are 
	 * equal
	 */
	return p1 == p2;
}
/*------------------ End of namednodemap functions -----------------------*/

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * A DOM entity reference
 */
struct dom_entity_reference {
	dom_node_internal base;		/**< Base node */
};

static struct dom_node_vtable er_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable er_protect_vtable = {
	DOM_ER_PROTECT_VTABLE
};

/**
 * Create an entity reference
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_entity_reference_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_entity_reference **result)
{
	dom_entity_reference *e;
	dom_exception err;

	/* Allocate the comment node */
	e = (dom_entity_reference *)malloc(sizeof(dom_entity_reference));
	if (e == NULL)
		return DOM_NO_MEM_ERR;

	e->base.base.vtable = &er_vtable;
	e->base.vtable = &er_protect_vtable;

	/* And initialise the node */
	err = _dom_entity_reference_initialise(&e->base, doc, 
			DOM_ENTITY_REFERENCE_NODE, name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		free(e);
		return err;
	}

	*result = e;

	return DOM_NO_ERR;
}

/**
 * Destroy an entity reference
 *
 * \param entity  The entity reference to destroy
 *
 * The contents of ::entity will be destroyed and ::entity will be freed.
 */
void _dom_entity_reference_destroy(dom_entity_reference *entity)
{
	/* Finalise base class */
	_dom_entity_reference_finalise(&entity->base);

	/* Destroy fragment */
	free(entity);
}

/**
 * Get the textual representation of an EntityRererence
 *
 * \param entity  The entity reference to get the textual representation of
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unrer the string once it has
 * finished with it.
 */
dom_exception _dom_entity_reference_get_textual_representation(
		dom_entity_reference *entity, dom_string **result)
{
	UNUSED(entity);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-----------------------------------------------------------------------*/

/* Following comes the protected vtable  */

/* The virtual destroy function of this class */
void _dom_er_destroy(dom_node_internal *node)
{
	_dom_entity_reference_destroy((dom_entity_reference *) node);
}

/* The copy constructor of this class */
dom_exception _dom_er_copy(dom_node_internal *old, dom_node_internal **copy)
{
	dom_entity_reference *new_er;
	dom_exception err;

	new_er = (dom_entity_reference *)malloc(sizeof(dom_entity_reference));
	if (new_er == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_node_copy_internal(old, new_er);
	if (err != DOM_NO_ERR) {
		free(new_er);
		return err;
	}

	*copy = (dom_node_internal *) new_er;

	return DOM_NO_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

/**
 * Test whether a DOM implementation implements a specific feature
 * and version
 *
 * \param feature  The feature to test for
 * \param version  The version number of the feature to test for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_implementation_has_feature(
		const char *feature, const char *version,
		bool *result)
{
	UNUSED(feature);
	UNUSED(version);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create a document type node
 *
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param doctype    Pointer to location to receive result
 * \return DOM_NO_ERR on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed,
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the feature
 *                                   "XML" and the language exposed through
 *                                   Document does not support XML
 *                                   namespaces.
 *
 * The doctype will be referenced, so the client need not do this
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception dom_implementation_create_document_type(
		const char *qname, const char *public_id, 
		const char *system_id,
		struct dom_document_type **doctype)
{
	struct dom_document_type *d;
	dom_string *qname_s = NULL, *prefix = NULL, *lname = NULL;
	dom_string *public_id_s = NULL, *system_id_s = NULL;
	dom_exception err;

	if (qname == NULL) {
		return DOM_INVALID_CHARACTER_ERR;
	}

	err = dom_string_create((const uint8_t *) qname,
				strlen(qname), &qname_s);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_namespace_split_qname(qname_s, &prefix, &lname);
	if (err != DOM_NO_ERR) {
		dom_string_unref(qname_s);
		return err;
	}

	if (public_id != NULL) {
		err = dom_string_create((const uint8_t *) public_id,
				strlen(public_id), &public_id_s);
		if (err != DOM_NO_ERR) {
			dom_string_unref(lname);
			dom_string_unref(prefix);
			dom_string_unref(qname_s);
			return err;
		}
	}

	if (system_id != NULL) {
		err = dom_string_create((const uint8_t *) system_id,
				strlen(system_id), &system_id_s);
		if (err != DOM_NO_ERR) {
			dom_string_unref(public_id_s);
			dom_string_unref(lname);
			dom_string_unref(prefix);
			dom_string_unref(qname_s);
			return err;
		}
	}

	/* Create the doctype */
	err = _dom_document_type_create(qname_s, public_id_s, system_id_s, &d);

	if (err == DOM_NO_ERR)
		*doctype = d;

	dom_string_unref(system_id_s);
	dom_string_unref(public_id_s);
	dom_string_unref(prefix);
	dom_string_unref(lname);
	dom_string_unref(qname_s);

	return err;
}

/**
 * Create a document node
 *
 * \param impl_type  The type of document object to create
 * \param ns         The namespace URI of the document element
 * \param qname      The qualified name of the document element
 * \param doctype    The type of document to create
 * \param doc        Pointer to location to receive result
 * \return DOM_NO_ERR on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or if ::qname
 *                                   has a prefix and ::namespace is NULL,
 *                                   or if ::qname is NULL and ::namespace
 *                                   is non-NULL, or if ::qname has a prefix
 *                                   "xml" and ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or if ::impl does not support the "XML"
 *                                   feature and ::namespace is non-NULL,
 *         DOM_WRONG_DOCUMENT_ERR    if ::doctype is already being used by a
 *                                   document, or if it was not created by
 *                                   ::impl,
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the feature
 *                                   "XML" and the language exposed through
 *                                   Document does not support XML
 *                                   namespaces.
 *
 * The document will be referenced, so the client need not do this
 * explicitly. The client must unref the document once it has
 * finished with it.
 */
dom_exception dom_implementation_create_document(
		uint32_t impl_type,
		const char *ns, const char *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc)
{
	struct dom_document *d;
	dom_string *namespace_s = NULL, *qname_s = NULL;
	dom_exception err;

	if (ns != NULL) {
		err = dom_string_create((const uint8_t *) ns,
				strlen(ns), &namespace_s);
		if (err != DOM_NO_ERR)
			return err;
	}

	if (qname != NULL) {
		err = dom_string_create((const uint8_t *) qname, 
				strlen(qname), &qname_s);
		if (err != DOM_NO_ERR) {
			dom_string_unref(namespace_s);
			return err;
		}
	}

	if (qname_s != NULL && _dom_validate_name(qname_s) == false) {
		dom_string_unref(qname_s);
		dom_string_unref(namespace_s);
		return DOM_INVALID_CHARACTER_ERR;
	}
  
	err = _dom_namespace_validate_qname(qname_s, namespace_s);
	if (err != DOM_NO_ERR) {
		dom_string_unref(qname_s);
		dom_string_unref(namespace_s);
		return DOM_NAMESPACE_ERR;
	}

	if (doctype != NULL && dom_node_get_parent(doctype) != NULL) {
		dom_string_unref(qname_s);
		dom_string_unref(namespace_s);
		return DOM_WRONG_DOCUMENT_ERR;
	}

	/* Create document object that reflects the required APIs */
    err = _dom_document_create(&d);

	if (err != DOM_NO_ERR) {
		dom_string_unref(qname_s);
		dom_string_unref(namespace_s);
		return err;
	}

	/* Set its doctype, if necessary */
	if (doctype != NULL) {
		struct dom_node *ins_doctype = NULL;

		err = dom_node_append_child((struct dom_node *) d, 
				(struct dom_node *) doctype, &ins_doctype);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) d);
			dom_string_unref(qname_s);
			dom_string_unref(namespace_s);
			return err;
		}

		/* Not interested in inserted doctype */
		if (ins_doctype != NULL)
			dom_node_unref(ins_doctype);
	}

	/* Create root element and attach it to document */
	if (qname_s != NULL) {
		struct dom_element *e;
		struct dom_node *inserted;

		err = dom_document_create_element_ns(d, namespace_s, qname_s, &e);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) d);
			dom_string_unref(qname_s);
			dom_string_unref(namespace_s);
			return err;
		}

		err = dom_node_append_child((struct dom_node *) d,
				(struct dom_node *) e, &inserted);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) e);
			dom_node_unref((struct dom_node *) d);
			dom_string_unref(qname_s);
			dom_string_unref(namespace_s);
			return err;
		}

		/* No longer interested in inserted node */
		dom_node_unref(inserted);

		/* Done with element */
		dom_node_unref((struct dom_node *) e);
	}

	/* Clean up strings we created */
	dom_string_unref(qname_s);
	dom_string_unref(namespace_s);

	*doc = d;

	return DOM_NO_ERR;
}

/**
 * Retrieve a specialized object which implements the specified
 * feature and version
 *
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param object   Pointer to location to receive object
 * \return DOM_NO_ERR.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 */
dom_exception dom_implementation_get_feature(
		const char *feature, const char *version,
		void **object)
{
	UNUSED(feature);
	UNUSED(version);
	UNUSED(object);

	return DOM_NOT_SUPPORTED_ERR;
}
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * DOM named node map
 */
struct dom_namednodemap {
	dom_document *owner;	/**< Owning document */

	void *priv;			/**< Private data */

	struct nnm_operation *opt;	/**< The underlaid operation 
		 			 * implementations */

	uint32_t refcnt;		/**< Reference count */
};

/**
 * Create a namednodemap
 *
 * \param doc   The owning document
 * \param priv  The private data of this dom_namednodemap
 * \param opt   The operation function pointer
 * \param map   Pointer to location to receive created map
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * ::head must be a node owned by ::doc and must be either an Element or
 * DocumentType node.
 *
 * If ::head is of type Element, ::type must be DOM_ATTRIBUTE_NODE
 * If ::head is of type DocumentType, ::type may be either
 * DOM_ENTITY_NODE or DOM_NOTATION_NODE.
 *
 * The returned map will already be referenced, so the client need not
 * explicitly reference it. The client must unref the map once it is
 * finished with it.
 */
dom_exception _dom_namednodemap_create(dom_document *doc,
		void *priv, struct nnm_operation *opt,
		dom_namednodemap **map)
{
	dom_namednodemap *m;

	m = (dom_namednodemap *)malloc(sizeof(dom_namednodemap));
	if (m == NULL)
		return DOM_NO_MEM_ERR;

	m->owner = doc;

	m->priv = priv;
	m->opt = opt;

	m->refcnt = 1;

	*map = m;

	return DOM_NO_ERR;
}

/**
 * Claim a reference on a DOM named node map
 *
 * \param map  The map to claim a reference on
 */
void dom_namednodemap_ref(dom_namednodemap *map)
{
	assert(map != NULL);
	map->refcnt++;
}

/**
 * Release a reference on a DOM named node map
 *
 * \param map  The map to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * map will be released
 */
void dom_namednodemap_unref(dom_namednodemap *map)
{
	if (map == NULL)
		return;

	if (--map->refcnt == 0) {
		/* Call the implementation specific destroy */
		map->opt->namednodemap_destroy(map->priv);

		/* Destroy the map object */
		free(map);
	}
}

/**
 * Retrieve the length of a named node map
 *
 * \param map     Map to retrieve length of
 * \param length  Pointer to location to receive length
 * \return DOM_NO_ERR.
 */
dom_exception dom_namednodemap_get_length(dom_namednodemap *map,
		dom_ulong *length)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_get_length(map->priv, length);
}

/**
 * Retrieve an item by name from a named node map
 *
 * \param map   The map to retrieve the item from
 * \param name  The name of the item to retrieve
 * \param node  Pointer to location to receive item
 * \return DOM_NO_ERR.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_get_named_item(dom_namednodemap *map,
		dom_string *name, dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_get_named_item(map->priv, name, node);
}

/**
 * Add a node to a named node map, replacing any matching existing node
 *
 * \param map   The map to add to
 * \param arg   The node to add
 * \param node  Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::arg was created from a
 *                                         different document than ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::arg is an Attr that is
 *                                         already an attribute on another
 *                                         Element,
 *         DOM_HIERARCHY_REQUEST_ERR       if the type of ::arg is not
 *                                         permitted as a member of ::map.
 *
 * ::arg's nodeName attribute will be used to store it in ::map. It will
 * be accessible using the nodeName attribute as the key for lookup.
 *
 * Replacing a node by itself has no effect.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_set_named_item(dom_namednodemap *map,
		dom_node *arg, dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_set_named_item(map->priv, arg, node);
}

/**
 * Remove an item by name from a named node map
 *
 * \param map   The map to remove from
 * \param name  The name of the item to remove
 * \param node  Pointer to location to receive removed item
 * \return DOM_NO_ERR                      on success,
 *         DOM_NOT_FOUND_ERR               if there is no node named ::name
 *                                         in ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_remove_named_item(
		dom_namednodemap *map, dom_string *name,
		dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_remove_named_item(map->priv, name, node);
}

/**
 * Retrieve an item from a named node map
 *
 * \param map    The map to retrieve the item from
 * \param index  The map index to retrieve
 * \param node   Pointer to location to receive item
 * \return DOM_NO_ERR.
 *
 * ::index is a zero-based index into ::map.
 * ::index lies in the range [0, length-1]
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_item(dom_namednodemap *map,
		dom_ulong index, dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_item(map->priv, index, node);
}

/**
 * Retrieve an item by namespace/localname from a named node map
 *
 * \param map        The map to retrieve the item from
 * \param ns         The namespace URI of the item to retrieve
 * \param localname  The local name of the node to retrieve
 * \param node       Pointer to location to receive item
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_get_named_item_ns(
		dom_namednodemap *map, dom_string *ns,
		dom_string *localname, dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_get_named_item_ns(map->priv, ns,
			localname, node);
}

/**
 * Add a node to a named node map, replacing any matching existing node
 *
 * \param map   The map to add to
 * \param arg   The node to add
 * \param node  Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::arg was created from a
 *                                         different document than ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::arg is an Attr that is
 *                                         already an attribute on another
 *                                         Element,
 *         DOM_HIERARCHY_REQUEST_ERR       if the type of ::arg is not
 *                                         permitted as a member of ::map.
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * ::arg's namespaceURI and localName attributes will be used to store it in
 * ::map. It will be accessible using the namespaceURI and localName
 * attributes as the keys for lookup.
 *
 * Replacing a node by itself has no effect.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_set_named_item_ns(
		dom_namednodemap *map, dom_node *arg,
		dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_set_named_item_ns(map->priv, arg, node);
}

/**
 * Remove an item by namespace/localname from a named node map
 *
 * \param map        The map to remove from
 * \param ns         The namespace URI of the item to remove
 * \param localname  The local name of the item to remove
 * \param node       Pointer to location to receive removed item
 * \return DOM_NO_ERR                      on success,
 *         DOM_NOT_FOUND_ERR               if there is no node named ::name
 *                                         in ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly.
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception _dom_namednodemap_remove_named_item_ns(
		dom_namednodemap *map, dom_string *ns,
		dom_string *localname, dom_node **node)
{
	assert(map->opt != NULL);
	return map->opt->namednodemap_remove_named_item_ns(map->priv, ns,
			localname, node);
}

/**
 * Compare whether two NamedNodeMap are equal.
 *
 */
bool _dom_namednodemap_equal(dom_namednodemap *m1, 
		dom_namednodemap *m2)
{
	assert(m1->opt != NULL);
	return (m1->opt == m2->opt && m1->opt->namednodemap_equal(m1->priv,
			m2->priv));
}

/**
 * Update the dom_namednodemap to make it as a proxy of another object
 *
 * \param map	The dom_namednodemap
 * \param priv	The private data to change to
 */
void _dom_namednodemap_update(dom_namednodemap *map, void *priv)
{
	map->priv = priv;
}
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

static bool _dom_node_permitted_child(const dom_node_internal *parent, 
		const dom_node_internal *child);
static inline dom_exception _dom_node_attach(dom_node_internal *node, 
		dom_node_internal *parent,
		dom_node_internal *previous, 
		dom_node_internal *next);
static inline void _dom_node_detach(dom_node_internal *node);
static inline dom_exception _dom_node_attach_range(dom_node_internal *first, 
		dom_node_internal *last,
		dom_node_internal *parent, 
		dom_node_internal *previous, 
		dom_node_internal *next);
static inline dom_exception _dom_node_detach_range(dom_node_internal *first,
		dom_node_internal *last);
static inline void _dom_node_replace(dom_node_internal *old, 
		dom_node_internal *replacement);

static struct dom_node_vtable node_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable node_protect_vtable = {
	DOM_NODE_PROTECT_VTABLE
};



/*----------------------------------------------------------------------*/

/* The constructor and destructor of this object */

/* Create a DOM node and compose the vtable */
dom_node_internal * _dom_node_create(void)
{
	dom_node_internal *node = (dom_node_internal *)malloc(sizeof(struct dom_node_internal));
	if (node == NULL)
		return NULL;

	node->base.vtable = &node_vtable;
	node->vtable = &node_protect_vtable;
	return node;
}

/**
 * Destroy a DOM node
 *
 * \param node  The node to destroy
 *
 * ::node's parent link must be NULL and its reference count must be 0.
 *
 * ::node will be freed.
 *
 * This function should only be called from dom_node_unref or type-specific
 * destructors (for destroying child nodes). Anything else should not
 * be attempting to destroy nodes -- they should simply be unreferencing
 * them (so destruction will occur at the appropriate time).
 */
void _dom_node_destroy(struct dom_node_internal *node)
{
	struct dom_document *owner = node->owner;
	bool null_owner_permitted = (node->type == DOM_DOCUMENT_NODE || 
			node->type == DOM_DOCUMENT_TYPE_NODE);

	assert(null_owner_permitted || owner != NULL); 

	if (!null_owner_permitted) {
		/* Claim a reference upon the owning document during 
		 * destruction to ensure that the document doesn't get 
		 * destroyed before its contents. */
		dom_node_ref(owner);
	}

	/* Finalise this node, this should also destroy all the child nodes. */
	_dom_node_finalise(node);

	if (!null_owner_permitted) {
		/* Release the reference we claimed on the document. If this
		 * is the last reference held on the document and the list
		 * of nodes pending deletion is empty, then the document will
		 * be destroyed. */
		dom_node_unref(owner);
	}

	/* Release our memory */
	free(node);
}

/**
 * Initialise a DOM node
 *
 * \param node       The node to initialise
 * \param doc        The document which owns the node
 * \param type       The node type required
 * \param name       The node (local) name, or NULL
 * \param value      The node value, or NULL
 * \param ns         Namespace URI to use for node, or NULL
 * \param prefix     Namespace prefix to use for node, or NULL
 * \return DOM_NO_ERR on success.
 *
 * ::name, ::value, ::namespace, and ::prefix  will have their reference 
 * counts increased.
 */
dom_exception _dom_node_initialise(dom_node_internal *node,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value,
		dom_string *ns, dom_string *prefix)
{
	node->owner = doc;

	if (name != NULL)
		node->name = dom_string_ref(name);
	else
		node->name = NULL;

	if (value != NULL)
		node->value = dom_string_ref(value);
	else
		node->value = NULL;

	node->type = type;

	node->parent = NULL;
	node->first_child = NULL;
	node->last_child = NULL;
	node->previous = NULL;
	node->next = NULL;

	/* Note: nodes do not reference the document to which they belong,
	 * as this would result in the document never being destroyed once
	 * the client has finished with it. The document will be aware of
	 * any nodes that it owns through 2 mechanisms:
	 *
	 * either a) Membership of the document tree
	 * or     b) Membership of the list of nodes pending deletion
	 *
	 * It is not possible for any given node to be a member of both
	 * data structures at the same time.
	 *
	 * The document will not be destroyed until both of these
	 * structures are empty. It will forcibly attempt to empty
	 * the document tree on document destruction. Any still-referenced
	 * nodes at that time will be added to the list of nodes pending
	 * deletion. This list will not be forcibly emptied, as it contains
	 * those nodes (and their sub-trees) in use by client code.
	 */

	if (ns != NULL)
		node->ns = dom_string_ref(ns);
	else
		node->ns = NULL;

	if (prefix != NULL)
		node->prefix = dom_string_ref(prefix);
	else
		node->prefix = NULL;

	node->user_data = NULL;

	node->base.refcnt = 1;

	list_init(&node->pending_list);
	if (node->type != DOM_DOCUMENT_NODE) {
		/* A Node should be in the pending list when it is created */
		dom_node_mark_pending(node);
	}

	return DOM_NO_ERR;
}

/**
 * Finalise a DOM node
 *
 * \param node  The node to finalise
 *
 * The contents of ::node will be cleaned up. ::node will not be freed.
 * All children of ::node should have been removed prior to finalisation.
 */
void _dom_node_finalise(dom_node_internal *node)
{
	struct dom_user_data *u, *v;
	struct dom_node_internal *p;
	struct dom_node_internal *n = NULL;

	/* Destroy user data */
	for (u = node->user_data; u != NULL; u = v) {
		v = u->next;

		if (u->handler != NULL)
			u->handler(DOM_NODE_DELETED, u->key, u->data, 
					NULL, NULL);

		dom_string_unref(u->key);
		free(u);
	}
	node->user_data = NULL;

	if (node->prefix != NULL) {
		dom_string_unref(node->prefix);
		node->prefix = NULL;
	}

	if (node->ns != NULL) {
		dom_string_unref(node->ns);
		node->ns = NULL;
	}

	/* Destroy all the child nodes of this node */
	p = node->first_child;
	while (p != NULL) {
		n = p->next;
		p->parent = NULL;
		dom_node_try_destroy(p);
		p = n;
	}

	/* Paranoia */
	node->next = NULL;
	node->previous = NULL;
	node->last_child = NULL;
	node->first_child = NULL;
	node->parent = NULL;

	if (node->value != NULL) {
		dom_string_unref(node->value);
		node->value = NULL;
	}

	if (node->name != NULL) {
		dom_string_unref(node->name);
		node->name = NULL;
	}

	/* Detach from the pending list, if we are in it,
	 * this part of code should always be the end of this function. */
	if (node->pending_list.prev != &node->pending_list) {
		assert (node->pending_list.next != &node->pending_list); 
		list_del(&node->pending_list);
		if (node->owner != NULL && node->type != DOM_DOCUMENT_NODE) {
			/* Deleting this node from the pending list may cause
			 * the list to be null and we should try to destroy 
			 * the document. */
			_dom_document_try_destroy(node->owner);
		}
	}
}


/* ---------------------------------------------------------------------*/

/* The public virtual function of this interface Node */

/**
 * Retrieve the name of a DOM node
 *
 * \param node    The node to retrieve the name of
 * \param result  Pointer to location to receive node name
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_get_node_name(dom_node_internal *node,
		dom_string **result)
{
	dom_string *node_name, *temp;
	dom_exception err;

	/* Document Node and DocumentType Node can have no owner */
	assert(node->type == DOM_DOCUMENT_TYPE_NODE ||
			node->type == DOM_DOCUMENT_NODE ||
			node->owner != NULL);

	assert(node->name != NULL);

	/* If this node was created using a namespace-aware method and
	 * has a defined prefix, then nodeName is a QName comprised
	 * of prefix:name. */
	if (node->prefix != NULL) {
		dom_string *colon;

		err = dom_string_create((const uint8_t *) ":", SLEN(":"), 
				&colon);
		if (err != DOM_NO_ERR) {
			return err;
		}

		/* Prefix + : */
		err = dom_string_concat(node->prefix, colon, &temp);
		if (err != DOM_NO_ERR) {
			dom_string_unref(colon);
			return err;
		}

		/* Finished with colon */
		dom_string_unref(colon);

		/* Prefix + : + Localname */
		err = dom_string_concat(temp, node->name, &node_name);
		if (err != DOM_NO_ERR) {
			dom_string_unref(temp);
			return err;
		}

		/* Finished with temp */
		dom_string_unref(temp);
	} else {
		node_name = dom_string_ref(node->name);
	}

	*result = node_name;

	return DOM_NO_ERR;
}

/**
 * Retrieve the value of a DOM node
 *
 * \param node    The node to retrieve the value of
 * \param result  Pointer to location to receive node value
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_node_get_node_value(dom_node_internal *node,
		dom_string **result)
{
	if (node->value != NULL)
		dom_string_ref(node->value);

	*result = node->value;

	return DOM_NO_ERR;
}

/**
 * Set the value of a DOM node
 *
 * \param node   Node to set the value of
 * \param value  New value for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if the node is readonly and the
 *                                         value is not defined to be null.
 *
 * The new value will have its reference count increased, so the caller
 * should unref it after the call (as the caller should have already claimed
 * a reference on the string). The node's existing value will be unrefed.
 */
dom_exception _dom_node_set_node_value(dom_node_internal *node,
		dom_string *value)
{
	/* TODO
	 * Whether we should change this to a virtual function? 
	 */
	/* This is a NOP if the value is defined to be null. */
	if (node->type == DOM_DOCUMENT_NODE || 
			node->type == DOM_DOCUMENT_FRAGMENT_NODE || 
			node->type == DOM_DOCUMENT_TYPE_NODE || 
			node->type == DOM_ELEMENT_NODE || 
			node->type == DOM_ENTITY_NODE || 
			node->type == DOM_ENTITY_REFERENCE_NODE || 
			node->type == DOM_NOTATION_NODE) {
		return DOM_NO_ERR;
	}

	/* Ensure node is writable */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* If it's an attribute node, then delegate setting to 
	 * the type-specific function */
	if (node->type == DOM_ATTRIBUTE_NODE)
		return dom_attr_set_value((struct dom_attr *) node, value);

	if (node->value != NULL)
		dom_string_unref(node->value);

	if (value != NULL)
		dom_string_ref(value);

	node->value = value;

	return DOM_NO_ERR;
}

/**
 * Retrieve the type of a DOM node
 *
 * \param node    The node to retrieve the type of
 * \param result  Pointer to location to receive node type
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_get_node_type(dom_node_internal *node, 
		dom_node_type *result)
{
	*result = node->type;

	return DOM_NO_ERR;
}

/**
 * Retrieve the parent of a DOM node
 *
 * \param node    The node to retrieve the parent of
 * \param result  Pointer to location to receive node parent
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_parent_node(dom_node_internal *node,
		dom_node_internal **result)
{
	/* Attr nodes have no parent */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a parent node, then increase its reference count */
	if (node->parent != NULL)
		dom_node_ref(node->parent);

	*result = node->parent;

	return DOM_NO_ERR;
}

/**
 * Retrieve a list of children of a DOM node
 *
 * \param node    The node to retrieve the children of
 * \param result  Pointer to location to receive child list
 * \return DOM_NO_ERR.
 *
 * The returned NodeList will be referenced. It is the responsibility
 * of the caller to unref the list once it has finished with it.
 */
dom_exception _dom_node_get_child_nodes(dom_node_internal *node,
		struct dom_nodelist **result)
{
	/* Can't do anything without an owning document.
	 * This is only a problem for DocumentType nodes 
	 * which are not yet attached to a document. 
	 * DocumentType nodes have no children, anyway. */
	if (node->owner == NULL)
		return DOM_NOT_SUPPORTED_ERR;

	return _dom_document_get_nodelist(node->owner, DOM_NODELIST_CHILDREN,
			node, NULL, NULL, NULL, result);
}

/**
 * Retrieve the first child of a DOM node
 *
 * \param node    The node to retrieve the first child of
 * \param result  Pointer to location to receive node's first child
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_first_child(dom_node_internal *node,
		dom_node_internal **result)
{
	/* If there is a first child, increase its reference count */
	if (node->first_child != NULL)
		dom_node_ref(node->first_child);

	*result = node->first_child;

	return DOM_NO_ERR;
}

/**
 * Retrieve the last child of a DOM node
 *
 * \param node    The node to retrieve the last child of
 * \param result  Pointer to location to receive node's last child
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_last_child(dom_node_internal *node,
		dom_node_internal **result)
{
	/* If there is a last child, increase its reference count */
	if (node->last_child != NULL)
		dom_node_ref(node->last_child);

	*result = node->last_child;

	return DOM_NO_ERR;
}

/**
 * Retrieve the previous sibling of a DOM node
 *
 * \param node    The node to retrieve the previous sibling of
 * \param result  Pointer to location to receive node's previous sibling
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_previous_sibling(dom_node_internal *node,
		dom_node_internal **result)
{
	/* Attr nodes have no previous siblings */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a previous sibling, increase its reference count */
	if (node->previous != NULL)
		dom_node_ref(node->previous);

	*result = node->previous;

	return DOM_NO_ERR;
}

/**
 * Retrieve the subsequent sibling of a DOM node
 *
 * \param node    The node to retrieve the subsequent sibling of
 * \param result  Pointer to location to receive node's subsequent sibling
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_next_sibling(dom_node_internal *node,
		dom_node_internal **result)
{
	/* Attr nodes have no next siblings */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a subsequent sibling, increase its reference count */
	if (node->next != NULL)
		dom_node_ref(node->next);

	*result = node->next;

	return DOM_NO_ERR;
}

/**
 * Retrieve a map of attributes associated with a DOM node
 *
 * \param node    The node to retrieve the attributes of
 * \param result  Pointer to location to receive attribute map
 * \return DOM_NO_ERR.
 *
 * The returned NamedNodeMap will be referenced. It is the responsibility
 * of the caller to unref the map once it has finished with it.
 *
 * If ::node is not an Element, then NULL will be returned.
 */
dom_exception _dom_node_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result)
{
	UNUSED(node);
	*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Retrieve the owning document of a DOM node
 *
 * \param node    The node to retrieve the owner of
 * \param result  Pointer to location to receive node's owner
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_get_owner_document(dom_node_internal *node,
		struct dom_document **result)
{
	/* Document nodes have no owner, as far as clients are concerned 
	 * In reality, they own themselves as this simplifies code elsewhere */
	if (node->type == DOM_DOCUMENT_NODE) {
		*result = NULL;

		return DOM_NO_ERR;
	}

	/* If there is an owner, increase its reference count */
	if (node->owner != NULL)
		dom_node_ref(node->owner);

	*result = node->owner;

	return DOM_NO_ERR;
}

/**
 * Insert a child into a node
 *
 * \param node       Node to insert into
 * \param new_child  Node to insert
 * \param ref_child  Node to insert before, or NULL to insert as last child
 * \param result     Pointer to location to receive node being inserted
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly,
 *         DOM_NOT_FOUND_ERR               if ::ref_child is not a child of
 *                                         ::node.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * Attempting to insert a node before itself is a NOP.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_insert_before(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *ref_child,
		dom_node_internal **result)
{
	dom_exception err;
	dom_node_internal *n;
	
	assert(node != NULL);
	
	/* Ensure that new_child and node are owned by the same document */
	if ((new_child->type == DOM_DOCUMENT_TYPE_NODE && 
			new_child->owner != NULL && 
			new_child->owner != node->owner) ||
			(new_child->type != DOM_DOCUMENT_TYPE_NODE &&
			new_child->owner != node->owner))
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure node isn't read only */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure that ref_child (if any) is a child of node */
	if (ref_child != NULL && ref_child->parent != node)
		return DOM_NOT_FOUND_ERR;
	
	/* Ensure that new_child is not an ancestor of node, nor node itself */
	for (n = node; n != NULL; n = n->parent) {
		if (n == new_child)
			return DOM_HIERARCHY_REQUEST_ERR;
	}

	/* Ensure that new_child is permitted as a child of node */
	if (new_child->type != DOM_DOCUMENT_FRAGMENT_NODE && 
			!_dom_node_permitted_child(node, new_child))
		return DOM_HIERARCHY_REQUEST_ERR;

	/* Attempting to insert a node before itself is a NOP */
	if (new_child == ref_child) {
		dom_node_ref(new_child);
		*result = new_child;

		return DOM_NO_ERR;
	}

	/* If new_child is already in the tree and 
	 * its parent isn't read only, remove it */
	if (new_child->parent != NULL) {
		if (_dom_node_readonly(new_child->parent))
			return DOM_NO_MODIFICATION_ALLOWED_ERR;

		_dom_node_detach(new_child);
	}

	/* When a Node is attached, it should be removed from the pending 
	 * list */
	dom_node_remove_pending(new_child);

	/* If new_child is a DocumentFragment, insert its children.
	 * Otherwise, insert new_child */
	if (new_child->type == DOM_DOCUMENT_FRAGMENT_NODE) {
		/* Test the children of the docment fragment can be appended */
		dom_node_internal *c = new_child->first_child;
		for (; c != NULL; c = c->next)
			if (!_dom_node_permitted_child(node, c))
				return DOM_HIERARCHY_REQUEST_ERR;

		if (new_child->first_child != NULL) {
			err = _dom_node_attach_range(new_child->first_child,
					new_child->last_child, 
					node, 
					ref_child == NULL ? node->last_child 
							  : ref_child->previous,
					ref_child == NULL ? NULL 
							  : ref_child);
			if (err != DOM_NO_ERR)
				return err;

			new_child->first_child = NULL;
			new_child->last_child = NULL;
		}
	} else {
		err = _dom_node_attach(new_child, 
				node, 
				ref_child == NULL ? node->last_child 
						  : ref_child->previous, 
				ref_child == NULL ? NULL 
						  : ref_child);
		if (err != DOM_NO_ERR)
			return err;

	}

	/* DocumentType nodes are created outside the Document so, 
	 * if we're trying to attach a DocumentType node, then we
	 * also need to set its owner. */
	if (node->type == DOM_DOCUMENT_NODE &&
			new_child->type == DOM_DOCUMENT_TYPE_NODE) {
		/* See long comment in _dom_node_initialise as to why 
		 * we don't ref the document here */
		new_child->owner = (struct dom_document *) node;
	}

	/** \todo Is it correct to return DocumentFragments? */

	dom_node_ref(new_child);
	*result = new_child;

	return DOM_NO_ERR;
}

/**
 * Replace a node's child with a new one
 *
 * \param node       Node whose child to replace
 * \param new_child  Replacement node
 * \param old_child  Child to replace
 * \param result     Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly,
 *         DOM_NOT_FOUND_ERR               if ::old_child is not a child of
 *                                         ::node,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * If ::new_child is a DocumentFragment, ::old_child is replaced by all of
 * ::new_child's children.
 * If ::new_child is already in the tree, it is first removed.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_replace_child(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *old_child,
		dom_node_internal **result)
{
	dom_node_internal *n;

	/* We don't support replacement of DocumentType or root Elements */
	if (node->type == DOM_DOCUMENT_NODE && 
			(new_child->type == DOM_DOCUMENT_TYPE_NODE || 
			new_child->type == DOM_ELEMENT_NODE))
		return DOM_NOT_SUPPORTED_ERR;

	/* Ensure that new_child and node are owned by the same document */
	if (new_child->owner != node->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure node isn't read only */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure that old_child is a child of node */
	if (old_child->parent != node)
		return DOM_NOT_FOUND_ERR;

	/* Ensure that new_child is not an ancestor of node, nor node itself */
	for (n = node; n != NULL; n = n->parent) {
		if (n == new_child)
			return DOM_HIERARCHY_REQUEST_ERR;
	}

	/* Ensure that new_child is permitted as a child of node */
	if (new_child->type == DOM_DOCUMENT_FRAGMENT_NODE) {
		/* If this node is a doc fragment, we should test all its 
		 * children nodes */
		dom_node_internal *c;
		c = new_child->first_child;
		while (c != NULL) {
			if (!_dom_node_permitted_child(node, c))
				return DOM_HIERARCHY_REQUEST_ERR;

			c = c->next;
		}
	} else {
		if (!_dom_node_permitted_child(node, new_child))
			return DOM_HIERARCHY_REQUEST_ERR;
	}

	/* Attempting to replace a node with itself is a NOP */
	if (new_child == old_child) {
		dom_node_ref(old_child);
		*result = old_child;

		return DOM_NO_ERR;
	}

	/* If new_child is already in the tree and 
	 * its parent isn't read only, remove it */
	if (new_child->parent != NULL) {
		if (_dom_node_readonly(new_child->parent))
			return DOM_NO_MODIFICATION_ALLOWED_ERR;

		_dom_node_detach(new_child);
	}

	/* When a Node is attached, it should be removed from the pending 
	 * list */
	dom_node_remove_pending(new_child);

	/* Perform the replacement */
	_dom_node_replace(old_child, new_child);

	/* Sort out the return value */
	dom_node_ref(old_child);
	/* The replaced node should be marded pending */
	dom_node_mark_pending(old_child);
	*result = old_child;

	return DOM_NO_ERR;
}

/**
 * Remove a child from a node
 *
 * \param node       Node whose child to replace
 * \param old_child  Child to remove
 * \param result     Pointer to location to receive removed node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly
 *         DOM_NOT_FOUND_ERR               if ::old_child is not a child of
 *                                         ::node,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_remove_child(dom_node_internal *node,
		dom_node_internal *old_child,
		dom_node_internal **result)
{
	dom_exception err;
	bool success = true;

	/* We don't support removal of DocumentType or root Element nodes */
	if (node->type == DOM_DOCUMENT_NODE &&
			(old_child->type == DOM_DOCUMENT_TYPE_NODE ||
			old_child->type == DOM_ELEMENT_NODE))
		return DOM_NOT_SUPPORTED_ERR;

	/* Ensure old_child is a child of node */
	if (old_child->parent != node)
		return DOM_NOT_FOUND_ERR;

	/* Ensure node is writable */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Detach the node */
	_dom_node_detach(old_child);

	/* When a Node is removed, it should be destroy. When its refcnt is not 
	 * zero, it will be added to the document's deletion pending list. 
	 * When a Node is removed, its parent should be NULL, but its owner
	 * should remain to be the document. */
	dom_node_ref(old_child);
	dom_node_try_destroy(old_child);
	*result = old_child;

	return DOM_NO_ERR;
}

/**
 * Append a child to the end of a node's child list
 *
 * \param node       Node to insert into
 * \param new_child  Node to append
 * \param result     Pointer to location to receive node being inserted
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_node_append_child(dom_node_internal *node,
		dom_node_internal *new_child,
		dom_node_internal **result)
{
	/* This is just a veneer over insert_before */
	return dom_node_insert_before(node, new_child, NULL, result);
}

/**
 * Determine if a node has any children
 *
 * \param node    Node to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_has_child_nodes(dom_node_internal *node, bool *result)
{
	*result = node->first_child != NULL;

	return DOM_NO_ERR;
}

/**
 * Clone a DOM node
 *
 * \param node    The node to clone
 * \param deep    True to deep-clone the node's sub-tree
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR        on success,
 *         DOM_NO_MEMORY_ERR on memory exhaustion.
 *
 * The returned node will already be referenced.
 *
 * The duplicate node will have no parent and no user data.
 *
 * If ::node has registered user_data_handlers, then they will be called.
 *
 * Cloning an Element copies all attributes & their values (including those
 * generated by the XML processor to represent defaulted attributes). It
 * does not copy any child nodes unless it is a deep copy (this includes
 * text contained within the Element, as the text is contained in a child
 * Text node).
 *
 * Cloning an Attr directly, as opposed to cloning as part of an Element,
 * returns a specified attribute. Cloning an Attr always clones its children,
 * since they represent its value, no matter whether this is a deep clone or
 * not.
 *
 * Cloning an EntityReference automatically constructs its subtree if a
 * corresponding Entity is available, no matter whether this is a deep clone
 * or not.
 *
 * Cloning any other type of node simply returns a copy.
 *
 * Note that cloning an immutable subtree results in a mutable copy, but
 * the children of an EntityReference clone are readonly. In addition, clones
 * of unspecified Attr nodes are specified.
 *
 * \todo work out what happens when cloning Document, DocumentType, Entity
 * and Notation nodes.
 *
 * Note: we adopt a OO paradigm, this clone_node just provide a basic operation
 * of clone. Special clones like Attr/EntitiReference stated above should 
 * provide their overload of this interface in their implementation file. 
 */
dom_exception _dom_node_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result)
{
	dom_node_internal *n, *child, *r;
	dom_exception err;
	dom_user_data *ud;

	assert(node->owner != NULL);

	err = dom_node_copy(node, &n);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (deep) {
		child = node->first_child;
		while (child != NULL) {
			err = dom_node_clone_node(child, deep, (void *) &r);
			if (err != DOM_NO_ERR) {
				dom_node_unref(n);
				return err;
			}

			err = dom_node_append_child(n, r, (void *) &r);
			if (err != DOM_NO_ERR) {
				dom_node_unref(n);
				return err;
			}
			
			/* Clean up the new node, we have reference it two
			 * times */
			dom_node_unref(r);
			dom_node_unref(r);
			child = child->next;
		}
	}

	*result = n;

	/* Call the dom_user_data_handlers */
	ud = node->user_data;
	while (ud != NULL) {
		if (ud->handler != NULL)
			ud->handler(DOM_NODE_CLONED, ud->key, ud->data, 
					(dom_node *) node, (dom_node *) n);
		ud = ud->next;
	}

	return DOM_NO_ERR;
}

/**
 * Normalize a DOM node
 *
 * \param node  The node to normalize
 * \return DOM_NO_ERR.
 *
 * Puts all Text nodes in the full depth of the sub-tree beneath ::node,
 * including Attr nodes into "normal" form, where only structure separates
 * Text nodes.
 */
dom_exception _dom_node_normalize(dom_node_internal *node)
{
	dom_node_internal *n, *p;
	dom_exception err;

	p = node->first_child;
	if (p == NULL)
		return DOM_NO_ERR;

	n = p->next;

	while (n != NULL) {
		if (n->type == DOM_TEXT_NODE && p->type == DOM_TEXT_NODE) {
			err = _dom_merge_adjacent_text(p, n);
			if (err != DOM_NO_ERR)
				return err;

			_dom_node_detach(n);
			dom_node_unref(n);
			n = p->next;
			continue;
		}
		if (n->type != DOM_TEXT_NODE) {
			err = dom_node_normalize(n);
			if (err != DOM_NO_ERR)
				return err;
		}
		p = n;
		n = n->next;
	}

	return DOM_NO_ERR;
}

/**
 * Test whether the DOM implementation implements a specific feature and
 * that feature is supported by the node.
 *
 * \param node     The node to test
 * \param feature  The name of the feature to test
 * \param version  The version number of the feature to test
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_is_supported(dom_node_internal *node,
		dom_string *feature, dom_string *version,
		bool *result)
{
	bool has;

	UNUSED(node);

	dom_implementation_has_feature(dom_string_data(feature),
			dom_string_data(version), &has);

	*result = has;

	return DOM_NO_ERR;
}

/**
 * Retrieve the namespace of a DOM node
 *
 * \param node    The node to retrieve the namespace of
 * \param result  Pointer to location to receive node's namespace
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_get_namespace(dom_node_internal *node,
		dom_string **result)
{
	assert(node->owner != NULL);

	/* If there is a namespace, increase its reference count */
	if (node->ns != NULL)
		*result = dom_string_ref(node->ns);
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Retrieve the prefix of a DOM node
 *
 * \param node    The node to retrieve the prefix of
 * \param result  Pointer to location to receive node's prefix
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_get_prefix(dom_node_internal *node,
		dom_string **result)
{
	assert(node->owner != NULL);
	
	/* If there is a prefix, increase its reference count */
	if (node->prefix != NULL)
		*result = dom_string_ref(node->prefix);
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Set the prefix of a DOM node
 *
 * \param node    The node to set the prefix of
 * \param prefix  Pointer to prefix string
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if the specified prefix contains
 *                                         an illegal character,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly,
 *         DOM_NAMESPACE_ERR               if the specified prefix is
 *                                         malformed, if the namespaceURI of
 *                                         ::node is null, if the specified
 *                                         prefix is "xml" and the
 *                                         namespaceURI is different from
 *                                         "http://www.w3.org/XML/1998/namespace",
 *                                         if ::node is an attribute and the
 *                                         specified prefix is "xmlns" and
 *                                         the namespaceURI is different from
 *                                         "http://www.w3.org/2000/xmlns",
 *                                         or if this node is an attribute
 *                                         and the qualifiedName of ::node
 *                                         is "xmlns".
 */
dom_exception _dom_node_set_prefix(dom_node_internal *node,
		dom_string *prefix)
{
	/* Only Element and Attribute nodes created using 
	 * namespace-aware methods may have a prefix */
	if ((node->type != DOM_ELEMENT_NODE &&
			node->type != DOM_ATTRIBUTE_NODE) || 
			node->ns == NULL) {
		return DOM_NO_ERR;
	}

	/** \todo validate prefix */

	/* Ensure node is writable */
	if (_dom_node_readonly(node)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	/* No longer want existing prefix */
	if (node->prefix != NULL) {
		dom_string_unref(node->prefix);
	}

	/* Set the prefix */
	if (prefix != NULL) {
		/* Empty string is treated as NULL */
		if (dom_string_length(prefix) == 0) {
			node->prefix = NULL;
		} else {
			node->prefix = dom_string_ref(prefix);
		}
	} else {
		node->prefix = NULL;
	}

	return DOM_NO_ERR;
}

/**
 * Retrieve the local part of a node's qualified name
 *
 * \param node    The node to retrieve the local name of
 * \param result  Pointer to location to receive local name
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_get_local_name(dom_node_internal *node,
		dom_string **result)
{	
	assert(node->owner != NULL);
	
	/* Only Element and Attribute nodes may have a local name */
	if (node->type != DOM_ELEMENT_NODE && 
			node->type != DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* The node may have a local name, reference it if so */
	if (node->name != NULL)
		*result = dom_string_ref(node->name);
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Determine if a node has any attributes
 *
 * \param node    Node to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_has_attributes(dom_node_internal *node, bool *result)
{
	UNUSED(node);
	*result = false;

	return DOM_NO_ERR;
}

/**
 * Retrieve the base URI of a DOM node
 *
 * \param node    The node to retrieve the base URI of
 * \param result  Pointer to location to receive base URI
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_node_get_base(dom_node_internal *node,
		dom_string **result)
{
	struct dom_document *doc = node->owner;
	assert(doc != NULL);

	return _dom_document_get_uri(doc, result);
}

/**
 * Compare the positions of two nodes in a DOM tree
 *
 * \param node   The reference node
 * \param other  The node to compare
 * \param result Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR when the nodes are from different DOM
 *                               implementations.
 *
 * The result is a bitfield of dom_document_position values.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_node_compare_document_position(dom_node_internal *node,
		dom_node_internal *other, uint16_t *result)
{
	UNUSED(node);
	UNUSED(other);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the text content of a DOM node
 *
 * \param node    The node to retrieve the text content of
 * \param result  Pointer to location to receive text content
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * If there is no text content in the code, NULL will returned in \a result.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_node_get_text_content(dom_node_internal *node,
		dom_string **result)
{
	dom_node_internal *n;
	dom_string *str = NULL;
	dom_string *ret = NULL;

	assert(node->owner != NULL);
	
	for (n = node->first_child; n != NULL; n = n->next) {
		if (n->type == DOM_COMMENT_NODE ||
		    n->type == DOM_PROCESSING_INSTRUCTION_NODE)
			continue;
		dom_node_get_text_content(n, (str == NULL) ? &str : &ret);
		if (ret != NULL) {
			dom_string *new_str;
			dom_string_concat(str, ret, &new_str);
			dom_string_unref(str);
			dom_string_unref(ret);
			str = new_str;
		}
	}
	
	*result = str;

	return DOM_NO_ERR;
}

/**
 * Set the text content of a DOM node
 *
 * \param node     The node to set the text content of
 * \param content  New text content for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly.
 *
 * Any child nodes ::node may have are removed and replaced with a single
 * Text node containing the new content.
 */
dom_exception _dom_node_set_text_content(dom_node_internal *node,
		dom_string *content)
{
	dom_node_internal *n, *p, *r;
	dom_document *doc;
	dom_text *text;
	dom_exception err;

	n = node->first_child;

	while (n != NULL) {
		p = n;
		n = n->next;
		/* Add the (void *) casting to avoid gcc warning:
		 * dereferencing type-punned pointer will break 
		 * strict-aliasing rules */
		err = dom_node_remove_child(node, p, (void *) &r);
		if (err != DOM_NO_ERR)
			return err;
	}

	doc = node->owner;
	assert(doc != NULL);

	err = dom_document_create_text_node(doc, content, &text);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_node_append_child(node, text, (void *) &r);
	if (err != DOM_NO_ERR)
		return err;

	return DOM_NO_ERR;
}

/**
 * Determine if two DOM nodes are the same
 *
 * \param node    The node to compare
 * \param other   The node to compare against
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * This tests if the two nodes reference the same object.
 */
dom_exception _dom_node_is_same(dom_node_internal *node,
		dom_node_internal *other, bool *result)
{
	*result = (node == other);

	return DOM_NO_ERR;
}

/**
 * Lookup the prefix associated with the given namespace URI
 *
 * \param node       The node to start prefix search from
 * \param namespace  The namespace URI
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_lookup_prefix(dom_node_internal *node,
		dom_string *ns, dom_string **result)
{
	if (node->parent != NULL)
		return dom_node_lookup_prefix(node, ns, result);
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Determine if the specified namespace is the default namespace
 *
 * \param node       The node to query
 * \param ns         The namespace URI to test
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_is_default_namespace(dom_node_internal *node,
		dom_string *ns, bool *result)
{
	if (node->parent != NULL)
		return dom_node_is_default_namespace(node, ns, result);
	else
		*result = false;
	return DOM_NO_ERR;
}

/**
 * Lookup the namespace URI associated with the given prefix
 *
 * \param node    The node to start namespace search from
 * \param prefix  The prefix to look for, or NULL to find default.
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_node_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result)
{
	if (node->parent != NULL)
		return dom_node_lookup_namespace(node->parent, prefix, result);
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/**
 * Determine if two DOM nodes are equal
 *
 * \param node    The node to compare
 * \param other   The node to compare against
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * Two nodes are equal iff:
 *   + They are of the same type
 *   + nodeName, localName, namespaceURI, prefix, nodeValue are equal
 *   + The node attributes are equal
 *   + The child nodes are equal
 *
 * Two DocumentType nodes are equal iff:
 *   + publicId, systemId, internalSubset are equal
 *   + The node entities are equal
 *   + The node notations are equal
 * TODO: in document_type, we should override this virtual function
 * TODO: actually handle DocumentType nodes differently
 */
dom_exception _dom_node_is_equal(dom_node_internal *node,
		dom_node_internal *other, bool *result)
{
	dom_exception err = DOM_NO_ERR;
	dom_namednodemap *m1 = NULL, *m2 = NULL;
	dom_nodelist *l1 = NULL, *l2 = NULL;
	*result = false;

	/* Compare the node types */
	if (node->type != other->type){
		/* different */
		err = DOM_NO_ERR;
		goto cleanup;
	}

	assert(node->owner != NULL);
	assert(other->owner != NULL);

	/* Compare node name */
	if (dom_string_isequal(node->name, other->name) == false) {
		/* different */
		goto cleanup;
	}

	/* Compare prefix */
	if (dom_string_isequal(node->prefix, other->prefix) == false) {
		/* different */
		goto cleanup;
	}

	/* Compare namespace URI */
	if (dom_string_isequal(node->ns, other->ns) == false) {
		/* different */
		goto cleanup;
	}

	/* Compare node value */
	if (dom_string_isequal(node->value, other->value) == false) {
		/* different */
		goto cleanup;
	}

	/* Compare the attributes */
	err = dom_node_get_attributes(node, &m1);
	if (err != DOM_NO_ERR) {
		/* error */
		goto cleanup;
	}
	
	err = dom_node_get_attributes(other, &m2);
	if (err != DOM_NO_ERR) {
		/* error */
		goto cleanup;
	}

	if (dom_namednodemap_equal(m1, m2) == false) {
		/* different */
		goto cleanup;
	}

	/* Compare the children */
	err = dom_node_get_child_nodes(node, &l1);
	if (err != DOM_NO_ERR) {
		/* error */
		goto cleanup;
	}

	err = dom_node_get_child_nodes(other, &l2);
	if (err != DOM_NO_ERR) {
		/* error */
		goto cleanup;
	}

	if (dom_nodelist_equal(l1, l2) == false) {
		/* different */
		goto cleanup;
	}

	*result = true;

cleanup:
	if (m1 != NULL)
		dom_namednodemap_unref(m1);
	if (m2 != NULL)
		dom_namednodemap_unref(m2);

	if (l1 != NULL)
		dom_nodelist_unref(l1);
	if (l2 != NULL)
		dom_nodelist_unref(l2);

	return err;
}

/**
 * Retrieve an object which implements the specialized APIs of the specified
 * feature and version.
 *
 * \param node     The node to query
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_get_feature(dom_node_internal *node,
		dom_string *feature, dom_string *version,
		void **result)
{
	bool has;

	dom_implementation_has_feature(dom_string_data(feature), 
			dom_string_data(version), &has);

	if (has) {
		*result = node;
	} else {
		*result = NULL;
	}

	return DOM_NO_ERR;
}

/**
 * Associate an object to a key on this node
 *
 * \param node     The node to insert object into
 * \param key      The key associated with the object
 * \param data     The object to associate with key, or NULL to remove
 * \param handler  User handler function, or NULL if none
 * \param result   Pointer to location to receive previously associated object
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_set_user_data(dom_node_internal *node,
		dom_string *key, void *data,
		dom_user_data_handler handler, void **result)
{
	struct dom_user_data *ud = NULL;
	void *prevdata = NULL;

	/* Search for user data */
	for (ud = node->user_data; ud != NULL; ud = ud->next) {
		if (dom_string_isequal(ud->key, key))
			break;
	};

	/* Remove it, if found and no new data */
	if (data == NULL && ud != NULL) {
		dom_string_unref(ud->key);

		if (ud->next != NULL)
			ud->next->prev = ud->prev;
		if (ud->prev != NULL)
			ud->prev->next = ud->next;
		else
			node->user_data = ud->next;

		*result = ud->data;

		free(ud);

		return DOM_NO_ERR;
	}

	/* Otherwise, create a new user data object if one wasn't found */
	if (ud == NULL) {
		ud = (struct dom_user_data *)malloc(sizeof(struct dom_user_data));
		if (ud == NULL)
			return DOM_NO_MEM_ERR;

		dom_string_ref(key);
		ud->key = key;
		ud->data = NULL;
		ud->handler = NULL;

		/* Insert into list */
		ud->prev = NULL;
		ud->next = node->user_data;
		if (node->user_data)
			node->user_data->prev = ud;
		node->user_data = ud;
	}

	prevdata = ud->data;

	/* And associate data with it */
	ud->data = data;
	ud->handler = handler;

	*result = prevdata;

	return DOM_NO_ERR;
}

/**
 * Retrieves the object associated to a key on this node
 *
 * \param node    The node to retrieve object from
 * \param key     The key to search for
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_node_get_user_data(dom_node_internal *node,
		dom_string *key, void **result)
{
	struct dom_user_data *ud = NULL;

	/* Search for user data */
	for (ud = node->user_data; ud != NULL; ud = ud->next) {
		if (dom_string_isequal(ud->key, key))
			break;
	};

	if (ud != NULL)
		*result = ud->data;
	else
		*result = NULL;

	return DOM_NO_ERR;
}


/*--------------------------------------------------------------------------*/

/* The protected virtual functions */

/* Copy the internal attributes of a Node from old to new */
dom_exception _dom_node_copy(dom_node_internal *old, dom_node_internal **copy)
{
	dom_node_internal *new_node;
	dom_exception err;

	new_node = (dom_node_internal *)malloc(sizeof(dom_node_internal));
	if (new_node == NULL)
		return DOM_NO_MEM_ERR;

	err = _dom_node_copy_internal(old, new_node);
	if (err != DOM_NO_ERR) {
		free(new_node);
		return err;
	}

	*copy = new_node;

	return DOM_NO_ERR;
}

dom_exception _dom_node_copy_internal(dom_node_internal *old, 
		dom_node_internal *newn)
{
	newn->base.vtable = old->base.vtable;
	newn->vtable = old->vtable;

	newn->name = dom_string_ref(old->name);

	/* Value - see below */

	newn->type = old->type;
	newn->parent = NULL;
	newn->first_child = NULL;
	newn->last_child = NULL;
	newn->previous = NULL;
	newn->next = NULL;

	assert(old->owner != NULL);

	newn->owner = old->owner;

	if (old->ns != NULL)
		newn->ns = dom_string_ref(old->ns);
	else
		newn->ns = NULL;

	if (old->prefix != NULL)
		newn->prefix = dom_string_ref(old->prefix);
	else
		newn->prefix = NULL;

	newn->user_data = NULL;
	newn->base.refcnt = 1;

	list_init(&newn->pending_list);

	/* Value */	
	if (old->value != NULL) {
		dom_string_ref(old->value);

		newn->value = old->value;
	} else {
		newn->value = NULL;
	}
	
	/* The new copyed node has no parent, 
	 * so it should be put in the pending list. */
	dom_node_mark_pending(newn);

	/* Intialise the EventTarget interface */
	return DOM_NO_ERR;
}


/*--------------------------------------------------------------------------*/

/*  The helper functions */

/**
 * Determine if a node is permitted as a child of another node
 *
 * \param parent  Prospective parent
 * \param child   Prospective child
 * \return true if ::child is permitted as a child of ::parent, false otherwise.
 */
bool _dom_node_permitted_child(const dom_node_internal *parent, 
		const dom_node_internal *child)
{
	bool valid = false;

	/* See DOM3Core $1.1.1 for details */

	switch (parent->type) {
	case DOM_ELEMENT_NODE:
	case DOM_ENTITY_REFERENCE_NODE:
	case DOM_ENTITY_NODE:
	case DOM_DOCUMENT_FRAGMENT_NODE:
		valid = (child->type == DOM_ELEMENT_NODE || 
			 child->type == DOM_TEXT_NODE || 
			 child->type == DOM_COMMENT_NODE || 
			 child->type == DOM_PROCESSING_INSTRUCTION_NODE || 
			 child->type == DOM_CDATA_SECTION_NODE || 
			 child->type == DOM_ENTITY_REFERENCE_NODE);
		break;

	case DOM_ATTRIBUTE_NODE:
		valid = (child->type == DOM_TEXT_NODE ||
			 child->type == DOM_ENTITY_REFERENCE_NODE);
		break;

	case DOM_TEXT_NODE:
	case DOM_CDATA_SECTION_NODE:
	case DOM_PROCESSING_INSTRUCTION_NODE:
	case DOM_COMMENT_NODE:
	case DOM_DOCUMENT_TYPE_NODE:
	case DOM_NOTATION_NODE:
	case DOM_NODE_TYPE_COUNT:
		valid = false;
		break;

	case DOM_DOCUMENT_NODE:
		valid = (child->type == DOM_ELEMENT_NODE ||
			 child->type == DOM_PROCESSING_INSTRUCTION_NODE ||
			 child->type == DOM_COMMENT_NODE ||
			 child->type == DOM_DOCUMENT_TYPE_NODE);

		/* Ensure that the document doesn't already 
		 * have a root element */
		if (child->type == DOM_ELEMENT_NODE) {
			dom_node_internal *n;
			for (n = parent->first_child; 
					n != NULL; n = n->next) {
				if (n->type == DOM_ELEMENT_NODE)
					valid = false;
			}
		}

		/* Ensure that the document doesn't already 
		 * have a document type */
		if (child->type == DOM_DOCUMENT_TYPE_NODE) {
			dom_node_internal *n;
			for (n = parent->first_child;
					n != NULL; n = n->next) {
				if (n->type == DOM_DOCUMENT_TYPE_NODE)
					valid = false;
			}
		}

		break;
	}

	return valid;
}

/**
 * Determine if a node is read only
 *
 * \param node  The node to consider
 */
bool _dom_node_readonly(const dom_node_internal *node)
{
	const dom_node_internal *n = node;

	/* DocumentType and Notation ns are read only */
	if (n->type == DOM_DOCUMENT_TYPE_NODE ||
			n->type == DOM_NOTATION_NODE)
		return true;
	
	/* Some Attr node are readonly */
	if (n->type == DOM_ATTRIBUTE_NODE)
		return _dom_attr_readonly((const dom_attr *) n);

	/* Entity ns and their descendants are read only 
	 * EntityReference ns and their descendants are read only */
	for (n = node; n != NULL; n = n->parent) {
		if (n->type == DOM_ENTITY_NODE
				|| n->type == DOM_ENTITY_REFERENCE_NODE)
			return true;
	}

	/* Otherwise, it's writable */
	return false;
}

/**
 * Attach a node to the tree
 *
 * \param node      The node to attach
 * \param parent    Node to attach ::node as child of
 * \param previous  Previous node in sibling list, or NULL if none
 * \param next      Next node in sibling list, or NULL if none
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_node_attach(dom_node_internal *node,
		dom_node_internal *parent, dom_node_internal *previous,
		dom_node_internal *next)
{
	return _dom_node_attach_range(node, node, parent, previous, next);
}

/**
 * Detach a node from the tree
 *
 * \param node  The node to detach
 */
void _dom_node_detach(dom_node_internal *node)
{
	/* When a Node is not in the document tree, it must be in the 
	 * pending list */
	dom_node_mark_pending(node);

	_dom_node_detach_range(node, node);
}

/**
 * Attach a range of nodes to the tree
 *
 * \param first     First node in the range
 * \param last      Last node in the range
 * \param parent    Node to attach range to
 * \param previous  Previous node in sibling list, or NULL if none
 * \param next      Next node in sibling list, or NULL if none
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * The range is assumed to be a linked list of sibling nodes.
 */
dom_exception _dom_node_attach_range(dom_node_internal *first, 
		dom_node_internal *last,
		dom_node_internal *parent, 
		dom_node_internal *previous, 
		dom_node_internal *next)
{
	dom_exception err;
	bool success = true;
	dom_node_internal *n;

	first->previous = previous;
	last->next = next;

	if (previous != NULL)
		previous->next = first;
	else
		parent->first_child = first;

	if (next != NULL)
		next->previous = last;
	else
		parent->last_child = last;

	for (n = first; n != last->next; n = n->next) {
		n->parent = parent;
	}

	return DOM_NO_ERR;
}

/**
 * Detach a range of nodes from the tree
 *
 * \param first  The first node in the range
 * \param last   The last node in the range
 *
 * The range is assumed to be a linked list of sibling nodes.
 */
dom_exception _dom_node_detach_range(dom_node_internal *first,
		dom_node_internal *last)
{
	bool success = true;
	dom_node_internal *parent;
	dom_node_internal *n;

	if (first->previous != NULL)
		first->previous->next = last->next;
	else
		first->parent->first_child = last->next;

	if (last->next != NULL)
		last->next->previous = first->previous;
	else
		last->parent->last_child = first->previous;

	parent = first->parent;
	for (n = first; n != last->next; n = n->next) {
		n->parent = NULL;
	}

	first->previous = NULL;
	last->next = NULL;

	return DOM_NO_ERR;
}

/**
 * Replace a node in the tree
 *
 * \param old          Node to replace
 * \param replacement  Replacement node
 *
 * This is not implemented in terms of attach/detach in case 
 * we want to perform any special replacement-related behaviour 
 * at a later date.
 */
void _dom_node_replace(dom_node_internal *old,
		dom_node_internal *replacement)
{
	dom_node_internal *first, *last;
	dom_node_internal *n;

	if (replacement->type == DOM_DOCUMENT_FRAGMENT_NODE) {
		first = replacement->first_child;
		last = replacement->last_child;

		replacement->first_child = replacement->last_child = NULL;
	} else {
		first = replacement;
		last = replacement;
	}

	first->previous = old->previous;
	last->next = old->next;

	if (old->previous != NULL)
		old->previous->next = first;
	else
		old->parent->first_child = first;

	if (old->next != NULL)
		old->next->previous = last;
	else
		old->parent->last_child = last;

	for (n = first; n != last->next; n = n->next) {
		n->parent = old->parent;
	}

	old->previous = old->next = old->parent = NULL;
}

/**
 * Merge two adjacent text nodes into one text node.
 *
 * \param p  The first text node
 * \param n  The second text node
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_merge_adjacent_text(dom_node_internal *p,
		dom_node_internal *n)
{
	dom_string *str;
	dom_exception err;

	assert(p->type == DOM_TEXT_NODE);
	assert(n->type == DOM_TEXT_NODE);

	err = dom_text_get_whole_text(n, &str);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_characterdata_append_data(p, str);
	if (err != DOM_NO_ERR)
		return err;

	dom_string_unref(str);

	return DOM_NO_ERR;
}

/**
 * Try to destroy this node. 
 * 
 * \param node	The node to destroy
 *
 * When some node owns this node, (such as an elment owns its attribute nodes)
 * when this node being not owned, the owner should call this function to try
 * to destroy this node. 
 *
 * @note: Owning a node does not means this node's refcnt is above zero.
 */
dom_exception _dom_node_try_destroy(dom_node_internal *node)
{
	if (node == NULL)
		return DOM_NO_ERR;

	if (node->parent == NULL) {
		if (node->base.refcnt == 0) {
			dom_node_destroy(node);
		} else if (node->pending_list.prev == &node->pending_list){
			assert (node->pending_list.next == &node->pending_list);
			list_append(&node->owner->pending_nodes,
					&node->pending_list);
		}
	}
        
        return DOM_NO_ERR;
}

/**
 * To add some node to the pending list, when a node is removed from its parent
 * or an attribute is removed from its element
 *
 * \param node  The Node instance
 */
void _dom_node_mark_pending(dom_node_internal *node)
{
	struct dom_document *doc = node->owner;

	/* TODO: the pending_list is located at in dom_document, but some
	 * nodes can be created without a document created, such as a 
	 * dom_document_type node. For this reason, we should test whether
	 * the doc is NULL. */ 
	if (doc != NULL) {
		/* The node must not be in the pending list */
		assert(node->pending_list.prev == &node->pending_list);

		list_append(&doc->pending_nodes, &node->pending_list);
	}
}

/**
 * To remove the node from the pending list, this may happen when
 * a node is removed and then appended to another parent
 *
 * \param node  The Node instance
 */
void _dom_node_remove_pending(dom_node_internal *node)
{
	struct dom_document *doc = node->owner;

	if (doc != NULL) {
		/* The node must be in the pending list */
		assert(node->pending_list.prev != &node->pending_list);

		list_del(&node->pending_list);
	}
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * DOM node list
 */
struct dom_nodelist {
	dom_document *owner;	/**< Owning document */

	dom_node_internal *root;	
			/**< Root of applicable subtree */

	nodelist_type type;	/**< Type of this list */

	union {
		struct {
			dom_string *name;
					/**< Tag name to match */
			bool any_name;		/**< The name is '*' */
		} n;
		struct {
			bool any_namespace;	/**< The namespace is '*' */
			bool any_localname;	/**< The localname is '*' */
			dom_string *ns;	/**< Namespace */
			dom_string *localname;	/**< Localname */
		} ns;			/**< Data for namespace matching */
	} data;

	uint32_t refcnt;		/**< Reference count */
};

/**
 * Create a nodelist
 *
 * \param doc        Owning document
 * \param type	     The type of the NodeList
 * \param root       Root node of subtree that list applies to
 * \param tagname    Name of nodes in list (or NULL)
 * \param ns         Namespace part of nodes in list (or NULL)
 * \param localname  Local part of nodes in list (or NULL)
 * \param list       Pointer to location to receive list
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * ::root must be a node owned by ::doc
 *
 * The returned list will already be referenced, so the client need not
 * do so explicitly. The client must unref the list once finished with it.
 */
dom_exception _dom_nodelist_create(dom_document *doc, nodelist_type type,
		dom_node_internal *root, dom_string *tagname,
		dom_string *ns, dom_string *localname,
		dom_nodelist **list)
{
	dom_nodelist *l;

	l = (dom_nodelist *)malloc(sizeof(dom_nodelist));
	if (l == NULL)
		return DOM_NO_MEM_ERR;

	dom_node_ref(doc);
	l->owner = doc;

	dom_node_ref(root);
	l->root = root;

	l->type = type;

	if (type == DOM_NODELIST_BY_NAME || 
	    type == DOM_NODELIST_BY_NAME_CASELESS) {
		assert(tagname != NULL);
		l->data.n.any_name = false;
		if (dom_string_byte_length(tagname) == 1) {
			const char *ch = dom_string_data(tagname);
			if (*ch == '*') {
				l->data.n.any_name = true;
			}
		}
	
		l->data.n.name = dom_string_ref(tagname);
	} else if (type == DOM_NODELIST_BY_NAMESPACE ||
		   type == DOM_NODELIST_BY_NAMESPACE_CASELESS) {
		l->data.ns.any_localname = false;
		l->data.ns.any_namespace = false;
		if (localname != NULL) {
			if (dom_string_byte_length(localname) == 1) {
				const char *ch = dom_string_data(localname);
				if (*ch == '*') {
				   l->data.ns.any_localname = true;
				}
			}
			dom_string_ref(localname);
		}
		if (ns != NULL) {
			if (dom_string_byte_length(ns) == 1) {
				const char *ch = dom_string_data(ns);
				if (*ch == '*') {
					l->data.ns.any_namespace = true;
				}
			}
			dom_string_ref(ns);
		}

		l->data.ns.ns = ns;
		l->data.ns.localname = localname;
	} 

	l->refcnt = 1;

	*list = l;

	return DOM_NO_ERR;
}

/**
 * Claim a reference on a DOM node list
 *
 * \param list  The list to claim a reference on
 */
void dom_nodelist_ref(dom_nodelist *list)
{
	assert(list != NULL);
	list->refcnt++;
}

/**
 * Release a reference on a DOM node list
 *
 * \param list  The list to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * list will be released
 */
void dom_nodelist_unref(dom_nodelist *list)
{
	if (list == NULL)
		return;

	if (--list->refcnt == 0) {
		dom_node_internal *owner = (dom_node_internal *) list->owner;
		switch (list->type) {
		case DOM_NODELIST_CHILDREN:
			/* Nothing to do */
			break;
		case DOM_NODELIST_BY_NAMESPACE:
		case DOM_NODELIST_BY_NAMESPACE_CASELESS:
			if (list->data.ns.ns != NULL)
				dom_string_unref(list->data.ns.ns);
			if (list->data.ns.localname != NULL)
				dom_string_unref(list->data.ns.localname);
			break;
		case DOM_NODELIST_BY_NAME:
		case DOM_NODELIST_BY_NAME_CASELESS:
			assert(list->data.n.name != NULL);
			dom_string_unref(list->data.n.name);
			break;
		}

		dom_node_unref(list->root);

		/* Remove list from document */
		_dom_document_remove_nodelist(list->owner, list);

		/* Destroy the list object */
		free(list);

		/* And release our reference on the owning document
		 * This must be last as, otherwise, it's possible that
		 * the document is destroyed before we are */
		dom_node_unref(owner);
	}
}

/**
 * Retrieve the length of a node list
 *
 * \param list    List to retrieve length of
 * \param length  Pointer to location to receive length
 * \return DOM_NO_ERR.
 */
dom_exception dom_nodelist_get_length(dom_nodelist *list, uint32_t *length)
{
	dom_node_internal *cur = list->root->first_child;
	uint32_t len = 0;

	/* Traverse data structure */
	while (cur != NULL) {
		/* Process current node */
		if (list->type == DOM_NODELIST_CHILDREN) {
			len++;
		} else if (list->type == DOM_NODELIST_BY_NAME) {
			if (list->data.n.any_name == true || (
					cur->name != NULL && 
					dom_string_isequal(cur->name, 
						list->data.n.name))) {
				if (cur->type == DOM_ELEMENT_NODE)
					len++;
			}
		} else if (list->type == DOM_NODELIST_BY_NAME_CASELESS) {
			if (list->data.n.any_name == true || (
					cur->name != NULL && 
					dom_string_caseless_isequal(cur->name, 
						list->data.n.name))) {
				if (cur->type == DOM_ELEMENT_NODE)
					len++;
			}
		} else if (list->type == DOM_NODELIST_BY_NAMESPACE) {
			if (list->data.ns.any_namespace == true ||
					dom_string_isequal(cur->ns,
					list->data.ns.ns)) {
				if (list->data.ns.any_localname == true ||
						(cur->name != NULL &&
						dom_string_isequal(cur->name,
						list->data.ns.localname))) {
					if (cur->type == DOM_ELEMENT_NODE)
						len++;
				}
			}
		} else if (list->type == DOM_NODELIST_BY_NAMESPACE_CASELESS) {
			if (list->data.ns.any_namespace == true ||
					dom_string_caseless_isequal(
					cur->ns,
					list->data.ns.ns)) {
				if (list->data.ns.any_localname == true ||
						(cur->name != NULL &&
						dom_string_caseless_isequal(
						cur->name,
						list->data.ns.localname))) {
					if (cur->type == DOM_ELEMENT_NODE)
						len++;
				}
			}
		} else {
			assert("Unknown list type" == NULL);
		}

		/* Now, find next node */
		if (list->type == DOM_NODELIST_CHILDREN) {
			/* Just interested in sibling list */
			cur = cur->next;
		} else {
			/* Want a full in-order tree traversal */
			if (cur->first_child != NULL) {
				/* Has children */
				cur = cur->first_child;
			} else if (cur->next != NULL) {
				/* No children, but has siblings */
				cur = cur->next;
			} else {
				/* No children or siblings. 
				 * Find first unvisited relation. */
				dom_node_internal *parent = cur->parent;

				while (parent != list->root &&
						cur == parent->last_child) {
					cur = parent;
					parent = parent->parent;
				}

				cur = cur->next;
			}
		}
	}

	*length = len;

	return DOM_NO_ERR;
}

/**
 * Retrieve an item from a node list
 *
 * \param list   The list to retrieve the item from
 * \param index  The list index to retrieve
 * \param node   Pointer to location to receive item
 * \return DOM_NO_ERR.
 *
 * ::index is a zero-based index into ::list.
 * ::index lies in the range [0, length-1]
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 *
 * NOTE: If \ref node contains a node pointer already, it will *NOT* be
 * unreffed.  Managing the lifetime of that is up to the caller.
 */
dom_exception _dom_nodelist_item(dom_nodelist *list,
		uint32_t index, dom_node **node)
{
	dom_node_internal *cur = list->root->first_child;
	uint32_t count = 0;

	/* Traverse data structure */
	while (cur != NULL) {
		/* Process current node */
		if (list->type == DOM_NODELIST_CHILDREN) {
			count++;
		} else if (list->type == DOM_NODELIST_BY_NAME) {
			if (list->data.n.any_name == true || (
					cur->name != NULL && 
					dom_string_isequal(cur->name, 
						list->data.n.name))) {
				if (cur->type == DOM_ELEMENT_NODE)
					count++;
			}
		} else if (list->type == DOM_NODELIST_BY_NAME_CASELESS) {
			if (list->data.n.any_name == true || (
					cur->name != NULL && 
					dom_string_caseless_isequal(cur->name, 
						list->data.n.name))) {
				if (cur->type == DOM_ELEMENT_NODE)
					count++;
			}
		} else if (list->type == DOM_NODELIST_BY_NAMESPACE) {
			if (list->data.ns.any_namespace == true || 
					(cur->ns != NULL &&
					dom_string_isequal(cur->ns,
						list->data.ns.ns))) {
				if (list->data.ns.any_localname == true ||
						(cur->name != NULL &&
						dom_string_isequal(cur->name,
						list->data.ns.localname))) {
					if (cur->type == DOM_ELEMENT_NODE)
						count++;
				}
			}
		} else if (list->type == DOM_NODELIST_BY_NAMESPACE_CASELESS) {
			if (list->data.ns.any_namespace == true || 
					(cur->ns != NULL &&
					dom_string_caseless_isequal(
						cur->ns,
						list->data.ns.ns))) {
				if (list->data.ns.any_localname == true ||
						(cur->name != NULL &&
						dom_string_caseless_isequal(
						cur->name,
						list->data.ns.localname))) {
					if (cur->type == DOM_ELEMENT_NODE)
						count++;
				}
			}
		} else {
			assert("Unknown list type" == NULL);
		}

		/* Stop if this is the requested index */
		if ((index + 1) == count) {
			break;
		}

		/* Now, find next node */
		if (list->type == DOM_NODELIST_CHILDREN) {
			/* Just interested in sibling list */
			cur = cur->next;
		} else {
			/* Want a full in-order tree traversal */
			if (cur->first_child != NULL) {
				/* Has children */
				cur = cur->first_child;
			} else if (cur->next != NULL) {
				/* No children, but has siblings */
				cur = cur->next;
			} else {
				/* No children or siblings.
				 * Find first unvisited relation. */
				dom_node_internal *parent = cur->parent;

				while (parent != list->root &&
						cur == parent->last_child) {
					cur = parent;
					parent = parent->parent;
				}

				cur = cur->next;
			}
		}
	}

	if (cur != NULL) {
		dom_node_ref(cur);
	}
	*node = (dom_node *) cur;

	return DOM_NO_ERR;
}

/**
 * Match a nodelist instance against a set of nodelist creation parameters
 *
 * \param list       List to match
 * \param type	     The type of the NodeList
 * \param root       Root node of subtree that list applies to
 * \param tagname    Name of nodes in list (or NULL)
 * \param ns         Namespace part of nodes in list (or NULL)
 * \param localname  Local part of nodes in list (or NULL)
 * \return true if list matches, false otherwise
 */
bool _dom_nodelist_match(dom_nodelist *list, nodelist_type type,
		dom_node_internal *root, dom_string *tagname, 
		dom_string *ns, dom_string *localname)
{
	if (list->root != root)
		return false;

	if (list->type != type)
		return false;
	
	switch (list->type) {
	case DOM_NODELIST_CHILDREN:
		return true;
	case DOM_NODELIST_BY_NAME:
		return dom_string_isequal(list->data.n.name, tagname);
	case DOM_NODELIST_BY_NAMESPACE:
		return dom_string_isequal(list->data.ns.ns, ns) &&
			dom_string_isequal(list->data.ns.localname, localname);
	case DOM_NODELIST_BY_NAME_CASELESS:
		return dom_string_caseless_isequal(list->data.n.name, tagname);
	case DOM_NODELIST_BY_NAMESPACE_CASELESS:
		return dom_string_caseless_isequal(list->data.ns.ns,
						   ns) &&
			dom_string_caseless_isequal(list->data.ns.localname,
						    localname);
	}
	
	return false;
}

/**
 * Test whether the two NodeList are equal
 *
 * \param l1  One list
 * \param l2  The other list
 * \reutrn true for equal, false otherwise.
 */
bool _dom_nodelist_equal(dom_nodelist *l1, dom_nodelist *l2)
{
	return _dom_nodelist_match(l1, l1->type, l2->root, l2->data.n.name, 
			l2->data.ns.ns, l2->data.ns.localname);
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

/**
 * A DOM processing instruction
 */
struct dom_processing_instruction {
	dom_node_internal base;		/**< Base node */
};

static struct dom_node_vtable pi_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable pi_protect_vtable = {
	DOM_PI_PROTECT_VTABLE
};
/**
 * Create a processing instruction
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_processing_instruction_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_processing_instruction **result)
{
	dom_processing_instruction *p;
	dom_exception err;

	/* Allocate the comment node */
	p = (dom_processing_instruction *)malloc(sizeof(dom_processing_instruction));
	if (p == NULL)
		return DOM_NO_MEM_ERR;
	
	p->base.base.vtable = &pi_vtable;
	p->base.vtable = &pi_protect_vtable;

	/* And initialise the node */
	err = _dom_processing_instruction_initialise(&p->base, doc,
			DOM_PROCESSING_INSTRUCTION_NODE,
			name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		free(p);
		return err;
	}

	*result = p;

	return DOM_NO_ERR;
}

/**
 * Destroy a processing instruction
 *
 * \param pi   The processing instruction to destroy
 *
 * The contents of ::pi will be destroyed and ::pi will be freed.
 */
void _dom_processing_instruction_destroy(dom_processing_instruction *pi)
{
	/* Finalise base class */
	_dom_processing_instruction_finalise(&pi->base);

	/* Free processing instruction */
	free(pi);
}

/*-----------------------------------------------------------------------*/

/* Following comes the protected vtable  */

/* The virtual destroy function of this class */
void _dom_pi_destroy(dom_node_internal *node)
{
	_dom_processing_instruction_destroy(
			(dom_processing_instruction *) node);
}

/* The copy constructor of this class */
dom_exception _dom_pi_copy(dom_node_internal *old, dom_node_internal **copy)
{
	dom_processing_instruction *new_pi;
	dom_exception err;

	new_pi = (dom_processing_instruction *)malloc(sizeof(dom_processing_instruction));
	if (new_pi == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_node_copy_internal(old, new_pi);
	if (err != DOM_NO_ERR) {
		free(new_pi);
		return err;
	}

	*copy = (dom_node_internal *) copy;

	return DOM_NO_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * Type of a DOM string
 */
enum dom_string_type {
	DOM_STRING_CDATA = 0,
	DOM_STRING_INTERNED = 1
};

/**
 * A DOM string
 *
 * Strings are reference counted so destruction is performed correctly.
 */
typedef struct dom_string_internal {
	dom_string base;

	union {
		struct {
			uint8_t *ptr;	/**< Pointer to string data */
			size_t len;	/**< Byte length of string */
		} cdata;
		lwc_string *intern;	/**< Interned string */
	} data;

	enum dom_string_type type;	/**< String type */
} dom_string_internal;

/**
 * Empty string, for comparisons against NULL
 */
static const dom_string_internal empty_string = {
	{ 0 },
	{ { (uint8_t *) "", 0 } },
	DOM_STRING_CDATA
};

void dom_string_destroy(dom_string *str)
{
	dom_string_internal *istr = (dom_string_internal *)str;
	if (str != NULL) {
		assert(istr->base.refcnt == 0);
		switch (istr->type) {
		case DOM_STRING_INTERNED:
			if (istr->data.intern != NULL) {
				lwc_string_unref(istr->data.intern);
			}
			break;
		case DOM_STRING_CDATA:
			free(istr->data.cdata.ptr);
			break;
		}

		free(str);
	}
}

/**
 * Create a DOM string from a string of characters
 *
 * \param ptr    Pointer to string of characters
 * \param len    Length, in bytes, of string of characters
 * \param str    Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will be copied for use by the 
 * returned DOM string.
 */
dom_exception dom_string_create(const uint8_t *ptr, size_t len, 
		dom_string **str)
{
	dom_string_internal *ret;

	if (ptr == NULL || len == 0) {
		ptr = (const uint8_t *) "";
		len = 0;
	}

	ret = (dom_string_internal *)malloc(sizeof(*ret));
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->data.cdata.ptr = (uint8_t *)malloc(len + 1);
	if (ret->data.cdata.ptr == NULL) {
		free(ret);
		return DOM_NO_MEM_ERR;
	}

	memcpy(ret->data.cdata.ptr, ptr, len);
	ret->data.cdata.ptr[len] = '\0';

	ret->data.cdata.len = len;

	ret->base.refcnt = 1;

	ret->type = DOM_STRING_CDATA;

	*str = (dom_string *)ret;

	return DOM_NO_ERR;
}

/**
 * Create an interned DOM string from a string of characters
 *
 * \param ptr    Pointer to string of characters
 * \param len    Length, in bytes, of string of characters
 * \param str    Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will be copied for use by the 
 * returned DOM string.
 */
dom_exception dom_string_create_interned(const uint8_t *ptr, size_t len, 
		dom_string **str)
{
	dom_string_internal *ret;

	if (ptr == NULL || len == 0) {
		ptr = (const uint8_t *) "";
		len = 0;
	}

	ret = (dom_string_internal *)malloc(sizeof(*ret));
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	if (lwc_intern_string((const char *) ptr, len, 
			&ret->data.intern) != lwc_error_ok) {
		free(ret);
		return DOM_NO_MEM_ERR;
	}

	ret->base.refcnt = 1;

	ret->type = DOM_STRING_INTERNED;

	*str = (dom_string *)ret;

	return DOM_NO_ERR;
}

/**
 * Make the dom_string be interned
 *
 * \param str     The dom_string to be interned
 * \param lwcstr  The result lwc_string	
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_string_intern(dom_string *str, 
		struct lwc_string_s **lwcstr)
{
	dom_string_internal *istr = (dom_string_internal *) str;
	/* If this string is already interned, do nothing */
	if (istr->type != DOM_STRING_INTERNED) {
		lwc_string *ret;
		lwc_error lerr;

		lerr = lwc_intern_string((const char *) istr->data.cdata.ptr, 
				istr->data.cdata.len, &ret);
		if (lerr != lwc_error_ok) {
			return _dom_exception_from_lwc_error(lerr);
		}

		free(istr->data.cdata.ptr);

		istr->data.intern = ret;

		istr->type = DOM_STRING_INTERNED;
	}

	*lwcstr = lwc_string_ref(istr->data.intern);

	return DOM_NO_ERR;
}

/**
 * Case sensitively compare two DOM strings
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return true if strings match, false otherwise
 */
bool dom_string_isequal(const dom_string *s1, const dom_string *s2)
{
	size_t len;
	const dom_string_internal *is1 = (dom_string_internal *) s1;
	const dom_string_internal *is2 = (dom_string_internal *) s2;

	if (s1 == NULL)
		is1 = &empty_string;

	if (s2 == NULL)
		is2 = &empty_string;

	if (is1->type == DOM_STRING_INTERNED && 
			is2->type == DOM_STRING_INTERNED) {
		bool match;

		(void) lwc_string_isequal(is1->data.intern, is2->data.intern,
			&match);

		return match;
	}

	len = dom_string_byte_length((dom_string *) is1);

	if (len != dom_string_byte_length((dom_string *)is2))
		return false;

	return 0 == memcmp(dom_string_data((dom_string *) is1), dom_string_data((dom_string *)is2), len);
}

/**
 * Trivial locale-agnostic lower case convertor
 */
static inline uint8_t dolower(const uint8_t c)
{
	if ('A' <= c && c <= 'Z')
		return c + 'a' - 'A';
	return c;
}

/**
 * Case insensitively compare two DOM strings
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return true if strings match, false otherwise
 */
bool dom_string_caseless_isequal(const dom_string *s1, const dom_string *s2)
{
	const uint8_t *d1 = NULL;
	const uint8_t *d2 = NULL;
	size_t len;
	const dom_string_internal *is1 = (dom_string_internal *) s1;
	const dom_string_internal *is2 = (dom_string_internal *) s2;

	if (s1 == NULL)
		is1 = &empty_string;

	if (s2 == NULL)
		is2 = &empty_string;

	if (is1->type == DOM_STRING_INTERNED && 
			is2->type == DOM_STRING_INTERNED) {
		bool match;

		if (lwc_string_caseless_isequal(is1->data.intern,
				is2->data.intern, &match) != lwc_error_ok)
			return false;

		return match;
	}

	len = dom_string_byte_length((dom_string *) is1);

	if (len != dom_string_byte_length((dom_string *)is2))
		return false;

	d1 = (const uint8_t *) dom_string_data((dom_string *) is1);
	d2 = (const uint8_t *) dom_string_data((dom_string *)is2);

	while (len > 0) {
		if (dolower(*d1) != dolower(*d2))
			return false;

		d1++;
		d2++;
		len--;
	}

	return true;
}


/**
 * Case sensitively compare DOM string with lwc_string
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return true if strings match, false otherwise
 *
 * Returns false if either are NULL.
 */
bool dom_string_lwc_isequal(const dom_string *s1, lwc_string *s2)
{
	size_t len;
	dom_string_internal *is1 = (dom_string_internal *) s1;

	if (s1 == NULL || s2 == NULL)
		return false;

	if (is1->type == DOM_STRING_INTERNED) {
		bool match;

		(void) lwc_string_isequal(is1->data.intern, s2, &match);

		return match;
	}

	/* Handle non-interned case */
	len = dom_string_byte_length(s1);

	if (len != lwc_string_length(s2))
		return false;

	return 0 == memcmp(dom_string_data(s1), lwc_string_data(s2), len);
}


/**
 * Case insensitively compare DOM string with lwc_string
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return true if strings match, false otherwise
 *
 * Returns false if either are NULL.
 */
bool dom_string_caseless_lwc_isequal(const dom_string *s1, lwc_string *s2)
{
	size_t len;
	const uint8_t *d1 = NULL;
	const uint8_t *d2 = NULL;
	dom_string_internal *is1 = (dom_string_internal *) s1;

	if (s1 == NULL || s2 == NULL)
		return false;

	if (is1->type == DOM_STRING_INTERNED) {
		bool match;

		if (lwc_string_caseless_isequal(is1->data.intern, s2, &match) != lwc_error_ok)
			return false;

		return match;
	}

	len = dom_string_byte_length(s1);

	if (len != lwc_string_length(s2))
		return false;

	d1 = (const uint8_t *) dom_string_data(s1);
	d2 = (const uint8_t *) lwc_string_data(s2);

	while (len > 0) {
		if (dolower(*d1) != dolower(*d2))
			return false;

		d1++;
		d2++;
		len--;
	}

	return true;
}


/**
 * Get the index of the first occurrence of a character in a dom string 
 * 
 * \param str  The string to search in
 * \param chr  UCS4 value to look for
 * \return Character index of found character, or -1 if none found 
 */
uint32_t dom_string_index(dom_string *str, uint32_t chr)
{
	const uint8_t *s;
	size_t clen, slen;
	uint32_t c, index;
	parserutils_error err;

	s = (const uint8_t *) dom_string_data(str);
	slen = dom_string_byte_length(str);

	index = 0;

	while (slen > 0) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &c, &clen);
		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		if (c == chr) {
			return index;
		}

		s += clen;
		slen -= clen;
		index++;
	}

	return (uint32_t) -1;
}

/**
 * Get the index of the last occurrence of a character in a dom string 
 * 
 * \param str  The string to search in
 * \param chr  UCS4 value to look for
 * \return Character index of found character, or -1 if none found
 */
uint32_t dom_string_rindex(dom_string *str, uint32_t chr)
{
	const uint8_t *s;
	size_t clen = 0, slen;
	uint32_t c, coff, index;
	parserutils_error err;

	s = (const uint8_t *) dom_string_data(str);
	slen = dom_string_byte_length(str);

	index = dom_string_length(str);

	while (slen > 0) {
		err = parserutils_charset_utf8_prev(s, slen, 
				(uint32_t *) &coff);
		if (err == PARSERUTILS_OK) {
			err = parserutils_charset_utf8_to_ucs4(s + coff, 
					slen - clen, &c, &clen);
		}

		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		if (c == chr) {
			return index;
		}

		slen -= clen;
		index--;
	}

	return (uint32_t) -1;
}

/**
 * Get the length, in characters, of a dom string
 *
 * \param str  The string to measure the length of
 * \return The length of the string, in characters
 */
uint32_t dom_string_length(dom_string *str)
{
	const uint8_t *s;
	size_t slen, clen;
	parserutils_error err;

	s = (const uint8_t *) dom_string_data(str);
	slen = dom_string_byte_length(str);

	err = parserutils_charset_utf8_length(s, slen, &clen);
	if (err != PARSERUTILS_OK) {
		return 0;
	}

	return clen;
}

/**
 * Get the UCS4 character at position index
 *
 * \param index  The position of the charater
 * \param ch     The UCS4 character
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_string_at(dom_string *str, uint32_t index, 
		uint32_t *ch)
{
	const uint8_t *s;
	size_t clen, slen;
	uint32_t c, i;
	parserutils_error err;

	s = (const uint8_t *) dom_string_data(str);
	slen = dom_string_byte_length(str);

	i = 0;

	while (slen > 0) {
		err = parserutils_charset_utf8_char_byte_length(s, &clen);
		if (err != PARSERUTILS_OK) {
			return (dom_exception) -1;
		}

		i++;
		if (i == index + 1)
			break;

		s += clen;
		slen -= clen;
	}

	if (i == index + 1) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &c, &clen);
		if (err != PARSERUTILS_OK) {
			return (dom_exception) -1;
		}

		*ch = c;
		return DOM_NO_ERR;
	} else {
		return DOM_DOMSTRING_SIZE_ERR;
	}
}

/** 
 * Concatenate two dom strings 
 * 
 * \param s1      The first string
 * \param s2      The second string
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will be referenced. The client
 * should dereference it once it has finished with it.
 */
dom_exception dom_string_concat(dom_string *s1, dom_string *s2,
		dom_string **result)
{
	dom_string_internal *concat;
	const uint8_t *s1ptr, *s2ptr;
	size_t s1len, s2len;

	assert(s1 != NULL);
	assert(s2 != NULL);

	s1ptr = (const uint8_t *) dom_string_data(s1);
	s2ptr = (const uint8_t *) dom_string_data(s2);
	s1len = dom_string_byte_length(s1);
	s2len = dom_string_byte_length(s2);

	concat = (dom_string_internal *)malloc(sizeof(*concat));
	if (concat == NULL) {
		return DOM_NO_MEM_ERR;
	}

	concat->data.cdata.ptr = (uint8_t *)malloc(s1len + s2len + 1);
	if (concat->data.cdata.ptr == NULL) {
		free(concat);

		return DOM_NO_MEM_ERR;
	}

	memcpy(concat->data.cdata.ptr, s1ptr, s1len);

	memcpy(concat->data.cdata.ptr + s1len, s2ptr, s2len);

	concat->data.cdata.ptr[s1len + s2len] = '\0';

	concat->data.cdata.len = s1len + s2len;

	concat->base.refcnt = 1;

	concat->type = DOM_STRING_CDATA;

	*result = (dom_string *)concat;

	return DOM_NO_ERR;
}

/**
 * Extract a substring from a dom string 
 *
 * \param str     The string to extract from
 * \param i1      The character index of the start of the substring
 * \param i2      The character index of the end of the substring
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it.
 */
dom_exception dom_string_substr(dom_string *str, 
		uint32_t i1, uint32_t i2, dom_string **result)
{
	const uint8_t *s;
	size_t slen;
	uint32_t b1, b2;
	parserutils_error err;

	/* target string is NULL equivalent to empty. */
	if (str == NULL)
		str = (dom_string *)&empty_string;

	s = (const uint8_t *) dom_string_data(str);
	slen = dom_string_byte_length(str);

	/* Initialise the byte index of the start to 0 */
	b1 = 0;
	/* Make the end a character offset from the start */
	i2 -= i1;

	/* Calculate the byte index of the start */
	while (i1 > 0) {
		err = parserutils_charset_utf8_next(s, slen, b1, &b1);
		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(s, slen, b2, &b2);
		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i2--;
	}

	/* Create a string from the specified byte range */
	return dom_string_create(s + b1, b2 - b1, result);
}

/**
 * Insert data into a dom string at the given location
 *
 * \param target  Pointer to string to insert into
 * \param source  Pointer to string to insert
 * \param offset  Character offset of location to insert at
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR          on success, 
 *         DOM_NO_MEM_ERR      on memory exhaustion,
 *         DOM_INDEX_SIZE_ERR  if ::offset > len(::target).
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it. 
 */
dom_exception dom_string_insert(dom_string *target,
		dom_string *source, uint32_t offset,
		dom_string **result)
{
	dom_string_internal *res;
	const uint8_t *t, *s;
	uint32_t tlen, slen, clen;
	uint32_t ins = 0;
	parserutils_error err;

	/* target string is NULL equivalent to empty. */
	if (target == NULL)
		target = (dom_string *)&empty_string;

	t = (const uint8_t *) dom_string_data(target);
	tlen = dom_string_byte_length(target);
	s = (const uint8_t *) dom_string_data(source);
	slen = dom_string_byte_length(source);

	clen = dom_string_length(target);

	if (offset > clen)
		return DOM_INDEX_SIZE_ERR;

	/* Calculate the byte index of the insertion point */
	if (offset == clen) {
		/* Optimisation for append */
		ins = tlen;
	} else {
		while (offset > 0) {
			err = parserutils_charset_utf8_next(t, tlen, 
					ins, &ins);

			if (err != PARSERUTILS_OK) {
				return DOM_NO_MEM_ERR;
			}

			offset--;
		}
	}

	/* Allocate result string */
	res = (dom_string_internal *)malloc(sizeof(*res));
	if (res == NULL) {
		return DOM_NO_MEM_ERR;
	}

	/* Allocate data buffer for result contents */
	res->data.cdata.ptr = (uint8_t *)malloc(tlen + slen + 1);
	if (res->data.cdata.ptr == NULL) {
		free(res);
		return DOM_NO_MEM_ERR;
	}

	/* Copy initial portion of target, if any, into result */
	if (ins > 0) {
		memcpy(res->data.cdata.ptr, t, ins);
	}

	/* Copy inserted data into result */
	memcpy(res->data.cdata.ptr + ins, s, slen);

	/* Copy remainder of target, if any, into result */
	if (tlen - ins > 0) {
		memcpy(res->data.cdata.ptr + ins + slen, t + ins, tlen - ins);
	}

	res->data.cdata.ptr[tlen + slen] = '\0';

	res->data.cdata.len = tlen + slen;

	res->base.refcnt = 1;

	res->type = DOM_STRING_CDATA;

	*result = (dom_string *)res;

	return DOM_NO_ERR;
}

/** 
 * Replace a section of a dom string
 *
 * \param target  Pointer to string of which to replace a section
 * \param source  Pointer to replacement string
 * \param i1      Character index of start of region to replace
 * \param i2      Character index of end of region to replace
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it. 
 */
dom_exception dom_string_replace(dom_string *target,
		dom_string *source, uint32_t i1, uint32_t i2,
		dom_string **result)
{
	dom_string_internal *res;
	const uint8_t *t, *s;
	uint32_t tlen, slen;
	uint32_t b1, b2;
	parserutils_error err;

	/* target string is NULL equivalent to empty. */
	if (target == NULL)
		target = (dom_string *)&empty_string;

	t = (const uint8_t *) dom_string_data(target);
	tlen = dom_string_byte_length(target);
	s = (const uint8_t *) dom_string_data(source);
	slen = dom_string_byte_length(source);

	/* Initialise the byte index of the start to 0 */
	b1 = 0;
	/* Make the end a character offset from the start */
	i2 -= i1;

	/* Calculate the byte index of the start */
	while (i1 > 0) {
		err = parserutils_charset_utf8_next(t, tlen, b1, &b1);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(t, tlen, b2, &b2);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i2--;
	}

	/* Allocate result string */
	res = (dom_string_internal *)malloc(sizeof(*res));
	if (res == NULL) {
		return DOM_NO_MEM_ERR;
	}

	/* Allocate data buffer for result contents */
	res->data.cdata.ptr = (uint8_t *)malloc(tlen + slen - (b2 - b1) + 1);
	if (res->data.cdata.ptr == NULL) {
		free(res);
		return DOM_NO_MEM_ERR;
	}

	/* Copy initial portion of target, if any, into result */
	if (b1 > 0) {
		memcpy(res->data.cdata.ptr, t, b1);
	}

	/* Copy replacement data into result */
	if (slen > 0) {
		memcpy(res->data.cdata.ptr + b1, s, slen);
	}

	/* Copy remainder of target, if any, into result */
	if (tlen - b2 > 0) {
		memcpy(res->data.cdata.ptr + b1 + slen, t + b2, tlen - b2);
	}

	res->data.cdata.ptr[tlen + slen - (b2 - b1)] = '\0';

	res->data.cdata.len = tlen + slen - (b2 - b1);

	res->base.refcnt = 1;

	res->type = DOM_STRING_CDATA;

	*result = (dom_string *)res;

	return DOM_NO_ERR;
}

/**
 * Calculate a hash value from a dom string 
 *
 * \param str  The string to calculate a hash of
 * \return The hash value associated with the string
 */
uint32_t dom_string_hash(dom_string *str)
{
	const uint8_t *s = (const uint8_t *) dom_string_data(str);
	size_t slen = dom_string_byte_length(str);
	uint32_t hash = 0x811c9dc5;

	while (slen > 0) {
		hash *= 0x01000193;
		hash ^= *s;

		s++;
		slen--;
	}

	return hash;
}

/**
 * Convert a lwc_error to a dom_exception
 * 
 * \param err  The input lwc_error
 * \return the dom_exception
 */
dom_exception _dom_exception_from_lwc_error(lwc_error err)
{
	switch (err) {
	case lwc_error_ok:
		return DOM_NO_ERR;
	case lwc_error_oom:
		return DOM_NO_MEM_ERR;
	case lwc_error_range:
		return DOM_INDEX_SIZE_ERR;
	}

	return DOM_NO_ERR;
}

/**
 * Get the raw character data of the dom_string.
 *
 * \param str	The dom_string object
 * \return      The C string pointer
 *
 * @note: This function is just provided for the convenience of accessing the 
 * raw C string character, no change on the result string is allowed.
 */
const char *dom_string_data(const dom_string *str)
{
	dom_string_internal *istr = (dom_string_internal *) str;
	if (istr->type == DOM_STRING_CDATA) {
		return (const char *) istr->data.cdata.ptr;
	} else {
		return lwc_string_data(istr->data.intern);
	}
}

/** Get the byte length of this dom_string 
 *
 * \param str	The dom_string object
 */
size_t dom_string_byte_length(const dom_string *str)
{
	dom_string_internal *istr = (dom_string_internal *) str;
	if (istr->type == DOM_STRING_CDATA) {
		return istr->data.cdata.len;
	} else {
		return lwc_string_length(istr->data.intern);
	}
}

/** Convert the given string to uppercase
 *
 * \param source 
 * \param ascii_only  Whether to only convert [a-z] to [A-Z]
 * \param upper       Result pointer for uppercase string.  Caller owns ref
 *
 * \return DOM_NO_ERR on success.
 *
 * \note Right now, will return DOM_NOT_SUPPORTED_ERR if ascii_only is false.
 */
dom_exception
dom_string_toupper(dom_string *source, bool ascii_only, dom_string **upper)
{
	const uint8_t *orig_s = (const uint8_t *) dom_string_data(source);
	const size_t nbytes = dom_string_byte_length(source);
	uint8_t *copy_s;
	size_t index = 0;
	dom_exception exc;
	
	if (ascii_only == false)
		return DOM_NOT_SUPPORTED_ERR;
	
	copy_s = (uint8_t *)malloc(nbytes);
	if (copy_s == NULL)
		return DOM_NO_MEM_ERR;
	memcpy(copy_s, orig_s, nbytes);
	
	while (index < nbytes) {
		if (orig_s[index] >= 'a' && orig_s[index] <= 'z') {
			copy_s[index] -= 'a' - 'A';
		}
		
		index++;
	}
	
	if (((dom_string_internal*)source)->type == DOM_STRING_CDATA) {
		exc = dom_string_create(copy_s, nbytes, upper);
	} else {
		exc = dom_string_create_interned(copy_s, nbytes, upper);
	}
	
	free(copy_s);
	
	return exc;
}

/** Convert the given string to lowercase
 *
 * \param source 
 * \param ascii_only  Whether to only convert [a-z] to [A-Z]
 * \param lower       Result pointer for lowercase string.  Caller owns ref
 *
 * \return DOM_NO_ERR on success.
 *
 * \note Right now, will return DOM_NOT_SUPPORTED_ERR if ascii_only is false.
 */
dom_exception
dom_string_tolower(dom_string *source, bool ascii_only, dom_string **lower)
{
	dom_string_internal *isource = (dom_string_internal *)source;
	dom_exception exc = DOM_NO_ERR;

	if (ascii_only == false)
		return DOM_NOT_SUPPORTED_ERR;

	if (isource->type == DOM_STRING_CDATA) {
		const uint8_t *orig_s = (const uint8_t *)
				dom_string_data(source);
		const size_t nbytes = dom_string_byte_length(source);
		size_t index = 0;
		uint8_t *copy_s;

		copy_s = (uint8_t *)malloc(nbytes);
		if (copy_s == NULL)
			return DOM_NO_MEM_ERR;
		memcpy(copy_s, orig_s, nbytes);

		while (index < nbytes) {
			if (orig_s[index] >= 'A' && orig_s[index] <= 'Z') {
				copy_s[index] += 'a' - 'A';
			}

			index++;
		}
		exc = dom_string_create(copy_s, nbytes, lower);

		free(copy_s);
	} else {
		bool equal;
		lwc_error err;
		lwc_string *l;

		err = lwc_string_tolower(isource->data.intern, &l);
		if (err != lwc_error_ok) {
			return DOM_NO_MEM_ERR;
		}

		if (lwc_string_isequal(isource->data.intern, l, &equal) ==
				lwc_error_ok && equal == true) {
			/* String is already lower case. */
			*lower = dom_string_ref(source);
		} else {
			/* TODO: create dom_string wrapper around existing
			 *       lwc string. */
			exc = dom_string_create_interned(
					(const uint8_t *)lwc_string_data(l),
					lwc_string_length(l), lower);
		}
		lwc_string_unref(l);
	}
	
	return exc;
}

/* exported function documented in string.h */
dom_exception dom_string_whitespace_op(dom_string *s,
		enum dom_whitespace_op op, dom_string **ret)
{
	const uint8_t *src_text = (const uint8_t *) dom_string_data(s);
	size_t len = dom_string_byte_length(s);
	const uint8_t *src_pos;
	const uint8_t *src_end;
	dom_exception exc;
	uint8_t *temp_pos;
	uint8_t *temp;

	if (len == 0) {
		*ret = dom_string_ref(s);
	}

	temp = (uint8_t *)malloc(len);
	if (temp == NULL) {
		return DOM_NO_MEM_ERR;
	}

	src_pos = src_text;
	src_end = src_text + len;
	temp_pos = temp;

	if (op & DOM_WHITESPACE_STRIP_LEADING) {
		while (src_pos < src_end) {
			if (*src_pos == ' '  || *src_pos == '\t' ||
			    *src_pos == '\n' || *src_pos == '\r' ||
			    *src_pos == '\f')
				src_pos++;
			else
				break;
		}
	}

	while (src_pos < src_end) {
		if ((op & DOM_WHITESPACE_COLLAPSE) &&
				(*src_pos == ' ' || *src_pos == '\t' ||
				*src_pos == '\n' || *src_pos == '\r' ||
				*src_pos == '\f')) {
			/* Got a whitespace character */
			do {
				/* Skip all adjacent whitespace */
				src_pos++;
			} while (src_pos < src_end &&
					(*src_pos == ' ' || *src_pos == '\t' ||
					*src_pos == '\n' || *src_pos == '\r' ||
					*src_pos == '\f'));
			/* Gets replaced with single space in output */
			*temp_pos++ = ' ';
		} else {
			/* Otherwise, copy to output */
			*temp_pos++ = *src_pos++;
		}
	}

	if (op & DOM_WHITESPACE_STRIP_TRAILING) {
		while (temp_pos > temp) {
			temp_pos--;
			if (*temp_pos != ' '  && *temp_pos != '\t' &&
			    *temp_pos != '\n' && *temp_pos != '\r' &&
			    *temp_pos != '\f') {
				temp_pos++;
				break;
			}
		}
	}

	/* New length */
	len = temp_pos - temp;

	/* Make new string */
	if (((dom_string_internal *) s)->type == DOM_STRING_CDATA) {
		exc = dom_string_create(temp, len, ret);
	} else {
		exc = dom_string_create_interned(temp, len, ret);
	}

	free(temp);

	return exc;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/* The virtual table for dom_text */
struct dom_text_vtable text_vtable = {
	{
		{
			DOM_NODE_VTABLE_CHARACTERDATA
		},
		DOM_CHARACTERDATA_VTABLE
	},
	DOM_TEXT_VTABLE
};

static struct dom_node_protect_vtable text_protect_vtable = {
	DOM_TEXT_PROTECT_VTABLE
};

/* Following comes helper functions */
typedef enum walk_operation {
	WALK_COLLECT,
	WALK_DELETE
} walk_operation;
typedef enum walk_order {
	WALK_LEFT,
	WALK_RIGHT
} walk_order;

/* Walk the logic-adjacent text in document order */
static dom_exception walk_logic_adjacent_text_in_order(
		dom_node_internal *node, walk_operation opt,
		walk_order order, dom_string **ret, bool *cont);
/* Walk the logic-adjacent text */
static dom_exception walk_logic_adjacent_text(dom_text *text, 
		walk_operation opt, dom_string **ret);
	
/*----------------------------------------------------------------------*/
/* Constructor and Destructor */

/**
 * Create a text node
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception _dom_text_create(dom_document *doc,
		dom_string *name, dom_string *value,
		dom_text **result)
{
	dom_text *t;
	dom_exception err;

	/* Allocate the text node */
	t = (dom_text *)malloc(sizeof(dom_text));
	if (t == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = _dom_text_initialise(t, doc, DOM_TEXT_NODE, name, value);
	if (err != DOM_NO_ERR) {
		free(t);
		return err;
	}

	/* Compose the vtable */
	((dom_node *) t)->vtable = &text_vtable;
	((dom_node_internal *) t)->vtable = &text_protect_vtable;

	*result = t;

	return DOM_NO_ERR;
}

/**
 * Destroy a text node
 *
 * \param doc   The owning document
 * \param text  The text node to destroy
 *
 * The contents of ::text will be destroyed and ::text will be freed.
 */
void _dom_text_destroy(dom_text *text)
{
	/* Finalise node */
	_dom_text_finalise(text);

	/* Free node */
	free(text);
}

/**
 * Initialise a text node
 *
 * \param text   The node to initialise
 * \param doc    The owning document
 * \param type   The type of the node
 * \param name   The name of the node to create
 * \param value  The text content of the node
 * \return DOM_NO_ERR on success.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 */
dom_exception _dom_text_initialise(dom_text *text,
		dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value)
{
	dom_exception err;

	/* Initialise the base class */
	err = _dom_characterdata_initialise(&text->base, doc, type,
			name, value);
	if (err != DOM_NO_ERR)
		return err;

	/* Perform our type-specific initialisation */
	text->element_content_whitespace = false;

	return DOM_NO_ERR;
}

/**
 * Finalise a text node
 *
 * \param doc   The owning document
 * \param text  The text node to finalise
 *
 * The contents of ::text will be cleaned up. ::text will not be freed.
 */
void _dom_text_finalise(dom_text *text)
{
	_dom_characterdata_finalise(&text->base);
}

/*----------------------------------------------------------------------*/
/* The public virtual functions */

/**
 * Split a text node at a given character offset
 *
 * \param text  The node to split
 * \param offset  Character offset to split at
 * \param result  Pointer to location to receive new node
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if ::offset is greater than the
 *                                         number of characters in ::text,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::text is readonly.
 *
 * The returned node will be referenced. The client should unref the node
 * once it has finished with it.
 */
dom_exception _dom_text_split_text(dom_text *text,
		uint32_t offset, dom_text **result)
{
	dom_node_internal *t = (dom_node_internal *) text;
	dom_string *value;
	dom_text *res;
	uint32_t len;
	dom_exception err;

	if (_dom_node_readonly(t)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	err = dom_characterdata_get_length(&text->base, &len);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (offset >= len) {
		return DOM_INDEX_SIZE_ERR;
	}

	/* Retrieve the data after the split point -- 
	 * this will be the new node's value. */
	err = dom_characterdata_substring_data(&text->base, offset, 
			len - offset, &value);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Create new node */
	err = _dom_text_create(t->owner, t->name, value, &res);
	if (err != DOM_NO_ERR) {
		dom_string_unref(value);
		return err;
	}

	/* Release our reference on the value (the new node has its own) */
	dom_string_unref(value);

	/* Delete the data after the split point */
	err = dom_characterdata_delete_data(&text->base, offset, len - offset);
	if (err != DOM_NO_ERR) {
		dom_node_unref(res);
		return err;
	}

	*result = res;

	return DOM_NO_ERR;
}

/**
 * Determine if a text node contains element content whitespace
 *
 * \param text    The node to consider
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_text_get_is_element_content_whitespace(
		dom_text *text, bool *result)
{
	*result = text->element_content_whitespace;

	return DOM_NO_ERR;
}

/**
 * Retrieve all text in Text nodes logically adjacent to a Text node
 *
 * \param text    Text node to consider
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_text_get_whole_text(dom_text *text,
		dom_string **result)
{
	return walk_logic_adjacent_text(text, WALK_COLLECT, result);
}

/**
 * Replace the text of a Text node and all logically adjacent Text nodes
 *
 * \param text     Text node to consider
 * \param content  Replacement content
 * \param result   Pointer to location to receive Text node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if one of the Text nodes being
 *                                         replaced is readonly.
 *
 * The returned node will be referenced. The client should unref the node
 * once it has finished with it.
 */
dom_exception _dom_text_replace_whole_text(dom_text *text,
		dom_string *content, dom_text **result)
{
	dom_exception err;
	dom_string *ret;

	err = walk_logic_adjacent_text(text, WALK_DELETE, &ret);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_characterdata_set_data(text, content);
	if (err != DOM_NO_ERR)
		return err;
	
	*result = text;
	dom_node_ref(text);

	return DOM_NO_ERR;
}

/*-----------------------------------------------------------------------*/
/* The protected virtual functions */

/* The destroy function of this class */
void __dom_text_destroy(dom_node_internal *node)
{
	_dom_text_destroy((dom_text *) node);
}

/* The copy constructor of this class */
dom_exception _dom_text_copy(dom_node_internal *old, dom_node_internal **copy)
{
	dom_text *new_text;
	dom_exception err;

	new_text = (dom_text *)malloc(sizeof(dom_text));
	if (new_text == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_text_copy_internal(old, new_text);
	if (err != DOM_NO_ERR) {
		free(new_text);
		return err;
	}

	*copy = (dom_node_internal *) new_text;

	return DOM_NO_ERR;
}

dom_exception _dom_text_copy_internal(dom_text *old, dom_text *newt)
{
	dom_exception err;

	err = dom_characterdata_copy_internal(old, newt);
	if (err != DOM_NO_ERR)
		return err;

	newt->element_content_whitespace = old->element_content_whitespace;

	return DOM_NO_ERR;
}

/*----------------------------------------------------------------------*/
/* Helper functions */

/**
 * Walk the logic adjacent text in certain order
 *
 * \param node   The start Text node
 * \param opt    The operation on each Text Node
 * \param order  The order
 * \param ret    The string of the logic adjacent text 
 * \param cont   Whether the logic adjacent text is interrupt here
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception walk_logic_adjacent_text_in_order(
		dom_node_internal *node, walk_operation opt,
		walk_order order, dom_string **ret, bool *cont)
{
	dom_exception err;
	dom_string *data, *tmp;
	dom_node_internal *parent;

	/* If we reach the leaf of the DOM tree, just return to continue
	 * to next sibling of our parent */
	if (node == NULL) {
		*cont = true;
		return DOM_NO_ERR;
	}

	parent = dom_node_get_parent(node);

	while (node != NULL) {
		dom_node_internal *p;

		/* If we reach the boundary of logical-adjacent text, we stop */
		if (node->type == DOM_ELEMENT_NODE || 
				node->type == DOM_COMMENT_NODE ||
				node->type == 
				DOM_PROCESSING_INSTRUCTION_NODE) {
			*cont = false;
			return DOM_NO_ERR;
		}

		if (node->type == DOM_TEXT_NODE) {
			/* According the DOM spec, text node never have child */
			assert(node->first_child == NULL);
			assert(node->last_child == NULL);
			if (opt == WALK_COLLECT) {
				err = dom_characterdata_get_data(node, &data);
				if (err != DOM_NO_ERR)
					return err;

				tmp = *ret;
				if (order == LEFT) {
					if (tmp != NULL) {
						err = dom_string_concat(data, tmp, ret);
						if (err != DOM_NO_ERR)
							return err;
					} else {
						dom_string_ref(data);
						*ret = data;
					}
				} else if (order == WALK_RIGHT) {
					if (tmp != NULL) {
						err = dom_string_concat(tmp, data, ret);
						if (err != DOM_NO_ERR)
							return err;
					} else {
						dom_string_ref(data);
						*ret = data;
					}
				}

				if (tmp != NULL)
					dom_string_unref(tmp);
				dom_string_unref(data);

				*cont = true;
				return DOM_NO_ERR;
			}

			if (opt == WALK_DELETE) {
				dom_node_internal *tn;
				err = dom_node_remove_child(node->parent,
						node, (void *) &tn);
				if (err != DOM_NO_ERR)
					return err;

				*cont = true;
				dom_node_unref(tn);
				return DOM_NO_ERR;
			}
		}

		p = dom_node_get_parent(node);
		if (order == WALK_LEFT) {
			if (node->last_child != NULL) {
				node = node->last_child;
			} else if (node->previous != NULL) {
				node = node->previous;
			} else {
				while (p != parent && node == p->last_child) {
					node = p;
					p = dom_node_get_parent(p);
				}

				node = node->previous;
			}
		} else {
			if (node->first_child != NULL) {
				node = node->first_child;
			} else if (node->next != NULL) {
				node = node->next;
			} else {
				while (p != parent && node == p->first_child) {
					node = p;
					p = dom_node_get_parent(p);
				}

				node = node->next;
			}
		}
	}

	return DOM_NO_ERR;
}

/**
 * Traverse the logic adjacent text.
 *
 * \param text  The Text Node from which we start traversal
 * \param opt   The operation code
 * \param ret   The returned string if the opt is WALK_COLLECT
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception walk_logic_adjacent_text(dom_text *text, 
		walk_operation opt, dom_string **ret)
{
	dom_node_internal *node = (dom_node_internal *) text;
	dom_node_internal *parent = node->parent;
	dom_node_internal *left = node->previous;
	dom_node_internal *right = node->next;
	dom_exception err;
	bool cont;
	
	if (parent->type == DOM_ENTITY_NODE) {
		return DOM_NOT_SUPPORTED_ERR;
	}

	*ret = NULL;

	/* Firstly, we look our left */
	err = walk_logic_adjacent_text_in_order(left, opt, WALK_LEFT, ret, &cont);
	if (err != DOM_NO_ERR) {
		if (opt == WALK_COLLECT) {
			dom_string_unref(*ret);
			*ret = NULL;
		}
		return err;
	}

	/* Ourself */
	if (opt == WALK_COLLECT) {
		dom_string *data = NULL, *tmp = NULL;
		err = dom_characterdata_get_data(text, &data);
		if (err != DOM_NO_ERR) {
			dom_string_unref(*ret);
			return err;
		}

		if (*ret != NULL) {
			err = dom_string_concat(*ret, data, &tmp);
			dom_string_unref(data);
			dom_string_unref(*ret);
			if (err != DOM_NO_ERR) {
				return err;
			}

			*ret = tmp;
		} else {
			*ret = data;
		}
	} else {
			dom_node_internal *tn;
			err = dom_node_remove_child(node->parent, node,
					(void *) &tn);
			if (err != DOM_NO_ERR)
				return err;
			dom_node_unref(tn);
	}

	/* Now, look right */
	err = walk_logic_adjacent_text_in_order(right, opt, WALK_RIGHT, ret, &cont);
	if (err != DOM_NO_ERR) {
		if (opt == WALK_COLLECT) {
			dom_string_unref(*ret);
			*ret = NULL;
		}
		return err;
	}

	return DOM_NO_ERR;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/* TypeInfo object */
struct dom_type_info {
	struct lwc_string_s *type;	/**< Type name */
	struct lwc_string_s *ns;	/**< Type namespace */
};

/**
 * Get the type name of this dom_type_info
 *
 * \param ti   The dom_type_info
 * \param ret  The name
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so this function call always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_type_info_get_type_name(dom_type_info *ti, 
		dom_string **ret)
{
	UNUSED(ti);
	UNUSED(ret);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Get the namespace of this type info
 *
 * \param ti   The dom_type_info
 * \param ret  The namespace
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so this function call always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_type_info_get_type_namespace(dom_type_info *ti,
		dom_string **ret)
{
	UNUSED(ti);
	UNUSED(ret);
	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Whether this type info is derived from another one
 *
 * \param ti         The dom_type_info
 * \param ns         The namespace of name
 * \param name       The name of the base typeinfo
 * \param method     The deriving method
 * \param ret        The return value
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so this function call always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_type_info_is_derived(dom_type_info *ti,
		dom_string *ns, dom_string *name,
		dom_type_info_derivation_method method, bool *ret)
{
	UNUSED(ti);
	UNUSED(ns);
	UNUSED(name);
	UNUSED(method);
	UNUSED(ret);
	return DOM_NOT_SUPPORTED_ERR;
}

