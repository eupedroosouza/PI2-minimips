#include <string.h>
#include "reset.h"

#include "encoding.h"
#include "main.h"


void resetMemory() {
    
    for (int i = 0; i < MEM_SIZE; i++) {
        memInstruction.instructions[i] = emptyInstruction;
    }
    memInstruction.size = 0;

  
    for (int i = 0; i < MEM_SIZE; i++) {
        memInstruction.data[i] = 0;
    }
    
    memInstruction.dataSize = 0; 
}


void resetRegisters() {
    for (int i = 0; i < REG_SIZE; i++) {
        registers[i] = 0;
    }

    // PC Reset
    pc = 0;
}