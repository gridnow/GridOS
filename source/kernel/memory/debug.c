#include <ddk/debug.h>

#include <section.h>
#include <process.h>

#include "object.h"

/**
	@brief Get the type name of the section
*/
xstring ks_type_name(struct ko_section * ks)
{
	xstring type = "δ֪";

	switch (ks->type & KS_TYPE_MASK)
	{
	case KS_TYPE_EXE:
		type = "��ִ�ж�";
		break;
	case KS_TYPE_PRIVATE:
		type = "�����";
		break;
	case KS_TYPE_FILE:
		type = "�ļ���";
		break;
	case KS_TYPE_DEVICE:
		type = "�̼�/�̶���";
		break;
	case KS_TYPE_SHARE:
		type = "�����";
		break;
	case KS_TYPE_KERNEL:
		type = "�ں˹����";
		break;
	case KS_TYPE_STACK:
		type = "��ջ��";
		break;
	default:
		type = "δ֪";
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

	printk("�����ڴ沼��:\n");
	printk("����            ���� ��ʼ��ַ                       �ߴ�                 ����\n");


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