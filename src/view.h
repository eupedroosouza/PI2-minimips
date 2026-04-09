#pragma once
#include "types.h"

void createRegisterTable(char table[13][255]);

void showRegisters();

void showClock(const Instruction *instruction, const Control *control);

void showClockInformation(char *msg);

void showClockRegisters(unsigned int reg1, int value1, unsigned int reg2, int value2);

void showClockUla(int input1, int input2, int ulaControl, const ULAOut *out);

void showClockPc();

void showStatistics();

void showMems();

void printAllProgramData();

void showLastState();

void viewInstruction(const Instruction *instruction, int idx, char * buffer);

void viewInstructionWithIndex(const Instruction *instruction, int idx);

void viewInstructions(const Instruction *instructions, int size);

void viewDataMem();

void createDataMemTable(char table[260][255]);