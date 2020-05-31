 /******************************************************************************
 * @filename	: 	ksz8851.h
 * @description : 	This file provides commands and functions to
 * 					communicate KSZ8851SNL ethernet controller.
 * @author      : 	M.Okan BUĞDAYCI
 * @author		: 	Mustafa KOCAOĞLU
 * @copyright   : 	Umbrella Inc.
 * @date        : 	01.05.2020
 * @revision	: 	v.1.0.0 - Driver files created
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "ksz8851.h"

/* Variables -----------------------------------------------------------------*/

/* Private functions prototypes ----------------------------------------------*/

//static uint8_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t *registerValue);
//static uint8_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t registerValue);

/* Private functions ---------------------------------------------------------*/

/**
* @brief Reads internal I/O registers of KSZ8851SNL.
* @param driver: address of KSZ8851_t struct that contains all driver params.
* @param register: address of internal register.
* @param registerValue:
* @return status of process
*/
//static uint8_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t *registerValue)
uint8_t ksz8851_read_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t *registerValue)
{
	uint32_t tickStart;
	uint16_t frameBuff = 0;
	uint8_t  cmdBuff[KSZ_REG_CMD_BUFF_SIZE] = {0};
	uint8_t  dataBuff[KSZ_REG_DATA_BUFF_SIZE] = {0};
	uint8_t  returnStatus;

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);

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
	returnStatus = driver->functions.SPI_TransmitReceiveData(cmdBuff, dataBuff, KSZ_REG_CMD_BUFF_SIZE);

	/*Wait 1mS to be sure SPI packets send and receive completely */
	tickStart = driver->functions.TIME_GetTick();
	while(driver->functions.TIME_GetTick() - tickStart < KSZ_TIME_WAIT_1MS);

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);

	/*Order data. While data is transferred in the MSB first mode in the SPI cycle, byte0 is the first byte to appear and the byte 3 is the last byte for the data phase.*/
	*registerValue = (uint16_t)((dataBuff[KSZ_REG_BUFF_BYTE3] << KSZ_1BYTE_SHIFTING_VALUE) | dataBuff[KSZ_REG_BUFF_BYTE2]);

	return returnStatus;
}

/**
* @brief  Writes internal I/O registers of KSZ8851SNL.
* @param  driver: address of KSZ8851_t struct that contains all driver params.
* @param  register: address of internal register.
* @retval status of process
*/
//static uint8_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t registerValue)
uint8_t ksz8851_write_register(KSZ8851_t *driver, KSZ8851_Registers_t registerAddr, uint16_t registerValue)
{
	uint32_t tickStart;
	uint16_t frameBuff = 0;
	uint8_t  cmdBuff[KSZ_REG_CMD_BUFF_SIZE] = {0};
	uint8_t  returnStatus;

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_RESET);

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
	returnStatus = driver->functions.SPI_TransmitData(cmdBuff, KSZ_REG_CMD_BUFF_SIZE);

	/*Wait 1mS to be sure SPI packets send completely */
	tickStart = driver->functions.TIME_GetTick();
	while(driver->functions.TIME_GetTick() - tickStart < KSZ_TIME_WAIT_1MS);

	/*Make chip select output (NSS) pin low before SPI operation*/
	driver->functions.GPIO_Control(driver->interface.cs_port, driver->interface.cs_pin, KSZ_GPIO_PIN_SET);

	return returnStatus;
}

/**
 * @brief Perform KSZ8851 hardware reset
 * @param driver
 */
void ksz8851_hard_reset(KSZ8851_t *driver)
{
	uint32_t tickStart;
	/*Pull reset output of mcu LOW state. KSZ reset input is active low logic*/
	driver->functions.GPIO_Control(driver->interface.rst_port, driver->interface.rst_pin, KSZ_GPIO_PIN_RESET);

	/*Wait 1mS to be sure SPI packets send completely */
	tickStart = driver->functions.TIME_GetTick();
	while(driver->functions.TIME_GetTick() - tickStart < KSZ_TIME_WAIT_1MS);

	/*Make output HIGH*/
	driver->functions.GPIO_Control(driver->interface.rst_port, driver->interface.rst_pin, KSZ_GPIO_PIN_SET);

	tickStart = driver->functions.TIME_GetTick();
	while(driver->functions.TIME_GetTick() - tickStart < KSZ_TIME_WAIT_1MS);
}

