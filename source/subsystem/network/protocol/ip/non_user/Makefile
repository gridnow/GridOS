include $(MY_BUILD_BASE)/config.mk


#要编译的文件列表
OBJS = main.o
PARTS = 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../non_user.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
