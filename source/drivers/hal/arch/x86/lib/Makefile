include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS 	= string_$(BITS).o memcpy_$(BITS).o rwlock.o
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../lib.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
