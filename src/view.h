#pragma once
#include "types.h"


// Registers

void createRegisterTable(char table[13][255]);

void showRegisters();


// End Registers

// Clock

void showClock(const Instruction *instruction, const Control *control);

void showClockPc();

void showClockInformation(char *msg, char *msg2);

void showClockRegisters(unsigned int reg1, int value1, unsigned int reg2, int value2);

void showClockUla(int input1, int input2, int ulaControl, const ULAOut *out);

// End clock


// Statistics

void showStatistics();

// End Statistics

// Instruction

void viewInstruction(const Instruction *instruction, int idx, char *buffer);

void createInstructionTable(char memInstructionTable[260][512]);

void viewInstructions();

// End Instruction

// Mem Data


void createDataMemTable(char table[260][255]);

void viewDataMem();

// End Mem Data

// Misc

void showMems();

void printAllProgramData();

void showLastState();

// End Misc
