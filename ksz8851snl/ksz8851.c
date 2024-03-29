 /******************************************************************************
 * @filename	: 	ksz8851.c
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "ksz8851.h"
#include "ksz8851_config.h"
/* Variables -----------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/

#define BUSYWAIT_UNTIL(cond, max_time)												\
  do {                                                                  			\
    volatile uint32_t t0;                                                  			\
    t0 = driver->functions.TIME_GetTick();                                                 			\
    while(!(cond) && TIMER_COMP((int32_t)driver->functions.TIME_GetTick(), (int32_t)((int32_t)t0 + ((int32_t)max_time))))__asm("nop");   	\
  } while(0)

/* Private functions prototypes ----------------------------------------------*/

/* Some specific purpose */
static bool ksz8851_is_there_reserverd_bit(KSZ8851_Registers_Addr_t registerAddr);
static void ksz8851_delayMs(KSZ8851_t *driver, uint32_t delay_time);

/* Register or TX/RX fifo operations */
static KSZ8851_Status_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t *registerValue);
static KSZ8851_Status_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t registerValue);
static KSZ8851_Status_t ksz8851_read_fifo(KSZ8851_t *driver, uint8_t *rxBuffer, uint16_t *frame_length);
static KSZ8851_Status_t ksz8851_write_fifo(KSZ8851_t *driver, uint8_t *txBuffer, uint16_t frame_length);

/* Some specific regsister operations */
static KSZ8851_Status_t ksz8851_set_registerBits(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t bit_no);
static KSZ8851_Status_t ksz8851_clear_registerBits(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t bit_no);
static KSZ8851_Status_t ksz8851_enable_interrupts(KSZ8851_t *driver);
static KSZ8851_Status_t ksz8851_disable_interrupts(KSZ8851_t *driver, uint16_t *current_reg_value);

/* Reset operations*/
static void ksz8851_hard_reset(KSZ8851_t *driver);
static KSZ8851_Status_t ksz8851_soft_reset(KSZ8851_t *driver, uint8_t soft_reset_type);

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
		uint32_t rst_port, uint16_t rst_pin, uint8_t *MAC_address, KSZ8851_Callbacks_t callbacks)
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
{
//	uint16_t tmpMAC_LOW = 0, tmpMAC_MID = 0, tmpMAC_HIGH = 0;
	uint16_t deviceID = 0;
	uint16_t tmpCurrentRegValue;
	uint8_t tryCounter = 0;
	KSZ8851_Status_t result = KSZ_OK;

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	/* SPI's slave select GPIO parameters */
	driver->interface.cs_port 	= cs_port;
	driver->interface.cs_pin 	= cs_pin;
#endif

	/* KSZ8851 NRST (hard reset) input's GPIO parameters */
	driver->interface.rst_port	= rst_port;
	driver->interface.rst_pin	= rst_pin;

	/* Addresses of user callback functions to control SPI and GPIO */
	driver->functions 			= callbacks;

	memcpy((uint8_t*)driver->MAC_Address.bytes, (uint8_t*)MAC_address, KSZ_MAC_ADDRR_LEN);

	/* Step 1: Perform hard reset to KSZ8851SNL */
	ksz8851_hard_reset(driver);


	do
	{
		/* Step 2: Read device ID to be sure device is KSZ8851SNL. if received value don't match, do this 3 time */
		ksz8851_read_register(driver, KSZ_REG_ADDR_CIDER0, &deviceID);

		tryCounter++;

	}while(((deviceID & KSZ_CHIP_ID_MASK) != KSZ_CHIP_ID) && tryCounter <= KSZ_PROCESS_TRY_LIMIT);

	if((deviceID & KSZ_CHIP_ID_MASK) != KSZ_CHIP_ID)
	{
		return KSZ_INIT_ERROR;

	}
	else
	{
		tryCounter = 0;
	}

	/* Step 3: Perform global soft reset */
	ksz8851_soft_reset(driver, KSZ_CONFIG_GLOBAL_SOFT_RESET);

	/* Step 4: Write QMU MAC_Addres (low, middle, high) */
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARL0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.low));
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARM0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.middle));
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARH0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.high));

