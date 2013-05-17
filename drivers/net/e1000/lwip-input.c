#include <lwip/init.h>
#include <lwip/err.h>
#include <lwip/ip.h>
#include <lwip/sockets.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <netif/etharp.h>
#include <cpuinfo.h>
#include <debug.h>
#include <asmfunc.h>
#include <vuos/vuos.h>

#include <types.h>
void e1000_write (char *write_buf, int len);

static struct netif ni;
static struct udp_pcb *pcb = NULL;

static struct tcp_pcb *tcp_pcb = NULL;

static volatile uint64_t a, b;

uint64_t rdtscp (unsigned int *aux);

err_t output (struct netif *ni, struct pbuf *p,
                      ip_addr_t *ipaddr)
{
  //unsigned int aux = cpuinfo->cpuid;
#if 0
  struct eth_addr *dst_mac;
  ip_addr_t *dst_ip;

  etharp_find_addr(ni, ipaddr, &dst_mac, &dst_ip);
  DEBUG ("dst_mac: %"X8_F":%"X8_F":%"X8_F":%"X8_F":%"X8_F":%"X8_F"\n", 0xD,
     (unsigned)dst_mac->addr[0], (unsigned)dst_mac->addr[1], (unsigned)dst_mac->addr[2],
     (unsigned)dst_mac->addr[3], (unsigned)dst_mac->addr[4], (unsigned)dst_mac->addr[5]);
  DEBUG ("first byte = %x dst_ip = %s\n", 0xD, ((char *)p->payload)[0], ipaddr_ntoa(dst_ip));
#endif
  etharp_output(ni, p, ipaddr);

  return ERR_OK;
}

err_t our_init(struct netif *unused)
{
    return 0;
}

#define BUF_SIZE 4096
//char *new_buf[BUF_SIZE];
void our_print(void *args, struct udp_pcb *pcb, struct pbuf *p,
                ip_addr_t *addr, u16_t port)
{
#if 0
    DEBUGF ("%c%c%c p->len = %d p = %p\n", ((char*)p->payload)[0], ((char*)p->payload)[1], ((char*)p->payload)[2],
        p->len, p);
#endif
    udp_sendto(pcb, p, addr, port);
    //memcpy(new_buf, p->payload, p->len);
}

err_t tcp_our_print(void *args, struct tcp_pcb *pcb, struct pbuf *p, err_t error)
{
  if (unlikely (pcb == NULL)) {
    panic ("pcb is NULL");
  }

  if (likely (p != NULL)) {
#if 0
    DEBUGF ("%c%c%c p->len = %d p = %p\n", ((char*)p->payload)[0], ((char*)p->payload)[1], ((char*)p->payload)[2],
        p->len, p);
    memcpy(new_buf, p->payload, p->len);
#endif
    tcp_write (pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
    tcp_output (pcb);
  } else {
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_close (pcb);
  }

  return ERR_OK;
}

void our_tcp_err (void *args, err_t err)
{
  panic ("out_tcp_err");
  tcp_close (tcp_pcb);
}

err_t
our_tcp_sent (void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  //DEBUG ("OUR_TCP_SENT!!!!!!!\n\n", 0xC);
  return ERR_OK;
}
err_t our_accept (void *arg, struct tcp_pcb *pcb, err_t err)
{
  if (pcb == NULL) {
    panic ("pcb is null in accept");
  }
  /* Decrease the listen backlog counter */
  tcp_accepted(tcp_pcb);
  tcp_arg (pcb, NULL);
  tcp_sent (pcb, our_tcp_sent);
  tcp_recv (pcb, tcp_our_print);
  tcp_err (pcb, our_tcp_err);

  return ERR_OK;
}

err_t arp_output (struct netif *netif, struct pbuf *p)
{
  unsigned int aux = cpuinfo->cpuid;
  if (unlikely (p == NULL)) {
    panic ("arp_output: pbuf is null");
  }
  if (unlikely (p->next != NULL)) {
    panic("Packet is too big");
  }

  e1000_write(p->payload, p->len);

  a = rdtscp (&aux);
//  DEBUG ("TOOK %d\n", 0xF, (a - b));

  return ERR_OK;
}

struct pbuf *p;
void initialization(void)
{
    static ip_addr_t ipaddr, netmask, gw;

    lwip_init();

    IP4_ADDR(&gw, 192,168,1,254);
    IP4_ADDR(&ipaddr, 192,168,1,2);
    IP4_ADDR(&netmask, 255,255,255,0);

    netif_add(&ni, &ipaddr, &netmask, &gw, NULL, our_init, ethernet_input);
    netif_set_default(&ni);
    netif_set_up(&ni);

    /* set MAC hardware address length */
    ni.hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    ni.hwaddr[0] = 0x52;
    ni.hwaddr[1] = 0x54;
    ni.hwaddr[2] = 0x00;
    ni.hwaddr[3] = 0x12;
    ni.hwaddr[4] = 0x34;
    ni.hwaddr[5] = 0x56;

    /* maximum transfer unit */
    ni.mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    ni.flags |= NETIF_FLAG_BROADCAST;
    ni.flags |= NETIF_FLAG_ETHARP;

    ni.linkoutput = &arp_output;
    ni.output = &output;

    pcb = udp_new();
    udp_bind(pcb, &ipaddr, 1234);
    udp_recv(pcb, our_print, NULL);

    tcp_pcb = tcp_new ();
    tcp_bind (tcp_pcb, IP_ADDR_ANY, 80);
    tcp_pcb = tcp_listen (tcp_pcb);
    tcp_accept (tcp_pcb, &our_accept);
}

void lwip_input(const char *packet, const int len)
{
  unsigned int aux = cpuinfo->cpuid;
  b = rdtscp (&aux);
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    memcpy (p->payload, packet, len);
    //p->tot_len = p->len = len;
/*
    p->payload = (char*)packet;
    p->next = NULL;
    p->ref = 1;
    p->flags = 0;
    p->type = PBUF_POOL;
    */
    ni.input(p, &ni);
    //pbuf_free (p);
}
