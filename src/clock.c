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
            case ADD_FUNCT: stats.executedInstructionsPerClass.add++;
                break;
            case SUB_FUNCT: stats.executedInstructionsPerClass.sub++;
                break;
            case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++;
                break;
            case OR_FUNCT: stats.executedInstructionsPerClass.or_inst++;
                break;
            default: stats.executedInstructionsPerClass.other++;
                break;
        }
    } else {
        switch (instruction->opcode) {
            case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++;
                break;
            case LW_OPCODE: stats.executedInstructionsPerClass.lw++;
                break;
            case SW_OPCODE: stats.executedInstructionsPerClass.sw++;
                break;
            case BEQ_OPCODE: stats.executedInstructionsPerClass.beq++;
                break;
            case J_OPCODE: stats.executedInstructionsPerClass.j++;
                break;
            default: stats.executedInstructionsPerClass.other++;
                break;
        }
    }


    showClock(instruction, &control);

    char bufferInformation[255];
    char bufferInformation2[255];
    strcpy(bufferInformation2, "");

    // Jump!
    if (control.jump) {
        pc = instruction->addr;
        sprintf(bufferInformation, " Salto executado para o endereço: %d.", pc);
        showClockPc();
        showClockInformation(bufferInformation, bufferInformation2);
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
        if (ulaOut.equal) {
            pc = pc + instruction->imm + 1;
            sprintf(bufferInformation, " Ramificação executada para o endereço: %d (entrada 1 (valor: %04d) é igual .",
                    pc, input1);
            sprintf(bufferInformation2, " a entrada 2 (valor: %04d)", input2);
        } else {
            sprintf(bufferInformation,
                    " Não executada a ramificação para o endereço: %d (entrada 1 (valor: %04d) não é igual", pc,
                    input1);
            sprintf(bufferInformation2, " a entrada 2 (valor: %04d)", input2);
            pc++;
        }
        showClockPc();
        showClockInformation(bufferInformation, bufferInformation2);
        return;
    }

    // Save Word
    if (control.wrtMem) {
        const int8_t addr = ulaOut.value;
        memData.data[addr] = register2;
        sprintf(bufferInformation, " Escrito dado na memória de dados no endereço: %03d, o valor: %04d.", addr,
                register2);
    }

    // Load World and anyone instruction what save on register
    if (control.wrtReg) {
        const int8_t addr = ulaOut.value;
        const int8_t value = control.memToReg == 0 ? memData.data[addr] : ulaOut.value;
        const unsigned int wrtReg = control.regDst == 0 ? instruction->rt : instruction->rd;
        registers[wrtReg] = value;
        if (control.memToReg == 0) {
            sprintf(bufferInformation,
                    " Carregado dado da memória de dados no endereço: %03d, e escrito no registrador: %1d, o valor: %04d.",
                    addr, wrtReg, value);
        } else {
            if (instruction->opcode == ADDI_OPCODE) {
                sprintf(bufferInformation,
                        " Adição do registrador %1d (valor: %04d) com o imediato %04d, e escrito no registrador %1d (resultado: %04d).",
                        instruction->rs, input1, instruction->imm, wrtReg, value);
            } else if (instruction->opcode == R_TYPE_OPCODE) {
                switch (instruction->funct) {
                    case ADD_FUNCT: {
                        sprintf(bufferInformation,
                                " Adição do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d), e escrito no",
                                instruction->rs, input1, instruction->rt, input2);
                        sprintf(bufferInformation2, " registrador %1d (resultado: %04d).", wrtReg, value);
                        break;
                    }
                    case SUB_FUNCT: {
                        sprintf(bufferInformation,
                                " Subtração do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d), e escrito no",
                                instruction->rs, input1, instruction->rt, input2);
                        sprintf(bufferInformation2, " registrador %1d (resultado: %04d).", wrtReg, value);
                        break;
                    }
                    case AND_FUNCT: {
                        sprintf(bufferInformation,
                                " Operação AND (e / &) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d), e escrito no.",
                                instruction->rs, input1, instruction->rt, input2);
                        sprintf(bufferInformation2, " registrador %1d (resultado: %04d).", wrtReg, value);
                        break;
                    }
                    case OR_FUNCT: {
                        sprintf(bufferInformation,
                                " Operação OR (ou / |) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d), e escrito no",
                                instruction->rs, input1, instruction->rt, input2);
                        sprintf(bufferInformation2, " registrador %1d (resultado: %04d).", wrtReg, value);
                        break;
                    }
                    default: break;
                }
            } else {
                sprintf(bufferInformation, " Executada operacao na ULA e escrito no registrador: $%1d o valor: %04d.",
                        wrtReg, value);
            }
        }
    }

    pc++;
    showClockPc();
    showClockInformation(bufferInformation, bufferInformation2);
}
