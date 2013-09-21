/*
	动态库运行时的动态链接函数(Lazy linking).
	在faso 中安装到系统中。
*/
	.global so_lazy_link
	.extern lazy_get_symbole_by_id
	.type	so_lazy_link, @function
so_lazy_link:
#if defined(__i386__)
	/*
		PLT传入了符号ID和模块ID。我们查找符号，并且调入符号所指向的地址
	*/
	call lazy_get_symbole_by_id
	addl $8		, %esp												/*还原PLT压入的值*/
	jmpl *%eax														/*调用解析到的符号地址*/
	
#elif defined(__mips__)
#warning "MIPS Lazy linking not implemented."
	jr $ra
#endif
	.size	so_lazy_link, .-so_lazy_link
