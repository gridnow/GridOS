include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = printf.o
ADD_INCLUDE +=-I../include

all:$(OBJS)
	$(LD) -r -o ../stdio.o *.o $(ARCH_LD_FLAGS)

	
include $(MY_BUILD_BASE)/cc.mk
