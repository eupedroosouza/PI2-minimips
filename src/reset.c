#include <string.h>
#include "reset.h"

#include "encoding.h"
#include "main.h"

PipelineRegisters pipeline;

void resetInstructions() {
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.instructions[i] = emptyInstruction;
    }
    memory.MemDatasize = 0;
}

void resetRegisters() {

    for (int i = 0; i < REG_SIZE; i++) {
        registers[i] = 0;
    }

    // PC Reset
    pc = 0;

    // regs reset
    pipeline.if_id = (IF_ID){0};
    pipeline.id_ex = (ID_EX){0};
    pipeline.ex_mem = (EX_MEM){0};
    pipeline.mem_wb = (MEM_WB){0};
}

void resetData() {
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.data[i] = 0;
    }
    memory.MemDatasize = 0;
}
