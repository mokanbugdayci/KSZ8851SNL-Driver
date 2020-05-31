/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "ksz8851.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MCU_UART_CLK_ENABLE()		__HAL_RCC_USART1_CLK_ENABLE()
#define MCU_UART_PORT_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint8_t spi_buffer_tx[32] = {"TEST DATA SENT FROM MASTER"};
uint16_t kszRegVal = 0, MAC_low = 0, MAC_mid = 0, MAC_high = 0;
uint8_t spi_buffer_tx[32] = {0};
uint8_t spi_buffer_rx[32] = {0};
uint8_t	uart_buffer_tx[32] = {0};
uint8_t spi_return = 0;
KSZ8851_t ksz8851;
uint8_t mac[KSZ_MAC_ADDRR_LEN] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};
uint8_t spi_status1, spi_status2, spi_status3;
uint8_t counter = 0;
//void *test;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
uint32_t callback_ksz8851_GetTick(void);
uint8_t callback_ksz8851_SPI_Transmit(uint8_t *pTxBuffer, uint16_t dataLenght);
uint8_t callback_ksz8851_SPI_TransmitReceive(uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint16_t dataLenght);
void 	callback_ksz8851_GPIO_Control(uint32_t port, uint16_t pin, uint8_t pinStatus);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

KSZ8851_Callbacks_t callbacks = {callback_ksz8851_GetTick,
								 callback_ksz8851_SPI_Transmit,
								 callback_ksz8851_SPI_TransmitReceive,
								 callback_ksz8851_GPIO_Control};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
//  printf("\e[94m\r\n");

  HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_RESET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_SET);

//  HAL_SetTickFreq(HAL_TICK_FREQ_10HZ);

  ksz8851_init(&ksz8851, (uint32_t)SPI1_CS_GPIO_Port, SPI1_CS_Pin, (uint32_t)ETH_NRST_GPIO_Port, ETH_NRST_Pin, mac, callbacks);
