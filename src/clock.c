#include "clock.h"

#include "control.h"
#include "debugger.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "utils.h"

int clock() {
    debugLn("PC: %d", pc);

    if (pc > (memInstruction.size - 1)) {
        println("Não há instruções para executar.");
        if (memInstruction.size == 0) {
            println("Você carregou o programa? Use a opção [1] para carregar a memória de instrução do simulador.");
        }
        return -1;
    }

    // Busca
    Instruction *instruction = &memInstruction.instructions[pc];
    debugInstructionWithIndex(instruction, pc);

    const Control control = makeControl(instruction);
    debugControl(&control, instruction);

    // Jump!
    if (control.jump) {
        pc = instruction->addr;
        return 0;
    }

    const Register input1 = registers[instruction->rs];
    const Register register2 = registers[instruction->rt];
    const int8_t input2 = control.ulaSource == 0 ? register2 : instruction->imm;

    const ULAOut ulaOut = ula(input1, input2, control.ulaControl);

    // Branch!
    if (ulaOut.zeroUla && control.branch) {
        pc = pc + instruction->imm + 1;
        return 0;
    }

    // Save Word
    if (control.wrtMem) {
        const int8_t addr = ulaOut.value;
        memData.data[addr] = register2;
    }

    // Load World and anyone instruction what save on register
    if (control.wrtReg) {
        const int8_t addr = ulaOut.value;
        const int8_t value = control.memToReg == 0 ? memData.data[addr] : ulaOut.value;
        const unsigned int wrtReg = control.regDst == 0 ? instruction->rt : instruction->rd;
        registers[wrtReg] = value;
    }

    pc++;

    return 0;
}
