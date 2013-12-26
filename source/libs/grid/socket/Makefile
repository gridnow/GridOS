DLL=yes
include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += $(CL_INCLUDE)

#要编译的文件列表
OBJS = socket.o 


all:$(OBJS)
	$(LD) -r -o ../socket.o *.o $(ARCH_LD_FLAGS)
	
include $(MY_BUILD_BASE)/cc.mk
