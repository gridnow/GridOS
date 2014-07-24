/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   The File Subsystem
 *   Wuxin
 */

#ifndef _MSC_VER
#include <types.h>
#endif

#include <cache.h>
#include <vfs.h>
#include <node.h>

void *fss_map_prepare_dbd(struct fss_file *file, void *process, uoffset file_pos)
{
     ssize_t ret;
     struct dbd *which;
     void *db_addr = NULL;
     unsigned long block_id;
     struct dbmr *map;
    
     block_id	= file_pos / FSS_CACHE_DB_SIZE;
     which	= fss_dbd_get(file, block_id);
     if (unlikely(!which))
		 goto end;

     ret	= fss_dbd_make_valid(file, which);
     if(ret < 0) 
		 goto end;
     map	= km_valloc(sizeof(struct dbmr));
     if (map == NULL)
		 goto  end;
	
     map->process	= process;
     map->base		= 0;
     db_addr		= which->buffer/*base*/ + file_pos % FSS_CACHE_DB_SIZE;
     ke_spin_lock(&which->map_lock);
     list_add_tail(&map->list ,&which->map_list);
     ke_spin_unlock(&which->map_lock);
	
end:
	 if (which)
		fss_dbd_put(which);

	 return db_addr;
	
}

void fss_map_destroy(struct fss_file *file)
{
	
}

int fss_map_init()
{
	return 0;
}



