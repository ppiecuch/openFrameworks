/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

#include <dom/dom.h>
#include <libwapcaplet/libwapcaplet.h>
#include <parserutils/parserutils.h>

#include "utils_p.h"

static const struct xml_char_range base_char_range[] = { {0x41, 0x5a}, 
	{0x61, 0x7a}, {0xc0, 0xd6}, {0xd8, 0xf6}, {0x00f8, 0x00ff},
	{0x100, 0x131}, {0x134, 0x13e}, {0x141, 0x148}, {0x14a, 0x17e},
	{0x180, 0x1c3}, {0x1cd, 0x1f0}, {0x1f4, 0x1f5}, {0x1fa, 0x217},
	{0x250, 0x2a8}, {0x2bb, 0x2c1}, {0x386, 0x386}, {0x388, 0x38a},
	{0x38c, 0x38c}, {0x38e, 0x3a1}, {0x3a3, 0x3ce}, {0x3d0, 0x3d6},
	{0x3da, 0x3da}, {0x3dc, 0x3dc}, {0x3de, 0x3de}, {0x3e0, 0x3e0},
	{0x3e2, 0x3f3}, {0x401, 0x40c}, {0x40e, 0x44f}, {0x451, 0x45c},
	{0x45e, 0x481}, {0x490, 0x4c4}, {0x4c7, 0x4c8}, {0x4cb, 0x4cc},
	{0x4d0, 0x4eb}, {0x4ee, 0x4f5}, {0x4f8, 0x4f9}, {0x531, 0x556},
	{0x559, 0x559}, {0x561, 0x586}, {0x5d0, 0x5ea}, {0x5f0, 0x5f2},
	{0x621, 0x63a}, {0x641, 0x64a}, {0x671, 0x6b7}, {0x6ba, 0x6be},
	{0x6c0, 0x6ce}, {0x6d0, 0x6d3}, {0x6d5, 0x6d5}, {0x6e5, 0x6e6},
	{0x905, 0x939}, {0x93d, 0x93d}, {0x958, 0x961}, {0x985, 0x98c},
	{0x98f, 0x990}, {0x993, 0x9a8}, {0x9aa, 0x9b0}, {0x9b2, 0x9b2},
	{0x9b6, 0x9b9}, {0x9dc, 0x9dd}, {0x9df, 0x9e1}, {0x9f0, 0x9f1},
	{0xa05, 0xa0a}, {0xa0f, 0xa10}, {0xa13, 0xa28}, {0xa2a, 0xa30},
	{0xa32, 0xa33}, {0xa35, 0xa36}, {0xa38, 0xa39}, {0xa59, 0xa5c},
	{0xa5e, 0xa5e}, {0xa72, 0xa74}, {0xa85, 0xa8b}, {0xa8d, 0xa8d},
	{0xa8f, 0xa91}, {0xa93, 0xaa8}, {0xaaa, 0xab0}, {0xab2, 0xab3},
	{0xab5, 0xab9}, {0xabd, 0xabd}, {0xae0, 0xae0}, {0xb05, 0xb0c},
	{0xb0f, 0xb10}, {0xb13, 0xb28}, {0xb2a, 0xb30}, {0xb32, 0xb33},
	{0xb36, 0xb39}, {0xb3d, 0xb3d}, {0xb5c, 0xb5d}, {0xb5f, 0xb61},
	{0xb85, 0xb8a}, {0xb8e, 0xb90}, {0xb92, 0xb95}, {0xb99, 0xb9a},
	{0xb9c, 0xb9c}, {0xb9e, 0xb9f}, {0xba3, 0xba4}, {0xba8, 0xbaa},
	{0xbae, 0xbb5}, {0xbb7, 0xbb9}, {0xc05, 0xc0c}, {0xc0e, 0xc10},
	{0xc12, 0xc28}, {0xc2a, 0xc33}, {0xc35, 0xc39}, {0xc60, 0xc61},
	{0xc85, 0xc8c}, {0xc8e, 0xc90}, {0xc92, 0xca8}, {0xcaa, 0xcb3},
	{0xcb5, 0xcb9}, {0xcde, 0xcde}, {0xce0, 0xce1}, {0xd05, 0xd0c},
	{0xd0e, 0xd10}, {0xd12, 0xd28}, {0xd2a, 0xd39}, {0xd60, 0xd61},
	{0xe01, 0xe2e}, {0xe30, 0xe30}, {0xe32, 0xe33}, {0xe40, 0xe45},
	{0xe81, 0xe82}, {0xe84, 0xe84}, {0xe87, 0xe88}, {0xe8a, 0xe8a},
	{0xe8d, 0xe8d}, {0xe94, 0xe97}, {0xe99, 0xe9f}, {0xea1, 0xea3},
	{0xea5, 0xea5}, {0xea7, 0xea7}, {0xeaa, 0xeab}, {0xead, 0xeae},
	{0xeb0, 0xeb0}, {0xeb2, 0xeb3}, {0xebd, 0xebd}, {0xec0, 0xec4},
	{0xf40, 0xf47}, {0xf49, 0xf69}, {0x10a0, 0x10c5}, {0x10d0, 0x10f6},
	{0x1100, 0x1100}, {0x1102, 0x1103}, {0x1105, 0x1107}, {0x1109, 0x1109},
	{0x110b, 0x110c}, {0x110e, 0x1112}, {0x113c, 0x113c}, {0x113e, 0x113e},
	{0x1140, 0x1140}, {0x114c, 0x114c}, {0x114e, 0x114e}, {0x1150, 0x1150},
	{0x1154, 0x1155}, {0x1159, 0x1159}, {0x115f, 0x1161}, {0x1163, 0x1163},
	{0x1165, 0x1165}, {0x1167, 0x1167}, {0x1169, 0x1169}, {0x116d, 0x116e},
	{0x1172, 0x1173}, {0x1175, 0x1175}, {0x119e, 0x119e}, {0x11a8, 0x11a8},
	{0x11ab, 0x11ab}, {0x11ae, 0x11af}, {0x11b7, 0x11b8}, {0x11ba, 0x11ba},
	{0x11bc, 0x11c2}, {0x11eb, 0x11eb}, {0x11f0, 0x11f0}, {0x11f9, 0x11f9},
	{0x1e00, 0x1e9b}, {0x1ea0, 0x1ef9}, {0x1f00, 0x1f15}, {0x1f18, 0x1f1d},
	{0x1f20, 0x1f45}, {0x1f48, 0x1f4d}, {0x1f50, 0x1f57}, {0x1f59, 0x1f59},
	{0x1f5b, 0x1f5b}, {0x1f5d, 0x1f5d}, {0x1f5f, 0x1f7d}, {0x1f80, 0x1fb4},
	{0x1fb6, 0x1fbc}, {0x1fbe, 0x1fbe}, {0x1fc2, 0x1fc4}, {0x1fc6, 0x1fcc},
	{0x1fd0, 0x1fd3}, {0x1fd6, 0x1fdb}, {0x1fe0, 0x1fec}, {0x1ff2, 0x1ff4},
	{0x1ff6, 0x1ffc}, {0x2126, 0x2126}, {0x212a, 0x212b}, {0x212e, 0x212e},
	{0x2180, 0x2182}, {0x3041, 0x3094}, {0x30a1, 0x30fa}, {0x3105, 0x312c},
	{0xac00, 0xd7a3}
};

