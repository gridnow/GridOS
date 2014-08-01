include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)

#要编译的文件列表
OBJS = memset$(BITS).o memcpy$(BITS).o string_$(BITS).o uncached.o delay.o
PARTS =

all:  subs_parts $(OBJS)
	ld -r -o ../lib.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
