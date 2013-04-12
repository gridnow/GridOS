#include <ddk/compiler.h>

/**
	构造内嵌初始化列表头，本来可以通过链接脚本来做，但是麻烦
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
