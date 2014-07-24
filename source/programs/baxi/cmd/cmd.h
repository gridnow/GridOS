#ifndef CMD_H
#define CMD_H

#include <errno.h>
#define CMD_RETURN_OK		(0)
/* ������������ѭposix�ı�׼������ */

struct cmd
{
	const char * name, *desc, *help;

	int (*func)(int argc, char *argv[]);
};
extern struct cmd cmd__head;

/**
	@brief Find a cmd by name
 */
struct cmd *command_find(char * name, int try_binary);

/**
	@brief ִ������
 */
int command_exec(void * cmd, char * cmdline);

#define CMD_ENTRY	__attribute__((__section__(".cmd_entry")))

#endif