#include <compiler.h>

DLLEXPORT __weak int strcmp(const char *src1, const char *src2)
{
	int ret = 0 ;

	while(!(ret = *(unsigned char *)src1 - *(unsigned char *)src2) && *src1)
	{
		++src1, ++src2;
	}
                
	if (ret < 0)
	{
		ret = -1 ;
	}
	else if (ret > 0)
	{
		ret = 1 ;
	}
                
	return ret;
}