const struct xml_char_group base_char_group = {
		sizeof(base_char_range) / sizeof(base_char_range[0]),
				base_char_range};

static const struct xml_char_range char_range[] = { {0x100, 0xd7ff},
	{0xe000, 0xfffd}, {0x10000, 0x10ffff} 
};

const struct xml_char_group char_group = { 
		sizeof(char_range) / sizeof(char_range[0]), char_range};

static const struct xml_char_range combining_char_range[] = { {0x300, 0x345},
	{0x360, 0x361}, {0x483, 0x486}, {0x591, 0x5a1}, {0x5a3, 0x5b9},
	{0x5bb, 0x5bd}, {0x5bf, 0x5bf}, {0x5c1, 0x5c2}, {0x5c4, 0x5c4},
	{0x64b, 0x652}, {0x670, 0x670}, {0x6d6, 0x6dc}, {0x6dd, 0x6df},
	{0x6e0, 0x6e4}, {0x6e7, 0x6e8}, {0x6ea, 0x6ed}, {0x901, 0x903},
	{0x93c, 0x93c}, {0x93e, 0x94c}, {0x94d, 0x94d}, {0x951, 0x954},
	{0x962, 0x963}, {0x981, 0x983}, {0x9bc, 0x9bc}, {0x9be, 0x9be},
	{0x9bf, 0x9bf}, {0x9c0, 0x9c4}, {0x9c7, 0x9c8}, {0x9cb, 0x9cd},
	{0x9d7, 0x9d7}, {0x9e2, 0x9e3}, {0xa02, 0xa02}, {0xa3c, 0xa3c},
	{0xa3e, 0xa3e}, {0xa3f, 0xa3f}, {0xa40, 0xa42}, {0xa47, 0xa48},
	{0xa4b, 0xa4d}, {0xa70, 0xa71}, {0xa81, 0xa83}, {0xabc, 0xabc},
	{0xabe, 0xac5}, {0xac7, 0xac9}, {0xacb, 0xacd}, {0xb01, 0xb03},
	{0xb3c, 0xb3c}, {0xb3e, 0xb43}, {0xb47, 0xb48}, {0xb4b, 0xb4d},
	{0xb56, 0xb57}, {0xb82, 0xb83}, {0xbbe, 0xbc2}, {0xbc6, 0xbc8},
	{0xbca, 0xbcd}, {0xbd7, 0xbd7}, {0xc01, 0xc03}, {0xc3e, 0xc44},
	{0xc46, 0xc48}, {0xc4a, 0xc4d}, {0xc55, 0xc56}, {0xc82, 0xc83},
	{0xcbe, 0xcc4}, {0xcc6, 0xcc8}, {0xcca, 0xccd}, {0xcd5, 0xcd6},
	{0xd02, 0xd03}, {0xd3e, 0xd43}, {0xd46, 0xd48}, {0xd4a, 0xd4d},
	{0xd57, 0xd57}, {0xe31, 0xe31}, {0xe34, 0xe3a}, {0xe47, 0xe4e},
	{0xeb1, 0xeb1}, {0xeb4, 0xeb9}, {0xebb, 0xebc}, {0xec8, 0xecd},
	{0xf18, 0xf19}, {0xf35, 0xf35}, {0xf37, 0xf37}, {0xf39, 0xf39},
	{0xf3e, 0xf3e}, {0xf3f, 0xf3f}, {0xf71, 0xf84}, {0xf86, 0xf8b},
	{0xf90, 0xf95}, {0xf97, 0xf97}, {0xf99, 0xfad}, {0xfb1, 0xfb7},
	{0xfb9, 0xfb9}, {0x20d0, 0x20dc}, {0x20e1, 0x20e1}, {0x302a, 0x302f},
	{0x3099, 0x3099}, {0x309a, 0x309a}
};

