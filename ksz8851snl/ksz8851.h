 /******************************************************************************
 * @filename	: 	ksz8851.h
 * @description : 	This file provides commands and functions to
 * 					communicate KSZ8851SNL ethernet controller.
 * @author      : 	M.Okan BUĞDAYCI
 * @copyright   : 	GNU licence.
 * @date        : 	01.05.2020
 * @revision	: 	v.1.0.0 - Driver files created
 
 This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/
 
 ******************************************************************************/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KSZ8851_H
#define __KSZ8851_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "ksz8851_config.h"
/* Defines -------------------------------------------------------------------*/

#define KSZ_CHIP_ID												0x8870		//bit 15-8: chip family id - 0x88, bit 7-4: chip ID 0x7 is assigned to KSZ8851SNL, bit 3-1: revision ID, revision id might change according the revision.
#define KSZ_CHIP_ID_MASK										0xFFF0		//masks last nibble of chip ID which represents revision ID.

#define KSZ_MAC_ADDRR_LEN										6			//bytes
#define KSZ_1BYTE_SHIFTING_VALUE								8			//to shift the variable content one byte
#define KSZ_TIME_WAIT_1MS										1
#define KSZ_TIME_WAIT_10MS										10
#define KSZ_TIME_WAIT_20MS										20

#define KSZ_PROCESS_TRY_LIMIT									3

#define KSZ_REG_BUFF_BYTE0										0
#define KSZ_REG_BUFF_BYTE1										1
#define KSZ_REG_BUFF_BYTE2										2
#define KSZ_REG_BUFF_BYTE3										3

#define KSZ_REG_CMD_BUFF_SIZE									4			//bytes
#define KSZ_REG_DATA_BUFF_SIZE									4			//bytes

#define KSZ_REG_CMD_BYTE0_MASK									0xFF00		//masks LSB byte of command to put one byte buffer
#define KSZ_REG_CMD_BYTE1_MASK									0x00FF		//masks MSB byte of command to put one byte buffer

#define KSZ_REG_CMD_SHIFT_VALUE									14			//shifting step the selected register bytes between frame bits 14-15

#define KSZ_REG_ADDR_MASK_VALUE									0x03F0		//masking value to make zero don't care address bits 0x03F0
#define KSZ_REG_ADDR_EVEN_CHECK_VALUE							2			//it is used to check register address even or odd
#define KSZ_REG_ADDR_BIT_SHIFT_VALUE							2			//shifting step the register address between frame bits 2-9

#define KSZ_REG_BYTES_SELECT_0_1_MASK_VALUE						0x03		//to select register's byte0 and byte1
#define KSZ_REG_BYTES_SELECT_2_3_MASK_VALUE						0x0C		//to select register's byte2 and byte3
#define KSZ_REG_BYTES_SELECT_BIT_SHIFT_VALUE					10			//shifting step the selected register bytes between frame bits 10-13

#define KSZ_DWORD_VALUE											4			// Dword (32 bit, 4 byte)

#define KSZ_RX_FRAME_LEN_MULTIPLE_VALUE							0x03		//While Rx frame reading from KSZ frame data must be reading dword aligned (multiple of 4 bytes).
																			//bitwise and this value with rx frame len give us idea how many bytes pad there will be in the rx frame reading
																			//ref: KSZ datasheet section 3.5.6


/* Specific configuration and status values for some register*/


/* Global reset register configuration values bit by bit */
#define KSZ_CONFIG_GLOBAL_SOFT_RESET							0x0001		// Resets PHY, MAC, DMA and all registers value are set to default value.
#define KSZ_CONFIG_QMU_MODULE_SOFT_RESET						0x0002		// Only resets TXQ and RXQ memories.


/* TX frame data pointer register configuration values */
#define KSZ_CONFIG_TX_FR_DPOINTER_AUTO_INC						0x4000		// Enable QMU Transmit Frame Data Pointer Auto increment.

/* QMU transmit control register configuration values bit by bit */
#define KSZ_CONFIG_TX_CTRL_TX_ENABLE							0x0001		// Enable transmit
#define KSZ_CONFIG_TX_CTRL_CRC_ENABLE							0x0002		// Enable adding a CRC to the end of transmit frame
#define KSZ_CONFIG_TX_CTRL_PAD_ENABLE							0x0004		// Enable adding a padding to a packet shorter than 64 bytes
#define KSZ_CONFIG_TX_CTRL_FLOW_ENABLE							0x0008		// Enable transmit flow control
#define KSZ_CONFIG_TX_CTRL_FLUSH_QUEUE							0x0010		// Clear transmit queue, reset tx frame pointer
#define KSZ_CONFIG_TX_CTRL_IP_CHECKSUM							0x0020		// Enable IP frame checksum generation
#define KSZ_CONFIG_TX_CTRL_TCP_CHECKSUM							0x0040		// Enable TCP frame checksum generation
#define KSZ_CONFIG_TX_CTRL_UDP_CHECKSUM							0x0080		// Enable UDP frame checksum generation
#define KSZ_CONFIG_TX_CTRL_ICMP_CHECKSUM						0x0100		// Enable ICMP frame checksum generation

/* QMU transmit status register values */

/* TX frame data pointer register configuration values */
#define KSZ_CONFIG_RX_FR_DPOINTER_AUTO_INC						0x4000		// Enable QMU Receive Frame Data Pointer Auto increment.

