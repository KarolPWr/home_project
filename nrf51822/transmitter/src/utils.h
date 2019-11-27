#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>

typedef struct division
{
    int16_t quotient;
    int16_t remainder;
    uint8_t err;
}division;

division divide(int16_t x, int16_t y);

#endif /* UTILS_H */