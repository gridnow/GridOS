DLL=yes
include $(MY_BUILD_BASE)/config.mk

OBJS					= main.o fd.o
TARGET					= grid.so
DEPENDS					= -lhead_so.o 
#DEPENDS				+=../../pthread/pthread.o
DEPENDS					+= ../../libs/common/*.o $(SYSTEM_BUILD_TMP)/libs/elf2/$(ARCH_DIR)/elf.o
ADD_INCLUDE				+=-I./include $(CL_INCLUDE)
LDFLAGS					+=-Ttext-segment=0x70000000 -init dll_preinit
PARTS					= crt0 dlfun malloc string stdlib stdio socket dirent sys arch

all: subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ADD_LIBS) $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 
