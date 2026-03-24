#include "encoding.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"


unsigned int binaryToUnsignedInt(const char *serializedBinary) {
    unsigned int decimal = 0;
    const size_t tamanho = strlen(serializedBinary);

    for (int i = 0; i < tamanho; i++) {
        if (serializedBinary[i] == '1') {
            decimal |= (1 << (tamanho - 1 - i));
        }
    }
    return decimal;
}

void extendSignal(char *baseSerializedBinary, char *buffer, const int size) {
    const size_t originalBinarySize = strlen(baseSerializedBinary);
    if (originalBinarySize > size) {
        println("Você está tentando extender um sinal para um tamanho menor do que seu sinal original: %s (%d) -> %d.",
                originalBinarySize, baseSerializedBinary, size);
        return;
    }
    const char firstCharacter = baseSerializedBinary[0];
    int i = 0;
    for (; i < size - originalBinarySize; i++) {
        buffer[i] = firstCharacter;
    }
    buffer[i] = '\0';
    strcat(buffer, baseSerializedBinary);
}

int complementOfTwoToInt(const char *serializedBinary) {
    const size_t size = strlen(serializedBinary);

    const char firstCharacter = serializedBinary[0];
    switch (firstCharacter) {
        case '0': {
            return (int) binaryToUnsignedInt(serializedBinary);
        }
        case '1': {
            char binary[size + 1];
            subtractOneOnBinary(serializedBinary, binary);


            char invertedBinary[size + 1];
            invertBinary(binary, invertedBinary);

            const unsigned int value = binaryToUnsignedInt(invertedBinary);
            return (-1) * (int) value;
        }

        default: {
            return -1;
        }
    }
}
