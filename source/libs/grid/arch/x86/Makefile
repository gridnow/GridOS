DLL=yes
include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = 
PARTS = $(BITS)

all:subs_parts $(OBJS)
	$(LD) -r -o ../x86.o *.o $(ARCH_LD_FLAGS)
	$(CP) $(BITS)/pthread_spin_*.o $(MY_BUILD_BASE)/build/
	
include $(MY_BUILD_BASE)/cc.mk
