#include "encoding.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

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

int complementOfTwoToInt(char *serializedBinary) {
    const size_t size = strlen(serializedBinary);

    const char firstCharacter = serializedBinary[0];
    switch (firstCharacter) {
        case '0': {
            return binaryToInt(serializedBinary, size, POSITIVE);
        }
        case '1': {
            char binary[size + 1];
            subtractOneOnBinary(serializedBinary, binary, size);


            char invertedBinary[size + 1];
            invertBinary(binary, invertedBinary, size);

            return binaryToInt(invertedBinary, size, NEGATIVE);
        }

        default: {
            println("Erro ao tentar converter complemento de dois para int, binário inválido: %s", serializedBinary);
            return -1;
        }
    }
}
