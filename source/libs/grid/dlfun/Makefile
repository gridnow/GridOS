DLL=yes
include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=-I../include $(CL_INCLUDE)

#要编译的文件列表
OBJS = dlopen.o dlsym.o dlerr.o dlclose.o dladdr.o dl_crt.o lazy.o

all:  subs $(OBJS)
	$(LD) -r -o ../dlfun.o *.o $(ARCH_LD_FLAGS)
	#because we are using embeded c code
	rm dl_crt.o

subs:

include $(MY_BUILD_BASE)/cc.mk
