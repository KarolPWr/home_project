/*
 * nrf24_hal.h
 *
 *  Created on: Aug 1, 2019
 *      Author: Karol
 */

#ifndef NRF24_HAL_H_
#define NRF24_HAL_H_

#ifndef __NRF24_HAL_H
#define __NRF24_HAL_H


// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions
#include "stm32f3xx_hal.h"
extern SPI_HandleTypeDef hspi2; //so its not undefined. Defined for the first time in main.c

// SPI port peripheral
#define nRF24_SPI_PORT             &hspi2


// CE (chip enable) pin (PB2)
#define nRF24_CE_PORT              GPIOB
#define nRF24_CE_PIN               GPIO_PIN_2
#define nRF24_CE_L()               HAL_GPIO_WritePin(nRF24_CE_PORT, nRF24_CE_PIN, GPIO_PIN_RESET);
#define nRF24_CE_H()               HAL_GPIO_WritePin(nRF24_CE_PORT, nRF24_CE_PIN, GPIO_PIN_SET);

// CSN (chip select negative) pin (PB11)
#define nRF24_CSN_PORT             GPIOB
#define nRF24_CSN_PIN              GPIO_PIN_11
#define nRF24_CSN_L()              HAL_GPIO_WritePin(nRF24_CSN_PORT, nRF24_CSN_PIN, GPIO_PIN_RESET);
#define nRF24_CSN_H()              HAL_GPIO_WritePin(nRF24_CSN_PORT, nRF24_CSN_PIN, GPIO_PIN_SET);

//// IRQ pin (PB10)
//#define nRF24_IRQ_PORT             GPIOB
//#define nRF24_IRQ_PIN              GPIO_Pin_10


// Function prototypes
uint8_t nRF24_LL_RW(uint8_t data);

#endif // __NRF24_HAL_H

#endif /* NRF24_HAL_H_ */