const struct xml_char_group combining_char_group = {
		sizeof(combining_char_range) / sizeof(combining_char_range[0]), 
		combining_char_range };

static const struct xml_char_range digit_char_range[] = { {0x30, 0x39}, 
	{0x660, 0x669}, {0x6f0, 0x6f9}, {0x966, 0x96f}, {0x9e6, 0x9ef}, 
	{0xa66, 0xa6f}, {0xae6, 0xaef}, {0xb66, 0xb6f}, {0xbe7, 0xbef}, 
	{0xc66, 0xc6f}, {0xce6, 0xcef}, {0xd66, 0xd6f}, {0xe50, 0xe59}, 
	{0xed0, 0xed9}, {0xf20, 0xf29}
};

const struct xml_char_group digit_char_group = {
		sizeof(digit_char_range) / sizeof(digit_char_range[0]), 
		digit_char_range };

static const struct xml_char_range extender_range[] = { {0xb7, 0xb7}, 
	{0x2d0, 0x2d0}, {0x2d1, 0x2d1}, {0x387, 0x387}, {0x640, 0x640}, 
	{0xe46, 0xe46}, {0xec6, 0xec6}, {0x3005, 0x3005}, {0x3031, 0x3035}, 
	{0x309d, 0x309e}, {0x30fc, 0x30fe}
};

const struct xml_char_group extender_group = {
		sizeof(extender_range) / sizeof(extender_range[0]),
				extender_range };

static const struct xml_char_range ideographic_range[] = { {0x3007, 0x3007},
	{0x3021, 0x3029}, {0x4e00, 0x9fa5}
};

const struct xml_char_group ideographic_group = {
		sizeof(ideographic_range) / sizeof(ideographic_range[0]), 
		ideographic_range };

/* The binary search helper function */
static bool binary_search(unsigned int ch, int left, int right, 
		const struct xml_char_range *range);

/* Search for ch in range[left, right] */
bool binary_search(unsigned int ch, int left, int right, 
		const struct xml_char_range *range)
{
	int mid;

	if (left > right)
		return false;

	mid = (left + right) / 2;
	if (ch >= range[mid].start && ch <= range[mid].end)
		return true;

	if (ch < range[mid].start)
		return binary_search(ch, left, mid - 1, range);

	if (ch > range[mid].end)
		return binary_search(ch, mid + 1, right, range);

	return false;
}

/**
 * Test whether certain character beint32_ts to some XML character group
 *
 * \param ch     The character being tested
 * \param group  The character group
 * \return true if the character beint32_ts to the group, false otherwise.
 *
 * Generally, we use an algorithm like binary search to find the desired 
 * character in the group. The time complexity is about lg(n) and here n is
 * at most 180, so, I think the algorithm is fast enough for name validation.
 */
bool _dom_is_character_in_group(unsigned int ch,
		const struct xml_char_group *group)
{
	int len = group->len;
	const struct xml_char_range *range = group->range;

	if (ch < range[0].start || ch > range[len-1].end)
		return false;

	return binary_search(ch, 0, len - 1, range);
}

#ifdef CHVALID_DEBUG
/* The following is the testcases for this file. 
 * Compile this file :
 *
 * gcc -o test -DCHVALID_DEBUG character_valid.c
 *
 */

int main(int argc, char **argv)
{
	unsigned int ch = 0x666;

	assert(is_digit(ch) == true);
	assert(is_base_char(ch) == false);
	assert(is_char(ch) == true);
	assert(is_extender(ch) == false);
	assert(is_combining_char(ch) == false);
	assert(is_ideographic(ch) == false);

	ch = 0xf40;

	assert(is_digit(ch) == false);
	assert(is_base_char(ch) == true);
	assert(is_char(ch) == true);
	assert(is_extender(ch) == false);
	assert(is_combining_char(ch) == false);
	assert(is_ideographic(ch) == false);

	printf("The test pass.\n");
	return 0;
}

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2006 Rob Kendrick <rjek@rjek.com>
 * Copyright 2006 Richard Wilson <info@tinct.net>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/* The hash table entry */
struct _dom_hash_entry {
	void *key;			/**< The key pointer */
	void *value;			/**< The value pointer */
	struct _dom_hash_entry *next;	/**< Next entry */
};

