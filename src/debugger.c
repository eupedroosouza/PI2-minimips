#include "debugger.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"

const char *typeStr[4] = {"I", "J", "R", "O"};
const char *boolStr[2] = {"F", "T"};
const char *memToRegStr[2] = {"mem", "ula"};
const char *ulaSourceStr[2] = {"register", "immediate"};

bool debug = false;

void instructionsDebuggerHeader() {
    // Topo da tabela
    println(
        "┌─────┬──────────────────┬────────────────────────────────┬───────┬────┬─────┬─────┬─────┬─────┬──────┬─────┐");
    printf("│");
    printOnCenter("#", 5);
    printf("|");
    printOnCenter("Binary", 18);
    printf("│");
    printOnCenter("Assembly", 32);
    printf("│");
    printOnCenter("Type", 7);
    printf("│");
    printOnCenter("OP", 4);
    printf("│");
    printOnCenter("RS", 5);
    printf("│");
    printOnCenter("RT", 5);
    printf("│");
    printOnCenter("RD", 5);
    printf("│");
    printOnCenter("Funct", 5);
    printf("│");
    printOnCenter("Imm", 6);
    printf("│");
    printOnCenter("Addr", 5);
    printf("│\n");
    println(
        "├─────┼──────────────────┼────────────────────────────────┼───────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    // println(
    //     "├─────┼──────────────────┼────────────────────────────────┼───────┼────┼────┼────┼─────┼─────┼─────┼─────┤");
}

void internalDebugInstruction(Instruction *instruction, const int idx) {
    char strIdx[4];
    if (idx == -1) {
        strcpy(strIdx, " - ");
    } else {
        snprintf(strIdx, sizeof(strIdx), "%03d", idx);
    }

    printf("│ %-3s │ %-16s │ %-30.30s │", strIdx, instruction->stringedInstruction, instruction->asmInstruction);

    printf("   %-1s   │ %02d │  %1d  │  %1d  │  %1d  │  %1d  │ %04d │ %03d │\n",
           typeStr[instruction->type],
           instruction->opcode,
           instruction->rs,
           instruction->rt,
           instruction->rd,
           instruction->funct,
           instruction->imm,
           instruction->addr);
}

void instructionsDebuggerFooter() {
    println(
        "└─────┴──────────────────┴────────────────────────────────┴───────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
}

void debugInstruction(Instruction *instruction) {
    if (!debug) {
        return;
    }
    println(" Instruction debug:");
    instructionsDebuggerHeader();
    internalDebugInstruction(instruction, -1);
    instructionsDebuggerFooter();
}

void debugInstructionWithIndex(Instruction *instruction, const int idx) {
    if (!debug) {
        return;
    }
    println(" Instruction #%d debug:", idx);
    instructionsDebuggerHeader();
    internalDebugInstruction(instruction, idx);
    instructionsDebuggerFooter();
}

void debugInstructions(Instruction *instructions, const int size) {
    if (!debug) {
        return;
    }
    println(" Instructions debug:");
    instructionsDebuggerHeader();
    for (int i = 0; i < size; i++) {
        internalDebugInstruction(&instructions[i], i);
    }
    instructionsDebuggerFooter();
}

void debugControl(const Control *control, const Instruction *instruction) {
    if (!debug) {
        return;
    }

    char memToRegBuffer[14];
    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control->memToReg], control->memToReg);
    char memToReg[14];
    centerString(memToRegBuffer, memToReg, 13);

    const int8_t ulaSourceValue = (control->ulaSource == 0) ? registers[instruction->rt] : instruction->imm;
    char ulaSourceBuffer[39];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (source: %d, value: %04d)", ulaSourceStr[control->ulaSource], control->ulaSource, ulaSourceValue);
    char ulaSource[39];
    centerString(ulaSourceBuffer, ulaSource, 38);

    println(" Control debug:");
    println("┌──────┬────────┬─────────┬─────────────┬──────────────────────────────────────┬──────────┬─────────┬──────────┐");
    println("│ Jump │ Branch │ Reg Dst │  Mem to Reg │               Ula Source             │ Ula Ctrl │ Wrt Reg │  Wrt Mem │");
    println("├──────┼────────┼─────────┼─────────────┼──────────────────────────────────────┼──────────┼─────────┼──────────┤");
    printf("│  %s   │   %s    │   %2d    │%-13s│%-38s│    %2d    │    %s    │     %s    │\n",
            boolStr[control->jump ? 1 : 0],
            boolStr[control->branch ? 1 : 0],
            control->regDst,
            memToReg,
            ulaSource,
            control->ulaControl,
            boolStr[control->wrtReg ? 1 : 0],
            boolStr[control->wrtMem ? 1 : 0]);

    println("└──────┴────────┴─────────┴─────────────┴──────────────────────────────────────┴──────────┴─────────┴──────────┘");
}

void debugDataMem() {
    println("┌─────┬───────┐" );
    println("│  #  │ Value │");
    println("├─────┼───────┤");
    for (int i = 0; i < 256; i++) {
        println("│ %03d │  %03d  │", i, memData.data[i]);
    }
    println("└─────┴───────┘");
}