include $(MY_BUILD_BASE)/config.mk
ADD_INCLUDE += -I. -Iarch/$(ARCH_DIR) -include configs.h

#要编译的文件列表
OBJS	= search.o probe.o bus.o access.o pci.o setup-bus.o setup-res.o pci-driver.o quirks.o host-bridge.o main.o
PARTS	= arch

all:  subs_parts $(OBJS)
	$(LD) -r -o ../pci.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
