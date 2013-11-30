/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   File Name ¥¶¿Ì
 */
#include <string.h>
#include "cl_fname.h"

const char *cl_locate_pure_file_name(const char *fname)
{
	char *pname;
	
	pname = strrchr(fname, '\\');
	if (pname)
		return pname + 1;
	pname = strrchr(fname, '/');
	if (pname)
		return pname +1;

	return fname;
}
