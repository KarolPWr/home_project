#include "dht22.h"

// void DHT22_start (void)
// {
// 	set_gpio_output ();  // set the pin as output
// 	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 0);   // pull the pin low
// 	DWT_Delay_us (500);   // wait for 500us
// 	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 1);   // pull the pin high
// 	DWT_Delay_us (30);   // wait for 30us
// 	set_gpio_input ();   // set as input
// }

// void check_response (void)
// {
// 	DWT_Delay_us (40);  
// 	if (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1))) 
// 	{
// 		DWT_Delay_us (80);
// 		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1))) check = 1;
// 	}
// 	while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go low
// }

void DHT22_start(void)
{
    nrf_gpio_cfg_output(DHT22_PIN);
    nrf_gpio_pin_clear(DHT22_PIN);
    nrf_delay_us(500);
    nrf_gpio_set(DHT22_PIN);
    nrf_delay_us(30);
    nrf_gpio_cfg_input(DHT22_PIN);
}

uint8_t check_response(void)
{
    uint8_t check = 0;
    nrf_delay_us(40);
    if (!nrf_gpio_pin_read(DHT22_PIN))
    {
        nrf_delay_us(80);
        if(nrf_gpio_pin_read(DHT22_PIN)) check = 1;
    }

    while(nrf_gpio_pin_read(DHT22_PIN));  // wait for the pin to go low
    return check;
}

// uint8_t read_data (void)
// {
// 	uint8_t i,j;
// 	for (j=0;j<8;j++)
// 	{
// 		while (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));   // wait for the pin to go high
// 		DWT_Delay_us (40);   // wait for 40 us
// 		if ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)) == 0)   // if the pin is low 
// 		{
// 			i&= ~(1<<(7-j));   // write 0
// 		}
// 		else i|= (1<<(7-j));  // if the pin is high, write 1
// 		while ((HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1)));  // wait for the pin to go low
// 	}
// 	return i;
// }

uint8_t read_data(void)
{
    uint8_t i,j;
    for(j=0; j<8; ++j)
    {
        while(!nrf_gpio_pin_read(DHT22_PIN));  // wait for the pin to go high
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(DHT22_PIN) == 0)   // if the pin is low 
        {
            i&= ~(1<<(7-j));  // write 0
        }
        else i|= (1<<(7-j));   // if the pin is high, write 1
        while(nrf_gpio_pin_read(DHT22_PIN)); // wait for the pin to go low
    }
    return i;

    }