#ifdef KSZ_INIT_USE_DEFAULT_DRIVER_SETTINGS

	/* Step 5: Enable QMU transmit frame data pointer auto increment in TXFDPR. Read current value of register and or with new value to prevent to change RESERVERD bits values */
	ksz8851_read_register(driver, KSZ_REG_ADDR_TXFDPR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_TXFDPR0, (KSZ_CONFIG_TX_FR_DPOINTER_AUTO_INC | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_TXFDPR0, &tmpCurrentRegValue);

	/* Step 6: Enable QMU Transmit flow control / Transmit padding / Transmit CRC and IP/TCP/UDP checksum generation. */
	ksz8851_read_register(driver, KSZ_REG_ADDR_TXCR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_TXCR0,
			(KSZ_CONFIG_TX_CTRL_FLOW_ENABLE |
			KSZ_CONFIG_TX_CTRL_PAD_ENABLE |
			KSZ_CONFIG_TX_CTRL_CRC_ENABLE |
			KSZ_CONFIG_TX_CTRL_IP_CHECKSUM |
			KSZ_CONFIG_TX_CTRL_TCP_CHECKSUM |
			KSZ_CONFIG_TX_CTRL_UDP_CHECKSUM | tmpCurrentRegValue));

//	ksz8851_read_register(driver, KSZ_REG_ADDR_TXCR0, &tmpCurrentRegValue);

	/* Step 7: Enable QMU Receive Frame Data Pointer Auto Increment. */
	ksz8851_read_register(driver, KSZ_REG_ADDR_RXFDPR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXFDPR0, (KSZ_CONFIG_RX_FR_DPOINTER_AUTO_INC | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_RXFDPR0, &tmpCurrentRegValue);

	/* Step 8: Configure QMU Receive Frame Threshold for one frame. */
	ksz8851_read_register(driver, KSZ_REG_ADDR_RXFCTR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXFCTR0, (KSZ_CONFIG_RX_FR_CTRL_THRESHOLD_1FR | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_RXFCTR0, &tmpCurrentRegValue);

	/* Step 9: Receive unicast/multicast(all)/broadcast frames, enable rx flow control, MAC address filter, IP/TCP/UDP checksum verification */
	ksz8851_read_register(driver, KSZ_REG_ADDR_RXCR1_0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXCR1_0,
			(KSZ_CONFIG_RX_CTRL1_RECEIVE_UNICAST |
			KSZ_CONFIG_RX_CTRL1_RECEIVE_ALL_MULTICAST |
			KSZ_CONFIG_RX_CTRL1_RECEIVE_BROADCAST |
			KSZ_CONFIG_RX_CTRL1_FLOW_ENABLE |
			KSZ_CONFIG_RX_CTRL1_MAC_FILTER |
			KSZ_CONFIG_RX_CTRL1_IP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL1_TCP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL1_UDP_CHECKSUM | tmpCurrentRegValue));

//	ksz8851_read_register(driver, KSZ_REG_ADDR_RXCR1_0, &tmpCurrentRegValue);

	/* Step 10: Enable QMU Receive UDP Lite frame checksum verification, UDP Lite frame checksum generation, IPv4/IPv6 UDP fragment frame pass, IPv4/IPv6 UDP UDP checksum field is zero pass, and single frame data burst */
	ksz8851_read_register(driver, KSZ_REG_ADDR_RXCR2_0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXCR2_0,
			(KSZ_CONFIG_RX_CTRL2_ICMP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_UDP_LITE_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_IPV6_UDP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_IPV6_UDP_NOCHECKSUM |
			KSZ_CONFIG_RX_CTRL2_DATA_BURST_FR_LEN | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_RXCR2_0, &tmpCurrentRegValue);

	/* Step 11: Enable QMU Receive IP Header Two-Byte Offset /Receive Frame Count Threshold/RXQ Auto-Dequeue frame */
	ksz8851_read_register(driver, KSZ_REG_ADDR_RXQCR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXQCR0,
			(KSZ_CONFIG_RX_CMD_AUTO_DEQUEUE_RXQ |
			KSZ_CONFIG_RX_CMD_FR_COUNT_THR_INT_ENABLE |
			KSZ_CONFIG_RX_CMD_IP_TWOBYTE_OFFSET_ENABLE | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_RXQCR0, &tmpCurrentRegValue);

	/* Step 12: Adjusts SPI Data Output (SO) Delay according to SPI master controller configuration. Adjust pin strength */
	ksz8851_read_register(driver, KSZ_REG_ADDR_OBCR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_OBCR0,
			(KSZ_CONFIG_ONCHIP_BUS_CLK_DIVIDEBY_1 |
			KSZ_CONFIG_ONCHIP_BUS_CLK_125MHZ |
			KSZ_CONFIG_ONCHIP_BUS_PIN_STRENG_8MA | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_OBCR0, &tmpCurrentRegValue);

	/* Step 13: Restart Port 1 auto-negotiation */
	ksz8851_read_register(driver, KSZ_REG_ADDR_P1CR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_P1CR0, (KSZ_CONFIG_PORT_AUTO_NEG_RESTART | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_P1CR0, &tmpCurrentRegValue);

	/* Step 13.1: Force link in half duplex if auto-negotiation is failed (e.g. KSZ8851 is connected to the Hub) */
	ksz8851_read_register(driver, KSZ_REG_ADDR_P1CR0, &tmpCurrentRegValue);

	if((tmpCurrentRegValue & KSZ_CONFIG_PORT_AUTO_NEG_RESTART) != KSZ_CONFIG_PORT_AUTO_NEG_RESTART)
	{
		ksz8851_write_register(driver, KSZ_REG_ADDR_P1CR0, (tmpCurrentRegValue | KSZ_CONFIG_PORT_FORCE_FULL_DUPLEX));		// force PHY in full duplex mdoe
//		ksz8851_read_register(driver, KSZ_REG_ADDR_P1CR0, &tmpCurrentRegValue);
	}

	/* Step 14: Clear the interrupts status */
	ksz8851_read_register(driver, KSZ_REG_ADDR_ISR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_ISR0, (KSZ_FLAGS_INTERRUPTS_ALL_CLEAR | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_ISR0, &tmpCurrentRegValue);

	/* Step 14.1: */
	ksz8851_read_register(driver, KSZ_REG_ADDR_FCLWR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_FCLWR0, (KSZ_CONFIG_WATERMARK_4KB | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_FCLWR0, &tmpCurrentRegValue);

	/* Step 14.2: */
	ksz8851_read_register(driver, KSZ_REG_ADDR_FCHWR0, &tmpCurrentRegValue);
	ksz8851_write_register(driver, KSZ_REG_ADDR_FCHWR0, (KSZ_CONFIG_WATERMARK_6KB | tmpCurrentRegValue));
//	ksz8851_read_register(driver, KSZ_REG_ADDR_FCHWR0, &tmpCurrentRegValue);

	/* Step 17: */

	/* Step 19: */

	/* Step 20: */

	/* Step 21: */

	/* Step 22: */

#endif			// KSZ_INIT_USE_DEFAULT_DRIVER_SETTINGS

	return result;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief
 * @param registerAddr
 * @return
 */
static bool ksz8851_is_there_reserverd_bit(KSZ8851_Registers_Addr_t registerAddr)
{
	bool status = true;

	/* Registers that have not any RESERVERD bit */
	switch(registerAddr)
	{
		case KSZ_REG_ADDR_MARL0:
		case KSZ_REG_ADDR_MARM0:
		case KSZ_REG_ADDR_MARH0:
		case KSZ_REG_ADDR_WF0CRC0_0:
		case KSZ_REG_ADDR_WF0CRC1_0:
		case KSZ_REG_ADDR_WF0BM0_0:
		case KSZ_REG_ADDR_WF0BM1_0:
		case KSZ_REG_ADDR_WF0BM2_0:
		case KSZ_REG_ADDR_WF0BM3_0:
		case KSZ_REG_ADDR_WF1CRC0_0:
		case KSZ_REG_ADDR_WF1CRC1_0:
		case KSZ_REG_ADDR_WF1BM0_0:
		case KSZ_REG_ADDR_WF1BM1_0:
		case KSZ_REG_ADDR_WF1BM2_0:
		case KSZ_REG_ADDR_WF1BM3_0:
		case KSZ_REG_ADDR_WF2CRC0_0:
		case KSZ_REG_ADDR_WF2CRC1_0:
		case KSZ_REG_ADDR_WF2BM0_0:
		case KSZ_REG_ADDR_WF2BM1_0:
		case KSZ_REG_ADDR_WF2BM2_0:
		case KSZ_REG_ADDR_WF2BM3_0:
		case KSZ_REG_ADDR_WF3CRC0_0:
		case KSZ_REG_ADDR_WF3CRC1_0:
		case KSZ_REG_ADDR_WF3BM0_0:
		case KSZ_REG_ADDR_WF3BM1_0:
		case KSZ_REG_ADDR_WF3BM2_0:
		case KSZ_REG_ADDR_WF3BM3_0:
		case KSZ_REG_ADDR_RXDTTR0:
		case KSZ_REG_ADDR_RXDBCTR0:
		case KSZ_REG_ADDR_RXFCTR0:
		case KSZ_REG_ADDR_TXNTFSR0:
		case KSZ_REG_ADDR_MAHTR0_0:
		case KSZ_REG_ADDR_MAHTR1_0:
		case KSZ_REG_ADDR_MAHTR2_0:
		case KSZ_REG_ADDR_MAHTR3_0:
		case KSZ_REG_ADDR_IADLR0:
		case KSZ_REG_ADDR_IADHR0:
		case KSZ_REG_ADDR_GSWUTR0:
		{
			status = false;
			break;
		}

		default:
			break;
	}

	return status;
}

/**
 * @brief
 * @param driver
 * @param delay_time
 */
static void ksz8851_delayMs(KSZ8851_t *driver, uint32_t delay_time)
{
	uint32_t tickStart = driver->functions.TIME_GetTick();
	while(driver->functions.TIME_GetTick() - tickStart < delay_time);
}

/**
* @brief Reads internal I/O registers of KSZ8851SNL.
* @param driver: address of KSZ8851_t struct that contains all driver params.
* @param register: address of internal register.
* @param *registerValue:
* @return status of process
*/
static KSZ8851_Status_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t *registerValue)
{
	uint16_t frameBuff = 0;
	uint8_t  cmdBuff[KSZ_REG_CMD_BUFF_SIZE] = {0};
	uint8_t  dataBuff[KSZ_REG_DATA_BUFF_SIZE] = {0};
	KSZ8851_Status_t  result = KSZ_OK;

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);

#endif

	/*Shift register addr to bits 9-2 and mask it to make 0 don't care bits*/
	KSZ_MAKE_FRAME_REG_ADDR(frameBuff, registerAddr);

	/* Checks register address whether odd or even and if it's even selects byte 0-1 */
	KSZ_MAKE_FRAME_REG_BYTES(frameBuff, registerAddr);

	/* Shifts cmd to frame bits 14-15 */
	KSZ_MAKE_FRAME_CMD(frameBuff, KSZ8851_READ_REG);

	/*Copy the read register frame to buffer*/
	cmdBuff[KSZ_REG_BUFF_BYTE0] = (uint8_t)((frameBuff & KSZ_REG_CMD_BYTE0_MASK) >> KSZ_1BYTE_SHIFTING_VALUE);			//fit MSB byte of cmd (16 bit) variable to one byte
	cmdBuff[KSZ_REG_BUFF_BYTE1] = (uint8_t)(frameBuff & KSZ_REG_CMD_BYTE1_MASK);										//fit LSB byte of cmd (16 bit) variable to one byte

	/*Call spi callback function to start spi tx/rx operation*/
	result = driver->functions.SPI_TransmitReceiveData(cmdBuff, dataBuff, KSZ_REG_CMD_BUFF_SIZE);

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)

	/*Wait 1mS to be sure SPI packets send and receive completely */
	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS);

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);

#endif

	/*Order data. While data is transferred in the MSB first mode in the SPI cycle, byte0 is the first byte to appear and the byte 3 is the last byte for the data phase.*/
	*registerValue = (uint16_t)((dataBuff[KSZ_REG_BUFF_BYTE3] << KSZ_1BYTE_SHIFTING_VALUE) | dataBuff[KSZ_REG_BUFF_BYTE2]);

	return result;
}

/**
* @brief  Writes internal I/O registers of KSZ8851SNL.
* @param  driver: address of KSZ8851_t struct that contains all driver params.
* @param  register: address of internal register.
* @return status of process
*/
static KSZ8851_Status_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t registerValue)
{
	uint16_t frameBuff = 0;
	uint8_t  cmdBuff[KSZ_REG_CMD_BUFF_SIZE] = {0};
	KSZ8851_Status_t  result = KSZ_OK;

	/* if The register that data will be written have some reserved register, first read after make bitwise or with default value
	 * to prevent unpredictable and fatal result (ref: KSZ8851 datasheet, section 4.2 Regsiter Map)*/
//	if(ksz8851_is_there_reserverd_bit(registerAddr))
//	{
//		if(ksz8851_read_register(driver, registerAddr, &tmpCurrentRegValue) == KSZ_OK)
//		{
//			registerValue |= tmpCurrentRegValue;
//		}
//	}

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);

#endif

	/*Shift register addr to bits 9-2 and mask it to make 0 don't care bits*/
	KSZ_MAKE_FRAME_REG_ADDR(frameBuff, registerAddr);

	/* Checks register address whether odd or even and if it's even selects byte 0-1 */
	KSZ_MAKE_FRAME_REG_BYTES(frameBuff, registerAddr);

	/* Shifts cmd to frame bits 14-15 */
	KSZ_MAKE_FRAME_CMD(frameBuff, KSZ8851_WRITE_REG);

	/*Copy the read register frame to buffer*/
	cmdBuff[KSZ_REG_BUFF_BYTE0] = (uint8_t)((frameBuff & KSZ_REG_CMD_BYTE0_MASK) >> KSZ_1BYTE_SHIFTING_VALUE);			//fit MSB byte of cmd (16 bit) variable to one byte
	cmdBuff[KSZ_REG_BUFF_BYTE1] = (uint8_t)(frameBuff & KSZ_REG_CMD_BYTE1_MASK);										//fit LSB byte of cmd (16 bit) variable to one byte
	cmdBuff[KSZ_REG_BUFF_BYTE2] = (uint8_t)(registerValue & KSZ_REG_CMD_BYTE1_MASK);									//fit LSB byte of data (16 bit) variable to one byte
	cmdBuff[KSZ_REG_BUFF_BYTE3] = (uint8_t)((registerValue & KSZ_REG_CMD_BYTE0_MASK) >> KSZ_1BYTE_SHIFTING_VALUE);		//fit MSB byte of data (16 bit) variable to one byte

	/*Call spi callback function to start spi tx operation*/
	result = driver->functions.SPI_TransmitData(cmdBuff, KSZ_REG_CMD_BUFF_SIZE);

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)

	/*Wait 1mS to be sure SPI packets send completely */
	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS);

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);

