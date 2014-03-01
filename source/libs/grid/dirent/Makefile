DLL=yes
include $(MY_BUILD_BASE)/config.mk


#要编译的文件列表
OBJS = opendir.o readdir.o closedir.o
ADD_INCLUDE +=-I../include

all:$(OBJS)
	$(LD) -r -o ../dirent.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
