#include <string.h>
#include "reset.h"
#include "main.h" 

void resetinstruc() {
  
    memInstruction.size = 0; 


    for (int i = 0; i < MEM_SIZE; i++) {
        memInstruction.instructions[i].stringedInstruction[0] = '\0';
        memInstruction.instructions[i].asmInstruction[0] = '\0';
        memInstruction.instructions[i].hexa[0] = '\0';
    }
}

void resetregist() {
    
    memset(registers, 0, sizeof(registers));

   
    pc = 0;
}

void resetdados() {
 
    memData.size = 0;

 
    memset(memData.data, 0, sizeof(memData.data));
}