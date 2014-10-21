/* ihash.c - Integer-keyed hash table functions.
    
   Written by Michael I. Bushnell.
   Revised by Miles Bader <miles@gnu.org>.
   Revised by Marcus Brinkmann <marcus@gnu.org>.
 */


#include <stdlib.h>
#include <errno.h>
#include <ihash.h>
#include <string.h>

#define assert(exp) 
/* Return 1 if the slot with the index IDX in the hash table HT is
   empty, and 0 otherwise.  */
static inline int
index_empty (hurd_ihash_t ht, unsigned int idx)
{
	return ht->items[idx].value == _HURD_IHASH_EMPTY
			|| ht->items[idx].value == _HURD_IHASH_DELETED;
}


/* Return 1 if the index IDX in the hash table HT is occupied by the
   element with the key KEY.  */
static inline int
index_valid (hurd_ihash_t ht, unsigned int idx, hurd_ihash_key_t key)
{
	return !index_empty (ht, idx) && ht->items[idx].key == key;
}


/* Given a hash table HT, and a key KEY, find the index in the table
   of that key.  You must subsequently check with index_valid() if the
   returned index is valid.  */
static inline int
find_index (hurd_ihash_t ht, hurd_ihash_key_t key)
{
	unsigned int idx;
	unsigned int up_idx;
	unsigned int mask = ht->size - 1;

	idx = key & mask;

	if (ht->items[idx].value == _HURD_IHASH_EMPTY || ht->items[idx].key == key)
		return idx;

	up_idx = idx;

	do
    {
		up_idx = (up_idx + 1) & mask;
		if (ht->items[up_idx].value == _HURD_IHASH_EMPTY
			|| ht->items[up_idx].key == key)
			return up_idx;
    }while (up_idx != idx);

	/* If we end up here, the item could not be found.  Return any
     invalid index.  */
	return idx;
}


/* Remove the entry pointed to by the location pointer LOCP from the
   hashtable HT.  LOCP is the location pointer of which the address
   was provided to hurd_ihash_add().  */
static inline void
locp_remove (hurd_ihash_t ht, hurd_ihash_locp_t locp)
{
	if (ht->cleanup)
		(*ht->cleanup) (*locp, ht->cleanup_data);
	*locp = _HURD_IHASH_DELETED;
	ht->nr_items--;
}


/* Construction and destruction of hash tables.  */

/* Initialize the hash table at address HT.  */
void
hurd_ihash_init (hurd_ihash_t ht, intptr_t locp_offs)
{
	ht->nr_items = 0;
	ht->size = 0;
	ht->locp_offset = locp_offs;
	ht->max_load = HURD_IHASH_MAX_LOAD_DEFAULT;
	ht->cleanup = 0;
}


/* Destroy the hash table at address HT.  This first removes all
   elements which are still in the hash table, and calling the cleanup
   function for them (if any).  */
void
hurd_ihash_destroy (hurd_ihash_t ht)
{
	if (ht->cleanup)
	{
		hurd_ihash_value_t value, *_hurd_ihash_valuep = NULL;
		hurd_ihash_cleanup_t cleanup = ht->cleanup;
		void *cleanup_data = ht->cleanup_data;

		HURD_IHASH_ITERATE (ht, value)
		(*cleanup) (value, cleanup_data);
    }

	if (ht->size > 0)
		free (ht->items);
}


/* Create a hash table, initialize it and return it in HT.  If a
   memory allocation error occurs, ENOMEM is returned, otherwise 0.  */
int
hurd_ihash_create (hurd_ihash_t *ht, intptr_t locp_offs)
{
	*ht = malloc (sizeof (struct hurd_ihash));
	if (*ht == NULL)
		return ENOMEM;

	hurd_ihash_init (*ht, locp_offs);

	return 0;
}


/* Destroy the hash table HT and release the memory allocated for it
   by hurd_ihash_create().  */
void
hurd_ihash_free (hurd_ihash_t ht)
{
	hurd_ihash_destroy (ht);
	free (ht);
}


/* Set the cleanup function for the hash table HT to CLEANUP.  The
   second argument to CLEANUP will be CLEANUP_DATA on every
   invocation.  */
void
hurd_ihash_set_cleanup (hurd_ihash_t ht, hurd_ihash_cleanup_t cleanup,
			void *cleanup_data)
{
	ht->cleanup = cleanup;
	ht->cleanup_data = cleanup_data;
}


/* Set the maximum load factor in binary percent to MAX_LOAD, which
   should be between 64 and 128.  The default is
   HURD_IHASH_MAX_LOAD_DEFAULT.  New elements are only added to the
   hash table while the number of hashed elements is that much binary
   percent of the total size of the hash table.  If more elements are
   added, the hash table is first expanded and reorganized.  A
   MAX_LOAD of 128 will always fill the whole table before enlarging
   it, but note that this will increase the cost of operations
   significantly when the table is almost full.

   If the value is set to a smaller value than the current load
   factor, the next reorganization will happen when a new item is
   added to the hash table.  */