/* QMU receive control register 1 configuration values bit by bit */
#define KSZ_CONFIG_RX_CTRL1_RX_ENABLE							0x0001		// Enable receive
#define KSZ_CONFIG_RX_CTRL1_INVERSE_FILTER						0x0002		// Receive with address check in inverse filtering mode
#define KSZ_CONFIG_RX_CTRL1_RECEIVE_ALL							0x0010		// Receive all incoming frames, regardless of frame's DA
#define KSZ_CONFIG_RX_CTRL1_RECEIVE_UNICAST						0x0020		// Receive unicast frames that match the device MAC address
#define KSZ_CONFIG_RX_CTRL1_RECEIVE_ALL_MULTICAST				0x0040		// Receive all the multicast frames (including broadcast frames)
#define KSZ_CONFIG_RX_CTRL1_RECEIVE_BROADCAST					0x0080		// Receive all the broadcast frames
#define KSZ_CONFIG_RX_CTRL1_RECEIVE_MULTICAST					0x0100		// Receive multicast frames that pass the CRC hash filtering
#define KSZ_CONFIG_RX_CTRL1_ERROR_FR_ENABLE						0x0200		// Enable receive CRC error frames
#define KSZ_CONFIG_RX_CTRL1_FLOW_ENABLE							0x0400		// Enable receive flow control
#define KSZ_CONFIG_RX_CTRL1_MAC_FILTER							0x0800		// Receive with address that pass MAC address filtering
#define KSZ_CONFIG_RX_CTRL1_IP_CHECKSUM							0x1000		// Enable IP frame checksum verification
#define KSZ_CONFIG_RX_CTRL1_TCP_CHECKSUM						0x2000		// Enable TCP frame checksum verification
#define KSZ_CONFIG_RX_CTRL1_UDP_CHECKSUM						0x4000		// Enable UDP frame checksum verification
#define KSZ_CONFIG_RX_CTRL1_FLUSH_QUEUE							0x8000		// Clear receive queue, reset rx frame pointer

/* QMU receive control register 2 configuration values bit by bit */
#define KSZ_CONFIG_RX_CTRL2_BLOCK_SAME_MAC						0x0001		// Receive drop frame if the SA is same as device MAC address.
#define KSZ_CONFIG_RX_CTRL2_ICMP_CHECKSUM						0x0002		// Enable ICMP frame checksum verification
#define KSZ_CONFIG_RX_CTRL2_UDP_LITE_CHECKSUM					0x0004		// Enable UDP Lite frame checksum generation and verification
#define KSZ_CONFIG_RX_CTRL2_IPV6_UDP_CHECKSUM					0x0008		// Receive pass IPv6 UDP frame with UDP checksum is zero
#define KSZ_CONFIG_RX_CTRL2_IPV6_UDP_NOCHECKSUM					0x0010		// No checksum generation and verification if IPv6 UDP is fragment
#define KSZ_CONFIG_RX_CTRL2_DATA_BURST_4BYTES					0x0000		// 4 bytes SPI Receive Data Burst Length
#define KSZ_CONFIG_RX_CTRL2_DATA_BURST_8BYTES					0x0020		// 8 bytes SPI Receive Data Burst Length
#define KSZ_CONFIG_RX_CTRL2_DATA_BURST_16BYTES					0x0040		// 16 bytes SPI Receive Data Burst Length
#define KSZ_CONFIG_RX_CTRL2_DATA_BURST_32BYTES					0x0060		// 32 bytes SPI Receive Data Burst Length
#define KSZ_CONFIG_RX_CTRL2_DATA_BURST_FR_LEN					0x0080		// Single frame data burst length


/* RX frame count and threshold register configuration values */
#define KSZ_CONFIG_RX_FR_CTRL_THRESHOLD_1FR						0x0001		// Configure Receive Frame Threshold for one frame.

/* RX command register configuration values bit by bit */
#define KSZ_CONFIG_RX_CMD_RELEASE_ERROR_FR						0x0001		// Release RX Error Frame
#define KSZ_CONFIG_RX_CMD_START_DMA_ACCESS						0x0008		// Start QMU transfer operation
#define KSZ_CONFIG_RX_CMD_AUTO_DEQUEUE_RXQ						0x0010		// Enable Auto Dequeue RXQ Frame
#define KSZ_CONFIG_RX_CMD_FR_COUNT_THR_INT_ENABLE				0x0020		// Enable RX interrupt on frame count threshold
#define KSZ_CONFIG_RX_CMD_BYTE_COUNT_THR_INT_ENABLE				0x0040		// Enable RX interrupt on byte count threshold
#define KSZ_CONFIG_RX_CMD_DURATION_TIM_THR_ENABLE				0x0080		// Enable RX interrupt on timer duration
#define KSZ_CONFIG_RX_CMD_IP_TWOBYTE_OFFSET_ENABLE				0x0200		// Enable adding 2-bytes offset before IP frame header

/* RX command register status values bit by bit */
#define KSZ_STATUS_RX_CMD_FR_COUNT_THR_INT						0x0400		// RX interrupt is occured on frame count threshold
#define KSZ_STATUS_RX_CMD_BYTE_COUNT_THR_INT					0x0800		// RX interrupt is occured on byte count threshold
#define KSZ_STATUS_RX_CMD_DURATION_TIM_THR_INT					0x1000		// RX interrupt is occured on timer duration

/* On-Chip bus control register configuration values bit by bit */
#define KSZ_CONFIG_ONCHIP_BUS_CLK_DIVIDEBY_1					0x0000		// Bus clock devided by 1
#define KSZ_CONFIG_ONCHIP_BUS_CLK_DIVIDEBY_2					0x0001		// Bus clock devided by 2
#define KSZ_CONFIG_ONCHIP_BUS_CLK_DIVIDEBY_3					0x0002		// Bus clock devided by 3
#define KSZ_CONFIG_ONCHIP_BUS_CLK_125MHZ						0x0000		// Bus clock 125 MHZ
#define KSZ_CONFIG_ONCHIP_BUS_PIN_STRENG_8MA					0x0000		// Output Pin Drive Strength 8 mA
#define KSZ_CONFIG_ONCHIP_BUS_PIN_STRENG_16MA					0x0040		// Output Pin Drive Strength 16 mA