#endif

	return result;
}

/**
 * @brief
 * @param driver
 * @param rxBuffer
 * @param frame_length
 * @return result
 */
static KSZ8851_Status_t ksz8851_read_fifo(KSZ8851_t *driver, uint8_t *rxBuffer, uint16_t *frame_length)
{
	uint8_t  cmdBuff[KSZ_REG_CMD_BUFF_SIZE] = {0}, padding_length = 0;
	KSZ8851_Status_t result = KSZ_OK;

	/* The data length that read from KSZ must be DWORD aligned during read fifo operation, in here we are checking frame length whether DWORD aligned or not
	 * ref: KSZ datasheet section 3.5.6 */
	padding_length = (*frame_length & KSZ_RX_FRAME_LEN_MULTIPLE_VALUE);

	if(padding_length != 0)
	{
		padding_length = KSZ_DWORD_VALUE - padding_length;
	}

	*frame_length += padding_length;

	cmdBuff[KSZ_REG_BUFF_BYTE0] = KSZ8851_READ_RX_FIFO;

	/* Start QMU DMA transfer operation to read frame data from the RXQ to host CPU. */
	result = ksz8851_set_registerBits(driver, KSZ_REG_ADDR_RXQCR0, 3);		// todo: define bit number

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	/* Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);
#endif

	/* Call spi callback function to start spi tx operation for reading fifo command */
	result |= driver->functions.SPI_TransmitData(cmdBuff, KSZ_REG_CMD_BUFF_SIZE);	// todo: check this from another driver.

	/* Call spi callback function to start spi rx operation */
	result |= driver->functions.SPI_ReceiveData(rxBuffer, *frame_length);

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);
#endif

	/* Start QMU DMA transfer operation to read frame data from the RXQ to host CPU. */
	result |= ksz8851_clear_registerBits(driver, KSZ_REG_ADDR_RXQCR0, 3); // todo: define bit number


	return result;
}

