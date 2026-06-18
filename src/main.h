#pragma once
#include "types.h"

extern bool debug;

extern PC pc;
extern Register registers[8];

// Acessa o registrador de pipeline global definido no projeto
// extern PipelineRegisters pipeline;

extern MemData memData;
extern MemInstruction memInstruction;

extern Instruction emptyInstruction;