/* Port 1 control register configuration values bit by bit */
#define KSZ_CONFIG_PORT_AD_10BT_HALF_DUPLEX            			0x0001    	// Advertise 10 half-duplex capability
#define KSZ_CONFIG_PORT_AD_10BT_FULL_DUPLEX        				0x0002   	// Advertise 10 full-duplex capability
#define KSZ_CONFIG_PORT_AD_100BT_HALF_DUPLEX         			0x0004   	// Advertise 100 half-duplex capability
#define KSZ_CONFIG_PORT_AD_100BT_FULL_DUPLEX     				0x0008   	// Advertise 100 full-duplex capability
#define KSZ_CONFIG_PORT_AD_FLOW_CONTROL_PAUSE       			0x0010   	// Advertise pause capability
#define KSZ_CONFIG_PORT_FORCE_FULL_DUPLEX        				0x0020   	// Force PHY in full duplex mode
#define KSZ_CONFIG_PORT_FORCE_100_MBIT           				0x0040   	// Force PHY 100Mbps
#define KSZ_CONFIG_PORT_AUTO_NEG_ENABLE          				0x0080   	// Enable auto-negotiation
#define KSZ_CONFIG_PORT_FORCE_MDIX               				0x0200   	// Force MDI-X
#define KSZ_CONFIG_PORT_AUTO_MDIX_DISABLE        				0x0400   	// Disable auto MDI-X
#define KSZ_CONFIG_PORT_AUTO_NEG_RESTART         				0x2000   	// Restart auto-negotiation
#define KSZ_CONFIG_PORT_TX_DISABLE               				0x4000   	// Disable port transmit
#define KSZ_CONFIG_PORT_LED_OFF                  				0x8000   	// Turn off all the port LEDs (LED3/LED2/LED1/LED0)

/* Interrupt status flag's register configuration values bit by bit */
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
//#define KSZ_FLAGS_INTERRUPTS_
#define KSZ_FLAGS_INTERRUPTS_ALL_CLEAR							0xEB42		// Clear all interrupt flags

/* Flow control watermark configuration values */
#define KSZ_CONFIG_WATERMARK_4KB								0x0400		// 4KB watermark
#define KSZ_CONFIG_WATERMARK_6KB								0x0600		// 6KB watermark


#define KSZ_CONFIG_CLEAR_ALL_BITS								0x0000
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_
//#define KSZ_CONFIG_



/* Enums ---------------------------------------------------------------------*/

typedef enum
{
	KSZ8851_READ_REG 		= 0x00,		//!< KSZ8851_READ_REG
	KSZ8851_WRITE_REG		= 0x01,		//!< KSZ8851_WRITE_REG
	KSZ8851_READ_RX_FIFO	= 0x02,		//!< KSZ8851_READ_RX_FIFO
	KSZ8851_WRITE_TX_FIFO	= 0x03		//!< KSZ8851_WRITE_TX_FIFO

}KSZ8851_Cmd;

typedef enum
{
	KSZ_GPIO_PIN_RESET = 0,
	KSZ_GPIO_PIN_SET

}KSZ8851_GPIO_Control;

