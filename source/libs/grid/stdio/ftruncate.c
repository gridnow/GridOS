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
	@brief �����ļ������С

	@param[in] int fd			Ŀ���ļ�������ļ�������
	@param[in] ssize_t length	��Ŀ���ļ����󳤶ȵ���Ϊlength

	@return	�ɹ�����0��ʧ����Ϊ-1;
*/
DLLEXPORT int ftruncate(int fd, off_t length)
{
	int ret = -1;
	
	return ret;
}