/* The hash table */
struct dom_hash_table {
	const dom_hash_vtable *vtable;	/**< Vtable */
	void *pw;			/**< Client data */
	unsigned int nchains;		/**< Number of chains */
	struct _dom_hash_entry **chain;	/**< The chain head */
	uint32_t nentries;		/**< The entries in this table */
};


/**
 * Create a new hash table, and return a context for it.  The memory consumption
 * of a hash table is approximately 8 + (nchains * 12) bytes if it is empty.
 *
 * \param chains  Number of chains/buckets this hash table will have.  This
 *                should be a prime number, and ideally a prime number just
 *                over a power of two, for best performance and distribution
 * \param vtable  Client vtable
 * \param pw      Client private data
 * \return struct dom_hash_table containing the context of this hash table or
 *         NULL if there is insufficent memory to create it and its chains.
 */
dom_hash_table *_dom_hash_create(unsigned int chains, 
		const dom_hash_vtable *vtable, void *pw)
{
	dom_hash_table *r = (dom_hash_table *)malloc(sizeof(struct dom_hash_table));
	if (r == NULL) {
		return NULL;
	}

	r->vtable = vtable;
	r->pw = pw;
	r->nentries = 0;
	r->nchains = chains;
	r->chain = (_dom_hash_entry **)calloc(chains, sizeof(struct _dom_hash_entry *));
	if (r->chain == NULL) {
		free(r);
		return NULL;
	}

	return r;
}

/**
 * Clone a hash table.
 *
 * \param ht        Hash table to clone.
 *
 * \return The cloned hash table.
 */
dom_hash_table *_dom_hash_clone(dom_hash_table *ht)
{
	void *key = NULL, *nkey = NULL;
	void *value = NULL, *nvalue = NULL;
	uintptr_t c1, *c2 = NULL;
	struct dom_hash_table *ret;
	
	ret = _dom_hash_create(ht->nchains, ht->vtable, ht->pw);
	if (ret == NULL)
		return NULL;

	while ( (key = _dom_hash_iterate(ht, &c1, &c2)) != NULL) {
		nkey = ht->vtable->clone_key(key, ht->pw);
		if (nkey == NULL) {
			_dom_hash_destroy(ret);
			return NULL;
		}

		value = _dom_hash_get(ht, key);
		nvalue = ht->vtable->clone_value(value, ht->pw);
		if (nvalue == NULL) {
			ht->vtable->destroy_key(nkey, ht->pw);
			_dom_hash_destroy(ret);
			return NULL;
		}

		if (_dom_hash_add(ret, nkey, nvalue, false) == false) {
			_dom_hash_destroy(ret);
			return NULL;
		}
	}

	return ret;
}

/**
 * Destroys a hash table, freeing all memory associated with it.
 *
 * \param ht        Hash table to destroy. After the function returns, this
 *                  will nolonger be valid
 */
void _dom_hash_destroy(dom_hash_table *ht) 
{
	unsigned int i;

	if (ht == NULL)
		return;

	for (i = 0; i < ht->nchains; i++) {
		if (ht->chain[i] != NULL) {
			struct _dom_hash_entry *e = ht->chain[i];
			while (e) {
				struct _dom_hash_entry *n = e->next;
				ht->vtable->destroy_key(e->key, ht->pw);
				ht->vtable->destroy_value(e->value, ht->pw);
				free(e);
				e = n;
			}
		}
	}

	free(ht->chain);
	free(ht);
}

/**
 * Adds a key/value pair to a hash table
 *
 * \param  ht     The hash table context to add the key/value pair to.
 * \param  key    The key to associate the value with.
 * \param  value  The value to associate the key with.
 * \return true if the add succeeded, false otherwise.  (Failure most likely
 *         indicates insufficent memory to make copies of the key and value.
 */
bool _dom_hash_add(dom_hash_table *ht, void *key, void *value, 
		bool replace)
{
	unsigned int h, c;
	struct _dom_hash_entry *e;

	if (ht == NULL || key == NULL || value == NULL)
		return false;

	h = ht->vtable->hash(key, ht->pw);
	c = h % ht->nchains;

	for (e = ht->chain[c]; e; e = e->next) {
		if (ht->vtable->key_isequal(key, e->key, ht->pw)) {
			if (replace == true) {
				e->value = value;
				return true;
			} else {
				return false;
			}
		}
	}

	e = (struct _dom_hash_entry *)malloc(sizeof(struct _dom_hash_entry));
	if (e == NULL) {
		return false;
	}

	e->key = key;
	e->value = value;

	e->next = ht->chain[c];
	ht->chain[c] = e;
	ht->nentries++;

	return true;
}

/**
 * Looks up a the value associated with with a key from a specific hash table.
 *
 * \param  ht   The hash table context to look up
 * \param  key  The key to search for
 * \return The value associated with the key, or NULL if it was not found.
 */
