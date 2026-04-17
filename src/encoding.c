#include "encoding.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"


// Using int32 because is not used only for 8 bits values (e.g: convert all 16 bits in hexa)
int binaryToUnsignedInt(const char *serializedBinary) {
    int decimal = 0;
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

int8_t complementOfTwoToInt(const char *serializedBinary) {
    const size_t size = strlen(serializedBinary);

    const char firstCharacter = serializedBinary[0];
    switch (firstCharacter) {
        case '0': {
            // Convert to signed
            return (int8_t) binaryToUnsignedInt(serializedBinary);
        }
        case '1': {
            char binary[size + 1];
            subtractOneOnBinary(serializedBinary, binary);


            char invertedBinary[size + 1];
            invertBinary(binary, invertedBinary);

            const uint8_t value = binaryToUnsignedInt(invertedBinary);
            // Convert to signed
            return (int8_t) (-1 * value);
        }
        default: {
            return -1;
        }
    }
}
