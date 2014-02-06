/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   系统初始化进程
 */

#include <stdio.h>
#include <dlfcn.h>

int test_bss_data;

#define MAX_SERVICE_ARGC 10

static void startup_desktop()
{
#if 0
	char error[1024];
	void *libwine_handle;
	//char *args[]={"wineloader","c:\\gcc.exe","/c","/FAs","c:\\myfile.c",NULL};				//For VC++6.0 cl
	char *args[]={"wineloader","c:\\gcc.exe","c:\\myfile.c",NULL};							//For notepad
	//char *args[]={"wineloader","c:\\AngryBirds\\AngryBirds.exe",NULL};							//For angrybirds
	//char *args[]={"wineloader","c:\\mplayer.exe","c:\\a.flv","-vo", "none",NULL};										//For notepad
	//char *args[]={"wineloader","c:\\gcc.exe","/DLL","c:\\windows\\myfile.obj"};				//For VC++6.0 linker
	//char *args[]={"wineloader","c:\\gcc.exe","-s","myfile","-l","language\\chinese.oic"};		//For Olanguage
	void (*wine_init)(int argc, char *argv[], char *error, int error_size );
	char *pWineBaseDir = "/0:/wine/";
	
	FASetCurrentThreadNameA("winapp 主线程.");
	FAVfsInitTTY();												//必须将客户端的0,1,2 FD 对应的位置预留起来,WINE内部是固定用这几个来作为CONSOLE处理的,它要将0,1,2发送给server,导致发送失败
	setenv("WINEDLLPATH","dlls",TRUE);							//WINE dll目录;
	setenv("WINEPREFIX",pWineBaseDir,TRUE);						//Server Connect需要得到当前工作目录；
	//setenv("WINEDEBUG","+all,trace-heap",TRUE);					//OPen all messages
	setenv("WINEDEBUG","+trace-heap",TRUE);					//OPen all messages
	setenv("PWD","0:",TRUE);
	setenv("DISPLAY","unix:0.0",TRUE);							/*	X defualt display name */
	
	dprintf("\n正在启动 wine server...");
	FAStartupServiceA("WINESERVER",TRUE);
	dprintf("\n正在启动 wine ...");
#ifdef __i386__
	__asm("mov $0,%ax\n\t "
		  "mov %ax,%fs\n\t");
#endif
	
	
	/*
	 切换当前工作路径
	 */
	if(FAChangeDirByStringA(pWineBaseDir)!=FASTATUS_SUCCESS)
	{
		dprintf("\n系统没有安装Wine.");
		return 0;
	}
	/*
	 打开第一个动态库
	 */
	libwine_handle = dlopen(ELF_FILE_NAME,NULL);
	if(libwine_handle==NULL)
	{
		dprintf("\nWINLOADER:无法找到%s.",ELF_FILE_NAME);
		return 0;
	}
	FASleepThreadMs(5000);						//waiting the wineserver is ready.
	wine_init	= dlsym(libwine_handle,"wine_init");/*Call wine Init*/
	if(wine_init)
		wine_init( sizeof(args)/(sizeof(char*))-1/*MUST escape the null*/, args, error, sizeof(error) );
	else
	{
		dprintf("\nWINLOADER:无法找到入口wine_init.");
		return 0;
	}
	/*Load the libwine,因为wine_init这个入口函数在libwine.a中，根据loader\main.c中的流程，在运行wine_init之前还要运行preloader,和pthread,不知道是干什么的*/
	return 0;
#endif
}

static void startup_services()
{
	char *argv[MAX_SERVICE_ARGC];
	int argc = 1;
	void *server;
	int (*entry)(int argc, char **argv);
//	setenv("WINEPREFIX","/0:/wine/",TRUE);
//	setenv("WINEDEBUG","+all,trace-heap",TRUE);
	printf("%s %s %d.\n", __FUNCTION__, __FILE__, __LINE__);
	server = dlopen("wineserver", RTLD_GLOBAL);
	if (!server)
		goto next;printf("%s %s %d.\n", __FUNCTION__, __FILE__, __LINE__);
	entry = dlentry(server);
	if (!entry)
		goto next;printf("%s %s %d.\n", __FUNCTION__, __FILE__, __LINE__);
	argc = 2;
	argv[0] = "wineserver";
	argv[1] = "-f";
	printf("启动Wineserver...");
	dlcall_posix_entry(entry, argc, argv);
	printf("OK.\n");
 
	return;
next:printf("%s %s %d.\n", __FUNCTION__, __FILE__, __LINE__);;
}

void main()
{
	FILE *fp;	
	
	printf("初始化进程启动中(stack = %p), stderr = %p...\n", &fp, stderr);	
	
	y_process_create("NES", "0:\\os\\i386\\nes.exe 0:\\os\\i386\\90.nes");
	
	/* Startup service */
	//startup_services();
	printf("\npreparing message loop.\n");
	y_message_loop();
}
