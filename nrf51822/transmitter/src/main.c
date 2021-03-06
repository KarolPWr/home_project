/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/** @file
*
* @defgroup nrf_dev_button_radio_tx_example_main main.c
* @{
* @ingroup nrf_dev_button_radio_tx_example
*
* @brief Radio Transceiver Example Application main file.
*
* This file contains the source code for a sample application using the NRF_RADIO peripheral.
*
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "radio_config.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_error.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

#include "dht22.h"
#include "utils.h"

#define APP_TIMER_PRESCALER      0                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE  2                           /**< Size of timer operation queues. */

static uint32_t                   packet;                    /**< Packet to transmit. */

uint8_t dht22_port = 0; 
uint8_t dht22_pin = 19;  // CHANGE


#define NRF_LOG_BACKEND_SERIAL_USES_UART 1

#undef NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE
#define NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE 115200

// #define NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE 9600
// #define NRF_LOG_BACKEND_SERIAL_UART_TX_PIN 9
// #define NRF_LOG_BACKEND_SERIAL_UART_RX_PIN 11

/**@brief Function for sending packet.
 */
void send_packet()
{
    // send the packet:
    NRF_RADIO->EVENTS_READY = 0U;
    NRF_RADIO->TASKS_TXEN   = 1;

    while (NRF_RADIO->EVENTS_READY == 0U)
    {
        // wait
    }
    NRF_RADIO->EVENTS_END  = 0U;
    NRF_RADIO->TASKS_START = 1U;

    while (NRF_RADIO->EVENTS_END == 0U)
    {
        // wait
    }

    // uint32_t err_code = bsp_indication_set(BSP_INDICATE_SENT_OK);
    NRF_LOG_INFO("The packet was sent\r\n");
    // APP_ERROR_CHECK(err_code);

    NRF_RADIO->EVENTS_DISABLED = 0U;
    // Disable radio
    NRF_RADIO->TASKS_DISABLE = 1U;

    while (NRF_RADIO->EVENTS_DISABLED == 0U)
    {
        // wait
    }
}


/**@brief Function for initialization oscillators.
 */
void clock_initialization()
{
    /* Start 16 MHz crystal oscillator */
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;

    /* Wait for the external oscillator to start up */
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
    {
        // Do nothing.
    }

    /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}


/**
 * @brief Function for application main entry.
 * @return 0. int return type required by ANSI/ISO standard.
 */
int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    clock_initialization();
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Transmitter started\r\n");

    uint8_t hum_int, hum_dec,temp_int, temp_dec;
    int16_t RH = 0, TEMP = 0;
    division temper;
    

    // Set radio configuration parameters
    radio_configure();

    // Set payload pointer
    NRF_RADIO->PACKETPTR = (uint32_t)&packet;

    //uint8_t counter =0;
    while (true)
    {
            DHT22_start ();
            check_response ();
            hum_int = read_data ();
            hum_dec = read_data ();
            temp_int = read_data ();
            temp_dec = read_data ();
            // checksum = read_data ();

            /* Merge integral and decimal bytes together */
            TEMP = ((temp_int<<8)|(temp_dec & 0xff));
			RH = ((hum_int<<8)|(hum_dec & 0xff));

            // TEMP = ROUNDED_DIV(TEMP, 10); //TODO make it divide using integers also test for negative
            RH = ROUNDED_DIV(RH, 10);

            temper = divide(TEMP, 10);




            NRF_LOG_INFO("TEMP: %i.%i RH: %i\r\n", temper.quotient, temper.remainder, RH);

            packet = TEMP;
            send_packet();
            NRF_LOG_INFO("The contents of the package was %u\r\n", (unsigned int)packet);
            nrf_delay_ms(1000);
            //++counter;
            
        
        NRF_LOG_FLUSH();
        // __WFE();
    }
}


/**
 *@}
 **/