/**
 *
 * @param driver
 * @param txBuffer
 * @param frame_length
 * @return result
 */
static KSZ8851_Status_t ksz8851_write_fifo(KSZ8851_t *driver, uint8_t *txBuffer, uint16_t frame_length)
{
	uint16_t tmpCurrentRegValue;
	KSZ8851_Status_t result = KSZ_OK;

	/* Disable all interrupts before starting fifo writing process, keep which IER content to enable current interrupts after process */
	result = ksz8851_disable_interrupts(driver, &tmpCurrentRegValue);

	/* Start QMU DMA transfer operation to read frame data from the RXQ to host CPU. */
	result |= ksz8851_set_registerBits(driver, KSZ_REG_ADDR_RXQCR0, 3);		// todo: define bit number

#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	/* Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);
#endif



#if !defined(KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER) && !defined(KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE)
	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);
#endif

	return result;
}

/**
 * @brief
 * @param driver
 * @param registerAddr
 * @param bit_no
 * @return result
 */
static KSZ8851_Status_t ksz8851_set_registerBits(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t bit_no)
{
	uint16_t tmpCurrentRegValue;
	KSZ8851_Status_t result = KSZ_OK;

	/* Read current value in the register */
	result = ksz8851_read_register(driver, registerAddr, &tmpCurrentRegValue);

	/* Set only issued bit/bits */
	tmpCurrentRegValue |= (0x0001 << bit_no);

	/* Write new register value */
	result |= ksz8851_write_register(driver, registerAddr, tmpCurrentRegValue);

	return result;
}

