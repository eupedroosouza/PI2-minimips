#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

const char *boolStr[2] = {"F", "T"};

void println(char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    printf("\n");
}

void debugLn(char *msg, ...) {
    if (debug) {
        printf("DEBUG: ");
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
        printf("\n");
    }
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


void invertBinary(const char *originBinary, char *buffer) {
    const size_t size = strlen(originBinary);
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

void subtractOneOnBinary(const char *originBinary, char *buffer) {
    const size_t size = strlen(originBinary);
    strcpy(buffer, originBinary); // Clean the buffer
    for (int i = (int) (size - 1); i > 0; i--) {
        if (buffer[i] == '1') {
            buffer[i] = '0';
            return;
        }
        buffer[i] = '1';
    }
}


int readIntInStdinSafely() {
    char buffer[12];
    char *endptr;

    setbuf(stdin, NULL);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    errno = 0;
    const long value = strtol(buffer, &endptr, 10);
    if (errno == ERANGE || endptr == buffer || value > __INT_MAX__) {
        return -1;
    }

    return (int) value;
}

void printOnCenter(const char *msg, const int width) {
    const int len = (int) strlen(msg);
    const int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s", msg);
    for (int i = 0; i < (width - len - padding); i++) {
        printf(" ");
    }
}

void centerString(const char *str, char *buffer, const int width) {
    const int len = (int) strlen(str);
    strcpy(buffer, "");
    const int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++) {
        strcat(buffer, " ");
    }
    strcat(buffer, str);
    for (int i = 0; i < (width - len - padding); i++) {
        strcat(buffer, " ");
    }
}