/* Register addresses of KSZ8851SNL */
typedef enum
{
	KSZ_REG_ADDR_CCR0      = 0x08,		//Chip configuration register byte 0
	KSZ_REG_ADDR_CCR1      = 0x09,		//Chip configuration register byte 1


	KSZ_REG_ADDR_MARL0     = 0x10,		//MAC address register low byte 0
	KSZ_REG_ADDR_MARL1     = 0x11,		//MAC address register low byte 1
	KSZ_REG_ADDR_MARM0     = 0x12,		//MAC address register middle byte 0
	KSZ_REG_ADDR_MARM1     = 0x13,		//MAC address register middle byte 1
	KSZ_REG_ADDR_MARH0     = 0x14,		//MAC address register high byte 0
	KSZ_REG_ADDR_MARH1     = 0x15,		//MAC address register high byte 1


	KSZ_REG_ADDR_OBCR0     = 0x20,		//ON-CHIP BUS CONTROL REGISTER BYTE 0
	KSZ_REG_ADDR_OBCR1     = 0x21,		//ON-CHIP BUS CONTROL REGISTER BYTE 1


	KSZ_REG_ADDR_EEPCR0    = 0x22,		//EEPROM Control REGISTER BYTE 0
	KSZ_REG_ADDR_EEPCR1    = 0x23,		//EEPROM Control REGISTER BYTE 1


	KSZ_REG_ADDR_MBIR0     = 0x24,		//Memory BIST Info Register Byte 0
	KSZ_REG_ADDR_MBIR1     = 0x25,		//Memory BIST Info Register Byte 1


	KSZ_REG_ADDR_GRR0      = 0x26,		//Global Reset Register Byte 0
	KSZ_REG_ADDR_GRR1      = 0x27,		//Global Reset Register Byte 1


	KSZ_REG_ADDR_WFCR0     = 0x2A,		//Wakeup Frame Control Register Byte 0
	KSZ_REG_ADDR_WFCR1     = 0x2B,		//Wakeup Frame Control Register Byte 1


	KSZ_REG_ADDR_WF0CRC0_0 = 0x30,		//Wakeup Frame 0 CRC0 Register Byte 0
	KSZ_REG_ADDR_WF0CRC0_1 = 0x31,		//Wakeup Frame 0 CRC0 Register Byte 1
	KSZ_REG_ADDR_WF0CRC1_0 = 0x32,		//Wakeup Frame 0 CRC1 Register Byte 0
	KSZ_REG_ADDR_WF0CRC1_1 = 0x33,		//Wakeup Frame 0 CRC1 Register Byte 1
	KSZ_REG_ADDR_WF0BM0_0  = 0x34,		//Wakeup Frame 0 Byte Mask 0 Register Byte 0
	KSZ_REG_ADDR_WF0BM0_1  = 0x35,		//Wakeup Frame 0 Byte Mask 0 Register Byte 1
	KSZ_REG_ADDR_WF0BM1_0  = 0x36,		//Wakeup Frame 0 Byte Mask 1 Register Byte 0
	KSZ_REG_ADDR_WF0BM1_1  = 0x37,		//Wakeup Frame 0 Byte Mask 1 Register Byte 1
	KSZ_REG_ADDR_WF0BM2_0  = 0x38,		//Wakeup Frame 0 Byte Mask 2 Register Byte 0
	KSZ_REG_ADDR_WF0BM2_1  = 0x39,		//Wakeup Frame 0 Byte Mask 2 Register Byte 1
	KSZ_REG_ADDR_WF0BM3_0  = 0x3A,		//Wakeup Frame 0 Byte Mask 3 Register Byte 0
	KSZ_REG_ADDR_WF0BM3_1  = 0x3B,		//Wakeup Frame 0 Byte Mask 3 Register Byte 1


	KSZ_REG_ADDR_WF1CRC0_0 = 0x40,		//Wakeup Frame 1 CRC0 Register Byte 0
	KSZ_REG_ADDR_WF1CRC0_1 = 0x41,		//Wakeup Frame 1 CRC0 Register Byte 1
	KSZ_REG_ADDR_WF1CRC1_0 = 0x42,		//Wakeup Frame 1 CRC1 Register Byte 0
	KSZ_REG_ADDR_WF1CRC1_1 = 0x43,		//Wakeup Frame 1 CRC1 Register Byte 1
	KSZ_REG_ADDR_WF1BM0_0  = 0x44,		//Wakeup Frame 1 Byte Mask 0 Register Byte 0
	KSZ_REG_ADDR_WF1BM0_1  = 0x45,		//Wakeup Frame 1 Byte Mask 0 Register Byte 1
	KSZ_REG_ADDR_WF1BM1_0  = 0x46,		//Wakeup Frame 1 Byte Mask 1 Register Byte 0
	KSZ_REG_ADDR_WF1BM1_1  = 0x47,		//Wakeup Frame 1 Byte Mask 1 Register Byte 1
	KSZ_REG_ADDR_WF1BM2_0  = 0x48,		//Wakeup Frame 1 Byte Mask 2 Register Byte 0
	KSZ_REG_ADDR_WF1BM2_1  = 0x49,		//Wakeup Frame 1 Byte Mask 2 Register Byte 1
	KSZ_REG_ADDR_WF1BM3_0  = 0x4A,		//Wakeup Frame 1 Byte Mask 3 Register Byte 0
	KSZ_REG_ADDR_WF1BM3_1  = 0x4B,		//Wakeup Frame 1 Byte Mask 3 Register Byte 1


	KSZ_REG_ADDR_WF2CRC0_0 = 0x50,		//Wakeup Frame 2 CRC0 Register Byte 0
	KSZ_REG_ADDR_WF2CRC0_1 = 0x51,		//Wakeup Frame 2 CRC0 Register Byte 1
	KSZ_REG_ADDR_WF2CRC1_0 = 0x52,		//Wakeup Frame 2 CRC1 Register Byte 0
	KSZ_REG_ADDR_WF2CRC1_1 = 0x53,		//Wakeup Frame 2 CRC1 Register Byte 1
	KSZ_REG_ADDR_WF2BM0_0  = 0x54,		//Wakeup Frame 2 Byte Mask 0 Register Byte 0
	KSZ_REG_ADDR_WF2BM0_1  = 0x55,		//Wakeup Frame 2 Byte Mask 0 Register Byte 1
	KSZ_REG_ADDR_WF2BM1_0  = 0x56,		//Wakeup Frame 2 Byte Mask 1 Register Byte 0
	KSZ_REG_ADDR_WF2BM1_1  = 0x57,		//Wakeup Frame 2 Byte Mask 1 Register Byte 1
	KSZ_REG_ADDR_WF2BM2_0  = 0x58,		//Wakeup Frame 2 Byte Mask 2 Register Byte 0
	KSZ_REG_ADDR_WF2BM2_1  = 0x59,		//Wakeup Frame 2 Byte Mask 2 Register Byte 1
	KSZ_REG_ADDR_WF2BM3_0  = 0x5A,		//Wakeup Frame 2 Byte Mask 3 Register Byte 0
	KSZ_REG_ADDR_WF2BM3_1  = 0x5B,		//Wakeup Frame 2 Byte Mask 3 Register Byte 1


	KSZ_REG_ADDR_WF3CRC0_0 = 0x60,		//Wakeup Frame 3 CRC0 Register Byte 0
	KSZ_REG_ADDR_WF3CRC0_1 = 0x61,		//Wakeup Frame 3 CRC0 Register Byte 1
	KSZ_REG_ADDR_WF3CRC1_0 = 0x62,		//Wakeup Frame 3 CRC1 Register Byte 0
	KSZ_REG_ADDR_WF3CRC1_1 = 0x63,		//Wakeup Frame 3 CRC1 Register Byte 1
	KSZ_REG_ADDR_WF3BM0_0  = 0x64,		//Wakeup Frame 3 Byte Mask 0 Register Byte 0
	KSZ_REG_ADDR_WF3BM0_1  = 0x65,		//Wakeup Frame 3 Byte Mask 0 Register Byte 1
	KSZ_REG_ADDR_WF3BM1_0  = 0x66,		//Wakeup Frame 3 Byte Mask 1 Register Byte 0
	KSZ_REG_ADDR_WF3BM1_1  = 0x67,		//Wakeup Frame 3 Byte Mask 1 Register Byte 1
	KSZ_REG_ADDR_WF3BM2_0  = 0x68,		//Wakeup Frame 3 Byte Mask 2 Register Byte 0
	KSZ_REG_ADDR_WF3BM2_1  = 0x69,		//Wakeup Frame 3 Byte Mask 2 Register Byte 1
	KSZ_REG_ADDR_WF3BM3_0  = 0x6A,		//Wakeup Frame 3 Byte Mask 3 Register Byte 0
	KSZ_REG_ADDR_WF3BM3_1  = 0x6B,		//Wakeup Frame 3 Byte Mask 3 Register Byte 1


	KSZ_REG_ADDR_TXCR0     = 0x70,		//Transmit Control Register Byte 0
	KSZ_REG_ADDR_TXCR1     = 0x71,		//Transmit Control Register Byte 1


	KSZ_REG_ADDR_TXSR0     = 0x72,		//Transmit Status Register Byte 0
	KSZ_REG_ADDR_TXSR1     = 0x73,		//Transmit Status Register Byte 1


	KSZ_REG_ADDR_RXCR1_0   = 0x74,		//Receive Control Register 1 Byte 0
	KSZ_REG_ADDR_RXCR1_1   = 0x75,		//Receive Control Register 1 Byte 1
	KSZ_REG_ADDR_RXCR2_0   = 0x76,		//Receive Control Register 2 Byte 0
	KSZ_REG_ADDR_RXCR2_1   = 0x77,		//Receive Control Register 2 Byte 1


	KSZ_REG_ADDR_TXMIR0    = 0x78,		//TXQ Memory Information Register Byte 0
	KSZ_REG_ADDR_TXMIR1    = 0x79,		//TXQ Memory Information Register Byte 1


	KSZ_REG_ADDR_RXFHSR0   = 0x7C,		//Receive Frame Header Status Register Byte 0
	KSZ_REG_ADDR_RXFHSR1   = 0x7D,		//Receive Frame Header Status Register Byte 1
	KSZ_REG_ADDR_RXFHBCR0  = 0x7E,		//Receive Frame Header Byte Count Register Byte 0
	KSZ_REG_ADDR_RXFHBCR1  = 0x7F,		//Receive Frame Header Byte Count Register Byte 0


	KSZ_REG_ADDR_TXQCR0    = 0x80,		//TXQ Command Register Byte 0
	KSZ_REG_ADDR_TXQCR1    = 0x81,		//TXQ Command Register Byte 1


	KSZ_REG_ADDR_RXQCR0    = 0x82,		//RXQ Command Register Byte 0
	KSZ_REG_ADDR_RXQCR1    = 0x83,		//RXQ Command Register Byte 1


	KSZ_REG_ADDR_TXFDPR0   = 0x84,		//TX Frame Data Pointer Register Byte 0
	KSZ_REG_ADDR_TXFDPR1   = 0x85,		//TX Frame Data Pointer Register Byte 1


	KSZ_REG_ADDR_RXFDPR0   = 0x86,		//RX Frame Data Pointer Register Byte 0
	KSZ_REG_ADDR_RXFDPR1   = 0x87,		//RX Frame Data Pointer Register Byte 1


	KSZ_REG_ADDR_RXDTTR0   = 0x8C,		//RX Duration Timer Threshold Register Byte 0
	KSZ_REG_ADDR_RXDTTR1   = 0x8D,		//RX Duration Timer Threshold Register Byte 1


	KSZ_REG_ADDR_RXDBCTR0  = 0x8E,		//RX Data Byte Count Threshold Register Byte 0
	KSZ_REG_ADDR_RXDBCTR1  = 0x8F,		//RX Data Byte Count Threshold Register Byte 1


	KSZ_REG_ADDR_IER0      = 0x90,		//Interrupt Enable Register Byte 0
	KSZ_REG_ADDR_IER1      = 0x91,		//Interrupt Enable Register Byte 1


	KSZ_REG_ADDR_ISR0      = 0x92,		//Interrupt Status Register Byte 0
	KSZ_REG_ADDR_ISR1      = 0x93,		//Interrupt Status Register Byte 1


	KSZ_REG_ADDR_RXFCTR0   = 0x9C,		//RX Frame Count & Threshold Register Byte 0
	KSZ_REG_ADDR_RXFCTR1   = 0x9D,		//RX Frame Count & Threshold Register Byte 1


	KSZ_REG_ADDR_TXNTFSR0  = 0x9E,		//TX Next Total Frames Size Register Byte 0
	KSZ_REG_ADDR_TXNTFSR1  = 0x9F,		//TX Next Total Frames Size Register Byte 1


	KSZ_REG_ADDR_MAHTR0_0  = 0xA0,		//MAC Address Hash Table Register 0 Byte 0
	KSZ_REG_ADDR_MAHTR0_1  = 0xA1,		//MAC Address Hash Table Register 0 Byte 1
	KSZ_REG_ADDR_MAHTR1_0  = 0xA2,		//MAC Address Hash Table Register 1 Byte 0
	KSZ_REG_ADDR_MAHTR1_1  = 0xA3,		//MAC Address Hash Table Register 1 Byte 1
	KSZ_REG_ADDR_MAHTR2_0  = 0xA4,		//MAC Address Hash Table Register 2 Byte 0
	KSZ_REG_ADDR_MAHTR2_1  = 0xA5,		//MAC Address Hash Table Register 2 Byte 1
	KSZ_REG_ADDR_MAHTR3_0  = 0xA6,		//MAC Address Hash Table Register 3 Byte 0
	KSZ_REG_ADDR_MAHTR3_1  = 0xA7,		//MAC Address Hash Table Register 3 Byte 1


	KSZ_REG_ADDR_FCLWR0    = 0xB0,		//Flow Control Low Watermark Register Byte 0
	KSZ_REG_ADDR_FCLWR1    = 0xB1,		//Flow Control Low Watermark Register Byte 1
	KSZ_REG_ADDR_FCHWR0    = 0xB2,		//Flow Control High Watermark Register Byte 0
	KSZ_REG_ADDR_FCHWR1    = 0xB3,		//Flow Control High Watermark Register Byte 1
	KSZ_REG_ADDR_FCOWR0    = 0xB4,		//Flow Control Overrun Watermark Register Byte 0
	KSZ_REG_ADDR_FCOWR1    = 0xB5,		//Flow Control Overrun Watermark Register Byte 1


	KSZ_REG_ADDR_CIDER0    = 0xC0,		//Chip ID and Enable Register Byte 0
	KSZ_REG_ADDR_CIDER1    = 0xC1,		//Chip ID and Enable Register Byte 1


	KSZ_REG_ADDR_CGCR0     = 0xC6,		//Chip Global Control Register Byte 0
	KSZ_REG_ADDR_CGCR1     = 0xC7,		//Chip Global Control Register Byte 1


	KSZ_REG_ADDR_IACR0     = 0xC8,		//Indirect Access Control Register Byte 0
	KSZ_REG_ADDR_IACR1     = 0xC9,		//Indirect Access Control Register Byte 1


	KSZ_REG_ADDR_IADLR0    = 0xD0,		//Indirect Access Data Low Register Byte 0
	KSZ_REG_ADDR_IADLR1    = 0xD1,		//Indirect Access Data Low Register Byte 1
	KSZ_REG_ADDR_IADHR0    = 0xD2,		//Indirect Access Data High Register Byte 0
	KSZ_REG_ADDR_IADHR1    = 0xD3,		//Indirect Access Data High Register Byte 1


	KSZ_REG_ADDR_PMECR0    = 0xD4,		//Power Management Event Control Register Byte 0
	KSZ_REG_ADDR_PMECR1    = 0xD5,		//Power Management Event Control Register Byte 1


	KSZ_REG_ADDR_GSWUTR0   = 0xD6,		//Go-Sleep & Wake-Up Time Register Byte 0
	KSZ_REG_ADDR_GSWUTR1   = 0xD7,		//Go-Sleep & Wake-Up Time Register Byte 1


	KSZ_REG_ADDR_PHYRR0    = 0xD8,		//PHY Reset Register Byte 0
	KSZ_REG_ADDR_PHYRR1    = 0xD9,		//PHY Reset Register Byte 1


	KSZ_REG_ADDR_P1MBCR0   = 0xE4,		//PHY 1 MII-Register Basic Control Register Byte 0
	KSZ_REG_ADDR_P1MBCR1   = 0xE5,		//PHY 1 MII-Register Basic Control Register Byte 1


	KSZ_REG_ADDR_P1MBSR0   = 0xE6,		//PHY 1 MII-Register Basic Status Register Byte 0
	KSZ_REG_ADDR_P1MBSR1   = 0xE7,		//PHY 1 MII-Register Basic Status Register Byte 1


	KSZ_REG_ADDR_PHY1ILR0  = 0xE8,		//PHY 1 PHY ID Low Register Byte 0
	KSZ_REG_ADDR_PHY1ILR1  = 0xE9,		//PHY 1 PHY ID Low Register Byte 1
	KSZ_REG_ADDR_PHY1IHR0  = 0xEA,		//PHY 1 PHY ID High Register Byte 0
	KSZ_REG_ADDR_PHY1IHR1  = 0xEB,		//PHY 1 PHY ID High Register Byte 1


	KSZ_REG_ADDR_P1ANAR0   = 0xEC,		//PHY 1 Auto-Negotiation Advertisement Register Byte 0
	KSZ_REG_ADDR_P1ANAR1   = 0xED,		//PHY 1 Auto-Negotiation Advertisement Register Byte 1
	KSZ_REG_ADDR_P1ANLPR0  = 0xEE,		//PHY 1 Auto-Negotiation Link Partner Register Ability Byte 0
	KSZ_REG_ADDR_P1ANLPR1  = 0xEF,		//PHY 1 Auto-Negotiation Link Partner Register Ability Byte 1


	KSZ_REG_ADDR_P1SCLMD0  = 0xF4,		//Port 1 PHY Special Control/Status, LinkMD® Register Byte 0
	KSZ_REG_ADDR_P1SCLMD1  = 0xF5,		//Port 1 PHY Special Control/Status, LinkMD® Register Byte 1
	KSZ_REG_ADDR_P1CR0     = 0xF6,		//Port 1 Control Register Byte 0
	KSZ_REG_ADDR_P1CR1     = 0xF7,		//Port 1 Control Register Byte 1
	KSZ_REG_ADDR_P1SR0     = 0xF8,		//Port 1 Status Register Byte 0
	KSZ_REG_ADDR_P1SR1     = 0xF9,		//Port 1 Status Register Byte 1

}KSZ8851_Registers_Addr_t;



