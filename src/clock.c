#include "clock.h"

#include "control.h"
#include "debugger.h"
#include "main.h"
#include "types.h"
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
    debugControl(&control, instruction, registers);

    pc++;

    return  0;
}
