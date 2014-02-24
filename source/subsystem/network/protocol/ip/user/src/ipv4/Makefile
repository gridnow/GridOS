include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=-I../../include

OBJS	= ip.o ip_addr.o icmp.o inet_chksum.o ip_frag.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../ipv4.o *.o $(ARCH_LD_FLAGS)
include $(MY_BUILD_BASE)/cc.mk
