include $(MY_BUILD_BASE)/config.mk


#要编译的文件列表
OBJS = 
PARTS = non_user user

all:  subs_parts $(OBJS)
	$(LD) -r -o ../ip_non_user.o non_user.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
