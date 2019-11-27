#include "utils.h"

division divide(int16_t x, int16_t y)
{

    division div = {0};

	// handle divisibility by 0
	if (y == 0)
	{
		div.err = 1;
        return div;
	}

	// store sign of the result
	int16_t sign = 1;
	if (x * y < 0)
		sign = -1;

	// convert both dividend and divisor to positive
	x = abs(x), y = abs(y);

	// initialize quotient by 0
	int16_t quotient = 0;

	// loop till dividend x is more than the divisor y
	while (x >= y)
	{
	   x = x - y;		// perform reduction on dividend
	   quotient++;		// increase quotient by 1
	}

    div.quotient = sign*quotient;
    div.remainder = x; //i guess thats a remainder? 

	return div;
}