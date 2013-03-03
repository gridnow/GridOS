include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= vsprintf.o digits.o num2str.o
PARTS	=

all:  subs_parts $(OBJS)
	ld -r -o ../common.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
