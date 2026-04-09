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

    char bufferInformation[255];

    // Jump!
    if (control.jump) {
        pc = instruction->addr;
        sprintf(bufferInformation, " Jump executado para o endereço: %d.", pc);
        showClockPc();
        showClockInformation(bufferInformation);
        return;
    }

    const Register input1 = registers[instruction->rs];
    const Register register2 = registers[instruction->rt];
    showClockRegisters(instruction->rs, input1, instruction->rt, register2);
    const int8_t input2 = control.ulaSource == 0 ? register2 : instruction->imm;

    const ULAOut ulaOut = ula(input1, input2, control.ulaControl);
    showClockUla(input1, input2, control.ulaControl, &ulaOut);


    // Branch!
    if (control.branch) {

        if (ulaOut.zeroUla) {
            pc = pc + instruction->imm + 1;
            sprintf(bufferInformation, " Branch executada para o endereco: %d (entrada 1: %d e igual entrada 2: %d).", pc, input1, input2);
        } else {
            sprintf(bufferInformation, " Nao executado o branch para o endereco: %d (entrada 1: %d não e igual entrada 2: %d).", pc, input1,
                    input2);
            pc++;
        }
        showClockPc();
        showClockInformation(bufferInformation);
        return;
    }

    // Save Word
    if (control.wrtMem) {
        const int8_t addr = ulaOut.value;
        memData.data[addr] = register2;
        sprintf(bufferInformation, " Escrito dado na memoria de dados no endereco: %03d o valor: %04d.", addr, register2);
    }

    // Load World and anyone instruction what save on register
    if (control.wrtReg) {
        const int8_t addr = ulaOut.value;
        const int8_t value = control.memToReg == 0 ? memData.data[addr] : ulaOut.value;
        const unsigned int wrtReg = control.regDst == 0 ? instruction->rt : instruction->rd;
        registers[wrtReg] = value;
        if (control.memToReg == 0) {
            sprintf(bufferInformation, " Carregado dado da memoria de dados no endereco: %03d e escrito no registrador: %1d o valor: %04d.", addr,wrtReg, value);
        } else {
            sprintf(bufferInformation, " Executada operacao na ULA e escrito no registrador: $%1d o valor: %04d.", wrtReg, value);
        }
    }

    pc++;
    showClockPc();
    showClockInformation(bufferInformation);
}
