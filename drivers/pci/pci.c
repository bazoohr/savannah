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
#include <vuos/vuos.h>
#include <ioapic.h>
#include <interrupt.h>
/* ========================================== */
#include "pci.h"
/* ========================================== */
#define NR_PCIDEV 50
#define BAR_NR		6	/* Number of base-address registers */
/* ========================================== */
static int nr_pcidev = 0;
/* ========================================== */
static struct pcidev
{
	uint8_t busnr;
	uint8_t dev;
	uint8_t func;
	uint8_t baseclass;
	uint8_t subclass;
	uint8_t infclass;
  uint8_t headt;
	uint16_t vid;
	uint16_t did;
	uint8_t ilr;
	uint16_t sts;
	int ipr;

	uint8_t inuse;
	struct bar
	{
		int flags;
		int nr;
		uint32_t base;
		uint32_t size;
	} bar[BAR_NR];
	int bar_nr;
} pcidev[NR_PCIDEV];
/* ========================================== */
static void
pci_attr_w8 (int devidx, int port, uint8_t value)
{
	PCII_WREG8_ (pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port, value);
	//outl (PCII_CONFADD, PCII_UNSEL);
}
/* ========================================== */
static void
pci_attr_w16 (int devidx, int port, uint16_t value)
{
	PCII_WREG16_ (pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port, value);
	//outl (PCII_CONFADD, PCII_UNSEL);
}
/* ========================================== */
static void
pci_attr_w32 (int devidx, int port, uint32_t value)
{
	PCII_WREG32_ (pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port, value);
	//outl (PCII_CONFADD, PCII_UNSEL);
}
/* ========================================== */
static uint8_t
pci_attr_r8_u (int devidx, int port)
{
	uint8_t v;

	v = PCII_RREG8_ (pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port);
	//outl (PCII_CONFADD, PCII_UNSEL);

  return v;
}
/* ========================================== */
static uint16_t
pci_attr_r16 (int devidx, int port)
{
  uint16_t v;

	v= PCII_RREG16_ (pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port);

  //outl(PCII_CONFADD, PCII_UNSEL);

  return v;
}
/* ========================================== */
static uint32_t
pci_attr_r32_u (int devidx, int port)
{
	uint32_t v;

	v = PCII_RREG32_(pcidev[devidx].busnr, pcidev[devidx].dev, pcidev[devidx].func, port);
	//outl(PCII_CONFADD, PCII_UNSEL);

	return v;
}
/* ========================================== */
static uint16_t
pci_attr_rsts (int devidx)
{
	uint16_t v;

	v = PCII_RREG16_ (pcidev[devidx].busnr, 0, 0, PCI_SR);
	//outl (PCII_CONFADD, PCII_UNSEL);

	return v;
}
/* ========================================== */
static int record_bar(int devidx, int bar_nr, int last)
{
	int reg, type, dev_bar_nr, width;
	uint32_t bar, bar2;
	uint16_t cmd;

	/* Start by assuming that this is a 32-bit bar, taking up one DWORD. */
	width = 1;

	reg = PCI_BAR+4*bar_nr;

	bar = pci_attr_r32_u(devidx, reg);
  if (bar != 0xFFFFFFFF) {
    DEBUG ("bar = %x\n", 0xA, bar);
  }
	if (bar & PCI_BAR_IO) {
		/* Disable I/O access before probing for BAR's size */
		cmd = pci_attr_r16(devidx, PCI_CR);
		pci_attr_w16(devidx, PCI_CR, cmd & ~PCI_CR_IO_EN);

		/* Probe BAR's size */
		pci_attr_w32(devidx, reg, 0xffffffff);
		bar2 = pci_attr_r32_u(devidx, reg);

		/* Restore original state */
		pci_attr_w32(devidx, reg, bar);
		pci_attr_w16(devidx, PCI_CR, cmd);

		bar &= PCI_BAR_IO_MASK;		/* Clear non-address bits */
		bar2 &= PCI_BAR_IO_MASK;
		bar2 = (~bar2 & 0xffff) + 1;

		dev_bar_nr = pcidev[devidx].bar_nr++;
#define PBF_IO		1	/* I/O else memory */
		pcidev[devidx].bar[dev_bar_nr].flags = PBF_IO;
		pcidev[devidx].bar[dev_bar_nr].base = bar;
		pcidev[devidx].bar[dev_bar_nr].size = bar2;
		pcidev[devidx].bar[dev_bar_nr].nr = bar_nr;
		if (bar == 0) {
      panic ("Bar is zero!");
		}
	} else {
		type = (bar & PCI_BAR_TYPE);

		switch(type) {
		case PCI_TYPE_32:
		case PCI_TYPE_32_1M:
			break;

		case PCI_TYPE_64:
			/* A 64-bit BAR takes up two consecutive DWORDs. */
			if (last)
				return width;
			width++;

			bar2 = pci_attr_r32_u(devidx, reg+4);

			/* If the upper 32 bits of the BAR are not zero, the
			 * memory is inaccessible to us; ignore the BAR.
			 */
			if (bar2 != 0)
				return width;

			break;

		default:
			/* Ignore the BAR. */
			return width;
		}

		/* Disable mem access before probing for BAR's size */
		cmd = pci_attr_r16(devidx, PCI_CR);
		pci_attr_w16(devidx, PCI_CR, cmd & ~PCI_CR_MEM_EN);

		/* Probe BAR's size */
		pci_attr_w32(devidx, reg, 0xffffffff);
		bar2 = pci_attr_r32_u(devidx, reg);

		/* Restore original values */
		pci_attr_w32(devidx, reg, bar);
		pci_attr_w16(devidx, PCI_CR, cmd);

		if (bar2 == 0)
			return width;	/* Reg. is not implemented */

		bar &= PCI_BAR_MEM_MASK;	/* Clear non-address bits */
		bar2 &= PCI_BAR_MEM_MASK;
		bar2 = (~bar2)+1;

		dev_bar_nr = pcidev[devidx].bar_nr++;
		pcidev[devidx].bar[dev_bar_nr].flags= 0;
		pcidev[devidx].bar[dev_bar_nr].base= bar;
		pcidev[devidx].bar[dev_bar_nr].size= bar2;
		pcidev[devidx].bar[dev_bar_nr].nr= bar_nr;
		if (bar == 0) {
      panic("Bar is zero!");
		}
	}

	return width;
}
/* ========================================== */
static void
record_bars (int devidx, int last_reg)
{
	int i, reg, width;

	for (i = 0, reg= PCI_BAR; reg <= last_reg; i += width, reg += 4 * width)
	{
		width = record_bar(devidx, i, reg == last_reg);
	}
}
/* ========================================== */
static void probe_bus (int busnr)
{
	uint32_t dev, func;
	uint16_t vid;
	uint16_t did;

	for (dev= 0; dev<32; dev++) {
		for (func= 0; func < 8; func++) {
			//pci_attr_wsts(devidx, PSR_SSE|PSR_RMAS|PSR_RTAS);
      pcidev[nr_pcidev].busnr = busnr;
      pcidev[nr_pcidev].dev = dev;
      pcidev[nr_pcidev].func = func;

			vid   = pci_attr_r16  (nr_pcidev, PCI_VID);
			did   = pci_attr_r16  (nr_pcidev, PCI_DID);
      if (vid == NO_VID && did == NO_VID)
        continue;

      pcidev[nr_pcidev].vid = vid;
      pcidev[nr_pcidev].did = did;
      pcidev[nr_pcidev].headt = pci_attr_r8_u (nr_pcidev, PCI_HEADT);
      pcidev[nr_pcidev].sts   = pci_attr_rsts (nr_pcidev);

      pcidev[nr_pcidev].baseclass = pci_attr_r8_u (nr_pcidev, PCI_BCR);
      pcidev[nr_pcidev].subclass  = pci_attr_r8_u (nr_pcidev, PCI_SCR);
      pcidev[nr_pcidev].infclass  = pci_attr_r8_u (nr_pcidev, PCI_PIFR);

      pcidev[nr_pcidev].ilr = pci_attr_r8_u (nr_pcidev, PCI_ILR);
      pcidev[nr_pcidev].ipr = pci_attr_r8_u (nr_pcidev, PCI_IPR);

      DEBUG ("prob_bus: vid = %x did = %x headt = %x sts = %x\nbaseclass = %x subclass = %x infclass = %x\nipr = %x ilr = %x\n================\n", 0xE,
          pcidev[nr_pcidev].vid, pcidev[nr_pcidev].did, pcidev[nr_pcidev].headt, pcidev[nr_pcidev].sts, pcidev[nr_pcidev].baseclass, pcidev[nr_pcidev].subclass, pcidev[nr_pcidev].infclass, pcidev[nr_pcidev].ipr, pcidev[nr_pcidev].ilr);
      if (pcidev[nr_pcidev].baseclass == SUBCLASS_NET) {
        DEBUG ("Found a network card, setting irq...\n", 0xA);
        DEBUG ("PCI: interrupt pin = %d line = %d dev %d func %d \n", 0xC, PCII_RREG8_(busnr, dev, func, PCI_IPR), PCII_RREG8_(busnr, dev, func, PCI_ILR), dev, func);
        uint32_t devfunc = ((dev * 8) + func);
        DEBUG ("Device function = %d\n", 0xC, devfunc);
        uint8_t pirq = (((devfunc >> 3) + pcidev[nr_pcidev].ipr - 2) & 0x3);
        DEBUG ("pirq = %d\n", 0xC, pirq);
        DEBUG ("irq = %d\n", 0xC, pirq + 16);
        //ioapic_enable(pirq + 16, cpuinfo->cpuid);
        ioapic_enable (pirq + 16, 3);
        add_irq (32 + pirq + 16);
        pci_attr_w8 (nr_pcidev,  PCI_ILR, NET_IRQ);
        DEBUG ("PCI: interrupt pin = %d line = %d\n", 0xC, PCII_RREG8_(busnr, dev, func, PCI_IPR), PCII_RREG8_(busnr, dev, func, PCI_ILR));
        record_bars (nr_pcidev, PCI_BAR_6);
      //  return;
      }
      nr_pcidev++;
    }
  }
}
void
print_e1000_reg (void)
{
  int i;
  int j;
  for (i = 0; i < nr_pcidev; i++) {
    if (pcidev[i].baseclass == SUBCLASS_NET) {
      for (j = 0; j < pcidev[i].bar_nr; j++) {
	DEBUG ("Command = %x\t ", 0xE, PCII_RREG16_(pcidev[i].busnr, pcidev[i].dev, pcidev[i].func, PCI_CR));
	DEBUG ("status= %x\n ", 0xE, PCII_RREG16_(pcidev[i].busnr, pcidev[i].dev, pcidev[i].func, PCI_SR));
        DEBUG ("base = %x\t", 0xA, pcidev[i].bar[j].base);
        DEBUG ("size = %x\n", 0xA, pcidev[i].bar[j].size);
      }
    }
  }
}
/* ========================================== */
void
pci_get_e1000_reg (phys_addr_t *base, size_t *size)
{
  int i;
  int j;
  for (i = 0; i < nr_pcidev; i++) {
    if (pcidev[i].baseclass == SUBCLASS_NET) {
      for (j = 0; j < pcidev[i].bar_nr; j++) {
        *base = pcidev[i].bar[j].base;
        *size = pcidev[i].bar[j].size;
        return;
      }
    }
  }
}
/* ========================================== */
void
__pci_init (void)
{
	uint32_t bus, dev, func;
	uint16_t vid, did;
  int i;

	bus= 0;
	dev= 0;
	func= 0;

	vid = PCII_RREG16_(bus, dev, func, PCI_VID);
	did = PCII_RREG16_(bus, dev, func, PCI_DID);
	outl (PCII_UNSEL, PCII_CONFADD);

  DEBUG ("PCI: vid = %x did = %x\n", 0xC, vid, did);

  for (i = 0; i < 8; i++) {
    DEBUG ("BUS number %x\n", 0xC, i);
    probe_bus (i);
  }

  print_e1000_reg ();
}