void
hurd_ihash_set_max_load (hurd_ihash_t ht, unsigned int max_load)
{
	ht->max_load = max_load;
}


/* Helper function for hurd_ihash_add.  Return 1 if the item was
   added, and 0 if it could not be added because no empty slot was
   found.  The arguments are identical to hurd_ihash_add.

   We are using open address hashing.  As the hash function we use the
   division method with linear probe.  */
static inline int
add_one (hurd_ihash_t ht, hurd_ihash_key_t key, hurd_ihash_value_t value)
{
	unsigned int idx;
	unsigned int first_free;
	unsigned int mask = ht->size - 1;

	idx = key & mask;
	first_free = idx;

	if (ht->items[idx].value != _HURD_IHASH_EMPTY && ht->items[idx].key != key)
	{
		unsigned int up_idx = idx;

		do
		{
			up_idx = (up_idx + 1) & mask;
			if (ht->items[up_idx].value == _HURD_IHASH_EMPTY
				|| ht->items[up_idx].key == key)
	    	{
				idx = up_idx;
				break;
	    	}
		}while (up_idx != idx);
    }

	/* Remove the old entry for this key if necessary.  */
	if (index_valid (ht, idx, key))
		locp_remove (ht, &ht->items[idx].value);

	/* If we have not found an empty slot, maybe the last one we
     looked at was empty (or just got deleted).  */
	if (!index_empty (ht, first_free))
		first_free = idx;
 
	if (index_empty (ht, first_free))
	{
		ht->nr_items++;
		ht->items[first_free].value = value;
		ht->items[first_free].key = key;

		if (ht->locp_offset != HURD_IHASH_NO_LOCP)
			*((hurd_ihash_locp_t *) (((char *) value) + ht->locp_offset))
	  		= &ht->items[first_free].value;

      	return 1;
    }

  	return 0;
}

  
/* Add ITEM to the hash table HT under the key KEY.  If there already
   is an item under this key, call the cleanup function (if any) for
   it before overriding the value.  If a memory allocation error
   occurs, ENOMEM is returned, otherwise 0.  */
int
hurd_ihash_add (hurd_ihash_t ht, hurd_ihash_key_t key, hurd_ihash_value_t item)
{
	struct hurd_ihash old_ht = *ht;
	int was_added;
	unsigned int i;
	int r;

	if (ht->size)
	{
      /* Only fill the hash table up to its maximum load factor.  */
      if (hurd_ihash_get_load (ht) <= ht->max_load)
		if (add_one (ht, key, item))
	  		return 0;
    }

	/* The hash table is too small, and we have to increase it.  */
	ht->nr_items = 0;
	if (ht->size == 0)
      ht->size = HURD_IHASH_MIN_SIZE;
	else
      ht->size <<= 1;

	/* calloc() will initialize all values to _HURD_IHASH_EMPTY implicitly.  */
	ht->items = malloc ((ht->size) * sizeof (struct _hurd_ihash_item));

	if (ht->items == NULL)
    {
		*ht = old_ht;
		return ENOMEM;
    }
	memset(ht->items, 0, ht->size * sizeof(struct _hurd_ihash_item));
	/* We have to rehash the old entries.  */
	for (i = 0; i < old_ht.size; i++)
		if (!index_empty (&old_ht, i))
		{
			was_added = add_one (ht, old_ht.items[i].key, old_ht.items[i].value);
			/* It will not happen or our code is so stupied */
			if (!was_added)
				goto err_and_free_itmes_buffer;
		}

	/* Finally add the new element!  */
	was_added = add_one (ht, key, item);
	if (!was_added)
		goto err_and_free_itmes_buffer;
	r = 0;
end:
	if (old_ht.size > 0)
		free (old_ht.items);

	return r;

err_and_free_itmes_buffer:
	free(ht->items);
	ht->items = NULL;
	r = ENOMEM;
	goto end;
}


/* Find and return the item in the hash table HT with key KEY, or NULL
   if it doesn't exist.  */
hurd_ihash_value_t
hurd_ihash_find (hurd_ihash_t ht, hurd_ihash_key_t key)
{
	if (ht->size == 0)
		return NULL;
	else
    {
		int idx = find_index (ht, key);
		return index_valid (ht, idx, key) ? ht->items[idx].value : NULL;
    }
}


/* Remove the entry with the key KEY from the hash table HT.  If such
   an entry was found and removed, 1 is returned, otherwise 0.  */
int
hurd_ihash_remove (hurd_ihash_t ht, hurd_ihash_key_t key)
{
	if (ht->size != 0)
    {
		int idx = find_index (ht, key);
      
		if (index_valid (ht, idx, key))
		{
			locp_remove (ht, &ht->items[idx].value);
			return 1;
		}
	}

	return 0;
}


/* Remove the entry pointed to by the location pointer LOCP from the
   hashtable HT.  LOCP is the location pointer of which the address
   was provided to hurd_ihash_add().  This call is faster than
   hurd_ihash_remove().  */
void
hurd_ihash_locp_remove (hurd_ihash_t ht, hurd_ihash_locp_t locp)
{
	locp_remove (ht, locp);
}
