#pragma once

#include <stdint.h>

typedef enum {
    POSITIVE = 1, NEGATIVE = -1
} Signal;

// Use somente para valores sem sinal (registradores rs, rt e rd, etc)
unsigned int binaryToUnsignedInt(const char *serializedBinary);

// Se o valor binário estiver em complemento de dois, desfazer antes de chamar essa função.
// Use para valores que foram desfeitos no complemento de dois e passe o sinal como parâmetro.
inline int binaryToInt(const char *serializedBinary, const Signal signal) {
    return (int) signal * (int) binaryToUnsignedInt(serializedBinary);
}


void extendSignal(char *baseSerializedBinary, char *buffer, int size);

int complementOfTwoToInt(char *serializedBinary);
