ROOT = ../..

include $(ROOT)/rules.mak

MYDIR = lib/lwip/

CFLAGS  += -D_SYSTEM -I ./ -I ./include/ -I ../include -I./include/ipv4 -I./include/ipv6 -I $(ROOT)/include/

AR=ar
ARFLAGS=cr

#include "${CURDIR}/api/Makefile.inc"
#-include "${CURDIR}/core/Makefile.inc"
#-include "${CURDIR}/netif/Makefile.inc"

#SRCS += sys_arch.c
#SRCS += \
#	api/api_lib.c \
#  	api/api_msg.c \
#  	api/err.c \
#  	api/netbuf.c \
#  	api/netdb.c \
#	api/netifapi.c \
#  	api/sockets.c \
#  	api/tcpip.c

SRCS += \
	core/def.c \
	core/dns.c \
	core/tcp_out.c \
	core/timers.c \
	core/mem.c \
	core/init.c \
	core/tcp.c \
	core/raw.c \
	core/dhcp.c \
	core/sys.c \
	core/udp.c \
	core/tcp_in.c \
	core/netif.c \
	core/memp.c \
	core/pbuf.c \
	core/stats.c \
	\
	core/ipv4/inet_chksum.c \
	core/ipv4/ip.c \
	core/ipv4/igmp.c \
	core/ipv4/icmp.c \
	core/ipv4/ip_addr.c \
	core/ipv4/ip_frag.c \
	core/ipv4/autoip.c \
	\
	core/snmp/mib_structs.c \
	core/snmp/mib2.c \
	core/snmp/msg_out.c \
	core/snmp/asn1_enc.c \
	core/snmp/asn1_dec.c \
	core/snmp/msg_in.c

SRCS += \
	netif/etharp.c \
	netif/ethernetif.c \
	netif/slipif.c \
	\
	netif/ppp/chap.c \
	netif/ppp/magic.c \
	netif/ppp/pap.c \
	netif/ppp/fsm.c \
	netif/ppp/vj.c \
	netif/ppp/md5.c \
	netif/ppp/ppp_oe.c \
	netif/ppp/lcp.c \
	netif/ppp/ipcp.c \
	netif/ppp/auth.c \
	netif/ppp/ppp.c \
	netif/ppp/randm.c \
	netif/ppp/chpms.c

SRCS += sys_arch.c

OBJ = $(subst .S,.o,$(filter %.S,$(SRCS))) $(subst .c,.o,$(filter %.c,$(SRCS)))

$(LWIPLIB): $(OBJ)
	$(call silent_command, $(AR) $(ARFLAGS) ../$@ $(OBJ), "    ARCH  $@")

.PHONY: clean
.PHONY: distclean
clean:
	$(call silent_command, rm -f *.o *.d *.a $(LWIPLIB) && find . -name \*.o -delete, "    CLEAN $(MYDIR)")
distclean:
	$(call silent_command, rm -f *.o *.d *.a $(LWIPLIB) && find . -name \*.o -delete, "    CLEAN ALL $(MYDIR)")
