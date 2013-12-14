/*
	动态库运行时的动态链接函数(Lazy linking).
	在faso 中安装到系统中。
*/
#if defined(__i386__)
	.global so_lazy_link
	.extern lazy_get_symbole_by_id
so_lazy_link:
	/*
		PLT传入了符号ID和模块ID。我们查找符号，并且调入符号所指向的地址
	*/
	call lazy_get_symbole_by_id
	addl $8		, %esp												/*还原PLT压入的值*/
	jmpl *%eax														/*调用解析到的符号地址*/
	.size	so_lazy_link, .-so_lazy_link

	.global dl_call_posix_entry
	.extern dl_call_posix_entry
dl_call_posix_entry:	
	/* see glibc's EXE entry code for register usage at start.S */
	/* esp+1: entry, +2: argc, +3: argv */
	mov 12(%esp), %ecx												/* argv */
	mov 8(%esp), %eax												/* ARGC */
	push %ecx
	push %eax
	jmp *(4+8)(%esp)
	.size	dl_call_posix_entry, .-dl_call_posix_entry
	
#else
#warning "Lazy linking not implemented."	
#endif