/* Those return states of KSZ driver */
typedef uint8_t KSZ8851_Status_t;
enum
{
	KSZ_OK					= 0x00,
	KSZ_ERROR				= 0x01,
	KSZ_BUSY				= 0x02,
	KSZ_TIMEOUT				= 0x03,
	KSZ_INIT_ERROR			= 0x05,
};

typedef uint8_t KSZ8851_Reset_Type_t;
enum
{
	KSZ_RESET_TYPE_HARD			= 0x01,
	KSZ_RESET_TYPE_SOFT_QMU		= 0x02,
	KSZ_RESET_TYPE_SOFT_GLOBAL,

};

/* Structs -------------------------------------------------------------------*/

typedef struct
{
	uint32_t (*TIME_GetTick)(void);																						// function pointer for user callback. To get sys tick value to use between process.
	KSZ8851_Status_t  (*SPI_TransmitData)(uint8_t *pTxBuffer, uint16_t dataLength);										// function pointer for user callback. To transmit data (simplex) using default spi func.
	KSZ8851_Status_t  (*SPI_ReceiveData)(uint8_t *pRxBuffer, uint16_t dataLength);
	KSZ8851_Status_t  (*SPI_TransmitReceiveData)(uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint16_t dataLength);			// function pointer for user callback. To transmit and receive (full duplex) using default spi func.
	void  	 (*GPIO_Control)(uint32_t port, uint16_t pin, uint8_t pinStatus);											// function pointer for user callback. To select slave before spi comm or to control reset input of ksz8851

}KSZ8851_Callbacks_t;

