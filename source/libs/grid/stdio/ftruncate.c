/**
	The Grid Core Library
 */

/**
	Stream file
	ZhaoYu,Yaosihai
 */
#include "file.h"

#include "sys/file_req.h"


/**
	@brief 调整文件对象大小

	@param[in] int fd			目标文件对象的文件描述符
	@param[in] ssize_t length	将目标文件对象长度调整为length

	@return	成功返回0，失败则为-1;
*/
DLLEXPORT int ftruncate(int fd, off_t length)
{
	int ret = -1;
	
	return ret;
}