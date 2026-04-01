#pragma once


#include <stdint.h>
#include "types.h"

typedef struct {
    bool zeroUla;
    int8_t value;
} ULAOut;

ULAOut ula(int8_t input1, int8_t input2, int ulaControl);
