#Must use ABICALLS by setting NORMAL_APP, the ld.sys will call the dynamic entry with t9 as base on MIPS
NORMAL_APP=yes

include $(MY_BUILD_BASE)/config.mk

OBJS					= main.o
TARGET					= fnotify
DEPENDS					+=-lgrid -lhead_exe.o -lpthread

all: subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ADD_LIBS) $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 
