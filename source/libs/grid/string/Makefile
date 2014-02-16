DLL=yes
include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += $(CL_INCLUDE)

#要编译的文件列表
OBJS = memcpy.o strcpy.o strncmp.o strncpy.o strlen.o strcmp.o strnlen.o memcmp.o strrchr.o strchr.o strcat.o


all:$(OBJS)
	$(LD) -r -o ../string.o *.o $(ARCH_LD_FLAGS)
	
include $(MY_BUILD_BASE)/cc.mk
