include $(MY_BUILD_BASE)/config.mk
include ../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS = 
PARTS = $(ARCH_DIR) 

all:  subs_parts $(OBJS)
	ld -r -o ../arch.o *.o $(ARCH_LD_FLAGS)


include $(MY_BUILD_BASE)/cc.mk
