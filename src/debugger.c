#include "debugger.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"

const char *typeStr[4] = {"I", "J", "R", "O"};

bool debug = false;

void instructionsDebuggerHeader() {
    println(
        "┌─────┬──────────────────┬────────────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┐");
    println(
        "│  #  │      Binary      │            Assembly            │  Type  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println(
        "├─────┼──────────────────┼────────────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
}

void debugInstruction(const Instruction *instruction, const int idx, char *buffer) {
    char strIdx[4];
    if (idx == -1) {
        strcpy(strIdx, " - ");
    } else {
        snprintf(strIdx, sizeof(strIdx), "%03d", idx);
    }

    sprintf(buffer,
            "│ %-3s │ %-16s │ %-30.30s │    %-1s   │ %02d │  %1d  │  %1d  │  %1d  │  %1d  │ %04d │ %03d │",
            strIdx,
            instruction->stringedInstruction,
            instruction->asmInstruction,
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
        "└─────┴──────────────────┴────────────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
}

void debugInstructionWithIndex(const Instruction *instruction, const int idx) {
    if (!debug) {
        return;
    }
    println(" Instruction #%d debug:", idx);
    instructionsDebuggerHeader();
    char buffer[256];
    debugInstruction(instruction, idx, buffer);
    println(buffer);
    instructionsDebuggerFooter();
}

void debugInstructions(const Instruction *instructions, const int size) {
    if (!debug) {
        return;
    }
    println(" Instructions debug:");
    instructionsDebuggerHeader();
    for (int i = 0; i < size; i++) {
        char buffer[256];
        debugInstruction(&instructions[i], i, buffer);
        println(buffer);
    }
    instructionsDebuggerFooter();
}

void debugDataMem() {
    char table[260][255];
    createDataMemTable(table);
    for (int i = 0; i < 260; i++) {
        println(table[i]);
    }
}

void createDataMemTable(char table[260][255]) {
    sprintf(table[0], "┌─────┬───────┐");
    sprintf(table[1], "│  #  │ Value │");
    sprintf(table[2], "├─────┼───────┤");
    for (int i = 0; i < 256; i++) {
        sprintf(table[i + 3], "│ %03d │  %03d  │", i, memData.data[i]);
    }
    sprintf(table[259], "└─────┴───────┘");
}
