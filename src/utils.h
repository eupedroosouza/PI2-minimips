#pragma once

#include <stdbool.h>

extern bool debug;

void println(char *msg, ...);

void debugLn(char *msg, ...);


void charsToString(char *buffer, int size, ...);

int binaryCharToInt(char binary);


void invertBinary(const char *originBinary, char *buffer);

void subtractOneOnBinary(const char *originBinary, char *buffer);

// Le um inteiro positivo com segurança
// Retorna -1 se houver algum erro
int readIntInStdinSafely();