void *_dom_hash_get(struct dom_hash_table *ht, void *key)
{
	unsigned int h, c;
	struct _dom_hash_entry *e;

	if (ht == NULL || key == NULL)
		return NULL;

	h = ht->vtable->hash(key, ht->pw);
	c = h % ht->nchains;

	for (e = ht->chain[c]; e; e = e->next) {
		if (ht->vtable->key_isequal(key, e->key, ht->pw))
			return e->value;
	}

	return NULL;
}

/**
 * Delete the key from the hashtable.
 *
 * \param ht   The hashtable object
 * \param key  The key to delete
 * \return The deleted value
 */
void *_dom_hash_del(struct dom_hash_table *ht, void *key)
{
	unsigned int h, c;
	struct _dom_hash_entry *e, *p;
	void *ret;

	if (ht == NULL || key == NULL)
		return NULL;

	h = ht->vtable->hash(key, ht->pw);
	c = h % ht->nchains;

	p = ht->chain[c];
	for (e = p; e; p = e, e = e->next) {
		if (ht->vtable->key_isequal(key, e->key, ht->pw)) {
			if (p != e) {
				p->next = e->next;
			} else {
				/* The first item in this chain is target*/
				ht->chain[c] = e->next;
			}

			ret = e->value;
			free(e);
			ht->nentries--;
			return ret;
		}
	}
	
	return NULL;
}

/**
 * Iterate through all available hash keys.
 *
 * \param  ht  The hash table context to iterate.
 * \param  c1  Pointer to first context
 * \param  c2  Pointer to second context (set to 0 on first call)
 * \return The next hash key, or NULL for no more keys
 */
void *_dom_hash_iterate(struct dom_hash_table *ht, uintptr_t *c1,
		uintptr_t **c2)
{
	struct _dom_hash_entry **he = (struct _dom_hash_entry **) c2;

	if (ht == NULL)
		return NULL;

	if (!*he)
		*c1 = -1;
	else
		*he = (*he)->next;

	if (*he)
		return (*he)->key;

	while (!*he) {
		(*c1)++;
		if (*c1 >= ht->nchains)
			return NULL;
		*he = ht->chain[*c1];
	}
	return (*he)->key;
}

/**
 * Get the number of elements in this hash table 
 *
 * \param ht  The hash table
 * 
 * \return the number of elements
 */
uint32_t _dom_hash_get_length(struct dom_hash_table *ht)
{
	return ht->nentries;
}

/*-----------------------------------------------------------------------*/

/* A simple test rig.  To compile, use:
 * gcc -g  -o hashtest -I../ -I../../include  -DTEST_RIG  hashtable.c
 *
 * If you make changes to this hash table implementation, please rerun this
 * test, and if possible, through valgrind to make sure there are no memory
 * leaks or invalid memory accesses.  If you add new functionality, please
 * include a test for it that has good coverage along side the other tests.
 */

#ifdef TEST_RIG


/**
 * Hash a pointer, returning a 32bit value.  
 *
 * \param  ptr  The pointer to hash.
 * \return the calculated hash value for the pointer.
 */

static inline unsigned int _dom_hash_pointer_fnv(void *ptr)
{
	return (unsigned int) ptr;
}

static void *test_alloc(void *p, size_t size, void *ptr)
{
	if (p != NULL) {
		free(p);
		return NULL;
	}

	if (p == NULL) {
		return malloc(size);
	}
}

int main(int argc, char *argv[])
{
	struct dom_hash_table *a, *b;
	FILE *dict;
	char keybuf[BUFSIZ], valbuf[BUFSIZ];
	int i;
	char *cow="cow", *moo="moo", *pig="pig", *oink="oink",
			*chicken="chikcken", *cluck="cluck",
			*dog="dog", *woof="woof", *cat="cat", 
			*meow="meow";
	void *ret;

	a = _dom_hash_create(79, _dom_hash_pointer_fnv, test_alloc, NULL);
	assert(a != NULL);

	b = _dom_hash_create(103, _dom_hash_pointer_fnv, test_alloc, NULL);
	assert(b != NULL);

	_dom_hash_add(a, cow, moo ,true);
	_dom_hash_add(b, moo, cow ,true);

	_dom_hash_add(a, pig, oink ,true);
	_dom_hash_add(b, oink, pig ,true);

	_dom_hash_add(a, chicken, cluck ,true);
	_dom_hash_add(b, cluck, chicken ,true);

	_dom_hash_add(a, dog, woof ,true);
	_dom_hash_add(b, woof, dog ,true);

	_dom_hash_add(a, cat, meow ,true);
	_dom_hash_add(b, meow, cat ,true);

#define MATCH(x,y) assert(!strcmp((char *)hash_get(a, x), (char *)y)); \
		assert(!strcmp((char *)hash_get(b, y), (char *)x))
	MATCH(cow, moo);
	MATCH(pig, oink);
	MATCH(chicken, cluck);
	MATCH(dog, woof);
	MATCH(cat, meow);

	assert(hash_get_length(a) == 5);
	assert(hash_get_length(b) == 5);

	_dom_hash_del(a, cat);
	_dom_hash_del(b, meow);
	assert(hash_get(a, cat) == NULL);
	assert(hash_get(b, meow) == NULL);

	assert(hash_get_length(a) == 4);
	assert(hash_get_length(b) == 4);

	_dom_hash_destroy(a, NULL, NULL);
	_dom_hash_destroy(b, NULL, NULL);

	/* This test requires /usr/share/dict/words - a large list of English
	 * words.  We load the entire file - odd lines are used as keys, and
	 * even lines are used as the values for the previous line.  we then
	 * work through it again making sure everything matches.
	 *
	 * We do this twice - once in a hash table with many chains, and once
	 * with a hash table with fewer chains.
	 */

	a = _dom_hash_create(1031, _dom_hash_pointer_fnv, test_alloc, NULL);
	b = _dom_hash_create(7919, _dom_hash_pointer_fnv, test_alloc, NULL);

	dict = fopen("/usr/share/dict/words", "r");
	if (dict == NULL) {
		fprintf(stderr, "Unable to open /usr/share/dict/words - \
				extensive testing skipped.\n");
		exit(0);
	}

	while (!feof(dict)) {
		fscanf(dict, "%s", keybuf);
		fscanf(dict, "%s", valbuf);
		_dom_hash_add(a, keybuf, valbuf, true);
		_dom_hash_add(b, keybuf, valbuf, true);
	}

	for (i = 0; i < 5; i++) {
		fseek(dict, 0, SEEK_SET);

		while (!feof(dict)) {
			fscanf(dict, "%s", keybuf);
			fscanf(dict, "%s", valbuf);
			assert(strcmp(hash_get(a, keybuf), valbuf) == 0);
			assert(strcmp(hash_get(b, keybuf), valbuf) == 0);
		}
	}

	_dom_hash_destroy(a, NULL, NULL);
	_dom_hash_destroy(b, NULL, NULL);

	fclose(dict);

	return 0;
}

