include $(MY_BUILD_BASE)/config.mk


OBJS					= pack.o
TARGET					= bsp.bin

PARTS =

all: update_exp.sys subs_parts $(OBJS)	
	$(LD) $(OBJS) $(ARCH_LD_FLAGS) -Ttext-segment 0 --section-start .MIPS.options=0x10000000 --section-start .data=0 -o $(TARGET)
	$(OBJCOPY) -S -O binary -j .data $(TARGET) $(TARGET)
	$(CP) $(TARGET) $(SYSTEM_DIR)
	rm *.o
	
update_exp.sys:
	$(OBJCOPY) -S linux.drv linux.drv
	#$(CP) $(SYSTEM_DIR)/hal.exp.sys .
	
include $(MY_BUILD_BASE)/cc.mk

 
