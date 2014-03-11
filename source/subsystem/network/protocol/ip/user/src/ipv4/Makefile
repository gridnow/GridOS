include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=-I../../include -I../../include/ipv4 -I../../include/arch

OBJS	= ip.o ip_addr.o icmp.o inet_chksum.o ip_frag.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../ipv4.o *.o $(ARCH_LD_FLAGS)
include $(MY_BUILD_BASE)/cc.mk
