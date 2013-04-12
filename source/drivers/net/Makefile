include $(MY_BUILD_BASE)/config.mk

#OBJS					= main.o
#TARGET						= netdevice.sys
#DEPENDS					+=-lhal.exp.sys -ldevmgr.sys -lsimple.sys
#LDFLAGS					+= -Ttext-segment=$(KMOD_SIMPLE_BASE) -shared

PARTS = ethernet
		
#目前用simple模块链接所有的网卡设备驱动程序，日后作为单独模块装载
all: subs_parts $(OBJS)
	$(LD) -r -o ../base/netdevice.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

 