typedef struct
{
#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	volatile uint32_t cs_port;
#endif

	volatile uint32_t rst_port;

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	volatile uint16_t cs_pin;
#endif

	volatile uint16_t rst_pin;

}KSZ8851_Interface_t;

typedef union
{
	volatile uint8_t bytes[KSZ_MAC_ADDRR_LEN];								// All bytes of MAC Address. byte[0] indicates MSB of MAC high address

	struct
	{
		volatile uint16_t high;												// Example MAC: 01:23:45:67:89:ab   High addr is, 0x0123.
		volatile uint16_t middle;
		volatile uint16_t low;
	}Group;

}MAC_Address_t;

typedef struct
{
	union
		{
			volatile uint16_t all;

			struct
			{
				volatile uint16_t	frame_ID					:6,
									Reserved0					:6,
									max_Collision				:1,
									late_Collision				:1,
									Reserved1					:2;
			}Bits;

		}Tx;

	union
		{
			volatile uint16_t all;

			struct
			{
				volatile uint16_t	crc_Error					:1,
									runt_Frame					:1,
									frame_Too_Long				:1,
									ethernet_Type_Frame			:1,
									MII_Error					:1,
									unicast_Frame				:1,
									multicast_Frame				:1,
									broadcast_Frame				:1,
									Reserved0					:2,
									UDP_Frame_Checksum			:1,
									TCP_Frame_Checksum			:1,
									IP_Frame_Checksum			:1,
									ICMP_Frame_Checksum			:1,
									Reserved1					:1,
									present_Frame_Valid			:1;
			}Bits;

		}Rx_Frame_Header;

	union
		{
			volatile uint16_t all;

			struct
			{
				volatile uint16_t	Reserved0					:1,
									SPI_Bus_Error				:1,						// When this bit is set, it indicates that SPI bus error status has occurred.
									energy_Detect				:1,						// When this bit is set, it depends what it indicates according to IER bit 2 and bit 1. Checkout datasheet of product.
									linkup_Detect				:1,						// When this bit is set, it indicates that wake-up from linkup detect statushas occurred.
									Rx_Magic_Packet_Detect		:1,						// When this bit is set, it indicates that Receive magic packet detect status has occurred.
									Rx_Wakeup_Fr_Detect			:1,						// When this bit is set, it indicates that Receive wakeup frame detect status has occurred.
									Tx_Space_Available			:1,						// When this bit is set, it indicates that Transmit memory space available status has occurred.
									Reserved1					:1,
									Rx_Process_Stopped			:1,						// When this bit is set, it indicates that the Receive Process has stopped
									Tx_Process_Stopped			:1,						// When this bit is set, it indicates that the Transmit Process has stopped.
									Reserved2					:1,
									Rx_Overrun					:1,						// When this bit is set, it indicates that the Receive Overrun status has occurred.
									Reserved3					:1,
									Rx_Frame_Available			:1,						// When this bit is set, it indicates that the QMU RXQ has received at least a frame from the MAC interface and the frame is ready for the host CPU to process
									Tx_Frame_Available			:1,						// When this bit is set, it indicates that the TXQ MAC has transmitted at least a frame on the MAC interface and the QMU TXQ is ready for new frames from the host
									link_Status_Changed			:1;						// When this bit is set, it indicates that the link status has changed from link up to link down, or link down to link up.
			}Bits;

		}Interrupt;

	union
		{
			volatile uint16_t all;

			struct
			{
				volatile uint16_t	extended_Reg_Capable		:1,						// When this bit is set, it indicates that extended register is capable.
									jabber_Test					:1,						// Not supported by KSZ8851
									link						:1,						// When this bit is set, it indicates that link is up.(reset: link is down)
									auto_Negotiation_Capable	:1,						// When this bit is set, it indicates that auto-negotiation capable.
									Reserved0					:1,
									auto_Negotiation_Complete	:1,						// When this bit is set, it indicates that auto-negotiation complete.
									preamble_Suppressed			:1,						// Not supported by KSZ8851
									Reserved1					:4,
									Base10_Half_Capable			:1,						// When this bit is set, it indicates that 10BASE-T half-duplex capable.
									Base10_Full_Capable			:1,						// When this bit is set, it indicates that 10BASE-T full-duplex capable.
									Base100_Half_Capable		:1,						// When this bit is set, it indicates that 100BASE-T half-duplex capable.
									Base100_Full_Capable		:1,						// When this bit is set, it indicates that 100BASE-TX full-duplex capable.
									Base100_T4_Capable			:1;						// When this bit is set, it indicates that 100BASE-T4 capable.
			}Bits;

		}PHY1_MII_Basic;

	union
		{
			volatile uint16_t all;

			struct
			{
				volatile uint16_t	partner_10BT_Half_D_Capable	:1,						// When this bit is set, it indicates that link partner 10BT half-duplex capable.
									partner_10BT_Full_D_Capable	:1,						// When this bit is set, it indicates that link partner 10BT full-duplex capable.
									partner_100BT_Half_D_Capable:1,						// When this bit is set, it indicates that link partner 100BT half-duplex capable.
									partner_100BT_Full_D_Capable:1,						// When this bit is set, it indicates that link partner 100BT full-duplex capable.
									partner_Flow_Cont_Capable	:1,						// When this bit is set, it indicates that link partner flow control (pause) capable.
									link_Is_Good				:1,						// When this bit is set, it indicates that link good.
									auto_Negotiation_Done		:1,						// When this bit is set, it indicates that auto-negotiation done.
									MDI_X						:1,						// When this bit is set, it indicates that MDI enable and MDI-X disable
									Reserved0					:1,
									operation_Duplex			:1,						// When this bit is set, it indicates that link duplex is full.
									operation_Speed				:1,						// When this bit is set, it indicates that link speed is 100 Mbps (reset: 10 Mbps)
									Reserved1					:2,
									polarity_Is_Reverse			:1,						// When this bit is set, it indicates that  polarity is reversed.
									Reserved2					:1,
									HP_Auto_MDI_X				:1;						// When this bit is set, it indicates that HP Auto MDI-X mode enabled (reset: Microchip Auto MDI-X mode.)
			}Bits;

		}Port1;

}KSZ8851_Status_Reg_t;