/**
 * @brief
 * @param driver
 * @param registerAddr
 * @param bit_no
 * @return
 */
static KSZ8851_Status_t ksz8851_clear_registerBits(KSZ8851_t *driver, KSZ8851_Registers_Addr_t registerAddr, uint16_t bit_no)
{
	uint16_t tmpCurrentRegValue;
	KSZ8851_Status_t result = KSZ_OK;

	/* Read current value in the register */
	result = ksz8851_read_register(driver, registerAddr, &tmpCurrentRegValue);

	/* Reset only issued bit/bits */
	tmpCurrentRegValue &= ~(0x0001 << bit_no);

	/* Write new register value */
	result |= ksz8851_write_register(driver, registerAddr, tmpCurrentRegValue);

	return result;
}

/**
 * @brief
 * @param driver
 * @param register_value
 * @return result
 */
static KSZ8851_Status_t ksz8851_enable_interrupts(KSZ8851_t *driver, uint16_t register_value)
{
	KSZ8851_Status_t result = KSZ_OK;

	result = ksz8851_write_register(driver, KSZ_REG_ADDR_IER0, KSZ_CONFIG_CLEAR_ALL_BITS);

	return result;
}

/**
 * @brief
 * @param driver
 * @param current_reg_value
 * @return result
 */
