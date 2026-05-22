#pragma once

#include "types.h"

void decodeWord(Word * instruction, const char *serializedBinary);

void loadInstructionsOnMem();

void convertToAssemblyInstruction(const Word * instruction, char *buffer);
void convertToPrettyAssemblyInstruction(const Word * instruction, char *buffer);

void saveInstructionOnAssembly();