typedef struct
{
	volatile KSZ8851_Status_Reg_t Status;

}KSZ8851_Registers_t;

typedef struct
{
	volatile KSZ8851_Interface_t 	interface;
	volatile KSZ8851_Callbacks_t 	functions;
	volatile MAC_Address_t 			MAC_Address;
	volatile KSZ8851_Registers_t	Registers;

}KSZ8851_t;


/* Macros --------------------------------------------------------------------*/

/* Shifts cmd to frame bits 14-15 */
#define KSZ_MAKE_FRAME_CMD(frame_buff, cmd)  				frame_buff |= (cmd << KSZ_REG_CMD_SHIFT_VALUE)

/* Shifts register address to frame bits 2-9 and mask don't care bits */
#define KSZ_MAKE_FRAME_REG_ADDR(frame_buff, reg_addr)		frame_buff |= (uint16_t)(reg_addr << KSZ_REG_ADDR_BIT_SHIFT_VALUE) & KSZ_REG_ADDR_MASK_VALUE

/* Checks register address whether odd or even and if it's even selects byte 0-1 */
#define KSZ_MAKE_FRAME_REG_BYTES(frame_buff, reg_addr)		((reg_addr & KSZ_REG_ADDR_EVEN_CHECK_VALUE) != 0) ? (frame_buff |= (uint16_t)(KSZ_REG_BYTES_SELECT_2_3_MASK_VALUE << KSZ_REG_BYTES_SELECT_BIT_SHIFT_VALUE)) : \
																													(frame_buff |= (uint16_t)(KSZ_REG_BYTES_SELECT_0_1_MASK_VALUE << KSZ_REG_BYTES_SELECT_BIT_SHIFT_VALUE))
