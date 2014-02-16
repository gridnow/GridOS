/**
	
	�����������
*/
#include "kb_unicode.h"

#include <errno.h>
#include <types.h>
#include <string.h>

#include <ddk/input.h>
#include <ddk/slab.h>


#define KEY_MAX			0x2ff
#define KEY_UNPRESS		1
#define IF_UP			0x80
#define IF_EXTEND		0xe0

#define EXT_CODE_1			1
#define EXT_CODE_2			2

#define BASE_KEY_MAX_NUM	111

#define KEY_UP				112
#define KEY_DOWN			113
#define KEY_LEFT			114
#define KEY_RIGHT			115





#define K(t,s,f) ((t)|((s)<<8)|((f)<<16)|(0)<<24)
//static struct ifi_device * default_dev;

/* �������е��ַ��� */
static unsigned int code_key[KEY_MAX] = {
	'\000',K('\033', 0, 0),'1','2','3','4','5','6','7','8','9','0','-','=','\177','\t',																				
	'q','w','e','r','t','y','u','i','o','p','[',']','\r','\000','a','s',
	'd','f','g','h','j','k','l',';','\'','`','\000','\\','z','x','c','v',
	'b','n','m',',','.','/','\000','*','\000',' ','\000','\201','\202','\203','\204','\205',
	'\206','\207','\210','\211','\212','\000','\000','7','8','9','-','4','5','6','+','1',
	'2','3','0','\177','\000','\000','\213','\214','\000','\000','\000','\000','\000','\000','\000','\000',
	'\r','\000','/',0,0,0,0,0,0,0,0,0,0,0,0,0,
	K('\033','[','A'),K('\033','[','B'),K('\033','[','D'),K('\033','[','C'),0,0,0,0,0,0,0,0,0,0,0,0
};


/* ��������ɨ����Ĳ��ұ� */
unsigned char double_key_table[KEY_MAX] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,											/* 0x00 - 0x0f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,											/* 0x10 - 0x1f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,											/* 0x20 - 0x2f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,											/* 0x30 - 0x3f */
	0,0,0,0,0,0,0,0,KEY_UP,0,0,KEY_LEFT,0,KEY_RIGHT,0,0,						/* 0x40 - 0x4f */
	KEY_DOWN,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,										/* 0x50 - 0x5f */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,											/* 0x60 - 0x6f */					
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

/* ��������ɨ����Ĳ��ұ� */
unsigned char single_key_table[BASE_KEY_MAX_NUM] = {
	0, KEY_ESC, KEY_1, KEY_2,
	KEY_3, KEY_4, KEY_5, KEY_6,
	KEY_7, KEY_8, KEY_9, KEY_0,
	KEY_SUBT, KEY_EQUAL, 14, KEY_TAB,
	KEY_Q, KEY_W, KEY_E, KEY_R,			
	KEY_T, KEY_Y, KEY_U, KEY_I,			
	KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE,
	KEY_SHIFT_ENTER, 29, KEY_A, KEY_S,			
	KEY_D, KEY_F, KEY_G, KEY_H,			
	KEY_J, KEY_K, KEY_L, KEY_SICOLON,		
	KEY_SINGLE_QUOTE, KEY_KOP, 42, KEY_RIGHT_SLASH,	
	KEY_Z, KEY_X, KEY_C, KEY_V,			
	KEY_B, KEY_N, KEY_M, KEY_COMMA,		
	KEY_POINT, KEY_LEFT_SLASH, 54, KEY_MULT,		
	56, KEY_SPACE, 58, 59,
	60, 61, 62, 63,
	64, 65, 66, 67,
	68, 69, 70, KEY_P_7,			
	KEY_P_8, KEY_P_9, KEY_p_SUBT, KEY_P_4,			
	KEY_P_5, KEY_P_6, KEY_P_PLUS, KEY_P_1,			
	KEY_P_2, KEY_P_3, KEY_P_0, 83,
	84, 85, 86, 87,
	88, 89, 90, 91,
	92, 93, 94, 95,
	96, 97,	KEY_DEV, 99,
	100, 101, 102, 103,
	104, 105, 106, 107,
	108, 109, 110		
};	

