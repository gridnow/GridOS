#Must use ABICALLS by setting NORMAL_APP, the ld.sys will call the dynamic entry with t9 as base on MIPS
NORMAL_APP=yes

include $(MY_BUILD_BASE)/config.mk

OBJS					= test_new.o
TARGET					= test_new
DEPENDS					+=-lhead_exe.o -lgrdc++ -lpthread -lgrid 
ADD_INCLUDE             += -I$(MY_BUILD_BASE)/include/c++/std \
							-I$(MY_BUILD_BASE)/include/c++ $(STD_INCLUDE)
my_c++flags += -c

all: subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) *.o $(DEPENDS)  $(ADD_LIBS) $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 
