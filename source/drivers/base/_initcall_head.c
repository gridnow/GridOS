#include <ddk/compiler.h>

/**
	������Ƕ��ʼ���б�ͷ����������ͨ�����ӽű������������鷳
*/
define_driver_initcall_head(initcall_head);

void start_embeded_driver()
{
	initcall_t *fn = (initcall_t*)&initcall_head;
	while (*(++fn))
	{
		(*fn)();
	} 
}
