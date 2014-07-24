#include <ddk/debug.h>

#include <section.h>
#include <process.h>

#include "object.h"

/**
	@brief Get the type name of the section
*/
xstring ks_type_name(struct ko_section * ks)
{
	xstring type = "未知";

	switch (ks->type & KS_TYPE_MASK)
	{
	case KS_TYPE_EXE:
		type = "可执行段";
		break;
	case KS_TYPE_PRIVATE:
		type = "常规段";
		break;
	case KS_TYPE_FILE:
		type = "文件段";
		break;
	case KS_TYPE_DEVICE:
		type = "固件/固定段";
		break;
	case KS_TYPE_SHARE:
		type = "共享段";
		break;
	case KS_TYPE_KERNEL:
		type = "内核共享段";
		break;
	case KS_TYPE_STACK:
		type = "堆栈段";
		break;
	default:
		type = "未知";
		break;
	}

	return type;
}

void ks_show_by_process(struct ko_process *who)
{
	struct ko_process *p;	
	struct ko_section *ks;
	struct km_vm_node *node;
	struct list_head *tmp;

	printk("进程内存布局:\n");
	printk("名称            保护 起始地址                       尺寸                 类型\n");


		p = who;
		printk("%s\n", cl_object_get_name(p));
			
		list_for_each(tmp, &p->vm_list)
		{
			node = list_entry(tmp, struct km_vm_node, node);
			ks = (struct ko_section*)node;
			
			printk("                 %d", ks->prot);
			printk("    %x", ks->node.start);	
			printk("            %x", ks->node.size);
			printk("    %s", ks_type_name(ks));			
			
			/* Additional information */
			if ((ks->type & KS_TYPE_MASK) == KS_TYPE_EXE)
			{
				printk("(%s)", cl_object_get_name(ks->priv.exe.exe_object));
			}
			printk("\n");
		}		
}