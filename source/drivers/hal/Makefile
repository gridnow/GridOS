include $(MY_BUILD_BASE)/config.mk
include hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)

TARGET					= hal.sys
LDFLAGS					=-L$(SYSTEM_DIR) --entry=start 
DEPENDS 				= --warn-unresolved-symbols
ADD_LDFLAGS				+= -T main/arch/$(ARCH_DIR)/lds$(BITS).txt

DEPENDS					+= ../../libs/common/*.o
#要编译的文件列表
OBJS	= 
PARTS	= main time libs

all:  subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ARCH_LD_FLAGS)

#
#i386? we can test in virtual machine
#
ifeq ($(ARCH),i386)
	$(CP) $(TARGET) $(TARGET).org
	$(OBJCOPY) -O binary -S $(TARGET)
	$(CP) $(TARGET) $(SYSTEM_DIR)
endif

include $(MY_BUILD_BASE)/cc.mk
