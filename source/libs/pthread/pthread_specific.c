
#include <pthread.h>
#include <ystd.h>
#include "ihash.h"


/**
	@brief: This function set value to thread_specific data by the key.
*/
int pthread_setspecific (pthread_key_t key, const void *value)
{
	hurd_ihash_t speci_tb = (hurd_ihash_t)get_current_pt_specific();

	/* key is valid ? */
	if (key > __pthread_key_nums ||
		__destructort_arry[key] == DESTRUCTORT_INVALID)
		return -EINVAL;
	
	/* has created? */
	if (!speci_tb)
	{
		int ret;
		
		/* if failt, it must out of memory */
		if (hurd_ihash_create(&speci_tb, HURD_IHASH_NO_LOCP) != 0)
			return -ENOMEM;
		
		/* add speci_tb to speci_tables */
		pthread_spin_lock(&__pthread_specific_lock);
		ret = find_elem_index_and_add_to_arry(speci_tb, &__pthread_specific_arry, (int *)&__pthread_specific_nums, NULL);
		pthread_spin_unlock(&__pthread_specific_lock);

		/* errno is always no mem */
		if (ret < 0)
		{
			hurd_ihash_destroy(speci_tb);
			return -ENOMEM;
		}
		
		/* set to pthread */
		set_current_pt_specific((void *)speci_tb);
	}
	
	/* add to ihash tables */
	if (hurd_ihash_add(speci_tb, (hurd_ihash_key_t)key, (hurd_ihash_value_t)value) != 0)
		return -ENOMEM;

	return 0;
}


/**
	@brief get thread specific value base key
*/
void *pthread_getspecific(pthread_key_t key)
{
	hurd_ihash_t speci_tb = (hurd_ihash_t)get_current_pt_specific();

	/* key is valid ? */
	if (key > __pthread_key_nums ||
		__destructort_arry[key] == DESTRUCTORT_INVALID)
		return NULL;
	
	/* not exist, immedily exit */
	if (!speci_tb)
		return NULL;

	/* find value */
	return hurd_ihash_find(speci_tb, (hurd_ihash_key_t)key);
}





