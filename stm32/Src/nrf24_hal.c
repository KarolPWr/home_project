/*
 * nrf24_hal.c
 *
 *  Created on: Aug 1, 2019
 *      Author: Karol
 */

#include "nrf24_hal.h"
#include "stm32f3xx_hal.h"


// Low level SPI transmit/receive function (hardware depended)
// input:
//   data - value to transmit via SPI
// return: value received from SPI
uint8_t nRF24_LL_RW(uint8_t data) {

	uint8_t pRxData = 0;
	HAL_SPI_TransmitReceive(nRF24_SPI_PORT, &data, &pRxData, 1, 1000);
	return pRxData;


}