/**
	@brief �浽dev�Ļ�������

	@return
		The size in bytes we have stored
*/
static int store_code(struct ifi_device *dev, unsigned char *table_name, unsigned char scancode)
{
	unsigned char *code;
	int stored = 0;
	struct ifi_package *store_pkg = dev->ifi_buffer;

	/* Ҫת�����Ǵ������ */
	if (table_name)
	{
		int i, count;
		
		/* Split the code to buffer */
		code = (unsigned char*)&code_key[table_name[scancode]];
		for (count = 0, i = 0; ;i++, count++)
		{
			if (code[i] == 0)
				break;
		}

		/* �ܴ��𣿱��⸲�� */
		if (ke_atomic_read(&dev->free_count) < count)
			goto end;

		/* Store it */
		for (i = 0; i < count; i++)
		{
			store_pkg[dev->store_pos].code		= code[i];
			store_pkg[dev->store_pos++].flags	= 0;

			/* ��Ҫ�ع��������� */
			if (dev->store_pos == IFI_DEVICE_PKG_COUNT)
				dev->store_pos = 0;

			/* TODO һ���Լ��Ϳ��ԣ�����һ��һ�εļ�������û��������� */
			ke_atomic_dec(&dev->free_count);
			stored++;
		}

	}
	else
	{
		/* �ܴ��𣿱��⸲�� */
		if (ke_atomic_read(&dev->free_count) < 1)
			goto end;

		store_pkg[dev->store_pos].code		= 0;
		store_pkg[dev->store_pos++].flags	= 0;
		ke_atomic_dec(&dev->free_count);

		/* ��Ҫ�ع��������� */
		if (dev->store_pos == IFI_DEVICE_PKG_COUNT)
			dev->store_pos = 0;
		stored++;
	}

end:
	return stored;
}

/*
*���ϲ�ӿڵ��õ�kb���뺯��
*/
static int kb_read_input(struct ifi_device * dev, struct ifi_package * input, int len)
{
	struct ifi_package *store_pkg = NULL;
	/* No device ? */
	if (!dev)
		return 0;
	
	store_pkg = dev->ifi_buffer;
	/* �м���? */
	while (ke_atomic_read(&dev->free_count) == IFI_DEVICE_PKG_COUNT)
		ke_wait_for_completion(&dev->data_ready);
	//printk("dev fetch pos is %d\n", dev->fetch_pos);
	/* �У��ݶ�Ϊһ���ֽڵض� */
	input->code		= store_pkg[dev->fetch_pos].code;
	input->flags	= store_pkg[dev->fetch_pos].flags;
	dev->fetch_pos++;
	//for debug
	//printk("input code %d\n", input->code);
	//printk("\033 is %d\n", '\033');
	if (dev->fetch_pos == IFI_DEVICE_PKG_COUNT)
		dev->fetch_pos = 0;
	ke_atomic_inc(&dev->free_count);

	return 1;
}

/**
	@brief input the kbd/mouse/touch screen input stream

	@note
		����ͬһ�����󲻿�����		
*/
static int kb_input_stream(struct ifi_device * dev, void * buf, size_t size)
{	
	unsigned char scancode = 0;														//��������ɨ����
	int inpt_len = 0;						
 	if (!dev)
 		return -1;
		
	/*  ��ȡɨ����  */	
	scancode = *(unsigned char *)buf;
	//ke_input_debug(scancode);

	/* 
		�ж��Ƿ�Ϊ�๦��ť,һЩ�๦��ť����0xe0��ͷ��ɨ���� 
		Ŀǰ��ʽ�в�û�иù���,���Ե����������ҽ�������������
	*/
	if (scancode == IF_EXTEND) 
	{
		dev->ext_code = EXT_CODE_1;
		return inpt_len;
	}

	/* �����Ŀǰû�в������������ */
	if (scancode & IF_UP)
		inpt_len = store_code(dev, NULL, 0);
	else if (dev->ext_code == EXT_CODE_1 || ((scancode > 71)
				&& (scancode < 81)))//����ǵ�һɨ������0xE0��ͷ������double_key_table���ѯ���������ͨ�ַ���single_key_table��ѯ
	{
		inpt_len = store_code(dev, double_key_table, scancode);
		dev->ext_code = 0;
	}
	else
	{
		inpt_len = store_code(dev, single_key_table, scancode);
	}
	/* To the distribution layer */
	ke_complete(&dev->data_ready);
	
	return inpt_len;
}

static struct ifi_dev_ops kb_dev_ops = {
		.ifi_dev_read = kb_read_input,
	};




/*
*@function:����kb�豸
*/

int ifi_create_kb(void)
{
	struct ifi_device *ifi_dev = ifi_device_create();
	if (!ifi_dev)
	{
		return -ENOMEM;
	}
	
	ifi_dev->dev_ops          = &kb_dev_ops;
	ifi_dev->ifi_input_stream = kb_input_stream;
	ifi_dev->dev_type         = IFI_DEV_STD_IN;
	return 0;
}


