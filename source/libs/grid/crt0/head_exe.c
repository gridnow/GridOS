#include <types.h>
#include <string.h>
#include <errno.h>

static int crt0_split_cmdline(char * cmdline, int max_size, int *argc, int max_argc, char **argv)
{
	char * cur;
	int i = 0;
	int count = 0;
	
	/* �ָ��ַ��� */
	while (i < max_size)
	{
		/* ȥ�����ͷ�Ŀո� */
		if (cmdline[i] == ' ' || cmdline[i] == '	'/*tab */)
		{
			i++;
			continue;
		}
		
		/* ��������һ��һ�μ�¼��argv */
		argv[count] = &cmdline[i];
		count++;
		if (count >= max_argc)
			return E2BIG;
		
		/* ����һ���� */
		cur = strchr(&cmdline[i], ' ');
		if (cur)
		{
			i += (int)cur - (int)&cmdline[i] + 1/*�����ָ���*/;
			cur[0] = 0;
		}
		else
			break;
	}
	
	* argc = count;
	return 0;
}

extern int main(int argc, char ** argv);
#if defined(__i386__) || defined(__arm__)
int _start(unsigned long para)
#elif defined(__mips__)
int __start(unsigned long para)
#endif
{
#define MAX_ARGV 64
	int r;
	int argc;
	char *argv[MAX_ARGV];
	
	/* Handle argc argv */
	r = crt0_split_cmdline((char*)para, strlen((char*)para), &argc, MAX_ARGV, argv);
	if (r) return;
	
	r = main(argc, argv);
	
	return r;
}