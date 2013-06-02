include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS 	= 
PARTS 	= mach-s3c64xx

all:  subs_parts $(OBJS)
	$(LD) -r -o ../s3c.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk
