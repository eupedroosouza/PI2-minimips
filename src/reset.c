#include <string.h>
#include "reset.h"
#include "stats.h"
#include "encoding.h"
#include "main.h"


void resetMemory() {
    
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.memory[i] = emptyInstruction;
    }
    memory.size = 0;

  
    for (int i = 0; i < MEM_SIZE; i++) {
        memory.memory[i].data = 0;
    }
    
    memory.dataSize = 0; 
}


void resetRegisters() {
    for (int i = 0; i < REG_SIZE; i++) {
        registers.general[i] = 0;
    }

    // PC Reset
    pc = 0;

    resetStatistics();
}
