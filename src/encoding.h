#pragma once

// Use somente para valores sem sinal (registradores rs, rt e rd, etc)
unsigned int binaryToUnsignedInt(const char *serializedBinary);

void extendSignal(char *baseSerializedBinary, char *buffer, int size);

int complementOfTwoToInt(const char *serializedBinary);
