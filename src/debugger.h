#pragma once
#include "types.h"
#include <stdbool.h>

extern bool debug;

void debugInstruction(const Instruction *instruction, int idx, char * buffer);

void debugInstructionWithIndex(const Instruction *instruction, int idx);

void debugInstructions(const Instruction *instructions, int size);

void debugControl(const Control *control, const Instruction *instruction);

void createControlTable(char table[5][255], const Control *control, const Instruction *instruction);

void debugDataMem();

void createDataMemTable(char table[260][255]);