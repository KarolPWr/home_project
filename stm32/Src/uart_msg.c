/*
 * uart_msg.c
 *
 *  Created on: Aug 11, 2019
 *      Author: Karol
 */

#include "stm32f3xx_hal.h"
#include "uart_msg.h"
#include <stdarg.h>

void my_print(uint8_t *string)
{
	HAL_StatusTypeDef error;
	while( *string != '\0')
	{
	error = HAL_UART_Transmit(&huart2, string++, sizeof(uint8_t), HAL_MAX_DELAY);
	}
}

HAL_StatusTypeDef my_print_char(uint8_t *character)
{
	HAL_StatusTypeDef error;
	error = HAL_UART_Transmit(&huart2, character, sizeof(uint8_t), HAL_MAX_DELAY);
	return error;
}


void debug_printf(uint8_t* format,...)
{
	uint8_t *traverse;
	uint8_t i;
	uint8_t *s;

	//Module 1: Initializing Myprintf's arguments
	va_list arg;
	va_start(arg, format);

	for(traverse = format; *traverse != '\0'; traverse++)
	{
		while( *traverse != '%' )
		{
			if(*traverse == '\0')
				goto end;
			//need to add check for string end!!!!
			my_print_char(traverse);
			traverse++;
		}

		traverse++;

		//Module 2: Fetching and executing arguments
		switch(*traverse)
		{
			case 'c' : i = va_arg(arg,int);		//Fetch char argument
						my_print(i);
						break;

			case 'd' : i = va_arg(arg,int); 		//Fetch Decimal/Integer argument
						if(i<0)
						{
							i = -i;
							my_print('-');
						}
						my_print(convert(i,10));
						break;

			case 'o': i = va_arg(arg,uint8_t); //Fetch Octal representation
						my_print(convert(i,8));
						break;

			case 's': s = va_arg(arg,uint8_t *); 		//Fetch string
						my_print(s);
						break;

			case 'x': i = va_arg(arg,int); //Fetch Hexadecimal representation
						my_print(convert(i,16));
						break;
		}
	}

	//Module 3: Closing argument list to necessary clean-up
	end:
	va_end(arg);
}

uint8_t *convert(uint8_t num, uint8_t base)
{
	static uint8_t Representation[]= "0123456789ABCDEF";
	static uint8_t buffer[50];
	uint8_t *ptr;

	ptr = &buffer[49];
	*ptr = '\0';

	do
	{
		*--ptr = Representation[num%base];
		num /= base;
	}while(num != 0);

	return(ptr);
}
