include $(MY_BUILD_BASE)/config.mk


#要编译的文件列表
OBJS = memcpy.o strcpy.o strcmp.o strnlen.o memcmp.o strchr.o


all:$(OBJS)
	$(LD) -r -o ../string.o *.o $(ARCH_LD_FLAGS)
	
include $(MY_BUILD_BASE)/cc.mk
