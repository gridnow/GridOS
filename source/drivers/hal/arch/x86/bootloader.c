/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   X86 Bootloader support
 */

#include <hal_config.h>

#define FAK_LOADER_DATA_STRUCTURE_ENTRY 0x10000
#define FAK_LOADER_MAX_FILE_TO_LOAD 32
#define FAK_LOADER_MAX_FILE_NAME_TO_LOAD 32

struct fak_loader_data
{
	unsigned int boot_from_drive;	//
	char loaded_file_name[FAK_LOADER_MAX_FILE_TO_LOAD][FAK_LOADER_MAX_FILE_NAME_TO_LOAD];
	unsigned int loaded_file_phy_address[FAK_LOADER_MAX_FILE_TO_LOAD]; //address of the load file
	unsigned int loaded_file_length[FAK_LOADER_MAX_FILE_TO_LOAD];	//length for the file,aligned in 16 bytes
	int loaded_file_count; //how many files actually being loaded
	unsigned int loaded_length_of_the_last_file;//used to calculate the end phy addresss
	char hd_mask[4]; //4 ide device ,1=HARDDISK,2=ATAPI,0=NULL;
	unsigned int boot_partition_id;
	unsigned long vesa_addr;
	short x_resolution,y_resolution,bits_per_pixel;
	//above data are from loader
};

#define GET_LOADER_DATA(P) struct fak_loader_data  * P = (void*)HAL_GET_BASIC_KADDRESS(FAK_LOADER_DATA_STRUCTURE_ENTRY);

void x86_bootloader_get_video_info(unsigned long * fb, int *x, int *y, int *bpp)
{
	GET_LOADER_DATA(p);
	if (fb)
		*fb = p->vesa_addr;
	if (x)
		*x = p->x_resolution;
	if (y)
		*y = p->y_resolution;
	if (bpp)
		*bpp = p->bits_per_pixel;
}
