#pragma once

#include <stdbool.h>

extern bool debug;

void println(char *msg, ...);

void charsToString(char *buffer, int size, ...);

int binaryCharToInt(char binary);


void invertBinary(const char * originBinary, char * buffer, size_t size);

void subtractOneOnBinary(const char * originBinary, char * buffer, size_t size);