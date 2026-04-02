#pragma once
#include "types.h"

void createRegisterTable(char table[13][255]);

void showRegisters();

void showClock(const Instruction *instruction, const Control *control);

void showClockInformation(char *msg);

void showClockRegisters(unsigned int reg1, int value1, unsigned int reg2, int value2);

void showClockUla(int input1, int input2, int ulaControl, const ULAOut *out);

void showStatistics();

void showMems();
