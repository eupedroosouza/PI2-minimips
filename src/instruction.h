#pragma once

#include "types.h"

Instruction decodeInstruction(const char *serializedBinary);

void loadInstructionsOnMem();