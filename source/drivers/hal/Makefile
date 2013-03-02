include $(MY_BUILD_BASE)/config.mk
include hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

TARGET					= hal.sys
LDFLAGS					=-L$(SYSTEM_DIR) --entry=start 
DEPENDS 				= --warn-unresolved-symbols
ADD_LDFLAGS				+= -T main/arch/$(ARCH_DIR)/lds$(BITS).txt

#要编译的文件列表
OBJS	= 
PARTS	= main time

all:  subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ARCH_LD_FLAGS)


include $(MY_BUILD_BASE)/cc.mk
