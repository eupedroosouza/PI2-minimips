#pragma once
#include "types.h"
#include <stdbool.h>


extern bool debug;

void debugInstruction(Instruction *instruction);

void debugInstructionWithIndex(Instruction *instruction, int idx);

void debugInstructions(Instruction *instructions, int size);

void debugControl(const Control *control, const Instruction *instruction);

void debugDataMem();