DLL=yes

include $(MY_BUILD_BASE)/config.mk

TARGET = tcpip.so
DEPENDS = -lhead_so.o -lgrid -lpthread
CL_INCLUDE  += -I$(MY_BUILD_BASE)/source/libs/grid/include
ADD_INCLUDE += -I./include -I./include/lwip -I./include/ipv4 -I./include/netif -I./include/arch $(CL_INCLUDE)

#要编译的文件列表
OBJS = main.o
PARTS = src

all:  subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(MY_BUILD_BASE)/source/libs/common/ring_buff.o $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)


include $(MY_BUILD_BASE)/cc.mk
