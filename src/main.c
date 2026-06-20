#include "main.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "reset.h"
#include "stats.h"

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif


#include "colors.h"
#include "instruction.h"
#include  "interface.h"
#include  "types.h"
#include  "utils.h"

bool debug = false;

PC pc;
Register registers[8];

PipelineRegisters pipeline;

MemInstruction memInstruction;
MemData memData;

int main(const int argCount, char *args[]) {
    setlocale(LC_ALL, "");

    initStatistics();
    // Create empty Instruction
    decodeInstruction(&emptyInstruction, "0000000000000000");

    // Reset (do that to prevents incorrect data (trash data) on not loaded data)
    resetRegisters(&pipeline);
    resetData();
    resetInstructions();
    // End-reset

    // start menu based on curses
    menu2();

    return 0;
}

