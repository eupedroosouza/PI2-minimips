#pragma once
#include "types.h"

extern Instruction emptyInstruction;

// Use somente para valores sem sinal (registradores rs, rt e rd, etc)
uint8_t binaryToUnsignedInt(const char *serializedBinary);

void extendSignal(char *baseSerializedBinary, char *buffer, int size);

int8_t complementOfTwoToInt(const char *serializedBinary);