
/**
	This API for pthread key create and delete.
*/

#include <pthread.h>
#include <stdlib.h>

static pthread_spinlock_t __pthread_key_lock;
typedef void *(*destructort_t)(void *);
static destructort_t *__destructort_arry = NULL;
static pthread_key_t __pthread_key_nums = 0;

static pthread_spinlock_t __pthread_specific_lock;
static void **__pthread_specific_arry = NULL;
static unsigned long __pthread_specific_nums = 0;

#define SPECIFIC_INVALID    ((void *)-1)
#define DESTRUCTORT_INVALID ((destructort_t)-1)

/**
	@brief
		add thread specific ptr to specific_arry
	@return
		0 for success
		others for errno.
*/
static int 
find_elem_index_and_add_to_arry(void *elem, void ***arry, 
											int *arry_nums, int *arry_index)
{
	int specific_index;
	int new_size;
	void **pthread_specific_p;
	
	/* first find a invalid index */
	for (specific_index = 0; (specific_index < (*arry_nums)) &&
			((*arry)[specific_index] != (void *)-1);
			specific_index++)
		;

	/* ok, find a valid index to store. */
	if (specific_index < (*arry_nums))
	{
		(*arry)[specific_index] = elem;
		if (arry_index)
			*arry_index = specific_index;
		return 0;
	}

	/* no space for store, must remalloc */
	if ((*arry_nums) == 0)
		new_size = 8;
	else
		new_size = (*arry_nums) << 1;

	pthread_specific_p = malloc(new_size * sizeof(*(*arry)));
	if (!pthread_specific_p)
		return -ENOMEM;

	(*arry_nums) = new_size;
	/* init */
	while (new_size)
	{
		new_size--;
		pthread_specific_p[new_size] = (void *)-1;
	}

	/* assigned for pthread_arry */
	if ((*arry))
	{
		memcpy(pthread_specific_p, (*arry), specific_index * sizeof(*(*arry)));
		free((*arry));
	}

	(*arry) = pthread_specific_p;
	(*arry)[specific_index] = elem;
	
	if (arry_index)
			*arry_index = specific_index;
	
	return 0;
}


/**
	@brief
		key[out] for store key
		destructor[in] for delete key value produce
	@return
		0 success. 
		others for errno.
*/
int pthread_key_create(pthread_key_t *key, void *(*destructor)(void *))
{
	int ret;
	
	//pthread_spin_lock(&__pthread_key_lock);
	ret = find_elem_index_and_add_to_arry(destructor, (void *)&__destructort_arry, 
											(int *)&__pthread_key_nums, (int *)key);
	//pthread_spin_unlock(&__pthread_key_lock);
	return ret;
}


/**
	@brief
		key[in] for delete key

	@return
		0 success. 
		others for errno.
*/
int pthread_key_delete(pthread_key_t key)
{
	int i = 0;
	
	/* key is valid ? */
	if (key > __pthread_key_nums ||
		__destructort_arry[key] == DESTRUCTORT_INVALID)
		return -EINVAL;

	//pthread_spin_lock(&__pthread_specific_lock);

	/* foreach pthread specific */
	for (; i < __pthread_specific_nums; i++)
	{
		/* removed from ihash */
		if (__pthread_specific_arry[i] != SPECIFIC_INVALID)
			hurd_ihash_remove(__pthread_specific_arry[i], key);
	}

	//pthread_spin_lock(&__pthread_key_lock);
	__destructort_arry[key] = (destructort_t)-1;
	//pthread_spin_unlock(&__pthread_key_lock);
	
	//pthread_spin_unlock(&__pthread_specific_lock);
	return 0;
}

/**
	@brief
		called for when pthread lib init.
*/
void pthread_locks_init(void)
{
	pthread_spin_lock(&__pthread_key_lock);
	pthread_spin_lock(&__pthread_specific_lock);
}

