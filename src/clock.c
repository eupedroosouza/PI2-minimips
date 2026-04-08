#include "clock.h"

#include <stdio.h>

#include "back.h"
#include "control.h"
#include "encoding.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"

void clock() {

    saveLastState();

    // Busca
    // Prevents incorrect access of the memory
    const Instruction *instruction = (pc < memInstruction.size) ? &memInstruction.instructions[pc] : &emptyInstruction;
    const Control control = makeControl(instruction);

    // --- Início da Coleta de Estatísticas ---
    stats.executedInstructions++;

    // Incrementa por Tipo
    if (instruction->type == R) {
        stats.executedInstructionsPerType.r++;
    } else if (instruction->type == I) {
        stats.executedInstructionsPerType.i++;
    } else if (instruction->type == J) {
        stats.executedInstructionsPerType.j++;
    } else {
        stats.executedInstructionsPerType.other++;
    }

    // Incrementa por Classe (Opcode/Funct)
    if (instruction->opcode == R_TYPE_OPCODE) {
        switch (instruction->funct) {
            case ADD_FUNCT: stats.executedInstructionsPerClass.add++; break;
            case SUB_FUNCT: stats.executedInstructionsPerClass.sub++; break;
            case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++; break;
            case OR_FUNCT:  stats.executedInstructionsPerClass.or_inst++; break;
            default: stats.executedInstructionsPerClass.other++; break;
        }
    } else {
        switch (instruction->opcode) {
            case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++; break;
            case LW_OPCODE:   stats.executedInstructionsPerClass.lw++; break;
            case SW_OPCODE:   stats.executedInstructionsPerClass.sw++; break;
            case BEQ_OPCODE:  stats.executedInstructionsPerClass.beq++; break;
            case J_OPCODE:    stats.executedInstructionsPerClass.j++; break;
            default: stats.executedInstructionsPerClass.other++; break;
        }
    }
   

    showClock(instruction, &control);

    // Jump!
    if (control.jump) {
        pc = instruction->addr;
        char buffer[255];
        sprintf(buffer, " Executed jump to address: %d.", pc);
        showClockInformation(buffer);
        return;
    }

    const Register input1 = registers[instruction->rs];
    const Register register2 = registers[instruction->rt];
    showClockRegisters(instruction->rs, input1, instruction->rt, register2);
    const int8_t input2 = control.ulaSource == 0 ? register2 : instruction->imm;

    const ULAOut ulaOut = ula(input1, input2, control.ulaControl);
    showClockUla(input1, input2, control.ulaControl, &ulaOut);

    // Branch!
    if (ulaOut.zeroUla && control.branch) {
        pc = pc + instruction->imm + 1;
        char buffer[255];
        sprintf(buffer, " Executed branch to address: %d (input 1 %d equal input 2 %d).", pc, input1, input2);
        showClockInformation(buffer);
        return;
    }

    // Save Word
    if (control.wrtMem) {
        const int8_t addr = ulaOut.value;
        memData.data[addr] = register2;
        char buffer[255];
        sprintf(buffer, " Written in data memory to address: %03d value: %04d.", addr, register2);
        showClockInformation(buffer);
    }

    // Load World and anyone instruction what save on register
    if (control.wrtReg) {
        const int8_t addr = ulaOut.value;
        const int8_t value = control.memToReg == 0 ? memData.data[addr] : ulaOut.value;
        const unsigned int wrtReg = control.regDst == 0 ? instruction->rt : instruction->rd;
        registers[wrtReg] = value;
        char buffer[255];
        sprintf(buffer, " Written in register: $%1d value: %04d.", wrtReg, value);
        showClockInformation(buffer);
    }

    pc++;
}
