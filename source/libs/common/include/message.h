/**
*  @defgroup api_msg
*  @ingroup common_libs
*
*  ��Ϣ���������ؼ���
*
*  @{
*/
#ifndef COMMON_LIBS_MSG_H
#define COMMON_LIBS_MSG_H

#include <ystd.h>
#include <list.h>

#define MSG_STATUS_NEW		(1 << 1)
#define MSG_STATUS_USED_BIT	(1 << 3)

#define MSG_FLAGS_ADDRESS	(1 << 4)
#define MSG_FLAGS_SYNC		(1 << 5)

#define __data_size__(DATA_COUNT) ((DATA_COUNT)  * sizeof(MSG_DATA_TYPE))
#define __add_msg_count__(DATA_COUNT) ((__data_size__(DATA_COUNT) + sizeof(struct y_message) - 1)/*max possible size*/ / sizeof(struct y_message))
#define MSG_MAKE(NAME, FLAGS, DATA_COUNT, WHAT) \
	struct y_message __msg##NAME__[__add_msg_count__(DATA_COUNT)/* Data part */ + 1/*The head of msg*/];	\
	struct y_message *(NAME) = &__msg##NAME__[0];	\
	(NAME)->count = (DATA_COUNT);	\
	(NAME)->flags = (FLAGS);	\
	(NAME)->what = (unsigned long)WHAT;
#define MSG_MAKE_OUT_PARA(MSG, WRITE_NAME) \
	unsigned long *WRITE_NAME = (unsigned long*) &(MSG)[1];	


/**
	@brief Data ops on received msg
*/
typedef unsigned long * MSG_PDATA_TYPE;
typedef unsigned long MSG_DATA_TYPE;
#define MSG_DATA_START(DESC, ORG, CUR) CUR = (MSG_PDATA_TYPE)ORG; \
	MSG_GET_NEXT_SLOT(DESC, CUR/*Head is a msg node*/);

#define MSG_DATA_READ_NEXT(DESC, CUR, DATA, TYPE) do { \
	DATA = *(TYPE*)CUR; \
	MSG_GET_NEXT_DATA(DESC, CUR); \
} while (0)

#define MSG_DATA_WRITE_NEXT(DESC, CUR, DATA, TYPE) do { \
	*(TYPE*)CUR = (DATA); \
	MSG_GET_NEXT_DATA(DESC, CUR); \
} while (0)

/**
	@brief Get next slot
*/
#define MSG_GET_NEXT_SLOT(DESC, CUR) do { \
	CUR = (void*)CUR + sizeof(struct y_message);/* each node is a slot */ \
	if ((unsigned long)((DESC)->slots) + ((DESC)->slot_buffer_size)/* Buffer limit */ <= (unsigned long)CUR) \
		CUR = (DESC)->slots; \
} while (0)

#define MSG_GET_NEXT_DATA(DESC, CUR) do { \
	CUR = (void*)CUR + sizeof(MSG_DATA_TYPE);/* each node is a DATA */ \
	if ((unsigned long)((DESC)->slots) + ((DESC)->slot_buffer_size)/* Buffer limit */ <= (unsigned long)CUR) \
		CUR = (DESC)->slots; \
} while (0)

struct y_message_instance;

typedef bool (*message_filter)(struct y_message *what);
typedef void (*message_sleep)(struct y_message_instance *msg_instance);
typedef void (*message_response_sync)(struct y_message *what);
typedef y_message_func (*message_find_handler)(struct y_message_instance *msg_instance, message_id_t id);

struct y_message_instance
{
	message_filter filter;
	message_sleep sleep;
	message_response_sync response_sync;
	message_find_handler find_handler;
	void * slots;
	void * current_slot;
	int slot_buffer_size;
};

/**
	@brief goto into message loop

	The function will not exist NOW. When fetch a message it tries to handle it.
*/
void message_loop(struct y_message_instance *instance);

/**
	@brief ��������Ϣ��״̬��ʼ��

	һ�����ڳ�ʼ���ոմ�������Ϣ������
*/
void message_reset_all(struct y_message_instance *instance);

#endif

/** @} */
