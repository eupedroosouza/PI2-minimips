#include "clock.h"

#include <stdio.h>

#include "back.h"
#include "control.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "utils.h"
#include "view.h"

int clock() {

    saveLastState();

    if (pc > (memInstruction.size - 1)) {
        println("Não há instruções para executar.");
        if (memInstruction.size == 0) {
            println("Você carregou o programa? Use a opção [1] para carregar a memória de instrução do simulador.");
        }
        return -1;
    }

    // Busca
    const Instruction *instruction = &memInstruction.instructions[pc];
    const Control control = makeControl(instruction);

    showClock(instruction, &control);

    // Jump!
    if (control.jump) {
        pc = instruction->addr;
        char buffer[255];
        sprintf(buffer, " Executed jump to address: %d.", pc);
        showClockInformation(buffer);
        return 0;
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
        return 0;
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

    return 0;
}
