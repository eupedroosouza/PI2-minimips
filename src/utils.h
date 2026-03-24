#pragma once


void println(char *msg, ...);

void debugLn(char *msg, ...);


void charsToString(char *buffer, int size, ...);

int binaryCharToInt(char binary);


void invertBinary(const char *originBinary, char *buffer);

void subtractOneOnBinary(const char *originBinary, char *buffer);

// Le um inteiro positivo com segurança
// Retorna -1 se houver algum erro
int readIntInStdinSafely();

void printOnCenter(const char *msg, int width);

void centerString(const char *str, char *buffer, int width);
