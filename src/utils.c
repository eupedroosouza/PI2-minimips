#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

#include <string.h>

void println(char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    printf("\n");
}

void charsToString(char *buffer, const int size, ...) {
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; i++) {
        buffer[i] = (char) va_arg(args, int);
    }
    buffer[size] = '\0';
    va_end(args);
}

int binaryToInt(const char binary) {
    if (binary == '1') {
        return 1;
    }
    if (binary == '0') {
        return 0;
    }
    // Invalid binary?
    return -1;
}


void invertBinary(const char *originBinary, char *buffer, const size_t size) {
    for (int i = 0; i < size; i++) {
        char replacementCharacter;
        const char character = originBinary[i];
        if (character == '1')
            replacementCharacter = '0';
        else if (character == '0')
            replacementCharacter = '1';
        else {
            replacementCharacter = '?';
        }
        buffer[i] = replacementCharacter;
    }
    buffer[size] = '\0';
}

void subtractOneOnBinary(const char *originBinary, char *buffer, const size_t size) {
    strcpy(buffer, originBinary); // Clean the buffer
    for (int i = (int) (size - 1); i > 0; i--) {
        if (buffer[i] == '1') {
            buffer[i] = '0';
            return;
        }
        buffer[i] = '1';
    }
}