#endif
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/** XML prefix */
static dom_string *xml;
/** XMLNS prefix */
static dom_string *xmlns;

/* The namespace strings */
static const char *namespaces[DOM_NAMESPACE_COUNT] = {
	NULL,
	"http://www.w3.org/1999/xhtml",
	"http://www.w3.org/1998/Math/MathML",
	"http://www.w3.org/2000/svg",
	"http://www.w3.org/1999/xlink",
	"http://www.w3.org/XML/1998/namespace",
	"http://www.w3.org/2000/xmlns/"
};

dom_string *dom_namespaces[DOM_NAMESPACE_COUNT] = {
	NULL,
};

/**
 * Initialise the namespace component
 *
 * \return DOM_NO_ERR on success.
 */
static dom_exception _dom_namespace_initialise(void)
{
	int i;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "xml", SLEN("xml"), &xml);
	if (err != DOM_NO_ERR) {
		return err;
	}

	err = dom_string_create((const uint8_t *) "xmlns", SLEN("xmlns"), 
			&xmlns);
	if (err != DOM_NO_ERR) {
		dom_string_unref(xml);
		xml = NULL;

		return err;
	}

	for (i = 1; i < DOM_NAMESPACE_COUNT; i++) {
		err = dom_string_create(
				(const uint8_t *) namespaces[i],
				strlen(namespaces[i]), &dom_namespaces[i]);
		if (err != DOM_NO_ERR) {
			dom_string_unref(xmlns);
			xmlns = NULL;

			dom_string_unref(xml);
			xml = NULL;

			return err;
		}
	}

	return DOM_NO_ERR;
}

#ifdef FINALISE_NAMESPACE
/**
 * Finalise the namespace component
 *
 * \return DOM_NO_ERR on success.
 */
dom_exception _dom_namespace_finalise(void)
{
	int i;

	if (xmlns != NULL) {
		dom_string_unref(xmlns);
		xmlns = NULL;
	}

	if (xml != NULL) {
		dom_string_unref(xml);
		xml = NULL;
	}

	for (i = 1; i < DOM_NAMESPACE_COUNT; i++) {
		if (dom_namespaces[i] != NULL) {
			dom_string_unref(dom_namespaces[i]);
			dom_namespaces[i] = NULL;
		}
	}

	return DOM_NO_ERR;
}
#endif

/**
 * Ensure a QName is valid
 *
 * \param qname      The qname to validate
 * \param namespace  The namespace URI associated with the QName, or NULL
 * \return DOM_NO_ERR                if valid,
 *         DOM_INVALID_CHARACTER_ERR if ::qname contains an invalid character,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns".
 */
