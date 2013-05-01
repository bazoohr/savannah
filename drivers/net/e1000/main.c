#include <cdef.h>
#include <types.h>
#include <const.h>
#include <asmfunc.h>
#include <panic.h>
#include <ipc.h>
#include <string.h>
#include <cpuinfo.h>
#include <fs.h>
#include <debug.h>
#include <channel.h>
#include <lib_mem.h>
#include <vuos/vuos.h>
#include <lapic.h>
#include <interrupt.h>
#include <ioapic.h>
#include <gdt.h>

void print_e1000_reg (void);



void initialization(void);
void lwip_input(const char *packet, const int len);



#include "e1000_reg.h"
#include "e1000.h"
/* ========================================== */
#define APIC_ICR_TRIGGER		(1 << 15)
#define APIC_ICR_INT_MASK		(1 << 16)
/* ========================================== */
static struct e1000_rx_desc rx_desc[E1000_RXDESC_NR] __aligned (_4KB_);
static struct e1000_tx_desc tx_desc[E1000_TXDESC_NR] __aligned (_4KB_);
/* ========================================== */
static uint8_t rx_buf[E1000_RXDESC_NR * E1000_IOBUF_SIZE] __aligned (_4KB_);
static uint8_t tx_buf[E1000_TXDESC_NR * E1000_IOBUF_SIZE] __aligned (_4KB_);
/* ========================================== */
static phys_addr_t base_reg = 0;
static phys_addr_t size_reg = 0;
/* ========================================== */
void pci_get_e1000_reg (phys_addr_t *base, size_t *size);
void __pci_init (void);
/* ========================================== */
static void
e1000_reg_write (uint32_t reg, uint32_t value)
{
  /* Assume a sane register. */
  if (reg > 0x1ffff) {
    panic ("E1000: regs > 0x1ffff");
  }

  /* Write to memory mapped register. */
  *(volatile uint32_t *)(base_reg + reg) = value;
}
/* ========================================== */
static uint32_t
e1000_reg_read(uint32_t reg)
{
  uint32_t value;

  /* Assume a sane register. */
  if (reg >= 0x1ffff) {
    panic ("E1000: regs < 0x1ffff");
  }

  /* Read from memory mapped register. */
  value = *(volatile uint32_t *)(base_reg + reg);

  /* Return the result. */
  return value;
}
/* ========================================== */
static void
e1000_reg_set (uint32_t reg, uint32_t value)
{
  uint32_t data;

  /* First read the current value. */
  data = e1000_reg_read (reg);

  /* Set value, and write back. */
  e1000_reg_write (reg, data | value);
}
/* ========================================== */
static void
e1000_reg_unset (uint32_t reg, uint32_t value)
{
  uint32_t data;

  /* First read the current value. */
  data = e1000_reg_read (reg);

  /* Unset value, and write back. */
  e1000_reg_write (reg, data & ~value);
}
/* ========================================== */
static void e1000_reset_hw(void)
{
  /* Assert a Device Reset signal. */
  e1000_reg_set (E1000_REG_CTRL, E1000_REG_CTRL_RST);

  /* TODO:
   *     Here, we have to wait for 1 micro-second, we should do that
   *     using timer, but since at the present time we don't have that
   *     functionality implemented, we use a for loop to make this delay.
   *     This should be replaced with the proper fuction using lapic timer.
   */
  volatile uint64_t i;
  for (i = 0; i < 999999; i++);
  /* Wait one microsecond. */
  //tickdelay(1);
}
/* ================================================= */
static void
e1000_init_buf (void)
{
  int i;
  phys_addr_t rx_buf_p;
  phys_addr_t rx_desc_p;
  phys_addr_t tx_buf_p;
  phys_addr_t tx_desc_p;

  rx_buf_p = virt2phys (cpuinfo, (virt_addr_t)rx_buf);
  if (unlikely (rx_buf_p == 0)) {
    panic ("e1000_init_buf: rx_buf_p = 0");
  }

  rx_desc_p = virt2phys (cpuinfo, (virt_addr_t)rx_desc);
  if (unlikely (rx_desc_p == 0)) {
    panic ("e1000_init_buf: rx_desc_p = 0");
  }

  tx_buf_p = virt2phys (cpuinfo, (virt_addr_t)tx_buf);
  if (unlikely (tx_buf_p == 0)) {
    panic ("e1000_init_buf: tx_buf_p = 0");
  }

  tx_desc_p = virt2phys (cpuinfo, (virt_addr_t)tx_desc);
  if (unlikely (tx_desc_p == 0)) {
    panic ("e1000_init_buf: tx_desc_p = 0");
  }
  /* Setup receive descriptors. */
  for (i = 0; i < E1000_RXDESC_NR; i++) {
    rx_desc[i].buffer = (uint32_t)((virt_addr_t)rx_buf_p + (i * E1000_IOBUF_SIZE));
  }

  /* Setup transmit descriptors. */
  for (i = 0; i < E1000_TXDESC_NR; i++) {
    tx_desc[i].buffer = (uint32_t)((virt_addr_t)tx_buf_p + (i * E1000_IOBUF_SIZE));
  }

  /*
   * Setup the receive ring registers.
   */
  e1000_reg_write(E1000_REG_RDBAL, rx_desc_p);
  e1000_reg_write(E1000_REG_RDBAH, 0);
  e1000_reg_write(E1000_REG_RDLEN, E1000_RXDESC_NR * sizeof (struct e1000_rx_desc));
  e1000_reg_write(E1000_REG_RDH,   0);
  e1000_reg_write(E1000_REG_RDT,   E1000_RXDESC_NR - 1);
  e1000_reg_unset(E1000_REG_RCTL,  E1000_REG_RCTL_BSIZE);
  e1000_reg_set(E1000_REG_RCTL,  E1000_REG_RCTL_EN);

  /*
   * Setup the transmit ring registers.
   */
  e1000_reg_write(E1000_REG_TDBAL, tx_desc_p);
  e1000_reg_write(E1000_REG_TDBAH, 0);
  e1000_reg_write(E1000_REG_TDLEN, E1000_TXDESC_NR * sizeof (struct e1000_tx_desc));
  e1000_reg_write(E1000_REG_TDH,   0);
  e1000_reg_write(E1000_REG_TDT,   0);
  e1000_reg_set(  E1000_REG_TCTL,  E1000_REG_TCTL_EN | E1000_REG_TCTL_PSP);
}
/* ================================================= */
static void
e1000_init (void)
{
  int i;

  __pci_init ();
  pci_get_e1000_reg (&base_reg, &size_reg);

  DEBUG ("size = %x base = %x\n", 0xA, base_reg, size_reg);
  if (!(e1000_reg_read (E1000_REG_STATUS) & E1000_ENABLED)) {
    panic ("E1000 is not enabled!");
  }

  DEBUG ("Status register before reset %x\n", 0xB, e1000_reg_read (E1000_REG_STATUS));
  e1000_reset_hw ();
  DEBUG ("Status register after reset %x\n", 0xB, e1000_reg_read (E1000_REG_STATUS));
  /*
   * Initialize appropriately, according to section 14.3 General Configuration
   * of Intel's Gigabit Ethernet Controllers Software Developer's Manual.
   */
  e1000_reg_set(E1000_REG_CTRL, E1000_REG_CTRL_ASDE | E1000_REG_CTRL_SLU);
  e1000_reg_unset(E1000_REG_CTRL, E1000_REG_CTRL_LRST);
  e1000_reg_unset(E1000_REG_CTRL, E1000_REG_CTRL_PHY_RST);
  e1000_reg_unset(E1000_REG_CTRL, E1000_REG_CTRL_ILOS);
  e1000_reg_write(E1000_REG_FCAL, 0);
  e1000_reg_write(E1000_REG_FCAH, 0);
  e1000_reg_write(E1000_REG_FCT,  0);
  e1000_reg_write(E1000_REG_FCTTV, 0);
  e1000_reg_unset(E1000_REG_CTRL, E1000_REG_CTRL_VME);

  /* Clear Multicast Table Array (MTA). */
  for (i = 0; i < 128; i++)
  {
    e1000_reg_write(E1000_REG_MTA + i, 0);
  }
  /* Initialize statistics registers. */
  for (i = 0; i < 64; i++)
  {
    e1000_reg_write(E1000_REG_CRCERRS + (i * 4), 0);
  }

  /*
   * We hardcode the mac address here. Our MAC address would be
   * 52:54:00:12:34:56
   */
  e1000_reg_write(E1000_REG_RAL, (uint32_t)0x12005452);
  e1000_reg_write(E1000_REG_RAH, (uint16_t)0x5634);
  e1000_reg_set(E1000_REG_RAH,   E1000_REG_RAH_AV);
  e1000_reg_set(E1000_REG_RCTL,  E1000_REG_RCTL_MPE);
  DEBUG ("MAC LOW = %x\n", 0xC, e1000_reg_read (E1000_REG_RAL));
  DEBUG ("MAC HIGH = %x\n", 0xC, e1000_reg_read (E1000_REG_RAH));

  e1000_init_buf ();

  /* Enable interrupts. */
  e1000_reg_set(E1000_REG_IMS, E1000_REG_IMS_LSC  |
      E1000_REG_IMS_RXO  |
      E1000_REG_IMS_RXT  |
      E1000_REG_IMS_TXQE |
      E1000_REG_IMS_TXDW);
}
/* ========================================== */
void
e1000_write (char *write_buf, int len)
{
  int /*head,*/ tail;

  /* Find the head, tail and current descriptors. */
  //head =  e1000_reg_read(E1000_REG_TDH);
  tail =  e1000_reg_read(E1000_REG_TDT);

  /* Copy bytes to TX queue buffers. */
  memcpy (&tx_buf[tail * E1000_IOBUF_SIZE], write_buf, len);

  /* Move to next descriptor. */
  tx_desc[tail].status = 0;
  tx_desc[tail].length = len;
  tx_desc[tail].command = E1000_TX_CMD_EOP | E1000_TX_CMD_FCS | E1000_TX_CMD_RS;
  tail   = (tail + 1) % E1000_TXDESC_NR;
  /* Increment tail. Start transmission. */
  e1000_reg_write(E1000_REG_TDT,  tail);
}
/* ========================================== */
static void
e1000_read (void)
{
  int tail, cur;
  static int i = 0;
  struct e1000_rx_desc *desc;

  DEBUG ("%dth packet received!\n", 0xA, i++);

  //head = e1000_reg_read(E1000_REG_RDH);
  tail = e1000_reg_read(E1000_REG_RDT);
  cur  = (tail + 1) % E1000_RXDESC_NR;
  desc = &rx_desc[tail];
  char *buf = (char *)((uint64_t)(rx_buf + (cur * E1000_IOBUF_SIZE)));
  /* Increment tail. */
  e1000_reg_write(E1000_REG_RDT, cur);

  lwip_input (buf, desc->length);
}
/* ========================================== */
void rx_packet(void)
{
  int pin = 19;
  uint32_t cause;


  cause = e1000_reg_read (E1000_REG_ICR);

  //print_e1000_reg ();
  //DEBUG ("Mask = %x\n", 0xE, ioapic_read (REG_TABLE + 2 * pin));
  ioapic_enable_pin (pin);
  e1000_reg_write (0xD8, e1000_reg_read (E1000_REG_ICR));
  //DEBUG ("Mask = %x\n", 0xE, ioapic_read (REG_TABLE + 2 * pin));
  lapic_eoi();

  if (cause & (E1000_REG_ICR_RXO | E1000_REG_ICR_RXT)) {
    e1000_read ();
  }
#if 0
  uint32_t lo = ioapic_read(REG_TABLE+ pin * 2);

  bool is_masked = lo & APIC_ICR_INT_MASK;

  /* set mask and edge */
  lo |= APIC_ICR_INT_MASK;
  lo &= ~APIC_ICR_TRIGGER;
  ioapic_write(REG_TABLE + pin * 2, lo);

  /* set back to level and restore the mask bit */
  lo = ioapic_read(REG_TABLE + pin * 2);

  lo |= APIC_ICR_TRIGGER;
  if (is_masked)
    lo |= APIC_ICR_INT_MASK;
  else
    lo &= ~APIC_ICR_INT_MASK;
  ioapic_write(REG_TABLE +  pin * 2, lo);

  DEBUG ("Mask = %x\n", 0xE, ioapic_read (REG_TABLE + 2 * pin));
#endif
}
/* ========================================== */
int
main (int argc, char **argv)
{
  create_default_gdt ();
  interrupt_init ();
  lapic_on ();
  add_irq (32 + 19);
  DEBUG ("Mask = %x\n", 0xE, ioapic_read (REG_TABLE + 2 * (19)));

  initialization ();
  sti();

  e1000_init ();
  for (;;) {
    __asm__ __volatile__ ("hlt\n\t");
  }

  return 0;
}
