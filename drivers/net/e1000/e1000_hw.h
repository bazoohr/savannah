/**
 * @file e1000.h
 *
 * @brief Hardware specific datastructures of the Intel
 *        Pro/1000 Gigabit Ethernet card(s).
 *
 * Parts of this code is based on the DragonflyBSD (FreeBSD)
 * implementation, and the fxp driver for Minix 3.
 *
 * @see http://svn.freebsd.org/viewvc/base/head/sys/dev/e1000/
 * @see fxp.c
 *
 * @author Niek Linnenbank <nieklinnenbank@gmail.com>
 * @date September 2009
 *
 */

#ifndef __E1000_HW_H
#define __E1000_HW_H

#include <types.h>

/**
 * @name Datastructures.
 * @{
 */

/**
 * @brief Receive Descriptor Format.
 */
struct e1000_rx_desc
{
    uint32_t buffer;	/**< Address of the receive data buffer (64-bit). */
    uint32_t buffer_h;     /**< High 32-bits of the receive data buffer (unused). */
    uint16_t length;	/**< Size of the receive buffer. */
    uint16_t checksum;	/**< Packet checksum. */
    uint8_t  status;	/**< Descriptor status. */
    uint8_t  errors;	/**< Descriptor errors. */
    uint16_t special;	/**< VLAN information. */    
};

/**
 * @brief Transmit Descriptor Format.
 */
struct e1000_tx_desc
{
    uint32_t buffer;	/**< Address of the transmit buffer (64-bit). */
    uint32_t buffer_h;	/**< High 32-bits of the transmit buffer (unused). */
    uint16_t length;	/**< Size of the transmit buffer contents. */
    uint8_t  checksum_off; /**< Checksum Offset. */
    uint8_t  command;	/**< Command field. */
    uint8_t  status;	/**< Status field. */
    uint8_t  checksum_st;  /**< Checksum Start. */
    uint16_t special;	/**< Optional special bits. */
};
#if 0
/**
 * @brief ICH GbE Flash Hardware Sequencing Flash Status Register bit breakdown.
 * @see http://gitweb.dragonflybsd.org
 */
union ich8_hws_flash_status
{
    struct ich8_hsfsts
    {
        unsigned flcdone    :1; /**< bit 0 Flash Cycle Done */
        unsigned flcerr     :1; /**< bit 1 Flash Cycle Error */
        unsigned dael       :1; /**< bit 2 Direct Access error Log */
        unsigned berasesz   :2; /**< bit 4:3 Sector Erase Size */
        unsigned flcinprog  :1; /**< bit 5 flash cycle in Progress */
        unsigned reserved1  :2; /**< bit 13:6 Reserved */
        unsigned reserved2  :6; /**< bit 13:6 Reserved */
        unsigned fldesvalid :1; /**< bit 14 Flash Descriptor Valid */
        unsigned flockdn    :1; /**< bit 15 Flash Config Lock-Down */
    } hsf_status;
    u16_t regval;
};

/**
 * @brief ICH GbE Flash Hardware Sequencing Flash control Register bit breakdown.
 * @see http://gitweb.dragonflybsd.org
 */
union ich8_hws_flash_ctrl
{
    struct ich8_hsflctl
    {
        unsigned flcgo      :1;   /**< 0 Flash Cycle Go */
        unsigned flcycle    :2;   /**< 2:1 Flash Cycle */
        unsigned reserved   :5;   /**< 7:3 Reserved  */
        unsigned fldbcount  :2;   /**< 9:8 Flash Data Byte Count */
        unsigned flockdn    :6;   /**< 15:10 Reserved */
    } hsf_ctrl;
    u16_t regval;
};

/**
 * @brief ICH Flash Region Access Permissions.
 * @see http://gitweb.dragonflybsd.org
 */
union ich8_hws_flash_regacc
{
    struct ich8_flracc
    {
        unsigned grra      :8; /**< 0:7 GbE region Read Access */
        unsigned grwa      :8; /**< 8:15 GbE region Write Access */
        unsigned gmrag     :8; /**< 23:16 GbE Master Read Access Grant */
        unsigned gmwag     :8; /**< 31:24 GbE Master Write Access Grant */
    } hsf_flregacc;
    u16_t regval;
};
#endif
/**
 * @}
 */

/**
 * @name Receive Status Field Bits.
 * @{
 */

/** Passed In-exact Filter. */
#define E1000_RX_STATUS_PIF	(1 << 7) 
 
/** End of Packet. */
#define E1000_RX_STATUS_EOP	(1 << 1)

/** Descriptor Done. */
#define E1000_RX_STATUS_DONE	(1 << 0)

/**
 * @}
 */

/**
 * @name Receive Errors Field Bits.
 * @{
 */

/** RX Data Error. */
#define E1000_RX_ERROR_RXE	(1 << 7)

/** Carrier Extension Error. */
#define E1000_RX_ERROR_CXE	(1 << 4)

/** Sequence/Framing Error. */
#define E1000_RX_ERROR_SEQ	(1 << 2)

/** CRC/Alignment Error. */
#define E1000_RX_ERROR_CE	(1 << 0)

/**
 * @}
 */

/**
 * @name Transmit Command Field Bits.
 * @{
 */

/** End of Packet. */
#define E1000_TX_CMD_EOP	(1 << 0)

/** Insert FCS/CRC. */
#define E1000_TX_CMD_FCS	(1 << 1)

/** Report Status. */
#define E1000_TX_CMD_RS		(1 << 3)

/**
 * @}
 */

#endif /* __E1000_HW_H */
