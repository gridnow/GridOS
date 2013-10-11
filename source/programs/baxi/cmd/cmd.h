#ifndef CMD_H
#define CMD_H

struct cmd
{
	const char * name, *desc, *help;

	int (*func)(int argc, char *argv[]);
};

/**
	@brief Find a cmd by name
 */
struct cmd *command_find(char * name);

/**
	@brief Ö´ÐÐÃüÁî
 */
int command_exec(void * cmd, char * cmdline);

#define CMD_ENTRY	__attribute__((__section__(".cmd_entry")))

#endif