#pragma once
#include "types.h"

extern Statistics stats;

void initStatistics();
void computeInstructionStats(const Instruction *instruction);
void showStatistics();