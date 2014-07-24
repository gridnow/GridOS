DLL=yes
include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = 
PARTS = $(ARCH_DIR)

all:subs_parts $(OBJS)
	$(LD) -r -o ../arch.o $(ARCH_DIR).o $(ARCH_LD_FLAGS)

	#pthread related
	$(RM) -f $(MY_BUILD_BASE)/build/pthread_spin_*.o
	$(CP) $(ARCH_DIR)/$(BITS)/pthread_spin_*.o $(MY_BUILD_BASE)/build/	
include $(MY_BUILD_BASE)/cc.mk
