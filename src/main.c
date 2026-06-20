#include "main.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>


#include "reset.h"
#include "stats.h"

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

    // init ncurses
    (void) initscr();
    keypad(stdscr, TRUE); // enable use special keys
    cbreak();
    noecho(); // no send clicked button
    start_color(); // start clock supports
    curs_set(FALSE); // remove cursor
    init_pair(1, COLOR_BLACK, COLOR_WHITE);  // black letter, white background

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

    endwin();

    return 0;
}