//  printf("MAC Addrr: %x %x %x %x %x %x \r\n", ksz8851.MAC_address[0], ksz8851.MAC_address[1],
//		  	  	  	  	  	  	  	  	  	  ksz8851.MAC_address[2], ksz8851.MAC_address[3],
//											  ksz8851.MAC_address[4], ksz8851.MAC_address[5]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == false)
	  {
		  if(ksz8851_read_register(&ksz8851, KSZ_REG_ADDR_CIDER0, &kszRegVal) == 0)
		  {

				  ksz8851_read_register(&ksz8851, KSZ_REG_ADDR_CIDER0, &kszRegVal);
				  printf("\r\nKSZ8851 CHIP ID: %x\r\n", kszRegVal);

				  printf("WRITE NEW MAC ADDRESSES: LOW: %x MIDDLE: %x HIGH: %x\r\n", ksz8851.MAC_Address.Group.low, ksz8851.MAC_Address.Group.middle, ksz8851.MAC_Address.Group.high);
				  spi_status1 = ksz8851_write_register(&ksz8851, KSZ_REG_ADDR_MARL0, ksz8851.MAC_Address.Group.low);
				  spi_status1 = ksz8851_write_register(&ksz8851, KSZ_REG_ADDR_MARL0, ksz8851.MAC_Address.Group.low);
				  spi_status2 = ksz8851_write_register(&ksz8851, KSZ_REG_ADDR_MARM0, ksz8851.MAC_Address.Group.middle);
				  spi_status3 = ksz8851_write_register(&ksz8851, KSZ_REG_ADDR_MARH0, ksz8851.MAC_Address.Group.high);

				  ksz8851_read_register(&ksz8851, KSZ_REG_ADDR_MARL0, &MAC_low);
				  printf("NEW MAC ADDR LOW: %x, SPI STATUS: %d\r\n", MAC_low, spi_status1);
				  ksz8851_read_register(&ksz8851, KSZ_REG_ADDR_MARM0, &MAC_mid);
				  printf("NEW MAC ADDR MIDDLE: %x, SPI STATUS: %d\r\n", MAC_mid, spi_status2);
				  ksz8851_read_register(&ksz8851, KSZ_REG_ADDR_MARH0, &MAC_high);
				  printf("NEW MAC ADDR HIGH: %x, SPI STATUS: %d\r\n", MAC_high, spi_status3);

				  if(ksz8851.MAC_Address.Group.low != MAC_low)
				  {
					  printf("WRITING ERROR MAC ADDR LOW! Current: %x, Desired: %x , SPI Comm Status: %d\r\n", MAC_low, ksz8851.MAC_Address.Group.low, spi_status1);
				  }

				  if(ksz8851.MAC_Address.Group.middle != MAC_mid)
				  {
					  printf("WRITING ERROR MAC ADDR MIDDLE! Current: %x, Desired: %x , SPI Comm Status: %d\r\n", MAC_mid, ksz8851.MAC_Address.Group.middle, spi_status2);
				  }

				  if(ksz8851.MAC_Address.Group.high != MAC_high)
				  {
					  printf("WRITING ERROR MAC ADDR HIGH! Current: %x, Desired: %x , SPI Comm Status: %d\r\n", MAC_high, ksz8851.MAC_Address.Group.high, spi_status3);
				  }

				  ksz8851.MAC_Address.Group.low += 1;
				  ksz8851.MAC_Address.Group.middle += 1;
				  ksz8851.MAC_Address.Group.high += 1;

				  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

				  HAL_Delay(500);
		  }
		  else
		  {
			  printf("SPI MASTER TX COMM ERROR\r\n");
		  }

		  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

		  while(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == false);
		  memset(spi_buffer_rx, 0, sizeof(spi_buffer_rx));
	  }

	  HAL_Delay(200);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint8_t mcu_UartExt_Write(uint8_t* pData, uint16_t Size, uint32_t Timeout)
{
	MCU_UART_CLK_ENABLE();
	MCU_UART_PORT_CLK_ENABLE();
//	return 1;
	return (uint8_t)HAL_UART_Transmit(&huart1, pData, Size, Timeout);
}


int _write(int file, char *data, int len)
{
	UNUSED(file);
	uint8_t status = mcu_UartExt_Write((uint8_t*)data, len, 300);
	// return # of bytes written - as best we can tell
	return (status == HAL_OK ? len : 0);
}

//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
//	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//	printf("RECEIVED SPI DATA [MASTER]: %x %x %x %x\r\n", spi_buffer_rx[3], spi_buffer_rx[2], spi_buffer_rx[1], spi_buffer_rx[0]);
//}

uint32_t callback_ksz8851_GetTick(void)
{
	return HAL_GetTick();
}

uint8_t callback_ksz8851_SPI_Transmit(uint8_t *pTxBuffer, uint16_t dataLenght)
{
	uint8_t retStatus = HAL_SPI_Transmit(&hspi1, pTxBuffer, dataLenght, 4000);
//	HAL_Delay(1);
	printf("callback_ksz8851_transmit(txBuff: %x %x %x %x, length: %d)\r\n", *(pTxBuffer), *(pTxBuffer+1), *(pTxBuffer+2), *(pTxBuffer+3), dataLenght);
	return retStatus;
}

uint8_t callback_ksz8851_SPI_TransmitReceive(uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint16_t dataLenght)
{
	uint8_t retStatus = HAL_SPI_TransmitReceive(&hspi1, pTxBuffer, pRxBuffer, dataLenght, 4000);
	printf("callback_ksz8851_transmitReceive(rxBuff: %x %x %x %x, length: %d)\r\n", *(pRxBuffer), *(pRxBuffer+1), *(pRxBuffer+2), *(pRxBuffer+3), dataLenght);
//	HAL_Delay(1);
	return retStatus;
}

void callback_ksz8851_GPIO_Control(uint32_t port, uint16_t pin, uint8_t pinStatus)
{
	HAL_GPIO_WritePin((GPIO_TypeDef *)port, pin, pinStatus);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
