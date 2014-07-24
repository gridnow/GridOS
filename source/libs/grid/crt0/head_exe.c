#include <types.h>
#include <string.h>
#include <errno.h>
#include <compiler.h>

int crt0_split_cmdline(char *cmdline, int max_size, int *argc, int max_argc, char **argv)
{
	char * cur;
	int i = 0;
	int count = 0;
	
	/* 分割字符串 */
	while (i < max_size)
	{
		/* 去掉命令开头的空格 */
		if (cmdline[i] == ' ' || cmdline[i] == '	'/*tab */)
		{
			i++;
			continue;
		}
		
		/* 把命令行一段一段记录到argv */
		argv[count] = &cmdline[i];
		count++;
		if (count >= max_argc)
			return E2BIG;
		
		/* 找下一个段 */
		cur = strchr(&cmdline[i], ' ');
		if (cur)
		{
			i += (int)cur - (int)&cmdline[i] + 1/*跳过分隔符*/;
			cur[0] = 0;
		}
		else
			break;
	}
	
	* argc = count;
	return 0;
}
extern void *crt_ld_unload_bootstrap(void *old_cmdline);
extern int main(int argc, char ** argv);
#if defined(__i386__) || defined(__arm__)
__noreturn int _start(void *cmdline)
#elif defined(__mips__)
__noreturn int __start(void *cmdline)
#endif
{
#define MAX_ARGV 64
	int r;
	int argc;
	char *argv[MAX_ARGV];
	
	cmdline = crt_ld_unload_bootstrap((void*)cmdline);

	/* Handle argc argv */
	r = crt0_split_cmdline((char*)cmdline, strlen(cmdline), &argc, MAX_ARGV, argv);
	if (r) goto end;
	
	r = main(argc, argv);
end:
	exit(r);
}
