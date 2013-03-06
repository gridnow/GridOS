/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Console
*/

#ifndef HAL_CONSOLE_H
#define HAL_CONSOLE_H

struct hal_console_ops
{
	int (*read)(char *buffer, int size);
	int (*write)(char *buffer, int size);
};

#endif