static KSZ8851_Status_t ksz8851_disable_interrupts(KSZ8851_t *driver, uint16_t *current_reg_value)
{
	KSZ8851_Status_t result = KSZ_OK;

	if(current_reg_value != NULL)
	{
		result = ksz8851_read_register(driver, KSZ_REG_ADDR_IER0, current_reg_value);
	}

	result |= ksz8851_write_register(driver, KSZ_REG_ADDR_IER0, KSZ_CONFIG_CLEAR_ALL_BITS);

	return result;
}

/**
 * @brief Perform KSZ8851 hardware reset
 * @param driver
 */
static void ksz8851_hard_reset(KSZ8851_t *driver)
{
	/*Pull reset output of mcu LOW state. KSZ reset input is active low logic*/
	driver->functions.GPIO_Control(driver->interface.rst_port, driver->interface.rst_pin, KSZ_GPIO_PIN_RESET);

	/*Wait 1mS to be sure SPI Reset occur completely */
	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS * 100);

	/*Make output HIGH*/
	driver->functions.GPIO_Control(driver->interface.rst_port, driver->interface.rst_pin, KSZ_GPIO_PIN_SET);

	/*Wait 1mS to be sure SPI Reset occur completely */
	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS * 200);
}

/**
 * @brief Perform KSZ8851 software reset
 * @param driver address of KSZ8851_Driver_Init_t struct that defined by user.
 * @param reset_type type of reset
 * @return process status
 */
static KSZ8851_Status_t ksz8851_soft_reset(KSZ8851_t *driver, uint8_t soft_reset_type)
{
	uint16_t tmpCurrentRegValue;
	KSZ8851_Status_t returnValue;

	returnValue = ksz8851_read_register(driver, KSZ_REG_ADDR_GRR0, &tmpCurrentRegValue);

	/* Set bits of global reset register according to soft_reset_type and than reset to terminate reset status */
	returnValue |= ksz8851_write_register(driver, KSZ_REG_ADDR_GRR0, (soft_reset_type | tmpCurrentRegValue));

	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS);

	returnValue |= ksz8851_write_register(driver, KSZ_REG_ADDR_GRR0, tmpCurrentRegValue);

	ksz8851_delayMs(driver, KSZ_TIME_WAIT_1MS);

	return returnValue;
}

