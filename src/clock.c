
#include "clock.h"
#include <stdio.h>
#include <string.h>

#include "back.h"
#include "control.h"
#include "encoding.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"

Control control = {0};
Registradores reg;
ULAOut ulaOut;

void clock() {
    saveState();

    int currentState = control.state;
    
    
    const Instruction *instruction = &reg.IR;
    Control next = control;
    makeControl(reg.IR.opcode, reg.IR.funct, &next);
   
    // stats.totalCycles++; // Incrementa 1 ciclo de clock
    

    if (currentState == 0 && reg.IR.type != OTHER) {
        stats.executedInstructions++;

        if (reg.IR.type == R) stats.executedInstructionsPerType.r++;
        else if (reg.IR.type == I) stats.executedInstructionsPerType.i++;
        else if (reg.IR.type == J) stats.executedInstructionsPerType.j++;
        else stats.executedInstructionsPerType.other++;

        if (reg.IR.opcode == R_TYPE_OPCODE) {
            switch (reg.IR.funct) {
                case ADD_FUNCT: stats.executedInstructionsPerClass.add++; break;
                case SUB_FUNCT: stats.executedInstructionsPerClass.sub++; break;
                case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++; break;
                case OR_FUNCT:  stats.executedInstructionsPerClass.or_inst++; break;
            }
        } else {
            switch (reg.IR.opcode) {
                case LW_OPCODE: stats.executedInstructionsPerClass.lw++; break;
                case SW_OPCODE: stats.executedInstructionsPerClass.sw++; break;
                case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++; break;
                case BEQ_OPCODE: stats.executedInstructionsPerClass.beq++; break;
                case J_OPCODE: stats.executedInstructionsPerClass.j++; break;
            }
        }
    }

   
    int8_t input1 = (next.ulaSourceA == 0) ? pc : registers[reg.IR.rs];
    int8_t input2;
    if (next.ulaSourceB == 0)      input2 = registers[reg.IR.rt];
    else if (next.ulaSourceB == 1) input2 = 1; // Geralmente usado para somar PC+1 (caso use)
    else                              input2 = reg.IR.imm; // Extensão de sinal do Imediato


    int ulaOp = (currentState == 7) ? reg.IR.funct : next.ulaControl;
    ULAOut out = ula(input1, input2, ulaOp);

    if (next.wrtReg) {
        int destination = (next.regDst == 1) ? reg.IR.rd : reg.IR.rt;
        int value = (next.memToReg == 1) ? memory.data[out.value] : out.value;
        registers[destination] = value;
    }

    if (next.wrtMem) {
        memory.data[out.value] = registers[reg.IR.rt];
    }

   
 

    switch (currentState){ // salva dados nos regs dependendo do estado do clock

        case 0: // salva memória de instrução no registrador IR
            reg.IR = memory.instructions[pc];
            pc ++;
        break;
        case 1: // salva valores saindo do banco de registradores nos regs A e B
            reg.A = registers[reg.IR.rs];
            reg.B = registers[reg.IR.rt];
        break;
        case 2:
            reg.ULAOut = reg.A + reg.IR.imm;
        break;
        case 3:
            reg.MDR = memory.data[reg.ULAOut]; // registrador MDR recebe dado da memória de dados (endereço calculado pela ULA)
        break;
        case 4:
            if (reg.IR.rt != 0) registers[reg.IR.rt] = reg.MDR;
            registers[reg.IR.rt] = reg.MDR; // valor da memória de dados vai para banco de registradores
        break;
        case 5:
            memory.data[reg.ULAOut] = reg.B; // valor de B é salvo na memória de dados
        break;
        case 6:
            if (reg.IR.rt != 0) registers[reg.IR.rt] = reg.ULAOut;
            registers[reg.IR.rt] = reg.ULAOut; // resultado da ULA no reg rt
        break;
        case 7: // salva o resultado da ULA no reg ulaOut
            ulaOut = ula(reg.A, reg.B, next.ulaControl); // chama ULA
            reg.ULAOut = ulaOut.value;
        break;
        case 8: // R type
            registers[reg.IR.rd] = reg.ULAOut; // resultado da ULA no reg rd
        break;
        case 9: // beq
            if (registers[reg.IR.rs] == registers[reg.IR.rt]) {
            pc = pc + reg.IR.imm - 1;
          }
         break;
        case 10: // jump
      
            pc = reg.IR.addr;
        break;

    }




    char bufferInformation[255] = "";
    char bufferInformation2[255] = "";

    sprintf(bufferInformation2, " Proximo ciclo ira para o estado: %d", next.state);

    
    control = next;

    // Exibe a tabela de sinais de controle atual da FSM
    showClock(instruction, &control);
    
    showClockInformation(bufferInformation, bufferInformation2);

    

    // Exibe a informação do PC atualizada
    showClockPc();

}
