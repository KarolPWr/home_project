/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nrf24.h"
#include "dbg.h"
#include "uart_msg.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SPI_CE GPIO_PIN_2    // portb
#define SPI_NSS GPIO_PIN_11    // portb
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void test_debug(void)
{
	uint8_t number = 24;
	  debug_printf("So this works\r\n");
	  debug_printf("So %s this works\r\n", number);


	  debug("Basic debug works\r");
	  log_err("Error logging works\r");
	  log_info("Info logging works\r");
	  log_warn("Warning logging works\r");

	  sentinel("NEVER SHOULD HAVE COME HERE\r");

	  uint8_t *test_ptr = malloc(sizeof(uint8_t));
	  check(test_ptr, "FAILED");
	  *test_ptr = NULL;
	  check(test_ptr, "SUCC");

	  error:
	  	  while(1);
}

void test_spi_read_write(void)
{
	  HAL_GPIO_WritePin(GPIOB, SPI_CE, GPIO_PIN_RESET);
	  uint8_t check = 1;
	  uint8_t TxBuf[] = {0x30, 'g', 'r', 'f', 2, 4};
	  uint8_t RxBuf[] = {0,0,0,0, 0,0};
	  check = nRF24_Check();   // this returns zero (fails)

	  nRF24_CSN_L();
	  if (HAL_SPI_TransmitReceive(&hspi2, TxBuf, RxBuf, 6, 1000) != HAL_OK)   //this is working OK
		  Error_Handler();
	  nRF24_CSN_H();

	  HAL_GPIO_WritePin(GPIOB, SPI_CE, GPIO_PIN_SET);


	  // finish write reg part -------------------------------------------------------------------------------

	  uint8_t TxBuf2[] = {0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	  uint8_t RxBuf2[] = {0,0,0,0, 0,0};

	  uint8_t status2 = 1;
	  HAL_GPIO_WritePin(GPIOB, SPI_CE, GPIO_PIN_RESET);

	  //status2 = nRF24_Check();   // this returns zero (fails)

	  nRF24_CSN_L();
	  if (HAL_SPI_TransmitReceive(&hspi2, TxBuf2, RxBuf2, 6, 1000) != HAL_OK)   //this is working OK
		  Error_Handler();
	  nRF24_CSN_H();


	  HAL_GPIO_WritePin(GPIOB, SPI_CE, GPIO_PIN_SET);
}


int send_packet(void)
{
	//config on rpi
//	STATUS           = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
//	RX_ADDR_P0-1     = 0xe7e7e7e7e7 0x3130303030
//	RX_ADDR_P2-5     = 0xc3 0xc4 0xc5 0xc6
//	TX_ADDR          = 0xe7e7e7e7e7
//	RX_PW_P0-6       = 0x00 0x20 0x00 0x00 0x00 0x00
//	EN_AA            = 0x3f
//	EN_RXADDR        = 0x02
//	RF_CH            = 0x4c
//	RF_SETUP         = 0x07
//	CONFIG           = 0x0e
//	DYNPD/FEATURE    = 0x00 0x00
//	Data Rate        = 1MBPS
//	Model            = nRF24L01+
//	CRC Length       = 16 bits
//	PA Power         = PA_MAX

	uint8_t ADDR[] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 }; // the TX address
	nRF24_DisableAA(0xFF); // disable ShockBurst
	nRF24_SetRFChannel(0x4c); // set RF channel to 2490MHz
	nRF24_SetDataRate(nRF24_DR_1Mbps); // 2Mbit/s data rate
	nRF24_SetCRCScheme(nRF24_CRC_2byte); // 1-byte CRC scheme
	nRF24_SetAddrWidth(5); // address width is 5 bytes
	nRF24_SetTXPower(nRF24_TXPWR_0dBm); // configure TX power
	nRF24_SetAddr(nRF24_PIPETX, ADDR); // program TX address
	nRF24_SetOperationalMode(nRF24_MODE_TX); // switch transceiver to the TX mode
	nRF24_SetPowerMode(nRF24_PWR_UP); // wake-up transceiver (in case if it sleeping)

	uint8_t pBuf[] = {'H', 'e'};
	uint8_t status;
	nRF24_WritePayload(pBuf, 2); // transfer payload data to transceiver
	nRF24_CE_H(); // assert CE pin (transmission starts)
	while (1) {
	    status = nRF24_GetStatus();
	    if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
	        // transmission ended, exit loop
	        break;
	    }
	}
	nRF24_CE_H(); // de-assert CE pin (nRF24 goes to StandBy-I mode)
	nRF24_ClearIRQFlags(); // clear any pending IRQ flags
	if (status & nRF24_FLAG_MAX_RT) {
	    // Auto retransmit counter exceeds the programmed maximum limit (payload in FIFO is not removed)
	    // Also the software can flush the TX FIFO here...
	    return 0;
	}
	if (status & nRF24_FLAG_TX_DS) {
	    // Successful transmission
	    return 1;
	}
	// In fact that should not happen
	return -4;
}
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
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_StatusTypeDef error;

	  log_info("System started\r");
	  if (!nRF24_Check())
	  {
		  log_err("Check failed \r");
		  log_info("Retrying...");
		  HAL_Delay(500); //delay just to make sure
		  if (!nRF24_Check())
		  {
			  log_err("Check failed again \r");
			  Error_Handler();
		  }
		  else
		  log_info("Check successful after retry\r");

	  }
	  else
		  log_info("Check successful\r");


	  HAL_Delay(500);

	  nRF24_Init();

	  log_info("System running\r");
	  send_packet();
	  HAL_Delay(1000);
	  nRF24_dump_config();


	  while(1)
	  {

	  }





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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_11, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB2 PB11 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	log_err("Error occurred, restart system!");
	while(1);
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
