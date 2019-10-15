#ifndef __DHT_22
#define __DHT_22

/* Library for AM2302 (DHT22) module */

#include "nrf_gpio.h"
#include "nrf_delay.h"

extern uint8_t dht22_port; 
extern uint8_t dht22_pin;  // change later

#define DHT22_PIN   NRF_GPIO_PIN_MAP(dht22_port,dht22_pin) 


/* Functions */
void DHT22_start (void);
uint8_t check_response (void);
uint8_t read_data (void);

















#endif