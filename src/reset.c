#include <string.h>
#include "reset.h"

#include "encoding.h"
#include "main.h"




void resetInstructions() {
    for (int i = 0; i < MEM_SIZE; i++) {
        memInstruction.instructions[i] = emptyInstruction;
    }
    memInstruction.size = 0;
    
}

void resetRegisters(PipelineRegisters *pipeline) {

    for (int i = 0; i < REG_SIZE; i++) {
        registers[i] = 0;
    }

    // PC Reset
    pc = 0;

    // regs reset
    pipeline->IF = (IF_ID){0};
    pipeline->ID = (ID_EX){0};
    pipeline->EX_MEM = (EX_MEM){0};
    pipeline->MEM_WEB = (MEM_WB){0};
}

void resetData() {
    for (int i = 0; i < MEM_SIZE; i++) {
        memData.data[i] = 0;
    }
    memData.size = 0;
}
