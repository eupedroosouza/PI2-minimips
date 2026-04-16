#pragma once

#include "types.h"

void decodeInstruction(Instruction * instruction, const char *serializedBinary);

void loadInstructionsOnMem();

void convertToAssemblyInstruction(const Instruction * instruction, char *buffer);
void convertToPrettyAssemblyInstruction(const Instruction * instruction, char *buffer);

void saveInstructionOnAssembly();