uint8_t ksz8851_soft_reset(KSZ8851_t *driver)
{

	return 0;
}

///**
// *
// * @param value
// * @return
// */
//uint16_t ksz8851_byte16_swap(uint16_t value)
//{
//	return (value >> 8) | (value << 8);
//}

/* Public functions ----------------------------------------------------------*/

/**
* @brief  Initialize the parameters for KSZ8851 driver. This function must be called after initialization of mcu's peripherals.
* @param  controller: address of KSZ8851_Driver_Init_t struct that defined by user.
* @param  cs_port: chip select (slave select) port.
* @param  cs_pin: chip select (slave select) pin number.
* @param  rst_port: KSZ8851 hardware reset control port.
* @param  rst_pin: KSZ8851 hardware reset control pin.
* @param  MAC_address: Ethernet MAC address of the device.
* @param  callbacks: the callback functions defined by user in MCU layer to send data over SPI and to control MCU GPIO's (chip select and hardware reset)
* @retval status of init process
*/
uint8_t ksz8851_init(KSZ8851_t *driver, uint32_t cs_port, uint16_t cs_pin, uint32_t rst_port, uint16_t rst_pin, uint8_t *MAC_address, KSZ8851_Callbacks_t callbacks)
{
	uint16_t deviceID = 0;
	uint16_t tmpReadRegister;
	uint8_t tryCounter = 0;
	uint8_t returnValue = false;

	/* SPI's slave select GPIO parameters */
	driver->interface.cs_port 	= cs_port;
	driver->interface.cs_pin 	= cs_pin;

	/* KSZ8851 NRST (hard reset) input's GPIO parameters */
	driver->interface.rst_port	= rst_port;
	driver->interface.rst_pin	= rst_pin;

	/* Addresses of user callback functions to control SPI and GPIO */
	driver->functions 			= callbacks;

	memcpy((uint8_t*)driver->MAC_Address.bytes, (uint8_t*)MAC_address, KSZ_MAC_ADDRR_LEN);

	do
	{
		/* Step 1: Perform hard reset to KSZ8851SNL */
		ksz8851_hard_reset(driver);

		/* Step 2: Perform global soft reset */

		/* Step 3: Read device ID to be sure device is KSZ8851SNL */
		ksz8851_read_register(driver, KSZ_REG_ADDR_CIDER0, &deviceID);

		tryCounter++;

	/* if received value don't match, do this 3 time */
	}while(((deviceID & KSZ_CHIP_ID_MASK) != KSZ_CHIP_ID) && tryCounter <= KSZ_PROCESS_TRY_LIMIT);

	tryCounter = 0;

	/* Step 4: Write QMU MAC_Addres (low, middle, high) */
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARL0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.low));
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARM0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.middle));
	ksz8851_write_register(driver, KSZ_REG_ADDR_MARH0, KSZ_BYTE_SWAP_U16(driver->MAC_Address.Group.high));

	/* Step 5: Enable QMU transmit frame data pointer auto increment in TXFDPR */
	ksz8851_write_register(driver, KSZ_REG_ADDR_TXFDPR0, KSZ_CONFIG_TX_FR_DPOINTER_AUTO_INC);

	/* Step 6: Enable QMU Transmit flow control / Transmit padding / Transmit CRC and IP/TCP/UDP checksum generation. */
	ksz8851_write_register(driver, KSZ_REG_ADDR_TXCR0,
			KSZ_CONFIG_TX_CTRL_FLOW_ENABLE |
			KSZ_CONFIG_TX_CTRL_PAD_ENABLE |
			KSZ_CONFIG_TX_CTRL_CRC_ENABLE |
			KSZ_CONFIG_TX_CTRL_IP_CHECKSUM |
			KSZ_CONFIG_TX_CTRL_TCP_CHECKSUM |
			KSZ_CONFIG_TX_CTRL_UDP_CHECKSUM);

	/* Step 7: Enable QMU Receive Frame Data Pointer Auto Increment. */
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXFDPR0, KSZ_CONFIG_RX_FR_DPOINTER_AUTO_INC);

	/* Step 8: Configure QMU Receive Frame Threshold for one frame. */
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXFCTR0, KSZ_CONFIG_RX_FR_CTRL_THRESHOLD_1FR);

	/* Step 9: Receive unicast/multicast(all)/broadcast frames, enable rx flow control, MAC address filter, IP/TCP/UDP checksum verification */
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXCR1_0,
			KSZ_CONFIG_RX_CTRL1_RECEIVE_UNICAST |
			KSZ_CONFIG_RX_CTRL1_RECEIVE_ALL_MULTICAST |
			KSZ_CONFIG_RX_CTRL1_RECEIVE_BROADCAST |
			KSZ_CONFIG_RX_CTRL1_FLOW_ENABLE |
			KSZ_CONFIG_RX_CTRL1_MAC_FILTER |
			KSZ_CONFIG_RX_CTRL1_IP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL1_TCP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL1_UDP_CHECKSUM);

	/* Step 10: Enable QMU Receive UDP Lite frame checksum verification, UDP Lite frame checksum generation, IPv4/IPv6 UDP fragment frame pass, IPv4/IPv6 UDP UDP checksum field is zero pass, and single frame data burst */
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXCR2_0,
			KSZ_CONFIG_RX_CTRL2_ICMP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_UDP_LITE_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_IPV6_UDP_CHECKSUM |
			KSZ_CONFIG_RX_CTRL2_IPV6_UDP_NOCHECKSUM |
			KSZ_CONFIG_RX_CTRL2_DATA_BURST_FR_LEN);

	/* Step 11: Enable QMU Receive IP Header Two-Byte Offset /Receive Frame Count Threshold/RXQ Auto-Dequeue frame */
	ksz8851_write_register(driver, KSZ_REG_ADDR_RXQCR0,
			KSZ_CONFIG_RX_CMD_AUTO_DEQUEUE_RXQ |
			KSZ_CONFIG_RX_CMD_FR_COUNT_THR_INT_ENABLE |
			KSZ_CONFIG_RX_CMD_IP_TWOBYTE_OFFSET_ENABLE);

	/* Step 12: Adjusts SPI Data Output (SO) Delay according to SPI master controller configuration. Adjust pin strength */
	ksz8851_write_register(driver, KSZ_REG_ADDR_OBCR0,
			KSZ_CONFIG_ONCHIP_BUS_CLK_DIVIDEBY_1 |
			KSZ_CONFIG_ONCHIP_BUS_CLK_125MHZ |
			KSZ_CONFIG_ONCHIP_BUS_PIN_STRENG_8MA);

	/* Step 13: Restart Port 1 auto-negotiation */
	ksz8851_write_register(driver, KSZ_REG_ADDR_P1CR0, KSZ_CONFIG_PORT_AUTO_NEG_RESTART);

	/* Step 13.1: Force link in half duplex if auto-negotiation is failed (e.g. KSZ8851 is connected to the Hub) */
	ksz8851_read_register(driver, KSZ_REG_ADDR_P1CR0, &tmpReadRegister);

	if((tmpReadRegister & KSZ_CONFIG_PORT_AUTO_NEG_RESTART) != KSZ_CONFIG_PORT_AUTO_NEG_RESTART)
	{
		ksz8851_write_register(driver, KSZ_REG_ADDR_P1CR0, (tmpReadRegister | KSZ_CONFIG_PORT_FORCE_FULL_DUPLEX));		// force PHY in full duplex mdoe
	}

	/* Step 14: Clear the interrupts status */
	ksz8851_write_register(driver, KSZ_REG_ADDR_ISR0, KSZ_FLAGS_INTERRUPTS_ALL_CLEAR);

	/* Step 14.1: */
	ksz8851_write_register(driver, KSZ_REG_ADDR_FCLWR0, KSZ_CONFIG_WATERMARK_4KB);

	/* Step 14.2: */
	ksz8851_write_register(driver, KSZ_REG_ADDR_FCHWR0, KSZ_CONFIG_WATERMARK_6KB);

	/* Step 17: */

	/* Step 19: */

	/* Step 20: */

	/* Step 21: */

	/* Step 22: */




	return returnValue;
}

