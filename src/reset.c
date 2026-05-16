#include <string.h>
#include "reset.h"

#include "encoding.h"
#include "main.h"


void resetMemory() {
    
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.instructions[i] = emptyInstruction;
    }
    memory.size = 0;

  
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.data[i] = 0;
    }
    
    memory.dataSize = 0; 
}


void resetRegisters() {
    for (int i = 0; i < REG_SIZE; i++) {
        registers.general[i] = 0;
    }

    // PC Reset
    pc = 0;
}