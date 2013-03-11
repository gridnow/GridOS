include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= h2d.o vsprintf.o
PARTS	=
ADD_INCLUDE +=-I../include

all:  subs_parts $(OBJS)
	$(LD) -r -o ../misc.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