dom_exception _dom_namespace_validate_qname(dom_string *qname,
		dom_string *ns)
{
	uint32_t colon, len;

	if (xml == NULL) {
		dom_exception err = _dom_namespace_initialise();
		if (err != DOM_NO_ERR)
			return err;
	}

	if (qname == NULL) {
		if (ns != NULL)
			return DOM_NAMESPACE_ERR;
		if (ns == NULL)
			return DOM_NO_ERR;
	}

	if (_dom_validate_name(qname) == false)
		return DOM_NAMESPACE_ERR;

	len = dom_string_length(qname);

	/* Find colon */
	colon = dom_string_index(qname, ':');

	if (colon == (uint32_t) -1) {
		/* No prefix */
		/* If namespace URI is for xmlns, ensure qname == "xmlns" */
		if (ns != NULL &&
				dom_string_isequal(ns,
				dom_namespaces[DOM_NAMESPACE_XMLNS]) &&
				dom_string_isequal(qname, xmlns) == false) {
			return DOM_NAMESPACE_ERR;
		}

		/* If qname == "xmlns", ensure namespace URI is for xmlns */
		if (ns != NULL &&
				dom_string_isequal(qname, xmlns) &&
				dom_string_isequal(ns,
				dom_namespaces[DOM_NAMESPACE_XMLNS]) == false) {
			return DOM_NAMESPACE_ERR;
		}
	} else if (colon == 0) {
		/* Some name like ":name" */
		if (ns != NULL)
			return DOM_NAMESPACE_ERR;
	} else {
		/* Prefix */
		dom_string *prefix;
		dom_string *lname;
		dom_exception err;

		/* Ensure there is a namespace URI */
		if (ns == NULL) {
			return DOM_NAMESPACE_ERR;
		}

		err = dom_string_substr(qname, 0, colon, &prefix);
		if (err != DOM_NO_ERR) {
			return err;
		}

		err = dom_string_substr(qname, colon + 1, len, &lname);
		if (err != DOM_NO_ERR) {
			dom_string_unref(prefix);
			return err;
		}

		if ((_dom_validate_ncname(prefix) == false) ||
		    (_dom_validate_ncname(lname) == false)) {
			dom_string_unref(prefix);
			dom_string_unref(lname);
			return DOM_NAMESPACE_ERR;
		}
		dom_string_unref(lname);

		/* Test for invalid XML namespace */
		if (dom_string_isequal(prefix, xml) &&
				dom_string_isequal(ns,
				dom_namespaces[DOM_NAMESPACE_XML]) == false) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		/* Test for invalid xmlns namespace */
		if (dom_string_isequal(prefix, xmlns) &&
				dom_string_isequal(ns,
				dom_namespaces[DOM_NAMESPACE_XMLNS]) == false) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		/* Test for presence of xmlns namespace with non xmlns prefix */
		if (dom_string_isequal(ns,
				dom_namespaces[DOM_NAMESPACE_XMLNS]) &&
				dom_string_isequal(prefix, xmlns) == false) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		dom_string_unref(prefix);
	}

	return DOM_NO_ERR;
}

/**
 * Split a QName into a namespace prefix and localname string
 *
 * \param qname      The qname to split
 * \param prefix     Pointer to location to receive prefix
 * \param localname  Pointer to location to receive localname
 * \return DOM_NO_ERR on success.
 *
 * If there is no prefix present in ::qname, then ::prefix will be NULL.
 *
 * ::prefix and ::localname will be referenced. The caller should unreference
 * them once finished.
 */