/* Byte swapping for uint16_t */
#define KSZ_BYTE_SWAP_U16(value)								(uint16_t) ((value >> KSZ_1BYTE_SHIFTING_VALUE) | (value << KSZ_1BYTE_SHIFTING_VALUE))

/* Time comparison, getting time difference and getting present time  */
#define TIMER_DIFF(a,b)     	((((int32_t)a)-((int32_t)b)))
#define TIMER_COMP(a,b)			(TIMER_DIFF((a),(b)) < 0)


//#define KSC_IS_GOOD(_ptr_)  (ptr->Register.link)

/* Variables -----------------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
//KSZ8851_Status_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t *registerValue);
//KSZ8851_Status_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t registerValue);

/* Public functions ----------------------------------------------------------*/

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
/**
* @brief  Initialize the parameters for KSZ8851 driver. This function must be called after initialization of mcu's peripherals.
* @param  driver: address of KSZ8851_Driver_Init_t struct that defined by user.
* @param  cs_port: chip select (slave select) port.
* @param  cs_pin: chip select (slave select) pin number.
* @param  rst_port: KSZ8851 hardware reset control port.
* @param  rst_pin: KSZ8851 hardware reset control pin.
* @param  MAC_address: Ethernet MAC address of the device.
* @param  callbacks: the calbback funtions defined by user in MCU layer to send data over spi and to control mcu gpio 's (chip select and hardware reset)
* @retval status of init process
*/
KSZ8851_Status_t ksz8851_init(KSZ8851_t *driver, uint32_t cs_port, uint16_t cs_pin,
		uint32_t rst_port, uint16_t rst_pin, uint8_t *MAC_address, KSZ8851_Callbacks_t callbacks);
#else
/**
* @brief  Initialize the parameters for KSZ8851 driver. This function must be called after initialization of mcu's peripherals.
* @param  driver: address of KSZ8851_Driver_Init_t struct that defined by user.
* @param  rst_port: KSZ8851 hardware reset control port.
* @param  rst_pin: KSZ8851 hardware reset control pin.
* @param  MAC_address: Ethernet MAC address of the device.
* @param  callbacks: the calbback funtions defined by user in MCU layer to send data over spi and to control mcu gpio 's (chip select and hardware reset)
* @retval status of init process
*/
KSZ8851_Status_t ksz8851_init(KSZ8851_t *driver, uint32_t rst_port, uint16_t rst_pin, uint8_t *MAC_address, KSZ8851_Callbacks_t callbacks);
#endif


#ifdef __cplusplus
}
#endif

#endif /* __KSZ8851_H */
