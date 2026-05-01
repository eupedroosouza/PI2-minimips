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

void resetRegisters() {
    for (int i = 0; i < REG_SIZE; i++) {
        registers[i] = 0;
    }

    // PC Reset
    pc = 0;
}

void resetData() {
    for (int i = 0; i < MEM_SIZE; i++) {
        MemInstruction.data[i] = 0;
    }
    MemInstruction.size = 0;
}