dom_exception _dom_namespace_split_qname(dom_string *qname,
		dom_string **prefix, dom_string **localname)
{
	uint32_t colon;
	dom_exception err;

	if (xml == NULL) {
		err = _dom_namespace_initialise();
		if (err != DOM_NO_ERR)
			return err;
	}

	/* Find colon, if any */
	colon = dom_string_index(qname, ':');

	if (colon == (uint32_t) -1) {
		/* None found => no prefix */
		*prefix = NULL;
		*localname = dom_string_ref(qname);
	} else {
		/* Found one => prefix */
		err = dom_string_substr(qname, 0, colon, prefix);
		if (err != DOM_NO_ERR) {
			return err;
		}

		err = dom_string_substr(qname, colon + 1,
				dom_string_length(qname), localname);
		if (err != DOM_NO_ERR) {
			dom_string_unref(*prefix);
			*prefix = NULL;
			return err;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Get the XML prefix dom_string 
 *
 * \return the xml prefix dom_string.
 * 
 * Note: The client of this function may or may not call the dom_string_ref
 * on the returned dom_string, because this string will only be destroyed when
 * the dom_finalise is called. But if the client call dom_string_ref, it must
 * call dom_string_unref to maintain a correct ref count of the dom_string.
 */
dom_string *_dom_namespace_get_xml_prefix(void)
{
	if (xml == NULL) {
		if (_dom_namespace_initialise() != DOM_NO_ERR)
			return NULL;
	}

	return xml;
}

/**
 * Get the XMLNS prefix dom_string.
 *
 * \return the xmlns prefix dom_string
 * 
 * Note: The client of this function may or may not call the dom_string_ref
 * on the returned dom_string, because this string will only be destroyed when
 * the dom_finalise is called. But if the client call dom_string_ref, it must
 * call dom_string_unref to maintain a correct ref count of the dom_string.
 */
dom_string *_dom_namespace_get_xmlns_prefix(void)
{
	if (xml == NULL) {
		if (_dom_namespace_initialise() != DOM_NO_ERR)
			return NULL;
	}

	return xmlns;
}

/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/* An combination of various tests */
static bool is_first_char(uint32_t ch);
static bool is_name_char(uint32_t ch);

/* Test whether the character can be the first character of
 * a NCName. */
static bool is_first_char(uint32_t ch)
{
	/* Refer http://www.w3.org/TR/REC-xml/ for detail */
	if (((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z')) ||
		(ch == '_') || (ch == ':') ||
		((ch >= 0xC0) && (ch <= 0xD6)) ||
		((ch >= 0xD8) && (ch <= 0xF6)) ||
		((ch >= 0xF8) && (ch <= 0x2FF)) ||
		((ch >= 0x370) && (ch <= 0x37D)) ||
		((ch >= 0x37F) && (ch <= 0x1FFF)) ||
		((ch >= 0x200C) && (ch <= 0x200D)) ||
		((ch >= 0x2070) && (ch <= 0x218F)) ||
		((ch >= 0x2C00) && (ch <= 0x2FEF)) ||
		((ch >= 0x3001) && (ch <= 0xD7FF)) ||
		((ch >= 0xF900) && (ch <= 0xFDCF)) ||
		((ch >= 0xFDF0) && (ch <= 0xFFFD)) ||
		((ch >= 0x10000) && (ch <= 0xEFFFF)))
		return true;

	if (is_letter(ch) || ch == (uint32_t) '_' || ch == (uint32_t) ':') {
		return true;
	} 

	return false;
}

/* Test whether the character can be a part of a NCName */
static bool is_name_char(uint32_t ch)
{
	/* Refer http://www.w3.org/TR/REC-xml/ for detail */
	if (((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z')) ||
		((ch >= '0') && (ch <= '9')) || /* !start */
		(ch == '_') || (ch == ':') ||
		(ch == '-') || (ch == '.') || (ch == 0xB7) || /* !start */
		((ch >= 0xC0) && (ch <= 0xD6)) ||
		((ch >= 0xD8) && (ch <= 0xF6)) ||
		((ch >= 0xF8) && (ch <= 0x2FF)) ||
		((ch >= 0x300) && (ch <= 0x36F)) || /* !start */
		((ch >= 0x370) && (ch <= 0x37D)) ||
		((ch >= 0x37F) && (ch <= 0x1FFF)) ||
		((ch >= 0x200C) && (ch <= 0x200D)) ||
		((ch >= 0x203F) && (ch <= 0x2040)) || /* !start */
		((ch >= 0x2070) && (ch <= 0x218F)) ||
		((ch >= 0x2C00) && (ch <= 0x2FEF)) ||
		((ch >= 0x3001) && (ch <= 0xD7FF)) ||
		((ch >= 0xF900) && (ch <= 0xFDCF)) ||
		((ch >= 0xFDF0) && (ch <= 0xFFFD)) ||
		((ch >= 0x10000) && (ch <= 0xEFFFF)))
		return true;

	if (is_letter(ch) == true)
		return true;
	if (is_digit(ch) == true)
		return true;
	if (is_combining_char(ch) == true)
		return true;
	if (is_extender(ch) == true)
		return true;

	return false;
}

/**
 * Test whether the name is a valid one according XML 1.0 standard.
 * For the standard please refer:
 *
 * http://www.w3.org/TR/2004/REC-xml-20040204/
 *
 * \param name  The name need to be tested
 * \return true if ::name is valid, false otherwise.
 */
bool _dom_validate_name(dom_string *name)
{
	uint32_t ch;
	size_t clen, slen;
	parserutils_error err;
	const uint8_t *s;

	if (name == NULL)
		return false;

	slen = dom_string_length(name);
	if (slen == 0)
		return false;

	s = (const uint8_t *) dom_string_data(name);
	slen = dom_string_byte_length(name);
	
	err = parserutils_charset_utf8_to_ucs4(s, slen, &ch, &clen);
	if (err != PARSERUTILS_OK) {
		return false;
	}
	
	if (is_first_char(ch) == false)
		return false;
	
	s += clen;
	slen -= clen;
	
	while (slen > 0) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &ch, &clen);
		if (err != PARSERUTILS_OK) {
			return false;
		}

		if (is_name_char(ch) == false)
			return false;

		s += clen;
		slen -= clen;
	}

	return true;
}

/**
 * Validate whether the string is a legal NCName.
 * Refer http://www.w3.org/TR/REC-xml-names/ for detail.
 *
 * \param str  The name to validate
 * \return true if ::name is valid, false otherwise.
 */
bool _dom_validate_ncname(dom_string *name)
{
	uint32_t ch;
	size_t clen, slen;
	parserutils_error err;
	const uint8_t *s;

	if (name == NULL)
		return false;

	slen = dom_string_length(name);
	if (slen == 0)
		return false;

	s = (const uint8_t *) dom_string_data(name);
	slen = dom_string_byte_length(name);
	
	err = parserutils_charset_utf8_to_ucs4(s, slen, &ch, &clen);
	if (err != PARSERUTILS_OK) {
		return false;
	}
	
	if (is_letter(ch) == false && ch != (uint32_t) '_')
		return false;
	
	s += clen;
	slen -= clen;
	
	while (slen > 0) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &ch, &clen);
		if (err != PARSERUTILS_OK) {
			return false;
		}

		if (is_name_char(ch) == false)
			return false;

		if (ch == (uint32_t) ':')
			return false;

		s += clen;
		slen -= clen;
	}

	return true;
}
