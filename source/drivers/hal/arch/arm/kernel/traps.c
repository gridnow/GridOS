
#include <linkage.h>
#include <compiler.h>

asmlinkage void __div0(void)
{
	printk("Division by zero in kernel.\n");
	while (1);
}
