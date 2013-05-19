include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h 

#要编译的文件列表
OBJS 	= memset.o memzero.o div64.o lib1funcs.o clearbit.o memcpy.o
OBJS	+=
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../lib.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
