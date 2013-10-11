#Mips need ABICALLS for normal app with so linked
NORMAL_APP=yes

include $(MY_BUILD_BASE)/config.mk

OBJS					= cmd.o ls.o cat.o cd.o cp.o rm.o

#NULL 文件必须在最后编译，因为它是一些命令列表的终止点
OBJS					+=_null.o
all: subs_parts $(OBJS)
	$(LD) -r -o ../cmd.o $(OBJS) $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
